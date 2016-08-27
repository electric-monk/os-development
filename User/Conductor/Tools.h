#ifndef __TOOLS_H__
#define __TOOLS_H__

#include "Runtime.h"

static inline UInt32 xchg(volatile UInt32 *addr, UInt32 newval)
{
    UInt32 result;
    
    // The + in "+m" denotes a read-modify-write operand.
    asm volatile("lock; xchgl %0, %1" :
                 "+m" (*addr), "=a" (result) :
                 "1" (newval) :
                 "cc");
    return result;
}

#endif // __TOOLS_H__
