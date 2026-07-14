#include <arch32/isr.h>
#include <lib/printk.h>
#include <stdint.h>

// Standard x86 CPU exceptions
const char *exception_messages[] = {
    "Division By Zero", "Debug",
    "Non Maskable Interrupt", "Breakpoint",
    "Into Detected Overflow", "Out of Bounds",
    "Invalid Opcode", "No Coprocessor",
    "Double Fault", "Coprocessor Segment Overrun",
    "Bad TSS", "Segment Not Present",
    "Stack Fault", "General Protection Fault",
    "Page Fault", "Unknown Interrupt",
    "Coprocessor Fault", "Alignment Check",
    "Machine Check", "SIMD Floating-Point Exception",
    "Virtualization Exception", "Control Protection Exception",
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved"
};

void handle_exception(int_registers_t* regs);

void handle_exception(int_registers_t* regs) {
    asm volatile("cli");
    if(regs->int_no < 32) {
        printk("%s", exception_messages[regs->int_no]);
    }
    while (1) {
        asm volatile("hlt");
    }
}

void install_exception_handlers() {
    for(int8_t i = 0; i < 32; i++) {
        install_interrupt_handler(i, handle_exception);
    }
    printk("exception handlers installed\n");
}
