#pragma once
#include <stdint.h>

#define HEAP_MAGIC  0xC101C0DE

typedef struct heap_mem_struct {
    uint32_t magic;
    uint32_t size;
    uint32_t isfree;
    struct heap_mem_struct* next;
} heap_mem_struct;  //16 bytes size

void init_kheap(uint32_t _heap_start);
void* kmalloc(uint32_t size);
void kfree(void* ptr);
