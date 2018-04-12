#include "x86.h"
#include "klib.h"
#include "system.h"

static handler_t handlers[256] = {0};

static const char *trap_messages[32] = {
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Detected overflow",
    "Out-of-bounds",
    "Invalid opcode",
    "No coprocessor",
    "Double fault",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown interrupt",
    "Coprocessor fault",
    "Alignment check",
    "Machine check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

// registers an interrupt handler for a certain idt index
void add_int_handler(uint8_t n, handler_t handler) {
    handlers[n] = handler;
}

void isr_handler(regs_t *r) {
    uint32_t n = r->int_no;
    
    // we have a handler for this int no, so run it
    // otherwise just print out some info
    if (handlers[n])
        handlers[n](r);

    else 
        kprintf("recieved trap %x (%d) -> %s \n", n, n, trap_messages[n]);


    // if the interrupt came from slave also send it the ack
    if (n >= 40) 
        outport(0xA0, 0x20);

    // if int is from master PIC send it the ACK
    if (n >= 32)
        outport(0x20, 0x20);

}

void irq_handler(regs_t *r) {
    uint32_t n = r->int_no;
    
    // we have a handler for this int no, so run it
    // otherwise just print out some info
    if (handlers[n])
        handlers[n](r);

    else 
        kprintf("recieved hardware int %x (%d) \n", n, n);


    // if the interrupt came from slave also send it the ACK
    if (n >= 40) 
        outport(0xA0, 0x20);
    
    // always send master the ACK
    outport(0x20, 0x20);

}

