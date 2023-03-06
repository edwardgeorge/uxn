#include <stdio.h>
#include <stdlib.h>

#include "uxn.h"
#include "devices/system.h"
#include "devices/file.h"
#include "devices/datetime.h"

/*
Copyright (c) 2021-2023 Devine Lu Linvega, Andrew Alderwick

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE.
*/

Uint16 deo_mask[] = {0x6a08, 0x0300, 0xc028, 0x8000, 0x8000, 0x8000, 0x8000, 0x0000, 0x0000, 0x0000, 0xa260, 0xa260, 0x0000, 0x0000, 0x0000, 0x0000};
Uint16 dei_mask[] = {0x0000, 0x0000, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07fd, 0x0000, 0x0000, 0x0000};

static int
emu_error(char *msg, const char *err)
{
	fprintf(stderr, "Error %s: %s\n", msg, err);
	return 1;
}

static int
console_input(Uxn *u, char c)
{
	Uint8 *d = &u->dev[0x10];
	d[0x02] = c;
	return uxn_eval(u, PEEK16(d));
}

static void
console_deo(Uint8 *d, Uint8 port)
{
	switch(port) {
	case 0x8:
		fputc(d[port], stdout);
		fflush(stdout);
		return;
	case 0x9:
		fputc(d[port], stderr);
		fflush(stderr);
		return;
	}
}

Uint8
uxn_dei(Uxn *u, Uint8 addr)
{
	switch(addr & 0xf0) {
	case 0xc0: return datetime_dei(u, addr);
	}
	return u->dev[addr];
}

void
uxn_deo(Uxn *u, Uint8 addr)
{
	Uint8 p = addr & 0x0f, d = addr & 0xf0;
	switch(d) {
	case 0x00: system_deo(u, &u->dev[d], p); break;
	case 0x10: console_deo(&u->dev[d], p); break;
	case 0xa0: file_deo(0, u->ram, &u->dev[d], p); break;
	case 0xb0: file_deo(1, u->ram, &u->dev[d], p); break;
	}
}

int
main(int argc, char **argv)
{
	Uxn u;
	int i;
	if(argc < 2)
		return emu_error("Usage", "uxncli game.rom args");
	if(!uxn_boot(&u, (Uint8 *)calloc(0x10000 * RAM_PAGES, sizeof(Uint8))))
		return emu_error("Boot", "Failed");
	if(!system_load(&u, argv[1]))
		return emu_error("Load", "Failed");
	if(!uxn_eval(&u, PAGE_PROGRAM))
		return u.dev[0x0f] & 0x7f;
	for(i = 2; i < argc; i++) {
		char *p = argv[i];
		while(*p) console_input(&u, *p++);
		console_input(&u, '\n');
	}
	while(!u.dev[0x0f]) {
		int c = fgetc(stdin);
		if(c != EOF)
			console_input(&u, (Uint8)c);
	}
	return u.dev[0x0f] & 0x7f;
}
