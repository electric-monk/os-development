#ifndef __CPU_H__
#define __CPU_H__

#include "descript.h"
#include "StandardPC_traps.h"

class Process;
class Thread;

class CPU
{
public:
    class Context
    {
    public:
        UInt32 EDI, ESI, EBX, EBP;
        UInt32 EIP; // Not saved by SwitchFrom(), but will be the last caller to call us (or interrupted method). We can also twiddle it if necessary.
        
        void SwitchFrom(Context **oldContext);
    };
    class FPUContext
    {
    public:
#ifdef FPU_LAZYISH
        FPUContext():active(false){}
        bool active;
#endif
        char data[108];
        void Store(void)
        {
            asm volatile ("fnsave (%0)" :: "r"(data));
        }
        void Restore(void)
        {
            asm volatile ("frstor (%0)" :: "r"(data));
        }
    };
    
    Context *scheduler;

    static CPU *Active asm("%gs:0");
    
    void PushInterruptFlag(void);
    void PopInterruptFlag(void);
    
#ifdef FPU_LAZYISH
    UInt32 lastFPUUser;
#endif

private:
    GDT_SEGMENT gdt[SEGMENT_COUNT];
    
    typedef struct {
        CPU *current;
        Process *process;
        Thread *thread;
    } CPU_LOCAL;
    
    CPU_LOCAL localStorage;
    TSS_BLOCK cpuTSS;
    
    UInt32 _interruptPushCount;
    bool _interruptWasEnabled;
    
public:
    // These methods must be called only by the CPU corresponding to the CPU object instance
    void InitSegments(void);
    void InitTSS(void *kernelStack, UInt32 kernelStackSize);
};

#endif // __CPU_H__
