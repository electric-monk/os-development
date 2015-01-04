#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include "KernelObject.h"

typedef void* InterruptHandlerHandle;

typedef bool (*InterruptHandler)(void *context, void *state);

// Abstract base class to provide generic interrupt interface
// to drivers.
class Interrupts : public KernelObject
{
public:
    virtual InterruptHandlerHandle RegisterHandler(int irq, InterruptHandler handler, void *context) = 0;
    virtual void UnregisterHandler(InterruptHandlerHandle handler) = 0;
    
    virtual void ConfigureSyscall(int irq) = 0;
};

#endif // __INTERRUPTS_H__
