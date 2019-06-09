OUTPUT_FORMAT("elf32-i386")
ENTRY(entry)
PHDRS
{
text PT_LOAD;
data PT_LOAD;
}
SECTIONS
{
    . = 0x10000;
    .text : {
        *(.text)
    } :text
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
    } :data
    . = ALIGN(4096);
    .bss : {
        *(.bss)
        *(COMMON)
    } :data
}
