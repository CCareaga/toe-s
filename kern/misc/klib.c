#include "klib.h"

void *memset(void *bufptr, uint8_t value, uint32_t size) {
    unsigned char *buf = (unsigned char *) bufptr;
    uint32_t i;

    for (i = 0; i < size; i++)
        buf[i] = (unsigned char) value;

    return bufptr;
}

// this code is from http://wiki.osdev.org/Printing_To_Screen#Printing_Integers
char *itoa(uint32_t val, uint32_t base){

    static char buf[32] = {0};

    int i = 30;
    
    if (val) {
        for(; val && i; --i, val /= base)
            buf[i] = "0123456789abcdef"[val % base];
    }
    else {
        buf[i--] = '0';
    }

    return &buf[i + 1];
}

void kprintf(char *fmt, ...) {
    char *temp = fmt;

    int i;
    char *s;

    va_list arg;
    va_start(arg, fmt);

    while (*temp) {
        if (*temp != '%') {
           vga_write_char(*temp); 
           temp++;
        }

        else {
            temp++;

            switch (*temp) {
                case 'c':
                    i = va_arg(arg, int);
                    vga_write(itoa(i, 10));
                    break;

                case 'd': 
                    i = va_arg(arg, int);     
                    if (i < 0) {   
                        i = -i;
                        vga_write_char('-'); 
                    } 

                    vga_write(itoa(i, 10));
                    break;   

                case 'o': 
                    i = va_arg(arg, int); 
                    vga_write(itoa(i, 8));
                    break; 
                
                case 's': 
                    s = va_arg(arg, char *);       
                    vga_write(s); 
                    break; 
                
                case 'x': 
                    i = va_arg(arg, int); 
                    vga_write(itoa(i, 16));
                    break; 
            }
             
            temp++;
        }
    }

    va_end(arg);
}

