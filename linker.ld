ENTRY(_start)

SECTIONS
{
    /*  Begin putting sections at 1 Megabyte */
    /* . = 1M; */

    /*  Begin putting sections at 0xC0000000 */
    . = 0xC0100000; 

    _kernel_start = .;
    
    /* AT to set physical address for grub */
    .boot ALIGN(4K) : AT(ADDR(.boot) - 0xC0000000) {
        *(.multiboot)
    }

    .text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000) {
        *(.text)
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000) {
        *(.rodata)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000) {
        *(.data)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000) {
        *(COMMON)     /* for compilers uninitialised */
        *(.bss)
    }
    
    /* needed for pmm */
    . = ALIGN(4096);
    _kernel_end = .;
}
