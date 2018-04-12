#include "dllist.h"

typedef struct node {
    void *data;

    dll_node_t *next;
    dll_node_t *prev;
} dll_node_t;

struct dllist {
    dll_node_t *head
};

dllist_t *dll_init();
