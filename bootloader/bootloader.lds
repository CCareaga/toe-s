ENTRY(start);

PHDRS 
{
	headers PT_PHDR FILEHDR PHDRS;
	code PT_LOAD FILEHDR;
}

SECTIONS
{
	 . = 0x7c00; 
	.text : { *(.text) } :code
	.data : { *(.data) }
	.bss : { *(.bss) }
	/DISCARD/ : { *(.eh_frame) }
}