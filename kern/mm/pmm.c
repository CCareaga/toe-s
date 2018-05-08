#include "pmm.h"
#include "x86.h"
#include "klib.h"
#include "kmalloc.h"
#include "bitset.h"

static bs_t *frames;

// first we create a bit set of frames we need to know how large the 
// bitset should be so we add the values from the multiboot info to 
// get the available memory in KB, since each page is 4k we divide this 
// number by 4 to get the number of pages. We then use the multiboot
// memory map to mark which frames are unavailable
void pmm_init(multiboot_t *mb) {
    uint32_t sz = mb->mem_upper + mb->mem_lower;
    kprintf("allocating %d page frames \n", (sz / 4)); 

    frames = bs_init(sz / 4); 

    uint32_t addr;

    mboot_memmap_t *mmap = (mboot_memmap_t *) P2V(mb->mmap_addr);

    while ((unsigned long) mmap < P2V(mb->mmap_addr) + mb->mmap_length) {
        if (mmap->type == 2) {
            kprintf("mmap type 2: 0x%x \n", mmap->length);
            for (unsigned long long int i = 0; i < mmap->length; i += PG_SZ) {
                addr = mmap->base_addr + i;

                if (addr > 0xffffffff) break;

                bs_set(frames, (addr & 0xfffff000) / PG_SZ);
            }
        }

        mmap = (mboot_memmap_t *) ((unsigned long) mmap + mmap->size + sizeof (mmap->size));
    }
}

// returns address of the first free frame in the bit set
uint32_t get_frame() {
    uint32_t idx = bs_first_clr(frames);

    if BAD_RETURN(frames, idx) {
        kprintf("OUT OF PAGE FRAMES!?!\n");
        return 0xffffffff;
    }

    set_frame(idx * PG_SZ);

    return idx * PG_SZ;
}

// mark the provided page frame in our bit set
uint32_t set_frame(uint32_t addr) {
    uint32_t idx = addr / PG_SZ;
    if (idx < bs_get_size(frames)) {
        bs_set(frames, idx);
        return 1;
    }
    else {
        return 0;
    }
}

void free_frame(uint32_t addr) {
    // uint32_t idx = addr / PG_SZ;
    bs_clr(frames, addr);
}
