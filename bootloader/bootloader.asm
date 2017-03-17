; ==============================================
; bootloader.asm
; a very simple bootloader that reads a sector from
; the floppy disk drive and jumps to said data
; ==============================================
global start

[bits 16]
[org 0x7c00]

start: jmp boot

boot:
	cli ; clear interrupts
	cld ; clear direction flag

	xor ah, ah
	int 0x13
	
	mov bx, 0x1000	; set bx to where we load the kernel

	mov al, 17		; set lower byte of ax to read 17 sectors
	mov ch, 0 		; set higher byte of cx to read track 0 (first track)
	mov cl, 2		; set lower byte of cx to read sector 2 (bootloader is sec1)
	mov dh, 0 		; set higher byte of dx to read head 0 (top side of disk)
	mov dl, 0 		; set lower byte of dx to read drive 0 (floppy drive)

	mov ah, 0x02	; read

	int 0x13		; call BIOS interrupt 13 to read drive

	mov bx, 3200
	mov al, 17		; set lower byte of ax to read 17 sectors
	mov ch, 1 		; set higher byte of cx to read track 1 (second track)
	mov cl, 1		; set lower byte of cx to read sector 1 
	mov dh, 0 		; set higher byte of dx to read head 0 (top side of disk)
	mov dl, 0 		; set lower byte of dx to read drive 0 (floppy drive)

	mov ah, 0x02	; read


	xchg bx, bx
	jmp pm_switch

	hlt				; this instruction should not execute


pm_switch:
	xor ax, ax
	mov ds, ax

	lgdt [gdt_desc]

	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp 08h:select_jump

[bits 32]
select_jump:
	xor eax, eax
	
	mov	ax, 0x10		; set data segments to data selector (0x10)
	mov	ds, ax
	mov	ss, ax
	mov es, ax
	mov esp, 0x9000

	xchg bx, bx 
	; do a far jump to set cs and go to kernel code
	jmp 08h:0x2000



gdt: ; Address for the GDT
gdt_null: ; Null Segment 
	dd 0 
	dd 0

;KERNEL_CODE equ $-gdt 
gdt_kernel_code: 
	dw 0FFFFh ; Limit 0xFFFF 
	dw 0 ; Base 0:15 
	db 0 ; Base 16:23 
	db 09Ah ; Present, Ring 0, Code, Non-conforming, Readable 
	db 11001111b	
	;db 00Fh ; Page-granular 
	db 0 ; Base 24:31

;KERNEL_DATA equ $-gdt 
gdt_kernel_data: 
	dw 0FFFFh ; Limit 0xFFFF 
	dw 0 ; Base 0:15 
	db 0 ; Base 16:23 
	db 092h ; Present, Ring 0, Data, Expand-up, Writable 
	db 00Fh ; Page-granular 
	db 0 ; Base 24:32
gdt_end: ; Used to calculate the size of the GDT

gdt_desc: ; The GDT descriptor 
	dw gdt_end - gdt - 1 ; Limit (size) 
	dd gdt ; Address of the GDT


; pad the file to file the rest of the sector (512 bytes) and add boot sig

times 510 - ($-$$) db 0
dw 0xAA55