#ifndef VMM_H
#define VMM_H

#include "sys.h"
#include "isr.h"
#include <stdint.h>

void init_paging(uint32_t end);

void switch_page_directory(uint32_t *dir);

void page_fault(registers *regs);

#endif
