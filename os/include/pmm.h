#ifndef PMM_H
#define PMM_H

#include "multiboot.h"
#include "sys.h"
#include "isr.h"
#include <stdint.h>

typedef struct page { 
    uint32_t present    : 1;   // Page present in memory 
    uint32_t rw         : 1;   // Read-only if clear, readwrite if set 
    uint32_t user       : 1;   // Supervisor level only if clear 
    uint32_t accessed   : 1;   // Has the page been accessed since last refresh? 
    uint32_t dirty      : 1;   // Has the page been written to since last refresh? 
    uint32_t unused     : 7;   // Amalgamation of unused and reserved bits 
    uint32_t frame      : 20;  // Frame address (shifted right 12 bits) 
} page_t; 

typedef struct page_table { 
    page_t pages[1024]; 
} page_table_t; 

typedef struct page_directory { 
    page_table_t *tables[1024]; // array of table pointers 
} page_directory_t;

void init_pmm(multiboot_info_t *mbi, uint32_t kern_end);

void push_frame(uint32_t addr);

uint32_t pop_frame();


void allocate_page(page_t *page, uint32_t kern, uint32_t writeable);

void free_page(page_t *page);

void init_paging();

void page_fault(registers *regs);
#endif
