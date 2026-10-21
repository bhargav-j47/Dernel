#pragma once
#include <stdint.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t pitch;
    uint8_t* frontbuff_addr;
    // reading and writing from vram (i.e frontbuff is very slow)
    // so create backbuff in heap and then flush to vram at once.
    uint8_t* backbuff_addr;
    uint32_t buff_size;
} vesa_display;

extern volatile uint8_t is_fb_avl;

void init_vesa(uint32_t magic, uint32_t mbinfo_ptr);
void vesa_flush(void);
void vesa_get_dis_dimensions(uint32_t* width, uint32_t* height);
void vesa_scroll_up(uint32_t bg_color, uint32_t pixel_rows);
void vesa_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void vesa_flush(void);
void vesa_put_pixel(uint32_t x, uint32_t y, uint32_t color);
void vesa_clear_screen();

//32-bit VESA RGB Color Palette (0x00RRGGBB) copied from website

// Helper macro to make custom RGB colors on the fly: VESA_RGB(255, 128, 0)
#define VESA_RGB(r, g, b) (((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))

//Standard 16 VGA-Equivalent Colors
#define VESA_BLACK         0x00000000
#define VESA_BLUE          0x000000AA
#define VESA_GREEN         0x0000AA00
#define VESA_CYAN          0x0000AAAA
#define VESA_RED           0x00AA0000
#define VESA_MAGENTA       0x00AA00AA
#define VESA_BROWN         0x00AA5500
#define VESA_LIGHT_GREY    0x00AAAAAA
#define VESA_DARK_GREY     0x00555555
#define VESA_LIGHT_BLUE    0x005555FF
#define VESA_LIGHT_GREEN   0x0055FF55
#define VESA_LIGHT_CYAN    0x0055FFFF
#define VESA_LIGHT_RED     0x00FF5555
#define VESA_LIGHT_MAGENTA 0x00FF55FF
#define VESA_YELLOW        0x00FFFF55
#define VESA_WHITE         0x00FFFFFF

//Pure Primary / Secondary Colors
#define VESA_PURE_RED      0x00FF0000
#define VESA_PURE_GREEN    0x0000FF00
#define VESA_PURE_BLUE     0x000000FF
#define VESA_ORANGE        0x00FFA500
#define VESA_PURPLE        0x00800080
#define VESA_PINK          0x00FF69B4

// Modern UI / OS Desktop Theme Colors
#define VESA_BG_DARK       0x001E1E2E  // Sleek dark terminal background
#define VESA_BG_BLUE       0x00004E98  // Classic desktop / BSOD blue
#define VESA_TEAL_RETRO    0x00008080  // Classic Windows 95 desktop teal
#define VESA_TASKBAR_GREY  0x002B2B2B  // Dark taskbar / window titlebar
#define VESA_ACCENT_BLUE   0x003B82F6  // Highlight / active window border
