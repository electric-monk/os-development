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

class BasicHeap
{
public:
    typedef unsigned long h_size;

public:
    BasicHeap(h_size gran = 8192);
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
    InterruptableSpinLock _lock;
#endif
    void *_root;
    h_size _total, _allocated, _count, _granularity;
};

#endif /* defined(__MemoryAllocTest__BasicHeap__) */
