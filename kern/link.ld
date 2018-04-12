/*
 * the kernels linker script, setups how the kernel's 
 * memory is laid out 
 */

OUTPUT_FORMAT(elf32-i386)
ENTRY(start)

SECTIONS
{
        . = 1M;
        phys = .;

        .text BLOCK(4K) : ALIGN(4K)
        {
                code = .;
                *(.multiboot)
                *(.text)
        }

        .rodata BLOCK(4K) : ALIGN(4K)
        {
                *(.rodata)
        }

        .data BLOCK(4K) : ALIGN(4K)
        {
                data = .;
                *(.data)
        }
        
        .bss BLOCK(4K) : ALIGN(4K)
        {
                bss = .;
                *(COMMON)
                *(.bss)
                *(.stack)
        }

        end = .;

        /DISCARD/ :
        {
                *(.comment)
                *(.eh_frame)
                *(.note.gnu.build-id)
        }
}
