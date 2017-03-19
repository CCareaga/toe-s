#include "timer.h"
#include "isr.h"
#include "vga.h"

uint32_t tick = 0; // save the current clock tick


// this is going to be called from our irq handler
// it will be added to our hadnler list and indexed using the interrupt number

static void timer_handler(registers* regs)
{
   tick++;
   vga_write("tick: ");
   vga_write(itoa(tick, 10));
   vga_write("\n");
}

// this function will set up our irq handler to be called when we recieve 
// interrupt 32 
// it will also tell the PIT what divisor to use so that it will send and interrupt
// periodically.

void init_timer(uint32_t frequency)
{
   // add our timer handler to the irq handler array irq 0
   add_handler(0, &timer_handler);

   // The value we send to the PIT is the value to divide it's input clock
   // (1193180 Hz) by, to get our required frequency. Important to note is
   // that the divisor must be small enough to fit into 16-bits.
   uint32_t divisor = 1193180 / frequency;

   // Send the command byte.
   outport(0x43, 0x36);

   // Send the frequency divisor.
   outport(0x40, divisor & 0xFF);
   outport(0x40, divisor >> 8);
}