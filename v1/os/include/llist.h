#ifndef LLIST_H
#define LLIST_H

#include "kheap.h"
#include <stdint.h>

void add_node(bin_t *bin, node_t *node);

void remove_node(bin_t *bin, node_t *node);

node_t *get_best_fit(bin_t *list, uint32_t size);
node_t *get_last_node(bin_t *list);

node_t *next(node_t *current);
node_t *prev(node_t *current);

node_t *make_node(uint32_t hole, uint32_t size);

void dump_bin(bin_t *bin);

#endif
