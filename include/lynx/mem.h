#ifndef MEM_H_
#define MEM_H_

#include <stdint.h>
#include <stddef.h>

static uint64_t heap_ptr = 0x200000;

static void* kmalloc(size_t size) {
    if (size == 0) return NULL;
    uint64_t ptr = heap_ptr;
    heap_ptr += size;
    // Align to 8 bytes
    if (heap_ptr % 8 != 0) {
        heap_ptr += 8 - (heap_ptr % 8);
    }
    return (void*)ptr;
}

static void* memset(void* ptr, int value, size_t num) {
    uint8_t* p = (uint8_t*)ptr;
    while(num--) *p++ = (uint8_t)value;
    return ptr;
}

// String helpers needed by FS and Kernel
static int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

static size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

static void strcpy(char* dest, const char* src) {
    while((*dest++ = *src++));
}

#endif