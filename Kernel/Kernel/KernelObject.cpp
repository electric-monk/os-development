#include "KernelObject.h"
#include "Collections.h"
#include "Thread.h"
#include "debug.h"

static KernelString *kernelPoolName = NULL;
static AutoreleasePool *kernelPool = NULL;

static KernelString* KernelPoolName(void)
{
    if (kernelPoolName == NULL)
        kernelPoolName = new KernelString("_AutoreleasePool");
    return kernelPoolName;
}

static void Kernel_Autorelease_Leak(void)
{
    kprintf("Autorelease: Leak!\n");
}

class AutoreleasePoolWrapper : public KernelObject
{
public:
    CLASSNAME(KernelObject, AutoreleasePoolWrapper);
    
    AutoreleasePoolWrapper(AutoreleasePool *pool)
    {
        _pool = pool;
    }
    
    AutoreleasePool *_pool;
};

static void SetPool(AutoreleasePool *pool)
{
    if (Thread::Active == NULL) {
        kernelPool = pool;
    } else {
        AutoreleasePoolWrapper *wrapper = pool ? new AutoreleasePoolWrapper(pool) : NULL;
        Thread::Active->KernelThreadLocalStorage()->Set(KernelPoolName(), wrapper);
        if (wrapper)
            wrapper->Release();
    }
}

static AutoreleasePool* GetPool(void)
{
    if (Thread::Active == NULL) {
        return kernelPool;
    } else {
        AutoreleasePoolWrapper *wrapper = (AutoreleasePoolWrapper*)Thread::Active->KernelThreadLocalStorage()->ObjectFor(KernelPoolName());
        return wrapper ? wrapper->_pool : NULL;
    }
}

static bool CompareStrings(const char *x, const char *y)
{
    while ((*x == *y) && (*x != '\0') && (*y != '\0')) {
        x++;
        y++;
    }
    return *x == *y;
}

bool KernelObject::IsDerivedFromClass(const char *name)
{
    const char *className;
    int count = 0;
    
    while ((className = GetClassName(count)) != NULL) {
        if (CompareStrings(className, name))
            return true;
        count++;
    }
    return false;
}

bool KernelObject::IsInstanceOfClass(const char *name)
{
    return CompareStrings(GetClassName(0), name);
}

void KernelObject::Autorelease(void)
{
    AutoreleasePool *pool;
    if (Thread::Active == NULL)
        pool = kernelPool;
    else
        pool = GetPool();
    if (pool == NULL)
        Kernel_Autorelease_Leak();
    else
        pool->AddObject(this);
}

AutoreleasePool::AutoreleasePool()
{
    _objects = new KernelArray();
    _previous = GetPool();
    SetPool(this);
}

AutoreleasePool::~AutoreleasePool()
{
    SetPool(_previous);
    _objects->Release();
}

void AutoreleasePool::Flush(void)
{
    _objects->Release();
    _objects = new KernelArray();
}

void AutoreleasePool::AddObject(KernelObject *object)
{
    _objects->Add(object);
    // Now, release the object - our array will have referenced it, so to make sure anything added to us ends up released once, we'll undo that now.
    // That way, when we're cleaned up, all the objects will be automatically released once.
    object->Release();
}
KernelObject::~KernelObject()
{
}

