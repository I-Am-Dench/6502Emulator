#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "testing.h"

Byte *MEMORY;

Byte *create_memory() {
    Byte *mem = calloc(MAX_MEM_SIZE, sizeof(Byte));
    memset(mem, 0xea, MAX_MEM_SIZE * sizeof(Byte));

    mem[0xe000] = 0x58; // cli  ; 2
    mem[0xe001] = 0x38; // sec  ; 2

    mem[0xe002] = 0x78; // sei  ; 2
    mem[0xe003] = 0x18; // clc  ; 2

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

    // -- clear interrupt disable and set carry
    cpu_run(cpu, 4);

    bool interrupt_0_carry_1 = !cpu_flag_is_set(cpu, FLAG_INTERUPT_DISABLE) && cpu_flag_is_set(cpu, FLAG_CARRY);
    assert("INTERRUPT DISABLE 0 and CARRY 1", interrupt_0_carry_1);

    // -- set interrupt disable and clear carry
    cpu_run(cpu, 4);

    bool interrupt_1_carry_0 = cpu_flag_is_set(cpu, FLAG_INTERUPT_DISABLE) && !cpu_flag_is_set(cpu, FLAG_CARRY);
    assert("INTERRUPT DISABLE 1 and CARRY 0", interrupt_1_carry_0);

    putchar('\n');
    cpu_print_flags(cpu);

    puts("\nALL TESTS PASSED");

    cpu_destroy(cpu);
    free(MEMORY);

    return 0;
}