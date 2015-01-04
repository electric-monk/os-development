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

// CPU
static inline void CPU_Interrupt_Enable(void) { asm("sti"); }
static inline void CPU_Interrupt_Disable(void) { asm("cli"); }

#endif // __CPU_INTR_H__
