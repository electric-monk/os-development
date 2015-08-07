#include "mem_virtual.h"
#include "Collections.h"
#include "StandardPC.h"
#include "mem_logical.h"
#include "Interrupts.h"
#include "Process.h"
#include "StandardPC_traps.h"
#include "Thread.h"
#include "Scheduler.h"
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
        void *voidFaultingAddress = (void*)faulting_address;
        Thread *activeThread = Thread::Active;  // Ideally we'd always implement the 'block on unhandled fault' code, but if there's no thread, there's nothing to block
        if (activeThread != NULL)
            handler->AddFault(voidFaultingAddress, activeThread);
        handler->HandlePageFault(voidFaultingAddress);
        if (activeThread != NULL) {
            if (activeThread->_state == tsBlocked)    // AddFault will set this, CheckMap will reset it. Anyone else messing with it will cause a crash anyway, so we can use it
                Scheduler::EnterFromInterrupt();
            else
                activeThread->_state = tsRunning;    // CheckMap will only set it to tsRunnable, but if we get here it's obviously running
        }
        return true;
    }
    return false;
}

// This method adds a newly occurred fault, by noting the calling thread and address.
void VirtualMemory::AddFault(void *address, Thread *callee)
{
    // First, mark thread non-runnable
    callee->_state = tsBlocked;
    // Now, add the address and callee to the list
    KernelNumber *addressObject = new KernelNumber((UInt32)address);
    KernelArray *blockedThreadsForPage = (KernelArray*)_pendingFaults->ObjectFor(addressObject);
    if (blockedThreadsForPage == NULL) {
        blockedThreadsForPage = new KernelArray();
        _pendingFaults->Set(addressObject, blockedThreadsForPage);
        blockedThreadsForPage->Release();
    }
    addressObject->Release();
    blockedThreadsForPage->Add(callee);
}

// This method checks to see if a Map() call has affected any threads blocked on a page fault they encountered.
void VirtualMemory::CheckMap(void *address)
{
    KernelNumber *addressObject = new KernelNumber((UInt32)address);
    KernelArray *blockedThreadsForPage = (KernelArray*)_pendingFaults->ObjectFor(addressObject);
    if (blockedThreadsForPage == NULL) {
        addressObject->Release();
        return;
    }
    blockedThreadsForPage->AddRef();
    _pendingFaults->Set(addressObject, NULL);
    addressObject->Release();
    UInt32 max = blockedThreadsForPage->Count();
    for (UInt32 i = 0; i < max; i++) {
        Thread *blockedThread = (Thread*)blockedThreadsForPage->ObjectAt(i);
        blockedThread->_state = tsRunnable;
    }
    blockedThreadsForPage->Release();
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

void VirtualMemory::Init(Process *process)
{
    _pendingFaults = new KernelDictionary();
    if (_identifierCounter == 0)
        _identifierCounter++;
    _identifier = _identifierCounter++;
    _process = process;
    KernelNumber *number = new KernelNumber(_identifier);
    s_identifierMap->Set(number, this);
    number->Release();
//    kprintf("VirtualMemory 0x%.8x: process 0x%.8x from 0x%.8x to 0x%.8x\n", this, _process, _linear, _linear + _length);
}

VirtualMemory::VirtualMemory(Process *process, UInt32 length)
{
    Init(process);
    _length = length;
    _linear = PageDirectory()->Map(FlagForProcess(_process, fmNotPresent), _process ? pmApplication : pmKernel, (PhysicalPointer)(_identifier << 1), _length >> 12);
}

VirtualMemory::VirtualMemory(Process *process, void *linearBase, UInt32 length)
{
    Init(process);
    _length = length;
    _linear = linearBase;
    // Map each page
    length = PAGE_LIMITED(length) / PAGE_SIZE;  // Convert to pages
    UInt32 offset = (UInt32)linearBase;
    while (length != 0) {
        PageDirectory()->Map(FlagForProcess(_process, fmNotPresent), (void*)offset, (PhysicalPointer)(_identifier << 1));
        length--;
        offset += PAGE_SIZE;
    }
}

VirtualMemory::~VirtualMemory()
{
//    kprintf("VirtualMemory 0x%.8x end\n", this);
    KernelNumber *number = new KernelNumber(_identifier);
    s_identifierMap->Set(number, NULL);
    number->Release();
    PageDirectory()->Unmap(_linear, (size_t)_length >> 12);
    _pendingFaults->Release();
}

SPageDirectoryInfo* VirtualMemory::PageDirectory(void)
{
    return _process ? &_process->pageDirectory : &rootAddressSpace;
}

void VirtualMemory::Map(/*MAP_FLAGS*/int permissions, void *linearAddress, PhysicalPointer physicalAddress)
{
    Map(permissions, linearAddress, physicalAddress, [](void*){return 0;});
}

void VirtualMemory::Map(/*MAP_FLAGS*/int permissions, void *linearAddress, PhysicalPointer physicalAddress, bicycle::function<int(void*)> completion)
{
    if ((linearAddress < _linear) || (linearAddress >= (((char*)_linear) + _length)))
        return; // TODO: Error!
    PageDirectory()->Map(FlagForProcess(_process, permissions), linearAddress, physicalAddress);
    completion(linearAddress);
    if (!(permissions & fmNotPresent))
        CheckMap(linearAddress);
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
