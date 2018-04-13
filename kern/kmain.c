#include "klib.h"
#include "multiboot.h"
#include "x86.h"
#include "system.h"
#include "pmm.h"
#include "vmm.h"
#include "kmalloc.h"

extern void *end;

int kmain(multiboot_t *mboot, uint32_t mboot_mag, uintptr_t esp) { 
   
    // initialize vga text-mode, cyan on black bg
    vga_init();
    vga_set_color(CYAN, BLACK);
    
    // set up the global desc. table and int desc. table
    gdt_init();
    idt_init();
    
    kprintf(" \1 TOE-S \1 \n");
    kprintf("end: 0x%x \n", &end);

    // initialize memory management
    pmm_init(mboot);
    vmm_init();

    kprintf("paging and heap initialized \n\n");
    
    uint32_t a, b, c, d, e, f;
    kprintf("a: 0x%x \n", a = kmalloc(8));
    kprintf("b: 0x%x \n", b = kmalloc(8));
    kprintf("c: 0x%x \n", c = kmalloc(8));
    kprintf("d: 0x%x \n", d = kmalloc(8));

    kprintf("phys of a: 0x%x \n", get_physical(a));

    kfree(a);
    kfree(c);

    kprintf("e: 0x%x \n", e = kmalloc(24));

    kfree(b);
    kprintf("f: 0x%x \n", f = kmalloc(24));

    
    // int *p = 0xc0000000;
    // *p = 1;

    // for (;;) {}

    return 0;
}

