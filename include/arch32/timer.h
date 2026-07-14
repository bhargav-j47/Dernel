#pragma once
#include <stdint.h>
#include <arch32/isr.h>

void init_pit(uint32_t freq);
void ksleep(uint32_t t);
