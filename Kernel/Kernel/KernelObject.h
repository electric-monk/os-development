#ifndef __KERNELOBJECT_H__
#define __KERNELOBJECT_H__

#include "tools.h"
#include "KernelFunction.h"

#define CLASSNAME(super,name)         const char* GetClassName(int level) { if (level == 0) return #name; return super::GetClassName(level - 1); }

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
    
    // Anybody wanting to use a class with some home-made RTTI needs to make sure the relevant classes implement this method
    virtual const char* GetClassName(int level)
    {
        if (level != 0)
            return NULL;
        return "KernelObject";
    }
    
    bool IsDerivedFromClass(const char *name);
    bool IsInstanceOfClass(const char *name);
    
protected:
    virtual ~KernelObject();
    
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
    CLASSNAME(KernelObject, KernelFunction);
    
    KernelFunction(bicycle::function<func> callback) { m_function = callback; }
    
    bicycle::function<func> Pointer(void) { return m_function; }
    
private:
    bicycle::function<func> m_function;
};

#endif // __KERNELOBJECT_H__
