#include "CPU.h"
#include "descript.h"
#include "StandardPC_traps.h"
#include "mem_logical.h"
#include "debug.h"
#include "Thread.h"

extern "C" void SwitchContexts(CPU::Context**, CPU::Context*);

void CPU::Context::SwitchFrom(Context **oldContext)
{
    SwitchContexts(oldContext, this);
}

void CPU::InitSegments(void)
{
    // System segments
    gdt[SEG_KCODE].Set(STA_X | STA_R, 0, 0xFFFFFFFF, 0);
    gdt[SEG_KDATA].Set(STA_W, 0, 0xFFFFFFFF, 0);
    gdt[SEG_UCODE].Set(STA_X | STA_R, 0, 0xFFFFFFFF, DPL_USER);
    gdt[SEG_UDATA].Set(STA_W, 0, 0xFFFFFFFF, DPL_USER);

    // Our own segments
    gdt[SEG_KCPU].Set(STA_W, &localStorage, PAGE_LIMITED(sizeof(CPU_LOCAL)), 0);

    // Tell the CPU to use the GDT (this is the part that must be run on the relevant CPU)
    cpu_ptr data;
    data.base = UInt32(gdt);
    data.limit = (sizeof(GDT_SEGMENT) * SEGMENT_COUNT) - 1;
    gdt_flush(&data);
    
    // Load GS for our kernel TLS
    asm volatile("movw %0, %%gs" : : "r" (UInt16(SEG_KCPU << 3)));
    
    // Set up TLS
    CPU::Active = this;
    Thread::Active = NULL;
}
