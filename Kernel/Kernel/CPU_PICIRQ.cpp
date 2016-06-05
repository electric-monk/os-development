#include "tools.h"
#include "runtime.h"

// Most of this code appears to be boilerplate, taken from xv6

// I/O Addresses of the two programmable interrupt controllers
#define IO_PIC1         0x20    // Master (IRQs 0-7)
#define IO_PIC2         0xA0    // Slave (IRQs 8-15)

#define ICW1_ICW4       0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE     0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL      0x08		/* Level triggered (edge) mode */
#define ICW1_INIT       0x10		/* Initialization - required! */

#define ICW4_8086       0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM       0x10		/* Special fully nested (not) */

#define IRQ_SLAVE       2       // IRQ at which slave connects to master

static int s_IRQDisable = 0;
static int s_IRQMask = 0xFFFF;

static void SetMask(UInt16 mask)
{
    outb(IO_PIC1 + 1, mask);
    outb(IO_PIC2 + 1, mask >> 8);
}

static UInt16 GetISR(void)
{
    outb(IO_PIC1, 0x0b);
    outb(IO_PIC2, 0x0b);
    return (inb(IO_PIC2) << 8) | inb(IO_PIC1);
}

void CPU_PIC_Init(int baseVector)
{
    // mask all interrupts
    outb(IO_PIC1+1, 0xFF);
    outb(IO_PIC2+1, 0xFF);
    
    // Set up master (8259A-1)
    
    // ICW1:  0001g0hi
    //    g:  0 = edge triggering, 1 = level triggering
    //    h:  0 = cascaded PICs, 1 = master only
    //    i:  0 = no ICW4, 1 = ICW4 required
    outb(IO_PIC1, ICW1_INIT | ICW1_ICW4);
    
    // ICW2:  Vector offset
    outb(IO_PIC1 + 1, baseVector);
    
    // ICW3:  (master PIC) bit mask of IR lines connected to slaves
    //        (slave PIC) 3-bit # of slave's connection to master
    outb(IO_PIC1 + 1, 1<<IRQ_SLAVE);
    
    // ICW4:  000nbmap
    //    n:  1 = special fully nested mode
    //    b:  1 = buffered mode
    //    m:  0 = slave PIC, 1 = master PIC
    //      (ignored when b is 0, as the master/slave role
    //      can be hardwired).
    //    a:  1 = Automatic EOI mode
    //    p:  0 = MCS-80/85 mode, 1 = intel x86 mode
    outb(IO_PIC1 + 1, ICW4_8086);
    
    // Set up slave (8259A-2)
    outb(IO_PIC2, ICW1_INIT | ICW1_ICW4); // ICW1
    outb(IO_PIC2 + 1, baseVector + 8);    // ICW2
    outb(IO_PIC2 + 1, IRQ_SLAVE);         // ICW3
    outb(IO_PIC2 + 1, ICW4_8086);         // ICW4
    
    // OCW3:  0ef01prs
    //   ef:  0x = NOP, 10 = clear specific mask, 11 = set specific mask
    //    p:  0 = no polling, 1 = polling mode
    //   rs:  0x = NOP, 10 = read IRR, 11 = read ISR
    outb(IO_PIC1, 0x68);             // clear specific mask
    outb(IO_PIC1, 0x0a);             // read IRR by default
    
    outb(IO_PIC2, 0x68);             // OCW3
    outb(IO_PIC2, 0x0a);             // OCW3
    
    SetMask(s_IRQMask);
}

void CPU_PIC_MasterEnable(bool enable)
{
    if (enable) {
        if (s_IRQDisable == 0) {
            // Error
            return;
        }
        if ((--s_IRQDisable) == 0)
            SetMask(s_IRQMask);
    } else {
        if ((s_IRQDisable++) == 0)
            SetMask(0xFFFF);
    }
}

void CPU_PIC_Enable(int irq, bool enable)
{
    if ((irq < 0) || (irq >= 16) || (irq == IRQ_SLAVE))
        return;
    // Update mask
    if (enable)
        s_IRQMask &= ~(1 << irq);
    else
        s_IRQMask |= 1 << irq;
    if (s_IRQMask & 0xFF00)
        s_IRQMask &= ~(1 << IRQ_SLAVE);
    else
        s_IRQMask |= 1 << IRQ_SLAVE;
    // If they're disabled, ignore it for now
    if (s_IRQDisable != 0)
        return;
    // Update
    SetMask(s_IRQMask);
}

void CPU_PIC_SendEOI(int irq)
{
    if ((irq < 0) || (irq >= 16))
        return;
    if (irq >= 8) {
        outb(IO_PIC2, 0x60 | (irq - 8));
        irq = IRQ_SLAVE;
    }
    outb(IO_PIC1, 0x60 | irq);
}

bool CPU_PIC_IsSpurious(int irq)
{
    if (irq == 7) {
        return !(GetISR() & (1 << irq));
    } else if (irq == 15) {
        UInt16 flag = GetISR();
        if (flag & (1 << irq))
            return false;
        CPU_PIC_SendEOI(IRQ_SLAVE);
        return true;
    } else {
        return false;
    }
}
