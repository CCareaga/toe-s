#ifndef _X86_H
#define _X86_H

#include "types.h"

#define PG_ROUND_UP(x)  ((x & 0xfffff000) + 0x1000)
#define PG_SZ           0x1000

#define PD_IDX(va)      (((uint32_t)(va) >> 22) & 0x3FF)
#define PT_IDX(va)      (((uint32_t)(va) >> 12) & 0x3FF)

#define PDE_P           0x1
#define PDE_W           0x2
#define PDE_U           0x4

#define HIGHER_HALF         0x1
#define HIGHER_HALF_START   0xC0000000

typedef struct dt_p {
    uint16_t  lim;
    uint32_t base;
} __attribute__((packed)) dt_p;

typedef struct reg_struct {
    uint32_t gs, fs, es, ds;                            // segment selectors pushed in common
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;    // pushed by the pusha inst
    uint32_t int_no, err_code;                          // pushed by the isr or irq common
    uint32_t eip, cs, eflags, useresp, ss;              // pushed by processor
} regs_t;

typedef struct page {
    uint32_t present : 1;           // set if the page is present in memory
    uint32_t rw      : 1;           // set: writeable, not set: read-only
    uint32_t usr     : 1;           // set: user-mode, not set: kernel mode
    uint32_t access  : 1;           // set if the page has been accessed
    uint32_t dirty   : 1;           // set if the page has been written to
    uint32_t unused  : 7;           // reserved bits
    uint32_t addr    : 20;          // upper 20 bits of physical frame addr
} __attribute__((packed)) page_t;

typedef struct page_tab {
    page_t pages[1024];             // 1024 page table entries
} pg_tab_t;

typedef struct page_dir { 
    uintptr_t   phys_tabs[1024];    // physical addresses of the page tables for the cpu
    pg_tab_t *tables[1024];         // the possibly virtual addresses of the tables
    uintptr_t   phys;               // physical address of this page directory itself
} pg_dir_t;

typedef struct tregs {
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} tregs_t;

typedef void (*handler_t)(regs_t *);

void idt_init();
void gdt_init();
void add_int_handler(uint8_t n, handler_t handler);

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

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

#endif
