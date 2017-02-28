global load_kern
extern main

[bits 32]

load_kern: 
	cli	
	call main
	hlt
