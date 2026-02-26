#include <lynx/mem.h>

// The SINGLE global heap pointer
uint64_t heap_ptr = 0x200000; // Start at 2MB

void* kmalloc(size_t size) {
    if (size == 0) return NULL;
    uint64_t ptr = heap_ptr;
    heap_ptr += size;
    // Align to 8 bytes for 64-bit speed
    if (heap_ptr % 8 != 0) {
        heap_ptr += 8 - (heap_ptr % 8);
    }
    return (void*)ptr;
}

void* memset(void* ptr, int value, size_t num) {
    uint8_t* p = (uint8_t*)ptr;
    while(num--) *p++ = (uint8_t)value;
    return ptr;
}

void* memcpy(void* dst, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dst;
    const uint8_t* s = (const uint8_t*)src;
    while(n--) *d++ = *s++;
    return dst;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

void strcpy(char* dest, const char* src) {
    while((*dest++ = *src++));
}