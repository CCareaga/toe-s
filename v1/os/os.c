#include "include/vga.h"
#include "include/gdt.h"
#include "include/idt.h"
#include "include/timer.h"
#include "include/keyboard.h"
#include "include/sys.h"
#include "include/multiboot.h"
#include "include/pmm.h"
#include "include/vmm.h"

extern uint32_t end;
extern uint32_t start;

int kmain(multiboot_info_t *mbi) {
	
	init_vga();
	init_gdt();
	
	set_color(0x2, 0);
	vga_writeln("kernel loaded!");

	set_color(0xb, 0);
	vga_writeln("GDT installed!");

	init_idt();
	set_color(0x4, 0);
	vga_writeln("IDT installed!");
    vga_writeln(NULL);

	set_color(0xd, 0);
	//init_timer(1);
	init_keyboard();

	init_pmm(mbi, (uint32_t) &end);
    init_paging();    
   
    vga_write("end of kernel: ");
    vga_writeln(itoa(&end, 16));

	set_color(0x8, 0);

    vga_writeln("Paging enabled!");
    
    vga_writeln(NULL);
	set_color(0x9, 0);

    char *a = "this char array was allocated at compile time... lame";
    vga_writeln(a);
    vga_write("address: ");
    vga_writeln(itoa(a, 16));

    vga_writeln(NULL);
    
    char *b, *c;
    size_t i;

    b = (char *) kmalloc(sizeof(char) * 256);
    c = "this char array was dynamically allocated at runtime!";
    for (i = 0; i < 54; i++) {
        b[i] = c[i];
    }
    vga_writeln(b);
    vga_write("address: ");
    vga_writeln(itoa(b, 16));
    kfree(b);
    
   
    for(;;){
		//asm volatile ("hlt");
	}
}

