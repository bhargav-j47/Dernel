#pragma once
#include <stdint.h>

void init_kconsole();
void kconsole_putchar(const char c);
void kconsole_rmchar();
void kcon_set_color(uint32_t fg, uint32_t bg);
