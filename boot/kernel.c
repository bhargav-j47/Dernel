#include <stdint.h>
#include <stddef.h>
#include <kernel.h>

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

void kernel_main(void) {
    
    const char* strg = "Hi dernel loaded successfully";
    uint8_t color = 0x0A; 

    size_t i =0 ;

    while (strg[i] != '\0') {
        vga_buffer[i] = (uint16_t)strg[i] | ((uint16_t)color << 8);
        i++;
    }
    
}
