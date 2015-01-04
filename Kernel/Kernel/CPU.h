#ifndef __CPU_H__
#define __CPU_H__

#include "descript.h"
#include "StandardPC_traps.h"

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
    
    Context *scheduler;

    static CPU *Active asm("%gs:0");

private:
    GDT_SEGMENT gdt[SEGMENT_COUNT];
    
    typedef struct {
        CPU *current;
        void *_placeholder_process;
        Thread *thread;
    } CPU_LOCAL;
    
    CPU_LOCAL localStorage;
    
public:
    // These methods must be called only by the CPU corresponding to the CPU object instance
    void InitSegments(void);
};

#endif // __CPU_H__
