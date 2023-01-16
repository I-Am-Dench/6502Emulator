#ifndef TESTING_H
#define TESTING_H

#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"

void assert(const char *msg, bool ok) {
    if (ok) {
        printf("ASSERT [SUCCESS]: %s\n", msg);
    } else {
        printf("ASSERT [FAILED]: %s\n", msg);
        exit(1);
    }
}

void print_mem_address(Byte *mem, Word address) {
    printf("MEMORY[0x%04x] = 0x%02x\n", address, mem[address]);
}

void print_mem_range(Byte *mem, Word start, Word end) {
    for (Word address=start; address < end; address++) {
        if (address % 16 == 0) {
            printf("%04x: ", address & 0x00ff);
        }

        printf("%02x ", mem[address]);

        if ((address & 0x0f) == 0x0f) {
            putchar('\n');
        }
    }
}

void print_mem_stack(Byte *mem) {
    for (Word address=0x01ff; address >= 0x0100; address--) {
        if ((address & 0x0f) == 0x0f) {
            printf("%02x: ", address & 0x00ff);
        }

        printf("%02x ", mem[address]);

        if ((address & 0x0f) == 0x00) {
            putchar('\n');
        }
    }
}

#endif