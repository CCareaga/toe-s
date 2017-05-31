#include "include/pmm.h"

extern void load_page_directory(uint32_t page_directory);
extern void enable_paging();

uint32_t *frame_stack;
uint32_t top = 0x0;

page_directory_t *page_dir;

// right here we provide a pointer to the multiboot info, this
// contains a memory map that will tell us which addresses we push to the 
// frame stack and which ones we shouldn't. We also need the location of the kernel
// end so that we dont push any of that memory to the stack
void init_pmm(multiboot_info_t *mbi, uint32_t kern_end) {
	if(mbi->flags & (1 << 6)) {
		
		uint32_t maddr = mbi->mmap_addr;
   		uint32_t mlen = mbi->mmap_length;
		multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
		
		uint32_t alloc_start, alloc_end;
      
		while(mmap < (maddr + mlen)) {
			mmap = (multiboot_memory_map_t *) ((uint32_t) mmap + mmap->size + sizeof(uint32_t));
			alloc_start = mmap->addr;
			alloc_end = mmap->addr + mmap->len;

	        if (mmap->type == 1) {
	        	// dont allocate any memory below 1 MiB
	        	if (mmap->addr < 0x100000) continue;

	        	if (alloc_start % 0x1000 != 0) {
	        		// start of region is not page align, round up
	        		alloc_start += (0x1000 - alloc_start % 0x1000);
	        	}

	        	if (alloc_end % 0x1000 != 0) {
	        		// end of region is not page aligned, round down
	        		alloc_end -= alloc_end % 0x1000;
	        	}
	        	vga_write("START: ");
	        	vga_write(itoa(alloc_start, 16));
	        	vga_write(" END: ");
	        	vga_write(itoa(alloc_end, 16));
	        	vga_write("\n");

	        	uint32_t curr = alloc_start;

	        	while(curr < (alloc_end - 0x1000)) {
	        		if (curr > kern_end) {  // dont overwrite kernel!!!
	        		    push_frame(curr);
                    }
	        		curr += 0x1000;
	        	}

	        }
		}
	}
}

// this method rights the address of the last frame to the beginning
// of our newest frame. Then sets the new address to our current top

void push_frame(uint32_t addr) {
	uint32_t *page_start = (uint32_t *) addr; 	// create pointer to page start
	
	*page_start = top; 	// assign our old top to it
	top = addr;			// set our current top to this new address
} 

// this methid will pop a new frame off the stack. it will grab the address
// stored in top, reassign top to our next add

uint32_t pop_frame() {
	if (top == 0x0) { // there is no memory left, for now just panic
		vga_write("NO MEMORY");
		return -1;
	}

	uint32_t ret = top; // save our current top (value to return)
	top = *(uint32_t *) top; 	// top is now the value that was at address top (next frame)
	return ret; 		// return the value we saved
}

void allocate_page(page_t *page, uint32_t kern, uint32_t writeable) {

    if (page->frame) {
        return; // this page already has a frame!
    }

    uint32_t addr = pop_frame(); // get a new frame from top of stack

    page->present = 1;              // set the present bit of the page
    page->rw = (writeable) ? 1:0;   // is this a writeable page
    page->user = (kern) ? 0:1;      // is this a user page?
    page->frame = addr >> 12;        // frame address is multiple of 0x1000 so shift 3 bytes
}

void free_page(page_t *page) {
    uint32_t addr = page->frame << 12;

    if (!addr) {
        return; // this page had no frame allocated...
    }

    push_frame(addr);
    page->frame = 0x0;
}

void init_paging() {
    size_t i, j;
    uint32_t addr = 0x0;
    
    page_table_t *page_tab;

    page_dir = (page_directory_t*) pop_frame();
    memset(page_dir, 0, 0x1000);

    for (i = 0; i < 1024; i++) {
        page_tab = (page_table_t*) pop_frame();
        memset(page_tab, 0, 0x1000);

        for (j = 0; j < 1024; j++) { // fill page table
            page_tab->pages[j].frame = addr >> 12;
            page_tab->pages[j].present = 1;
            page_tab->pages[j].rw = 1;
            page_tab->pages[j].user = 0;
            addr += 0x1000;
            // allocate_page((page_t*) &page_tab->pages[j], 1, 1); // kernel level, writeable 
        }
        page_dir->tables[i] = (uint32_t) page_tab | 3;
    }

    load_page_directory((uint32_t) page_dir);    
    enable_paging();
}

void page_fault(registers *regs) {
    uint32_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

    int present   = !(regs->err_code & 0x1); 
    int rw = regs->err_code & 0x2;
    int us = regs->err_code & 0x4; 
    int reserved = regs->err_code & 0x8;
    int id = regs->err_code & 0x10;

    vga_write("Page fault! ( ");
    if (present) vga_write("present ");
    if (rw) vga_write("read-only ");
    if (us) vga_write("user-mode ");
    if (reserved) vga_write("reserved ");
    vga_write(") at 0x");
    vga_write(itoa(faulting_address, 16));
    vga_write("\n");
}
