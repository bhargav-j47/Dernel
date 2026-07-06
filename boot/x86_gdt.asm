global x86_load_gdt

x86_load_gdt:
    mov eax, [esp + 4]  ;get function attribute to eax
    lgdt [eax]          ; load gdt
    
    ; reload data segments
    ; kernel data segment index is 2 
    ; 2 * 8 bytes per descriptor = 16 (0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; reload code segment using far jump
    ; kerel code seg index is 1
    ; 1 * 8 = 8 (0x08)
    jmp 0x08:.flush_cs

.flush_cs:
    ret


global tss_flush

tss_flush:
    mov ax, 0x28        ; The offset in the GDT to our TSS (Index 5)
    ltr ax              ; Load Task Register
    ret
