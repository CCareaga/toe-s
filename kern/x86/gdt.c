#include "x86.h"

// this file implments the setting up of the global desciptor table (GDT)
// segmentation using the gdt is x86 specific and not going to really be used
// we want a flat address space so we set all segments from 0x0 -> 0xffffffff
// the processor will add the base and offset of an address except the base is
// 0x0 so the address is just the offset itself. 
// 
// Even though the segments cover the entire address space we still need 
// segments for both data and code, and we need segments for both kernel
// and user. so we have 4 segments in total (also intel makes us add a zero
// segment as the first entry in the gdt)

typedef struct gdt_e {
    uint16_t    lim_low;
    uint16_t    base_low;
    uint8_t     base_mid;
    uint8_t     access;
    uint8_t     gran;
    uint8_t     base_high;
} __attribute__((packed)) gdt_e;


static gdt_e gdt[5];
static dt_p gdt_ptr;

extern void gdt_install(uint32_t);

static void gdt_set_entry(uint8_t idx, uint64_t base, uint64_t lim, uint8_t acc, uint8_t gran) {
    gdt[idx].base_low  = (base & 0xFFFF);
    gdt[idx].base_mid  = ((base >> 16) & 0xFF); 
    gdt[idx].base_high = ((base >> 24) & 0xFF); 

    gdt[idx].lim_low   = (lim & 0xFFFF);        
    gdt[idx].gran      = (lim >> 16) & 0x0f;
    gdt[idx].gran     |= (gran & 0xF0);

    gdt[idx].access    = acc;
}

void gdt_init() {
    gdt_ptr.base = (uint32_t) &gdt;
    gdt_ptr.lim = (sizeof(gdt_e) * 5) - 1;

    uint8_t acc_bytes[5] = {0x0, 0x9A, 0x92, 0xFA, 0xF2};
    
    gdt_set_entry(0, 0, 0, 0, 0);

    for (int i = 1; i < 5; i++)
        gdt_set_entry(i, 0, 0xFFFFFFFF, acc_bytes[i], 0xCF);
    
    gdt_install((uint32_t) &gdt_ptr);
}
