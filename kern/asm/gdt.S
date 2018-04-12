.section .text
.align 4

.global gdt_install
.type gdt_install, @function

gdt_install:
    /* load the gdt ptr into eax */
    mov 4(%esp), %eax
    lgdt (%eax)

    /* load all the segment registers with 0x10 */
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %ss
    
    /* long jump to set the cs register */
    ljmp $0x08, $done

done:
    ret
