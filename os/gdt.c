#include "sys.h"
#include "gdt.h"
#include "vga.h"

gdt_e segments[5];
gdt_p gdt_ptr;

extern void gdt_install(u32int);

static void create_segment(s32int, u32int, u32int, u8int, u8int);

void init_gdt() {
	// array of the different access bytes:
	// first two are for kernel code and data (ring 0)
	// second two are for user space code and data (ring 3)
	u8int acc_bytes[4] = {0x9A, 0x92, 0xFA, 0xF2};

	// size of the entry times length of table (5) - 1
	gdt_ptr.limit = (sizeof(gdt_e) * 5) - 1;
	// pointer to the start of the table
	gdt_ptr.base = (u32int) &segments;

	create_segment(0, 0, 0, 0, 0); //null entry

	for (int i = 0; i < 4; i++) {
		create_segment(i+1, 0, 0xFFFFFFFF, acc_bytes[i], 0xCF);
	}

	// pass the gdt pointer to our assembly routine to install it
	gdt_install((u32int) &gdt_ptr);
}

static void create_segment(s32int index, u32int base, u32int lim, u8int acc, u8int gran) {

	segments[index].b_low    = (base & 0xFFFF);
   	segments[index].b_middle = (base >> 16) & 0xFF;
   	segments[index].b_high   = (base >> 24) & 0xFF;

   	segments[index].l_low   = (lim & 0xFFFF);
   	segments[index].gran = (lim >> 16) & 0x0F;

   	segments[index].gran |= gran & 0xF0;
   	segments[index].acc      = acc;
}
