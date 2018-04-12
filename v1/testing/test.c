#include <stdio.h>
#include <stdlib.h>

#include "kheap.h"
#include "llist.h"

int main() {
    bin_t *list = (bin_t *) malloc(sizeof(bin_t));
    list->size = 0;

    node_t *node32 = make_node(0, 32);
    add_node(list, node32);

    node_t *node15_1 = make_node(1, 15);
    add_node(list, node15_1);

    node_t *node18 = make_node(0, 18);
    add_node(list, node18);

    node_t *node2 = make_node(0, 2);
    add_node(list, node2);

    node_t *node15_0 = make_node(0, 15);
    add_node(list, node15_0);
    
    node_t *node_26 = make_node(0, 26);
    add_node(list, node_26);

    node_t *node_4 = make_node(0, 4);
    add_node(list, node_4);

    node_t *found = get_best_fit(list, 33);
    printf("NODE: %x, size: %d\n", found, found->size);
    dump_bin(list);

    return 0;
}
