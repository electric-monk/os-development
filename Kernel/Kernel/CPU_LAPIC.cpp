#include "tools.h"

// Most of this code is taken verbatim from xv6, it appears to be boilerplate

volatile UInt32 *g_LAPIC;

static void LAPIC_Write(int index. int value)
{
    g_LAPIC[index] = value;
    g_LAPIC[ID];    // Read, to complete write
}

static void microdelay(int us)
{
    
}

// Local APIC registers, divided by 4 for use as uint[] indices.
#define ID      (0x0020/4)   // ID
#define VER     (0x0030/4)   // Version
#define TPR     (0x0080/4)   // Task Priority
#define EOI     (0x00B0/4)   // EOI
#define SVR     (0x00F0/4)   // Spurious Interrupt Vector
#define ENABLE     0x00000100   // Unit Enable
#define ESR     (0x0280/4)   // Error Status
#define ICRLO   (0x0300/4)   // Interrupt Command
#define INIT       0x00000500   // INIT/RESET
#define STARTUP    0x00000600   // Startup IPI
#define DELIVS     0x00001000   // Delivery status
#define ASSERT     0x00004000   // Assert interrupt (vs deassert)
#define DEASSERT   0x00000000
#define LEVEL      0x00008000   // Level triggered
#define BCAST      0x00080000   // Send to all APICs, including self.
#define BUSY       0x00001000
#define FIXED      0x00000000
#define ICRHI   (0x0310/4)   // Interrupt Command [63:32]
#define TIMER   (0x0320/4)   // Local Vector Table 0 (TIMER)
#define X1         0x0000000B   // divide counts by 1
#define PERIODIC   0x00020000   // Periodic
#define PCINT   (0x0340/4)   // Performance Counter LVT
#define LINT0   (0x0350/4)   // Local Vector Table 1 (LINT0)
#define LINT1   (0x0360/4)   // Local Vector Table 2 (LINT1)
#define ERROR   (0x0370/4)   // Local Vector Table 3 (ERROR)
#define MASKED     0x00010000   // Interrupt masked
#define TICR    (0x0380/4)   // Timer Initial Count
#define TCCR    (0x0390/4)   // Timer Current Count
#define TDCR    (0x03E0/4)   // Timer Divide Configuration

#define IO_RTC  0x70

void CPU_LAPIC_Init(void)
{
    if (!g_LAPIC)
        return;
    
    // Enable local APIC; set spurious interrupt vector.
    LAPIC_Write(SVR, ENABLE | (T_IRQ0 + IRQ_SPURIOUS));
    
    // The timer repeatedly counts down at bus frequency
    // from lapic[TICR] and then issues an interrupt.
    // If xv6 cared more about precise timekeeping,
    // TICR would be calibrated using an external time source.
    LAPIC_Write(TDCR, X1);
    LAPIC_Write(TIMER, PERIODIC | (T_IRQ0 + IRQ_TIMER));
    LAPIC_Write(TICR, 10000000);
    
    // Disable logical interrupt lines.
    LAPIC_Write(LINT0, MASKED);
    LAPIC_Write(LINT1, MASKED);
    
    // Disable performance counter overflow interrupts
    // on machines that provide that interrupt entry.
    if(((g_LAPIC[VER]>>16) & 0xFF) >= 4)
        LAPIC_Write(PCINT, MASKED);
    
    // Map error interrupt to IRQ_ERROR.
    LAPIC_Write(ERROR, T_IRQ0 + IRQ_ERROR);
    
    // Clear error status register (requires back-to-back writes).
    LAPIC_Write(ESR, 0);
    LAPIC_Write(ESR, 0);
    
    // Ack any outstanding interrupts.
    LAPIC_Write(EOI, 0);
    
    // Send an Init Level De-Assert to synchronise arbitration ID's.
    LAPIC_Write(ICRHI, 0);
    LAPIC_Write(ICRLO, BCAST | INIT | LEVEL);
    while(g_LAPIC[ICRLO] & DELIVS);
    
    // Enable interrupts on the APIC (but not on the processor).
    LAPIC_Write(TPR, 0);
}

int CPU_LAPIC_Number(void)
{
    // Cannot call cpu when interrupts are enabled:
    // result not guaranteed to last long enough to be used!
    // Would prefer to panic but even printing is chancy here:
    // almost everything, including cprintf and panic, calls cpu,
    // often indirectly through acquire and release.
    if(readeflags()&FL_IF){
        static int n;
        if(n++ == 0)
            kprintf("cpu called from %x with interrupts enabled\n",
                    __builtin_return_address(0));
    }
    
    if(g_LAPIC)
        return g_LAPIC[ID]>>24;
    return 0;
}

void CPU_LAPIC_Acknowledge(void)
{
    if (g_LAPIC)
        LAPIC_Write(EOI, 0);
}

void CPU_LAPIC_StartProcessor(UInt8 ApicId, void *address)
{
    int i;
    UInt16 *wrv;
    
    // "The BSP must initialize CMOS shutdown code to 0AH
    // and the warm reset vector (DWORD based at 40:67) to point at
    // the AP startup code prior to the [universal startup algorithm]."
    outb(IO_RTC, 0xF);  // offset 0xF is shutdown code
    outb(IO_RTC + 1, 0x0A);
    wrv = (UInt16*)P2V((0x40 << 4 | 0x67));  // Warm reset vector
    wrv[0] = 0;
    wrv[1] = UInt32(address) >> 4;
    
    // "Universal startup algorithm."
    // Send INIT (level-triggered) interrupt to reset other CPU.
    LAPIC_Write(ICRHI, ApicId << 24);
    LAPIC_Write(ICRLO, INIT | LEVEL | ASSERT);
    microdelay(200);
    LAPIC_Write(ICRLO, INIT | LEVEL);
    microdelay(100);    // should be 10ms, but too slow in Bochs!
    
    // Send startup IPI (twice!) to enter code.
    // Regular hardware is supposed to only accept a STARTUP
    // when it is in the halted state due to an INIT.  So the second
    // should be ignored, but it is part of the official Intel algorithm.
    // Bochs complains about the second one.  Too bad for Bochs.
    for(i = 0; i < 2; i++){
        LAPIC_Write(ICRHI, ApicId << 24);
        LAPIC_Write(ICRLO, STARTUP | (UInt32(address) >> 12));
        microdelay(200);
    }
}
