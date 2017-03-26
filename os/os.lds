/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(load_kern)

phys = 0x00100000;

SECTIONS
{

	. = 0x1M;
	start = .; _start = .; __start = .;

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(.bss)
	}

	. = ALIGN(4K);

    end = .; _end = .; __end = .;
}