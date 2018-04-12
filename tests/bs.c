#include "bs.h"
#include <stdlib.h>
#include <stdio.h>

struct bitset {
    uint32_t sz;    // sz of the bits array (num of bits * 32)
    uint32_t *bits; // array of integers representing bits
};

// first we have to round to the nearest multiple of 32 because
// we are using integers to store the bitset then we malloc enough
// integers to store the entire bitset
bs_t *bs_init(uint32_t sz) {
    uint32_t rnd = sz % 32;
    uint32_t bit_cnt = sz + (32 - rnd);
    uint32_t arr_sz = bit_cnt / 32;

    bs_t *bs = malloc(sizeof(bs_t));
    bs->bits = malloc(sizeof(uint32_t) * arr_sz);
    bs->sz = arr_sz;

    return bs;
}

void bs_set(bs_t *bs, uint32_t idx) {
    // ASSERT(idx < bs->sz * 32);
    uint32_t arr_idx = idx / 32;
    uint32_t offset = idx % 32;

    bs->bits[arr_idx] |= 0x1 << offset;
}

void bs_clr(bs_t *bs, uint32_t idx) {
    // ASSERT(idx < bs->sz * 32);
    uint32_t arr_idx = idx / 32;
    uint32_t offset = idx % 32;
    uint32_t mask = 0xffffffff ^ (0x1 << offset); // 11110111..111

    bs->bits[arr_idx] &= mask;
}

uint8_t bs_tst(bs_t *bs, uint32_t idx) {
    // ASSERT(idx < bs->sz * 32);
    uint32_t arr_idx = idx / 32;
    uint32_t offset = idx % 32;
    uint32_t mask = (0x1 << offset); // 000100..00

    return (bs->bits[arr_idx] & mask) >> offset;
}

void bs_dump(bs_t *bs) {
    for (int i = 0; i < bs->sz; i++) {
        for (int n = 0; n < 32; n++) {
            printf("%d ", (bs->bits[i] & (0x1 << n)) >> n);
        }
        printf("\n");
    }
}
