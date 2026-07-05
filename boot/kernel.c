#include <stdint.h>
#include <stddef.h>
#include <kernel.h>
#include <vga.h>
#include <gdt.h>

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

void kernel_main(void) {

    const char* strg =
        "dernel loaded successfully";
    clear_screen();
    set_color(GREEN | BLACK << 4);
    printk("Hii Bhargav here\n%s %d\n", strg, -43 );

    init_gdt();

}
