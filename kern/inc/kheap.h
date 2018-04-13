#ifndef _KHEAP_H
#define _KHEAP_H 

#include "types.h"

#define KHEAP_START 0xC0000000
#define KHEAP_END   0xC0001000
#define KHEAP_MAX   0xCFF00000

#define MIN_ALLOC_SZ 4

void kheap_pre_init();
void kheap_init();
    
void *kheap_alloc(uint32_t sz);
void kheap_free(void  *addr);

#endif

