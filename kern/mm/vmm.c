#include "vmm.h"
#include "pmm.h"
#include "kmalloc.h"
#include "klib.h"
#include "kheap.h"

extern uint32_t stack_top;
extern uint32_t stack_bottom;

pg_dir_t *kern_dir;
pg_dir_t *current_dir;

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
            // kprintf("grabbing the first free phys frame 0x%x \n", frame);

            if (frame != 0xffffffff)
                pg->addr = frame / PG_SZ;
            else 
                kprintf("WAIT WUT, there are no more frames?");
        }
    }
}

void unmap_page(page_t *pg) {
    pg->present = 0;
    pg->rw  = 0;
    pg->usr = 0;

    if (pg->addr) {
        free_frame(pg->addr);
        pg->addr = 0;
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
    // int id = r->err_code & 0x10;

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

    // page = get_page(0, 1, kern_dir);
    // page->present = 0;
    set_frame(0);

    kheap_pre_init();

    uint32_t vaddr;
    
    // first we map from 0xC0001000 -> kernel end to the same address minus 0xC0000000
    // P2V and V2P are used to convert between higher half and physical addresses. 
    // we also map one extra page at the end to hold the page table that will be created 
    // for the kernel heap in the next for loop (kind of messy...)
    for (vaddr = P2V(0x0); vaddr <= (uint32_t) kmalloc_get_end() + 0x1000; vaddr += PG_SZ) {
        page = get_page(vaddr, 1, kern_dir);
        map_page(page, 1, 1, V2P(vaddr));
    }

    // map some pages for the kernel heap, these dont have to be identity mapped
    for (vaddr = KHEAP_START; vaddr <= KHEAP_END; vaddr += PG_SZ) {
        page = get_page(vaddr, 1, kern_dir);
        map_page(page, 1, 1, NULL);
    }

    // TEMPORARY: map 4 pages for the relocated stack testing!
    for (vaddr = 0xE0000000; vaddr <= 0xE0004000; vaddr += PG_SZ) {
        page = get_page(vaddr, 1, kern_dir);
        map_page(page, 1, 1, NULL);
    }

    add_int_handler(14, &page_fault);
    switch_page_directory(kern_dir->phys);
    current_dir = kern_dir;

    kheap_init();
}

// this function takes a virtual address and returns the physical
// address it is mapped to, it just looks it up in the page dir
uint32_t get_physical(uint32_t virt, pg_dir_t* pd) {
    if (!pd) pd = current_dir;

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
    page_t *page;

    for (uint32_t i = start; i < start + sz; i += PG_SZ) {
        page = get_page(i, 1, pd);
        map_page(page, 1, 1, NULL);
    }

    return start;
}

// this function is used to relocate the stack, I need to do this in order
// to ensure that when I fork the new process has its own kernel stack.
// if we dont do this the new process will have a copy of its parent stack
void relocate_stack(uint32_t *bottom, uint32_t size) {
    
    allocate_stack((uint32_t) bottom, size, current_dir);

    uint32_t stack_sz = (char *) &stack_top - (char *) &stack_bottom;
    uint32_t offset = (uint32_t) ((char *) bottom - (uint32_t) &stack_bottom);
    
    // copy the old stack to our new area
    memcpy(&stack_bottom, bottom, stack_sz);

    uint32_t *new_top = (uint32_t *) ((char *) bottom + stack_sz);

    // iterate through the entire stack changing return addresses
    for (uint32_t *i = bottom; i < new_top; i++) {
        if (*i < (uint32_t) &stack_top && *i > (uint32_t) &stack_bottom) {
            *i += offset;
        }
    }

    // load the new stack into stack registers
    uint32_t old_esp, new_esp; 
    uint32_t old_ebp, new_ebp;  

    asm volatile("mov %%esp, %0" : "=r" (old_esp));
    asm volatile("mov %%ebp, %0" : "=r" (old_ebp));

    new_esp = old_esp + offset;
    new_ebp = old_ebp + offset;

    asm volatile("mov %0, %%esp" : : "r" (new_esp));
    asm volatile("mov %0, %%ebp" : : "r" (new_ebp));
}

void copy_page(pg_dir_t *dir, uint32_t *virt, uint32_t *buffer) {
    // switch to the new page directory
    switch_page_directory(dir->phys);
    
    // get the page and create a new table if need be
    // then get it a physical address, then copy the data
    page_t *page = get_page((uint32_t) virt, 1, dir);
    map_page(page, 1, 1, NULL);
    memcpy(buffer, virt, PG_SZ);

    // switch back to the new one
    switch_page_directory(current_dir->phys);
}

pg_dir_t *copy_pg_dir(pg_dir_t *dir) {

    uint32_t phys;
    pg_dir_t *new = kmalloc_ap(sizeof(pg_dir_t), &phys);
    memset(new, 0, sizeof(pg_dir_t));
    new->phys = phys;

    uint32_t *copy_buf = kmalloc(PG_SZ);

    // this will map the kernel, this part of the new directory
    // is mapped to the same physical addresses as the kernel, this is
    // so that every page directory has the kernel mapped in
    for (uint32_t i = 768; i < 1023; i++) {
        pg_tab_t *table = dir->tables[i];
        
        if (table) {
            new->tables[i] = table;
            memcpy(dir->phys_tabs, new->phys_tabs, sizeof(uintptr_t) * 1024);
        }
    }
    
    // now we copy anything from 0x0 up to the bottom of the kernel
    // we have to go through each page and copy the contents to a new
    // physical frame so that the new page dir has a copy...
    for (uint32_t i = 0; i < 768; i++) {
        pg_tab_t *table = dir->tables[i];
        
        if (table) {
            for (uint32_t p = 0; p < 1023; p++) {
                if (table->pages[p].addr) { 
                    // this page is actually mapped in so we copy it to a buffer
                    // in the kernel (already mapped) so we can copy to the new dir
                    uint32_t *pg_addr = IDX2PG(i, p);
                    memcpy(pg_addr, copy_buf, PG_SZ);
                    copy_page(new, pg_addr, copy_buf);
                }
            }
        }
    }

    return new;
}

