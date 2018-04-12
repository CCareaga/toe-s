#ifndef _BITSET_H
#define _BITSET_H

#include <stdint.h>

typedef struct bitset bs_t;

bs_t *bs_init(uint32_t sz);

void bs_dump(bs_t *bs);
void bs_set(bs_t *bs, uint32_t idx);
void bs_clr(bs_t *bs, uint32_t idx);
uint8_t bs_tst(bs_t *bs, uint32_t idx);

#endif
