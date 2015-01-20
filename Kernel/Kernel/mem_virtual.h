#ifndef __MEM_VIRTUAL_H__
#define __MEM_VIRTUAL_H__

#include "KernelObject.h"
#include "Collections.h"
#include "mem_physical.h"

class Process;
class Interrupts;
class SPageDirectoryInfo;

class VirtualMemory : public KernelObject
{
public:
    static void ConfigureService(Interrupts *interruptSource);
    
    VirtualMemory(Process *process, UInt32 length);
    
    void* LinearBase(void) { return _linear; }
    UInt32 LinearLength(void) { return _length; }
    
protected:
    ~VirtualMemory();
    
    virtual void HandlePageFault(void *linearAddress) = 0;  // Called from ISR - if more work is necessary, place the request on a queue before handling
    
    // These two methods allow a virtual memory manager to add/remove a single page, be it in response to a page fault or just because they felt like it
    void Map(/*MAP_FLAGS*/int permissions, void *linearAddress, PhysicalPointer physicalAddress);
    void Unmap(void *linearAddress);
    
    SPageDirectoryInfo* PageDirectory(void);
    
private:
    Process *_process;
    UInt32 _identifier;
    void *_linear;
    UInt32 _length;
};

class GrowableStack : public VirtualMemory
{
public:
    GrowableStack(Process *process, UInt32 maximumPages);
    
    void* StackTop(void) { return ((char*)LinearBase()) + LinearLength(); }
    
protected:
    ~GrowableStack();
    
    void HandlePageFault(void *linearAddress);
};

#endif // __MEM_VIRTUAL_H__
