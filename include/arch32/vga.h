#pragma once
#include <stdint.h>

#define WIDTH 80
#define HEIGHT 25

#define BLACK           0x0
#define LIGHT_GREY      0x7
#define BLUE            0x1
#define LIGHT_BLUE      0x9
#define GREEN           0x2
#define LIGHT_GREEN     0xA
#define RED             0x4
#define LIGHT_RED       0xC
#define YELLOW          0xE
#define DARK_GREY       0x8
#define WHITE           0xF

void vga_set_color(const uint16_t color);
void vga_new_line();
void vga_scroll_up();
void vga_scroll_down();
void vga_clear_screen();
void vga_putchar(const char c);
void vga_rmchar();
