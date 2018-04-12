#ifndef _VGA_H
#define _VGA_H

#include "types.h"

#define BLACK   0
#define BLUE    1
#define GREEN   2
#define CYAN    3
#define RED     4
#define MAGENTA 5
#define BROWN   6
#define GRAY    7

#define DGRAY    8
#define LBLUE    9
#define LGREEN   0xA
#define LCYAN    0xB
#define LRED     0xC
#define LMAGENTA 0xD
#define YELLOW   0xE
#define WHITE    0xF

    
void vga_init();

void vga_writeln(const char* str);
void vga_write(const char* str);
void vga_write_char(char c);

void vga_clear();
void vga_set_color(uint8_t fore, uint8_t back);

#endif 


