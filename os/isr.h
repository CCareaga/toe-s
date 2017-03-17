#ifndef ISR_H
#define ISR_H

#include <stdint.h>

// NOTE: the variable 'useless' is the esp pushed onto the stack when pusha is executed
// this is not popped of the stack when popa is executed. We instead use the esp that
// the CPU pushes automatically.

typedef struct reg_struct
{
   uint32_t ds;                  // Data segment selector
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   uint32_t int_no, err_code;    // Interrupt number and error code (if applicable)
   uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} registers;

#endif