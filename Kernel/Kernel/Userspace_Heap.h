#ifndef __USERSPACE_HEAP_H__
#define __USERSPACE_HEAP_H__

#include "mem_virtual.h"

// TODO: Make this class support swapping to disk by counting page use
// TODO: Also make this class support in-place compression

class UserspaceHeap : public VirtualMemory
{
public:
    CLASSNAME(VirtualMemory, UserspaceHeap);
    
    UserspaceHeap(Process *process, UInt32 length);
    
protected:
    void HandlePageFault(void *linearAddress);
};

#endif // __USERSPACE_HEAP_H__
