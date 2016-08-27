; Userspace entrypoint
; Copyright (C) 2008-2015 Colin Munro

[BITS 32]

; Exports
[global entry]
[global testprint]

; From C/C++
[extern presysmain] ; this is in the c file
[extern __cxa_finalize] ; C++ runtime support

; From the linker
extern start_ctors, end_ctors, start_dtors, end_dtors, start_init_array, end_init_array

testprint:
  mov eax, [esp+4]
  int 0xff
  ret

  ; Process Start!
entry:

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
  
init_array_loop:
  mov ebx, start_init_array
  jmp .test
.body:
  call [ebx]
  add ebx,4
.test:
  cmp ebx, end_init_array
  jb .body
  
kernel_main:
  ; Jump to kernel main()
  
  call presysmain
  
dynamic_dtors:
  ; Call any dynamically added C++ destructors
  sub esp, 4
  mov [esp], dword 0x0
  call __cxa_finalize
  add esp, 4

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

; TODO: Something here
