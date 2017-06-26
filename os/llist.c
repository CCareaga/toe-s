#include "include/llist.h"
#include "include/vga.h"

uint32_t hdrsz = sizeof(header_t) / 4;

// this function will add the data in x to the end of our kernel bin linekd list
// x should be a pointer to a header. this function depends on the fact that the 
// pointer to the next chunk is 8 bytes away for the header

void add(uint32_t *x, uint32_t *list) {
    // if the list is null then put x at location list is pointing too
    if (*list == 0x0) {
        *list = x;
        return;
    }

    // the list is not null so iterate through the list by deferencing 
    uint32_t *current = (uint32_t *) *list;
    while (next(current) != 0x0) {
        // when we derefernce we add 8 to skip the header data
        current = (uint32_t *) next(current);
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

    // this is the only tem in the list so our list is now null
    if (xprev == NULL && xnext == NULL) {
        *list = NULL;
    }

    // this is the last chnk in the list simply disconnect it from the list
    else if (xnext == NULL) {
        // do some crazy pointer stuff to get the previous chunks next pointer
        uint32_t *prev_next = xprev + hdrsz;
        *prev_next = NULL;
    }
    
    // this is the first chunk in the list so make the second one the head
    else if (xprev == NULL) {
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
    return (header_t *) x; // return pointer ot our now free block
}

header_t *next(header_t *current) {
    return (header_t *) *(((uint32_t *) current) + hdrsz);
}

header_t *prev(header_t *current) {
    return (header_t *) *(((uint32_t *) current) + hdrsz + 1);
}
