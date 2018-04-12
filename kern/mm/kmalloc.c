#include "kmalloc.h"
#include "kheap.h"
#include "x86.h"

extern void *end;
extern uint8_t kheap_initialized;

static uintptr_t alloc_ptr = (uintptr_t) &end;

// this is the allocation helper function, before the kernel heap is initialized
// we simply move an allocation ptr that starts at the end of the kernel
static void *kmalloc_helper(uint32_t sz, uint8_t aligned, uint32_t *paddr) {
    void *addr;

    if (kheap_initialized) {
        addr = kheap_alloc(sz);

        return addr;
    }

    // this is when there is no kernel heap! 
    if (aligned) 
        alloc_ptr = PG_ROUND_UP(alloc_ptr);

    if (paddr)
        *paddr = alloc_ptr;

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

uintptr_t kmalloc_get_end() {
    return alloc_ptr;
}

