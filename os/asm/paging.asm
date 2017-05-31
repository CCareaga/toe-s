; some small procedures to read and write paging related registers
global load_page_directory

load_page_directory:
    mov eax, [esp + 4]
    mov cr3, eax
    ret

global enable_paging

enable_paging:
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    ret
