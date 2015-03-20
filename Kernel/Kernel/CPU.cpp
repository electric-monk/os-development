#include "CPU.h"
#include "descript.h"
#include "StandardPC_traps.h"
#include "mem_logical.h"
#include "debug.h"
#include "Thread.h"
#include "Process.h"
#include "CPU_intr.h"

extern "C" void SwitchContexts(CPU::Context**, CPU::Context*);

void CPU::Context::SwitchFrom(Context **oldContext)
{
    SwitchContexts(oldContext, this);
}

void CPU::InitSegments(void)
{
    _interruptPushCount = 0;
    
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
    Process::Active = NULL;
    Thread::Active = NULL;
}

void CPU::InitTSS(void *kernelStack, UInt32 kernelStackSize)
{
    gdt[SEG_TSS].Set16(STS_T32A, &cpuTSS, sizeof(cpuTSS) - 1, 0);
    gdt[SEG_TSS].system = 0;
    cpuTSS.SS0 = SEG_KDATA << 3;
    cpuTSS.ESP0 = UInt32(kernelStack) + kernelStackSize;
    asm volatile("ltr %%ax" : : "a" (SEG_TSS << 3));
}

void CPU::PushInterruptFlag(void)
{
    UInt32 flag = GetEflag();
    CPU_Interrupt_Disable();
    if (_interruptPushCount == 0)
        _interruptWasEnabled = flag & FL_IF;
    _interruptPushCount++;
}

void CPU::PopInterruptFlag(void)
{
    _interruptPushCount--;
    if (_interruptPushCount == 0)
        if (_interruptWasEnabled)
            CPU_Interrupt_Enable();
}
