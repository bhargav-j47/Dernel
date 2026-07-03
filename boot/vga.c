#include <stdint.h>
#include <vga.h>


volatile uint16_t* vgabuffer = (uint16_t*) 0xB8000;
uint16_t def_color = (BLACK << 4) | (DARK_GREY);
uint16_t cur_color;

uint16_t line = 0;
uint16_t column = 0;

void clear_screen() {
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

void new_line() {
    if (line < HEIGHT) {
        column=0;
        line++;
    }else {
        column=0;
        scroll_up();
    }
}

void scroll_up(){
   for (uint16_t y=0; y < HEIGHT-1; y++) {
        for (uint16_t x=0; x < WIDTH-1; x++) {
            vgabuffer[y * WIDTH + x] = vgabuffer[(y+1) * WIDTH + x];
        }
    }

}

void printk(const char *fmt, const uint16_t color) {
    cur_color = color;

    while (*fmt) {
        switch (*fmt) {
            case '\n':
                new_line();
                break;
            default:
                if (column == WIDTH) {
                    new_line();
                }
                vgabuffer[line * WIDTH + column] =
                    (uint16_t)*fmt | ((uint16_t)cur_color << 8);
                column++;
                break;
        }
        fmt++;
    }
}
