#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "keyboard.h"
#include "sys.h"

void kmain() {
	
	init_vga();
	init_gdt();
	
	set_color(0x2, 0);
	vga_write("kernel loaded!\n");

	set_color(0xb, 0);
	vga_write("GDT installed!\n");

	init_idt();
	vga_write("IDT installed!\n");

	set_color(0xd, 0);
	//init_timer(1);
	init_keyboard();

	for(;;){
		//asm volatile ("hlt");
	}
}

