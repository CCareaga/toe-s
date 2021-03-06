#ifndef VGA_H
#define VGA_H

#include "sys.h"
#include <stddef.h>

void init_vga();
void vga_scroll();

void vga_writeln(const char* str);
void vga_write(const char* str);

void vga_clear();

void update_csr();

void set_color(uint8_t fore, uint8_t back);

#endif
