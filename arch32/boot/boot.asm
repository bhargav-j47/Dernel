global _start
global boot_page_dir

extern kernel_main

section .text
_start:
    cli
    ;mov stack pointer to stack_top
    mov esp, (stack_top - 0xC0000000)     
    push ebx                ;mulitboot info ptr req arg for kernel_main
    push eax                ;multiboot magic req arg for kernel_main

    ;=============setup paging==================== 
    ; paging is not set so we substract higher half 
    ; value xC0000000 for now cause linker expects 
    ; 0xC0000000 but physically present from 0x00000000
    ; grub puts kernel after 1Mb but we map from 0 
    ; cause we map vga mem and other multiboot info
    ; we map exactly 4MB
    mov edi, (boot_page_table - 0xC0000000)
    mov esi, 0
    mov ecx, 1024          ; map 1024 entries i.e 4MB

.fill_page_tale:
    mov edx, esi
    or edx, 3
    mov [edi], edx
    add esi, 4096          ; move to next physical frame 4kb
    add edi, 4             ; move to next page table entry
    loop .fill_page_tale   ; since ecx 1024 it will run for 1024 times i.e 4MB
    
    ; setup boot page dir to boot_page_table
    mov eax, (boot_page_table - 0xC0000000)
    or eax, 3
    mov ebx, (boot_page_dir - 0xC0000000)

    ; first map virtual 1st 4MB to physcical 4MB
    mov [ebx], eax
    ; now map higher map 0xC0000000 to phycial 4MB
    ; 0xC0000000 = 3GB, 3072 MB  each page_dir 4MB
    ; so 3072/4= 768th dir we need.
    ; 1 page dir entry = 32 bits = 4 bytes
    ; memory addr= 768 * 4 = 3072 
    ; pretty confusing :) took me 30 min to understand this :(
    mov [ebx + 3072], eax

    ; now finally setup paging
    mov cr3, ebx    ; ebx holds boot_page_dir
 
    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax

    ; now jump to higher half kernel
    mov eax, .higher_half
    jmp eax

.higher_half:
    ; Add 3GB to stack pointer
    add esp, 0xC0000000
    
    ; unmap identity mapping 
    mov dword [boot_page_dir], 0    ; mov value 0 to 1st page dir entry
    mov ebx, cr3
    mov cr3, ebx

    ;call kernel_main
    call kernel_main
    hlt

.HLT_kernel:
    cli
    hlt
    jmp .HLT_kernel
 
section .rodata

section .bss
    align 16
    resb 8192       ;reserve 8kb for stack
stack_top:
    align 4096
boot_page_dir:
    resb 4096
boot_page_table:
    resb 4096
