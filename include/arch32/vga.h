#pragma once
#include <stdint.h>

#define WIDTH 80
#define HEIGHT 25

#define VGA_BLACK           0x0
#define VGA_LIGHT_GREY      0x7
#define VGA_BLUE            0x1
#define VGA_LIGHT_BLUE      0x9
#define VGA_GREEN           0x2
#define VGA_LIGHT_GREEN     0xA
#define VGA_RED             0x4
#define VGA_LIGHT_RED       0xC
#define VGA_YELLOW          0xE
#define VGA_DARK_GREY       0x8
#define VGA_WHITE           0xF

void vga_set_color(const uint16_t color);
void vga_new_line();
void vga_scroll_up();
void vga_scroll_down();
void vga_clear_screen();
void vga_putchar(const char c);
void vga_rmchar();
