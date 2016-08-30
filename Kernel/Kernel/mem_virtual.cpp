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
#include "CPU_intr.h"

static UInt32 _identifierCounter = 0;

// TODO: not a kerneldictionary, something faster
static KernelDictionary *s_identifierDictionary;
static KernelNumber *s_utilityNumber = NULL;
static void InitMap(void)
{
    s_identifierDictionary = new KernelDictionary();
    s_utilityNumber = new KernelNumber(0);
}
class WeakWrap : public KernelObject
{
public:
    WeakWrap(VirtualMemory *mem)
    {
        _mem = mem;
    }
    VirtualMemory* Value(void)
    {
        return _mem;
    }
private:
    VirtualMemory *_mem;
};
static void SetMap(UInt32 identifier, VirtualMemory *vmem)
{
    KernelNumber *number = new KernelNumber(identifier);
    WeakWrap *wrap = new WeakWrap(vmem);
    s_identifierDictionary->Set(number, wrap);
    number->Release();
    wrap->Release();
}
static VirtualMemory* GetMap(UInt32 identifier)
{
    s_utilityNumber->Reset(identifier);
    WeakWrap *wrap = (WeakWrap*)s_identifierDictionary->ObjectFor(s_utilityNumber);
    return wrap ? wrap->Value() : NULL;
}

bool VirtualMemory::PageFaultHandler(void *context, void *state)
{
    UInt32 faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
    UInt32 data = currentAddressSpace->AddressRaw((void*)faulting_address);
    int identifier = data >> 1;
    VirtualMemory *handler = GetMap(identifier);
    if (handler != NULL) {
        InterruptEnabler enabler;   // Enabling interrupts allows us to use memory allocation/etc.
        void *voidFaultingAddress = (void*)faulting_address;
        Thread *activeThread = Thread::Active;  // Ideally we'd always implement the 'block on unhandled fault' code, but if there's no thread, there's nothing to block
        if (activeThread != NULL)
            handler->AddFault(voidFaultingAddress, activeThread);
        handler->HandlePageFault(voidFaultingAddress);
        if (activeThread != NULL)
            handler->UpdateThread(voidFaultingAddress, activeThread);
        return true;
    }
    return false;
}

// This method adds a newly occurred fault, by noting the calling thread and address.
void VirtualMemory::AddFault(void *address, Thread *callee)
{
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
        if (blockedThread != Thread::Active)
            blockedThread->_state = tsRunnable;
    }
    blockedThreadsForPage->Release();
}

// This method checks if a thread is meant to be blocked now
void VirtualMemory::UpdateThread(void *address, Thread *callee)
{
    KernelNumber *addressObject = new KernelNumber((UInt32)address);
    KernelArray *blockedThreadsForPage = (KernelArray*)_pendingFaults->ObjectFor(addressObject);
    addressObject->Release();
    if (blockedThreadsForPage != NULL) {
        if (blockedThreadsForPage->Contains(callee)) {
            InterruptDisabler disabler;
            callee->_state = tsBlocked;
            Scheduler::EnterFromInterrupt();
        }
    }
}

void VirtualMemory::ConfigureService(Interrupts *interruptSource)
{
    InitMap();
    interruptSource->RegisterHandler(pePageFault, PageFaultHandler, NULL);
}

static UInt32 FlagForProcess(Process *process, UInt32 flags)
{
    // TODO: Should probably use a flag, not just process itself
    return flags | ((process != NULL) ? fmUser : 0);
}

void VirtualMemory::Init(Process *process)
{
    _pendingFaults = new KernelDictionary();
    if (_identifierCounter == 0)
        _identifierCounter++;
    _identifier = _identifierCounter++;
    _process = process;
    SetMap(_identifier, this);
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
    SetMap(_identifier, NULL);
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
    CLASSNAME(VirtualMemory, GrowableStackMapping);
    
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
