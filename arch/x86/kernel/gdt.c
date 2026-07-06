#include <stdint.h>
#include <vga.h>
#include <kstring.h>
#include <gdt.h>

// helper macros
// for flags
#define GDT_FLAG_GRAN_1B    0x00
#define GDT_FLAG_GRAN_4K    0x80
#define GDT_FLAG_32B        0x40
#define GDT_FLAG_64B        0x20

//for access byte
#define GDT_ACCESS_PRESENT  0x80
#define GDT_ACCESS_RING0    0x00
#define GDT_ACCESS_RING3    0x60
#define GDT_ACCESS_DS       0x10
#define GDT_ACCESS_CS       0x18
#define GDT_ACCESS_TSS      0x09    // 0b00001001
#define GDT_ACCESS_DIR_0    0x00
#define GDT_ACCESS_DIR_1    0x04
#define GDT_ACCESS_CONF_0   0x00
#define GDT_ACCESS_CONF_1   0x04
#define GDT_ACCESS_CS_R     0x02
#define GDT_ACCESS_DS_RW    0x02

#define GET_GDT_LIMITL(limit)       ((limit) & 0xFFFF)
#define GET_GDT_BASEL(base)         ((base) & 0xFFFF)
#define GET_GDT_BASEM(base)         (((base) >> 16) & 0xFF)
#define GET_GDT_FLAGS(flags, limit) ((((limit) >> 16) & 0xF) | ((flags) & 0xF0))
#define GET_GDT_BASEH(base)         (((base) >> 24) & 0xFF)

#define GDT_SEG(base, limit, access, flags) {      \
    GET_GDT_LIMITL(limit),                         \
    GET_GDT_BASEL(base),                           \
    GET_GDT_BASEM(base),                           \
    (access),                                      \
    GET_GDT_FLAGS(flags, limit),                   \
    GET_GDT_BASEH(base)                            \
}

typedef struct {
    uint16_t limit;                 // limit low 16 bytes
    uint16_t base_l;                // base low 16 bytes
    uint8_t base_m;                 // base middle 8 bytes
    uint8_t access;                 // access byte
    uint8_t flags;                  // flags 4 byte + limit high 4 bytes
    uint8_t base_h;                 // base high 8 bytes
} __attribute__((packed)) segment_descriptor;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) GDT_struct;

typedef struct {
    uint32_t prev_tss;              // req for hardware task switch
    uint32_t esp0;                  // ring0 stack pointer
    uint32_t ss0;                   // ring0 stack segment
    uint32_t esp1, ss1;             // ring1 esp, ss unused
    uint32_t esp2, ss2;             // ring2 esp, ss unused
    uint32_t cr3, eip, eflags, eax, ecx;
    uint32_t edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;                   // local desc. table not used
    uint16_t trap, iomap_base;
} __attribute__((packed)) tss_entry_t;

tss_entry_t tss_entry;              //tss struct

extern void x86_load_gdt(GDT_struct* gdt);

extern void x86_flush_tss();

void set_tss_kernel_stack(uint32_t stack) {
    tss_entry.esp0 = stack;
}

void init_gdt() {
    printk("setting up gdt\n");

    //set up TSS data
    kmemset(&tss_entry, 0, sizeof(tss_entry_t));
    tss_entry.ss0 = 0x10;                       // Kernel Data Segment offset 2*8=16
    set_tss_kernel_stack(0);                    // To be updated on context switch
    tss_entry.iomap_base = sizeof(tss_entry_t);


    segment_descriptor seg_desc_entries[]= {

        //null descritptor
        GDT_SEG(0, 0, 0, 0),

        //kernel code descritptor
        GDT_SEG(0, 0xFFFF,
                GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CS |
                GDT_ACCESS_CONF_0  | GDT_ACCESS_CS_R,
                GDT_FLAG_GRAN_4K   | GDT_FLAG_32B ),

        //kernel data descritptor
        GDT_SEG(0, 0xFFFF,
                GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DS |
                GDT_ACCESS_DIR_0   | GDT_ACCESS_DS_RW,
                GDT_FLAG_GRAN_4K   | GDT_FLAG_32B ),

        //user code descritptor
        GDT_SEG(0, 0xFFFF,
                GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_CS |
                GDT_ACCESS_CONF_0  | GDT_ACCESS_CS_R,
                GDT_FLAG_GRAN_4K   | GDT_FLAG_32B ),

        //user data descritptor
        GDT_SEG(0, 0xFFFF,
                GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_DS |
                GDT_ACCESS_DIR_0   | GDT_ACCESS_DS_RW,
                GDT_FLAG_GRAN_4K   | GDT_FLAG_32B ),

        //TSS
        GDT_SEG((uint32_t)&tss_entry, sizeof(tss_entry_t) - 1,
                GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_TSS,
                0 ),
    };

    GDT_struct GDT_ptr;
    GDT_ptr.limit = (sizeof(segment_descriptor) * 6) - 1;
    GDT_ptr.base  = (uint32_t)seg_desc_entries;

    x86_load_gdt(&GDT_ptr);
    printk("GDT is loaded\n");
}
