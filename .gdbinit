target remote localhost:1234 
symbol-file kernel.sym
foc c
b kmain
b start
b _start
c
