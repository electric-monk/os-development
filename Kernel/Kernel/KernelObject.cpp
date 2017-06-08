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

namespace KernelObject_Internal {
    class DestructionWatcherHandle : public KernelObject::DestructionWatcherHandle
    {
    public:
        DestructionWatcherHandle(KernelObject::DestructionWatcherHandle **start, KernelObject::DestructionWatcherHandle **end, bicycle::function<void(void)> onDestroy)
        {
            _destroy = onDestroy;
            
            // Link to linked list
            _start = start;
            _end = end;
            _next = NULL;
            _last = *_end;
            if (_last == NULL)
                *_start = this;
            else
                ((DestructionWatcherHandle*)_last)->_next = this;
            *_end = this;
        }
        
        ~DestructionWatcherHandle()
        {
            if (_last)
                ((DestructionWatcherHandle*)_last)->_next = _next;
            if (_next)
                ((DestructionWatcherHandle*)_next)->_last = _last;
            if (*_start == this)
                *_start = _next;
            if (*_end == this)
                *_end = _last;
        }
        
        void Run(void)
        {
            _destroy();
        }
        
    private:
        KernelObject::DestructionWatcherHandle **_start, **_end, *_last, *_next;
        bicycle::function<void(void)> _destroy;
    };
    
    class HandledObject : public KernelObject
    {
    public:
        CLASSNAME(KernelObject, KernelObject_Internal::HandledObject);
        
        Handle GetHandle(void)
        {
            return _handle;
        }
        KernelObject* Object(void)
        {
            return _object;
        }
        
        bool UserAddRef(void)
        {
            // Currently, it's always fine to retain
            _object->AddRef();
            _userRetainCount++;
            return true;
        }
        
        bool UserRelease(void)
        {
            // Only release for retains the userspace process made
            if (_userRetainCount == 0)
                return false;
            _userRetainCount--;
            _object->Release();
            return true;
        }
        
        void AdjustRetain(UInt32 adjust)
        {
            _userRetainCount += adjust;
        }
        
        HandledObject(int handle, KernelObject *object, bicycle::function<int(HandledObject*)> onDestroy, UInt32 initialUserRetains)
        :_handle(handle), _object(object), _userRetainCount(initialUserRetains)
        {
            _handle = handle;
            _object = object;
            _watcher = object->Watch([this, onDestroy]{
                ASSERT(_userRetainCount == 0);
                _watcher = NULL;    // Remove our own reference, as it'll now be released
                onDestroy(this);        // Call the real handler
            });
        }
        
        ~HandledObject()
        {
            if (_watcher)
                delete _watcher;
            // If we hit here, the process stopped using the handle before releasing it
            while (_userRetainCount) {
                _object->Release();
                _userRetainCount--;
            }
        }
        
    private:
        Handle _handle;
        KernelObject *_object;
        DestructionWatcherHandle *_watcher;
        UInt32 _userRetainCount;
    };
}

KernelObject::DestructionWatcherHandle* KernelObject::Watch(bicycle::function<void(void)> onDestroy)
{
    return new KernelObject_Internal::DestructionWatcherHandle(&_watchStart, &_watchEnd, onDestroy);
}

KernelObject::~KernelObject()
{
    while (_watchStart) {
        ((KernelObject_Internal::DestructionWatcherHandle*)_watchStart)->Run();
        delete _watchStart;
    }
}

ObjectMapper::ObjectMapper()
{
    _handleMap = new KernelDictionary();
    _objectMap = new KernelDictionary();
    _nextHandle = 1;
}

ObjectMapper::~ObjectMapper()
{
    _handleMap->Release();
    _objectMap->Release();
}

Handle ObjectMapper::Map(KernelObject *object, UInt32 userspaceRetainCount)
{
    if (object == NULL)
        return 0;
    // Do we already have it?
    KernelNumber *handleObject = new KernelNumber((UInt32)object);
    KernelObject_Internal::HandledObject *entry = (KernelObject_Internal::HandledObject*)_objectMap->ObjectFor(handleObject);
    if (entry) {
        handleObject->Release();
        entry->AdjustRetain(userspaceRetainCount);
        return entry->GetHandle();
    }
    // Find a handle number
    KernelNumber *handleNumber = new KernelNumber(_nextHandle);
    while (_handleMap->ObjectFor(handleNumber)) {
        _nextHandle++;
        handleNumber->Reset(_nextHandle);
    }
    _nextHandle++;  // Prepare for next time
    if (_nextHandle == 0)
        _nextHandle++;
    // Create new item
    entry = new KernelObject_Internal::HandledObject(handleNumber->Value(), object, [this](KernelObject_Internal::HandledObject *activeEntry){
        // TODO: a queue - who knows where this will be running
        KernelNumber *number = new KernelNumber(activeEntry->GetHandle());
        _handleMap->Set(number, NULL);
        number->Release();
        KernelNumber *object = new KernelNumber((UInt32)activeEntry->Object());
        _objectMap->Set(object, NULL);
        object->Release();
        return 0;
    }, userspaceRetainCount);
    _handleMap->Set(handleNumber, entry);
    _objectMap->Set(handleObject, entry);
    entry->Release();
    handleNumber->Release();
    handleObject->Release();
    // Return new handle
    return entry->GetHandle();
}

static KernelObject_Internal::HandledObject* FindHandledObject(KernelDictionary *map, Handle object)
{
    if (object == 0)
        return NULL;
    KernelNumber *number = new KernelNumber(object);
    KernelObject_Internal::HandledObject *entry = (KernelObject_Internal::HandledObject*)map->ObjectFor(number);
    number->Release();
    return entry;
}

bool ObjectMapper::MapAddRef(Handle object)
{
    KernelObject_Internal::HandledObject *entry = FindHandledObject(_handleMap, object);
    return entry ? entry->UserAddRef() : false;
}

bool ObjectMapper::MapRelease(Handle object)
{
    KernelObject_Internal::HandledObject *entry = FindHandledObject(_handleMap, object);
    return entry ? entry->UserRelease() : false;
}

void ObjectMapper::Unmap(Handle object)
{
    if (object == 0)
        return;
    KernelNumber *number = new KernelNumber(object);
    KernelObject_Internal::HandledObject *entry = (KernelObject_Internal::HandledObject*)_handleMap->ObjectFor(number);
    if (entry) {
        _handleMap->Set(number, NULL);
        KernelNumber *objectNumber = new KernelNumber((UInt32)entry->Object());
        _objectMap->Set(objectNumber, NULL);
        objectNumber->Release();
    }
    number->Release();
}

void ObjectMapper::Unmap(KernelObject *object)
{
    if (object == NULL)
        return;
    KernelNumber *number = new KernelNumber((UInt32)object);
    KernelObject_Internal::HandledObject *entry = (KernelObject_Internal::HandledObject*)_objectMap->ObjectFor(number);
    if (entry) {
        _objectMap->Set(number, NULL);
        KernelNumber *handleNumber = new KernelNumber(entry->GetHandle());
        _handleMap->Set(handleNumber, NULL);
        handleNumber->Release();
    }
    number->Release();
}

KernelObject* ObjectMapper::Find(Handle object)
{
    KernelObject_Internal::HandledObject *entry = FindHandledObject(_handleMap, object);
    return entry ? entry->Object() : NULL;
}
