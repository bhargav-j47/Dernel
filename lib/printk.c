#include <lib/printk.h>
#include <stdint.h>
#include <arch32/vga.h>
#include <stdarg.h>


void print_str(const char* st) {
    while (*st) {
        buff_putchar(*st);
        st++;
    }
}

void print_uint(uint32_t d) {
    if (d==0) {
        buff_putchar('0');
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
        buff_putchar(buff[--i]);   
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
            case '%':
                fmt++;
                switch (*fmt){
                    case 'd':
                        d = va_arg(args, int32_t);
                        if (d < 0) {
                            buff_putchar('-');
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
                        buff_putchar(c);
                        break;

                    case '%':
                        buff_putchar((const char)'%');
                        break;

                    default:
                        goto err;
                        break;
                }
                break;

            default:
                buff_putchar(*fmt);
                break;
        }
        fmt++;
    }

err:
    va_end(args);
    console_flush();        //temporary fix will remove once multitasking is on
    return;
}


extern void vga_putchar(char c);

// Flushes the memory buffer to the active hardware console
void console_flush() {
    while (log_read_ptr != log_write_ptr) {
        char c = kernel_log[log_read_ptr];
        
        vga_putchar(c); // Send to x86 VGA
        
        log_read_ptr = (log_read_ptr + 1) % KERNEL_LOG_SIZE;
    }
}
