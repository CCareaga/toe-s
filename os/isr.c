#include "isr.h"
#include "vga.h"
#include "sys.h"

// This gets called from our ASM interrupt handler.
void isr_handler(registers regs)
{
   vga_write("recieved interrupt: ");
   vga_write(itoa(regs.int_no, 10));
}