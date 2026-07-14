#pragma once
#include <stdint.h>

void pic_remap(int offset1, int offset2);
void pic_send_eoi(uint8_t irq);

void pic_irq_set_mask(uint8_t pic_irq_line);
void pic_irq_clear_mask(uint8_t pic_irq_line);
