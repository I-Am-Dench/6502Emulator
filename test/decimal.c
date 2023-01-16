#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "testing.h"

Byte *MEMORY;

Byte *create_memory() {
    Byte *mem = calloc(MAX_MEM_SIZE, sizeof(Byte));
    memset(mem, 0xea, MAX_MEM_SIZE * sizeof(Byte));

    // TODO: Implement test program

    mem[0xe000] = 0x02;

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

    cpu_run(cpu, 0);

    cpu_destroy(cpu);
    free(MEMORY);

    return 0;
}