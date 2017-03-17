#ifndef VGA_H
#define VGA_H

#include "sys.h"
#include <stddef.h>

void init_vga();
void vga_scroll();
void vga_write(const char* str);
void vga_clear();

void update_csr();

void set_color(uint8_t fore, uint8_t back);

void print_hex(uint32_t hex);
void print_dec(uint32_t dec);
void mem_print(uint32_t *add, uint32_t amount);

#endif