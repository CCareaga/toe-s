#ifndef PAGING_H
#define PAGING_H

// paging related definitions
// 
// each page directory consists of 1024 entries, each entry consists
// of 
// virtual address composition
// ========10=================10==================12==========
// |                 |                  |                    |
// | page dir. index | page table index | offset within page | 
// |                 |                  |                    |
// ===========================================================
//      PDX(va)            PTX(va)

// 0x3ff -> 0b00...1111111111 (10 bits)
#define PDX(va) (((uint32_t)(va) >> PDXSHIFT) & 0x3FF)
#define PTX(va) (((uint32_t)(va) >> PTXSHIFT) & 0x3FF)
    
#define PAGERUP(a) (((a) + PAGE_SZ - 1) & ~(PAGE_SZ - 1))
#define PAGERDOWN(a) ((a) & ~(PAGE_SZ - 1))

#define PAGE_SZ 4096
#define NUMPDE  1024
#define NUMPTE  1024

#define PGSHIFT     12
#define PTXSHIFT    12
#define PDXSHIFT    22

// page table flags
#define PTE_P 0x001
#define PTE_W 0x002
#define PTE_U 0x004

typedef struct page_table {
    uint32_t pages[1024];
} page_table_t;

typedef struct page_dir {
    page_table_t *tables[1024];
} page_dir_t;

#endif
