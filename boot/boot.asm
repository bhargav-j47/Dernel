global _start

extern kernel_main

section .text
_start:
    cli
    mov esp, stack_top      ;mov stack pointer to stack_top
    call kernel_main
    hlt
HLT_kernel:
    cli
    hlt
    jmp HLT_kernel
 
section .rodata

section .bss
    align 16
    resb 8192       ;reserve 8kb for stack
stack_top:


