#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <stdint.h>
#include <stddef.h>

extern uint8_t inport(uint16_t _port);
extern void outport(uint16_t _port, uint8_t _data);
extern void bochs_break();

void* memset(void* bufptr, uint8_t value, uint32_t size);
char* itoa(uint32_t value, uint32_t base);

char convert_hex(uint8_t);

#endif