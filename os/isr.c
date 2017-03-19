#include "isr.h"
#include "vga.h"
#include "sys.h"

isr_t handlers[256];

// for irq it should be numbers from 32 - 47
void add_handler(uint8_t n, isr_t handler)
{
	handlers[n] = handler;
}

// This gets called from our ASM interrupt handler.
void isr_handler(registers *regs) {
	set_color(0x4, 0);
   	vga_write("recieved interrupt: ");
   	vga_write(itoa(regs->int_no, 10));
   	vga_write("\n");
}

void irq_handler(registers *regs) {

	if (regs->int_no >= 40) {
		// if the interrupt came from slave PIC only send EOI to slave
		outport(0xA0, 0x20);
	}
	
	if (handlers[regs->int_no - 32] != 0) {

    	isr_t handler = handlers[regs->int_no - 32];
    	handler(regs);
    }

    // send EOI to master no matter what
   	outport(0x20, 0x20);
}