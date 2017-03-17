global load_kern
extern kmain

[bits 32]

load_kern: 
	cli	
	call kmain
	hlt
