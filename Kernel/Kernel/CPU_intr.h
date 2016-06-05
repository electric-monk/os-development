#ifndef __CPU_INTR_H__
#define __CPU_INTR_H__

// IOAPIC
void CPU_IOAPIC_Init(void);
void CPU_IOAPIC_Enable(int cpunum, int irq, bool enable);

// LAPIC
void CPU_LAPIC_Init(void);
int CPU_LAPIC_Number(void);
void CPU_LAPIC_Acknowledge(void);
void CPU_LAPIC_StartProcessor(UInt8 ApicId, void *address);

// PICIRQ
void CPU_PIC_Init(int baseVector); // IRQ_Vector == baseVector + irq
void CPU_PIC_MasterEnable(bool enable);
void CPU_PIC_Enable(int irq, bool enable);
void CPU_PIC_SendEOI(int irq);
bool CPU_PIC_IsSpurious(int irq);

// CPU
static inline void CPU_Interrupt_Enable(void) { asm("sti"); }
static inline void CPU_Interrupt_Disable(void) { asm("cli"); }

#include "descript.h"
#include "StandardPC_traps.h"

class InterruptEnabler
{
public:
    InterruptEnabler()
    {
        UInt32 flag = GetEflag();
        CPU_Interrupt_Enable();
        _was = flag & FL_IF;
    }
    ~InterruptEnabler()
    {
        if (!_was)
            CPU_Interrupt_Disable();
    }
private:
    bool _was;
};

class InterruptDisabler
{
public:
    InterruptDisabler()
    {
        UInt32 flag = GetEflag();
        CPU_Interrupt_Disable();
        _was = flag & FL_IF;
    }
    ~InterruptDisabler()
    {
        if (_was)
            CPU_Interrupt_Enable();
    }
private:
    bool _was;
};

#endif // __CPU_INTR_H__
