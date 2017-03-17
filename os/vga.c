#include "vga.h"
#include "sys.h"

#define HEIGHT 25
#define WIDTH 80
#define BUFFER_LEN 2000

static int cursor;
static uint16_t* vga_mem;
static uint16_t attribute;

static char hex_chars[16] = {"0123456789abcdef"};

void init_vga() {

    vga_mem = (uint16_t*) 0xB8000;
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
			vga_mem[cursor] = (uint16_t) (str[i] | attribute);
			cursor++;
		}
		update_csr();
		i++;
	}
}

void vga_clear() {

	int i;
	for (i = 0; i < (WIDTH * HEIGHT); i++) {
		vga_mem[i] = (uint16_t) (attribute | ' ');
	}
	cursor = 0;
	update_csr();
}

void update_csr() {

    outport(0x3D4, 14);
    outport(0x3D5, cursor >> 8);
    outport(0x3D4, 15);
    outport(0x3D5, cursor);
}

void set_color(uint8_t fore, uint8_t back) {
    /* Top 4 bytes are the background, bottom 4 bytes
    *  are the foreground color */
    attribute = ((back << 4) | (fore & 0x0F)) << 8;
}

void print_hex(uint32_t hex) {
	uint8_t byte;
	char hex_str[8];
	for (int i = 0; i < 8; i++) {
		byte = ((hex << i*4) & 0xF0000000) >> 28;
		hex_str[i] = hex_chars[byte];
	}

	vga_write(&hex_str[0]);
}

void print_dec(uint32_t dec) {
	while(dec) {
		char digit = (dec % 10) + '0';
		vga_write(&digit);
		dec /= 10;
	}
}

void mem_print(uint32_t *add, uint32_t amount) {
	uint32_t i = 0;
	while(i < amount) {
		print_hex(add[i]);
		vga_write(" ");
		i++;
	}

}