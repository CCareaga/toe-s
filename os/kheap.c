#include "include/kheap.h"
#include "include/vmm.h"

uint32_t offset = sizeof(node_t) - 8;
extern paging_t *kpaging;

void init_heap(heap_t *heap, uint32_t start, uint32_t end, uint32_t max, uint8_t super, uint8_t ro) {
    // now since the heap starts with one huge section of memory we need to make 
    // give it a header and place it in the last bin, the last bin holds any chunks that 
    // are too large to fit into any other bin
    node_t *init_region = (node_t *) start;
    init_region->hole = 1;
    init_region->size = KHEAP_INIT_SIZE - sizeof(node_t) - sizeof(footer_t);

    create_foot(init_region);

    add_node(heap->bins[get_bin_index(init_region->size)], init_region);

    heap->start = start;
    heap->end = end;
    heap->max = max;
    heap->super = super;
    heap->ro = ro;
}

void *alloc(heap_t *heap, size_t size) {
    // uint32_t index = log10(size) / log10(2); // get the log base 2 of the size

    uint32_t index = get_bin_index(size);
    bin_t *temp = (bin_t *) heap->bins[index];
    node_t *found = get_best_fit(temp, size);

    // if the bin for this size is empty then we move to the next biggest until we
    // find a non-empty bin or we get to the last bin
    while (found == NULL) {
        temp = heap->bins[++index];
        found = get_best_fit(temp, size);
    }

    // the chunk we found is larger than we need so take what we need by splitting
    // and place the other piece in the correct bin!
    if ((found->size - size) > 4) {
        node_t *split = ((char *) found + sizeof(node_t) + sizeof(footer_t)) + size; // pointer to new chnk
        split->size = found->size - size - sizeof(node_t) - sizeof(footer_t);
        split->hole = 1;
   
        create_foot(split);

        // calculate index of our split off chunk so we can place in the correct bin
        uint32_t new_idx = get_bin_index(split->size);

        add_node(heap->bins[new_idx], split); // add the peice we split off

        found->size = size; // make sure we assign the actual size we used!
        create_foot(found); // also create foot for this new chunk
    }

    found->hole = 0; 
    remove_node(heap->bins[index], found); // remove the requested chunk
    
    // check if the wilderness node is getting to small, if so expand heap
    node_t *wild = get_wilderness(heap);
    if (wild->size < MIN_WILDERNESS) {
        uint8_t success = expand(heap, 0x1000);
        if (success == 0) {
            return NULL;
        }
    }
    else if (wild->size > MAX_WILDERNESS) {
        contract(heap, 0x1000);
    }

    found->prev = NULL;
    found->next = NULL;
    return &found->next; 

}

// this is a generic free function which is why we take the specific heap as an arg
// hopefully we can re use this class for a user space heap (idk yet) and in that case
// we would jsut use a different heap pointer! yay
void free(heap_t *heap, void *p) {
    bin_t *list;
    footer_t *new_foot, *old_foot;

    // the address provided to the free function is the start of the free block, but
    // we want the header to we subtract the header size
    node_t *head = (node_t *) ((char *) p - offset);
    if (head == heap->start) {
        head->hole = 1; // make sure this gets done
        add_node(heap->bins[get_bin_index(head->size)], head);
        return;
    }

    // now we need the header of the next and previous blocks in the arena
    // to get the next chunk we just get the foot and add size of foot
    node_t *next = (node_t *) ((char *) get_foot(head) + sizeof(footer_t));
    // this is hideous but it does the job ... ugh
    node_t *prev = (node_t *) * ((uint32_t *) ((char *) head - sizeof(footer_t)));
    
    // the previous block is free as well, coalesce
    if (prev->hole) {
        // get the list this block is in and remove it 
        list = heap->bins[get_bin_index(prev->size)];
        remove_node(list, prev);

        // calculate new meta data and add to correct bin
        prev->size += sizeof(footer_t) + sizeof(node_t) + head->size;
        new_foot = get_foot(head);
        new_foot->header = prev;

        head = prev;
    }
    // the next block is free too so coalesce
    if (next->hole) {
        list = heap->bins[get_bin_index(next->size)];
        remove_node(list, next);

        head->size += sizeof(node_t) + next->size;

        old_foot = get_foot(next);
        old_foot->header = 0;
        next->size = 0;
        next->hole = 0;
        
        new_foot = get_foot(head);
        new_foot->header = head;
    }

    // add the new node that may have been coalesced or not
    head->hole = 1; // make sure this gets done
    add_node(heap->bins[get_bin_index(head->size)], head);
}

// this method expands the given heap by the given amount
// as long as the new size doesn't pass the heap max
// returns 1 on success and zero on failure
uint8_t expand(heap_t *heap, size_t sz) {
    size_t old_size = heap->end - heap->start;
    size_t new_size = old_size + sz;

    if ((new_size & 0xfffff000) != 0) { // if not page aligned
        new_size &= 0xfffff000;
        new_size += 0x1000;

        sz = new_size - old_size;
    }

    // make sure we can still expand
    if (heap->start + new_size > heap->max) {
        vga_writeln("heap maxed out! can't expand");
        return 0;
    }

    size_t i = old_size;
    // map some pages to for the added heap space
    while (i < new_size) {
        alloc_frame(get_page(heap->start + i, kpaging), 1, 1);
        i += 0x1000;
    }

    node_t *wild = get_wilderness(heap);
    footer_t *old_foot = get_foot(wild);
    old_foot->header = NULL; // delete old foot

    wild->size += sz; // make sure we update the wilderness node
    footer_t *new_foot = get_foot(wild);
    new_foot->header = wild;

    heap->end = heap->start + new_size; // update the end of heap addr

    return 1;
}

void contract(heap_t *heap, size_t sz) {
    size_t old_size = heap->end - heap->start;
    size_t new_size = old_size - sz;
    
    if (new_size & 0x1000) {
        new_size &= 0x1000;
        new_size += 0x1000;

        sz = old_size - new_size;
    }

    if (new_size < MIN_HEAP_SIZE) {
        new_size = MIN_HEAP_SIZE;
    }

    uint32_t i = old_size - 0x1000;
    while (new_size < i) {
        free_frame(get_page(heap->start + i, kpaging));
        i -= 0x1000;
    }
    
    node_t *wild = get_wilderness(heap);
    // don't worry about clearing the old footer it is gone!
    wild->size -= sz; // make sure we update the wilderness node
    footer_t *new_foot = get_foot(wild);
    new_foot->header = wild;

    heap->end = heap->start + new_size; // update the end of heap addr
}

// this converts a size to its corresponding bin index
uint32_t get_bin_index(size_t sz) {
    uint32_t index = 0;
    sz = sz < 4 ? 4 : sz; // if size is less than 4 make it 4

    while (sz >>= 1) index++; // calculate log2(size);
    index -= 2; // since 4 bytes is the lowest subtract two from index
    
    if (index > 8) index = 8; // if sz was bigger than 1024, make it 8
    return index;
}

// this function gets the footer address then writes the address of the provided
// node to the header field of the footer
void create_foot(node_t *head) {
    footer_t *foot = get_foot(head);
    foot->header = head;
}

//  this function uses some pointer arithmetic to get the addr of the foot
footer_t *get_foot(node_t *node) {
    return (footer_t *) ((char *) node + sizeof(node_t) + node->size);
}

// we can assume the wilderness footer will always be at the end of the heap
// so we use it to get the header for the wilderness chunk
node_t *get_wilderness(heap_t *heap) {
    footer_t *wild_foot = (footer_t *) ((char *) heap->end - sizeof(footer_t));
    return wild_foot->header;
}
