.intel_syntax noprefix

.globl gdt_install 

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

.globl idt_install

idt_install:
   mov eax, [esp+4]  
   lidt [eax]        
   sti
   ret               
