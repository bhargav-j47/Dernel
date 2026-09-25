#include <stdint.h>
#include <arch32/memory.h>
#include <arch32/pmm.h>
#include <arch32/vmm.h>
#include "arch32/kheap.h"

extern uint32_t _kernel_end;

void init_memory(uint32_t magic, uint32_t mbinfo_ptr) {
    //run pmm
    init_pmm(magic, mbinfo_ptr);

    //init vmm
    init_vmm();

    //setup heap
    init_kheap(_kernel_end);
}
