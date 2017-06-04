#include "include/vmm.h"

uint32_t page_dir[1024] __attribute__((aligned(4096)));
uint32_t page_tables[1024][1024] __attribute__((aligned(4096)));

uint32_t placement_addr = 0x0;

// this is a simple allocator to dynamically move a pointer to the end of reserved
// memory. this is useful becuase i can dynamically allocate a stack of frames
// and then map it as part of my kernel

uint32_t kmalloc(uint32_t size) {
    uint32_t ret = placement_addr;
    placement_addr += size;
    return ret;
}

uint32_t kmalloc_page() {
    return kmalloc(0x1000);
}

void page_round() {
    placement_addr &= 0xFFFFF000;
    placement_addr += 0x1000;
}

// right now all this does is identity map from 0x0 to the end of our kernel
// I use two huge global variables for the page directory and the page tables
// so the kernel goes from 0x10000 to around 0x500000, therefore we use two 
// tables.

// TODO: I may want to create definitions for setting bits on the PDE and PTE's
// or maybe create structs for the page dirs and tables...

void init_paging() {
    size_t i, j;
    uint32_t addr = 0x0;
    uint32_t frame;
    uint32_t index;

    for (i = 0; i < 1024; i++) {
        page_dir[i] = 0x2; // clear the pd, all tables writeable and not present
    }

    // here we iterate from the start of memory up to the end of our kernel
    // NOTE: I don't use alloc_frame for this because all of the frames I am mapping
    // here aren't on my frame stack (pmm.c)
    while(addr <= placement_addr) {
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
        // we have to map memory from 0x0 to the end of the kernel
        // so we have two page tables to put in our pd we or the address 
        // with 3 to mark these entries present
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
    // halt so that we dont trigger endless page faults.
    asm volatile ("hlt");
}

// Next I need to implement some malloc/free type functions... a.k.a I need to set up 
// a heap for dynamic allocation. Probably need to quite a bit more reading for that
