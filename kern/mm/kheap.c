#include "kheap.h"
#include "kmalloc.h"
#include "klib.h"
#include "vmm.h"

#define BIN_CNT 9
#define BLCK_OVERHEAD (sizeof(block_t) + sizeof(footer_t))
#define OFFSET 8

typedef struct block_t {
    uint8_t hole;           // if this block free?
    uint32_t size;          // what is its size

    struct block_t* next;   // next node in the bin
    struct block_t* prev;   // previous node in the bin
} block_t;

typedef struct {
    block_t *head;      // sorted doubly linked list head
} bin_t;

typedef struct {
    uint32_t start;         // start of the heap
    uint32_t end;           // end of the heap
    uint32_t max;           // max end of the heap
    uint8_t super;          // priv level
    uint8_t ro;             // read only?
    bin_t *bins[BIN_CNT];   // bins, one for each size (9 possible)
} heap_t;


typedef struct { 
    block_t *header;        // pointer to the header of this block
} footer_t;

uint8_t kheap_initialized = 0;
static heap_t *kheap;

// prototypes
void add_block_sorted(bin_t *bin, block_t *blck);
void remove_block(bin_t *bin, block_t *blck);
bin_t *get_bin(uint32_t sz);
uint32_t get_bin_index(uint32_t sz);
block_t *get_first_fit(bin_t *bin, uint32_t sz);
block_t *get_wilderness();
void create_foot(block_t *blck);
footer_t *get_foot(block_t *blck);


// this is the heap pre initialization, it uses the dummy allocator
// to create the needed heap structures.
void kheap_pre_init() {
    kheap = kmalloc(sizeof(heap_t));
    kheap->start = KHEAP_START;
    kheap->end = KHEAP_END;

    for (int i = 0; i < BIN_CNT; i++) 
        kheap->bins[i] = kmalloc(sizeof(bin_t));
}

// this is the second stage of initializaiton, this happens after we map
// in the needed pages for our kernel heap. all we do here is create an
// initial block and place it in its respective bin
void kheap_init() {
    block_t *init = (block_t *) KHEAP_START;
    init->size = KHEAP_END - KHEAP_START - BLCK_OVERHEAD;
    init->next = NULL;
    init->prev = NULL;

    create_foot(init);
    add_block_sorted(get_bin(init->size), init);

    kheap_initialized = 1;
}
    
// this function is used to allocate a page from the heap. if we are
// requesting a page it is probably for a paging structure so we 
// make sure it is alined as well
void *kheap_alloc_a(uint32_t size) {
    block_t *ret, *wild, *new_wild; 

    // first we grab the wilderness block and remove it
    wild = get_wilderness();
    uint32_t old_sz = wild->size;
    wild->hole = 1;
    remove_block(get_bin(wild->size), wild);
    
    uint32_t on_boundary = (((char *) wild + 8) && 0x00000FFF) == 0;

    if (!on_boundary) {
        uint32_t extra = (PG_SZ) - ((uint32_t) wild & 0x00000FFF);

        // this is the small chunk created when we round up to pg boundary
        wild->size = extra - BLCK_OVERHEAD - 8;
        create_foot(wild);
        wild->hole = 1;
        add_block_sorted(get_bin(wild->size), wild);

        // this is the block that is given back that is on a pg boundary
        ret = (block_t *) ((char *) wild + wild->size + BLCK_OVERHEAD);
        ret->size = size;
        create_foot(ret);
        ret->hole = 0;

        // this is the new wilderness 
        new_wild = (block_t *) ((char *) ret + size + BLCK_OVERHEAD);
        new_wild->size = old_sz - wild->size - ret->size - (2 * BLCK_OVERHEAD);
        new_wild->hole = 1;
        create_foot(new_wild);
        add_block_sorted(get_bin(new_wild->size), new_wild);
    }

    else {
        // this case the wilderness block is on a page boundary!
        new_wild = (block_t *) ((char *) wild + size + BLCK_OVERHEAD);
        new_wild->hole = 1;
        create_foot(new_wild);
        add_block_sorted(get_bin(new_wild->size), new_wild);

        ret = wild;
        ret->size = size;
        ret->hole = 0;
        create_foot(ret);
    }
    
    memset(&ret->next, 0, (ret->size + 8));
    return &ret->next; 
}

// allocate a block of a certain size using the kheap
// what we do here is check the first bin where a block on this size
// *should* be, if there isn't one to fill the request we move to the 
// next largest bin. worst case we eventually get to th wilderness block
// if a block is way larger than the request we simply split it 
void *kheap_alloc(uint32_t sz) {
    uint32_t idx = get_bin_index(sz);
    bin_t *tmp = kheap->bins[idx];

    block_t *found = get_first_fit(tmp, sz);

    while (!found) {
        tmp = kheap->bins[++idx];
        found = get_first_fit(tmp, sz);
    }

    if ((found->size - sz) > (BLCK_OVERHEAD + MIN_ALLOC_SZ)) {
        block_t *split = (block_t *) ((char *) found + BLCK_OVERHEAD + sz);
        split->size = found->size - sz - BLCK_OVERHEAD;
        split->hole = 1;

        create_foot(split);
        add_block_sorted(get_bin(split->size), split);

        found->size = sz;
        create_foot(found);
    }
    
    found->hole = 0;
    remove_block(tmp, found);

    memset(&found->next, 0, (found->size + 8));
    return &found->next;
}

// this function frees an address provided by kheap_alloc
// what we do i check the block before and after this block
// in memory, then we depending one whether or not those blocks
// are free we coalesce the block then place it into a bin
void kheap_free(void  *addr) {
    block_t *blck = (block_t *) ((char *) addr - OFFSET);

    if (blck == (block_t *) kheap->start) {
        blck->hole = 1;
        add_block_sorted(get_bin(blck->size), blck);
        return;
    }

    block_t *next = (block_t *) ((char *) get_foot(blck) + sizeof(footer_t));

    footer_t *f = (footer_t *) ((char *) blck - sizeof(footer_t));
    block_t *prev = f->header;

    if (prev->hole) {
        remove_block(get_bin(prev->size), prev);

        prev->size += BLCK_OVERHEAD + blck->size;
        footer_t *foot = get_foot(blck);
        foot->header = prev;

        blck = prev;
    }

    if (next->hole) {
        remove_block(get_bin(next->size), next);

        blck->size += BLCK_OVERHEAD + next->size;

        footer_t *old_foot = get_foot(next);
        old_foot->header = 0;
        next->size = 0;

        footer_t *new_foot = get_foot(blck);
        new_foot->header = blck;
    }

    blck->hole = 1;
    add_block_sorted(get_bin(blck->size), blck);
}

// creates a footer structure for the given block
void create_foot(block_t *blck) {
    footer_t *foot = get_foot(blck);
    foot->header = blck;
}

// uses the size of a block to calculate and return the footer position
footer_t *get_foot(block_t *blck) {
    return (footer_t *) ((char *) blck + sizeof(block_t) + blck->size);
}

// explicit block list =====================

// adds a block to a bin making sure to maintain sorted order
void add_block_sorted(bin_t *bin, block_t *blck) {
    blck->next = NULL;
    blck->prev = NULL;

    if (!bin->head) {
        bin->head = blck;
        return;
    }

    block_t *temp = bin->head;
    block_t *prev = NULL;

    while (temp && temp->size <= blck->size) {
        prev = temp;
        temp = temp->next;
    }

    if (!temp) {
        prev->next = blck;
        blck->prev = prev;
    }

    else {
        if (prev) {
            blck->next = temp;
            prev->next = blck;

            blck->prev = prev;
            temp->prev = blck;
        }
        else {
            blck->next = bin->head;
            bin->head->prev = blck;
            bin->head = blck;
        }
    }
}

// removes a block from the provided bin
void remove_block(bin_t *bin, block_t *blck) {
    if (!bin->head) return;
    if (bin->head == blck) {
        bin->head = bin->head->next;

        blck->next = NULL;
        blck->prev = NULL;
        return;
    }

    block_t *temp = bin->head->next;
    while (temp) {
        if (temp == blck) { // we found it
            if (temp->next == NULL) { // last item in bin
                temp->prev->next = NULL;
            }
            else { // middle item, relink 
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
            }
            
            blck->next = NULL;
            blck->prev = NULL;
            return;
        }

        temp = temp->next;
    }
    
    kprintf("block not in bin... whatttt \n");
}

// gets the first block that is large enough by iterating
// through the provided free block bin
block_t *get_first_fit(bin_t *bin, uint32_t sz) {
    if (!bin->head) return NULL;

    block_t *temp = bin->head;

    while (temp) {
        if (temp->size >= sz) {
            return temp;
        }

        temp = temp->next;
    }

    return  NULL;
}

// gets a pointer to a bin given the size
bin_t *get_bin(uint32_t sz) {
    return kheap->bins[get_bin_index(sz)];
}

// calculates which bin to look in first for the provided size
uint32_t get_bin_index(uint32_t sz) {
    uint32_t index = 0;
    sz = sz < 4 ? 4 : sz;

    while (sz >>= 1) index++; 
    index -= 2;

    index = (index > BIN_CNT - 1) ?  (BIN_CNT - 1) : index;
    return index;
}

// we can assume the wilderness footer will always be at the end of the heap
// so we use it to get the header for the wilderness chunk
block_t *get_wilderness() {
    footer_t *wild_foot = (footer_t *) ((char *) kheap->end - sizeof(footer_t));
    return wild_foot->header;
}

