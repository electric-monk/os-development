#ifndef __MEM_VIRTUAL_H__
#define __MEM_VIRTUAL_H__

#include "KernelObject.h"
#include "Collections.h"
#include "mem_physical.h"

class Process;
class Interrupts;
class SPageDirectoryInfo;
class GrowableStackMapping;
class Thread;

class VirtualMemory : public KernelObject
{
public:
    CLASSNAME(KernelObject, VirtualMemory);
    
    static void ConfigureService(Interrupts *interruptSource);
    
    VirtualMemory(Process *process, UInt32 length);                     // Find pages to map
    VirtualMemory(Process *process, void *linearBase, UInt32 length);   // Map specified address
    
    void* LinearBase(void) { return _linear; }
    UInt32 LinearLength(void) { return _length; }
    
    Process* GetProcess(void) { return _process; }
    
protected:
    ~VirtualMemory();
    
    // When implementing this, there are a few choices:
    // 1) Map a page and return - note that while it's within an ISR, interrupts will have been enabled, so take as long as you like
    // 2) Don't map a page and return - the thread will enter the scheduler as if the timer interrupt had done it (allowing you to trigger some other thread to do something)
    // 3) Turn interrupts on, thus switching to kernel thread mode. This allows the thread to do useful work inside the kernel, whilst blocking the user side of the thread. Use the InterruptEnabler class to ensure interrupt correctness.
    virtual void HandlePageFault(void *linearAddress) = 0;
    
    // These two methods allow a virtual memory manager to add/remove a single page, be it in response to a page fault or just because they felt like it
    void Map(/*MAP_FLAGS*/int permissions, void *linearAddress, PhysicalPointer physicalAddress);
    void Map(/*MAP_FLAGS*/int permissions, void *linearAddress, PhysicalPointer physicalAddress, bicycle::function<void(void*)> completion);
    void Unmap(void *linearAddress);
    bool IsMapped(void *linearAddress);
    
    SPageDirectoryInfo* PageDirectory(void);
    
private:
    Process *_process;
    UInt32 _identifier;
    void *_linear;
    UInt32 _length;
    
    void Init(Process *process);
    
    // Automatic thread blocking functionality
    void AddFault(void *address, Thread *callee);
    void CheckMap(void *address);
    void UpdateThread(void *address, Thread *callee);
    KernelDictionary *_pendingFaults;
    
    static bool PageFaultHandler(void *context, void *state);
};

class GrowableStack : public VirtualMemory
{
public:
    CLASSNAME(VirtualMemory, GrowableStack);
    
    GrowableStack(Process *process, UInt32 maximumPages);
    
    void* StackTop(void) { return ((char*)LinearBase()) + LinearLength(); }
    
    VirtualMemory* MapIntoKernel(void);
    
protected:
    ~GrowableStack();
    
    void HandlePageFault(void *linearAddress);
    
private:
    friend GrowableStackMapping;
    PhysicalPointer GetAddress(UInt32 offset);
};

#endif // __MEM_VIRTUAL_H__
