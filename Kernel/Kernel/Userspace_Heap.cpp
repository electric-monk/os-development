#include "Userspace_Heap.h"
#include "mem_logical.h"

UserspaceHeap::UserspaceHeap(Process *process, UInt32 length)
:VirtualMemory(process, length)
{
    
}

void UserspaceHeap::HandlePageFault(void *linearAddress)
{
    PhysicalPointer newPage = CPhysicalMemory::AllocateContiguousPages();
    // TODO: Page should support paging out/compression/etc.
    Map(fmWritable, linearAddress, newPage);
}
