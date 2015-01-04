#ifndef __DESCRIPT_H__
#define __DESCRIPT_H__

#include "runtime.h"

typedef struct 
{
    UInt16 limit;
    UInt32 base;
} __attribute__((packed)) cpu_ptr;

/**** Global Descriptor Table ****/

typedef struct
{
    UInt16 limit_low;
    UInt16 base_low;
    UInt8 base_middle;
    UInt8 access;
    UInt8 granularity;
    UInt8 base_high;
} __attribute__((packed)) gdt_entry;

void gdt_flush(cpu_ptr *address);

/**** Local Descriptor Table ****/

/**** Task State Segment ****/

/**** Interrupt Descriptor Table ****/

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
} __attribute__((packed)) idt_entry;

void ldt_flush(cpu_ptr *address);

#endif // __DESCRIPT_H__
