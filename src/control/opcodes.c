#include "controls.h"

#define NOP { FETCH_OPCODE }

#define CLEAR_FLAG { CTRL(WR_MX__NONE, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__0,  (SF_MX__ALU | SF_MX__OPCODE), A_MX__0, C_MX__0, NX_MX__END) }
#define SET_FLAG   { CTRL(WR_MX__NONE, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__FF, (SF_MX__ALU | SF_MX__OPCODE), A_MX__0, C_MX__0, NX_MX__END) }

#define BRANCH {\
    CTRL(WR_MX__PCL, R_MX__PCL, AD_MX__PC, INC_MX__NONE, LUOP__B, SF_MX__ALU, A_MX__A, C_MX__0,     NX_MX__EXIT_CC),\
    CTRL(WR_MX__PCH, R_MX__PCH, AD_MX__PC, INC_MX__NONE, LUOP__B, SF_MX__ALU, A_MX__A, C_MX__ADSIC, NX_MX__NEXT),\
    FETCH_OPCODE\
}

ControlWord OPCODES[MAX_OPCODES][CONTROLS_PER_OPCODE] = {
    [0x00] = { /* BRK */
        CTRL(WR_MX__MEM, R_MX__PCH, AD_MX__SP,  INC_MX__SPd,   LUOP__0, SF_MX__NONE,    A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__MEM, R_MX__PCL, AD_MX__SP,  INC_MX__SPd,   LUOP__0, SF_MX__NONE,    A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__MEM, R_MX__P,   AD_MX__SP,  INC_MX__SPd,   LUOP__0, SF_MX__NONE,    A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__PCL, R_MX__A,   AD_MX__fCP, INC_MX__DPLi,  LUOP__0, SF_MX__NONE,    A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__PCH, R_MX__A,   AD_MX__fDP, INC_MX__NONE,  LUOP__0, SF_MX__SEI_CLD, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__DPL, R_MX__A,   AD_MX__PC,  INC_MX__IR_LD, LUOP__0, SF_MX__NONE,    A_MX__0, C_MX__0, NX_MX__END_INT)
    },
    [0x01] = { /* ORA (Ind, X) */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE,  LUOP__B,       SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__A, AD_MX__zDP, INC_MX__DPLi,  LUOP__0,       SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,       SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DPt, INC_MX__NONE,  LUOP__0,       SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__A_OR_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x02] = { /* KIL - IMPLEMENT */
        FETCH_OPCODE
    },
    /* SLO (Ind, X) */
    /* LDD zpg */
    [0x05] = { /* ORA zpg */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,       SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__A_OR_B, (SF_MX__ALU | SF_MX__NZ), A_MX__A, C_MX__0, NX_MX__END),
    },
    [0x06] = { /* ASL zpg */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__0, AD_MX__PBA, INC_MX__NONE, LUOP__B, (SF_MX__ALU | SF_MX__NZC), A_MX__B, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* SLO zpg */
    [0x08] = { /* PHP */
        CTRL(WR_MX__MEM, R_MX__P, AD_MX__SP, INC_MX__SPd, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    [0x09] = { /* ORA # */
        CTRL(WR_MX__A, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__A_OR_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x0a] = { /* ASL A */
        CTRL(WR_MX__A, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__A, (SF_MX__ALU | SF_MX__NZC), A_MX__A, C_MX__0, NX_MX__END)
    },
    /* ANC # */
    /* LDD Abs */
    [0x0d] = { /* ORA Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC, INC_MX__PCi,   LUOP__0,       SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP, INC_MX__NONE,  LUOP__0,       SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__A_OR_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x0e] = { /* ASL Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC,  INC_MX__PCi,  LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,  INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__0, AD_MX__PBA, INC_MX__NONE, LUOP__B, (SF_MX__ALU | SF_MX__NZC), A_MX__B, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T, AD_MX__DP,  INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* SLO Abs */
    [0x10] = BRANCH, /* BPL Rel */
    [0x11] = { /* ORA (Ind),Y */
        CTRL(WR_MX__NONE, R_MX__A,  AD_MX__zDP,                  INC_MX__DPLi,  LUOP__0,       SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__DPL,  R_MX__Y, (AD_MX__DPH_LD | AD_MX__zDP), INC_MX__NONE,  LUOP__B,       SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A,  AD_MX__DP,                   INC_MX__NONE,  LUOP__0,       SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A,  AD_MX__PC,                   INC_MX__IR_LD, LUOP__A_OR_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    /* KIL */
    /* SLO (Ind),Y */
    /* LDD zpg,X */
    [0x15] = {
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE,  LUOP__B,       SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,       SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__A_OR_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x16] = {
        CTRL(WR_MX__DPL, R_MX__X, AD_MX__zDP, INC_MX__NONE, LUOP__B, SF_MX__ALU, A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T, R_MX__0, AD_MX__PBA, INC_MX__NONE, LUOP__B, (SF_MX__ALU | SF_MX__NZC), A_MX__B, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__MEM, R_MX__T, AD_MX__zDP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* SLO zpg,X */
    [0x18] = CLEAR_FLAG, /* CLC */
    [0x19] = { /* ORA Abs,Y */
        CTRL(WR_MX__DPL,  R_MX__Y, AD_MX__DPH_LD, INC_MX__PCi,  LUOP__B,       SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,    INC_MX__NONE, LUOP__0,       SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,    INC_MX__PCi,  LUOP__A_OR_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x1a] = NOP,
    /* SLO Abs,Y */
    /* LDD Abs,X */
    [0x1d] = { /* ORA Abs,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__DPH_LD, INC_MX__PCi,  LUOP__B,       SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,    INC_MX__NONE, LUOP__0,       SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,    INC_MX__PCi,  LUOP__A_OR_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x1e] = { /* ASL Abs,X */
        CTRL(WR_MX__DPL,  R_MX__X,   AD_MX__DPH_LD, INC_MX__PCi,  LUOP__B,  SF_MX__ALU,               A_MX__A, C_MX__0,  NX_MX__NEXT),
        CTRL(WR_MX__DPH,  R_MX__DPH, AD_MX__DPt,   INC_MX__NONE, LUOP__0,  SF_MX__ALU,               A_MX__A, C_MX__IC, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A,   AD_MX__DP,    INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,  NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__0,   AD_MX__PBA,   INC_MX__NONE, LUOP__B, (SF_MX__ALU | SF_MX__NZC), A_MX__B, C_MX__0,  NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T,   AD_MX__DP,    INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,  NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* SLOT Abs,X */
    [0x20] = { /* JSR Abs */
        CTRL(WR_MX__NONE, R_MX__A,   AD_MX__SP, INC_MX__NONE,  LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__PCH, AD_MX__SP, INC_MX__SPd,   LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__PCL, AD_MX__SP, INC_MX__SPd,   LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__DPH,  R_MX__A,   AD_MX__PC, INC_MX__PCi,   LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A,   AD_MX__DP, INC_MX__IR_LD, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__END),
    },
    [0x21] = { /* AND (Ind,X) */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE,  LUOP__B,        SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__A, AD_MX__zDP, INC_MX__DPLi,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DPt, INC_MX__NONE,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__A_AND_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    /* KIL */
    /* RLA (Ind,X) */
    [0x24] = { /* BIT zpg */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,        SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__A_AND_B, (SF_MX__ALU | SF_MX__NZV), A_MX__0, C_MX__BIT, NX_MX__END)
    },
    [0x25] = { /* AND zpg */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__A_AND_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x26] = { /* ROL zpg */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__0, AD_MX__PBA, INC_MX__NONE, LUOP__B, (SF_MX__ALU | SF_MX__NZC), A_MX__B, C_MX__C, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* RLA zpg */
    [0x28] = { /* PLP */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__SP, INC_MX__SPi,  LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__P,    R_MX__A, AD_MX__SP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    [0x29] = { /* AND # */
        CTRL(WR_MX__A, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__A_AND_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x2a] = { /* ROL A */
        CTRL(WR_MX__A, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__A, (SF_MX__ALU | SF_MX__NZC), A_MX__A, C_MX__C, NX_MX__END)
    },
    /* ANC # */
    [0x2c] = { /* BIT Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC, INC_MX__PCi,   LUOP__0,        SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP, INC_MX__NONE,  LUOP__0,        SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__A_AND_B, (SF_MX__ALU | SF_MX__NZV), A_MX__0, C_MX__BIT, NX_MX__END)
    },
    [0x2d] = { /* AND Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC, INC_MX__PCi,   LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP, INC_MX__NONE,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__A_AND_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x2e] = { /* ROL Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC,  INC_MX__PCi,  LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,  INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__0, AD_MX__PBA, INC_MX__NONE, LUOP__B, (SF_MX__ALU | SF_MX__NZC), A_MX__B, C_MX__C, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T, AD_MX__DP,  INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* RLA Abs */
    [0x30] = BRANCH, /* BMI Rel */
    [0x31] = { /* AND (Ind),Y */
        CTRL(WR_MX__NONE, R_MX__A,  AD_MX__zDP,                  INC_MX__DPLi,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__DPL,  R_MX__Y, (AD_MX__DPH_LD | AD_MX__zDP), INC_MX__NONE,  LUOP__B,        SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A,  AD_MX__DP,                   INC_MX__NONE,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A, R_MX__A,     AD_MX__PC,                   INC_MX__IR_LD, LUOP__A_AND_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    /* KIL */
    /* RLA (Ind),Y */
    /* LDD zpg,X */
    [0x35] = { /* AND zpg,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE,  LUOP__B,        SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__A_AND_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x36] = { /* ROL zpg,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE, LUOP__B,  SF_MX__ALU,               A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__0, AD_MX__PBA, INC_MX__NONE, LUOP__B, (SF_MX__ALU | SF_MX__NZC), A_MX__B, C_MX__C, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* RLA zpg,X */
    [0x38] = SET_FLAG, /* SEC */
    [0x39] = { /* AND Abs,Y */
        CTRL(WR_MX__DPL,  R_MX__Y, AD_MX__DPH_LD, INC_MX__PCi,   LUOP__B,        SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,     INC_MX__NONE,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,     INC_MX__IR_LD, LUOP__A_AND_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x3a] = NOP,
    /* RLA Abs,Y */
    /* LDD Abs,X */
    [0x3d] = { /* AND Abs,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__DPH_LD, INC_MX__PCi,   LUOP__B,        SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,     INC_MX__NONE,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,     INC_MX__IR_LD, LUOP__A_AND_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x3e] = { /* ROL Abs,X */
        CTRL(WR_MX__DPL,  R_MX__X,   AD_MX__DPH_LD, INC_MX__PCi,  LUOP__B,  SF_MX__ALU,               A_MX__A, C_MX__0,  NX_MX__NEXT),
        CTRL(WR_MX__DPH,  R_MX__DPH, AD_MX__DP,     INC_MX__NONE, LUOP__0,  SF_MX__ALU,               A_MX__A, C_MX__IC, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A,   AD_MX__DP,     INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,  NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__0,   AD_MX__PBA,    INC_MX__NONE, LUOP__B, (SF_MX__ALU | SF_MX__NZC), A_MX__B, C_MX__C,  NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T,   AD_MX__DP,     INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,  NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* RLA Abs,X */
    [0x40] = { /* RTI */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__SP, INC_MX__SPi, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__P,    R_MX__A, AD_MX__SP, INC_MX__SPi, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__PCL,  R_MX__A, AD_MX__SP, INC_MX__SPi, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__PCH,  R_MX__A, AD_MX__SP, INC_MX__SPi, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    [0x41] = { /* EOR (Ind,X) */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE,  LUOP__B,        SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__A, AD_MX__zDP, INC_MX__DPLi,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DPt, INC_MX__NONE,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__A_XOR_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    /* KIL */
    /* SRE (Ind,X) */
    /* LDD zpg */
    [0x45] = {
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__A_XOR_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END),
    },
    [0x46] = { /* LSR zpg */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__0, AD_MX__PBA, INC_MX__NONE, LUOP__B, (SF_MX__ALU | SF_MX__NZC), A_MX__0, C_MX__LSR, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* SRE zpg */
    [0x48] = { /* PHA */
        CTRL(WR_MX__MEM, R_MX__A, AD_MX__SP, INC_MX__SPd, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    [0x49] = { /* EOR # */
        CTRL(WR_MX__A, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__A_XOR_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x4a] = { /* LSR A */
        CTRL(WR_MX__A, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__A, (SF_MX__ALU | SF_MX__NZC), A_MX__0, C_MX__LSR, NX_MX__END)
    },
    /* ALR # */
    [0x4c] = { /* JMP Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC, INC_MX__PCi,   LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP, INC_MX__IR_LD, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x4d] = { /* EOR Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC, INC_MX__PCi,   LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP, INC_MX__NONE,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__A_XOR_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x4e] = { /* LSR Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC,  INC_MX__PCi,  LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,  INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__0, AD_MX__PBA, INC_MX__NONE, LUOP__B, (SF_MX__ALU | SF_MX__NZC), A_MX__0, C_MX__LSR, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T, AD_MX__DP,  INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* SRE Abs */
    [0x50] = BRANCH, /* BVC Rel */
    [0x51] = { /* EOR (Ind),Y */
        CTRL(WR_MX__NONE, R_MX__A,  AD_MX__zDP,                  INC_MX__DPLi,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__DPL,  R_MX__Y, (AD_MX__DPH_LD | AD_MX__zDP), INC_MX__NONE,  LUOP__B,        SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A,  AD_MX__DP,                   INC_MX__NONE,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A,  AD_MX__PC,                   INC_MX__IR_LD, LUOP__A_XOR_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    /* KIL */
    /* SRE (Ind),Y */
    /* LDD zpg,X */
    [0x55] = { /* EOR zpg,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE,  LUOP__B,        SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__A_XOR_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x56] = { /* LSR zpg,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE, LUOP__B,  SF_MX__ALU,               A_MX__A, C_MX__0,   NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__0, AD_MX__PBA, INC_MX__NONE, LUOP__B, (SF_MX__ALU | SF_MX__NZC), A_MX__0, C_MX__LSR, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* SRE zpg,X */
    [0x58] = CLEAR_FLAG, /* CLI */
    [0x59] = { /* EOR Abs,Y */
        CTRL(WR_MX__DPL,  R_MX__Y, AD_MX__DPH_LD, INC_MX__PCi,   LUOP__B,        SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,     INC_MX__NONE,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,     INC_MX__IR_LD, LUOP__A_XOR_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x5a] = NOP,
    /* SRE Abs,Y */
    /* LDD Abs,X */
    [0x5d] = { /* EOR Abs,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__DPH_LD, INC_MX__PCi,   LUOP__B,        SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,     INC_MX__NONE,  LUOP__0,        SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,     INC_MX__IR_LD, LUOP__A_XOR_B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x5e] = { /* LSR Abs,X */
        CTRL(WR_MX__DPL,  R_MX__X,   AD_MX__DPH_LD, INC_MX__PCi,  LUOP__B,  SF_MX__ALU,               A_MX__A, C_MX__0,   NX_MX__NEXT),
        CTRL(WR_MX__DPH,  R_MX__DPH, AD_MX__DP,     INC_MX__NONE, LUOP__0,  SF_MX__ALU,               A_MX__A, C_MX__IC,  NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A,   AD_MX__DP,     INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__0,   AD_MX__PBA,    INC_MX__NONE, LUOP__B, (SF_MX__ALU | SF_MX__NZC), A_MX__0, C_MX__LSR, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T,   AD_MX__DP,     INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* SRE Abs,X */
    [0x60] = { /* RTS */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__SP, INC_MX__SPi, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__PCL, R_MX__A, AD_MX__SP, INC_MX__SPi, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__PCH, R_MX__A, AD_MX__SP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPERAND,
        FETCH_OPCODE
    },
    [0x61] = { /* ADC (Ind,X) */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE,  LUOP__B,  SF_MX__ALU,                A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__A, AD_MX__zDP, INC_MX__DPLi,  LUOP__0,  SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,  SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DPt, INC_MX__NONE,  LUOP__0,  SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZCV), A_MX__A, C_MX__C, NX_MX__END_D),
    },
    /* KIL */
    /* RRA (Ind,X) */
    /* LDD zpg */
    [0x65] = { /* ADC zpg */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,  SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZCV), A_MX__A, C_MX__C, NX_MX__END_D)
    },
    [0x66] = { /* ROR zpg */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__0, AD_MX__PBA, INC_MX__NONE, LUOP__B, (SF_MX__ALU | SF_MX__NZC), A_MX__0, C_MX__ROR, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* RRA zpg */
    [0x68] = { /* PLA */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__SP, INC_MX__SPi,   LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__SP, INC_MX__NONE,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0x69] = { /* ADC # */
        CTRL(WR_MX__A, R_MX__A, AD_MX__PC, INC_MX__PCi, LUOP__B, (SF_MX__ALU | SF_MX__NZCV), A_MX__A, C_MX__C, NX_MX__END_D)
    },
    [0x6a] = { /* ROR A */
        CTRL(WR_MX__A, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__A, (SF_MX__ALU | SF_MX__NZC), A_MX__0, C_MX__ROR, NX_MX__END)
    },
    /* ARR # */
    [0x6c] = { /* (Abs) */
        CTRL(WR_MX__DPH, R_MX__A, AD_MX__PC, INC_MX__PCi,  LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__PCL, R_MX__A, AD_MX__DP, INC_MX__DPLi, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__PCH, R_MX__A, AD_MX__DP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    [0x6d] = { /* ADC Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC, INC_MX__PCi,   LUOP__0,  SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP, INC_MX__NONE,  LUOP__0,  SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZCV), A_MX__A, C_MX__C, NX_MX__END_D),
    },
    [0x6e] = { /* ROR Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC,  INC_MX__PCi,  LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,  INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__0, AD_MX__PBA, INC_MX__NONE, LUOP__B, (SF_MX__ALU | SF_MX__NZC), A_MX__0, C_MX__ROR, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T, AD_MX__DP,  INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* RRA Abs */
    [0x70] = BRANCH, /* BVS Rel */
    [0x71] = { /* ADC (Ind),Y */
        CTRL(WR_MX__NONE, R_MX__A,  AD_MX__zDP,                  INC_MX__DPLi,  LUOP__0,  SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__DPL,  R_MX__Y, (AD_MX__DPH_LD | AD_MX__zDP), INC_MX__NONE,  LUOP__B,  SF_MX__ALU,                A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A,  AD_MX__DP,                   INC_MX__NONE,  LUOP__0,  SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A,  AD_MX__PC,                   INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZCV), A_MX__A, C_MX__C, NX_MX__END_D)
    },
    /* KIL */
    /* RRA (Ind),Y */
    /* LDD zpg,X */
    [0x75] = { /* ADC zpg,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE,  LUOP__B,  SF_MX__ALU,                A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,  SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZCV), A_MX__A, C_MX__C, NX_MX__END_D)
    },
    [0x76] = { /* ROR zpg,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE, LUOP__B,  SF_MX__ALU,               A_MX__A, C_MX__0,   NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__0, AD_MX__PBA, INC_MX__NONE, LUOP__B, (SF_MX__ALU | SF_MX__NZC), A_MX__0, C_MX__ROR, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* RRA zpg,X */
    [0x78] = SET_FLAG, /* SEI */
    [0x79] = { /* ADC Abs,Y */
        CTRL(WR_MX__DPL,  R_MX__Y, AD_MX__DPH_LD, INC_MX__PCi,   LUOP__B,  SF_MX__ALU,                A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,     INC_MX__NONE,  LUOP__0,  SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,     INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZCV), A_MX__A, C_MX__C, NX_MX__END_D)
    },
    [0x7a] = NOP,
    /* RRA Abs,Y */
    /* LDD Abs,X */
    [0x7d] = { /* ADC Abs,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__DPH_LD, INC_MX__PCi,   LUOP__B,  SF_MX__ALU,                A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,     INC_MX__NONE,  LUOP__0,  SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,     INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZCV), A_MX__A, C_MX__C, NX_MX__END_D)
    },
    [0x7e] = { /* ROR Abs,X */
        CTRL(WR_MX__DPL,  R_MX__X,   AD_MX__DPH_LD, INC_MX__PCi,  LUOP__B,  SF_MX__ALU,               A_MX__A, C_MX__0,   NX_MX__NEXT),
        CTRL(WR_MX__DPH,  R_MX__DPH, AD_MX__DP,     INC_MX__NONE, LUOP__0,  SF_MX__ALU,               A_MX__A, C_MX__IC,  NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A,   AD_MX__DP,     INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__0,   AD_MX__PBA,    INC_MX__NONE, LUOP__B, (SF_MX__ALU | SF_MX__NZC), A_MX__0, C_MX__ROR, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T,   AD_MX__DP,     INC_MX__NONE, LUOP__0,  SF_MX__NONE,              A_MX__0, C_MX__0,   NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* RRA Abs,X */
    /* LDD # */
    [0x81] = { /* STA (Ind,X) */
        CTRL(WR_MX__DPL, R_MX__X, AD_MX__zDP, INC_MX__NONE, LUOP__B, SF_MX__ALU,  A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,   R_MX__A, AD_MX__zDP, INC_MX__DPLi, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__DPH, R_MX__A, AD_MX__zDP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__MEM, R_MX__A, AD_MX__DPt, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* LDD # */
    /* SAX (Ind,X) */
    [0x84] = { /* STY zpg */
        CTRL(WR_MX__MEM, R_MX__Y, AD_MX__zDP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    [0x85] = { /* STA zpg */
        CTRL(WR_MX__MEM, R_MX__A, AD_MX__zDP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    [0x86] = { /* STX zpg */
        CTRL(WR_MX__MEM, R_MX__X, AD_MX__zDP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* SAX zpg */
    [0x88] = { /* DEY */
        CTRL(WR_MX__Y, R_MX__Y, AD_MX__PC, INC_MX__IR_LD, LUOP__FF, (SF_MX__ALU | SF_MX__NZ), A_MX__A, C_MX__0, NX_MX__END)
    },
    /* LDD # */
    [0x8a] = { /* TXA */
        CTRL(WR_MX__A, R_MX__X, AD_MX__PC, INC_MX__IR_LD, LUOP__0, (SF_MX__ALU | SF_MX__NZ), A_MX__A, C_MX__0, NX_MX__END)
    },
    /* XAA # */
    [0x8c] = { /* STY Abs */
        CTRL(WR_MX__DPH, R_MX__A, AD_MX__PC, INC_MX__PCi,  LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__MEM, R_MX__Y, AD_MX__DP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    [0x8d] = { /* STA Abs */
        CTRL(WR_MX__DPH, R_MX__0, AD_MX__PC, INC_MX__PCi,  LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__MEM, R_MX__A, AD_MX__DP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    [0x8e] = { /* STX Abs */
        CTRL(WR_MX__DPH, R_MX__0, AD_MX__PC, INC_MX__PCi,  LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__MEM, R_MX__X, AD_MX__DP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* SAX Abs */
    [0x90] = BRANCH, /* BCC Rel */
    [0x91] = { /* STA (Ind),Y */
        CTRL(WR_MX__NONE, R_MX__A,   AD_MX__zDP, INC_MX__DPLi, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0,  NX_MX__NEXT),
        CTRL(WR_MX__DPL,  R_MX__Y,   AD_MX__zDP, INC_MX__NONE, LUOP__B, SF_MX__ALU,  A_MX__A, C_MX__0,  NX_MX__NEXT),
        CTRL(WR_MX__DPH,  R_MX__DPH, AD_MX__DP,  INC_MX__NONE, LUOP__0, SF_MX__ALU,  A_MX__A, C_MX__IC, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__A,   AD_MX__DP,  INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0,  NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* KIL */
    /* AHX (Ind),Y */
    [0x94] = { /* STY zpg,X */
        CTRL(WR_MX__DPL, R_MX__X, AD_MX__zDP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__MEM, R_MX__Y, AD_MX__zDP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    [0x95] = { /* STA zpg,X */
        CTRL(WR_MX__DPL, R_MX__X, AD_MX__zDP, INC_MX__NONE, LUOP__B, SF_MX__ALU,  A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__MEM, R_MX__A, AD_MX__zDP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    [0x96] = { /* STX zpg,Y */
        CTRL(WR_MX__DPL, R_MX__Y, AD_MX__zDP, INC_MX__NONE, LUOP__B, SF_MX__ALU,  A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__MEM, R_MX__X, AD_MX__zDP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* SAX zpg,Y */
    [0x98] = { /* TYA */
        CTRL(WR_MX__A, R_MX__Y, AD_MX__PC, INC_MX__IR_LD, LUOP__0, (SF_MX__ALU | SF_MX__NZ), A_MX__A, C_MX__0, NX_MX__END)
    },
    [0x99] = { /* STA Abs,Y */
        CTRL(WR_MX__DPL, R_MX__Y, AD_MX__PC, INC_MX__PCi,  LUOP__B, SF_MX__ALU,  A_MX__A, C_MX__0,  NX_MX__NEXT),
        CTRL(WR_MX__DPH, R_MX__A, AD_MX__DP, INC_MX__NONE, LUOP__B, SF_MX__ALU,  A_MX__0, C_MX__IC, NX_MX__NEXT),
        CTRL(WR_MX__MEM, R_MX__A, AD_MX__DP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0,  NX_MX__NEXT),
        FETCH_OPCODE
    },
    [0x9a] = { /* TXS */
        CTRL(WR_MX__SP, R_MX__X, AD_MX__PC, INC_MX__IR_LD, LUOP__0, SF_MX__ALU, A_MX__A, C_MX__0, NX_MX__END)
    },
    /* TAS Abs,Y */
    /* SHY Abs,X */
    [0x9d] = { /* STA Abs,X */
        CTRL(WR_MX__DPL, R_MX__X, AD_MX__PC, INC_MX__PCi,  LUOP__B, SF_MX__ALU,  A_MX__A, C_MX__0,  NX_MX__NEXT),
        CTRL(WR_MX__DPH, R_MX__A, AD_MX__DP, INC_MX__NONE, LUOP__B, SF_MX__ALU,  A_MX__0, C_MX__IC, NX_MX__NEXT),
        CTRL(WR_MX__MEM, R_MX__A, AD_MX__DP, INC_MX__NONE, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0,  NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* SHX Abs,Y */
    /* AHX Abs,Y */
    [0xa0] = { /* LDY # */
        CTRL(WR_MX__Y, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0xa1] = { /* LDA (Ind,X) */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE,  LUOP__B,  SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__A, AD_MX__zDP, INC_MX__DPLi,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DPt, INC_MX__NONE,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0xa2] = { /* LDX # */
        CTRL(WR_MX__X, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    /* LAX (Ind,X) */
    [0xa4] = { /* LDY zpg */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__Y,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0xa5] = { /* LDA zpg */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0xa6] = { /* LDX zpg */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__X,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    /* LAX zpg */
    [0xa8] = { /* TAY */
        CTRL(WR_MX__Y, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__0, (SF_MX__ALU | SF_MX__NZ), A_MX__A, C_MX__0, NX_MX__END)
    },
    [0xa9] = { /* LDA # */
        CTRL(WR_MX__A, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0xaa] = { /* TAX */
        CTRL(WR_MX__X, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__0, SF_MX__NONE, A_MX__0, C_MX__0, NX_MX__END)
    },
    /* LXA # */
    [0xac] = { /* LDY Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC, INC_MX__PCi,   LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP, INC_MX__NONE,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__Y,    R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0xad] = { /* LDA Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC, INC_MX__PCi,   LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP, INC_MX__NONE,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0xae] = { /* LDX Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC, INC_MX__PCi,   LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP, INC_MX__NONE,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    /* LAX Abs */
    [0xb0] = BRANCH, /* BCS Rel */
    [0xb1] = { /* LDA (Ind),Y */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__DPLi,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__DPL,  R_MX__Y, AD_MX__zDP, INC_MX__NONE,  LUOP__B,  SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,  INC_MX__NONE,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    /* KIL */
    /* LAX (Ind),Y */
    [0xb4] = { /* LDY zpg,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE,  LUOP__B,  SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__Y,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0xb5] = { /* LDA zpg,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE,  LUOP__B,  SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__0, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0xb6] = { /* LDX zpg,Y */
        CTRL(WR_MX__DPL,  R_MX__Y, AD_MX__zDP, INC_MX__NONE,  LUOP__B,  SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__X,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    /* LAX zpg,Y */
    [0xb8] = CLEAR_FLAG, /* CLV */
    [0xb9] = { /* LDA Abs,Y */
        CTRL(WR_MX__DPL,  R_MX__Y, AD_MX__DPH_LD, INC_MX__PCi,   LUOP__B,  SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,     INC_MX__NONE,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,     INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0xba] = { /* TSX */
        CTRL(WR_MX__X, R_MX__SP, AD_MX__PC, INC_MX__IR_LD, LUOP__0, (SF_MX__ALU | SF_MX__NZ), A_MX__A, C_MX__0, NX_MX__END)
    },
    /* LAS Abs,Y */
    [0xbc] = { /* LDY Abs,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__DPH_LD, INC_MX__PCi,   LUOP__B,  SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,     INC_MX__NONE,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__Y,    R_MX__A, AD_MX__PC,     INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0xbd] = { /* LDA Abs,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__DPH_LD, INC_MX__PCi,   LUOP__B,  SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,     INC_MX__NONE,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,     INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    [0xbe] = { /* LDX Abs,Y */
        CTRL(WR_MX__DPL,  R_MX__Y, AD_MX__DPH_LD, INC_MX__PCi,   LUOP__B,  SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,     INC_MX__NONE,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__X,    R_MX__A, AD_MX__PC,     INC_MX__IR_LD, LUOP__B, (SF_MX__ALU | SF_MX__NZ), A_MX__0, C_MX__0, NX_MX__END)
    },
    /* LAX Abs,Y */
    [0xc0] = { /* CPY # */
        CTRL(WR_MX__NONE, R_MX__Y, AD_MX__PC, INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZC), A_MX__A, C_MX__1, NX_MX__END)
    },
    [0xc1] = { /* CMP (Ind,X) */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE,  LUOP__B,   SF_MX__ALU,               A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__A, AD_MX__zDP, INC_MX__DPLi,  LUOP__0,   SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,   SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DPt, INC_MX__NONE,  LUOP__0,   SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZC), A_MX__A, C_MX__1, NX_MX__END)
    },
    /* LDD # */
    /* DCP (Ind,X) */
    [0xc4] = { /* CPY zpg */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,   SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__Y, AD_MX__PC,  INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZC), A_MX__A, C_MX__1, NX_MX__END)
    },
    [0xc5] = { /* CMP zpg */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,   SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZC), A_MX__A, C_MX__1, NX_MX__END)
    },
    [0xc6] = { /* DEC zpg */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE, LUOP__0,   SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__A, AD_MX__PBA, INC_MX__NONE, LUOP__FF, (SF_MX__ALU | SF_MX__NZ), A_MX__B, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T, AD_MX__zDP, INC_MX__NONE, LUOP__0,   SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* DCP zpg */
    [0xc8] = { /* INY */
        CTRL(WR_MX__Y, R_MX__Y, AD_MX__PC, INC_MX__IR_LD, LUOP__0, (SF_MX__ALU | SF_MX__NZ), A_MX__A, C_MX__1, NX_MX__END)
    },
    [0xc9] = { /* CMP # */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZC), A_MX__A, C_MX__1, NX_MX__END)
    },
    [0xca] = { /* DEX */
        CTRL(WR_MX__X, R_MX__X, AD_MX__PC, INC_MX__IR_LD, LUOP__FF, (SF_MX__ALU | SF_MX__NZ), A_MX__A, C_MX__0, NX_MX__END)
    },
    /* ASX # */
    [0xcc] = { /* CPY Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC, INC_MX__PCi,   LUOP__0,   SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP, INC_MX__NONE,  LUOP__0,   SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__Y, AD_MX__PC, INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZC), A_MX__A, C_MX__1, NX_MX__END)
    },
    [0xcd] = { /* CMP Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC, INC_MX__PCi,   LUOP__0,   SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP, INC_MX__NONE,  LUOP__0,   SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZC), A_MX__A, C_MX__1, NX_MX__END)
    },
    [0xce] = { /* DEC Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC,  INC_MX__PCi,  LUOP__0,   SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,  INC_MX__NONE, LUOP__0,   SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__A, AD_MX__PBA, INC_MX__NONE, LUOP__FF, (SF_MX__ALU | SF_MX__NZ), A_MX__B, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T, AD_MX__DP,  INC_MX__NONE, LUOP__0,   SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* DCP Abs */
    [0xd0] = BRANCH, /* BNE Rel */
    [0xd1] = { /* CMP (Ind),Y */
        CTRL(WR_MX__NONE, R_MX__A,  AD_MX__zDP,                  INC_MX__DPLi,  LUOP__0,   SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__DPL,  R_MX__Y, (AD_MX__DPH_LD | AD_MX__zDP), INC_MX__NONE,  LUOP__B,   SF_MX__ALU,               A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A,  AD_MX__DP,                   INC_MX__NONE,  LUOP__0,   SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A,  AD_MX__PC,                   INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZC), A_MX__A, C_MX__1, NX_MX__END)
    },
    /* KIL */
    /* DCP (Ind),Y */
    /* LDD zpg,X */
    [0xd5] = { /* CMP zpg,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE,  LUOP__B,   SF_MX__ALU,               A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,   SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZC), A_MX__A, C_MX__1, NX_MX__END)
    },
    [0xd6] = { /* DEC zpg,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE, LUOP__B,   SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE, LUOP__0,   SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__A, AD_MX__PBA, INC_MX__NONE, LUOP__FF, (SF_MX__ALU | SF_MX__NZ), A_MX__B, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T, AD_MX__zDP, INC_MX__NONE, LUOP__0,   SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* DCP zpg,X */
    [0xd8] = CLEAR_FLAG, /* CLD */
    [0xd9] = { /* CMP Abs,Y */
        CTRL(WR_MX__DPL,  R_MX__Y, AD_MX__DPH_LD, INC_MX__PCi,   LUOP__B,   SF_MX__ALU,               A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,     INC_MX__NONE,  LUOP__0,   SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__PC,     INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZC), A_MX__A, C_MX__1, NX_MX__END)
    },
    [0xda] = NOP,
    /* DCP Abs,Y */
    /* LDD Abs,X */
    [0xdd] = { /* CMP Abs,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__DPH_LD, INC_MX__PCi,   LUOP__B,   SF_MX__ALU,               A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,     INC_MX__NONE,  LUOP__0,   SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__PC,     INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZC), A_MX__A, C_MX__1, NX_MX__END)
    },
    [0xde] = { /* DEC Abs,X */
        CTRL(WR_MX__DPL,  R_MX__X,   AD_MX__DPH_LD, INC_MX__PCi,  LUOP__B,   SF_MX__ALU,              A_MX__A, C_MX__0,  NX_MX__NEXT),
        CTRL(WR_MX__DPH,  R_MX__DPH, AD_MX__DP,     INC_MX__NONE, LUOP__0,   SF_MX__ALU,              A_MX__A, C_MX__IC, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A,   AD_MX__DP,     INC_MX__NONE, LUOP__0,   SF_MX__NONE,             A_MX__0, C_MX__0,  NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__A,   AD_MX__PBA,    INC_MX__NONE, LUOP__FF, (SF_MX__ALU | SF_MX__NZ), A_MX__B, C_MX__0,  NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T,   AD_MX__DP,     INC_MX__NONE, LUOP__0,   SF_MX__NONE,             A_MX__0, C_MX__0,  NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* DCP Abs,X */
    [0xe0] = { /* CPX # */
        CTRL(WR_MX__NONE, R_MX__X, AD_MX__PC, INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZC), A_MX__A, C_MX__1, NX_MX__END)
    },
    [0xe1] = { /* SBC (Ind,X) */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE,  LUOP__B,   SF_MX__ALU,                A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__A, AD_MX__zDP, INC_MX__DPLi,  LUOP__0,   SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,   SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DPt, INC_MX__NONE,  LUOP__0,   SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZCV), A_MX__A, C_MX__C, NX_MX__END_D)
    },
    /* LDD # */
    /* ISC (Ind,X) */
    [0xe4] = { /* CPX zpg */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,   SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__X, AD_MX__PC,  INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZC), A_MX__A, C_MX__1, NX_MX__END)
    },
    [0xe5] = { /* SBC zpg */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,   SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZCV), A_MX__A, C_MX__C, NX_MX__END_D)
    },
    [0xe6] = { /* INC zpg */
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__A, AD_MX__PBA, INC_MX__NONE, LUOP__0, (SF_MX__ALU | SF_MX__NZ), A_MX__B, C_MX__1, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* ISC zpg */
    [0xe8] = { /* INX */
        CTRL(WR_MX__X, R_MX__X, AD_MX__PC, INC_MX__IR_LD, LUOP__0, (SF_MX__ALU | SF_MX__NZ), A_MX__A, C_MX__1, NX_MX__END)
    },
    [0xe9] = { /* SBC # */
        CTRL(WR_MX__A, R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZCV), A_MX__A, C_MX__C, NX_MX__END_D)
    },
    [0xea] = NOP,
    /* SBC # */
    [0xec] = { /* CPX Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC, INC_MX__PCi,   LUOP__0,   SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP, INC_MX__NONE,  LUOP__0,   SF_MX__NONE,              A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__X, AD_MX__PC, INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZC), A_MX__A, C_MX__1, NX_MX__END)
    },
    [0xed] = { /* SBC # */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC, INC_MX__PCi,   LUOP__0,   SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP, INC_MX__NONE,  LUOP__0,   SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC, INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZCV), A_MX__A, C_MX__C, NX_MX__END_D)
    },
    [0xee] = { /* INC Abs */
        CTRL(WR_MX__DPH,  R_MX__A, AD_MX__PC,  INC_MX__PCi,  LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,  INC_MX__NONE, LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__A, AD_MX__PBA, INC_MX__NONE, LUOP__0, (SF_MX__ALU | SF_MX__NZ), A_MX__B, C_MX__1, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T, AD_MX__DP,  INC_MX__NONE, LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* ISC Abs */
    [0xf0] = BRANCH, /* BEQ Rel */
    [0xf1] = { /* SBC (Ind),Y */
        CTRL(WR_MX__NONE, R_MX__A,  AD_MX__zDP,                  INC_MX__DPLi,  LUOP__0,   SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__DPL,  R_MX__Y, (AD_MX__DPH_LD | AD_MX__zDP), INC_MX__NONE,  LUOP__B,   SF_MX__ALU,                A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A,  AD_MX__DP,                   INC_MX__NONE,  LUOP__0,   SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A,  AD_MX__PC,                   INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZCV), A_MX__A, C_MX__C, NX_MX__END_D)
    },
    /* KIL */
    /* ISC (Ind),Y */
    /* LDD zpg,X */
    [0xf5] = { /* SBC zpg,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE,  LUOP__B,   SF_MX__ALU,                A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE,  LUOP__0,   SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,  INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZCV), A_MX__A, C_MX__C, NX_MX__END_D)
    },
    [0xf6] = { /* INC zpg,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__zDP, INC_MX__NONE, LUOP__B,  SF_MX__ALU,              A_MX__A, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__A, AD_MX__PBA, INC_MX__NONE, LUOP__0, (SF_MX__ALU | SF_MX__NZ), A_MX__B, C_MX__1, NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T, AD_MX__zDP, INC_MX__NONE, LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0, NX_MX__NEXT),
        FETCH_OPCODE
    },
    /* ISC zpg,X */
    [0xf8] = SET_FLAG, /* SED */
    [0xf9] = { /* SBC Abs,Y */
        CTRL(WR_MX__DPL,  R_MX__Y, AD_MX__DPH_LD, INC_MX__PCi,   LUOP__B,   SF_MX__ALU,                A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,     INC_MX__NONE,  LUOP__0,   SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,     INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZCV), A_MX__A, C_MX__C, NX_MX__END_D)
    },
    [0xfa] = NOP,
    /* ISC Abs,Y */
    /* LDD Abs,X */
    [0xfd] = { /* SBC Abs,X */
        CTRL(WR_MX__DPL,  R_MX__X, AD_MX__DPH_LD, INC_MX__PCi,   LUOP__B,   SF_MX__ALU,                A_MX__A, C_MX__0, NX_MX__INCDPH_C),
        CTRL(WR_MX__NONE, R_MX__A, AD_MX__DP,     INC_MX__NONE,  LUOP__0,   SF_MX__NONE,               A_MX__0, C_MX__0, NX_MX__NEXT),
        CTRL(WR_MX__A,    R_MX__A, AD_MX__PC,     INC_MX__IR_LD, LUOP__nB, (SF_MX__ALU | SF_MX__NZCV), A_MX__A, C_MX__C, NX_MX__END_D)
    },
    [0xfe] = { /* INC Abs,X */
        CTRL(WR_MX__DPL,  R_MX__X,   AD_MX__DPH_LD, INC_MX__PCi,  LUOP__B,  SF_MX__ALU,              A_MX__A, C_MX__0,  NX_MX__NEXT),
        CTRL(WR_MX__DPH,  R_MX__DPH, AD_MX__DP,     INC_MX__NONE, LUOP__0,  SF_MX__ALU,              A_MX__A, C_MX__IC, NX_MX__NEXT),
        CTRL(WR_MX__NONE, R_MX__A,   AD_MX__DP,     INC_MX__NONE, LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0,  NX_MX__NEXT),
        CTRL(WR_MX__T,    R_MX__A,   AD_MX__PBA,    INC_MX__NONE, LUOP__0, (SF_MX__ALU | SF_MX__NZ), A_MX__B, C_MX__1,  NX_MX__NEXT),
        CTRL(WR_MX__MEM,  R_MX__T,   AD_MX__DP,     INC_MX__NONE, LUOP__0,  SF_MX__NONE,             A_MX__0, C_MX__0,  NX_MX__NEXT),
        FETCH_OPCODE
    }
    /* ISC Abs,X */
};