#include "include/vmm.h"

extern heap_t *kheap;

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

// I may not even need this function but not sure yet
uint32_t kmalloc_page() {
    return kmalloc(0x1000);
}

// this functions will round to the nearest page boundary
// probably not very space efficient but what can ya do :/
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
    uint32_t *page;
    uint32_t index;

    heap_t *heap = kmalloc(sizeof(heap_t)); 

    page_round();

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
        page = get_page(addr); 
        *page = addr | 3;

        addr += 0x1000; // move to the start of the next page!
    }

    index = (addr >> 12) / 1024; // calculate how many tables we used

    for (i = 0; i <= index; i++) {
        // now we set the corresponding element in our page directory to the 
        // address of the page table we are using at the moment
        // we have to map memory from 0x0 to the end of the kernel
        // so we have two page tables to put in our pd we or the address 
        // with 3 to mark these entries present
        page_dir[i] = ((uint32_t) page_tables[i]) | 3;
    }
   
    for (i = KHEAP_START; i < KHEAP_START + KHEAP_INIT_SIZE; i += 0x1000) {
        page = get_page(i);
        alloc_frame(page, 0, 0);
    }
    
    add_handler(14, &page_fault);
    switch_page_directory((uint32_t) page_dir);    
    init_heap(KHEAP_START, KHEAP_START + KHEAP_INIT_SIZE, KHEAP_MAX, heap);
}

// this function will take the given address and use it to index the correct page table
// it will then return a pointer to that PTE
uint32_t *get_page(uint32_t address) {
    uint32_t frame = address >> 12;
    uint32_t index = frame / 1024;

    if (!(page_dir[index] & 1)) {
        page_dir[index] = ((uint32_t) page_tables[index]) | 3;
    }

    return &page_tables[index][frame % 1024]; 
}

// this function will take the given page pointer (PTE pointer), it will grab a frame for it
// and then set the correct bits to map the address
void alloc_frame(uint32_t *page, uint8_t kern, uint8_t write) {
    uint32_t address = pop_frame();
    *page |= 1;                  // set present
    *page |= (write) ? 2 : 0;    // set writeable
    *page |= (kern) ? 4 : 0;     // set priv level
    *page |= address;            // set the frame address
}

// this is the opposite of allocate frame, it simply pushes the frame address back onto the stack
// and clears the page table entry
void free_frame(uint32_t *page) {
   uint32_t frame = *page & 0xFFFFF000; // get just the frame addr
   push_frame(frame); // push it back on to the stack
   *page = 0x0; // clear the info for this page in the PT
}

// function to switch to the provided directory
void switch_page_directory(uint32_t *dir) {
    asm volatile("mov %0, %%cr3":: "r"(dir));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

// page fault handler, currently it just prints some info and hangs
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
