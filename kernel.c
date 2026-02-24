/* kernel.c */
#include <stdint.h>
#include <stddef.h>
#include "mem.h"
#include "fs.h"

/* --- Hardware Drivers --- */
// (Keeping these compact as they are unchanged)
volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
const int WIDTH = 80, HEIGHT = 25;
int term_x = 0, term_y = 0;
uint8_t term_color = 0x0F;

uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}
void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

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

char kbd_US[128] = { 0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ' };

char get_input_char() {
    while (1) {
        if (inb(0x64) & 1) {
            uint8_t sc = inb(0x60);
            if (!(sc & 0x80)) return kbd_US[sc];
        }
    }
}

/* --- Shell --- */

void shell_prompt() {
    term_print("user:/");
    if (current_dir != fs_root) {
        term_print(current_dir->name);
    }
    term_print("> ");
}

void execute_command(char* cmd) {
    term_print("\n");
    if (strcmp(cmd, "help") == 0) {
        term_print("Commands: ls, cd [dir], mkdir [dir], touch [file], cat [file], clear\n");
    }
    else if (strcmp(cmd, "clear") == 0) {
        term_clear();
    }
    else if (strcmp(cmd, "ls") == 0) {
        fs_list(current_dir, term_print);
    }
    // mkdir
    else if (cmd[0]=='m' && cmd[1]=='k' && cmd[2]=='d' && cmd[3]=='i' && cmd[4]=='r' && cmd[5]==' ') {
        char* name = cmd + 6;
        fs_mkdir(current_dir, name);
        term_print("Directory created.\n");
    }
    // touch
    else if (cmd[0]=='t' && cmd[1]=='o' && cmd[2]=='u' && cmd[3]=='c' && cmd[4]=='h' && cmd[5]==' ') {
        char* name = cmd + 6;
        fs_mkfile(current_dir, name, "");
        term_print("File created.\n");
    }
    // cat
    else if (cmd[0]=='c' && cmd[1]=='a' && cmd[2]=='t' && cmd[3]==' ') {
        fs_node_t* f = fs_find(current_dir, cmd + 4);
        if (f && f->type == FS_FILE) {
            term_print(f->data); term_print("\n");
        } else {
            term_print("File not found or is directory.\n");
        }
    }
    // cd
    else if (cmd[0]=='c' && cmd[1]=='d' && cmd[2]==' ') {
        char* name = cmd + 3;
        if (strcmp(name, "..") == 0) {
            if (current_dir->parent) current_dir = current_dir->parent;
        } 
        else if (strcmp(name, "/") == 0) {
            current_dir = fs_root;
        }
        else {
            fs_node_t* next = fs_find(current_dir, name);
            if (next && next->type == FS_DIR) {
                current_dir = next;
            } else {
                term_print("Directory not found.\n");
            }
        }
    }
    else if (cmd[0] != 0) {
        term_print("Unknown command.\n");
    }
}

void kmain(void) {
    term_clear();
    fs_init();
    
    // Create initial structure
    fs_node_t* home = fs_mkdir(fs_root, "home");
    fs_node_t* user = fs_mkdir(home, "user");
    fs_mkfile(user, "hello.txt", "Hello from inside /home/user!");
    fs_mkfile(fs_root, "boot.cfg", "KERNEL=1");

    term_print("My64-OS v2.0 (Hierarchical FS)\n");

    char buffer[128];
    int idx = 0;
    shell_prompt();

    while (1) {
        char c = get_input_char();
        if (c == '\n') {
            buffer[idx] = 0;
            execute_command(buffer);
            idx = 0;
            shell_prompt();
        } else if (c == '\b') {
            if (idx > 0) { idx--; term_putc('\b'); }
        } else if (c) {
            buffer[idx++] = c;
            char tmp[2] = {c,0};
            term_print(tmp);
        }
    }
}