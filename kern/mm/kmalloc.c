#include "kmalloc.h"
#include "kheap.h"
#include "x86.h"
#include "pmm.h"

extern void *end;
extern uint8_t kheap_initialized;

static char *alloc_ptr = (char *) &end;

// this is the allocation helper function, before the kernel heap is initialized
// we simply move an allocation ptr that starts at the end of the kernel
// this wont be used after the kernel heap is initialized therefore I use 
// V2P to for returning the physical address. After that there is no garuantee 
// V2P will return the physical addr so after that we use 'get_physical'
static void *kmalloc_helper(uint32_t sz, uint8_t aligned, uint32_t *paddr) {
    void *addr;

    if (kheap_initialized) {
        return kheap_alloc(sz);
    }

    // this is when there is no kernel heap! 
    if (aligned) 
        alloc_ptr = (char *) PG_ROUND_UP((uint32_t) alloc_ptr);

    if (paddr)
        *paddr = (uint32_t) V2P(alloc_ptr);

    addr = (void *) alloc_ptr;
    alloc_ptr += sz;
    return addr;
}

void kfree(void *p) {
    kheap_free(p); 
}

void *kmalloc(uint32_t sz) {
    return kmalloc_helper(sz, 0, NULL);
}

void *kmalloc_a(uint32_t sz) {
    return kmalloc_helper(sz, 1, NULL);
}

void *kmalloc_p(uint32_t sz, uint32_t *addr) {
    return kmalloc_helper(sz, 0, addr);
}

void *kmalloc_ap(uint32_t sz, uint32_t *addr) {
    return kmalloc_helper(sz, 1, addr);
}

void *kmalloc_get_end() {
    return (void *) alloc_ptr;
}

