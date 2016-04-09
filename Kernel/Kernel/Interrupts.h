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

// A simple class for providing userspace services.
class SystemService
{
public:
    void Register(void);
    void Unregister(void);
    
protected:
    // When a system call occurs, we jump straight into Handle. It'll execute in the kernel, but within the context of the calling Process, so all pointers should remain valid.
    // Parameter values may be changed to return values
    virtual void Handle(UInt64 *parameters) = 0;
    
    // used only for registration, indicates the interrupt (or whatever underlying mechanism is being employed) to hook
    virtual int Interrupt(void) = 0;
    
private:
    InterruptHandlerHandle _handle;
};

#endif // __INTERRUPTS_H__
