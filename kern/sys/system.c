#include "system.h"

inline uint8_t inport(uint16_t _port) {

    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

inline void outport(uint16_t _port, uint8_t _data) {
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

inline void interrupt(uint8_t num) {
    return;
}

void bochs_break() {
    asm volatile ("xchg %bx, %bx");
}

