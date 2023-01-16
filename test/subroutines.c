#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "testing.h"

Byte *MEMORY;

Byte *create_memory() {
    Byte *mem = calloc(MAX_MEM_SIZE, sizeof(Byte));
    memset(mem, 0xea, MAX_MEM_SIZE * sizeof(Byte));

    mem[0xe000] = 0x38; // sec          ; 2
    mem[0xe001] = 0x20; // jsr $e009    ; 6 + 11
    mem[0xe002] = 0x09;
    mem[0xe003] = 0xe0;
    mem[0xe004] = 0x18; // clc          ; 2
    mem[0xe005] = 0x20; // jsr $e009    ; 6 + 13
    mem[0xe006] = 0x09;
    mem[0xe007] = 0xe0;
    mem[0xe008] = 0x02; // kil
    mem[0xe009] = 0xb0; // bcs +#$05    ; 2(3)
    mem[0xe00a] = 0x05;
    mem[0xe00b] = 0xa9; // lda #$00     ; 2
    mem[0xe00c] = 0x00;
    mem[0xe00d] = 0x4c; // jmp $e012    ; 3
    mem[0xe00e] = 0x12;
    mem[0xe00f] = 0xe0;
    mem[0xe010] = 0xa9; // lda #$ff     ; 2
    mem[0xe011] = 0xff;
    mem[0xe012] = 0x60; // rts          ; 6

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
    MEMORY = create_memory();
    CPU *cpu = cpu_create(get_mem);

    cpu_run(cpu, 19);

    assert("A = 0xff", cpu_get_a(cpu) == 0xff);

    cpu_run(cpu, 21);

    assert("A = 0x00", cpu_get_a(cpu) == 0x00);

    putchar('\n');
    cpu_print_flags(cpu);
    cpu_print_registers(cpu);

    puts("\nALL TESTS PASSED");

    cpu_destroy(cpu);
    free(MEMORY);

    return 0;
}