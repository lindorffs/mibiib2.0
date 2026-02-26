#ifndef TAR_H_
#define TAR_H_

#include <stdint.h>
#include <stddef.h>

#include "fs.h"

uint32_t oct2bin(const char* str, int size);

void load_tar(uint64_t address);

#endif