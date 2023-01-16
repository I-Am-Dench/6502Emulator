#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

#include "control/controls.h"

typedef uint8_t Byte;
typedef uint16_t Word;

#define FLAG_CARRY            1
#define FLAG_ZERO             1 << 1
#define FLAG_INTERUPT_DISABLE 1 << 2
#define FLAG_DECIMAL          1 << 3
#define FLAG_BREAK            1 << 4
#define FLAG_OVERFLOW         1 << 6
#define FLAG_NEGATIVE         1 << 7

#define VECTOR_NMI_LOW    0xfffa
#define VECTOR_NMI_HIGH   0xfffb
#define VECTOR_RESET_LOW  0xfffc
#define VECTOR_RESET_HIGH 0xfffd
#define VECTOR_IRQ_LOW    0xfffe
#define VECTOR_IRQ_HIGH   0xffff

#define INTERRUPT_NONE  0
#define INTERRUPT_RESET 1
#define INTERRUPT_NMI   1 << 2
#define INTERRUPT_IRQ   1 << 3

#define MAX_MEM_SIZE 65536

extern ControlWord OPCODES[MAX_OPCODES][CONTROLS_PER_OPCODE];

typedef struct _cpu CPU;

typedef Byte (*MemGetter)(Word, Byte, bool is_read);

/*
Allocates memory for, initializes, and returns a pointer to the CPU.

The microcode ROM is initialized to OPCODES, which is a 2 dimensional ControlWord array of size MAX_OPCODES by CONTROLS_PER_OPCODE.

The MemGetter callback is run in each call to cpu_pulse and should handle what happens when the CPU tries to read or write to memory.
*/
CPU *cpu_create(MemGetter);

/*
Frees the CPU memory allocated by cpu_create.
*/
void cpu_destroy(CPU*);

/*
Emulates a single clock cycle of the 6502 using a slightly modified version of the microcode ROM and pipeline specified by the C74-6502 Project: https://c74project.com/
*/
void cpu_pulse(CPU*);

/*
Calls cpu_pulse the number of times specified by max_cycles. If max_cycles == 0, this function will run until an undocumented JAM (aka KIL) opcode is read into the IR.

If INTERRUPT_RESET is set in cpu->interrupt_latch when this function is called, the cpu_reset function is called first before the main loop begins.
*/
void cpu_run(CPU*, uint32_t max_cycles);

/*
Runs the INTERRUPT_RESET routine and returns on completion. This may run a few cycles more than the typical 7 cycles you see during startup due to the cpu possibly being in the middle of another instruction.
*/
void cpu_reset(CPU*);

/*
Sets the state of the CPU's interrupt latch, based on the internal state of the CPU.

The interrupt parameter be a bitwise OR of the interrupt macros: INTERRUPT_RESET, INTERRUPT_NMI, or INTERRUPT_IRQ. However, only one interrupt control will be set.

INTERRUPT_IRQ applies its logic each time cpu_interrupt is called and FLAG_INTERRUPT_DISABLE is cleared.
INTERRUPT_NMI applies its logic once when cpu_interrupt is called and must be cleared on another call before it latches again.
INTERRUPT_RESET applies its logic once when cpu_interrupt is called and must be cleared on another call before it latches again.

INTERRUPT_NMI and INTERRUPT_RESET act this way to emulated both interrupts latching on rising edges.

The actual 6502 latches interrupts when the control signal goes low (or on the falling edges for NMI and RESETS). This emulator uses the opposite to remain consistent across the rest of this implementation. 

INTERRUPT_NMI will take precedence over INTERRUPT_IRQ, and INTERRUPT_RESET will take precedence over both.
*/
void cpu_interrupt(CPU*, Byte interrupt);

Word cpu_get_pc(CPU*);
Word cpu_get_dp(CPU*);

Byte cpu_get_a(CPU*);
Byte cpu_get_x(CPU*);
Byte cpu_get_y(CPU*);

Byte cpu_get_sp(CPU*);
Byte cpu_get_ir(CPU*);

void cpu_set_flag(CPU*, Byte);
void cpu_clear_flag(CPU*, Byte);
bool cpu_flag_is_set(CPU*, Byte);

void cpu_print_flags(CPU*);
void cpu_print_registers(CPU*);

#endif