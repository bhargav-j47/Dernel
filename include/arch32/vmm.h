#pragma once
#include <stdint.h>

// Hardware flags for Page Directory and Page Table Entries
#define PAGE_PRESENT       0x001  // Bit 0: Page is in memory
#define PAGE_WRITE         0x002  // Bit 1: 0 = Read-Only, 1 = Read/Write
#define PAGE_USER          0x004  // Bit 2: 0 = Kernel only, 1 = Ring 3 (User) access
#define PAGE_WRITE_THROUGH 0x008  // Bit 3: Hardware cache write-through
#define PAGE_CACHE_DISABLE 0x010  // Bit 4: Hardware cache disable (use for VESA/MMIO)
#define PAGE_ACCESSED      0x020  // Bit 5: CPU sets this when page is read/written
#define PAGE_DIRTY         0x040  // Bit 6: CPU sets this when page is written (PTE only)
#define PAGE_SIZE          0x080  // Bit 7: PDE only - 0 = 4KB page table, 1 = 4MB page
#define PAGE_GLOBAL        0x100  // Bit 8: Don't flush from TLB on CR3 switch (PGE flag)

// masks for extracting parts of a 32-bit entry
#define PAGE_FRAME_MASK    0xFFFFF000 // Extracts the 20-bit physical address
#define PAGE_FLAGS_MASK    0x00000FFF // Extracts the 12-bit hardware flags

void init_vmm();

void vmm_alloc_page(uint32_t page, uint32_t flags);
void vmm_free_page(uint32_t page);
