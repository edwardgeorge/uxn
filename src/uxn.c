#include "uxn.h"

/*
Copyright (u) 2022-2024 Devine Lu Linvega, Andrew Alderwick, Andrew Richards

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE.
*/

#define OPC(opc, init, body) {\
	case 0x00|opc: {enum{_2=0,_r=0}; init; body; break;}\
	case 0x20|opc: {enum{_2=1,_r=0}; init; body; break;}\
	case 0x40|opc: {enum{_2=0,_r=1}; init; body; break;}\
	case 0x60|opc: {enum{_2=1,_r=1}; init; body; break;}\
	case 0x80|opc: {enum{_2=0,_r=0}; k = uxn.wst.ptr; init; uxn.wst.ptr = k; body; break;}\
	case 0xa0|opc: {enum{_2=1,_r=0}; k = uxn.wst.ptr; init; uxn.wst.ptr = k; body; break;}\
	case 0xc0|opc: {enum{_2=0,_r=1}; k = uxn.rst.ptr; init; uxn.rst.ptr = k; body; break;}\
	case 0xe0|opc: {enum{_2=1,_r=1}; k = uxn.rst.ptr; init; uxn.rst.ptr = k; body; break;}\
}

/* Microcode */

#define INC(s) uxn.s.dat[uxn.s.ptr++]
#define DEC(s) uxn.s.dat[--uxn.s.ptr]
#define JMI { pc += uxn.ram[pc++] << 8 | uxn.ram[pc++]; }
#define JMP(x) { if(_2) pc = (x); else pc += (Sint8)(x); }
#define POx(o) { if(_2) { PO2(o) } else PO1(o) }
#define PO2(o) { if(_r) o = DEC(rst) | (DEC(rst) << 8); else o = DEC(wst) | (DEC(wst) << 8); }
#define PO1(o) { if(_r) o = DEC(rst); else o = DEC(wst); }
#define PUx(y) { if(_2) { PU2(y) } else PU1(y) }
#define PU2(y) { tt = (y); PU1(tt >> 8) PU1(tt) }
#define PU1(y) { if(_r) INC(rst) = y; else INC(wst) = y; }
#define PFx(y) { if(_2) { PF2(y) } else PF1(y) }
#define PF2(y) { tt = (y); PF1(tt >> 8) PF1(tt) }
#define PF1(y) { if(_r) INC(wst) = y; else INC(rst) = y; }
#define DEI(p, o) { if(_2) { o = (emu_dei(p) << 8) | emu_dei(p + 1); } else o = emu_dei(p); }
#define DEO(p, y) { if(_2) { emu_deo(p, y >> 8), emu_deo(p + 1, y); } else emu_deo(p, y); }
#define PEK(o, x, r) { if(_2) { r = (x); o = uxn.ram[r++] << 8 | uxn.ram[r]; } else o = uxn.ram[(x)]; }
#define POK(x, y, r) { if(_2) { r = (x); uxn.ram[r++] = y >> 8, uxn.ram[r] = y; } else uxn.ram[(x)] = (y); }

int
uxn_eval(Uint16 pc)
{
	int a,b,c,k;
	Uint8 t;
	Uint16 tt;
	if(!pc || uxn.dev[0x0f]) return 0;
	for(;;) {
		switch(uxn.ram[pc++]) {
		/* BRK */ case 0x00: return 1;
		/* JCI */ case 0x20: if(DEC(wst)) { JMI break; } pc += 2; break;
		/* JMI */ case 0x40: JMI break;
		/* JSI */ case 0x60: tt = pc + 2; INC(rst) = tt >> 8; INC(rst) = tt; JMI break;
		/* LI2 */ case 0xa0: INC(wst) = uxn.ram[pc++];
		/* LIT */ case 0x80: INC(wst) = uxn.ram[pc++]; break;
		/* L2r */ case 0xe0: INC(rst) = uxn.ram[pc++];
		/* LIr */ case 0xc0: INC(rst) = uxn.ram[pc++]; break;
		/* INC */ OPC(0x01, POx(a), PUx(a + 1))
		/* POP */ OPC(0x02, POx(a), 0)
		/* NIP */ OPC(0x03, POx(a) POx(b), PUx(a))
		/* SWP */ OPC(0x04, POx(a) POx(b), PUx(a) PUx(b))
		/* ROT */ OPC(0x05, POx(a) POx(b) POx(c), PUx(b) PUx(a) PUx(c))
		/* DUP */ OPC(0x06, POx(a), PUx(a) PUx(a))
		/* OVR */ OPC(0x07, POx(a) POx(b), PUx(b) PUx(a) PUx(b))
		/* EQU */ OPC(0x08, POx(a) POx(b), PU1(b == a))
		/* NEQ */ OPC(0x09, POx(a) POx(b), PU1(b != a))
		/* GTH */ OPC(0x0a, POx(a) POx(b), PU1(b > a))
		/* LTH */ OPC(0x0b, POx(a) POx(b), PU1(b < a))
		/* JMP */ OPC(0x0c, POx(a), JMP(a))
		/* JCN */ OPC(0x0d, POx(a) PO1(b), if(b) JMP(a))
		/* JSR */ OPC(0x0e, POx(a), PF2(pc) JMP(a))
		/* STH */ OPC(0x0f, POx(a), PFx(a))
		/* LDZ */ OPC(0x10, PO1(a), PEK(b, a, t) PUx(b))
		/* STZ */ OPC(0x11, PO1(a) POx(b), POK(a, b, t))
		/* LDR */ OPC(0x12, PO1(a), PEK(b, pc + (Sint8)a, tt) PUx(b))
		/* STR */ OPC(0x13, PO1(a) POx(b), POK(pc + (Sint8)a, b, tt))
		/* LDA */ OPC(0x14, PO2(a), PEK(b, a, tt) PUx(b))
		/* STA */ OPC(0x15, PO2(a) POx(b), POK(a, b, tt))
		/* DEI */ OPC(0x16, PO1(a), DEI(a, b) PUx(b))
		/* DEO */ OPC(0x17, PO1(a) POx(b), DEO(a, b))
		/* ADD */ OPC(0x18, POx(a) POx(b), PUx(b + a))
		/* SUB */ OPC(0x19, POx(a) POx(b), PUx(b - a))
		/* MUL */ OPC(0x1a, POx(a) POx(b), PUx(b * a))
		/* DIV */ OPC(0x1b, POx(a) POx(b), PUx(a ? b / a : 0))
		/* AND */ OPC(0x1c, POx(a) POx(b), PUx(b & a))
		/* ORA */ OPC(0x1d, POx(a) POx(b), PUx(b | a))
		/* EOR */ OPC(0x1e, POx(a) POx(b), PUx(b ^ a))
		/* SFT */ OPC(0x1f, PO1(a) POx(b), PUx(b >> (a & 0xf) << (a >> 4)))
		}
	}
}
