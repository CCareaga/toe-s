#ifndef _KMALLOC_H
#define _KMALLOC_H

#include "types.h"

void *kmalloc_ap(uint32_t sz, uint32_t *addr);
void *kmalloc_p(uint32_t sz, uint32_t *addr);
void *kmalloc_a(uint32_t sz);
void *kmalloc(uint32_t sz);

void *kmalloc_get_end();

#endif
