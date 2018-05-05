.section .text
.align 4

.global get_eip
.type get_eip, @function

get_eip:
    pop %eax
    jmp %eax

