; multiboot header 

section .multiboot
    align 8
header_start:
    dd 0xe85250d6                   ; magic number
    dd 0                            ; architecture        
    dd header_end - header_start    ; header length

    ;checksum x100000000 is to avoid compiler warnings 
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ;end tags 
    dd 0                            ; type
    dd 0                            ; flags
    dd 8                            ; size
header_end:
