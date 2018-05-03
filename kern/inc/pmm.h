#ifndef _PMM_H
#define _PMM_H

#include "multiboot.h"

#define KERN_OFFSET 0xC0000000
#define P2V(x) (x + KERN_OFFSET)
#define V2P(x) (x - KERN_OFFSET)

void pmm_init(multiboot_t *mb);

uint32_t get_frame();
uint32_t set_frame(uint32_t addr);
void free_frame(uint32_t addr);

#endif
