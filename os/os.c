#include "vga.h"
#include "gdt.h"
#include "sys.h"

void main() {

	init_gdt();
	init_vga();

	set_color(0x2, 0);
	vga_write("kernel loaded!\n");
	set_color(0xb, 0);
	vga_write("gdt installed!");

	//print_hex(0x9a);

	bochs_break();
	
	for(;;){}
	asm("hlt");
}






