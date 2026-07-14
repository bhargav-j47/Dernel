#include <stdint.h>
#include <arch32/pic.h>
#include <arch32/io.h>
#include <lib/printk.h>

#define PIC1_CMD    0x20
#define PIC1_DATA   0x21
#define PIC2_CMD    0xA0
#define PIC2_DATA   0xA1

#define ICW1_INIT   0x10
#define ICW1_ICW4   0x01
#define ICW4_8086   0x01


void pic_remap(int offset1, int offset2) {
    
    printk("remapping programmable interrupt controller\n");

    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, offset1);      // vector offset for master
    io_wait();
    outb(PIC2_DATA, offset2);      // vector offset for slave
    io_wait();

    outb(PIC1_DATA, 4);            // tell master there's a slave at IRQ2
    io_wait();
    outb(PIC2_DATA, 2);            // tell slave its cascade identity
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, mask1);        // restore saved masks
    outb(PIC2_DATA, mask2);

    printk("successfully remapped programmable interrupt controller\n");
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8)
        outb(PIC2_CMD, 0x20);
    outb(PIC1_CMD, 0x20);
}

void pic_irq_set_mask(uint8_t pic_irq_line) {
    uint16_t port = pic_irq_line < 8 ? PIC1_DATA : PIC2_DATA;
    if (pic_irq_line >= 8) pic_irq_line -= 8;
    outb(port, inb(port) | (1 << pic_irq_line));
}

void pic_irq_clear_mask(uint8_t pic_irq_line) {
    uint16_t port = pic_irq_line < 8 ? PIC1_DATA : PIC2_DATA;
    if (pic_irq_line >= 8) pic_irq_line -= 8;
    outb(port, inb(port) & ~(1 << pic_irq_line));
}
