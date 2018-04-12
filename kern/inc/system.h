#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "types.h"

uint8_t inport(uint16_t _port);
void outport(uint16_t _port, uint8_t _data);
void interrupt(uint8_t num);
void bochs_break();

void *memset(void *bufptr, uint8_t value, uint32_t size);
char *itoa(uint32_t value, uint32_t base);

#endif
