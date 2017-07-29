#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>
// #include <math.h>
#include <stddef.h>
#include "vga.h"

#define KHEAP_START 0xC0000000
#define KHEAP_INIT_SIZE 0x1000000
#define KHEAP_MAX 0xCFFFF000

extern uint8_t kheap_init;

typedef struct {
    uint32_t hole;
    uint32_t size;
} header_t;

typedef struct { 
    header_t *header;
} footer_t;

typedef uint32_t** bins_t;

typedef struct {
    uint32_t start;
    uint32_t end;
    uint32_t max;
    bins_t bins;
} heap_t;

void init_heap(uint32_t start, uint32_t end, uint32_t max, heap_t *heap);

void *find_hole(size_t size, heap_t *heap);

void *alloc(heap_t *heap, size_t size);
void free(heap_t *heap, void *p);

uint32_t get_bin_index(uint32_t sz);
void create_foot(header_t *head);
footer_t *get_foot(header_t *head);
#endif
