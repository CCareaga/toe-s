#include "timer.h"
#include "klib.h"
#include "system.h"
#include "x86.h"
#include "sched.h"

#define FREQ 10000
#define CLOCK_FREQ 1193180

uint32_t ticks = 0;

static void timer_handler(regs_t *r) {
    ticks++;

    if (ticks % (2 * FREQ) == 0) {
        ticks = 0;
        schedule();
    }
}

void timer_init() {
    add_int_handler(32, &timer_handler); 

    uint32_t div = CLOCK_FREQ / FREQ;

    outport(0x43, 0x36);
    
    outport(0x40, (div & 0xFF));
    outport(0x40, ((div >> 8) & 0xFF));
}


