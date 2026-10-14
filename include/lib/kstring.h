#pragma once
#include <stdint.h>
#include <stddef.h>

void* kmemset(void* ptr, int value, size_t num);
void* kmemcpy(void* dest, const void* src, size_t num);
