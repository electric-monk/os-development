#include "tools.h"
#include "runtime.h"

// Most of this code appears to be boilerplate, taken from xv6

// I/O Addresses of the two programmable interrupt controllers
#define IO_PIC1         0x20    // Master (IRQs 0-7)
#define IO_PIC2         0xA0    // Slave (IRQs 8-15)

#define IRQ_SLAVE       2       // IRQ at which slave connects to master

static int s_IRQDisable = 0;
static int s_IRQMask = 0xFFFF;

static void SetMask(UInt16 mask)
{
    outb(IO_PIC1 + 1, mask);
    outb(IO_PIC2 + 1, mask >> 8);
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
    outb(IO_PIC1, 0x11);
    
    // ICW2:  Vector offset
    outb(IO_PIC1+1, baseVector);
    
    // ICW3:  (master PIC) bit mask of IR lines connected to slaves
    //        (slave PIC) 3-bit # of slave's connection to master
    outb(IO_PIC1+1, 1<<IRQ_SLAVE);
    
    // ICW4:  000nbmap
    //    n:  1 = special fully nested mode
    //    b:  1 = buffered mode
    //    m:  0 = slave PIC, 1 = master PIC
    //      (ignored when b is 0, as the master/slave role
    //      can be hardwired).
    //    a:  1 = Automatic EOI mode
    //    p:  0 = MCS-80/85 mode, 1 = intel x86 mode
    outb(IO_PIC1+1, 0x3);
    
    // Set up slave (8259A-2)
    outb(IO_PIC2, 0x11);                  // ICW1
    outb(IO_PIC2+1, baseVector + 8);      // ICW2
    outb(IO_PIC2+1, IRQ_SLAVE);           // ICW3
    // NB Automatic EOI mode doesn't tend to work on the slave.
    // Linux source code says it's "to be investigated".
    outb(IO_PIC2+1, 0x3);                 // ICW4
    
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
