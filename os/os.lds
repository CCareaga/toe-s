ENTRY(load_kern);

PHDRS 
{
	code PT_LOAD;
}

SECTIONS
{
	.text 0x500: ALIGN(0x100) { *(.text) } :code
	
	.rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }
	.bss : { *(.bss) }
}