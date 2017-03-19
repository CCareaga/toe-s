; instead of writing 32 separate int handlers we write a generic macro
; one for interrupts with error codes and one with interrupts without
; this allows us to define these routines programatically!

extern isr_handler

%macro ISR_NOERR 1  
    global isr%1
    isr%1:
        push byte 0
        push %1
        jmp isr_common
%endmacro

%macro ISR_ERR 1
    global isr%1
    isr%1:
        push %1
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
ISR_NOERR 8
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
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10   ; Load the Kernel Data Segment descriptor!
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp   ; Push us the stack
    push eax
    mov eax, isr_handler
    call eax       ; A special call, preserves the 'eip' register
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP! 

; ================================= IRQ ==================================

; This macro creates a stub for an IRQ - the first parameter is
; the IRQ number, the second is the ISR number it is remapped to.

extern irq_handler

%macro IRQ 2
  global irq%1
  irq%1:
    push byte 0
    push %2
    jmp irq_common
%endmacro

; this will create a bunch of handlers for the IRQ's 
; since we remapped the PIC we need IRQ 0 to ISR 32 and so on

IRQ   0, 32
IRQ   1, 33
IRQ   2, 34
IRQ   3, 35
IRQ   4, 36
IRQ   5, 37
IRQ   6, 38
IRQ   7, 39
IRQ   8, 40
IRQ   9, 41
IRQ  10, 42
IRQ  11, 43
IRQ  12, 44
IRQ  13, 45
IRQ  14, 46
IRQ  15, 47

; when an IRQ comes through it will trigger an entry in our IDT (32 - 47)
; that will clal one of our IRQ macros and that in turn will call this 
; routine below

; this is pretty much exactly the same as isr_common except it calls
; irq_handler 

irq_common:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10   ; Load the Kernel Data Segment descriptor!
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp   ; Push us the stack
    push eax
    mov eax, irq_handler
    call eax       ; A special call, preserves the 'eip' register
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!