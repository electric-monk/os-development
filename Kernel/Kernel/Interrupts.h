#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include "KernelObject.h"

typedef void* InterruptHandlerHandle;

typedef bool (*InterruptHandler)(void *context, void *state);
typedef bicycle::function<bool(void *state)> InterruptHandlerBlock;

// Abstract base class to provide generic interrupt interface
// to drivers.
class Interrupts : public KernelObject
{
public:
    CLASSNAME(KernelObject, Interrupts);
    
    virtual InterruptHandlerHandle RegisterHandler(int irq, InterruptHandler handler, void *context)
    {
        // Convenience implementation
        return RegisterHandler(irq, [handler, context](void *state){
            return handler(context, state);
        });
    }
    virtual InterruptHandlerHandle RegisterHandler(int irq, InterruptHandlerBlock handler) = 0;
    virtual void UnregisterHandler(InterruptHandlerHandle handler) = 0;
    
    virtual void ConfigureSyscall(int irq) = 0;
};

#endif // __INTERRUPTS_H__
