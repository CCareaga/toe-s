ENTRY(boot_entry)
phys = 0x00100000;
SECTIONS
{
  . = 1M;

  start = .; _start = .; __start = .;

  .text phys : AT(phys)
  {
    code = .;
    *(._multiboot_header)
    *(.text)
  }

  . = ALIGN(4096);

  .data BLOCK(4K) : ALIGN(4096)
  {
    data = .;
    *(.data)
    *(.rodata)
  }

  . = ALIGN(4096);

  .bss BLOCK(4K) : ALIGN(4096)
  {
    bss = .;
    *(COMMON)
    *(.bss)
  }

  . = ALIGN(4096);

  end = .; _end = .; __end = .;

  /DISCARD/ :
  {
    *(.comment)
  }
}