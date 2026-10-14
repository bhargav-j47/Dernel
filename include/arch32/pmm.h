#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * pmm bitmap represents array for our frame 0 means avl 1 means used
 * one index in bitmap represents 8 frames i.e BLOCKS_PER_BYTE 
 * 1 byte 8 bit each bit one frame
 * 11001000 means frame 0,1,2,4,5 is used 
 * let frame 102 is needed so bitmap index is 102/16 and offset is 102%16
 */

#define FRAME_SIZE 4096
#define BLOCKS_PER_BYTE 8

extern uint32_t pmm_bitmap_size;

uint32_t pmm_alloc_frame();
void pmm_free_frame(uint32_t frame);

void pmm_init_region(uint64_t start_addr, uint64_t length);
void init_pmm(uint32_t magic, uint32_t mbinfo_ptr);
