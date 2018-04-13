#ifndef _PMM_H
#define _PMM_H

#include "multiboot.h"

void pmm_init(multiboot_t *mb);

uint32_t get_frame();
uint32_t set_frame(uint32_t addr);

#endif
