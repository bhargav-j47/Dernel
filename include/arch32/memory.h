#pragma once
#include <stdint.h>

#define MEM_HEAP_END    0xDFFFFFFF
#define MEM_MMIO_START  0xE0000000
#define MEM_MMIO_END    0xEFFFFFFF

void init_memory(uint32_t magic, uint32_t mbinfo_ptr);
