#include <stdint.h>
#include <arch32/pmm.h>
#include <arch32/vmm.h>
#include "lib/printk.h"

// Extracts Page Directory Index
#define PG_DIR_INDEX(address) ((address) >> 22)
// Extracts Page Table Index
#define PG_TBL_INDEX(address) (((address) >> 12) & 0x03FF)
// Extracts flags offset
#define PG_OFFSET(address) ((address) & 0x0FFF)

extern uint32_t boot_page_dir[];
uint32_t* page_directory = (uint32_t*) 0xFFFFF000;

void vmm_map_page(uint32_t page, uint32_t frame, uint32_t flags);
void vmm_unmap_page(uint32_t page);

void vmm_map_page(uint32_t page, uint32_t frame, uint32_t flags) {
    //align to 4kb
    page  &= PAGE_FRAME_MASK;
    frame &= PAGE_FRAME_MASK;
    
    uint32_t pg_dir_idx = PG_DIR_INDEX(page);
    uint32_t pg_tbl_idx = PG_TBL_INDEX(page);
    
    // if page table iteself is not present then first create page
    // table itslef
    if ((page_directory[pg_dir_idx] & PAGE_PRESENT) == 0) {

        //allocate new frame for page table and add it in page dir
        uint32_t new_frame = pmm_alloc_frame();
        page_directory[pg_dir_idx] = new_frame | (PAGE_PRESENT | 
                                     PAGE_WRITE | PAGE_USER);

        // calculate new page tables virtual addr 
        // 0xFFC00000 is just start of page dir 
        // i.e top 10 bit = 1023 and middle 10 bit= 0
        uint32_t* pg_tbl_virt_addr = (uint32_t*)(0xFFC00000 + (pg_dir_idx * 4096));

        // flush tlb
        asm volatile("invlpg [%0]" :: "r" (0xFFFFF000) : "memory");

        // zero out whole page table
        for (int i = 0; i < 1024; i++) {
            pg_tbl_virt_addr[i] = 0;
        }
    }
    //map actual requested frame to actual requested page
    uint32_t* pg_tbl =  (uint32_t*)(0xFFC00000 + (pg_dir_idx * 4096));
    pg_tbl[pg_tbl_idx] = frame | PG_OFFSET(flags);

    // flush tlb
    asm volatile("invlpg [%0]" :: "r" (0xFFFFF000) : "memory");
}

void vmm_unmap_page(uint32_t page) {
    page &= PAGE_FRAME_MASK;

    uint32_t pg_dir_idx = PG_DIR_INDEX(page);
    uint32_t pg_tbl_idx = PG_TBL_INDEX(page);

    if ((page_directory[pg_dir_idx] & PAGE_PRESENT) == 0) return;
    
    uint32_t* pg_tbl   = (uint32_t*)(0xFFC00000 + (pg_dir_idx * 4096));
    pg_tbl[pg_tbl_idx] = 0;

    asm volatile("invlpg [%0]" :: "r" (0xFFFFF000) : "memory");
}

void vmm_alloc_page(uint32_t page, uint32_t flags) {
    uint32_t frame = pmm_alloc_frame();

    if (frame == 0) {
        //panic here
        printk("panic can't allocate a new frame");
        return;
    }

    vmm_map_page(page, frame, flags);
}

void vmm_free_page(uint32_t page) {
    page &= PAGE_FRAME_MASK;

    uint32_t pg_dir_idx = PG_DIR_INDEX(page);
    uint32_t pg_tbl_idx = PG_TBL_INDEX(page);

    if ((page_directory[pg_dir_idx] & PAGE_PRESENT) == 0) return;

    uint32_t* pg_tbl   = (uint32_t*)(0xFFC00000 + (pg_dir_idx * 4096));
    uint32_t pg_tbl_entry = pg_tbl[pg_tbl_idx];
    
    if (pg_tbl_entry & (PAGE_PRESENT)) {
        uint32_t frame = pg_tbl_entry | PAGE_FRAME_MASK;
        pmm_free_frame(frame);
        vmm_unmap_page(page);
    }
}

void init_vmm() {
    printk("initializing virtual memory");

    // get the current physical frame of boot_page_dir from cr3
    uint32_t pd_frame;
    asm volatile("mov %0, cr3" : "=r" (pd_frame));
    
    // setup recursive paging for index 1023 
    // this means last entry in page dir points to page dir itself
    // since after paging we require to use virtual addr only
    // so to modify page_dir we will need page dir but page dir does
    // not map page dir itself this is solve by mappig page dir to 1023
    // index, i.e virtual page 0xFFFFF000 which mean top 10 byes to 1023
    // middle 10 bytes to 1023 so cpu reads 1023 and goes to page dir[1023]
    // this points to page_dir iteself, but cpu thinks it as page table and 
    // goes to middle 10 bytes i.e 1023 which points to page dir here you can
    // now edit page dir itself, and by changing middle 10 bytes you can 
    // access/edit exact page table as well.
    // pretty confusing hope i get it after i read it again. :)
    boot_page_dir[1023] = pd_frame | PAGE_PRESENT | PAGE_WRITE;

    //Reload CR3 to flush the TLB
    asm volatile("mov cr3, %0" :: "r" (pd_frame));
}
