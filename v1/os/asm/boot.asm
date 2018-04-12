; boot.asm - a bootloader using GRUB

MBOOT_PAGE_ALIGN    equ 1<<0    ; Load kernel and modules on a page boundary
MBOOT_MEM_INFO      equ 1<<1    ; Provide your kernel with memory info
MBOOT_HEADER_MAGIC  equ 0x1BADB002 ; Multiboot Magic value
; NOTE: We do not use MBOOT_AOUT_KLUDGE. It means that GRUB does not
; pass us a symbol table.
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

[bits 32]

global mboot                ; Make 'mboot' accessible from C.

extern code                 ; Start of the '.text' section.
extern bss                  ; Start of the .bss section.
extern end                  ; End of the last loadable section.

section ._multiboot_header
align 4

mboot:
    dd  MBOOT_HEADER_MAGIC        ; GRUB will search for this value on each
                                  ; 4-byte boundary in your kernel file
    dd  MBOOT_HEADER_FLAGS        ; How GRUB should load your file / settings
    dd  MBOOT_CHECKSUM            ; To ensure that the above values are correct
   
    dd  mboot                     ; Location of this descriptor
    dd  code                      ; Start of kernel '.text' (code) section.
    dd  bss                       ; End of kernel '.data' section.
    dd  end                       ; End of kernel.
    dd  boot_entry                ; Kernel entry point (initial EIP).

section .text
global boot_entry

boot_entry: 
	mov esp, stack
	jmp kern_jump

kern_jump:
    cli
    push ebx
	extern kmain
	call kmain
	jmp $


SECTION .bss
align 16
    resb 8192               ; This reserves 8KBytes of memory here
stack:

