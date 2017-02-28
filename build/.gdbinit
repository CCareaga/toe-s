set disassembly-flavor intel

set arch i386
target remote localhost:26000
layout reg
b *0x7c00
b *0x7c1f