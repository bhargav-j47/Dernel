#pragma once
#include <stdint.h>

typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss; // last two only if privilege changed
}__attribute__((packed)) int_registers_t;

typedef void (*isr_t)(int_registers_t*);   //function pointer
void install_interrupt_handler(uint8_t n, isr_t handler);
