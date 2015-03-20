#include "mem_virtual.h"
#include "Collections.h"
#include "StandardPC.h"
#include "mem_logical.h"
#include "Interrupts.h"
#include "Process.h"
#include "StandardPC_traps.h"
#include "debug.h"

static UInt32 _identifierCounter = 0;
static KernelDictionary *s_identifierMap;
static KernelNumber *s_utilityNumber = NULL;

// TODO: not a kerneldictionary, something faster

bool VirtualMemory::PageFaultHandler(void *context, void *state)
{
    UInt32 faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
    UInt32 data = currentAddressSpace->AddressRaw((void*)faulting_address);
    int identifier = data >> 1;
    s_utilityNumber->Reset(identifier);
    VirtualMemory *handler = (VirtualMemory*)s_identifierMap->ObjectFor(s_utilityNumber);
    if (handler != NULL) {
        handler->HandlePageFault((void*)faulting_address);
        return true;
    }
    return false;
}

void VirtualMemory::ConfigureService(Interrupts *interruptSource)
{
    s_identifierMap = new KernelDictionary();
    s_utilityNumber = new KernelNumber(0);
    interruptSource->RegisterHandler(pePageFault, PageFaultHandler, NULL);
}

static UInt32 FlagForProcess(Process *process, UInt32 flags)
{
    return flags | ((process != NULL) ? fmUser : 0);
}

VirtualMemory::VirtualMemory(Process *process, UInt32 length)
{
    if (_identifierCounter == 0)
        _identifierCounter++;
    _identifier = _identifierCounter++;
    _process = process;
    _length = length;
    _linear = PageDirectory()->Map(FlagForProcess(_process, fmNotPresent), _process ? pmApplication : pmKernel, (PhysicalPointer)(_identifier << 1), _length >> 12);
    KernelNumber *number = new KernelNumber(_identifier);
    s_identifierMap->Set(number, this);
    number->Release();
//    kprintf("VirtualMemory 0x%.8x: process 0x%.8x from 0x%.8x to 0x%.8x\n", this, _process, _linear, _linear + _length);
}

VirtualMemory::~VirtualMemory()
{
//    kprintf("VirtualMemory 0x%.8x end\n", this);
    KernelNumber *number = new KernelNumber(_identifier);
    s_identifierMap->Set(number, NULL);
    number->Release();
    PageDirectory()->Unmap(_linear, _length >> 12);
}

SPageDirectoryInfo* VirtualMemory::PageDirectory(void)
{
    return _process ? &_process->pageDirectory : &rootAddressSpace;
}

void VirtualMemory::Map(/*MAP_FLAGS*/int permissions, void *linearAddress, PhysicalPointer physicalAddress)
{
    if ((linearAddress < _linear) || (linearAddress >= (((char*)_linear) + _length)))
        return; // TODO: Error!
    PageDirectory()->Map(FlagForProcess(_process, permissions), linearAddress, physicalAddress);
}

void VirtualMemory::Unmap(void *linearAddress)
{
    Map(fmNotPresent, linearAddress, (PhysicalPointer)(_identifier << 1));
}

bool VirtualMemory::IsMapped(void *linearAddress)
{
    return PageDirectory()->Address(linearAddress) != CPhysicalMemory::Minimum;
}

GrowableStack::GrowableStack(Process *process, UInt32 maximumPages)
:VirtualMemory(process, (maximumPages + 1) * 4096)
{
    // No initialisation necessary
}

GrowableStack::~GrowableStack()
{
    char *base = (char*)LinearBase();
    for (UInt32 page = 0; page < LinearLength(); page += 4096, base += 4096) {
        PhysicalPointer pointer = PageDirectory()->Address(base);
        // pointer should not be CPhysicalMemory::Invalid
        if (pointer != CPhysicalMemory::Minimum)
            CPhysicalMemory::ReleasePages(pointer);
    }
}

static void StackOverflow(Process *process)
{
    kprintf("Stack overflow!\n");
}

void GrowableStack::HandlePageFault(void *linearAddress)
{
    if (linearAddress == LinearBase()) {
        // Thread is experiencing a stack overflow!
        StackOverflow(GetProcess());
    } else {
        PhysicalPointer newPage = CPhysicalMemory::AllocateContiguousPages();
        Map(fmWritable, linearAddress, newPage);
    }
}

class GrowableStackMapping : public VirtualMemory
{
public:
    GrowableStackMapping(GrowableStack *stack)
    :VirtualMemory(NULL, stack->LinearLength())
    {
        _stack = stack;
        _stack->AddRef();
    }
    
protected:
    ~GrowableStackMapping()
    {
        _stack->Release();
    }

    void HandlePageFault(void *linearAddress)
    {
        PhysicalPointer pointer = _stack->GetAddress(((char*)LinearBase()) - ((char*)linearAddress));
        if (pointer == CPhysicalMemory::Minimum)
            /* Stack overflow */;
        Map(fmWritable, linearAddress, pointer);
    }

private:
    GrowableStack *_stack;
};

PhysicalPointer GrowableStack::GetAddress(UInt32 offset)
{
    void *actualAddress = (char*)LinearBase() + offset;
    PhysicalPointer pointer = PageDirectory()->Address(actualAddress);
    if (pointer == CPhysicalMemory::Minimum) {
        HandlePageFault(actualAddress);
        pointer = PageDirectory()->Address(actualAddress);
    }
    return pointer;
}

VirtualMemory* GrowableStack::MapIntoKernel(void)
{
    if (GetProcess() == NULL)
        return this;
    GrowableStackMapping *mapping = new GrowableStackMapping(this);
    mapping->Autorelease();
    return mapping;
}
