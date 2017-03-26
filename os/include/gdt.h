#ifndef GDT_H
#define GDT_H

#include <stdint.h>
#include "vga.h"
#include "sys.h"

struct gdt_entry
{
   uint16_t l_low;
   uint16_t b_low;
   uint8_t  b_middle;
   uint8_t  acc;
   uint8_t  gran;
   uint8_t  b_high;
} __attribute__((packed));
typedef struct gdt_entry gdt_e;

// 48 bit structure we give to lgdt to install our gdt
struct gdt_ptr
{
   uint16_t limit;               
   uint32_t base;                
}__attribute__((packed));
typedef struct gdt_ptr gdt_p;

void init_gdt();

#endif