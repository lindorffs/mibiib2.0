#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdint.h>
#include <stddef.h>

#include "mem.h"
#include "io.h"

extern void kentry(uint64_t mb_info_addr, uint64_t magic);

void kmain(uint64_t mb_info_addr, uint64_t magic);

void term_putc(char c);

void term_print(const char* str);

void term_clear();


#endif