#include <stdio.h>
#include "bs.h"

int main() {
    // create bitset of 100 bits
    bs_t *bs = bs_init(100);

    bs_set(bs, 9);
    bs_set(bs, 12);
    bs_set(bs, 35);
    bs_dump(bs);

    printf("\n");

    bs_clr(bs, 12);
    bs_dump(bs);

    printf("%d \n", bs_tst(bs, 9));
    printf("%d \n", bs_tst(bs, 12));
}
