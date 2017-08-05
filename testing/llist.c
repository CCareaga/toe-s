#include <stdio.h>
#include <stdlib.h>

#include "llist.h"

void add_node(bin_t *bin, node_t* node) {
    node_t *temp = bin->head;

    if (bin->head == NULL) {
        bin->head = node;
        return;
    }
    
    // we need to save next and prev while we iterate
    node_t *current = bin->head;
    node_t *previous = NULL;
    // iterate until we get the the end of the list or we find a 
    // node whose size is
    while (current != NULL && current->size <= node->size) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) { // we reached the end of the list
        previous->next = node;
        node->prev = previous;
    }
    else {
        if (previous != NULL) { // middle of list, connect all links!
            node->next = current;
            previous->next = node;

            node->prev = previous;
            current->prev = node;
        }
        else { // head is the only element
            node->next = bin->head;
            bin->head = node;
            bin->head->prev = node;
        }
    }
}

void remove_node(bin_t * bin, node_t *node) {
    if (bin->head == NULL) return; 
    if (bin->head == node) { 
        bin->head = bin->head->next;
        return;
    }
    
    node_t *temp = bin->head->next;
    while (temp != NULL) {
        if (temp == node) { // found the node
            if (temp->next == NULL) { // last item
                temp->prev->next = NULL;
            }
            else { // middle item
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
            }
            // we dont worry about deleting the head here because we already checked that
            return;
        }
        temp = temp->next;
    }
    // item not in list! 
}

node_t *get_best_fit(bin_t *bin, uint32_t size) {
    if (bin->head == NULL) return NULL; // empty list!

    node_t *temp = bin->head;

    while (temp != NULL) {
        if (temp->size >= size) {
            return temp; // found a fit!
        }
        temp = temp->next;
    }
    return NULL; // no fit!
}

node_t *next(node_t *current) {
    return current->next;
}

node_t *prev(node_t *current) {
    return current->prev;
}

node_t *make_node(uint32_t hole, uint32_t size) {
    node_t *new_node = (node_t *) malloc(sizeof(node_t));
    new_node->size = size;
    new_node->hole = hole;
}

void dump_bin(bin_t *bin) {
    node_t *temp = bin->head;

    while (temp != NULL) {
        printf("%x hole: %d, size: %d \n", temp, temp->hole, temp->size);
        temp = temp->next;
    }
    printf("\n");
}
