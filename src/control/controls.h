#ifndef CONTROLS_H
#define CONTROLS_H

#include <stdint.h>

typedef uint32_t ControlWord;

#define CTRL(WR, R, AD, INC, LUOP, SF, A, C, NX) (WR << 28) | (R << 24) | (AD << 20) | (INC << 16) | (LUOP << 12) | (SF << 8) | (A << 6) | (C << 3) | NX

#define MAX_OPCODES         256
#define CONTROLS_PER_OPCODE 8

#define WR_MX__DPL   0x00
#define WR_MX__DPH   0x01
#define WR_MX__SP    0x02
#define WR_MX__X     0x03
#define WR_MX__Y     0x04
#define WR_MX__T     0x05
#define WR_MX__AXS   0x06
#define WR_MX__P     0x07
#define WR_MX__PCL   0x08
#define WR_MX__PCH   0x09
#define WR_MX__A     0x0a
#define WR_MX__NONE  0x0b
#define WR_MX__ML    0x0c
#define WR_MX__WAI   0x0d
#define WR_MX__DPHp1 0x0e
#define WR_MX__MEM   0x0f

#define R_MX__A    0x00
#define R_MX__DPH  0x01
#define R_MX__SP   0x02
#define R_MX__X    0x03
#define R_MX__Y    0x04
#define R_MX__T    0x05
#define R_MX__SPI  0x06
#define R_MX__P    0x07
#define R_MX__PCL  0x08
#define R_MX__PCH  0x09
#define R_MX__DPL  0x0a
#define R_MX__0    0x0b
#define R_MX__NONE 0x0c
#define R_MX__STP  0x0d
#define R_MX__AnX  0x0e
#define R_MX__BCG  0x0f

/*
    AD.MX DPH.LD enables parallel load of the DPH from Data Bus
*/
#define AD_MX__PC    0x00
#define AD_MX__DP    0x01
#define AD_MX__DPt   0x02
#define AD_MX__PBA   0x03
#define AD_MX__zDP   0x04
#define AD_MX__SP    0x05
#define AD_MX__fDP   0x06
#define AD_MX__fCP   0x07
#define AD_MX__DPH_LD 0x08

/*
    INC.MX 0x3 is the same as 0x2 but is ommitted for simplicity
    INC.MX IR.LD enables parallel load of the I register from Data Bus
*/
#define INC_MX__PCi   0x00
#define INC_MX__PCd   0x01
#define INC_MX__NONE  0x02
#define INC_MX__DPLi  0x04
#define INC_MX__DPLd  0x05
#define INC_MX__SPi   0x06
#define INC_MX__SPd   0x07
#define INC_MX__IR_LD 0x08

#define INC_OP__OP 0x01
#define INC_OP__PC 0x00
#define INC_OP__DP 0x04
#define INC_OP__SP 0x06

#define LUOP__0         0x00
#define LUOP__nA_AND_nB 0x01
#define LUOP__A_AND_nB  0x02
#define LUOP__nB        0x03
#define LUOP__nA_AND_B  0x04
#define LUOP__nA        0x05
#define LUOP__A_XOR_B   0x06
#define LUOP__nA_OR_nB  0x07
#define LUOP__A_AND_B   0x08
#define LUOP__n_A_XOR_B 0x09
#define LUOP__A         0x0a
#define LUOP__A_OR_nB   0x0b
#define LUOP__B         0x0c
#define LUOP__nA_OR_B   0x0d
#define LUOP__A_OR_B    0x0e
#define LUOP__FF        0x0f

/*
    SF.MX ALU enables the ALU
*/
#define SF_MX__NONE    0x00
#define SF_MX__OPCODE  0x01
#define SF_MX__SEI_CLD 0x02
#define SF_MX__Z       0x03
#define SF_MX__NZ      0x04
#define SF_MX__NZC     0x05
#define SF_MX__NZCV    0x06
#define SF_MX__NZV     0x07
#define SF_MX__ALU     0x08

#define C_MX__0     0x00
#define C_MX__1     0x01
#define C_MX__C     0x02
#define C_MX__IC    0x03
#define C_MX__LSR   0x04
#define C_MX__ROR   0x05
#define C_MX__BIT   0x06
#define C_MX__ADSIC 0x07

#define A_MX__0   0x00
#define A_MX__A   0x01
#define A_MX__B   0x02

#define NX_MX__NEXT     0x00
#define NX_MX__EXIT_CC  0x01
#define NX_MX__INCDPH_C 0x02
#define NX_MX__EXIT_BTF 0x03
#define NX_MX__END      0x04
#define NX_MX__END_D    0x05
#define NX_MX__END_INT  0x06
#define NX_MX__END_ARNC 0x07

#define FETCH_OPCODE  CTRL(WR_MX__DPL, R_MX__A,   AD_MX__PC, INC_MX__IR_LD, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__END)
#define FETCH_OPERAND CTRL(WR_MX__DPL, R_MX__A,   AD_MX__PC, INC_MX__PCi,   LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT)
#define INCREMENT_DPH CTRL(WR_MX__DPH, R_MX__DPH, AD_MX__PC, INC_MX__NONE,  LUOP__0, SF_MX__ALU,  A_MX__A, C_MX__1, NX_MX__NEXT)

#endif