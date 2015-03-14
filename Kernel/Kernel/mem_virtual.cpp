#include "mem_virtual.h"
#include "Collections.h"
#include "StandardPC.h"
#include "mem_logical.h"
#include "Interrupts.h"
#include "Process.h"
#include "StandardPC_traps.h"

static UInt32 _identifierCounter = 0;
static KernelDictionary *s_identifierMap;

static bool PageFaultHandler(void *context, void *state)
{
    TrapFrame *tf = (TrapFrame*)state;
    
}

void VirtualMemory::ConfigureService(Interrupts *interruptSource)
{
    s_identifierMap = new KernelDictionary();
    interruptSource->RegisterHandler(pePageFault, PageFaultHandler, NULL);
}

static UInt32 FlagForProcess(Process *process, UInt32 flags)
{
    return flags | ((process != NULL) ? fmUser : 0);
}

VirtualMemory::VirtualMemory(Process *process, UInt32 length)
{
    _identifier = _identifierCounter++;
    _process = process;
    _length = length;
    _linear = PageDirectory()->Map(FlagForProcess(_process, fmNotPresent), _process ? pmApplication : pmKernel, (PhysicalPointer)(_identifier << 1), _length >> 12);
    KernelNumber *number = new KernelNumber(_identifier);
    s_identifierMap->Set(number, this);
    number->Release();
}

VirtualMemory::~VirtualMemory()
{
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

void GrowableStack::HandlePageFault(void *linearAddress)
{
    if (linearAddress == LinearBase()) {
        // Thread is experiencing a stack overflow!
    } else {
        PhysicalPointer newPage = CPhysicalMemory::AllocateContiguousPages();
        Map(fmWritable, linearAddress, newPage);
    }
}
