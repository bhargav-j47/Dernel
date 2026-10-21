#include <stdint.h>
#include <arch32/kconsole.h>
#include <arch32/font.h>
#include <lib/printk.h>
#include <arch32/vesa.h>

static uint32_t m_rows = 0;
static uint32_t m_cols = 0;
static uint32_t line = 0;
static uint32_t col = 0;

static uint32_t fg_color = VESA_GREEN;
static uint32_t bg_color = VESA_BG_DARK;

void kconsole_scroll_up();
void kconsole_newline();
void kconsole_draw_char(const char c);

void kconsole_set_color(uint32_t fg, uint32_t bg) {
    fg_color = fg;
    bg_color = bg;
}

void kconsole_newline() {
    if (line < m_rows - 1) {
        col=0;
        line++;
    }else {
        col=0;
        kconsole_scroll_up();
    }
}

void kconsole_scroll_up() {
    vesa_scroll_up(bg_color, FONT_HEIGHT);
}

void kconsole_draw_char(const char c) {
    const uint8_t* font_glyph = &font_8x16[(uint8_t)c * FONT_HEIGHT];
    uint32_t base_x = col * FONT_WIDTH;
    uint32_t base_y = line * FONT_HEIGHT;

    // draw a single character one charater takes 8x16 pixes cause
    // our font is 8x16 
    for (uint8_t y = 0; y < FONT_HEIGHT; y++) {
        uint8_t bits = font_glyph[y];
        for (uint8_t x = 0; x < FONT_WIDTH; x++) {
            // Each glyph row is one byte; bit 7 MSB is the leftmost pixel.
            // Build a mask with only the bit for pixel column x set (0x80 >> x),
            // then AND it with the row byte: non-zero means this pixel is part of
            // the glyph foreground, zero means it's background.
            uint32_t color = (bits & (0x80 >> x)) ? fg_color : bg_color;
            vesa_put_pixel(base_x + x, base_y + y, color);
        }
    }
}

void kconsole_putchar(const char c) {
    if (c =='\n') {
        kconsole_newline();
        return;
    }

    if (c == '\b') {
        kconsole_rmchar();
        return;
    }
    
    if (col >= m_cols) {
        kconsole_newline();
    }

    kconsole_draw_char(c);
    col++;
}

void kconsole_rmchar() {
    if (col > 0) {
        col--;
    } else if (line > 0) {
        line--;
        col = m_cols - 1;
    } else {
        return;
    }

    kconsole_draw_char(' ');
}

void init_kconsole() {
    uint32_t width = 0, height = 0;
    vesa_get_dis_dimensions(&width, &height);

    m_cols = width / FONT_WIDTH;
    m_rows = height / FONT_HEIGHT;

    vesa_fill_rect(0, 0, width, height, bg_color);
    vesa_flush();
}
