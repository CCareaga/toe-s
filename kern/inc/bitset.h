#ifndef _BITSET_H
#define _BITSET_H

#include "types.h"

#define BAD_RETURN(a, x) (x == bs_get_size(a))

typedef struct bitset bs_t;

bs_t *bs_init(uint32_t sz);

uint32_t bs_get_size(bs_t *bs);
void bs_set(bs_t *bs, uint32_t idx);
void bs_clr(bs_t *bs, uint32_t idx);
uint8_t bs_tst(bs_t *bs, uint32_t idx);
void bs_dmp(bs_t *bs);
uint32_t bs_first_clr(bs_t *bs);

#endif
