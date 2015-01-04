; Kernel entrypoint
; Copyright (C) 2008-2010 Colin Munro

[BITS 32]

; Constants, reflected in linker script
	KERNEL_VIRTUAL_BASE		equ	0xC0100000
	KERNEL_PHYSICAL_BASE	equ	0x00100000

; Exports
[global entry]
[global BootPageDirectory]
[global PageMapBootstrap]

[global stack]

; From C/C++
[extern k_main] ; this is in the c file

; From the linker
extern start_ctors, end_ctors, start_dtors, end_dtors

STACKSIZE equ 0x4000                  ; that's 16k.

    MULTIBOOT_PAGE_ALIGN   equ 1<<0
    MULTIBOOT_MEMORY_INFO  equ 1<<1

    MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
    MULTIBOOT_HEADER_FLAGS equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
    CHECKSUM equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

SECTION mb-header ALIGN=4
    ; The Multiboot header (in NASM syntax)
        dd MULTIBOOT_HEADER_MAGIC
        dd MULTIBOOT_HEADER_FLAGS
        dd CHECKSUM

SECTION bootpagedir ALIGN=4096

KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE  >> 22)  ; Page directory index of kernel's 4MB PTE.

ALIGN 4096
BootPageDirectory:
	dd	(KERNEL_PHYSICAL_BASE & 0xFFC00000) | 0x81	; Present, 4MB (this page is only required until jumped into the kernel)
	times (KERNEL_PAGE_NUMBER - 1) dd 0	; Unmapped memory
	dd	(KERNEL_PHYSICAL_BASE & 0xFFC00000) | 0x83	; Present, writable, 4MB
	times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0	; Unmapped memory

SECTION bootpagedata ALIGN=4096 NOBITS

ALIGN 4096
PageMapBootstrap:
	resb 4096	; One page


SECTION .text

entry:
  ; Configure initial paging table for kernel
  
  mov ecx, (BootPageDirectory - (KERNEL_VIRTUAL_BASE & 0xFFC00000))
  mov cr3, ecx
  
  mov ecx, cr4
  or ecx, 0x00000010	; Enable 4MB pages
  mov cr4, ecx
  
  mov ecx, cr0
  or ecx, 0x80000000	; Enable paging
  mov cr0, ecx
  
  lea ecx, [reentry]
  jmp ecx
  
 reentry:
  ; Initialise stack
  
  mov esp, stack+STACKSIZE
  push eax
  push ebx
  
  ; Call all C++ constructors for static objects
  
static_ctors_loop:
  mov ebx, start_ctors
  jmp .test
.body:
  call [ebx]
  add ebx,4
.test:
  cmp ebx, end_ctors
  jb .body
  
  ; Jump to kernel main()
  
  call k_main

  ; Call all C++ destructors for static objects
  
static_dtors_loop:
  mov ebx, start_dtors
  jmp .test
.body:
  call [ebx]
  add ebx,4
.test:
  cmp ebx, end_dtors
  jb .body

  ; Halt

  cli  ; stop interrupts
ended:
  hlt ; halt the CPU
  jmp ended
  
section .bss
align 32
stack:
   resb STACKSIZE                     ; reserve 16k stack on a quadword boundary
