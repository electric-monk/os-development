//
//  BasicHeap.cpp
//  MemoryAllocTest
//
//  Created by Colin David Munro on 14/01/2014.
//  Copyright (c) 2014 MICE Software. All rights reserved.
//

#include "BasicHeap.h"
#ifndef TESTING
#include "runtime.h"
#include "tools.h"
#else
#define NULL 0
#include <new>
#endif

#define NOTFOUND    0xFFFFFFFF

#define CHECK_UNDERRUN
#define CHECK_OVERRUN

#define CLEAR_ALLOC
#define CLEAR_FREE

#define TEST_BEFORE 0xDEADBEEF
#define TEST_AFTER  0xDEADF00D
#define TEST_END    0xA110DEAD

#define BYTE_ALIGN  8

#ifdef TESTING
#define HEAP_LOCK
#else
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
#endif

namespace Internal {
    class Chunk {
    public:
        typedef enum {
            ChunkFree,
            ChunkAllocated,
            ChunkEnd,
        } ChunkType;
    private:
        ChunkType _type;
    protected:
        BasicHeap::h_size _size;
    public:
        Chunk(ChunkType newType, BasicHeap::h_size size)
        :_size(size), _type(newType)
        {
        }
        
        ChunkType Type(void)
        {
            return _type;
        }
        
        BasicHeap::h_size Size(void)
        {
            return _size;
        }
        
        Chunk* Next(void)
        {
            return (Chunk*)(((char*)this) + _size);
        }
    };
    
    class Allocation : public Chunk {
    private:
#if defined(CLEAR_ALLOC) || defined(CLEAR_FREE)
        void Scrub(void)
        {
            char *ptr = (char*)GetPointer();
            for (BasicHeap::h_size i = _size - sizeof(Allocation); i != 0; i--, ptr++)
                *ptr = 0;
        }
#endif
    public:
        Allocation(BasicHeap::h_size size)
        :Chunk(ChunkAllocated, size)
        {
#ifdef CLEAR_ALLOC
            Scrub();
#endif
        }
        
#ifdef CLEAR_FREE
        ~Allocation()
        {
            Scrub();
        }
#endif
        
        static Allocation* GetObject(void *pointer)
        {
            return (Allocation*)(((char*)pointer) - sizeof(Allocation));
        }
        
        void* GetPointer(void)
        {
            return ((char*)this) + sizeof(Allocation);
        }
    };
    
    class FreeSpace : public Chunk {
    private:
        BasicHeap *_heap;
    public:
        FreeSpace *last, *next;
        
        FreeSpace(BasicHeap *owner, BasicHeap::h_size size)
        :Chunk(ChunkFree, size), _heap(owner)
        {
            last = _heap->_freeEnd;
            next = NULL;
            _heap->_freeEnd = this;
            if (last)
                last->next = this;
            else
                _heap->_freeStart = this;
        }
        ~FreeSpace()
        {
            if (last)
                last->next = next;
            if (next)
                next->last = last;
            if (this == _heap->_freeStart)
                _heap->_freeStart = next;
            if (this == _heap->_freeEnd)
                _heap->_freeEnd = last;
        }
        
        FreeSpace* Split(BasicHeap::h_size size)
        {
            if (_size < (sizeof(FreeSpace) * 2))
                return this;
            if (size < sizeof(FreeSpace))
                size = sizeof(FreeSpace);
            size = (size + (BYTE_ALIGN - 1)) & -BYTE_ALIGN;   // Some sort of alignment
            
            BasicHeap *owner = _heap;
            BasicHeap::h_size total = _size;
            char *base = (char*)this;
            
            this->~FreeSpace();
            
            new (base + size) FreeSpace(owner, total - size);
            return new (base) FreeSpace(owner, size);
        }
        
        void Merge(void)
        {
            BasicHeap *heap = _heap;
            FreeSpace *freespace = this;
            while (true) {
                void *address = freespace;
                Internal::FreeSpace *next = (Internal::FreeSpace*)freespace->Next();
                if (next->Type() != Internal::Chunk::ChunkFree)
                    break;
                BasicHeap::h_size total = freespace->Size();
                freespace->~FreeSpace();
                total += next->Size();
                next->~FreeSpace();
                freespace = new (address) FreeSpace(heap, total);
            }
        }
    };
}

BasicHeap::BasicHeap()
{
    _freeStart = _freeEnd = NULL;
    _total = 0;
    _used = 0;
    _count = 0;
}

BasicHeap::~BasicHeap()
{
}

void BasicHeap::AddBlock(void *offset, BasicHeap::h_size length)
{
    new (((char*)offset) + length - sizeof(Internal::Chunk)) Internal::Chunk(Internal::Chunk::ChunkEnd, sizeof(Internal::Chunk));
    new (offset) Internal::FreeSpace(this, length - sizeof(Internal::Chunk));
    _total += length;
}

void* BasicHeap::Alloc(BasicHeap::h_size amount)
{
    HEAP_LOCK;
    Internal::FreeSpace *freespace;
    BasicHeap::h_size totalAmount = amount + sizeof(Internal::Allocation);
    for (freespace = _freeStart; freespace != NULL; freespace = freespace->next) {
//        freespace->Merge(); // Just in case
        BasicHeap::h_size available = freespace->Size() - sizeof(Internal::Allocation);
        if (available >= totalAmount) {
            if (available > (totalAmount + 30))
                freespace = freespace->Split(totalAmount);
            break;
        }
    }
    if (!freespace)
        return NULL;
    totalAmount = freespace->Size();    // You might be getting more than you asked for
    freespace->~FreeSpace();
    _used += totalAmount;
    _count++;
    Internal::Allocation *allocation = new (freespace) Internal::Allocation(totalAmount);
    return allocation->GetPointer();
}

static void HeapDebug(const char *error)
{
    //    kprintf(error);
#ifdef TESTING
    throw "Heap integrity failure";
#endif
}

void BasicHeap::Release(void *buffer)
{
    HEAP_LOCK;
    Internal::Allocation *allocation = Internal::Allocation::GetObject(buffer);
    BasicHeap::h_size size = allocation->Size();
    allocation->~Allocation();
    _used -= size;
    _count--;
    Internal::FreeSpace *freespace = new (allocation) Internal::FreeSpace(this, size);
    freespace->Merge();
}

BasicHeap::h_size BasicHeap::TotalMemory(void)
{
    return _total;
}

BasicHeap::h_size BasicHeap::UsedMemory(void)
{
    return _used;
}

BasicHeap::h_size BasicHeap::AllocationCount(void)
{
    return _count;
}

void BasicHeap::Test(void)
{
//    kprintf("%i total, %i used in %i allocations\n", _total, _used, _count);
//    kprintf("Free: ");
//    for (Internal::FreeSpace *freespace = _freeStart; freespace != NULL; freespace = freespace->next) {
//        kprintf("%i; ", freespace->Size());
//        if (freespace->Next()->Type() == Internal::Allocation::ChunkFree)
//            kprintf("*");
//    }
//    kprintf("\n");
}

void BasicHeap::Check(void *pointer)
{
}
