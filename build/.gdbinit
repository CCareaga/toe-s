set disassembly-flavor intel

set arch i386
symbol-file os/kernel.elf
target remote localhost:26000
layout reg
