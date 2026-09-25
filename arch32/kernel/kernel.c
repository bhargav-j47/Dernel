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
#include <arch32/pmm.h>
#include <arch32/memory.h>
#include <arch32/kheap.h>

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

void kernel_main(uint32_t magic, uint32_t mbinfo_ptr) {

    const char* strg = "dernel loaded successfully";
    vga_clear_screen();
    vga_set_color(GREEN | BLACK << 4);
    printk("Hii Bhargav here\n%s %d\n", strg, -43 );
    init_gdt();
    init_idt();
    install_exception_handlers();
    init_pit(100);
    init_keyboard();
    uint32_t v_mbinfo_ptr = mbinfo_ptr + (uint32_t)0xC0000000;
    init_memory(magic, v_mbinfo_ptr);
    asm volatile("sti"); // starts the intrrupts
    ksleep(1);

    //try heap
    uint32_t* new = (uint32_t*) kmalloc(5 * sizeof(uint32_t));
    new[0] = 1;
    new[1] = 3;
    new[2] = 6;
    new[3] = 9;
    new[4] = 323;
    
    for (int i = 0; i < 5; i++) {
        printk("no is: %d \n",new[i]);
    }

    for ( ; ; );
    //volatile int16_t z= 0;
    //volatile int16_t a = 4/z;
    //asm volatile("ud2"); //send invalid opcode
}
