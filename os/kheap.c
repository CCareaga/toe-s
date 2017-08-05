#include "include/kheap.h"

extern uint8_t kheap_init = 0x0;
uint32_t offset = sizeof(node_t) - 8;

void init_heap(uint32_t start, uint32_t end, uint32_t max, heap_t *heap) {
    // now since the heap starts with one huge section of memory we need to make 
    // give it a header and place it in the last bin, the last bin holds any chunks that 
    // are too large to fit into any other bin
    node_t *init_region = (node_t *) start;
    init_region->hole = 1;
    init_region->size = KHEAP_INIT_SIZE - sizeof(node_t) - sizeof(footer_t);

    create_foot(init_region);

    add_node(heap->bins[10], init_region);

    heap->start = start;
    heap->end = end;
    heap->max = max;
    kheap_init = 0x1;
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
    // dump_bin(heap->bins[index]);
    
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

        //add_node(heap->bins[get_bin_index(prev->size)], prev);
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

uint32_t get_bin_index(size_t sz) {
    uint32_t index = 0;
    while (sz >>= 1) index++; // calculate log2(size);
    if (index > 10) index = 10;
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
