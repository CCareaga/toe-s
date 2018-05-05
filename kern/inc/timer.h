#ifndef TIMER_H
#define TIMER_H

#include "types.h"
#include "x86.h"

void timer_init();
void timer_handler(regs_t *);

#endif
