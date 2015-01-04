
typedef struct {
    UInt32 reg;
    UInt32 pad[3];
    UInt32 data;
} IOAPIC;

#define IOAPIC  0xFEC00000   // Default physical address of IO APIC

#define REG_ID     0x00  // Register index: ID
#define REG_VER    0x01  // Register index: version
#define REG_TABLE  0x10  // Redirection table base

// The redirection table starts at REG_TABLE and uses
// two registers to configure each interrupt.
// The first (low) register in a pair contains configuration bits.
// The second (high) register contains a bitmask telling which
// CPUs can serve that interrupt.
#define INT_DISABLED   0x00010000  // Interrupt disabled
#define INT_LEVEL      0x00008000  // Level-triggered (vs edge-)
#define INT_ACTIVELOW  0x00002000  // Active low (vs high)
#define INT_LOGICAL    0x00000800  // Destination is CPU id (vs APIC ID)

static volatile IOAPIC *g_IOAPIC;

static UInt32 IOAPIC_Read(int reg)
{
    g_IOAPIC->reg = reg;
    return g_IOAPIC->data;
}

static void IOAPIC_Write(int reg, UInt32 data)
{
    g_IOAPIC->reg = reg;
    g_IOAPIC->data = data;
}

void CPU_IOAPIC_Init(void)
{
    int i, id, maxintr;
    
    if (!ismp)
        return;
    
    g_IOAPIC = (IOAPIC*)IOAPIC;
    maxintr = (IOAPIC_Read(REG_VER) >> 16) & 0xFF;
    id = IOAPIC_Read(REG_ID) >> 24;
//    if (id != ioapicid)
//        kprintf("id isn't equal to ioapicid; not a MP\n");
    
    // Mark all interrupts edge-triggered, active high, disabled,
    // and not routed to any CPUs.
    for(i = 0; i <= maxintr; i++)
        CPU_IOAPIC_Enable(0, i, false);
}

void CPU_IOAPIC_Enable(int cpunum, int irq, bool enable)
{
    if(!ismp)
        return;
    
    // Mark interrupt edge-triggered, active high,
    // enabled, and routed to the given cpunum,
    // which happens to be that cpu's APIC ID.
    int flag = T_IRQ0 + irq;
    if (!enable)
        flag |= INT_DISABLED;
    IOAPIC_Write(REG_TABLE + 2 * irq, flag);
    IOAPIC_Write(REG_TABLE + 2 * irq + 1, cpunum << 24);
}
