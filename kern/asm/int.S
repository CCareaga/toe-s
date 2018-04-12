.section .text
.align 4

.macro irq idx byte
    .global irq\idx
    .type irq\idx, @function
    irq\idx:
        cli
        push $0x00
        push $\byte
        jmp irq_common
.endm

irq 0, 32
irq 1, 33
irq 2, 34
irq 3, 35
irq 4, 36
irq 5, 37
irq 6, 38
irq 7, 39
irq 8, 40
irq 9, 41
irq 10, 42
irq 11, 43
irq 12, 44
irq 13, 45
irq 14, 46
irq 15, 47

.extern irq_handler
.type irq_handler, @function

irq_common:
    pusha

    push %ds
    push %es
    push %es
    push %gs

    mov $0x10, %ax
    mov %ax, %ds 
    mov %ax, %es 
    mov %ax, %fs 
    mov %ax, %gs 

    cld

    push %esp
    call irq_handler
    add $4, %esp

    pop %gs
    pop %fs
    pop %es
    pop %ds
    
    popa
    add $8, %esp

    iret

.macro isr_noerr idx
    .global isr\idx
    isr\idx:
        cli
        push $0
        push $\idx
        jmp isr_common
.endm

.macro isr_err idx
    .global isr\idx
    isr\idx:
        cli
        push $\idx
        jmp isr_common
.endm

/* Standard X86 interrupt service routines */
isr_noerr 0
isr_noerr 1
isr_noerr 2
isr_noerr 3
isr_noerr 4
isr_noerr 5
isr_noerr 6
isr_noerr 7
isr_err   8
isr_noerr 9
isr_err   10
isr_err   11
isr_err   12
isr_err   13
isr_err   14
isr_noerr 15
isr_noerr 16
isr_noerr 17
isr_noerr 18
isr_noerr 19
isr_noerr 20
isr_noerr 21
isr_noerr 22
isr_noerr 23
isr_noerr 24
isr_noerr 25
isr_noerr 26
isr_noerr 27
isr_noerr 28
isr_noerr 29
isr_noerr 30
isr_noerr 31

.extern isr_handler
.type isr_handler, @function

isr_common:
    pusha

    push %ds
    push %es
    push %fs
    push %gs
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    cld

    push %esp
    call isr_handler
    add $4, %esp
    pop %gs
    pop %fs
    pop %es
    pop %ds

    popa
    add $8, %esp

    iret

.global idt_install
.type idt_install, @function

idt_install:
    mov 4(%esp), %eax
    lidt (%eax)
    sti
    ret

