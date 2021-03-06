#ifndef __ELF_H__
#define __ELF_H__

// ELF base types

typedef UInt32 Elf32_Addr;
typedef UInt16 Elf32_Half;
typedef UInt32 Elf32_Off;
typedef SInt32 Elf32_Sword;
typedef UInt32 Elf32_Word;
typedef UInt8 Elf32_char;

// ELF structures

#define EI_NIDENT   16
typedef struct {
    Elf32_char e_ident[EI_NIDENT];
    Elf32_Half e_type;
#define ET_NONE     0
#define ET_REL      1
#define ET_EXEC     2
#define ET_DYN      3
#define ET_CORE     4
    Elf32_Half e_machine;
#define EM_NONE     0
#define EM_M32      1
#define EM_SPARC    2
#define EM_386      3
#define EM_68K      4
#define EM_88K      5
#define EM_860      7
#define EM_MIPS     8
    Elf32_Word e_version;
#define EV_CURRENT  1
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} __attribute((packed)) Elf32_Ehdr;

typedef struct {
    Elf32_Word  sh_name;
    Elf32_Word  sh_type;
#define SHT_NULL        0
#define SHT_PROGBITS    1
#define SHT_SYMTAB      2
#define SHT_STRTAB      3
#define SHT_RELA        4
#define SHT_HASH        5
#define SHT_DYNAMIC     6
#define SHT_NOTE        7
#define SHT_NOBITS      8
#define SHT_REL         9
#define SHT_SHLIB       10
#define SHT_DYNSYM      11
#define SHT_LOPROC      0x70000000
#define SHT_HIPROC      0x7FFFFFFF
#define SHT_LOUSER      0x80000000
#define SHT_HIUSER      0xFFFFFFFF
    Elf32_Word  sh_flags;
    Elf32_Addr  sh_addr;
    Elf32_Off   sh_offset;
    Elf32_Word  sh_size;
    Elf32_Word  sh_link;
    Elf32_Word  sh_info;
    Elf32_Word  sh_addralign;
    Elf32_Word  sh_entsize;
} __attribute((packed)) Elf32_Shdr;

typedef struct {
    Elf32_Word      st_name;
    Elf32_Addr      st_value;
    Elf32_Word      st_size;
    unsigned char   st_info;
#define ELF32_ST_BIND(i)        ((i)>>4)
#define STB_LOCAL   0
#define STB_GLOBAL  1
#define STB_WEAK    2
#define STB_LOPROC  13
#define STB_HIPROC  15
#define ELF32_ST_TYPE(i)        ((i)&0xF)
#define STT_NOTYPE  0
#define STT_OBJECT  1
#define STT_FUNC    2
#define STT_SECTION 3
#define STT_FILE    4
#define STT_LOPROC  13
#define STT_HIPROC  15
#define ELF32_ST_INFO(b,t)      (((b)<<4)+((t)&0xF))
    unsigned char   st_other;
    Elf32_Half      st_shndx;
} __attribute((packed)) Elf32_Sym;

// Fields
#define EI_MAG0     0
#define EI_MAG1     1
#define EI_MAG2     2
#define EI_MAG3     3
#define EI_CLASS    4
#define EI_DATA     5
#define EI_VERSION  6
// Values
#define ELFMAG0 0x7F
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'
#define ELFCLASS32      1
#define ELFCLASS64      2
#define ELFDATA2LSB     1
#define ELFDATA2MSB     2

typedef struct {
    Elf32_Word p_type;
#define PT_NULL     0
#define PT_LOAD     1
#define PT_DYNAMIC  2
#define PT_INTERP   3
#define PT_NOTE     4
#define PT_SHLIB    5
#define PT_PHDR     6
#define PT_LOPROC   0x70000000
#define PT_HIPROC   0x7FFFFFFF
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
#define PF_X        0x01
#define PF_W        0x02
#define PF_R        0x04
#define PF_MASKOS   0x0FF00000
#define PF_MASKPROC 0xF0000000
    Elf32_Word p_align;
} Elf32_Phdr;

#endif // __ELF_H__
