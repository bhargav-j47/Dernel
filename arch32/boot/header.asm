; multiboot header 

section .multiboot
    align 8
header_start:
    dd 0xe85250d6                   ; magic number
    dd 0                            ; architecture        
    dd header_end - header_start    ; header length

    ;checksum x100000000 is to avoid compiler warnings 
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

framebuffer_tag_start:
    dw 5                         ; Type
    dw 0                         ; Flags
    dd framebuffer_tag_end - framebuffer_tag_start ; Size = 20
    dd 1024                      ; Preferred Width
    dd 768                      ; Preferred Height
    dd 32                        ; Preferred Depth 
framebuffer_tag_end:

    ;end tag
    dd 0                            ; type
    dd 0                            ; flags
    dd 8                            ; size
header_end:
