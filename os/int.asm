; instead of writing 32 seperate int handlers we write a genereic macro
; one for interrupts with error codes and one with interrupts without
; this allows us to define these routines programatically!
[bits 32]
extern isr_handler

%macro ISR_NOERR 1  
  global isr%1
  isr%1:
    cli
    push byte 0
    push byte %1
    jmp isr_common
%endmacro

%macro ISR_ERR 1
  global isr%1
  isr%1:
    cli
    push byte %1
    jmp isr_common
%endmacro

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR 8
ISR_NOERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_ERR 30
ISR_NOERR 31


isr_common:
   pusha                    ; push general regs

   mov ax, ds               ; Lower 16-bits of eax = ds.
   push eax                 ; save the data segment descriptor

   mov ax, 0x10  ; load all of our segment register to kernel data seg
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax

   cld

   call isr_handler

   pop eax        ; grab our old seg selector off the stack and restore seg regs
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax

   popa                     
   add esp, 8     ; Cleans up the pushed error code and pushed ISR number
   sti
   iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP