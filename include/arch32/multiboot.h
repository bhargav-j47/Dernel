#pragma once
#include <stdint.h>
#include <arch32/vesa.h>

#define MULTIBOOT2_MAGIC 0x36d76289
#define MULTIBOOT_TAG_TYPE_END 0
#define MULTIBOOT_TAG_TYPE_MEMMAP 6
#define MULTIBOOT_TAG_TYPE_FRAMEBUFF 8

#define MULTIBOOT_MEMORY_AVAILABLE        1
#define MULTIBOOT_MEMORY_RESERVED         2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS              4
#define MULTIBOOT_MEMORY_BADRAM           5

typedef struct {
    uint32_t type;
    uint32_t size;
} multiboot_tag;

typedef struct {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t zero;
} __attribute__((packed)) multiboot_memmap_entry;

typedef struct {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    multiboot_memmap_entry entries[0]; // Flexible array member
} __attribute__((packed)) multiboot_tag_memmap;

typedef struct { 
    uint32_t type;
    uint32_t size;
    uint64_t addr;              // Physical address of VRAM
    uint32_t pitch;             // Bytes per scanline (Wait! Use pitch, not width * 4)
    uint32_t width;             // Pixels per row
    uint32_t height;            // Pixels per column
    uint8_t  bpp;               // Bits per pixel / depth (usually 32)
    uint8_t  fb_type;           // 1 = RGB linear framebuff 2 = EGA text
    uint16_t reserved;
} __attribute__((packed)) multiboot_tag_framebuff;

int parse_mem_multiboot(uint32_t magic, uint32_t mbinfo_ptr);
int parse_framebuff_multiboot(uint32_t magic, uint32_t mbinfo_ptr, vesa_display* dis);
