#include "include/vmm.h"

uint32_t page_dir[1024] __attribute__((aligned(4096)));
uint32_t page_tables[1024][1024] __attribute__((aligned(4096)));

void init_paging(uint32_t end) {
    size_t i, j;
    uint32_t addr = 0x0;
    uint32_t frame;
    uint32_t index;

    for (i = 0; i < 1024; i++) {
        page_dir[i] = 0x00000002; // clear the pd, writeable and not present
    }

    // here we iterate from the start of memory up to the end of our kernel
    while(addr <= end) {
        // we want to identity map the kernels memory so we iterate by 4 KiB
        // to get the address of the page we are looking at
        // we create indices to get our PTE from our page table array
        frame = addr >> 12;
        index = frame / 1024;
        // we index the array and set the PTE to our page address 
        page_tables[index][frame%1024] = addr | 3;

        addr += 0x1000; // move to the start of the next page!
    }

    for (i = 0; i <= index; i++) {
        // now we set the corresponding element in our page directory to the 
        // address of the page table we are using at the moment
        page_dir[i] = ((uint32_t) page_tables[i]) | 3;
    }
    
    add_handler(14, &page_fault);
    switch_page_directory((uint32_t) page_dir);    
}

void switch_page_directory(uint32_t *dir) {
    asm volatile("mov %0, %%cr3":: "r"(dir));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

void page_fault(registers *regs) {
    uint32_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

    int present   = !(regs->err_code & 0x1); 
    int rw = regs->err_code & 0x2;
    int us = regs->err_code & 0x4; 
    int reserved = regs->err_code & 0x8;
    int id = regs->err_code & 0x10;

    vga_write("Page fault! ( ");
    if (present) vga_write("present ");
    if (rw) vga_write("read-only ");
    if (us) vga_write("user-mode ");
    if (reserved) vga_write("reserved ");
    vga_write(") at 0x");
    vga_write(itoa(faulting_address, 16));
    vga_write("\n");
    return;
}
