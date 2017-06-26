#ifndef LLIST_H
#define LLIST_H

#include "kheap.h"
#include <stdint.h>

void add(uint32_t *x, uint32_t *list);

header_t *remove(uint32_t *x, uint32_t *list);

header_t *next(header_t *current);
header_t *prev(header_t *current);
#endif
