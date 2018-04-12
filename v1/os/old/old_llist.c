#include "include/llist.h"
#include "include/vga.h"

uint32_t hdrsz = sizeof(header_t) / 4;

// this function will add the data in x to the end of our kernel bin linekd list
// x should be a pointer to a header. this function depends on the fact that the 
// pointer to the next chunk is 8 bytes away for the header

void add(uint32_t *x, uint32_t *list) {
    // if the list is null then put x at location list is pointing too
    header_t *headx = (header_t *) x;

    if (*list == 0x0) {
        *list = x;
        uint32_t *p = x + hdrsz + 1;
        *p = list;
        return;
    }

    // the list is not null so iterate through the list by deferencing 
    header_t *current = (header_t *) *list;
    
    while (current->size < headx->size) {
        // when we derefernce we add 8 to skip the header data
        current = (header_t *) next(current);
    }

    // set the last item in the list's next to our new item
    uint32_t *cnext = ((uint32_t *) current) + hdrsz;
    *cnext  = x;
    
    // set the new item's prev to our last item in list
    uint32_t *xprev = (x + hdrsz + 1);
    *xprev = current;
}

// this function will remove the given chunk (given by a pointer to a header) 
header_t *remove(uint32_t *x, uint32_t *list) {
    uint32_t *xnext = next(x);
    uint32_t *xprev = prev(x);
    uint32_t *prev_addr = (uint32_t *) ((char *) x + sizeof(header_t) + 4);

    // this is the only item in the list so our list is now null
    if (*prev_addr == list && xnext == NULL) {
        *list = NULL;
    }

    // this is the last chnk in the list simply disconnect it from the list
    else if (xnext == NULL) {
        // do some crazy pointer stuff to get the previous chunks next pointer
        uint32_t *prev_next = xprev + hdrsz;
        *prev_next = NULL;
    }
    
    // this is the first chunk in the list so make the second one the head
    else if (*prev_addr = list) {
        uint32_t *next_prev = xnext + hdrsz + 1;
        *next_prev = NULL;
        *list = xnext;
    }
   
    // the provided chunk has neighbors on both sides so we connect them
    else {
        uint32_t *prev_next = xprev + hdrsz;
        uint32_t *next_prev = xnext + (hdrsz + 1);

        // make previous point to next and next point back to previous
        *prev_next = next;
        *next_prev = prev;
    }
    
    memset((x + hdrsz), 0, 8); // clear the links to other nodes
    return (header_t *) x; // return pointer to our now free block
}

// this function is supposed to backtrack until it finds an address that 
// is within the range of the heap bins, the address it finds should be
// the address of the bin that its in!
uint32_t *get_list(header_t *head, heap_t *heap) {
    // first get the address of the previous header
    uint32_t *prev_addr = (uint32_t *) *((char *) head + sizeof(header_t) + 4);
    
    // while this header is out of bin range continue back tracking
    while (prev_addr > &heap->bins[10] || prev_addr < &heap->bins[0]) {
        vga_writeln(itoa(prev_addr, 16));
        prev_addr = (uint32_t *) *((char *) prev_addr + sizeof(header_t) + 4);
    }
    
    return prev_addr;
}

// pointer arithmetic to get the next header in the list
header_t *next(header_t *current) {
    return (header_t *) *(((uint32_t *) current) + hdrsz);
}

// pointer arithmetic to get the address of the previous header in the list
header_t *prev(header_t *current) {
    return (header_t *) *(((uint32_t *) current) + hdrsz + 1);
}
