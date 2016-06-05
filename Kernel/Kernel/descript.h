#ifndef __DESCRIPT_H__
#define __DESCRIPT_H__

#include "runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
    UInt16 limit;
    UInt32 base;
} __attribute__((packed)) cpu_ptr;

#define DPL_USER    0x3     // User DPL

#define PAGE_SIZE       4096
#define PAGE_LIMITED(size)  (((size) + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1))

/**** Global Descriptor Table ****/
    
    // Application segment type bits
#define STA_X       0x8     // Executable segment
#define STA_E       0x4     // Expand down (non-executable segments)
#define STA_C       0x4     // Conforming code segment (executable only)
#define STA_W       0x2     // Writeable (non-executable segments)
#define STA_R       0x2     // Readable (executable segments)
#define STA_A       0x1     // Accessed
    
    // System segment type bits
#define STS_T16A    0x1     // Available 16-bit TSS
#define STS_LDT     0x2     // Local Descriptor Table
#define STS_T16B    0x3     // Busy 16-bit TSS
#define STS_CG16    0x4     // 16-bit Call Gate
#define STS_TG      0x5     // Task Gate / Coum Transmitions
#define STS_IG16    0x6     // 16-bit Interrupt Gate
#define STS_TG16    0x7     // 16-bit Trap Gate
#define STS_T32A    0x9     // Available 32-bit TSS
#define STS_T32B    0xB     // Busy 32-bit TSS
#define STS_CG32    0xC     // 32-bit Call Gate
#define STS_IG32    0xE     // 32-bit Interrupt Gate
#define STS_TG32    0xF     // 32-bit Trap Gate

typedef struct {
    UInt16 limitLow;
    UInt16 baseLow;
    UInt8 baseMiddle;
    union {
        struct {
            UInt8 type: 4;
            UInt8 system: 1;
            UInt8 dpl: 2;
            UInt8 present: 1;
            UInt8 limitHigh: 4;
            UInt8 custom: 1;
            UInt8 reserved1: 1;
            UInt8 mode: 1;  // 0: 16-bit, 1: 32-bit
            UInt8 granularity: 1;   // 1: 4K
        };
        UInt16 flags;
    };
    UInt8 baseHigh;
    
    void Set(UInt8 type, void *base, UInt32 limit, UInt8 dpl)
    {
        limitLow = (limit >> 12) & 0xFFFF;
        baseLow = UInt32(base) & 0xFFFF;
        baseMiddle = (UInt32(base) >> 16) & 0xFF;
        this->type = type;
        system = 1;
        this->dpl = dpl;
        present = 1;
        limitHigh = limit >> 28;
        custom = 0;
        reserved1 = 0;
        mode = 1;
        granularity = 1;
        baseHigh = UInt32(base) >> 24;
    }
    void Set16(UInt8 type, void *base, UInt32 limit, UInt8 dpl)
    {
        limitLow = limit & 0xFFFF;
        baseLow = UInt32(base) & 0xFFFF;
        baseMiddle = (UInt32(base) >> 16) & 0xFF;
        this->type = type;
        system = 1;
        this->dpl = dpl;
        present = 1;
        limitHigh = limit >> 16;
        custom = 0;
        reserved1 = 0;
        mode = 1;
        granularity = 0;
        baseHigh = UInt32(base) >> 24;
    }
} __attribute__((packed)) GDT_SEGMENT;

void gdt_flush(cpu_ptr *address);

/**** Local Descriptor Table ****/

/**** Task State Segment ****/

typedef struct {
    UInt32 Link;    // Old TS selector
    // Stack pointer and segment selector after increase in privilege level
    UInt32 ESP0;
    UInt16 SS0, padding0;
    // Etc.
    UInt32 *ESP1;
    UInt16 SS1, padding1;
    UInt32 *ESP2;
    UInt16 SS2, padding2;
    void *CR3;  // Page directory base
    UInt32 *EIP;    // Saved state from last task switch
    UInt32 EFlags, EAX, ECX, EDX, EBX;
    UInt32 *ESP, *EBP;
    UInt32 ESI, EDI;
    UInt16 ES, padding3;
    UInt16 CS, padding4;
    UInt16 SS, padding5;
    UInt16 DS, padding6;
    UInt16 FS, padding7;
    UInt16 GS, padding8;
    UInt16 LDT, padding9;
    UInt16 Trap, IOMapBase;
} __attribute__((packed)) TSS_BLOCK;

extern "C" UInt32 GetEflag(void);

/**** Interrupt Descriptor Table ****/

#define IDT_TYPE_TASK32 0x05
#define IDT_TYPE_INT16  0x06
#define IDT_TYPE_TRAP16 0x07
#define IDT_TYPE_INT32  0x0E
#define IDT_TYPE_TRAP32 0x0F
    
typedef struct {
    UInt16 baseLow;
    UInt16 sel;         /* Our kernel segment goes here! */
    UInt8 alwaysZero;   /* This will ALWAYS be set to 0! */
    union {
        struct {
            UInt8 gateType : 4;
            UInt8 segment : 1;
            UInt8 privilegeLevel : 2;
            UInt8 Present : 1;
        };
        UInt8 rawFlags; /* Set using the above table! */
    };
    UInt16 baseHigh;
    
    void Set(bool istrap, int segment, void *offset, int dpl)
    {
        baseLow = UInt16(UInt32(offset) & 0xFFFF);
        sel = segment;
        alwaysZero = 0;
        gateType = istrap ? IDT_TYPE_TRAP32 : IDT_TYPE_INT32;
        this->segment = 0;
        privilegeLevel = dpl;
        Present = 1;
        baseHigh = UInt16(UInt32(offset) >> 16);
    }
} __attribute__((packed)) idt_entry;

void idt_load(cpu_ptr *address);
    
#ifdef __cplusplus
}
#endif

#endif // __DESCRIPT_H__
