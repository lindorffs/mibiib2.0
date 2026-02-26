#ifndef IO_H_
#define IO_H_

#include <stdint.h>
#include <stddef.h>

static char kbd_US[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, // Ctrl
    'a','s','d','f','g','h','j','k','l',';','\'','`',
    0, // Left shift
    '\\',
    'z','x','c','v','b','n','m',',','.','/',
    0, // Right shift
    '*',
    0, // Alt
    ' ',
};
static void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
};

static uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
};


static char input_get_char(void) {
    while (1) {
        if (inb(0x64) & 1) { // keyboard buffer full
            uint8_t sc = inb(0x60);

            // Ignore key release events
            if (sc & 0x80)
                continue;

            if (sc < 128)
                return kbd_US[sc];
        }
    }
}

#endif