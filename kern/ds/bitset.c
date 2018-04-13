#include "bitset.h"
#include "kmalloc.h"
#include "klib.h"

struct bitset {
    uint32_t sz;    // size of bits array (32 * num of bits)
    uint32_t *bits; // array of integers representing bits
};

// first we have to round to the nearest multiple of 32 because
// we are using integers to store the bitset then we malloc enough
// integers to store the entire bitset
bs_t *bs_init(uint32_t sz) {
    uint32_t rnd = sz % 32;
    uint32_t bit_cnt = sz + (32 - rnd);
    uint32_t arr_sz = bit_cnt / 32;

    bs_t *bs = kmalloc(sizeof(bs_t));
    bs->bits = kmalloc(sizeof(uint32_t) * arr_sz);
    bs->sz = arr_sz;

    return bs;
}

// set a bit given by idx
void bs_set(bs_t *bs, uint32_t idx) {
    // ASSERT(idx < bs->sz * 32);
    if (idx < bs_get_size(bs)) {
        uint32_t arr_idx = idx / 32;
        uint32_t offset = idx % 32;

        bs->bits[arr_idx] |= 0x1 << offset;
    }
}

// clear a bit given by idx
void bs_clr(bs_t *bs, uint32_t idx) {
    // ASSERT(idx < bs->sz * 32);
    if (idx < bs_get_size(bs)) {
        uint32_t arr_idx = idx / 32;
        uint32_t offset = idx % 32;
        uint32_t mask = 0xffffffff ^ (0x1 << offset); // 11110111..111

        bs->bits[arr_idx] &= mask;
    }
}

// return the value of a bit given by idx
uint8_t bs_tst(bs_t *bs, uint32_t idx) {
    // ASSERT(idx < bs->sz * 32);
    if (idx < bs_get_size(bs)) {
        uint32_t arr_idx = idx / 32;
        uint32_t offset = idx % 32;
        uint32_t mask = (0x1 << offset); // 000100..00

        return (bs->bits[arr_idx] & mask) >> offset;
    }

    return 1;
}

// print the bitmap out
void bs_dmp(bs_t *bs) {
    for (uint32_t i = 0; i < bs->sz; i++) {
        for (uint32_t n = 0; n < 32; n++) {
            kprintf("%d ", (bs->bits[i] & (0x1 << n)) >> n);
        }
        kprintf("\n");
    }
}

// get the index of the first clear bit
// if the bit map is entirely full return the sz of the bit map
uint32_t bs_first_clr(bs_t *bs) {
    for (uint32_t i = 0; i < bs->sz; i++)
        for (uint32_t n = 0; n < 32; n++) 
            if (!bs_tst(bs, (i * 32) + n))
                return (i * 32) + n;

    return bs_get_size(bs);
}

uint32_t bs_get_size(bs_t *bs) {
    return bs->sz * 32;
}
