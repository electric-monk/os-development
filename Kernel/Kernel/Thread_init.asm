[BITS 32]

[GLOBAL ThreadEntryPoint]
[GLOBAL KernelThreadEntryPoint]
[GLOBAL AtomicIncrement]
[GLOBAL AtomicDecrement]

[GLOBAL GetEflag]

[SECTION .text]

GetEflag:
    pushfd
    pop eax
    ret

KernelThreadEntryPoint:
    ; We're not switching task levels, so iret does not remove ESP/SS from the stack
    add esp, 0x8
    ; Otherwise, it's the same!
ThreadEntryPoint: ; expects stack pre-configured for callback and context - like (void)ThreadEntryPoint(void *function, void *context)
    ; Get the address of the function
    pop edx
    ; Call the function, passing through the parameter that's already on the stack
    call edx
    ; Tell our object we stopped
    pop ebx
    mov eax, 0x3    ; THREAD_FUNCTION_TERMINATE
    int 0x83        ; THREAD_SYSCALL
    ; End of thread! Shouldn't really ever get here.
.deadloop:
    hlt
    jmp .deadloop

AtomicIncrement:    ; return 1 if it's nonzero after
    mov ecx, [esp+4]
    lock inc dword [ecx]
    mov ah, 0
    setnz al
    ret
AtomicDecrement:    ; return 1 if it's nonzero after
    mov ecx, [esp+4]
    lock dec dword [ecx]
    mov ah, 0
    setnz al
    ret
