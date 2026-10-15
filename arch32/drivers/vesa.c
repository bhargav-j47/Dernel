#include <stdint.h>
#include <arch32/multiboot.h>
#include <arch32/vesa.h>
#include <arch32/kheap.h>
#include <lib/kstring.h>
#include <lib/printk.h>
#include "arch32/vmm.h"

volatile uint8_t is_fb_avl = 0;
static vesa_display display;

void vesa_put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= display.width || y >= display.height) return;

    //Calculate exact byte offset using pitch and depth (bpp / 8)
    //Then Cast byte address to a 32-bit pixel pointer and write the color
    uint32_t byte_offset = (y * display.pitch) + (x * (display.depth / 8));
    *(uint32_t*)(display.backbuff_addr + byte_offset) = color;
}

void vesa_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    for (uint32_t row = 0; row < h; row++) {
        for (uint32_t col = 0; col < w; col++) {
            vesa_put_pixel(x + col, y + row, color);
        }
    }
}

void vesa_flush(void) {
    if (!is_fb_avl) return;
    kmemcpy(display.frontbuff_addr, display.backbuff_addr, display.buff_size);
}


void init_vesa(uint32_t magic, uint32_t mbinfo_ptr) {
    printk("initiating vesa graphics display\n");

    vesa_display* dis = &display;
    int res = parse_framebuff_multiboot(magic, mbinfo_ptr, dis);
    if (res < 0) {
        //panic 
        return;
    }

    // frontbuff_addr passed by multiboot2 is physical and not virtual
    // so map it to virtual mmio
    //
    dis->frontbuff_addr =
        (uint8_t*)vmm_map_mmio((uint32_t)dis->frontbuff_addr, dis->buff_size);

    if(dis->frontbuff_addr == NULL) {
        //panic
        return;
    }

    dis->backbuff_addr = (uint8_t*)kmalloc(dis->buff_size);
    if (dis->backbuff_addr == NULL) {
        //panic
        return;
    }
    kmemset(dis->backbuff_addr, 0, dis->buff_size);

    is_fb_avl = 1; //now vesa display is available
    
}

