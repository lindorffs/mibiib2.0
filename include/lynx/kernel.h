#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdint.h>
#include <stddef.h>

#include "mem.h"
#include "io.h"
void kmain();


void term_putc(char c);

void term_print(const char* str);

void term_clear();


#endif