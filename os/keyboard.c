#include "keyboard.h"

const char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r','t', 'y', 'u', 'i', 'o', 'p', '[', ']', 
    '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';','\'', '`',   0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/', 0, '*',
    0,	// Alt 
    ' ',// Space bar 
    0,	// Caps lock 
    0,	// 59 - F1 key ... > 
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	// < ... F10 
    0,	// 69 - Num lock
    0,	// Scroll Lock 
    0,	// Home key 
    0,	// Up Arrow 
    0,	// Page Up 
    '-',
    0,	// Left Arrow 
    0,
    0,	// Right Arrow 
    '+',
    0,	// 79 - End key
    0,	// Down Arrow
    0,	// Page Down
    0,	// Insert Key
    0,	// Delete Key
    0,   0,   0,
    0,	// F11 Key
    0,	// F12 Key 
    0,	// All other keys are undefined
};

static mod_struct modifiers = {0, 0, 0, 0, 0, 0};

// handle irq 1
void keyboard_handler(registers *regs) {
    uint8_t scancode;

    /* Read from the keyboard's data buffer */
    scancode = inport(0x60);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80) {
        //see if the user released the shift, alt, or control keys... 
    }
    else {
        // my vga_write function takes a const char* so this is how I do 
        // it... not sure if this is the way but it works
        const char key[] = {kbdus[scancode]};
        vga_write(&key);
    }
}

// add our handler to our IRQ array, keystroke triggers IRQ 1
void init_keyboard() {
    add_handler(1, &keyboard_handler);
}