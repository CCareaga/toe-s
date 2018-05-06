#include "sched.h"
#include "klib.h"

void kidle() {
    while (1) {
        kprintf("idling! ");
        asm volatile ("hlt");
    }
}

void test_task() {
    int i = 0;
    while (1) {
        if (i == 100000000) {
            kprintf(":-) ");
            i = 0;
        }

        i++;
    }
}
