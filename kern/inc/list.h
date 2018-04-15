#ifndef _LIST_H
#define _LIST_H

typedef struct links links_t;

#define container_of(ptr, type, memb) ({ \
    const typeof( ((type *)0)->member ) *mptr = (ptr); \
    (type *)( (char *) mptr - offsetof(type,member) ); \
}) 

#define get_links(ptr, type, member) \
    container_of(ptr, type, member)

#define list_add(type, head, node, memb) ({ \
    links_t *tmp = get_links(head, type, memb);\
    links_t *n = get_links(node, type, memb);\
    while (get_links(tmp)->next) tmp = get_links(tmp)->next; \
    get_links(tmp)->next = node; \
})\

#endif 
