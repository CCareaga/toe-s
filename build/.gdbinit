set disassembly-flavor intel

set arch i386
symbol-file kernel.sym
target remote localhost:26000
layout reg
