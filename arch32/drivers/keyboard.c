#include <stdint.h>
#include <arch32/keyboard.h>
#include <arch32/isr.h>
#include <lib/printk.h>
#include <arch32/io.h>

void keyboard_handler(int_registers_t* regs);

void init_keyboard() {
    printk("initializing keyboard\n");
    install_interrupt_handler(33, keyboard_handler);
    printk("keyboard is up\n");
}

void keyboard_handler(int_registers_t* regs) {
    uint8_t scan_code = inb(0x60) & 0x7F;
    uint8_t is_pressed = inb(0x60) & 0x80;

    printk("scan code is %c & is pressed %c\n",scan_code,is_pressed);
}
