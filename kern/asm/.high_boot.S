
.set MB_MAGIC,              0x1BADB002
.set MB_FLAG_PAGE_ALIGN,    1 << 0
.set MB_FLAG_MEMORY_INFO,   1 << 1
.set MB_FLAGS,              MB_FLAG_PAGE_ALIGN | MB_FLAG_MEMORY_INFO
.set MB_CHECKSUM,           -(MB_MAGIC + MB_FLAGS)

.set KERN_VBASE, 0xC0000000
.set KERN_PG_NUM, KERN_VBASE >> 22

.section .data
.align 0x1000

boot_pd:
    .int 0x00000083

    .skip (KERN_PG_NUM - 1) * 4

    .int 0x00000083

    .skip (1024 - KERN_PG_NUM - 1) * 4

/* .stack resides in .bss */
.section .bss, "aw", @nobits
stack_bottom:
    .skip 32768 /* 32KiB */
stack_top:

.section .text
.align 4

/* Multiboot section */
.long MB_MAGIC
.long MB_FLAGS
.long MB_CHECKSUM
.long 0x00000000 /* header_addr */
.long 0x00000000 /* load_addr */
.long 0x00000000 /* load_end_addr */
.long 0x00000000 /* bss_end_addr */
.long 0x00000000 /* entry_addr */

.long 0x00000000
.long 0
.long 0
.long 32

.set start, (_start - 0xC0000000)
.global start
.type start, @function

.extern kmain
.type kmain, @function

_start:
    movl (boot_pd - KERN_VBASE), %ecx
    movl %ecx, %cr3

    movl %cr4, %ecx
    orl $0x00000010, %ecx
    movl %ecx, %cr4

    movl %cr0, %ecx
    orl $0x80000000, %ecx
    movl %ecx, %cr0

    movl 1, %ecx

    jmpl higher_half

higher_half:
    /* Setup our stack */
    mov $stack_top, %esp

    /* Make sure our stack is 16-byte aligned */
    and $-16, %esp

    pushl %esp
    pushl %eax /* Multiboot header magic */
    pushl %ebx /* Multiboot header pointer */

    /* Disable interrupts and call kernel proper */
    cli
    call kmain

    /* Clear interrupts and hang if we return from kmain */
    cli

hang:
    hlt
    jmp hang
