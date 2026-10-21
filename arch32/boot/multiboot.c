#include <stdint.h>
#include <lib/printk.h>
#include <arch32/multiboot.h>
#include <arch32/pmm.h>
#include <arch32/vesa.h>

int parse_mem_multiboot(uint32_t magic, uint32_t mbinfo_ptr) {

    if (magic != MULTIBOOT2_MAGIC) {
        printk("multiboot bad magic address: %d",magic);
        return -2;
    }

    multiboot_tag* tag = (multiboot_tag*)(mbinfo_ptr + 8);

    while (tag->type != MULTIBOOT_TAG_TYPE_END) {
        if (tag->type == MULTIBOOT_TAG_TYPE_MEMMAP) {
            multiboot_tag_memmap* mtag = (multiboot_tag_memmap*)tag;

            uint8_t* entry_ptr = (uint8_t*)mtag->entries;
            uint8_t* mememmap_end = (uint8_t*)mtag + mtag->size;

            while (entry_ptr < mememmap_end) {
                multiboot_memmap_entry* entry = (multiboot_memmap_entry*)entry_ptr;

                if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
                    // pmm set available
                    pmm_init_region(entry->addr, entry->len);
                }
                entry_ptr += mtag->entry_size;
            }

        }

        // tags are 8 bit aligned so round current size to next mulitple of 8;
        tag = (multiboot_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7));
    }

    return 0;
}

int parse_framebuff_multiboot(uint32_t magic, uint32_t mbinfo_ptr, vesa_display* dis) {
    
    if (magic != MULTIBOOT2_MAGIC) {
        printk("multiboot bad magic address: %d",magic);
        return -2;
    }

    multiboot_tag* tag = (multiboot_tag*)(mbinfo_ptr + 8);
    while (tag->type != MULTIBOOT_TAG_TYPE_END) {
        if (tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFF) {
                multiboot_tag_framebuff* ftag = (multiboot_tag_framebuff*)tag;
                if (ftag->fb_type != 1) {
                    return -2;
                }
                dis->width = (uint32_t)ftag->width;
                dis->height = (uint32_t)ftag->height;
                dis->depth = (uint32_t)ftag->bpp;
                dis->pitch = (uint32_t)ftag->pitch;
                dis->buff_size = (uint32_t)(ftag->height * ftag->pitch);
                dis->frontbuff_addr = (uint8_t*)(ftag->addr);

                break;
        }
        // tags are 8 bit aligned so round current size to next mulitple of 8;
        tag = (multiboot_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7));
    }
    return 0;
}
