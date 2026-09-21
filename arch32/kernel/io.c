#include <stdint.h>
#include <arch32/io.h>

void outb(uint16_t port, uint8_t val) {
    asm volatile ("out %1, %0" : : "a"(val), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("in %0, %1" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outw(uint16_t port, uint16_t val) {
    asm volatile ("out %1, %0" : : "a"(val), "Nd"(port));
}

uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile ("in %0, %1" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outl(uint16_t port, uint32_t val) {
    asm volatile ("out %1, %0" : : "a"(val), "Nd"(port));
}

uint32_t inl(uint16_t port) {
    uint32_t ret;
    asm volatile ("in %0, %1" : "=a"(ret) : "Nd"(port));
    return ret;
}

void io_wait(void) {
    asm volatile ("out 0x80, al" : : "a"(0));
}
