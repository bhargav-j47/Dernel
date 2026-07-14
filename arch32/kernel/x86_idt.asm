global x86_load_idt

x86_load_idt:
    mov eax, [esp+4]
    lidt [eax]
    ret

