#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"

Byte *MEMORY;

Byte *create_memory(Byte term) {
    Byte *mem = calloc(MAX_MEM_SIZE, sizeof(Byte));
    memset(mem, 0xea, MAX_MEM_SIZE * sizeof(Byte));

    mem[0x0000] = 0x00;
    mem[0x0001] = 0x01;

    mem[0xe000] = 0xa2; // ldx #$0a
    mem[0xe001] = term;
    mem[0xe002] = 0xa5; // lda $00
    mem[0xe003] = 0x00;
    mem[0xe004] = 0xe0; // cpx #$00
    mem[0xe005] = 0x00;
    mem[0xe006] = 0xf0; // beq +$0f
    mem[0xe007] = 0x0f;
    mem[0xe008] = 0x18; // clc
    mem[0xe009] = 0x65; // adc $01
    mem[0xe00a] = 0x01;
    mem[0xe00b] = 0x48; // pha
    mem[0xe00c] = 0xa5; // lda $01
    mem[0xe00d] = 0x01;
    mem[0xe00e] = 0x85; // sta $00
    mem[0xe00f] = 0x00;
    mem[0xe010] = 0x68; // pla
    mem[0xe011] = 0x85; // sta $01
    mem[0xe012] = 0x01;
    mem[0xe013] = 0xca; // dex
    mem[0xe014] = 0x4c; // jmp $e002
    mem[0xe015] = 0x02;
    mem[0xe016] = 0xe0;

    mem[0xe017] = 0x02; // kil

    mem[VECTOR_RESET_LOW ] = 0x00;
    mem[VECTOR_RESET_HIGH] = 0xe0;

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

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Missing fibonacci term!\n");
        exit(1);
    }

    Byte fibonacci_term = (Byte) atoi(argv[1]);

    MEMORY = create_memory(fibonacci_term);
    CPU *cpu = cpu_create(get_mem);

    // Run cpu until a JAM (or KIL) opcode is reached
    cpu_run(cpu, 0);

    printf("Fibonacci[%u] = %u\n", fibonacci_term, cpu_get_a(cpu));

    cpu_destroy(cpu);
    free(MEMORY);

    return 0;
}