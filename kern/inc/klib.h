#ifndef _KLIB_H
#define _KLIB_H 

#include "types.h"
#include "vga.h"
#include "stdarg.h"

void kprintf(char *fmt, ...);
void *memset(void *bufptr, uint8_t value, uint32_t size);
void memcpy(void *srcptr, void *dstptr, uint32_t size);
char *itoa(uint32_t val, uint32_t base);

#endif
