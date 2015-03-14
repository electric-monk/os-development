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

void AutoreleasePool::AddObject(KernelObject *object)
{
    _objects->Add(object);
    // Now, release the object - our array will have referenced it, so to make sure anything added to us ends up released once, we'll undo that now.
    // That way, when we're cleaned up, all the objects will be automatically released once.
    object->Release();
}
