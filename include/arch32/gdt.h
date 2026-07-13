#pragma once
#include <stdint.h>

void set_tss_kernel_stack(uint32_t stack);
void init_gdt();
