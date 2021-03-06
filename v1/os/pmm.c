#include "include/pmm.h"

uint32_t *frame_stack;
uint32_t *top = 0x0;
uint32_t bottom = 0x0;

extern uint32_t placement_addr;

// right here we provide a pointer to the multiboot info, this
// contains a memory map that will tell us which addresses we push to the 
// frame stack and which ones we shouldn't. We also need the location of the kernel
// end so that we dont push any of that memory to the stack
void init_pmm(multiboot_info_t *mbi, uint32_t kern_end) {
    placement_addr = kern_end;
    bottom = kern_end;
    
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
                
                vga_write("Memory Map: \n");
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

void push_frame(uint32_t addr) {
    uint32_t *ptr = (uint32_t*) kmalloc(4); // allocate space for a frame addressi
    *ptr = addr;
    top = ptr;
} 

uint32_t pop_frame() {
    if (top == bottom) {
        vga_write("NO MEMORY");
        return -1;
    }

    uint32_t ret = *top;
    top -= 1;
    return ret;
}
