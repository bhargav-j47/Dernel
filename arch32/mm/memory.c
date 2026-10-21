#include <stdint.h>
#include <arch32/memory.h>
#include <arch32/pmm.h>
#include <arch32/vmm.h>
#include <arch32/kheap.h>
#include <lib/printk.h>

extern uint32_t _kernel_end;

void init_memory(uint32_t magic, uint32_t mbinfo_ptr) {
    //run pmm
    init_pmm(magic, mbinfo_ptr);

    //init vmm
    init_vmm();

    //setup heap
    //get the heap base by adding pmm_bitmap_size and aligning to 4KB
    uint32_t heap_base = (pmm_bitmap_end + 4095) & ~4095;
    init_kheap(heap_base);
}
