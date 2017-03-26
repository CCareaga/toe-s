#include "include/sys.h"
#include "include/vga.h"

inline uint8_t inport(uint16_t _port) {
    
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

inline void outport(uint16_t _port, uint8_t _data) {

    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

void bochs_break() {
	 asm volatile ("xchg %bx, %bx");
}


void* memset(void* bufptr, uint8_t value, uint32_t size) {
	unsigned char* buf = (unsigned char*) bufptr;
	for (size_t i = 0; i < size; i++)
		buf[i] = (unsigned char) value;
	return bufptr;
}

// this code is from http://wiki.osdev.org/Printing_To_Screen#Printing_Integers
char* itoa(uint32_t val, uint32_t base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
}