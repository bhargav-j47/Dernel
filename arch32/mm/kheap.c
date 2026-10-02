#include <stdint.h>
#include <arch32/pmm.h>
#include <arch32/vmm.h>
#include <arch32/kheap.h>
#include "lib/printk.h"

uint32_t heap_start;
uint32_t current_heap_addr;
uint32_t heap_end;

heap_mem_struct* heap_head;

static void expand_heap(uint32_t size);

static void expand_heap(uint32_t size) {
    //total size will actual heap needed + linked list struct overhead
    uint32_t total_size = size + sizeof(heap_mem_struct);
    //calcualte no of pages by aligning to 4k
    uint32_t pages_req = (total_size + 4095) / 4096;

    if (current_heap_addr + total_size >= heap_end) {
        printk("panic: kernel has exceeded its heap capacity");
        return;
    }

    uint32_t expand_start_addr = current_heap_addr;

    for (uint32_t i=0; i < pages_req; i++) {
        
        //allocate page and move current_heap_addr
        vmm_alloc_page(current_heap_addr, PAGE_WRITE | PAGE_PRESENT);
        current_heap_addr += 4096;
    }
    
    heap_mem_struct* new_block = (heap_mem_struct*) expand_start_addr;
    new_block->size = (pages_req * 4096) - sizeof(heap_mem_struct);
    new_block->magic = HEAP_MAGIC;
    new_block->isfree = 1;
    new_block->next = NULL;

    if (heap_head == NULL) {
        heap_head = new_block;
        return;
    }

    heap_mem_struct* iter = heap_head;
    while (iter->next != NULL) {
        iter = iter->next;
    }
    iter->next = new_block;
}

void* kmalloc(uint32_t size) {
    if (size == 0) return 0;

    try_allocation:
    size = (size + 15) & ~15;
    heap_mem_struct* iter = heap_head;

    while (iter != NULL) {
        if(iter->isfree && iter->size >= size) {

            // if free block is larger than what we want then split them into two
            if (iter->size > size + sizeof(heap_mem_struct) + 4 ) {
                uint32_t new_block_addr = (uint32_t)iter + sizeof(heap_mem_struct)
                                            + size;
                heap_mem_struct* new_block = (heap_mem_struct*) new_block_addr;
                new_block->magic = HEAP_MAGIC;
                new_block->isfree = 1;
                new_block->size = iter->size - size - sizeof(heap_mem_struct);
                new_block->next = iter->next;

                iter->size = size;
                iter->next = new_block;
            }
            // now mark current block as used and return memory addr
            iter->isfree = 0;
            return (void*)(uint32_t)iter + sizeof(heap_mem_struct);
        }
        iter = iter->next;
    }
    //if we came here then no free block is there in current heap so 
    //allocate more space for heap by expanding heap by size
    expand_heap(size);
    //now retry allocation
    goto try_allocation;
}

void kfree(void *ptr) {
    if (ptr == NULL) return;

    heap_mem_struct* block = (heap_mem_struct*)((uint32_t) ptr - sizeof(heap_mem_struct));

    if(block->magic != HEAP_MAGIC){
        //this block was corrupted
        printk("kernel panic: heap is corrupted");
        return;
    }
    
    block->isfree = 1;
    
    //merge multiple blocks if possiblet 
    heap_mem_struct* iter = heap_head;
    while (iter != NULL && iter->next != NULL) {
        if (iter->isfree && iter->next->isfree) {
            //just check if two blocks we want to merge are virtually continuous or not
            //helpful if heap is broken into pieces
            uint32_t expected_addr = (uint32_t)iter + sizeof(heap_mem_struct) + iter->size;
            if (expected_addr == (uint32_t)iter->next) {
                iter->size += iter->next->size + sizeof(heap_mem_struct);
                iter->next = iter->next->next;
                continue;
            }
        } 
        iter = iter->next;
    }
}

void init_kheap(uint32_t _heap_start) {
    heap_start = _heap_start;
    heap_end = heap_start + (256 * 1024 * 1024);
    current_heap_addr = heap_start;
    heap_head = NULL;
    // allocate 1Mb heap at start
    expand_heap(1024 * 1024);
}
