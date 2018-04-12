#include "vga.h"
#include "system.h"

#define HEIGHT 25
#define WIDTH 80
#define BUFFER_LEN 2000

static int cursor;
static uint16_t *vga_mem = (uint16_t *) 0xB8000;
static uint16_t attribute;

// set the color to white on black, clear the screen
void vga_init() {
    vga_set_color(WHITE, BLACK);
    vga_clear();
}

// scroll down a line, this is called when we reach the end of video memory
static void vga_scroll() {

    int i;

    for (i = 0; i < WIDTH * (HEIGHT - 1); ++i)
        vga_mem[i] = vga_mem[i + WIDTH];

    for(i = 0; i < WIDTH; ++i)
        vga_mem[WIDTH * (HEIGHT - 1) + i] = attribute;

    cursor = WIDTH * (HEIGHT - 1);
}

// move the cursor to the position saved in the cursor field
static void vga_update_csr() {
    outport(0x3D4, 14);
    outport(0x3D5, cursor >> 8);
    outport(0x3D4, 15);
    outport(0x3D5, cursor);
}

void vga_write_char(char c) {
    if (cursor == BUFFER_LEN) {
        vga_scroll();
    }
    
    switch (c) {
        case '\n':
            cursor = (cursor + 80) - (cursor % 80);
            break;

        case '\b':
            cursor--;
            vga_mem[cursor] = (uint16_t) (' ' | attribute);
            break;

        default:
            vga_mem[cursor] = (uint16_t) (c | attribute);
            cursor++;
            break;

    }

    vga_update_csr();
}

// call vga_writeln, but then write the newline character
void vga_writeln(const char *str) {
    if (str) 
        vga_write(str);

    vga_write("\n");
}

// write a string to the vga text mode memory, updates the cursor
void vga_write(const char *str) {

    int i = 0;

    while (str[i]) 
        vga_write_char(str[i++]);
}

// clear the entire vga text mode memory
void vga_clear() {

    int i;
    for (i = 0; i < (WIDTH * HEIGHT); i++) 
        vga_write_char(' ');

    cursor = 0;
    vga_update_csr();
}


// top 4 bytes are the background, bottom 4 bytes are the foreground color
void vga_set_color(uint8_t fore, uint8_t back) {
    attribute = ((back << 4) | (fore & 0x0F)) << 8;
}
