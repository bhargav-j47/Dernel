#include <stdint.h>
#include <lib/printk.h>
#include <arch32/multiboot.h>
#include <arch32/pmm.h>
#include <lib/kstring.h>

void pmm_init_region(uint64_t start_addr, uint64_t length);

void pmm_init_region(uint64_t start_addr, uint64_t length) {
    uint64_t end_addr = start_addr + length;

    //Align the start address UP to the nearest 4KB boundary
    uint64_t aligned_start = (start_addr + PAGE_SIZE - 1) / PAGE_SIZE * PAGE_SIZE;
    uint64_t aligned_end = end_addr / PAGE_SIZE * PAGE_SIZE;

    if (aligned_start >= aligned_end) {
        return;
    }

    for (uint64_t addr = aligned_start; addr < aligned_end; addr += PAGE_SIZE) {
        uint32_t frame = addr / PAGE_SIZE;
        if (frame >= pmm_total_frames) {
            break;
        }
        bitmap_clear(frame);
    }
}

int parse_multiboot(uint32_t magic, uint32_t mbinfo_ptr) {

    if (magic != MULTIBOOT2_MAGIC) {
        printk("multiboot bad magic address: %d",magic);
        return -2;
    }

    multiboot_tag *tag = (multiboot_tag*)(mbinfo_ptr + 8);

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

extern uint32_t* _kernel_end;
extern uint32_t* _kernel_start;

void pmm_init(uint32_t magic, uint32_t mbinfo_ptr) {

    printk("initializing physical memory");

    /*
     * round kernel end addr to next 4kb
     * place bitmap after kernel_end
     */
    uint32_t kernel_start = (uint32_t)&_kernel_start;
    uint32_t kernel_end = (uint32_t)&_kernel_end;
    kernel_end = (kernel_end + PAGE_SIZE -1) & ~(PAGE_SIZE -1);

    pmm_total_frames = (4ULL * 1024 * 1024 * 1024) / PAGE_SIZE; // 4 GB
    uint32_t bitmap_size = pmm_total_frames / BLOCKS_PER_BYTE;

    pmm_bitmap = (uint8_t*) kernel_end;

    //set all as used
    kmemset(pmm_bitmap, 0xFF, bitmap_size);

    //parse memory map
    parse_multiboot(magic, mbinfo_ptr);

    //reserve the used kernel memory
    uint32_t kernel_start_frame = kernel_start / PAGE_SIZE;
    uint32_t kernel_end_frame = (kernel_end + bitmap_size) / PAGE_SIZE;

    for (uint32_t i = kernel_start_frame; i <= kernel_end_frame; i++) {
        bitmap_set(i);
    }

    return;
}

void* pmm_alloc_frame() {
    // iterate to check free frame and allocate and return it
    for (uint32_t i=0; i < pmm_total_frames/BLOCKS_PER_BYTE; i++) {

        //if 0xFF means 11111111 so all 8 frames are used
        if(pmm_bitmap[i] == 0xFF) continue;

        //find free frame from exact 8 frames
        for (uint32_t bit=0; bit < 8; bit++) {
            // if it is 1 it is used
            if (pmm_bitmap[i] & (1 << bit)) continue;

            uint32_t frame_idx = (i * BLOCKS_PER_BYTE) + bit;

            if (frame_idx >= pmm_total_frames) {
                return NULL;
            }

            bitmap_set(frame_idx);

            // convert to physical addr and return
            return (void*)(frame_idx * PAGE_SIZE);
        }

    }
    return NULL;
}

void pmm_free_frame(void *frame) {

    uint32_t frame_idx = (uint32_t)(frame) / PAGE_SIZE;

    if (frame_idx >= pmm_total_frames) {
        return;
    }

    bitmap_clear(frame_idx);
}

