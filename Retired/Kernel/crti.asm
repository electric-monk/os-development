[global _init]
[global _fini]

SECTION .init
_init:
  push ebp
  mov esp, ebp

SECTION .fini
_fini:
  push ebp
  mov esp, ebp

