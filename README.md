# 6502 Emulator

This is a 6502 emulator written in C using the microcode ROM and pipeline specified from the [C74-6502 Project](https://c74project.com/). Originally, I wanted the emulator to follow the original 6502 internals more closely, but while programming I realized I was more or less writing an emulator for the C74-6502 instead and just ran with it. Although by the nature of the project it works as a 6502 emulator, I will at some point redo this project to more accurately program the 6502's [PLA](https://en.wikipedia.org/wiki/Programmable_logic_array) and pipeline.

I am not affiliated with the C74-6502 project, but please show your support it was the foundation of this current iteration of the project.

---

## Makefile

By default, the `Makefile` uses `gcc` to compile object files and `ar` to create the final archive.

There are 3 main recipes to be aware of when building the project:

- archive
- release
- debug

### **archive**

The `archive` recipe will compile the C source files in the `./src` and `./src/control` directories and compiles them into an archive using `ar rcs {ARCHIVE_OUT} {OBJECT_FILES}`. The output archive to placed into `./bin/cpu_6502.a`.

### **release**

The `release` recipe will run `archive`, but will compile the object files with the -O2 option.

### **debug**

The `debug` recipe will run `archive`, but will compile the object files with the -DCPU_DEBUG option. The debug archive will log debug statements for each step in the pipeline, i.e. every step that occurs in the `cpu_pulse` function, and the cycle number in the `cpu_run` function.

> Please note that the generated archive is always named `cpu_6502.a` regardless of the recipe used

---

## Compiling in Projects

Because the archive's name is not prefixed with `lib`, the archive should be added to the build options as `-l:cpu_6502.a` instead of `-lcpu_6502`. However, if you want to rename it to `libcpu_6502.a`, that is up to you.


## Including

Including the `cpu.h` header file gives you access to the allowed control functions and macros. Some basic functions include:

| Signature               | Description  |
| ----------------------- | -------------|
| `cpu_create(MemGetter)` | Allocates memory for, initializes, and returns a pointer to the CPU.</br></br>The microcode ROM is initialized to OPCODES, which is a 2 dimensional ControlWord array of size MAX_OPCODES by CONTROLS_PER_OPCODE.</br></br>The MemGetter callback is run in each call to cpu_pulse and should handle what happens when the CPU tries to read or write to memory.|
| `cpu_destroy(CPU*)` | Frees the CPU memory allocated by cpu_create. |
| `cpu_pulse(CPU*)` | Emulates a single clock cycle of the 6502 using a slightly modified version of the microcode ROM and pipeline specified by the C74-6502 Project: https://c74project.com/ |
| `cpu_run(CPU*, uint32_t max_cycles)` | Calls cpu_pulse the number of times specified by max_cycles. If max_cycles == 0, this function will run until an undocumented JAM (aka KIL) opcode is read into the IR.</br></br>If INTERRUPT_RESET is set in cpu->interrupt_latch when this function is called, the cpu_reset function is called first before the main loop begins. |
| `cpu_interrupt(CPU*, Byte interrupt)` | Sets the state of the CPU's interrupt latch, based on the internal state of the CPU.</br></br>The interrupt parameter be a bitwise OR of the interrupt macros: INTERRUPT_RESET, INTERRUPT_NMI, or INTERRUPT_IRQ. However, only one interrupt control will be set.</br></br>INTERRUPT_IRQ applies its logic each time cpu_interrupt is called and FLAG_INTERRUPT_DISABLE is cleared.</br>INTERRUPT_NMI applies its logic once when cpu_interrupt is called and must be cleared on another call before it latches again.</br>INTERRUPT_RESET applies its logic once when cpu_interrupt is called and must be cleared on another call before it latches again.</br></br>INTERRUPT_NMI and INTERRUPT_RESET act this way to emulated both interrupts latching on rising edges.</br></br>The actual 6502 latches interrupts when the control signal goes low (or on the falling edges for NMI and RESETS). This emulator uses the opposite to remain consistent across the rest of this implementation.</br></br>INTERRUPT_NMI will take precedence over INTERRUPT_IRQ, and INTERRUPT_RESET will take precedence over both. |

The flag macros, i.e. `FLAG_CARRY`, `FLAG_ZERO`, and `FLAG_INTERRUPT_DISABLE`, have a value of the respective bit they represent in the [status register](https://www.nesdev.org/wiki/Status_flags).

The vector macros, i.e. `VECTOR_RESET_LOW` and `VECTOR_RESET_HIGH`, have their repective 16-bit addresses that are read when an interrupt is active. For example, `VECTOR_NMI_LOW` is `0xfffa` and `VECTOR_NMI_HIGH` is `0xfffb`. These macros are generally not used, as programs would typically be loaded into memory pre-assembled.

The interrupt macros, i.e. `INTERRUPT_RESET` and `INTERRUPT_IRQ`, are bit flags specific to this implementation and are only used for calls to the `cpu_interrupt` function.

---

## Example Program

This is an example hardcoded program that calculates the 10th term (starting at 0) of the fibonacci sequence.

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"

Byte *MEMORY;

// Allocate and initialize the program memory
Byte *create_memory() {
    Byte *mem = calloc(MAX_MEM_SIZE, sizeof(Byte));
    memset(mem, 0xea, MAX_MEM_SIZE * sizeof(Byte));

    mem[0x0000] = 0x00;
    mem[0x0001] = 0x01;

    mem[0xe000] = 0xa2; // ldx #$0a
    mem[0xe001] = 0x0a;
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

// Callback function to access our program memory
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

    // Run cpu until a JAM (aka KIL) opcode is reached
    cpu_run(cpu, 0);

    printf("Fibonacci[10] = %u\n", cpu_get_a(cpu));

    cpu_destroy(cpu);
    free(MEMORY);

    return 0;
}
~~~

## Testing

To build and run the test files, run the make recipe `test_%` where `%` is replaced with the name of the source file minus the extension. For example, to build and run `test/subroutines.c`, run `make test_subroutines`.

---

## TODO

While I am confident that everything is work correctly at the moment, there are still a couple things I would like to test:

- Decimal Mode
- Branching on page boundaries
- DPH+1 on page boundaries
- Klaus2m5's 6502 functional tests found [here](https://github.com/Klaus2m5/6502_65C02_functional_tests)

---

Again, huge shoutout to the C74-6502 project as it was fundamental to completing this project.