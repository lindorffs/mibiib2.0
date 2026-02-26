#include <stdint.h>
#include <stddef.h>

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
const int WIDTH = 80, HEIGHT = 25;
int term_x = 0, term_y = 0;
uint8_t term_color = 0x0F;

void term_putc(char c) {
    if (c == '\n') { term_x = 0; term_y++; } 
    else if (c == '\b') {
        if (term_x > 0) term_x--;
        vga_buffer[term_y * WIDTH + term_x] = ((uint16_t)term_color << 8) | ' ';
    } else {
        vga_buffer[term_y * WIDTH + term_x] = ((uint16_t)term_color << 8) | c;
        term_x++;
    }
    if (term_x >= WIDTH) { term_x = 0; term_y++; }
    if (term_y >= HEIGHT) { term_y = 0; term_clear(); } // Simple wrap
}

void term_print(const char* str) {
    for (size_t i = 0; str[i]; i++) term_putc(str[i]);
}

void term_clear() {
    for (int i = 0; i < WIDTH * HEIGHT; i++) vga_buffer[i] = 0x0F20;
    term_x = 0; term_y = 0;
}