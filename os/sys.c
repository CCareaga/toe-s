#include "sys.h"

u8int inport(u16int _port) {
    
    u8int rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outport(u16int _port, u8int _data) {

    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

void bochs_break() {
	 asm volatile ("xchg %bx, %bx");
}