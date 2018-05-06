#ifndef _VMM_H
#define _VMM_H

#include "types.h"
#include "x86.h"

extern pg_dir_t *kern_dir;

void vmm_init();
uint32_t get_physical(uint32_t virt, pg_dir_t *pd);
uint32_t allocate_stack(uint32_t start, uint32_t sz, pg_dir_t* pd);
void relocate_stack(uint32_t *bottom, uint32_t size);

#endif
