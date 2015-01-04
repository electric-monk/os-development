; CPU Descriptor table interface code
; Copyright (C) 2008 Colin Munro
[BITS 32]

[global gdt_flush]
[global idt_load]

gdt_flush:
	mov eax, [esp+4]  ; Get the pointer to the GDT, passed as a parameter.
	lgdt [eax]        ; Load the new GDT pointer
	jmp 0x08:.flush   ; 0x08 is the offset to our code segment: Far jump!
.flush:
	mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
	mov ds, ax        ; Load all data segment selectors
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	ret

idt_load:
	mov eax, [esp+4]
	lidt [eax]
	ret
