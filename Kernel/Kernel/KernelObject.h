#ifndef __KERNELOBJECT_H__
#define __KERNELOBJECT_H__

#include "tools.h"

class KernelObject
{
public:
    KernelObject()
    {
        _count = 1;
    }
    
    void AddRef(void)
    {
        AtomicIncrement(&_count);
    }
    
    void Release(void)
    {
        if (!AtomicDecrement(&_count))
            delete this;
    }
    
protected:
    virtual ~KernelObject()
    {
        // Nothing to do
    }
    
private:
    UInt32 _count;
};

#endif // __KERNELOBJECT_H__
