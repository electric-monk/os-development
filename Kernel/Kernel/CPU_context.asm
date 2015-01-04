[BITS 32]

[GLOBAL SwitchContexts]

SwitchContexts: ; void SwitchContexts(Context **old, Context *new);
    mov eax, [esp+4]
    mov edx, [esp+8]
    
    ; Save old callee-save registers
    push ebp
    push ebx
    push esi
    push edi
    
    ; Switch stacks
    mov [eax], esp
    mov esp, edx

    ; Load new callee-save registers
    pop edi
    pop esi
    pop ebx
    pop ebp
    
    ret
