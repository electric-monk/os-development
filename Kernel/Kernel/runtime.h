#ifndef __RUNTIME_H__
#define __RUNTIME_H__

#ifndef NULL
#define NULL 0L
#endif

typedef unsigned int size_t;

typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned int UInt32;
typedef unsigned long long UInt64;

typedef signed char SInt8;
typedef signed short SInt16;
typedef signed int SInt32;
typedef signed long long SInt64;

#include "BasicHeap.h"

extern BasicHeap *s_coreHeap;

#endif // __RUNTIME_H__
