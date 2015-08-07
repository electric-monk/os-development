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
    static void ConfigureService(Interrupts *interruptSource);
    
    VirtualMemory(Process *process, UInt32 length);                     // Find pages to map
    VirtualMemory(Process *process, void *linearBase, UInt32 length);   // Map specified address
    
    void* LinearBase(void) { return _linear; }
    UInt32 LinearLength(void) { return _length; }
    
    Process* GetProcess(void) { return _process; }
    
protected:
    ~VirtualMemory();
    
    virtual void HandlePageFault(void *linearAddress) = 0;  // Called from ISR - if more work is necessary, place the request on a queue before handling. If you don't map it in before returning, thread will be blocked and task switched!
    
    // These two methods allow a virtual memory manager to add/remove a single page, be it in response to a page fault or just because they felt like it
    void Map(/*MAP_FLAGS*/int permissions, void *linearAddress, PhysicalPointer physicalAddress);
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
    KernelDictionary *_pendingFaults;
    
    static bool PageFaultHandler(void *context, void *state);
};

class GrowableStack : public VirtualMemory
{
public:
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
