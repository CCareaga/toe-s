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
    return 0;
}
