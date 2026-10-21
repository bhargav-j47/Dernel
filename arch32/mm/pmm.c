#include <stdint.h>
#include <lib/printk.h>
#include <arch32/multiboot.h>
#include <arch32/pmm.h>
#include <lib/kstring.h>

static uint8_t* pmm_bitmap = NULL;
static uint32_t pmm_total_frames;
static uint32_t pmm_used_frames = 0;

// Helper macros for bitwise operations
static inline void bitmap_set(uint32_t frame) {
    pmm_bitmap[frame / BLOCKS_PER_BYTE] |= (1 << (frame % BLOCKS_PER_BYTE));
}

static inline void bitmap_clear(uint32_t frame) {
    pmm_bitmap[frame / BLOCKS_PER_BYTE] &= ~(1 << (frame % BLOCKS_PER_BYTE));
}

static inline bool bitmap_test(uint32_t frame) {
    return pmm_bitmap[frame / BLOCKS_PER_BYTE] & (1 << (frame % BLOCKS_PER_BYTE));
}

void pmm_init_region(uint64_t start_addr, uint64_t length) {
    uint64_t end_addr = start_addr + length;

    //Align the start address UP to the nearest 4KB boundary
    uint64_t aligned_start = (start_addr + FRAME_SIZE - 1) / FRAME_SIZE * FRAME_SIZE;
    uint64_t aligned_end = end_addr / FRAME_SIZE * FRAME_SIZE;

    if (aligned_start >= aligned_end) {
        return;
    }

    for (uint64_t addr = aligned_start; addr < aligned_end; addr += FRAME_SIZE) {
        uint32_t frame = addr / FRAME_SIZE;
        if (frame >= pmm_total_frames) {
            break;
        }
        bitmap_clear(frame);
    }
}


extern uint32_t* _kernel_end;
extern uint32_t* _kernel_start;
uint32_t pmm_bitmap_size;
uint32_t pmm_bitmap_end; 

void init_pmm(uint32_t magic, uint32_t mbinfo_ptr) {

    printk("initializing physical memory\n");

    /*
     * round kernel end addr to next 4kb
     * place bitmap after kernel_end
     */
    uint32_t kernel_start = (uint32_t)&_kernel_start;
    uint32_t kernel_end = (uint32_t)&_kernel_end;
    kernel_end = (kernel_end + FRAME_SIZE -1) & ~(FRAME_SIZE -1);

    /* 
     * This is because pmm_bitmap array should not 
     * overwrite the multiboot info itself
     * If GRUB placed the multiboot data after kernel,
     * push the kernel_end forward so we don't overwrite it!
     */
    uint32_t mb_size = *(uint32_t*)mbinfo_ptr;
    uint32_t mb_end = mbinfo_ptr + mb_size;
    if (mb_end > kernel_end) {
        kernel_end = mb_end;
    }

    pmm_total_frames = (4ULL * 1024 * 1024 * 1024) / FRAME_SIZE; // 4 GB
    pmm_bitmap_size = pmm_total_frames / BLOCKS_PER_BYTE;
    pmm_bitmap = (uint8_t*) kernel_end;
    pmm_bitmap_end = (uint32_t)pmm_bitmap + pmm_bitmap_size;
   
    //set all as used
    kmemset(pmm_bitmap, 0xFF, pmm_bitmap_size);

    //parse memory map
    parse_mem_multiboot(magic, mbinfo_ptr);

    //reserve the used kernel memory
    uint32_t phys_kernel_start = (uint32_t) kernel_start - 0xC0000000 ;
    uint32_t phys_kernel_end =  (uint32_t) kernel_end - 0xC0000000;
    uint32_t kernel_start_frame = phys_kernel_start / FRAME_SIZE;
    uint32_t kernel_end_frame = (phys_kernel_end + pmm_bitmap_size) / FRAME_SIZE;

    for (uint32_t i = kernel_start_frame; i <= kernel_end_frame; i++) {
        bitmap_set(i);
        pmm_used_frames++;
    }

    //reserve the first 1mb though bios reserves it but it lefts first 640kb
    //as free which make pmm_alloc_frame to return 0th frame index which is 
    //used for error so we explicitely reserve 1st mb
    for (uint32_t i =0; i < 256; i++) {
        bitmap_set(i);
        pmm_used_frames++;
    }   
}

uint32_t pmm_alloc_frame() {
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
                printk("%d \n", pmm_total_frames);
                return 0;
            }

            bitmap_set(frame_idx);
            pmm_used_frames++;

            // convert to physical addr and return
            return (uint32_t)(frame_idx * FRAME_SIZE);
        }

    }
    return 0;
}

void pmm_free_frame(uint32_t frame) {

    uint32_t frame_idx = (uint32_t)(frame) / FRAME_SIZE;

    if (frame_idx >= pmm_total_frames) {
        return;
    }

    bitmap_clear(frame_idx);
    pmm_used_frames--;
}
