#include <stddef.h>
#include <stdint.h>
#include <arch32/multiboot.h>
#include <arch32/vesa.h>
#include <arch32/kheap.h>
#include <lib/kstring.h>
#include <lib/printk.h>
#include <arch32/vmm.h>
#include <arch32/timer.h>

volatile uint8_t is_fb_avl = 0;
static vesa_display display;

void vesa_get_dis_dimensions(uint32_t *width, uint32_t *height) {
    *width = display.width;
    *height = display.height;
}

void vesa_scroll_up(uint32_t bg_color, uint32_t pixel_rows) {
    if (pixel_rows >= display.height) return;

    //move display backbuff up by one row
    size_t size = (display.height - pixel_rows) * display.pitch;
    kmemcpy(display.backbuff_addr,
            display.backbuff_addr + (pixel_rows * display.pitch),
            size);
    vesa_fill_rect(0, display.height - pixel_rows, display.width,
                   pixel_rows, bg_color);
}

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

void vesa_clear_screen() {
    kmemset(display.backbuff_addr, 0, display.buff_size);
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

    vesa_clear_screen();

    is_fb_avl = 1; //now vesa display is available
    
    // Background (Dark Slate)
    vesa_fill_rect(0, 0, display.width, display.height, 0x001E1E2E);

    // Head Base (Green Square: 320x320)
    vesa_fill_rect(350, 150, 320, 320, 0x0000AA00);

    // Eyes (Black: 80x80 each)
    vesa_fill_rect(390, 210, 80, 80, 0x00000000); // Left Eye
    vesa_fill_rect(550, 210, 80, 80, 0x00000000); // Right Eye

    // Nose (Black: 80x120)
    vesa_fill_rect(470, 290, 80, 120, 0x00000000);

    // Mouth corners (Black: 40x120 each)
    vesa_fill_rect(430, 330, 40, 120, 0x00000000); // Left Mouth
    vesa_fill_rect(550, 330, 40, 120, 0x00000000); // Right Mouth

    vesa_flush();
}
