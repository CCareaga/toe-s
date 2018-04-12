#ifndef _DLLIST_H
#define _DLLIST_H

#include <stdint.h>

typedef struct dllist dllist_t;

dllist_t *dll_init();

void dll_add(dllist_t *dll, void *data);
void dll_sorted_add(dllist_t *dll, void *data);
void dll_remove(dllist_t *dll, void *data);

void *dll_pop_head(dllist_t *dll);


#endif
