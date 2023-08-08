#include "alu.h"

#include "control/controls.h"

#include <stdio.h>

/*
    Design taken from:
    http://6502.org/users/dieter/a1/a1_4.htm

    Dieter uses a (kind of) 'transposed' multiplexor where the select bits are used as the LU inputs.
    Typically, the selects bits, A and B, are used to select which bit from input I, in this case I0-I3, to output to Q.

    If I = 0110, and the select input is 01, bit 1 would be selected from I, which is 1 in this case. If the select input
    is 11, bit 3 would be selected from I, which would be 0.

    By essentially reversing how to think about this multiplexor, we can instead use the I inputs as what the c74-Project calls
    a 'lookup table'. For example, to XOR bits A and B the output Q must be 0 only when A and B, the select input,
    is either 00 or 11. Setting input I to 0110 would yield the proper results since input 00 and 11 select bits 0 and 3
    respectively. When the select input is 01 or 10, they output a 1 because they select bits 1 and 2 of input I respectively.

    For this implementation, the actually hardware logic is used to remove the need for arrays, jump tables, or looping through
    each bit. I could have just did a jump table for each operation, but where is the fun in that.
*/
Byte lu_operate(Byte a, Byte b, Byte luop) {
    Byte na = ~a;
    Byte nb = ~b;

    Byte bit0 = luop & 0x01 ? 0xff : 0x00;
    Byte bit1 = luop & 0x02 ? 0xff : 0x00;
    Byte bit2 = luop & 0x04 ? 0xff : 0x00;
    Byte bit3 = luop & 0x08 ? 0xff : 0x00;

    Byte and0 = na & nb & bit0;
    Byte and1 = a  & nb & bit1;
    Byte and2 = na & b  & bit2;
    Byte and3 = a  & b  & bit3;

    return and0 | and1 | and2 | and3;
}

Byte select_a(Byte a, Byte b, Byte a_mux) {
    Byte bit_a = a_mux & 0x01 ? 0xff : 0x00;
    Byte bit_b = a_mux & 0x02 ? 0xff : 0x00;

    return (a & bit_a) | (b & bit_b);
}

Byte get_overflow(Byte w, Byte r, Byte y) {
    return (w ^ r) & (w ^ y) & 0x80;
}

Byte get_bcd(Byte b, Byte carry_in, bool is_subtract) {
    Byte nibble = b & 0x0f;

    Byte correction = 0;
    if (is_subtract && !carry_in) {
        correction = 0x0a;  // This is the 4-bit equivalent of one's complement -6 without the sign extension on the whole byte 
    } else if (!is_subtract && (nibble > 9 || carry_in)) {
        correction = 0x06;
    }

    return nibble + correction;
}

Byte alu_operate(struct ALU *alu, Byte flags, Byte luop, Byte a_mux, Byte c_mux, bool is_subtract) {
    Byte a = alu->A;
    Byte b = alu->B;

    if (c_mux == C_MX__ADSIC) {
        b = (b & 0x80) ? 0xff : 0x00; // Sign extension
    }

    #ifdef CPU_DEBUG
    printf("ALU:\n");
    printf("\tA: 0x%02x\n", a);
    printf("\tB: 0x%02x\n", b);
    printf("\tLUOP: 0x%02x\n", luop);
    printf("\tA.MX: 0x%02x\n", a_mux);
    printf("\tC.MX: 0x%02x\n", c_mux);
    #endif

    Byte lu_x = select_a(alu->A, b, a_mux);
    Byte lu_y = lu_operate(a, b, luop);

    Byte carry_in = c_mux == C_MX__1 ? 0x01 : 0x00; // Hard code to either 0 or 1
    if (c_mux == C_MX__C) {
        carry_in = flags & FLAG_CARRY;              // Use flag bit
    } else if (c_mux == C_MX__ROR) {
        carry_in = (flags & FLAG_CARRY) << 7;       // Use flag bit on bit 7
    } else if (c_mux == C_MX__IC || c_mux == C_MX__ADSIC) {
        carry_in = alu->IC;                         // Use internal carry
    }

    Word adder = lu_y + lu_x + (carry_in & 0x01); // Adder is of type Word in order to capture the carry bit
    Byte shift = (Byte) ((adder & 0x00ff) >> 1) | (carry_in & 0x80);
    alu->IC = (adder >> 8) & 0x01;

    if (flags & FLAG_DECIMAL) {
        Byte lo = get_bcd(adder, carry_in, is_subtract);
        Byte hi = get_bcd(adder >> 4, lo & 0x10, is_subtract);
        alu->IC = (hi & 0x10) >> 4;
        if (is_subtract)
            alu->IC = !alu->IC;
            
        adder = (hi << 4) | lo;
    }

    alu->O = c_mux == C_MX__LSR || c_mux == C_MX__ROR ? shift : (Byte)adder;

    Byte carry_out = alu->IC;
    Byte zero = alu->O == 0 ? FLAG_ZERO : 0x00;

    Byte overflow = get_overflow(alu->O, alu->A, lu_y) ? FLAG_OVERFLOW : 0;
    Byte negative = alu->O & FLAG_NEGATIVE;

    if (c_mux == C_MX__BIT) {
        overflow = alu->B & FLAG_OVERFLOW;
        negative = alu->B & FLAG_NEGATIVE;
    }

    return negative | overflow | zero | carry_out;
}