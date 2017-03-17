#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include "sys.h"
#include "vga.h"

// structure of an entry in the IDT:
// low: low 16 bits of instructions to execute
// sel: the kernel segment selector for this entry
// zer0: unused always set to 0
// flags: type and attribute of the IDT entry

struct idt_struct
{
   uint16_t low;
   uint16_t  sel;
   uint8_t  zero;
   uint8_t  flags;
   uint16_t high;     
};

typedef struct idt_struct idt_entry;

// this is the struct we use to load the IDT the same way we
// did with the gdt except using lidt instruction

struct idt_ptr_struct
{
   uint16_t limit;
   uint32_t base;
} __attribute__((packed));
// this struct is 48 bits and not naturally aligned so we use packed
typedef struct idt_ptr_struct idt_ptr;

void init_idt();

// now interrupt numbers are represented using 1 byte hex values therefore there
// are 256 possible interrupts so we have to define entries for all of our possible interrupts..

// there are 32 CPU-dedicated interrupts that we definitely need entries for. (the first 32)

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

#endif