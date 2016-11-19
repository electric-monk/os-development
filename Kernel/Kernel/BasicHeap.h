//
//  BasicHeap.h
//  MemoryAllocTest
//
//  Created by Colin David Munro on 14/01/2014.
//  Copyright (c) 2014 MICE Software. All rights reserved.
//

#ifndef __MemoryAllocTest__BasicHeap__
#define __MemoryAllocTest__BasicHeap__

#ifndef TESTING
#include "Locking.h"
#endif

namespace Internal {
    class FreeSpace;
}

class BasicHeap
{
public:
    typedef unsigned long h_size;

public:
    BasicHeap();
    ~BasicHeap();
    
    // Management
    void AddBlock(void *offset, h_size length);
    
    // Simple allocation
    void* Alloc(h_size amount);
    void Release(void *buffer);
    
    // Statistics
    h_size TotalMemory(void);
    h_size UsedMemory(void);
    h_size FreeMemory(void);
    h_size AllocationCount(void);
    void Test(void);
    void Check(void*);
    
private:
#ifndef TESTING
    UninterruptableSpinLock _lock;
#endif
    friend class Internal::FreeSpace;
    Internal::FreeSpace *_freeStart, *_freeEnd;
    
    h_size _total, _used, _count;
};

#endif /* defined(__MemoryAllocTest__BasicHeap__) */
