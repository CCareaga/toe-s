#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>
// #include <math.h>
#include <stddef.h>
#include "vga.h"

#define KHEAP_START 0xC0000000
#define KHEAP_INIT_SIZE 0x1000000
#define KHEAP_MAX 0xCFFFF000

// extern uint8_t kheap_init;

typedef struct node_t {
    uint32_t hole;
    uint32_t size;
    struct node_t* next;
    struct node_t* prev;
} node_t;

typedef struct { 
    node_t *header;
} footer_t;

typedef struct {
    node_t* head;
    uint32_t size;
} bin_t;

typedef struct {
    uint32_t start;
    uint32_t end;
    uint32_t max;
    uint8_t super;
    uint8_t ro;
    bin_t *bins[9];
} heap_t;

void init_heap(heap_t *heap, uint32_t start, uint32_t end, uint32_t max, uint8_t super, uint8_t ro);

void *alloc(heap_t *heap, size_t size);
void free(heap_t *heap, void *p);
void expand(heap_t *heap, size_t sz);

uint32_t get_bin_index(uint32_t sz);
void create_foot(node_t *head);
footer_t *get_foot(node_t *head);

#endif
