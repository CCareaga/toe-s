#include "klib.h"
#include "multiboot.h"
#include "x86.h"
#include "system.h"
#include "pmm.h"
#include "vmm.h"
#include "kmalloc.h"
#include "timer.h"

extern void *end;
extern void higher_half(int offset);

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

    kfree(a);
    kfree(c);

    kprintf("e: 0x%x \n", e = kmalloc(24));

    kfree(b);
    kprintf("f: 0x%x \n", f = kmalloc(24));

#if HIGHER_HALF
    int local = 0xDEADBEEF;
    int *virt = (char *) &local + 0xc0000000;

    kprintf("value at 0x%x = 0x%x \n", virt, *virt);

    higher_half(HIGHER_HALF_START);
#endif

    timer_init();

    
    // int *p = 0xc0000000;
    // *p = 1;

    for (;;) {}

    return 0;
}

