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

void handle_page_fault(int_registers_t* regs);
void handle_exception(int_registers_t* regs);

void handle_page_fault(int_registers_t* regs) {
    asm volatile("cli");
    //print CR2 , EIP and some info when page fault happens
    uint32_t faulting_addr;
    asm volatile("mov %0, cr2" : "=r"(faulting_addr));

    int present  = regs->err_code & 0x1; // 0 = Page not present, 1 = Protection violation
    int rw       = regs->err_code & 0x2; // 0 = Read fault, 1 = Write fault
    int user     = regs->err_code & 0x4; // 0 = Kernel mode (Ring 0), 1 = User mode (Ring 3)

    printk("PAGE FAULT at 0x%x (EIP: 0x%x) [%s, %s, %s]\n", faulting_addr,
           regs->eip, present ? "protection-violation" : "non-present-page",
           rw ? "write" : "read", user ? "user-mode" : "kernel-mode");

    while (1) {
        asm volatile("hlt");
    }
}

void handle_exception(int_registers_t* regs) {
    asm volatile("cli");
    if(regs->int_no < 32 && regs->int_no != 14) {
        printk("%s\n", exception_messages[regs->int_no]);
    }
    while (1) {
        asm volatile("hlt");
    }
}

void install_exception_handlers() {
    for(int8_t i = 0; i < 32; i++) {
        if (i == 14)
            install_interrupt_handler(i, handle_page_fault);
        else
            install_interrupt_handler(i, handle_exception);
    }
    printk("exception handlers installed\n");
}
