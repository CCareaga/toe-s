#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "vga.h"
#include "isr.h"

typedef struct key_struct
{
	uint8_t capslock : 1;
	uint8_t numlock : 1;
	uint8_t scrolllock : 1;

	uint8_t shift : 1;
	uint8_t alt : 1;
	uint8_t ctrl : 1;
} mod_struct;

void init_keyboard();

#endif