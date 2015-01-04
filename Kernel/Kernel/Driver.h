#ifndef __DRIVER_H__
#define __DRIVER_H__

#include "KernelObject.h"

class Interrupts;

class Driver : public KernelObject
{
public:
    Driver(const char *name);
    
    virtual int Match(Driver *parent/*, context? */) = 0;
    virtual void Start(Driver *parent) = 0;
    virtual void Stop(void) = 0;
    
    virtual void Ready(void);   // Trigger matching of child drivers
    
    Driver* Parent(void) { return _parent; }
    Driver* Child(int index);
Interrupts* Test(void){return InterruptSource();}
protected:
    ~Driver();
    
    virtual bool ConnectChild(Driver *child);
    virtual void DetachChild(Driver *child);

    virtual Interrupts* InterruptSource(void);
    
private:
    Driver *_start, *_end;          // Children
    Driver *_previous, *_next;      // Siblings
    Driver *_parent;                // Parent
};

#endif // __DRIVER_H__
