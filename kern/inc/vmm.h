#ifndef _VMM_H
#define _VMM_H

#include "types.h"
#include "x86.h"

void vmm_init();
uint32_t get_physical(uint32_t virt, pg_dir_t *pd);


#endif
