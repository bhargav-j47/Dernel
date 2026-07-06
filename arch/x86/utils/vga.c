#include <stdint.h>
#include <vga.h>
#include <stdarg.h>


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

void set_color(const uint16_t color) {
    cur_color = color;
}

void print_char(const char c){
    if (column == WIDTH) {
        new_line();
    }

    vgabuffer[line * WIDTH + column] =
                    (uint16_t)c | ((uint16_t)cur_color << 8);

    column++;
}

void print_str(const char* st) {
    while (*st) {
        print_char(*st);
        st++;
    }
}

void print_uint(uint32_t d) {
    if (d==0) {
        print_char('0');
        return;
    }
    char buff[32];
    uint16_t i=0;
    uint16_t rem;

    while (d) {
         rem = d%10;
         buff[i] = rem + '0';
         i++;
         d /= 10;
    }

    while (i > 0) {
        print_char(buff[--i]);   
    }

    return;
}

void printk(const char* fmt, ...) {

    va_list args;
    char c;
    char* st;
    int32_t d;

    va_start(args, fmt);

    while (*fmt) {
        switch (*fmt) {
            case '\n':
                new_line();
                break;

            case '%':
                fmt++;
                switch (*fmt){
                    case 'd':
                        d = va_arg(args, int32_t);
                        if (d < 0) {
                            print_char('-');
                            d *= -1;
                        }
                        print_uint(d);
                        break;
    
                    case 'u':
                        d = va_arg(args, uint32_t);
                        print_uint(d);
                        break;

                    case 's': 
                        st = va_arg(args, char*);
                        print_str(st);
                        break;

                    case 'c': 
                        c = (char)va_arg(args,int);
                        print_char(c);
                        break;

                    case '%':
                        print_char((const char)'%');
                        break;

                    default:
                        goto err;
                        break;
                }
                break;

            default:
                print_char(*fmt);
                break;
        }
        fmt++;
    }

err:
    va_end(args);
    return;
}
