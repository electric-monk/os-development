//
//  BasicHeap.cpp
//  MemoryAllocTest
//
//  Created by Colin David Munro on 14/01/2014.
//  Copyright (c) 2014 MICE Software. All rights reserved.
//

#include "BasicHeap.h"

#define NOTFOUND    0xFFFFFFFF

#define CHECK_UNDERRUN
#define CHECK_OVERRUN

#define CLEAR_ALLOC
#define CLEAR_FREE

#define TEST_BEFORE 0xDEADBEEF
#define TEST_AFTER  0xDEADF00D
#define TEST_END    0xA110DEAD

class LockHeap
{
public:
    LockHeap(Locking::GenericLock *lock)
    {
        _lock = lock;
        _lock->Lock();
    }
    ~LockHeap()
    {
        _lock->Unlock();
    }
private:
    Locking::GenericLock *_lock;
};
#define HEAP_LOCK LockHeap locker(&_lock)

static void HeapDebug(const char *error, ...)
{
    // TODO
}

// This method takes an amount and a granularity, and returns the minimum number of granules to hold the amount
static inline BasicHeap::h_size ROUNDUP(BasicHeap::h_size amount, BasicHeap::h_size granularity)
{
    return (amount + (granularity - 1)) / granularity;
}

// This method takes a pointer to a base, a granularity and a number of granules, and returns a pointer to the actual address
static inline void* OFS2PTR(void *heap, BasicHeap::h_size granularity, BasicHeap::h_size offset)
{
    return (void*)(BasicHeap::h_size(heap) + (granularity * offset));
}

// This method takes a pointer to a base, a granularity and a pointer into the chunk, and returns the offset granules
static inline BasicHeap::h_size PTR2OFS(void *heap, BasicHeap::h_size granularity, void *pointer)
{
    return (BasicHeap::h_size(pointer) - BasicHeap::h_size(heap)) / granularity;
}

typedef struct BasicHeap_Heap {
    BasicHeap::h_size size; // Including header/etc.
    BasicHeap_Heap *next;
} BasicHeap_Heap;

typedef struct {
#ifdef CHECK_UNDERRUN
    BasicHeap::h_size _before;
#endif
    BasicHeap_Heap *base;
    BasicHeap::h_size size;
#ifdef CHECK_UNDERRUN
    BasicHeap::h_size _after;
#endif
} BasicHeap_Block;

static BasicHeap::h_size FindChunk(BasicHeap_Heap *heap, BasicHeap::h_size granularity, BasicHeap::h_size count)
{
    const BasicHeap::h_size *data = (BasicHeap::h_size*)(heap + 1);
    // TODO: Optimise
    BasicHeap::h_size max = heap->size / granularity;
    BasicHeap::h_size run = 0;
    for (BasicHeap::h_size pos = 0; pos < max; pos++) {
        BasicHeap::h_size index = pos / (sizeof(BasicHeap::h_size) * 8);
        int bitNumber = pos % (sizeof(BasicHeap::h_size) * 8);
        BasicHeap::h_size bit = 1 << bitNumber;
        if (!(data[index] & bit))
            run++;
        else
            run = 0;
        if (run == count)
            return pos - run + 1;
    }
    return NOTFOUND;
}

static void SetChunk(BasicHeap_Heap *heap, BasicHeap::h_size base, BasicHeap::h_size count, bool used)
{
    BasicHeap::h_size *data = (BasicHeap::h_size*)(heap + 1);
    // TODO: Optimise
    while (count != 0) {
        BasicHeap::h_size index = base / (sizeof(BasicHeap::h_size) * 8);
        int bitNumber = base % (sizeof(BasicHeap::h_size) * 8);
        BasicHeap::h_size bit = 1 << bitNumber;
        if (used)
            data[index] |= bit;
        else
            data[index] &= ~bit;
        count--;
        base++;
    }
}

BasicHeap::BasicHeap(BasicHeap::h_size gran)
{
    _root = NULL;
    _total = 0;
    _allocated = 0;
    _count = 0;
    _granularity = gran;
}

BasicHeap::~BasicHeap()
{
    if (_count != 0) {
        // TODO: Error! All of the memory allocated by this heap is about to vanish
    }
}

void BasicHeap::AddBlock(void *offset, BasicHeap::h_size length)
{
    HEAP_LOCK;
    _total += length;
    BasicHeap_Heap *heap = (BasicHeap_Heap*)offset;
    heap->size = length;
    h_size *data = (h_size*)(heap + 1);
    h_size bitFields = ROUNDUP(ROUNDUP(length, _granularity), sizeof(h_size) * 8);
    for (h_size count = bitFields; count != 0; count--, data++)
        *data = 0;
    SetChunk(heap, 0, ROUNDUP(sizeof(BasicHeap_Heap) + (sizeof(h_size) * bitFields), _granularity), true);
    heap->next = (BasicHeap_Heap*)_root;
    _root = heap;
}

void* BasicHeap::Alloc(BasicHeap::h_size amount)
{
    HEAP_LOCK;
    h_size required = amount + sizeof(BasicHeap_Block);
#ifdef CHECK_OVERRUN
    required += sizeof(BasicHeap::h_size);
#endif
    h_size count = ROUNDUP(required, _granularity);
    for (BasicHeap_Heap *heap = (BasicHeap_Heap*)_root; heap != NULL; heap = heap->next) {
        h_size offset = FindChunk(heap, _granularity, count);
        if (offset != NOTFOUND) {
            SetChunk(heap, offset, count, true);
            BasicHeap_Block *block = (BasicHeap_Block*)OFS2PTR(heap, _granularity, offset);
            block->base = heap;
            block->size = amount;
            _count++;
            _allocated += count * _granularity;
#ifdef CHECK_UNDERRUN
            block->_before = TEST_BEFORE;
            block->_after = TEST_AFTER;
#endif
#ifdef CLEAR_ALLOC
            char *blockStart = (char*)(block + 1);
            for (h_size length = (count * _granularity) - sizeof(BasicHeap_Block); length != 0; length--, blockStart++)
                *blockStart = length % 0xFF;
#endif
#ifdef CHECK_OVERRUN
            char *temp = (char*)(block + 1);
            temp += amount;
            ((BasicHeap::h_size*)temp)[0] = TEST_END;
#endif
            return block + 1;
        }
    }
    HeapDebug("Allocation failure for %i\n", amount);
    return NULL;
}

void BasicHeap::Release(void *buffer)
{
    HEAP_LOCK;
    BasicHeap_Block *block = ((BasicHeap_Block*)buffer) - 1;
#ifdef CHECK_UNDERRUN
    if (block->_before != TEST_BEFORE)
        HeapDebug("HEAP: _before invalid\n");
    if (block->_after != TEST_AFTER)
        HeapDebug("HEAP: _after invalid\n");
#endif
    h_size required = block->size + sizeof(BasicHeap_Block);
#ifdef CHECK_OVERRUN
    char *temp = (char*)(block + 1);
    temp += block->size;
    if (((BasicHeap::h_size*)temp)[0] != TEST_END)
        HeapDebug("HEAP: overrun invalid\n");
    required += sizeof(BasicHeap::h_size);
#endif
    h_size count = ROUNDUP(required, _granularity);
    h_size offset = PTR2OFS(block->base, _granularity, block);
    SetChunk(block->base, offset, count, false);
    _count--;
    _allocated -= count * _granularity;
#ifdef CLEAR_FREE
    char *blockStart = (char*)block;
    for (h_size amount = count * _granularity; amount != 0; amount--, blockStart++) {
        *blockStart = amount & 1 ? 0xFF : 0x55; // 0xFF55FF55
    }
#endif
}

BasicHeap::h_size BasicHeap::TotalMemory(void)
{
    return _total;
}

BasicHeap::h_size BasicHeap::UsedMemory(void)
{
    return _allocated;
}

BasicHeap::h_size BasicHeap::FreeMemory(void)
{
    return _total - _allocated;
}

BasicHeap::h_size BasicHeap::AllocationCount(void)
{
    return _count;
}
