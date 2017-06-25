#include "include/kheap.h"

void *find_hole(size_t size, heap_t *heap) {
    header_t *head = heap->start;
    header_t *next; 

    while (head < heap->end) {
        if (head->hole) {
            if (head->size >= size) {
                return head + (sizeof(header_t));
            }
            next = (char *) head + head->size;
            if (next < heap->end && next->hole) {
                head->size += (sizeof(header_t) + next->size);
            }
        }
        head += head->size;
    }
    return 0;
}

void init_heap(uint32_t start, uint32_t end, uint32_t max, heap_t *heap) {
    // now since the heap starts with one huge section of memory we need to make 
    // give it a header and place it in the last bin, the last bin holds any chunks that 
    // are too large to fit into any other bin
    header_t *init_region = (header_t *) start;
    init_region->hole = 1;
    init_region->size = KHEAP_INIT_SIZE - sizeof(header_t);
    add(init_region, &heap->bins[10]);
    vga_writeln(itoa(heap->bins[10], 16));
}

void *alloc(heap_t *heap, size_t size) {
    // uint32_t index = log10(size) / log10(2); // get th elog base 2 of the size

    vga_writeln(itoa(size, 10));
}
