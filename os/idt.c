#include "idt.h"

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
    //print_hex(idt_p.limit);
    // this sucks but for obvious reasons a for loop cannot be used :-(
    // 0x08 is the kernel code selector in our GDT and 0x8E represents
    // a 32-bit interrupt gate with 0-ring priv.

    vga_write("ugh");

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

   	// send the pointer of our idt descriptor struct to loading routine (dt.asm)
    //mem_print(&idt_p, 32);
    idt_install( (uint32_t) &idt_p);
}

static void add_entry(uint8_t vec, uint32_t add, uint16_t sel, uint8_t flags) {
	  entries[vec].low 	  = add & 0xFFFF; // grab the low 16 bits
   	entries[vec].high   = (add >> 16) & 0xFFFF; // bit shift to the right 16 bits and grab the low 16 bits
    entries[vec].sel    = sel;
   	entries[vec].zero 	= 0;
   	entries[vec].flags  = flags;
}