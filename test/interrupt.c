#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "testing.h"

#define NUM_TESTS 8

#define ZP_IRQ_COUNTER 0x00
#define ZP_NMI_COUNTER 0x01

#define init_interrupt(inter, delay) (struct interrupt) {inter, delay}

Byte *MEMORY;

struct interrupt {
    Byte interrupt;
    int delay;
};

Byte *create_memory() {
    Byte *mem = calloc(MAX_MEM_SIZE, sizeof(Byte));
    memset(mem, 0xea, MAX_MEM_SIZE * sizeof(Byte));

    mem[0xe000] = 0x58; // cli          ; 2
    mem[0xe001] = 0xa9; // lda #$ff     ; 2
    mem[0xe002] = 0xff;
    mem[0xe003] = 0x38; // sec          ; 2
    mem[0xe004] = 0xe9; // sbc #$ba     ; 2
    mem[0xe005] = 0xba;
    mem[0xe006] = 0x48; // pha          ; 3
    mem[0xe007] = 0xa9; // lda #$00     ; 2
    mem[0xe008] = 0x00;
    mem[0xe009] = 0x85; // sta $00      ; 3
    mem[0xe00a] = ZP_IRQ_COUNTER;
    mem[0xe00b] = 0x85; // sta $01      ; 3
    mem[0xe00c] = ZP_NMI_COUNTER;
    mem[0xe00d] = 0xea; // nop          ; 2 -- first IRQ
    mem[0xe00e] = 0xea; // nop          ; 2
    mem[0xe00f] = 0xea; // nop          ; 2
    mem[0xe010] = 0xea; // nop          ; 2
    mem[0xe011] = 0xea; // nop          ; 2
    mem[0xe012] = 0x4c; // jmp $e012    ; 3
    mem[0xe013] = 0x12;
    mem[0xe014] = 0xe0;

    // nmi
    mem[0xe800] = 0xe6; // inc $01      ; 5
    mem[0xe801] = ZP_NMI_COUNTER;
    mem[0xe802] = 0x40; // rti          ; 6

    // irq
    mem[0xf000] = 0xe6; // inc $00      ; 5
    mem[0xf001] = ZP_IRQ_COUNTER;
    mem[0xf002] = 0x40; // rti          ; 6

    mem[VECTOR_NMI_LOW ] = 0x00;
    mem[VECTOR_NMI_HIGH] = 0xe8;

    mem[VECTOR_RESET_LOW ] = 0x00;
    mem[VECTOR_RESET_HIGH] = 0xe0;

    mem[VECTOR_IRQ_LOW ] = 0x00;
    mem[VECTOR_IRQ_HIGH] = 0xf0;
    
    return mem;
}

Byte get_mem(Word address, Byte data, bool is_read) {
    if (is_read) {
        return MEMORY[address];
    } else {
        MEMORY[address] = data;
        return 0;
    }
}

void initialize_tests(struct interrupt tests[NUM_TESTS]) {
    tests[0] = init_interrupt(INTERRUPT_IRQ,   19);     // delay until initial setup code is finished
    tests[1] = init_interrupt(INTERRUPT_NMI,   20);     // delay until first IRQ is finished -- (first NOP) + IRQ
    tests[2] = init_interrupt(INTERRUPT_RESET, 18);     // delay until NMI is finished

    tests[3] = init_interrupt(INTERRUPT_IRQ | INTERRUPT_NMI, 32);   // delay until RESET is finished -- RESET + setup code
    tests[4] = init_interrupt(INTERRUPT_IRQ | INTERRUPT_NMI, 18);   // delay until NMI is finished
    tests[5] = init_interrupt(INTERRUPT_IRQ,                 18);   // delay until IRQ is finished
    tests[6] = init_interrupt(INTERRUPT_IRQ | INTERRUPT_NMI, 18);   // delay until IRQ is finished
    tests[7] = init_interrupt(INTERRUPT_NONE,                18);   // delay until NMI is finished
}

/* 
Each interrupt should usually take 13 (BRK plus RTI) + n cycles to complete, where n is the number of functional cycles minus the RTI
*/
int main(int argc, char *argv[]) {
    MEMORY = create_memory();
    CPU *cpu = cpu_create(get_mem);

    struct interrupt tests[NUM_TESTS];
    initialize_tests(tests);

    int test_index = 0;
    struct interrupt current_test = tests[0];

    cpu_reset(cpu);

    uint32_t cycles = 0;
    uint32_t target_cycle = 0;
    while (test_index < NUM_TESTS) {
        cycles++;
        // printf(">>> %u:\n", cycles);
    
        if (target_cycle == 0) {
            target_cycle = cycles + current_test.delay;
        }

        if (cycles == target_cycle) {
            cpu_interrupt(cpu, current_test.interrupt);

            current_test = tests[++test_index];
            target_cycle = 0;
        }

        cpu_pulse(cpu);
    }

    printf("Completed in %u cycles.\n\n", cycles);

    cpu_print_flags(cpu);
    cpu_print_registers(cpu);

    putchar('\n');

    print_mem_address(MEMORY, ZP_IRQ_COUNTER);
    print_mem_address(MEMORY, ZP_NMI_COUNTER);

    assert("IRQ COUNTER == 2", MEMORY[ZP_IRQ_COUNTER] == 2);
    assert("NMI COUNTER == 2", MEMORY[ZP_NMI_COUNTER] == 2);

    printf("ALL TESTS PASSED\n");

    cpu_destroy(cpu);
    free(MEMORY);

    return 0;
}