#include <stdint.h>
#include <stddef.h>
#include <arch32/kernel.h>
#include <arch32/vga.h>
#include <lib/printk.h>
#include <arch32/gdt.h>
#include <arch32/idt.h>
#include <arch32/exceptions.h>
#include <arch32/timer.h>
#include <arch32/keyboard.h>

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

void kernel_main(void) {

    const char* strg =
        "dernel loaded successfully";
    vga_clear_screen();
    vga_set_color(GREEN | BLACK << 4);
    printk("Hii Bhargav here\n%s %d\n", strg, -43 );
    init_gdt();
    init_idt();
    install_exception_handlers();
    init_pit(100);
    init_keyboard();
    asm volatile("sti"); // starts the intrrupts
    printk("Harshit loves Bhargav %d",3000);
    ksleep(1);
    for ( ; ; );
    //volatile int16_t z= 0;
    //volatile int16_t a = 4/z;
    //asm volatile("ud2"); //send invalid opcode
}
