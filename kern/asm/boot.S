
.set MB_MAGIC,              0x1BADB002
.set MB_FLAG_PAGE_ALIGN,    1 << 0
.set MB_FLAG_MEMORY_INFO,   1 << 1
.set MB_FLAGS,              MB_FLAG_PAGE_ALIGN | MB_FLAG_MEMORY_INFO
.set MB_CHECKSUM,           -(MB_MAGIC + MB_FLAGS)

.set KERN_VBASE, 0xC0000000
.set KERN_PG_NUM, (KERN_VBASE >> 22)

.section .data
.align 0x1000
.global boot_pd
boot_pd:
    .long 0x00000083
    .fill (KERN_PG_NUM - 1), 4, 0x000000000
    .long 0x00000083
    .fill (1024 - KERN_PG_NUM - 1), 4, 0x00000000

/* .stack resides in .bss */
.section .bss, "aw", @nobits
.align 0x1000
stack_bottom:
    .skip 16384 /* 16KiB */
stack_top:
.global stack_bottom
.global stack_top

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

.set start, (_start - KERN_VBASE)
.global start
.type start, @function

.extern kmain
.type kmain, @function

_start:
    mov $(boot_pd - KERN_VBASE), %ecx
    mov %ecx, %cr3

    mov %cr4, %ecx
    or $0x00000010, %ecx
    mov %ecx, %cr4

    mov %cr0, %ecx
    or $0x80000000, %ecx
    mov %ecx, %cr0


    movl $higher_half, %edx
    jmp *%edx
    
higher_half:
    movl $0, (boot_pd)
    invlpg 0

    /* Setup our stack */
    mov $stack_top, %esp

    /* Make sure our stack is 16-byte aligned */
    and $-16, %esp

    pushl %esp
    pushl %eax /* Multiboot header magic */
    add $KERN_VBASE, %ebx
    pushl %ebx /* Multiboot header pointer */

    /* Disable interrupts and call kernel proper */
    cli
    call kmain

    /* Clear interrupts and hang if we return from kmain */
    cli

hang:
    hlt
    jmp hang
