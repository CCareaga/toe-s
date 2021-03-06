#ifndef VMM_H
#define VMM_H

#include "sys.h"
#include "isr.h"
#include "kheap.h"
#include <stdint.h>

extern uint32_t placement_addr;
extern heap_t *kheap;

typedef struct paging {
    uint32_t dir[1024];
    uint32_t tables[1024][1024];
} paging_t;

void kfree(void *p);
uint32_t kmalloc(uint32_t size);
uint32_t kmalloc_page();
void page_round();

void init_paging();

uint32_t *get_page(uint32_t address, paging_t *paging);
void alloc_frame(uint32_t *page, uint8_t kern, uint8_t write);
void free_frame(uint32_t *page);

void switch_page_directory(uint32_t *dir);

void page_fault(registers *regs);

#endif
