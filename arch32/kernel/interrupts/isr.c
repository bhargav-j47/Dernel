#include <arch32/isr.h>
#include <arch32/pic.h>
#include <lib/printk.h>
#include <stdint.h>

isr_t interrupt_handlers[256];

void install_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

void isr_handler(int_registers_t* regs) {
    if(interrupt_handlers[regs->int_no] != 0) {
        interrupt_handlers[regs->int_no](regs);
    } else {
        // panic here
        printk("KERNEL PANIC! Unhandled Interrupt: %d", regs->int_no);
        asm volatile("cli");  // Disable interrupts
        while(1) {
            asm volatile("hlt");  // Halt the CPU forever
        }
    }
}

void irq_handler(int_registers_t* regs) {
    if(interrupt_handlers[regs->int_no] != 0) {
        interrupt_handlers[regs->int_no](regs);
    }
    pic_send_eoi(regs->int_no - 32);  // send EOI, even if unhandled
}

