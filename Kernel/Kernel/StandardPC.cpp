#include "StandardPC.h"
#include "StandardPC_traps.h"
#include "Interrupts.h"
#include "descript.h"
#include "mem_logical.h"
#include "debug.h"
#include "CPU_intr.h"
#include "CPU.h"
#include "Driver_ATA.h"
#include "Blocking.h"
#include "Scheduler.h"
#include "Collections.h"
#include "mem_virtual.h"

static bool s_schedulerEnabled = false;

void StartScheduling(void)
{
    s_schedulerEnabled = true;
}

static const char *s_trapNames[] = {
    /* 0x00 */ "Division by zero",
    /* 0x01 */ "Debugger",
    /* 0x02 */ "NMI",
    /* 0x03 */ "Breakpoint",
    /* 0x04 */ "Overflow",
    /* 0x05 */ "Bounds",
    /* 0x06 */ "Invalid opcode",
    /* 0x07 */ "Coprocessor not present",
    /* 0x08 */ "Double fault",
    /* 0x09 */ "Coprocessor overrun",
    /* 0x0A */ "Invalid TSS",
    /* 0x0B */ "Segment absent",
    /* 0x0C */ "Stack fault",
    /* 0x0D */ "General Protection Fault",
    /* 0x0E */ "Page fault",
    /* 0x0F */ NULL,
    /* 0x10 */ "Math fault",
    /* 0x11 */ "Alignment check",
    /* 0x12 */ "Machine check",
    /* 0x13 */ "Floating point exception",
};

const char* StandardPC::NameForTrap(unsigned int trap)
{
    if (trap >= (sizeof(s_trapNames) / sizeof(s_trapNames[0])))
        return NULL;
    return s_trapNames[trap];
}

class StandardPC_LegacyATA : public ATADriverNode
{
private:
    class SimplerSignal : public SimpleSignal
    {
    private:
        bool _set;
        
    public:
        SimplerSignal()
        :SimpleSignal(false)
        {
            _set = false;
        }
        
        void Activate(void)
        {
            if (_set)
                return;
            _set = true;
            Set();
        }
        void Deactivate(void)
        {
            if (!_set)
                return;
            _set = false;
            Reset();
        }
    };
    
    UInt16 _basePort;
    UInt16 _baseIRQ;
    InterruptHandlerHandle _interruptHandle;
    SimplerSignal *_interrupt;
    
    unsigned short GetPort(UInt32 address)
    {
        static const UInt16 offsets[] = {0, 1, 2, 3, 4, 5, 6, 7, 0x206};
        return _basePort + offsets[address];
    }
    
    static bool InterruptCallback(void *context, void *info)
    {
        StandardPC_LegacyATA *that = (StandardPC_LegacyATA*)context;
        kprintf("Starting disk interrupt %i\n", that->_baseIRQ);
        that->_interrupt->Activate();
        that->UpdateForInterrupt();
        return true;
    }
    
protected:
    ~StandardPC_LegacyATA()
    {
        _interrupt->Release();
    }
    
public:
    StandardPC_LegacyATA(UInt16 basePort, UInt16 baseIRQ)
    :ATADriverNode("ATA Legacy I/O")
    {
        _basePort = basePort;
        _baseIRQ = baseIRQ;
        _interrupt = new SimplerSignal();
        SetProperty(kDriver_Property_Bus, kDriver_Bus_SystemATA);
    }

    bool Start(Driver *parent)
    {
        _interruptHandle = InterruptSource()->RegisterHandler(_baseIRQ + PIC_IRQ_OFFSET, InterruptCallback, this);
        return ATADriverNode::Start(parent);
    }
    
    void Stop(void)
    {
        InterruptSource()->UnregisterHandler(_interruptHandle);
        ATADriverNode::Stop();
    }
    
    UInt8 inByte(UInt32 address)
    {
        return inb(GetPort(address));
    }
    UInt16 inShort(UInt32 address)
    {
        return inw(GetPort(address));
    }
    UInt32 inLong(UInt32 address)
    {
        return inl(GetPort(address));
    }
    void outByte(UInt32 address, UInt8 byte)
    {
        outb(GetPort(address), byte);
    }
    void outShort(UInt32 address, UInt16 byte)
    {
        outw(GetPort(address), byte);
    }
    void outLong(UInt32 address, UInt32 byte)
    {
        outl(GetPort(address), byte);
    }
    
    void inByteRep(UInt32 address, void *buffer, UInt32 length)
    {
        asm("rep insb"::"c"(length / sizeof(UInt8)), "d"(GetPort(address)), "D"(buffer));
    }
    void inShortRep(UInt32 address, void *buffer, UInt32 length)
    {
        asm("rep insw"::"c"(length / sizeof(UInt16)), "d"(GetPort(address)), "D"(buffer));
    }
    void inLongRep(UInt32 address, void *buffer, UInt32 length)
    {
//        asm("rep insd"::"c"(length / sizeof(UInt32)), "d"(GetPort(address)), "D"(buffer));
        inShortRep(address, buffer, length);
    }
    void outByteRep(UInt32 address, void *buffer, UInt32 length)
    {
        asm("rep outsb"::"c"(length / sizeof(UInt8)), "d"(GetPort(address)), "S"(buffer));
    }
    void outShortRep(UInt32 address, void *buffer, UInt32 length)
    {
        asm("rep outsw"::"c"(length / sizeof(UInt16)), "d"(GetPort(address)), "S"(buffer));
    }
    void outLongRep(UInt32 address, void *buffer, UInt32 length)
    {
//        asm("rep outsd"::"c"(length / sizeof(UInt32)), "d"(GetPort(address)), "S"(buffer));
        outShortRep(address, buffer, length);
    }
    
    BlockableObject* Interrupt(void)
    {
        return _interrupt;
    }
    
    void ResetInterrupt(void)
    {
        _interrupt->Deactivate();
    }
    
    bool DMAAvailable(void)
    {
        return false;
    }
    
    UInt8 readBusMasterStatus(void)
    {
        return 0;
    }
    
    void writeBusMasterStatus(UInt8 x)
    {
        // nothing
    }
};

class StandardPC_Interrupts : public Interrupts
{
private:
    class HandlerHandle
    {
    public:
        HandlerHandle(HandlerHandle **root, InterruptHandler handler, void *context, UInt16 irq)
        {
            _irq = irq;
            _root = root;
            _last = NULL;
            _next = *root;
            (*_root) = this;
            _handler = handler;
            _context = context;
        }
        ~HandlerHandle()
        {
            if (_last != NULL)
                _last->_next = _next;
            if (_next != NULL)
                _next->_last = _last;
            if ((*_root) == this)
                (*_root) = (_next != NULL) ? _next : _last;
        }
        
        static void Trigger(HandlerHandle *that, TrapFrame *frame)
        {
            while (that != NULL) {
                if (that->_handler(that->_context, frame))
                    return;
                that = that->_next;
            };
            // TODO: Unhandled trap
            kprintf("Unhandled trap: 0x%.2x\n", frame->TrapNumber);
        }
        UInt16 _irq;
    private:
        HandlerHandle **_root;
        HandlerHandle *_last, *_next;
        InterruptHandler _handler;
        void *_context;
    };
private:
    idt_entry entries[256];
    HandlerHandle *handlers[256];
public:
    StandardPC_Interrupts()
    {
        void **vectorBase = &vectors;
        for (int i = 0; i < 256; i++) {
            entries[i].Set(false, SEG_KCODE<<3, vectorBase[i], 0);
            handlers[i] = NULL;
        }
    }
    
    // Internal workings
    void Set(void)
    {
        cpu_ptr data;
        data.base = UInt32(entries);
        data.limit = sizeof(entries);
        idt_load(&data);
    }
    
    void Trigger(TrapFrame *frame)
    {
        HandlerHandle::Trigger(handlers[frame->TrapNumber], frame);
    }

    // External API
    InterruptHandlerHandle RegisterHandler(int irq, InterruptHandler handler, void *context)
    {
        bool wasUsed = handlers[irq] != NULL;
        InterruptHandlerHandle handle = new HandlerHandle(handlers + irq, handler, context, irq);
        if (!wasUsed) {
            // Any initialisation necessary?
            if ((irq >= PIC_IRQ_OFFSET) && (irq < (PIC_IRQ_OFFSET + 16)))
                CPU_PIC_Enable(irq - PIC_IRQ_OFFSET, true);
        }
        return handle;
    }
    
    void UnregisterHandler(InterruptHandlerHandle handler)
    {
        HandlerHandle *handle = (HandlerHandle*)handler;
        UInt16 irq = handle->_irq;
        delete handle;
        // Should this happen first?
        if (handlers[irq] == NULL) {
            // Do we need to uninitialise?
            if ((irq >= PIC_IRQ_OFFSET) && (irq < (PIC_IRQ_OFFSET + 16)))
                CPU_PIC_Enable(irq - PIC_IRQ_OFFSET, false);
        }
    }
    
    void ConfigureSyscall(int irq)
    {
        void **vectorBase = &vectors;
        entries[irq].Set(true, SEG_KCODE<<3, vectorBase[irq], DPL_USER);
        // Tell CPU to reload
            // TODO: multiprocessor
        Set();
    }
};

class StandardPC_Timer : public Driver
{
public:
    StandardPC_Timer()
    :Driver("PC Timer")
    {
    }
    
    bool Start(Driver *parent)
    {
        int divisor = 1193180 / 1000/*Hz*/;
        outb(0x43, 0x36);
        outb(0x40, divisor & 0xFF);
        outb(0x40, divisor >> 8);
        _timerInterrupt = InterruptSource()->RegisterHandler(PIC_IRQ_OFFSET + 0, InterruptCallback, this);
        return Driver::Start(parent);
    }
    
    void Stop(void)
    {
        InterruptSource()->UnregisterHandler(_timerInterrupt);
        Driver::Stop();
    }
    
private:
    InterruptHandlerHandle _timerInterrupt;
    
    static bool InterruptCallback(void *context, void *state)
    {
        Timer::TimerTick(MILLISECONDS(1));
        if (s_schedulerEnabled)
            Scheduler::EnterFromInterrupt();
        return true;
    }
};

class StandardPC_Factory : public DriverFactory
{
private:
    class Match_IDE : public DriverFactory::Match
    {
    public:
        Match_IDE(UInt16 port, UInt16 irq)
        {
            _port = port;
            _irq = irq;
        }
        
        int MatchValue(void)
        {
            return 99999;
        }
        Driver* Instantiate(void)
        {
            kprintf("Instantiating IDE %i\n", _port);
            return new StandardPC_LegacyATA(_port, _irq);
        }
        bool MatchMultiple(void)
        {
            return true;
        }
    private:
        UInt16 _port, _irq;
    };
    class Match_Timer : public DriverFactory::Match
    {
    public:
        Match_Timer(){}
        
        int MatchValue(void)
        {
            return 1000000;
        }
        Driver* Instantiate(void)
        {
            return new StandardPC_Timer();
        }
        bool MatchMultiple(void)
        {
            return true;
        }
        
    };
public:
    KernelArray* MatchForParent(Driver *parent)
    {
        KernelString *propertyBus = (KernelString*)parent->PropertyFor(kDriver_Property_Bus);
        if (propertyBus == NULL)
            return NULL;
        if (!propertyBus->IsEqualTo(kDriver_Bus_System))
            return NULL;
        KernelArray *result = new KernelArray();
        bool hasPCI = false;
        // Add timer
        Match_Timer *timer = new Match_Timer();
        result->Add(timer);
        timer->Release();
        // Try PCI?
        if (hasPCI) {
            // Add PCI device
        } else {
            // Add legacy (non-PCI) devices that we would otherwise have found via PCI
            Match_IDE *primary = new Match_IDE(0x1F0, 14);
            result->Add(primary);
            primary->Release();
            Match_IDE *secondary = new Match_IDE(0x170, 15);
            result->Add(secondary);
            secondary->Release();
        }
        result->Autorelease();
        return result;
    }
    
    static void Install(void)
    {
        StandardPC_Factory *that = new StandardPC_Factory();
        Driver::RegisterFactory(that);
        that->Release();
    }
};

static StandardPC_Interrupts s_interrupts;
static CPU s_CPUs[1];

// Assembly code entry point for all interrupts
extern "C" void trap(TrapFrame *frame)
{
    s_interrupts.Trigger(frame);
}

static void CPUMain(void)
{
    // IDT
    s_interrupts.Set();
    // tell everyone ready
    // start scheduler
    CPU_Interrupt_Enable();
}

static void AlternateCPU(void)
{
    // Switch page tables
    // Alternate CPU segments
    // Alternate CPU LAPIC
    // Main
    CPUMain();
}

#include "Process.h"
#include "Thread.h"

static void PrintStack(void **stackBottom)
{
    kprintf("Stack:");
    for (int i = 0; i < 24; i++, stackBottom++)
        kprintf(" %.8x", *stackBottom);
    kprintf("\n");
}

static bool GenericExceptionHandler(void *context, void *state)
{
    CPU_Interrupt_Disable();
    TrapFrame *tf = (TrapFrame*)state;
    kprintf("\nPANIC! CPU exception %.2x: %s\n", tf->TrapNumber, StandardPC::NameForTrap(tf->TrapNumber));
    kprintf("Error %.8x; address %.4x:%.8x\n", tf->ERR, tf->CS, tf->EIP);
    PrintStack((void**)tf->ESP);
    kprintf("CPU %.8x Process %.8x Thread %.8x\n", CPU::Active, Process::Active, Thread::Active);
    Thread::DebugPrint();
    while(1) asm("hlt");
    return true;
}

static bool PageFaultExceptionHandler(void *context, void *state)
{
    CPU_Interrupt_Disable();
    TrapFrame *tf = (TrapFrame*)state;
    UInt32 faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
    kprintf("\nPANIC! CPU exception %.2x: %s\n", tf->TrapNumber, StandardPC::NameForTrap(tf->TrapNumber));
    kprintf("Error %.8x; address %.4x:%.8x\n", tf->ERR, tf->CS, tf->EIP);
    kprintf("Attempting to access %.8x\n", faulting_address);
    PrintStack((void**)tf->ESP);
    kprintf("CPU %.8x Process %.8x Thread %.8x\n", CPU::Active, Process::Active, Thread::Active);
    Thread::DebugPrint();
    while(1) asm("hlt");
    return true;
}

StandardPC::StandardPC()
:Driver("root")
{
    // TODO: Make these separate drivers
    for (int i = 0; i < 0x20; i++)
        InterruptSource()->RegisterHandler(i, GenericExceptionHandler, NULL);
    InterruptSource()->RegisterHandler(0x0E, PageFaultExceptionHandler, NULL);
}

StandardPC::~StandardPC()
{
}

bool StandardPC::Start(Driver *parent)
{
    // Multiprocessor
    // Main CPU LAPIC
    // Main CPU Segments
    s_CPUs[0].InitSegments();
    // PIC
    CPU_PIC_Init(PIC_IRQ_OFFSET);
    // IOAPIC
    // Begin Main CPU
    CPUMain();
    // Configure driver infrastructure and start
    AutoreleasePool pool;
    StandardPC_Factory::Install();
    ATADriver::Install();
    VirtualMemory::ConfigureService(InterruptSource());
    SetProperty(kDriver_Property_Bus, kDriver_Bus_System);
    return Driver::Start(parent);
}

void StandardPC::Stop(void)
{
    _ataSecondary->Stop();
    DetachChild(_ataSecondary);
    _ataSecondary->Release();
    _ataPrimary->Stop();
    DetachChild(_ataPrimary);
    _ataPrimary->Release();
    Driver::Stop();
}

Interrupts* StandardPC::InterruptSource(void)
{
    return &s_interrupts;
}

CPU* StandardPC::GetCPU(int index)
{
    // TODO: multiprocessor
    if (index != 0)
        return NULL;
    return s_CPUs + index;
}
