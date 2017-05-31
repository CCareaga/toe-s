#include "include/vga.h"
#include "include/gdt.h"
#include "include/idt.h"
#include "include/timer.h"
#include "include/keyboard.h"
#include "include/sys.h"
#include "include/multiboot.h"
#include "include/pmm.h"

extern uint32_t end;
extern uint32_t start;

int kmain(multiboot_info_t *mbi) {
	
	init_vga();
	init_gdt();
	
	set_color(0x2, 0);
	vga_write("kernel loaded!\n");

	set_color(0xb, 0);
	vga_write("GDT installed!\n");

	init_idt();
	set_color(0x4, 0);
	vga_write("IDT installed!\n");

	set_color(0xd, 0);
	//init_timer(1);
	init_keyboard();

	
	init_pmm(mbi, (uint32_t) &end);
    init_paging();
	for(;;){
		//asm volatile ("hlt");
	}
}

