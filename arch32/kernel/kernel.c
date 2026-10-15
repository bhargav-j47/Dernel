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
#include <arch32/vesa.h>

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

void kernel_main(uint32_t magic, uint32_t mbinfo_ptr) {

    const char* strg = "dernel loaded successfully";
    //vga_clear_screen();
    //vga_set_color(VGA_GREEN | VGA_BLACK << 4);
    printk("Hii Bhargav here\n%s %d\n", strg, -43 );
    init_gdt();
    init_idt();
    install_exception_handlers();
    init_pit(100);
    init_keyboard();
    uint32_t v_mbinfo_ptr = mbinfo_ptr + (uint32_t)0xC0000000;
    init_memory(magic, v_mbinfo_ptr);
    asm volatile("sti"); // starts the intrrupts
    init_vesa(magic, v_mbinfo_ptr);
    vesa_fill_rect(100, 100, 150, 150, VESA_BLUE);
    vesa_fill_rect(200, 200, 150, 150, VESA_BLUE);
    vesa_fill_rect(400, 200, 150, 150, VESA_BLUE);
    vesa_fill_rect(600, 350, 150, 150, VESA_BLUE);
    vesa_flush();
    ksleep(1);

    //try heap
    uint32_t* old = (uint32_t*) kmalloc(5 * sizeof(uint32_t));
    old[0] = 1;
    old[1] = 3;
    old[2] = 6;
    old[3] = 9;
    old[4] = 323;
    
    for (int i = 0; i < 5; i++) {
        printk("old addr no is: %d \n",old + i);
    }
    //kfree(old);

    uint32_t* new = (uint32_t*) kmalloc(5 * sizeof(uint32_t));

    for (int i = 0; i < 5; i++) {
        printk("no is: %d \n",new + i);
    }

    while (1) {
        asm volatile("hlt");
    }
    //volatile int16_t z= 0;
    //volatile int16_t a = 4/z;
    //asm volatile("ud2"); //send invalid opcode
}
