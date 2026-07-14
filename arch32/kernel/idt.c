#include <arch32/idt.h>
#include <arch32/pic.h>
#include <lib/kstring.h>
#include <lib/printk.h>
#include <stdint.h>

#define KERNEL_CS 0x08
#define IDT_PRESENT 0X80
#define IDT_DPL_RING0 0X00
#define IDT_SYSTEM 0X00
#define IDT_INTR_GATE 0X0E  // 32 bit intrrupt gate
#define IDT_TRAP_GATE 0X0F  // 32 bit trap gate

#define FLAG (IDT_PRESENT | IDT_DPL_RING0 | IDT_SYSTEM | IDT_INTR_GATE)

typedef struct {
    uint16_t offset_l;
    uint16_t cs_segment;
    uint8_t reserved;
    uint8_t attributes;
    uint16_t offset_h;
} __attribute__((packed)) idt_entry;

typedef struct {
    uint16_t size;
    uint32_t base;
} __attribute__((packed)) IDTR;

IDTR idt_descriptor;
idt_entry idt_table[256];

// CPU Exceptions (ISR 0 - 31)
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

// Hardware Interrupts (IRQ 0 - 15)
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

extern void x86_load_idt(IDTR* idt);
void set_idt_gate(uint8_t n, uint32_t offset, uint16_t cs_segment,
                  uint8_t attributes);
void setup_idt_table();

void init_idt() {
    printk("initializing idt\n");

    pic_remap(0x20, 0x28);

    setup_idt_table();

    idt_descriptor.base = (uint32_t)idt_table;
    idt_descriptor.size = ((sizeof(idt_entry) * 256) - 1);
    x86_load_idt(&idt_descriptor);

    printk("idt loaded successfully\n");
}

void set_idt_gate(uint8_t n, uint32_t offset, uint16_t cs_segment,
                  uint8_t attributes) {
    idt_table[n].offset_l = offset & 0xFFFF;
    idt_table[n].offset_h = (offset >> 16) & 0xFFFF;
    idt_table[n].cs_segment = cs_segment;
    idt_table[n].reserved = 0;
    idt_table[n].attributes = attributes;
}

void setup_idt_table() {
    kmemset(&idt_table, 0, sizeof(idt_entry) * 256);

    set_idt_gate(0, (uint32_t)isr0, KERNEL_CS, (uint8_t)FLAG);

    set_idt_gate(0, (uint32_t)isr0, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(1, (uint32_t)isr1, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(2, (uint32_t)isr2, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(3, (uint32_t)isr3, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(4, (uint32_t)isr4, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(5, (uint32_t)isr5, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(6, (uint32_t)isr6, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(7, (uint32_t)isr7, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(8, (uint32_t)isr8, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(9, (uint32_t)isr9, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(10, (uint32_t)isr10, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(11, (uint32_t)isr11, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(12, (uint32_t)isr12, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(13, (uint32_t)isr13, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(14, (uint32_t)isr14, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(15, (uint32_t)isr15, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(16, (uint32_t)isr16, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(17, (uint32_t)isr17, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(18, (uint32_t)isr18, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(19, (uint32_t)isr19, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(20, (uint32_t)isr20, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(21, (uint32_t)isr21, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(22, (uint32_t)isr22, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(23, (uint32_t)isr23, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(24, (uint32_t)isr24, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(25, (uint32_t)isr25, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(26, (uint32_t)isr26, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(27, (uint32_t)isr27, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(28, (uint32_t)isr28, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(29, (uint32_t)isr29, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(30, (uint32_t)isr30, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(31, (uint32_t)isr31, KERNEL_CS, (uint8_t)FLAG);

    // irqs
    set_idt_gate(32, (uint32_t)irq0, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(33, (uint32_t)irq1, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(34, (uint32_t)irq2, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(35, (uint32_t)irq3, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(36, (uint32_t)irq4, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(37, (uint32_t)irq5, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(38, (uint32_t)irq6, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(39, (uint32_t)irq7, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(40, (uint32_t)irq8, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(41, (uint32_t)irq9, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(42, (uint32_t)irq10, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(43, (uint32_t)irq11, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(44, (uint32_t)irq12, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(45, (uint32_t)irq13, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(46, (uint32_t)irq14, KERNEL_CS, (uint8_t)FLAG);
    set_idt_gate(47, (uint32_t)irq15, KERNEL_CS, (uint8_t)FLAG);
}
