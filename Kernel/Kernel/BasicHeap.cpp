//
//  BasicHeap.cpp
//  MemoryAllocTest
//
//  Created by Colin David Munro on 14/01/2014.
//  Copyright (c) 2014 MICE Software. All rights reserved.
//

#include "BasicHeap.h"
#include "runtime.h"
#include "debug.h"

#define NOTFOUND    0xFFFFFFFF

#define ROUNDUP(val, div)           (((val) + ((div) - 1)) / (div))
#define OFS2PTR(heap, gran, base)   (((unsigned char*)(heap)) + ((gran) * (base)))
#define PTR2OFS(heap, gran, ptr)    ((((unsigned char*)(ptr)) - ((unsigned char*)(heap))) / (gran))

typedef struct BasicHeap_Heap {
    BasicHeap::h_size size; // Including header/etc.
    BasicHeap_Heap *next;
} BasicHeap_Heap;

typedef struct {
    BasicHeap_Heap *base;
    BasicHeap::h_size size;
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
//    kprintf("Allocating %i\n", amount);
//    Test();
    h_size required = amount + sizeof(BasicHeap_Block);
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
//            kprintf("=> %.8x\n", block + 1);
            return block + 1;
        }
    }
//    kprintf("=> failure:\n");
    return NULL;
}

void BasicHeap::Release(void *buffer)
{
    BasicHeap_Block *block = ((BasicHeap_Block*)buffer) - 1;
    h_size count = ROUNDUP((block->size + sizeof(BasicHeap_Block)), _granularity);
    h_size offset = PTR2OFS(block->base, _granularity, block);
    SetChunk(block->base, offset, count, false);
    _count--;
    _allocated -= count * _granularity;
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

void BasicHeap::Test(void)
{
    kprintf("Heaps:\n");
    int heapNo = 0;
    BasicHeap::h_size totalSize = 0;
    for (BasicHeap_Heap *heap = (BasicHeap_Heap*)_root; heap != NULL; heap = heap->next) {
        const BasicHeap::h_size *data = (BasicHeap::h_size*)(heap + 1);
        BasicHeap::h_size max = heap->size / _granularity;
        BasicHeap::h_size run = 0;
        BasicHeap::h_size total = 0;
        BasicHeap::h_size longestRun = 0;
        for (BasicHeap::h_size pos = 0; pos < max; pos++) {
            BasicHeap::h_size index = pos / (sizeof(BasicHeap::h_size) * 8);
            int bitNumber = pos % (sizeof(BasicHeap::h_size) * 8);
            BasicHeap::h_size bit = 1 << bitNumber;
            if (!(data[index] & bit)) {
                total++;
                run++;
            } else {
                if (run > longestRun)
                    longestRun = run;
                run = 0;
            }
        }
        if (run > longestRun)
            longestRun = run;
        int freeBytes = total * _granularity;
        int freeLongest = longestRun * _granularity;
        int amount = (max - total * 10000) / (max * 100);
        kprintf("\t%i: %i total, %i bytes free, %i longest contiguous bytes free, %i/10000 utilisation\n", heapNo, (int)heap->size, freeBytes, freeLongest, amount);
        heapNo++;
        totalSize += total;
    }
    kprintf("\tTotal in heaps: %i bytes\n", totalSize * _granularity);
}
