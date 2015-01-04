#include "tools.h"
#include "debug.h"
#include "runtime.h"

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
    return NULL;
}

void* operator new[] (size_t size)
{
	return NULL;
}

void operator delete (void * p)
{
}

void operator delete[] (void * p)
{
}
