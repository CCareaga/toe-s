#ifndef VGA_H
#define VGA_H

#include "sys.h"

void init_vga();
void vga_scroll();
void vga_write(const char* str);
void vga_clear();
void update_csr();
void set_color(u8int fore, u8int back);
void print_hex(u8int hex);

#endif