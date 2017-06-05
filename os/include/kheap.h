#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>
#include <stddef.h>

#define KHEAP_START 0xC0000000
#define KHEAP_INIT_SIZE 0x1000000
#define KHEAP_MAX 0xCFFFF000

typedef struct {
    uint8_t hole;
    uint32_t size;
} header_t;

typedef struct { 
    header_t *header;
} footer_t;

typedef struct {
    uint32_t start;
    uint32_t end;
    uint32_t max;
} heap_t;

void init_heap(uint32_t start, uint32_t end, uint32_t max, heap_t *heap);

void *find_hole(size_t size, heap_t *heap);

#endif

