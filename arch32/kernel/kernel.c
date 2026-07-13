#include <stdint.h>
#include <stddef.h>
#include <arch32/kernel.h>
#include <arch32/vga.h>
#include <lib/printk.h>
#include <arch32/gdt.h>

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

void kernel_main(void) {

    const char* strg =
        "dernel loaded successfully";
    vga_clear_screen();
    vga_set_color(GREEN | BLACK << 4);
    printk("Hii Bhargav here\n%s %d\n", strg, -43 );
    init_gdt();
}
