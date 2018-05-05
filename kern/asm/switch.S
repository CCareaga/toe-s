
# void ctx_swtch(ctx *old, ctx *new);

.global ctx_swtch
ctx_swtch:
    mov 4(%esp), %eax       # grab the old ctx
    mov 8(%esp), %edx       # this is the new ctx

    # push the values we want to save for when we run this program next
    # will will grap these when we switch back to this task
    movl %ebp, 16(%eax)
    movl %ebx, 12(%eax)
    movl %esi, 8(%eax)
    movl %edi, 4(%eax)

    # pushl 20(%eax) 
    pushl 16(%eax)
    pushl 12(%eax)
    pushl 8(%eax)
    pushl 4(%eax)

    movl %esp, (%eax)     # save the current stack pointer to old ctx
    movl (%edx), %esp     # now change esp to the new ctx stack

    popl %edi
    popl %esi
    popl %ebx
    popl %ebp

    ret

