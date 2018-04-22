.section .text
.align 4

.global higher_half
.type higher_half, @function

higher_half:
    mov 4(%ebp), %eax
    add 0xc0000000, %eax
    mov %eax, 4(%ebp)
    ret
