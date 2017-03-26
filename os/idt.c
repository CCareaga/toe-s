#include "include/idt.h"
#include "include/sys.h"

extern void idt_install(uint32_t);

static void add_entry(uint8_t, uint32_t, uint16_t, uint8_t);

// create an array of 256 idt entries, (many will be zero!)
idt_entry entries[256];

// create idt pointer struct to tell cpu where it is and its size
idt_ptr   idt_p;

void init_idt()
{
   	idt_p.limit = sizeof(idt_entry) * 256 - 1; // size
   	idt_p.base  = (uint32_t) &entries;		 // pointer to the start of entry array

    memset(&entries, 0, sizeof(idt_entry)*256); // set all entries to 0

    // when bios starts master PIC irq's are mapped 0x8-0xf this interferes
    // with the interrupts used by the CPU for exceptions and faults
    // irq 0-7 is mapped to 0x8-0xf and irq 8-15 is mapped to int 0x70 - 0x77
    // we remap these irq's to trigger interrupt 32-47

    // 0x20 is the I/O base address for master PIC
    // 0xA0 is the I/O base for the slave PIC

    // to access the data in we use 0x21 and 0xA1
 
    // starts the initialization sequence (in cascade mode)
    outport(0x20, 0x11);
    outport(0xA0, 0x11);
    
    // set offset of the master PIC to 0x20 (decimal 32)
    outport(0x21, 0x20);

    // set offset of the slave PIC to 0x28 (decimal 40)
    outport(0xA1, 0x28);

    // tell  master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outport(0x21, 0x04);

    // tell slave its cascade identity (0000 0010)
    outport(0xA1, 0x02);

    // 8086/88 (MCS-80/85) mode 
    outport(0x21, 0x01);
    outport(0xA1, 0x01);

    outport(0x21, 0x0);
    outport(0xA1, 0x0);

    // this sucks but for obvious reasons a for loop cannot be used :-(
    // 0x08 is the kernel code selector in our GDT and 0x8E represents
    // a 32-bit interrupt gate with 0-ring priv.

    add_entry(0, (uint32_t)isr0, 0x08, 0x8E);
    add_entry(1, (uint32_t)isr1, 0x08, 0x8E);
    add_entry(2, (uint32_t)isr2, 0x08, 0x8E);
    add_entry(3, (uint32_t)isr3, 0x08, 0x8E);
    add_entry(4, (uint32_t)isr4, 0x08, 0x8E);
    add_entry(5, (uint32_t)isr5, 0x08, 0x8E);
    add_entry(6, (uint32_t)isr6, 0x08, 0x8E);
    add_entry(7, (uint32_t)isr7, 0x08, 0x8E);
    add_entry(8, (uint32_t)isr8, 0x08, 0x8E);
    add_entry(9, (uint32_t)isr9, 0x08, 0x8E);
    add_entry(10, (uint32_t)isr10, 0x08, 0x8E);
    add_entry(11, (uint32_t)isr11, 0x08, 0x8E);
    add_entry(12, (uint32_t)isr12, 0x08, 0x8E);
    add_entry(13, (uint32_t)isr13, 0x08, 0x8E);
    add_entry(14, (uint32_t)isr14, 0x08, 0x8E);
    add_entry(15, (uint32_t)isr15, 0x08, 0x8E);
    add_entry(16, (uint32_t)isr16, 0x08, 0x8E);
   	add_entry(17, (uint32_t)isr17, 0x08, 0x8E);
   	add_entry(18, (uint32_t)isr18, 0x08, 0x8E);
   	add_entry(19, (uint32_t)isr19, 0x08, 0x8E);
   	add_entry(20, (uint32_t)isr20, 0x08, 0x8E);
   	add_entry(21, (uint32_t)isr21, 0x08, 0x8E);
   	add_entry(22, (uint32_t)isr22, 0x08, 0x8E);
   	add_entry(23, (uint32_t)isr23, 0x08, 0x8E);
   	add_entry(24, (uint32_t)isr24, 0x08, 0x8E);
   	add_entry(25, (uint32_t)isr25, 0x08, 0x8E);
   	add_entry(26, (uint32_t)isr26, 0x08, 0x8E);
   	add_entry(27, (uint32_t)isr27, 0x08, 0x8E);
   	add_entry(28, (uint32_t)isr28, 0x08, 0x8E);
   	add_entry(29, (uint32_t)isr29, 0x08, 0x8E);
   	add_entry(30, (uint32_t)isr30, 0x08, 0x8E);
   	add_entry(31, (uint32_t)isr31, 0x08, 0x8E);

    // add entries for our IRQ's!
    add_entry(32, (uint32_t)irq0, 0x08, 0x8E);
    add_entry(33, (uint32_t)irq1, 0x08, 0x8E);
    add_entry(34, (uint32_t)irq2, 0x08, 0x8E);
    add_entry(35, (uint32_t)irq3, 0x08, 0x8E);
    add_entry(36, (uint32_t)irq4, 0x08, 0x8E);
    add_entry(37, (uint32_t)irq5, 0x08, 0x8E);
    add_entry(38, (uint32_t)irq6, 0x08, 0x8E);
    add_entry(39, (uint32_t)irq7, 0x08, 0x8E);
    add_entry(40, (uint32_t)irq8, 0x08, 0x8E);
    add_entry(41, (uint32_t)irq9, 0x08, 0x8E);
    add_entry(42, (uint32_t)irq10, 0x08, 0x8E);
    add_entry(43, (uint32_t)irq11, 0x08, 0x8E);
    add_entry(44, (uint32_t)irq12, 0x08, 0x8E);
    add_entry(45, (uint32_t)irq13, 0x08, 0x8E);
    add_entry(46, (uint32_t)irq14, 0x08, 0x8E);
    add_entry(47, (uint32_t)irq15, 0x08, 0x8E);

   	// send the pointer of our idt descriptor struct to loading routine (dt.asm)

    idt_install( (uint32_t) &idt_p);
}

static void add_entry(uint8_t vec, uint32_t add, uint16_t sel, uint8_t flags) {
	entries[vec].low 	= add & 0xFFFF; // grab the low 16 bits
    entries[vec].high   = (add >> 16) & 0xFFFF; // bit shift to the right 16 bits and grab the low 16 bits
    entries[vec].sel    = sel;
   	entries[vec].zero 	= 0;
   	entries[vec].flags  = flags;
}