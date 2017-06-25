#ifndef LLIST_H
#define LLIST_H

#include "kheap.h"
#include <stdint.h>

void add(uint32_t *x, uint32_t *list);

header_t *remove(uint32_t *x, uint32_t *list);

#endif
