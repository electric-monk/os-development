#ifndef __RUNTIME_H__
#define __RUNTIME_H__

class BasicHeap;

#ifndef NULL
#define NULL 0L
#endif

typedef unsigned long size_t;

#ifdef __cplusplus
namespace std {
    typedef unsigned long size_t;
}
inline void* operator new  (std::size_t, void* __p) noexcept {return __p;}
inline void* operator new[](std::size_t, void* __p) noexcept {return __p;}
inline void  operator delete  (void*, void*) {}
inline void  operator delete[](void*, void*) {}
#endif

typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned int UInt32;
typedef unsigned long long UInt64;

typedef signed char SInt8;
typedef signed short SInt16;
typedef signed int SInt32;
typedef signed long long SInt64;

extern BasicHeap *s_coreHeap;

void CopyMemory(void *output, const void *input, UInt32 length);
void CopyFast(void *output, const void *input, UInt32 length);

#endif // __RUNTIME_H__
