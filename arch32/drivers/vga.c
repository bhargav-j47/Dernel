#include <stdint.h>
#include <arch32/vga.h>
#include <stdarg.h>
#include <lib/math.h>

volatile uint16_t* vgabuffer = (uint16_t*) 0xC00B8000;
uint16_t def_color = (VGA_BLACK << 4) | (VGA_DARK_GREY);
uint16_t cur_color;

uint16_t line = 0;
uint16_t column = 0;

void vga_clear_screen() {
    line = 0;
    column = 0;
    cur_color =def_color;

    for (uint16_t y=0; y < HEIGHT; y++) {
        for (uint16_t x=0; x < WIDTH; x++) {
            vgabuffer[y * WIDTH + x] =
                (uint16_t)' ' | ((uint16_t)def_color << 8);
        }
    }
}

void vga_new_line() {
    if (line < HEIGHT - 1) {
        column=0;
        line++;
    }else {
        column=0;
        vga_scroll_up();
    }
}

void vga_scroll_up(){
   for (uint16_t y=0; y < HEIGHT-1; y++) {
        for (uint16_t x=0; x < WIDTH-1; x++) {
            vgabuffer[y * WIDTH + x] = vgabuffer[(y+1) * WIDTH + x];
        }
    }

    for (uint16_t x = 0; x < WIDTH; x++) {
        vgabuffer[(HEIGHT - 1) * WIDTH + x] = 
            (uint16_t)' ' | ((uint16_t)cur_color << 8);
        }

}

void vga_set_color(const uint16_t color) {
    cur_color = color;
}

void vga_putchar(const char c) {
    if (c =='\n') {
        vga_new_line();
        return;
    }

    if (column >= WIDTH) {
        vga_new_line();
    }

    vgabuffer[line * WIDTH + column] =
                    (uint16_t)c | ((uint16_t)cur_color << 8);

    column++;
}

void vga_rmchar() {
    column = max(0, column-1);
    vgabuffer[line * WIDTH + column] =
        (uint16_t)' ' | ((uint16_t)def_color << 8);
}
