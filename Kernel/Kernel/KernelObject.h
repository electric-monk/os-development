#ifndef __KERNELOBJECT_H__
#define __KERNELOBJECT_H__

#include "tools.h"
#include "KernelFunction.h"

typedef UInt32 Handle;  // Used to represent a KernelObject in userspace

class KernelDictionary;

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
    
public:
    class DestructionWatcherHandle
    {
    public:
        virtual ~DestructionWatcherHandle() {}
    };
    DestructionWatcherHandle* Watch(bicycle::function<int(void)> onDestroy);
    
protected:
    virtual ~KernelObject();
    
private:
    UInt32 _count;
    DestructionWatcherHandle *_watchStart, *_watchEnd;
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

// This class allows a process (or the kernel itself) to map between actual kernel pointers, and per process handle. This provides
// security as each process can only see objects it's been told about. Note that this does not save a reference, the userspace
// process should manage objects itself. Released objects will be automatically removed (hence this class is closely tied to KernelObject).
class ObjectMapper : public KernelObject
{
public:
    CLASSNAME(KernelObject, ObjectMapper);
    
    ObjectMapper();
    
    Handle Map(KernelObject *object);
    void Unmap(Handle object);
    void Unmap(KernelObject *object);
    KernelObject* Find(Handle object);
    
protected:
    ~ObjectMapper();

private:
    Handle _nextHandle;
    KernelDictionary *_handleMap, *_objectMap;
};

#endif // __KERNELOBJECT_H__
