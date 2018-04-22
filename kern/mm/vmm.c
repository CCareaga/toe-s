#include "vmm.h"
#include "pmm.h"
#include "kmalloc.h"
#include "klib.h"
#include "kheap.h"

pg_dir_t *kern_dir;

extern void higher_half(int);

// this function fetches a page addr using the provided virtual 
// address and page directory. if the table that holds the page addr
// doesn't exist and the create flag is set, we allocate a new table
page_t *get_page(uint32_t addr, uint8_t create, pg_dir_t *pg_dir) {
    page_t *ret = 0;
    uint32_t pd_idx = PD_IDX(addr);
    uint32_t pt_idx = PT_IDX(addr);

    if (pg_dir->tables[pd_idx]) { // is there a table at this index
        ret = &pg_dir->tables[pd_idx]->pages[pt_idx];
    }
    else if (create) {
        uint32_t phys;

        pg_dir->tables[pd_idx] = kmalloc_ap(sizeof(pg_tab_t), &phys);
        memset(pg_dir->tables[pd_idx], 0, sizeof(pg_tab_t));

        pg_dir->phys_tabs[pd_idx] = phys | PDE_P | PDE_W | PDE_U;

        kprintf("creating a new page table \n");

        ret = &pg_dir->tables[pd_idx]->pages[pt_idx];
    }

    return ret;
}

// this function maps in a page, basically it grabs the first free frame
// and gives it to the page, it also sets the page attributes
// if address is provided the page is mapped to that physical address
void map_page(page_t *pg, uint8_t usr, uint8_t write, uint32_t addr) {
    pg->present = 1;
    pg->rw  = write;
    pg->usr = !usr;
    
    if (pg->addr == 0) {
        if (addr) {
            uint32_t success = set_frame(addr); 
            if (success)
                pg->addr = addr / PG_SZ;
            else {
                pg->present = 0;
                kprintf("could not map 0x%x, out of physical range \n", addr);
            }
        }

        else {
            uint32_t frame = get_frame();
            kprintf("grabbing the first free phys frame 0x%x \n", frame);

            if (frame != 0xffffffff)
                pg->addr = frame / PG_SZ;
            else 
                kprintf("WAIT WUT, there are no more frames?");
        }
    }
}

// function to switch to the provided directory
void switch_page_directory(uintptr_t dir) {
    asm volatile("mov %0, %%cr3":: "r"(dir));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

void page_fault(regs_t *r) {
    uint32_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

    int present   = !(r->err_code & 0x1); 
    int rw = r->err_code & 0x2;
    int us = r->err_code & 0x4; 
    int reserved = r->err_code & 0x8;
    int id = r->err_code & 0x10;

    kprintf("Page fault! ( ");
    if (present) kprintf("present ");
    if (rw) kprintf("read-only ");
    if (us) kprintf("user-mode ");
    if (reserved) kprintf("reserved ");
    kprintf(") at 0x%x", faulting_address);
    kprintf("\n");
    // halt so that we dont trigger endless page faults.

    asm volatile ("hlt");
}

// initialize virtual memory, for now just create a page dir
// identity map the kernel, and add the page fault handler
void vmm_init() {
    page_t *page;
    // first we need to allocate a page for our kernels page dir
    // we don't allocate any tables because that happens only when
    // we begin mapping pages, for now all the entires are zero

    uint32_t phys;
    kern_dir = kmalloc_ap(sizeof(pg_dir_t), &phys);
    memset(kern_dir, 0, sizeof(pg_dir_t));
    kern_dir->phys = phys;

    page = get_page(0, 1, kern_dir);
    page->present = 0;
    set_frame(0);

    kheap_pre_init();

    uint32_t vaddr = 0x1000;

    // identity map the from 0 all the way to the end of the dummy kernel heap since we 
    // also need to map in the real heap we map identity map in one page after the 
    // dummy heap ptr. then when we start mapping the heap there is already a spot
    // for the heaps page table... sort of confusing
    for (vaddr = 0x1000; vaddr <= (uint32_t) kmalloc_get_end() + 0x1000; vaddr += PG_SZ) {
        page = get_page(vaddr, 1, kern_dir);
        map_page(page, 1, 1, vaddr);
#if HIGHER_HALF
        page = get_page(vaddr + HIGHER_HALF_START, 1, kern_dir);
        map_page(page, 1, 1, vaddr);
#endif
    }

    // map some pages for the kernel heap, these dont have to be identity mapped
    for (vaddr = KHEAP_START; vaddr <= KHEAP_END; vaddr += PG_SZ) {
        page = get_page(vaddr, 1, kern_dir);
        map_page(page, 1, 1, NULL);
    }

    add_int_handler(14, &page_fault);
    switch_page_directory(kern_dir->phys);

    kheap_init();
}

// this function takes a virtual address and returns the physical
// address it is mapped to, it just looks it up in the page dir
uint32_t get_physical(uint32_t virt, pg_dir_t* pd) {
    uint32_t pd_idx = PD_IDX(virt);
    uint32_t pt_idx = PT_IDX(virt);
    uint32_t offset = virt % PG_SZ;
    uint32_t ret = 0;

    if (kern_dir->tables[pd_idx]) {
        page_t *page = &pd->tables[pd_idx]->pages[pt_idx];
        ret = ((uint32_t) page->addr * PG_SZ) + offset;
    }

    return ret;
}

// allocates some space for a stack by mappin' in some pages
uint32_t allocate_stack(uint32_t start, uint32_t sz, pg_dir_t* pd) {
    for (int i = start; i < start + sz; i += PG_SZ) {
        page = get_page(vaddr, 1, pd);
        map_page(page, 1, 1, vaddr);
    }
}

