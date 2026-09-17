ENTRY(_start)

SECTIONS
{
    /*  Begin putting sections at 1 Megabyte */
    . = 1M;

    _kernel_start = .;

    .boot BLOCK(4K) : ALIGN(4K) {
        *(.multiboot)
    }

    .text BLOCK(4K) : ALIGN(4K) {
        *(.text)
    }

    .rodata BLOCK(4K) : ALIGN(4K) {
        *(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K) {
        *(.data)
    }

    .bss BLOCK(4K) : ALIGN(4K) {
        *(COMMON)     /* for compilers uninitialised */
        *(.bss)
    }
    
    /* needed for pmm */
    . = ALIGN(4096);
    _kernel_end = .;
}
