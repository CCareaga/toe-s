#include "timer.h"
#include "klib.h"
#include "system.h"
#include "x86.h"

#define FREQ 1
#define CLOCK_FREQ 1193180

static void timer_handler(regs_t *r) {
    // kprintf("clock update \n");
    
}

void timer_init() {
    add_int_handler(32, &timer_handler); 

    uint32_t div = CLOCK_FREQ / FREQ;

    outport(0x43, 0x36);

    outport(0x40, div & 0xFF);
    outport(0x40, div >> 8);
}


