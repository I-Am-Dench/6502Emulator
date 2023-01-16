#ifndef ALU_H
#define ALU_H

#include "cpu.h"

struct ALU {
    Byte A, B;
    Byte O;
    Byte IC; // Internal Carry
};

/*
    Sets the output of the alu after operating based on the control bits from LUOP, A.MX, and C.MX.
    The resulting flags, corresponding to the internal CPU flags, are returns. The returned flags
    only correspond to the ALU's operation, not the CPU's state. I.e. the state of the Interrupt Disable
    flags is not retained in the output flags.
*/
Byte alu_operate(struct ALU*, Byte flags, Byte luop, Byte a_mux, Byte c_mux, bool is_subtract);

#endif