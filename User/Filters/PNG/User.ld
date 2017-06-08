OUTPUT_FORMAT("elf32-i386")
ENTRY(entry)
SECTIONS
{
    . = 0x10000;
    .text : {
        *(.text)
    }
        . = ALIGN(4096);
    .data : {
        *(.data)
   start_ctors = .;
   *(.ctor*)
   end_ctors = .;
   start_dtors = .;
   *(.dtor*)
   end_dtors = .;
   start_init_array = .;
   *(.init_array)
   end_init_array = .;
    }
        . = ALIGN(4096);
    .bss : {
        *(.bss)
        *(COMMON)
    }
}
