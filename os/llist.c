#include "include/llist.h"
#include "include/vga.h"

// this function will add the data in x to the end of our kernel bin linekd list
// x should be a pointer to a header. this function depends on the fact that the 
// pointer to the next chunk is 8 bytes away for the header

void add(uint32_t *x, uint32_t *list) {
    // if the list is null then put x at location list is pointing too
    if (*list == NULL) {
        *list = x;
        return;
    }
    // the list is not null so iterate through the list by deferencing 
    uint32_t *current = list;
    while (current != NULL) {
        // when we derefernce we add 8 to skip the header data
        current = (uint32_t *) (*(current + 8));
    }
    current = x; // current is null so assign it to the address of x
    *(x+12) = current - 8; // assign x's prev field to current (minus 8 to get header)
}

// this function will remove the given chunk (given by a pointer to a header) 
header_t *remove(uint32_t *x, uint32_t *list) {
    uint32_t *next = (x + 8);
    uint32_t *prev = (x + 12);

    // this is the last chnk in the list simply disconnect it from the list
    if (next == NULL) {
        // do some crazy pointer stuff to get the previous chunks next pointer
        uint32_t *prev_next = ((uint32_t *) *prev) + 8;
        prev_next = NULL;
    }

    // this is the only tem in the list so our list is now null
    if (prev == NULL) {
        list = NULL;
    }
   
    // the provided chunk has neighbors on both sides so we connect them
    else {
        uint32_t *prev_next = ((uint32_t *) *prev) + 8;
        uint32_t *next_prev = ((uint32_t *) *next) + 12;

        // make previous point to next and next point back to previous
        *prev_next = next;
        *next_prev = prev;
    }

    return (header_t*) x; // return pointer ot our now free block
}
