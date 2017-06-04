#include "include/isr.h"
#include "include/vga.h"
#include "include/sys.h"

isr_t handlers[256];

// for irq it should be numbers from 32 - 47
void add_handler(uint8_t n, isr_t handler)
{
	handlers[n] = handler;
}

// This gets called from our ASM interrupt handler.
void isr_handler(registers *regs) {
    set_color(0x4, 0);
    
    if (handlers[regs->int_no]) {
        isr_t handler = handlers[regs->int_no];
        handler(regs);
    }
    else {
        // there is no handler for this interrupt so use general handler
        vga_write("recieved interrupt: ");
        vga_write(itoa(regs->int_no, 16));
        vga_write(itoa(regs->err_code, 16));
        vga_write("\n");
    }
}

void irq_handler(registers *regs) {
	if (regs->int_no >= 40) {
		// if the interrupt came from slave PIC only send EOI to slave
		outport(0xA0, 0x20);
	}
	
	if (handlers[regs->int_no]) {
    	isr_t handler = handlers[regs->int_no];
    	handler(regs);
    }

    // send EOI to master no matter what
   	outport(0x20, 0x20);
}
