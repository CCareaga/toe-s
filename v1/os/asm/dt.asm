.intel_syntax noprefix

global gdt_install 

gdt_install:
   mov eax, [esp+4]  
   lgdt [eax]      

   mov ax, 0x10
   mov ds, ax        
   mov es, ax
   mov fs, ax
   mov gs, ax
   mov ss, ax
   jmp 0x08:.return   

.return:
   ret

global idt_install

idt_install:
   mov eax, [esp+4]  ; get the passed parameter from our c file. 
   lidt [eax]        ; load it up
   sti
   ret               ; get out! 
