#include "tools.h"
#include "debug.h"
#include "runtime.h"
#include "BasicHeap.h"

// Pure virtual functions

extern "C" {

int __cxa_pure_virtual() __attribute__((noinline, used));
int __cxa_pure_virtual()
{
    // oops, pure virtual called!
    FAILURE("Pure virtual method called.");
    return 0;
}

}

// Allocation

void* operator new (size_t size)
{
    return s_coreHeap->Alloc(size);
}

void* operator new[] (size_t size)
{
    return s_coreHeap->Alloc(size);
}

void operator delete (void * p)
{
    s_coreHeap->Release(p);
}

void operator delete[] (void * p)
{
    s_coreHeap->Release(p);
}

// Inexplicable C++ stuff required by GCC or something

// TODO: This needs to handle loading/unloading of modules (which should be possible)

#define ATEXIT_MAX_FUNCS        128

static struct {
    void (*destructor)(void*);
    void *objectPointer;
    void *dsoHandle;
} __exit_functions[ATEXIT_MAX_FUNCS];
UInt32 __exit_functions_count = 0;

extern "C" {
    
// ABI: What is this?
void *__dso_handle = 0;

// ABI
int __cxa_atexit(void (*f)(void*), void *objptr, void *dso)
{
    if (__exit_functions_count >= ATEXIT_MAX_FUNCS) {
        // TODO: Panic
        return -1;
    }
    __exit_functions[__exit_functions_count].destructor = f;
    __exit_functions[__exit_functions_count].objectPointer = objptr;
    __exit_functions[__exit_functions_count].dsoHandle = dso;
    __exit_functions_count++;
    return 0;
}

// ABI
void __cxa_finalize(void *f)
{
    UInt32 i = __exit_functions_count;
    while (i--) {
        if ((__exit_functions[i].destructor != NULL) && ((f == NULL) || (f == __exit_functions[i].destructor))) {
            __exit_functions[i].destructor(__exit_functions[i].objectPointer);
            __exit_functions[i].destructor = NULL;
        }
    }
}

// From libgcc
    // TODO: Check license

UInt64 __udivmoddi4(UInt64 num, UInt64 den, UInt64 *rem_p)
{
    UInt64 quot = 0, qbit = 1;
    
    if ( den == 0 ) {
        return 1/((unsigned)den); /* Intentional divide by zero, without
                                   triggering a compiler warning which
                                   would abort the build */
    }
    
    /* Left-justify denominator and count shift */
    while ( (SInt64)den >= 0 ) {
        den <<= 1;
        qbit <<= 1;
    }
    
    while ( qbit ) {
        if ( den <= num ) {
            num -= den;
            quot += qbit;
        }
        den >>= 1;
        qbit >>= 1;
    }
    
    if ( rem_p )
        *rem_p = num;
    
    return quot;
}

UInt64 __udivdi3(UInt64 num, UInt64 den)
{
    return __udivmoddi4(num, den, NULL);
}

UInt64 __umoddi3(UInt64 num, UInt64 den)
{
    UInt64 v;
    
    (void) __udivmoddi4(num, den, &v);
    return v;
}

void* memmove(void *dest, void *src, int length)
{
    char *cDest = (char*)dest;
    char *cSrc = (char*)src;
    if (cDest == cSrc)
        return cDest;
    if (UInt64(cDest) > UInt64(cSrc)) {
        // Copy up
        while (length) {
            *cDest = *cSrc;
            cDest++;
            cSrc++;
            length--;
        }
    } else {
        // Copy down
        cDest += length - 1;
        cSrc += length - 1;
        while (length) {
            *cDest = *cSrc;
            cDest--;
            cSrc--;
            length--;
        }
    }
    return cDest;
}
    
}
