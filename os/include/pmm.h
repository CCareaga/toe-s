#ifndef PMM_H
#define PMM_H

#include "multiboot.h"
#include <stdint.h>

void init_pmm(multiboot_info_t *mbi, uint32_t kern_end);

void push_frame(uint32_t addr);

uint32_t pop_frame();

#endif