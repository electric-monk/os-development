#ifndef __KERNELOBJECT_H__
#define __KERNELOBJECT_H__

#include "tools.h"
#include "KernelFunction.h"

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
    
    void Autorelease(void);
    
    virtual UInt32 Hash(void)
    {
        return UInt32(this);
    }
    
    virtual bool IsEqualTo(KernelObject *other)
    {
        return Hash() == other->Hash();
    }
    
protected:
    virtual ~KernelObject()
    {
        // Nothing to do
    }
    
private:
    UInt32 _count;
};

class KernelArray;

class AutoreleasePool
{
public:
    AutoreleasePool();
    ~AutoreleasePool();
    
    void Flush(void);
    
    void AddObject(KernelObject *object);
    
private:
    KernelArray *_objects;
    AutoreleasePool *_previous;
};

template<typename func> class KernelFunction : public KernelObject
{
public:
    KernelFunction(bicycle::function<func> callback) { m_function = callback; }
    
    bicycle::function<func> Pointer(void) { return m_function; }
    
private:
    bicycle::function<func> m_function;
};

#endif // __KERNELOBJECT_H__
