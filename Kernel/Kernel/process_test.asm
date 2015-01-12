[BITS 32]

[GLOBAL ProcessTestThreadOne]
[GLOBAL ProcessTestThreadTwo]

[SECTION .text]

ProcessTestThreadOne:
    mov ebx, 50
    jmp ProcessTestThread
ProcessTestThreadTwo:
    mov ebx, 51
    ; Fall through
ProcessTestThread:
    mov edx, 26
    mov ecx, 21
.loop:
    push edx
    pop eax
    add eax, 'A'-1
    int 0x99
    dec edx
    jnz .loop
    mov edx, 26
    jmp .loop
    