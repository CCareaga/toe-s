.section .text
.align 4

.global higher_half
.type higher_half, @function

higher_half:
    mov 4(%esp), %eax 

    // push %eax
    call change_stack

    call get_eip
    add $0xc0000007, %eax
    jmp %eax
    ret

get_eip:
    mov (%esp), %eax
    ret

change_stack:
    // mov 4(%esp), %ebx 

    mov $stack_top, %ecx // move the stack start into ecx
    add $2, %ecx
    
    change_loop:
        mov (%ecx), %eax // dereference the value at ecx

        cmp $start, %eax // compare the stack_bottom with our value
        jge possible_addr       // it may be in the range
        jmp do_loop             // its not, move on

        possible_addr:  
            cmp $end, %eax       // compare the stack_top to our value
            jle change_addr      // if its greater than the top its in range
            jmp do_loop

        change_addr:
            add $0xc0000000, %eax   // add the virtual offset to the stack addr
            mov %eax, (%ecx)        // place the value back on the stack

        do_loop:
            cmp $stack_bottom, %ecx    // check if we got to the bottom
            jle change_done         // if we are lower than the bottom
            sub $4, %ecx            // we are still in range subtract 4
            jmp change_loop         // back to the top of the loop
    
    change_done:
        ret
    

