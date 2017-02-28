#include "vga.h"
#include "types.h"
#include "sys.h"

#define HEIGHT 25
#define WIDTH 80
#define BUFFER_LEN 2000

static int cursor;
static u16int* vga_mem;
static u16int attribute;

void init_vga() {
    vga_mem = (u16int*) 0xB8000;
    cursor = 0;
	set_color(15, 0);
	vga_clear();
}

void vga_scroll() {
	int i;

	for (i = 0; i < WIDTH * (HEIGHT - 1); ++i)
		vga_mem[i] = vga_mem[i + WIDTH];

	for(i = 0; i < WIDTH; ++i)
		vga_mem[WIDTH * (HEIGHT - 1) + i] = attribute;

	cursor = WIDTH * (HEIGHT - 1);

}

void vga_write(const char* str) {
	
	int i = 0;

	while (str[i]) {
		
		if (cursor == BUFFER_LEN) {
			vga_scroll();
		}

		if (str[i] == '\n') {
			cursor = (cursor + 80) - (cursor % 80);
		}
		else {
			vga_mem[cursor] = (u16int) (str[i] | attribute);
			cursor++;
		}
		update_csr();
		i++;
	}
}

void vga_clear() {
	int i;
	for (i = 0; i < (WIDTH * HEIGHT); i++) {
		vga_mem[i] = (u16int) (attribute | ' ');
	}
	cursor = 0;
	update_csr();
}

void update_csr()
{
    outport(0x3D4, 14);
    outport(0x3D5, cursor >> 8);
    outport(0x3D4, 15);
    outport(0x3D5, cursor);
}

void set_color(u8int fore, u8int back)
{
    /* Top 4 bytes are the background, bottom 4 bytes
    *  are the foreground color */
    attribute = ((back << 4) | (fore & 0x0F)) << 8;
}