#include <stdint.h>
#include <stddef.h>
#include <kernel.h>
#include <vga.h>

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

void kernel_main(void) {

    const char* strg =
        "Hi dernel loaded successfully\nit is now on the new line";
    clear_screen();
    printk(strg, (GREEN | BLACK << 4));

}
