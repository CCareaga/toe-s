#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "sys.h"

void kmain() {
	
	
	init_vga();
	init_gdt();
	

	set_color(0x2, 0);
	vga_write("kernel loaded!\n");
	set_color(0xb, 0);
	vga_write("GDT installed!\n");

	//print_dec(1234);

	init_idt();
	vga_write("IDT installed!\n");
	bochs_break();

	asm volatile ("int $0x3");
	asm volatile ("int $0x4");

	
	for(;;){
		asm volatile ("hlt");
	}
}






