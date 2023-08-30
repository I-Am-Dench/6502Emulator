#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "cpu.h"
#include "alu.h"
#include "control/controls.h"

#define PARSE_BITS(word, n) word & ((0x01 << n) - 1); word >>= n

#define WORD_HIGH(word) (Byte)((word >> 8) & 0x00ff)

#define SET_RESET_OPCODE   1
#define SET_RESET_SEI_CLD  1 << 1
#define SET_RESET_ZERO     1 << 2
#define SET_RESET_NEGATIVE 1 << 3
#define SET_RESET_CARRY    1 << 4
#define SET_RESET_OVERFLOW 1 << 5

#define BRANCH_NEGATIVE 0
#define BRANCH_OVERFLOW 1
#define BRANCH_CARRY    2
#define BRANCH_ZERO     3

#define MI_SW__NONE        0
#define MI_SW__FETCH_OP    1
#define MI_SW__BRANCH_EXIT 1 << 1
#define MI_SW__INC_DPH     1 << 2
#define MI_SW__INT         1 << 3

#ifdef CPU_DEBUG
#define DLOG(format, ...) printf(format, ##__VA_ARGS__)
#else
#define DLOG(format, ...)
#endif

extern ControlWord OPCODES[MAX_OPCODES][CONTROLS_PER_OPCODE];

typedef struct _cpu {
    Byte interrupt_latch;

    bool reset_latch;
    bool nmi_latch;

    bool is_in_interrupt;

    Byte mi_sw; // microinstruction switch

    Byte flags; //NV-BDIZC
    Byte PCL, PCH, DPL, DPH; // Program Count, Data Pointer
    Byte A, X, Y, SP, T;

    Byte IR, Q; // Instruction register, Microinstruction index
    ControlWord nextCtrl; // Prefetched control word
    ControlWord (*micro_rom)[CONTROLS_PER_OPCODE];

    Word address_bus; // Store here since AD.MX *PBA selects the previous bus address

    MemGetter get_mem;

    struct ALU alu;
} CPU;

struct Controls {
    Byte WR, R, AD, INC, LUOP, SF, A, C, NX;
};

// == UTILITY FUNCTIONS == //
struct Controls parse_control_word(ControlWord control) {
    struct Controls ctrl;

    ctrl.NX   = PARSE_BITS(control, 3);
    ctrl.C    = PARSE_BITS(control, 3);
    ctrl.A    = PARSE_BITS(control, 2);
    ctrl.SF   = PARSE_BITS(control, 4);
    ctrl.LUOP = PARSE_BITS(control, 4);
    ctrl.INC  = PARSE_BITS(control, 4);
    ctrl.AD   = PARSE_BITS(control, 4);
    ctrl.R    = PARSE_BITS(control, 4);
    ctrl.WR   = PARSE_BITS(control, 4);

    return ctrl;
}

void print_microinstruction(struct Controls ctrl) {
    printf("\t  WR.MX: 0x%x\n", ctrl.WR);
    printf("\t   R.MX: 0x%x\n", ctrl.R);
    printf("\t  AD.MX: 0x%x\n", ctrl.AD);
    printf("\t INC.MX: 0x%x\n", ctrl.INC);
    printf("\tLUOP.MX: 0x%x\n", ctrl.LUOP);
    printf("\t  SF.MX: 0x%x\n", ctrl.SF);
    printf("\t   A.MX: 0x%x\n", ctrl.A);
    printf("\t   C.MX: 0x%x\n", ctrl.C);
    printf("\t  NX.MX: 0x%x\n", ctrl.NX);
}

// == CPU METHODS == //
CPU *cpu_create(MemGetter get_mem) {
    CPU *cpu = malloc(sizeof(CPU));
    cpu->interrupt_latch = INTERRUPT_RESET;
    cpu->nmi_latch = false;

    cpu->is_in_interrupt = false;

    cpu->flags = 1 << 5;
    cpu->A = 0;
    cpu->X = 0;
    cpu->Y = 0;

    cpu->IR = 0;
    cpu->Q = 0;
    cpu->nextCtrl = 0;
    cpu->micro_rom = OPCODES;

    cpu->address_bus = 0;

    cpu->get_mem = get_mem;

    struct ALU alu = {0, 0, 0, 0};
    cpu->alu = alu;

    return cpu;
}

void cpu_destroy(CPU *cpu) {
    free(cpu);
}

// == CONTROL MUTEXES == //

// Ignores DPH+1 and MEM for separate implementation
Byte *select_wr(CPU *cpu, Byte control) {
    DLOG("WRITE TO: ");
    switch (control) {
        case WR_MX__DPL:
            DLOG("DPL\n");
            return &cpu->DPL;
        case WR_MX__DPH:
            DLOG("DPH\n");
            return &cpu->DPH;
        case WR_MX__SP:
            DLOG("SP\n");
            return &cpu->SP;
        case WR_MX__X:
            DLOG("X\n");
            return &cpu->X;
        case WR_MX__Y:
            DLOG("Y\n");
            return &cpu->Y;
        case WR_MX__T:
            DLOG("T\n");
            return &cpu->T;
        case WR_MX__AXS:
            DLOG("AXS\n");
            return NULL; // Implement
        case WR_MX__P:
            DLOG("P\n");
            return &cpu->flags;
        case WR_MX__PCL:
            DLOG("PCL\n");
            return &cpu->PCL;
        case WR_MX__PCH:
            DLOG("PCH\n");
            return &cpu->PCH;
        case WR_MX__A:
            DLOG("A\n");
            return &cpu->A;
        default:
            DLOG("NONE\n");
            return NULL;
    }
}

// Ignores SPI
Byte select_r(CPU *cpu, Byte control) {
    switch (control) {
        case R_MX__A:
            return cpu->A;
        case R_MX__DPH:
            return cpu->DPH;
        case R_MX__SP:
            return cpu->SP;
        case R_MX__X:
            return cpu->X;
        case R_MX__Y:
            return cpu->Y;
        case R_MX__T:
            return cpu->T; // Implement
        case R_MX__SPI:
            return 0x00;
        case R_MX__P:
            return cpu->flags;
        case R_MX__PCL:
            return cpu->PCL;
        case R_MX__PCH:
            return cpu->PCH;
        case R_MX__DPL:
            return cpu->DPL;
        case R_MX__0:
        default:
            return 0x00;
    }
}

Byte select_sf(Byte control) {
    switch (control & 0x07) {
        case SF_MX__SEI_CLD:
            return SET_RESET_SEI_CLD;
        case SF_MX__Z:
            return SET_RESET_ZERO;
        case SF_MX__NZ:
            return SET_RESET_ZERO | SET_RESET_NEGATIVE;
        case SF_MX__NZC:
            return SET_RESET_ZERO | SET_RESET_NEGATIVE | SET_RESET_CARRY;
        case SF_MX__NZCV:
            return SET_RESET_ZERO | SET_RESET_NEGATIVE | SET_RESET_CARRY | SET_RESET_OVERFLOW;
        case SF_MX__NZV:
            return SET_RESET_ZERO | SET_RESET_NEGATIVE | SET_RESET_OVERFLOW;
        case SF_MX__OPCODE:
            return SET_RESET_OPCODE;
        default:
            return 0;
    }
}

Byte cpu_interrupt_constant(CPU *cpu) {
    if (cpu->interrupt_latch & INTERRUPT_NMI) {
        return 0xfa;
    }

    if (cpu->interrupt_latch & INTERRUPT_RESET) {
        return 0xfc;
    }

    if (cpu->interrupt_latch & INTERRUPT_IRQ) {
        return 0xfe;
    }

    return 0x00;
}

Word select_ad(CPU *cpu, Byte control) {
    switch (control & 0x07) {
        case AD_MX__PC:
            return cpu_get_pc(cpu);
        case AD_MX__DP:
            return cpu_get_dp(cpu);
        case AD_MX__DPt:
            return (cpu->DPH << 8) | cpu->T;
        case AD_MX__zDP:
            return 0x0000 | cpu->DPL;
        case AD_MX__SP:
            return 0x0100 | cpu->SP;
        case AD_MX__fDP:
            return 0xff00 | cpu->DPL;
        case AD_MX__fCP:
            return 0xff00 | cpu_interrupt_constant(cpu);
        default:
            return 0x0000;
    }
}

void increment_register(CPU *cpu, Byte control, Word address_bus) {
    Word s_bus = control & INC_OP__OP ? address_bus - 1 : address_bus + 1;
    Byte reg = control & 0x06;

    DLOG("INCREMENT/DECREMENT: ");
    switch (reg) {
        case INC_OP__PC:
            cpu->PCL = s_bus;
            cpu->PCH = WORD_HIGH(s_bus);
            DLOG("PC");
            break;
        case INC_OP__DP:
            cpu->DPL = s_bus;
            DLOG("DP");
            break;
        case INC_OP__SP:
            cpu->SP = s_bus;
            DLOG("SP");
            break;
        default:
            DLOG("NONE\n");
    }

    if (reg != INC_MX__NONE) {
        if (control & INC_OP__OP) {
            DLOG("--\n");
        } else {
            DLOG("++\n");
        }
    }
}

// == CPU OPERATIONAL FUNCTIONS == //

void cpu_update_flags(CPU *cpu, Byte flag_bus, Byte set_reset) {
    if (set_reset == SET_RESET_OPCODE) {
        Byte op = flag_bus >> 5; // Get opcode
        Byte op_flag = (op >> 1) & 0x03;

        Byte flag = FLAG_CARRY;
        if (op_flag == 1) {
            flag = FLAG_INTERUPT_DISABLE;
        } else if (op_flag == 2) {
            flag = FLAG_OVERFLOW;
        } else if (op_flag == 3) {
            flag = FLAG_DECIMAL;
        }

        op & 0x01 ? cpu_set_flag(cpu, flag) : cpu_clear_flag(cpu, flag);
    } else {
        if (set_reset & SET_RESET_SEI_CLD) {
            cpu_set_flag(cpu, FLAG_INTERUPT_DISABLE);
            cpu_clear_flag(cpu, FLAG_DECIMAL);
        }

        if (set_reset & SET_RESET_ZERO)
            flag_bus & FLAG_ZERO ? cpu_set_flag(cpu, FLAG_ZERO) : cpu_clear_flag(cpu, FLAG_ZERO);

        if (set_reset & SET_RESET_NEGATIVE)
            flag_bus & FLAG_NEGATIVE ? cpu_set_flag(cpu, FLAG_NEGATIVE) : cpu_clear_flag(cpu, FLAG_NEGATIVE);
        
        if (set_reset & SET_RESET_CARRY)
            flag_bus & FLAG_CARRY ? cpu_set_flag(cpu, FLAG_CARRY) : cpu_clear_flag(cpu, FLAG_CARRY);

        if (set_reset & SET_RESET_OVERFLOW)
            flag_bus & FLAG_OVERFLOW ? cpu_set_flag(cpu, FLAG_OVERFLOW) : cpu_clear_flag(cpu, FLAG_OVERFLOW);
    }
}

// Ignores BRK, RTI, and RTS
bool is_single_byte_instruction(Byte instruction) {
    Byte low = instruction & 0x0f;
    return low == 0x08 || low == 0x0a;
}

bool is_branch_instruction(Byte instruction) {
    return (instruction & 0x1f) == 0x10;
}

bool cpu_should_branch(CPU *cpu, Byte instruction) {
    Byte value       = (instruction >> 5) & 0x01;
    Byte branch_flag = (instruction >> 6) & 0x03;

    Byte flag = FLAG_NEGATIVE;
    if (branch_flag == BRANCH_OVERFLOW) {
        flag = FLAG_OVERFLOW;
    } else if (branch_flag == BRANCH_CARRY) {
        flag = FLAG_CARRY;
    } else if (branch_flag == BRANCH_ZERO) {
        flag = FLAG_ZERO;
    }

    return cpu_flag_is_set(cpu, flag) == value; 
}

bool is_kil_instruction(Byte instruction) {
    switch (instruction) {
        case 0x82:
        case 0xa2:
        case 0xc2:
        case 0xce:
            return false;
    }
    return (instruction & 0x0f) == 0x02;
}

// == CPU MAIN == //
void cpu_run(CPU *cpu, uint32_t max_cycles) {
    printf("CPU turned on.\n\n");
    uint32_t cycles = 0;

    // Run the reset sequence
    if (cpu->interrupt_latch & INTERRUPT_RESET)
        cpu_reset(cpu);

    while (true) {
        cycles++;
        DLOG(">>> %u:\n", cycles);

        cpu_pulse(cpu);

        if ((max_cycles != 0 && cycles == max_cycles) || is_kil_instruction(cpu->IR)) {
            break;
        }
    }

    printf("\nCPU turned off.\n");
    printf("Completed in %u cycles.\n", cycles);
}

// == CPU RESET == //
void cpu_reset(CPU *cpu) {
    printf("CPU resetting...\n\n");
    uint32_t cycles = 0;

    while (true) {
        cycles++;
        DLOG("(RESET)>>> %u:\n", cycles);

        cpu_pulse(cpu);

        if (cpu->interrupt_latch == INTERRUPT_NONE)
            break;
    }

    printf("CPU reset!\n");
}

// == CPU PULSE == //
void cpu_pulse(CPU *cpu) {
    DLOG("IR: 0x%02x; Q: %d\n", cpu->IR, cpu->Q);

    if (cpu->mi_sw & MI_SW__FETCH_OP) {
        cpu->mi_sw ^= MI_SW__FETCH_OP;      // toggle off FETCH.OP
        cpu->nextCtrl = FETCH_OPERAND;
        DLOG("FORCE FETCH OPERAND\n");
    }

    if (cpu->mi_sw & MI_SW__BRANCH_EXIT) {
        cpu->mi_sw ^= MI_SW__BRANCH_EXIT;
        cpu->mi_sw |= MI_SW__FETCH_OP;
        cpu->nextCtrl = FETCH_OPCODE;
        DLOG("BRANCH EXIT\n");
    }

    if (cpu->mi_sw & MI_SW__INC_DPH) {
        cpu->mi_sw ^= MI_SW__INC_DPH;       // toggle off INC.DPH
        cpu->nextCtrl = INCREMENT_DPH;
        DLOG("FORCE INCREMENT DPH\n");
    }

    // == FETCH CONTROL WORD
    struct Controls ctrl = parse_control_word(cpu->nextCtrl);
    DLOG("MICROINSTRUCTION: 0x%08x\n", cpu->nextCtrl);

    if (ctrl.NX == NX_MX__END_INT) {
        cpu->interrupt_latch = INTERRUPT_NONE;
        cpu->is_in_interrupt = false;
    }

    bool interrupt_on = (ctrl.INC & INC_MX__IR_LD) && cpu->interrupt_latch;
    bool inhibit_operand_increment = cpu->Q == 0 && (is_single_byte_instruction(cpu->IR) || cpu->is_in_interrupt);

    if (interrupt_on) {
        cpu->is_in_interrupt = true;
    }

    if (cpu->interrupt_latch) {
        DLOG("INTERRUPT PROGRESS: ");
        if (cpu->interrupt_latch & INTERRUPT_IRQ) {
            DLOG("IRQ");
        } else if (cpu->interrupt_latch & INTERRUPT_NMI) {
            DLOG("NMI");
        } else if (cpu->interrupt_latch & INTERRUPT_RESET) {
            DLOG("RESET");
        }
        DLOG("\n");
    }

    bool inhibit_increment = inhibit_operand_increment || interrupt_on;

    if (inhibit_increment) {
        ctrl.WR = WR_MX__NONE;
        ctrl.INC = (ctrl.INC & INC_MX__IR_LD) | INC_MX__NONE; // Only permit a load IR
        DLOG("PREVENTED PC INCREMENT\n");
    }

    // We only what test for branching once, on the first cycle of the instruction
    if (cpu->Q == 0 && is_branch_instruction(cpu->IR)) {
        if (cpu_should_branch(cpu, cpu->IR)) {
            DLOG("BRANCH TEST SUCCEEDED\n");
        } else {
            cpu->mi_sw |= MI_SW__BRANCH_EXIT;
            DLOG("BRANCH TEST FAILED\n");
        }
    }

    cpu->nextCtrl = ctrl.INC & INC_MX__IR_LD ? FETCH_OPERAND : cpu->micro_rom[cpu->IR][cpu->Q];

    DLOG("NEXT MICROINSTRUCTION: 0x%08x\n", cpu->nextCtrl);

    // == READ REGISTER
    Byte r_bus = select_r(cpu, ctrl.R);
    cpu->alu.A = r_bus;
    DLOG("R BUS: 0x%02x\n", r_bus);

    // == ALU CONTROL
    cpu->alu.A = r_bus;
    Byte alu_flags = alu_operate(&cpu->alu, cpu->flags, ctrl.LUOP, ctrl.A, ctrl.C, cpu->IR & 0x80);
    Byte flag_bus = (ctrl.SF & 0x07) == SF_MX__OPCODE ? cpu->IR : alu_flags;
    DLOG("ALU OUT: 0x%02x\n", cpu->alu.O);

    if ((ctrl.SF & 0x07) == SF_MX__OPCODE)
        DLOG("(OPCODE) ");
    DLOG("FLAG BUS: 0x%02x\n", flag_bus);

    // == ADDRESSING
    if ((ctrl.AD & 0x07) != AD_MX__PBA) {
        cpu->address_bus = select_ad(cpu, ctrl.AD); // Implement DPH.LD
    }
    DLOG("ADDRESS BUS: 0x%04x\n", cpu->address_bus);

    bool mem_read = ctrl.WR != WR_MX__MEM || (cpu->interrupt_latch & INTERRUPT_RESET);
    
    if (!mem_read)
        DLOG("DATA BUS - (OUT): 0x%02x\n", r_bus);

    Byte data_bus = cpu->get_mem(cpu->address_bus, r_bus, mem_read);
    cpu->alu.B = data_bus;

    if (mem_read)
        DLOG("DATA BUS - (IN): 0x%02x\n", data_bus);

    if (ctrl.AD & AD_MX__DPH_LD) {
        cpu->DPH = data_bus;
        DLOG("DPH.LD: LOAD DPH - 0x%02x\n", data_bus);
    }

    // == INCREMENT/DECREMENT
    increment_register(cpu, ctrl.INC, cpu->address_bus);

    if (ctrl.INC & INC_MX__IR_LD) {
        cpu->IR = interrupt_on ? 0x00 : data_bus;   // Load BRK instruction if currently in an interrupt
        if (interrupt_on) {
            DLOG("INTERRUPT ON - LOAD BRK\n");
        } else {
            DLOG("IR.LD: LOAD INSTRUCTION REGISTER - 0x%02x\n", cpu->IR);
        }

        // If loaded instruction is a BRK
        if (cpu->IR == 0) {
            cpu_set_flag(cpu, FLAG_BREAK);
        } else {
            cpu_clear_flag(cpu, FLAG_BREAK);
        }
    }

    // == WRITE BACK
    Byte *write_register = select_wr(cpu, ctrl.WR);
    Byte w_bus = ctrl.SF & SF_MX__ALU ? cpu->alu.O : data_bus; // Is ALU enabled
    DLOG("W BUS: 0x%02x\n", w_bus);

    cpu_update_flags(cpu, flag_bus, select_sf(ctrl.SF));

    if (write_register != NULL) {
        *write_register = w_bus;
    }

    #ifdef CPU_DEBUG
    cpu_print_flags(cpu);
    cpu_print_registers(cpu);
    #endif

    Byte sign_bit;
    // == NEXT
    switch (ctrl.NX & 0x0f) {
        case NX_MX__EXIT_CC:
            sign_bit = (cpu->alu.B >> 7) & 0x01;
            if (sign_bit ^ (alu_flags & 0x01)) {
                cpu->mi_sw |= MI_SW__BRANCH_EXIT;
                DLOG("BRANCH DID NOT CROSS PAGE BOUNDARY\n");
            } else {
                DLOG("BRANCH CROSSED PAGE BOUNDARY\n");
            }
            goto _NX_NEXT;
        case NX_MX__INCDPH_C:
            DLOG("DPH += 1? ");
            if (alu_flags & FLAG_CARRY) {
                cpu->mi_sw |= MI_SW__INC_DPH;
                DLOG("YES\n");
            } else {
                DLOG("NO\n");
            }
            break;  // Do not increment Q
        _NX_NEXT:
        case NX_MX__NEXT:
            cpu->Q++;
            DLOG("CONTINUE INSTRUCTION. Q++\n");
            break;
        case NX_MX__END_D:
            cpu_clear_flag(cpu, FLAG_DECIMAL);
            DLOG("END DECIMAL MODE.\n");
            goto _NX_END;
        case NX_MX__END_INT:
            DLOG("END INTERRUPT.\n");
            goto _NX_END;
        _NX_END:
        case NX_MX__END:
            cpu->Q = 0;
            DLOG("END INSTRUCTION. Q = 0\n");
            break;
    }

    DLOG("\n");
}

void cpu_interrupt(CPU *cpu, Byte interrupt) {
    Byte latch = INTERRUPT_NONE;

    #ifdef CPU_DEBUG
    printf("LATCHING INTERRUPT: ");
    if (interrupt & INTERRUPT_IRQ) {
        printf("IRQ ");
    }
    
    if (interrupt & INTERRUPT_NMI) {
        printf("NMI ");
    }
    
    if (interrupt & INTERRUPT_RESET) {
        printf("RESET");
    }
    
    if (interrupt == INTERRUPT_NONE) {
        printf("NONE");
    }
    putchar('\n');
    #endif

    if ((interrupt & INTERRUPT_IRQ) && !cpu_flag_is_set(cpu, FLAG_INTERUPT_DISABLE)) {
        latch = INTERRUPT_IRQ;
    }

    // INTERRUPT_NMI only overrides INTERRUPT_IRQ
    if ((interrupt & INTERRUPT_NMI) && !cpu->nmi_latch) {
        latch = INTERRUPT_NMI;
    }

    // INTERRUPT_RESET overrides both INTERRUPT_IRQ and INTERRUPT_NMI
    if ((interrupt & INTERRUPT_RESET) && !cpu->reset_latch) {
        latch = INTERRUPT_RESET;
    }

    cpu->reset_latch = !!(interrupt & INTERRUPT_RESET);
    cpu->nmi_latch = !!(interrupt & INTERRUPT_NMI);

    cpu->interrupt_latch = latch;

    #ifdef CPU_DEBUG
    printf("INTERRUPT LATCHED: ");
    if (latch & INTERRUPT_IRQ) {
        printf("IRQ");
    } else if (latch & INTERRUPT_NMI) {
        printf("NMI");
    } else if (latch & INTERRUPT_RESET) {
        printf("RESET");
    } else {
        printf("NONE");
    }
    putchar('\n');
    #endif
}

Word cpu_get_pc(CPU *cpu) {
    return (cpu->PCH << 8) | cpu->PCL;
}

Word cpu_get_dp(CPU *cpu) {
    return (cpu->DPH << 8) | cpu->DPL;
}

Byte cpu_get_a(CPU *cpu) {
    return cpu->A;
}

Byte cpu_get_x(CPU *cpu) {
    return cpu->X;
}

Byte cpu_get_y(CPU *cpu) {
    return cpu->Y;
}

Byte cpu_get_sp(CPU *cpu) {
    return cpu->SP;
}

Byte cpu_get_ir(CPU *cpu) {
    return cpu->IR;
}

void cpu_set_flag(CPU *cpu, Byte flags) {
    cpu->flags |= flags;
}

void cpu_clear_flag(CPU *cpu, Byte flags) {
    cpu->flags &= ~flags;
}

bool cpu_flag_is_set(CPU *cpu, Byte flags) {
    return (cpu->flags & flags) != 0;
}

void cpu_print_flags(CPU *cpu) {
    Byte mask = 1 << 7;
    Byte flags = cpu->flags;
    printf("       NV-BDIZC\n");
    printf("Flags: ");
    for (int i=8; i > 0; i--) {
        putchar(flags & mask ? '1' : '0');
        flags <<= 1;
    }
    putchar('\n');
}

void cpu_print_registers(CPU *cpu) {
    printf("Registers:\n");
    printf("A:  0x%02x\tX:  0x%02x\tY:  0x%02x\n", cpu->A, cpu->X, cpu->Y);
    printf("PC: 0x%04x\tSP: 0x%02x\n", cpu_get_pc(cpu), cpu->SP);
}