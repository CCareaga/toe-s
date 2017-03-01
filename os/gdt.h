#ifndef GDT_H
#define GDT_H

struct gdt_entry
{
   u16int l_low;
   u16int b_low;
   u8int  b_middle;
   u8int  acc;
   u8int  gran;
   u8int  b_high;
} __attribute__((packed));
typedef struct gdt_entry gdt_e;

// 48 bit structure we give to lgdt to install our gdt
struct gdt_ptr
{
   u16int limit;               
   u32int base;                
}__attribute__((packed));
typedef struct gdt_ptr gdt_p;

void init_gdt();

#endif