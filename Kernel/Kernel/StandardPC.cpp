#include "StandardPC.h"
#include "StandardPC_traps.h"
#include "Interrupts.h"
#include "descript.h"
#include "mem_logical.h"
#include "debug.h"
#include "CPU_intr.h"
#include "CPU.h"

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

class StandardPC_Interrupts : public Interrupts
{
private:
    class HandlerHandle
    {
    public:
        HandlerHandle(HandlerHandle **root, InterruptHandler handler, void *context)
        {
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
        return new HandlerHandle(handlers + irq, handler, context);
    }
    
    void UnregisterHandler(InterruptHandlerHandle handler)
    {
        delete (HandlerHandle*)handler;
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
    PrintStack((void**)tf->ESP);
    kprintf("Attempting to access %.8x\n", faulting_address);
    kprintf("CPU %.8x Process %.8x Thread %.8x\n", CPU::Active, Process::Active, Thread::Active);
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

int StandardPC::Match(Driver *parent/*, context? */)
{
    return 999999;
}

void StandardPC::Start(Driver *parent)
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
}

void StandardPC::Stop(void)
{
    
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
