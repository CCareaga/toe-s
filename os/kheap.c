#include "include/kheap.h"

extern uint8_t kheap_init = 0x0;

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
    kheap_init = 0x1;
}

void *alloc(heap_t *heap, size_t size) {
    // uint32_t index = log10(size) / log10(2); // get th elog base 2 of the size
    uint32_t index = 0;
    uint32_t sz = size;
    while (sz >>= 1) index++; // calculate log2(size);
   
    // if the calculated index is more than 10 (size > 1024) then
    // we set the index to 11 which will point to the large single block
    if (index > 10) {  
        index = 10;     
    }

    header_t *head = (header_t *) heap->bins[index];
    // if the bin for this size is empty then we move to the next biggest until we
    // find a non-empty bin or we get to the last bin
    while (head == NULL) {
        head = heap->bins[++index];
    }

    // the chunk we found is larger than we need so take what we need by splitting
    // and place the other piece in the correct bin!
    if (head->size > size) {
        header_t *split = ((char *) head + sizeof(header_t)) + size; // pointer to new chunk
        split->size = head->size - size - sizeof(header_t);
        split->hole = 1;

        // calculate index of our split off chunk so we can place in the correct bin
        uint32_t new_idx = 0;
        uint32_t new_sz = split->size;
        // some may say this is ugly but i think it is concise!
        while (new_sz >>= 1) new_idx++;
        if (new_idx > 10) new_idx = 10;

        add(split, &heap->bins[new_idx]); // add the peice we split off
    }

    head->hole = 0; 
    return ((void *) remove(head, &heap->bins[index])) + sizeof(header_t); // remove the requested chunk and return it
}
