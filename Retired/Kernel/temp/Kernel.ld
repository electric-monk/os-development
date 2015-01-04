OUTPUT_FORMAT("elf32-i386")
ENTRY(entry)
virt = 0xC0100000; /* 3 gig */
//virt = 0x100000; /* 3 gig */
phys = 0x100000; /* 1 meg */
SECTIONS
{
	. = virt;
	kern_start = .;
    .text : AT(phys)
    {
        code = .;
		*(mb-header)
        *(.text)
    }
        . = ALIGN(4096);
    .data : AT(phys + (data - code))
    {
        data = .;
        *(bootpagedir)
        *(.data)
   start_ctors = .;
   *(.ctor*)
   end_ctors = .;
   start_dtors = .;
   *(.dtor*)
   end_dtors = .;
    }
        . = ALIGN(4096);
    .bss : AT(phys + (bss - code))
    {
        bss = .;
	*(bootpagedata)
        *(.bss)
        *(COMMON)
    }
        . = ALIGN(4096);
    end = .;
    kern_end = .;
}
