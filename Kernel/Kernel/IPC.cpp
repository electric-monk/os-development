#include "IPC.h"
#include "mem_virtual.h"
#include "mem_logical.h"
#include "Process.h"
#include "Thread.h"
#include "debug.h"

#define PAGESIZE        4096

KernelBufferMemory::KernelBufferMemory(UInt64 maximumSize)
{
    _pageCount = (maximumSize + PAGESIZE - 1) / PAGESIZE;
    _pages = new PhysicalPointer[_pageCount];
    for (UInt64 i = 0; i < _pageCount; i++)
        _pages[i] = CPhysicalMemory::Invalid;
    _pagesUsed = 0;
}

KernelBufferMemory::~KernelBufferMemory()
{
    for (UInt64 i = 0; i < _pageCount; i++) {
        if (_pages[i] != CPhysicalMemory::Invalid)
            CPhysicalMemory::ReleasePages(_pages[i], 1);
    }
    delete[] _pages;
}

PhysicalPointer KernelBufferMemory::PointerForOffset(UInt64 offset)
{
    UInt64 index = offset / PAGESIZE;
    if (_pages[index] == CPhysicalMemory::Invalid) {
        _pagesUsed++;
        _pages[index] = CPhysicalMemory::AllocateContiguousPages(1);
    }
    return _pages[index];
}

UInt64 KernelBufferMemory::MaximumSize(void)
{
    return _pageCount * PAGESIZE;
}
UInt64 KernelBufferMemory::Size(void)
{
    return _pagesUsed * PAGESIZE;
}

void KernelBufferMemory::PerformOnBuffer(bool readonly, bicycle::function<int(void*)> operations)
{
    Map *mapping = new Map(NULL, this, readonly);
    operations(mapping->LinearBase());
    mapping->Release();
}

KernelBufferMemory::Map::Map(Process *process, KernelBufferMemory *memory, bool readonly)
:VirtualMemory(process, memory->MaximumSize())
{
    _memory = memory;
    _memory->AddRef();
    _readonly = readonly;
}

KernelBufferMemory::Map::~Map()
{
    _memory->Release();
}
    
void KernelBufferMemory::Map::HandlePageFault(void *linearAddress)
{
    if (IsMapped(linearAddress)) {
        // TODO: Probably an attempt to write a read-only area! Exception!
    }
    VirtualMemory::Map(_readonly ? 0 : fmWritable, linearAddress, _memory->PointerForOffset(UInt64(linearAddress) - UInt64(LinearBase())));
}

IpcEndpoint::IpcEndpoint()
{
    _remote = NULL;
    _fifo = new KernelFIFO();
    _fifoCount = 0;
}

IpcEndpoint::~IpcEndpoint()
{
    if (_remote) {
        _remote->_remote = NULL;
        _remote->SignalFor(_remote);
    }
    _fifo->Release();
}

void IpcEndpoint::Connect(IpcEndpoint *remote)
{
    ASSERT(_remote == NULL);
    ASSERT(remote->_remote == NULL);
    _remote = remote;
    remote->_remote = this;
}

bool IpcEndpoint::IsConnected(void)
{
    return _remote != NULL;
}

KernelBufferMemory* IpcEndpoint::CreateSendBuffer(UInt64 size)
{
    return new KernelBufferMemory(size);
}

void IpcEndpoint::SendBuffer(KernelBufferMemory *buffer)
{
    bool unSignalled = _remote->_fifoCount == 0;
    _remote->_fifo->Push(buffer);
    _remote->_fifoCount++;
    if (unSignalled)
        _remote->SignalFor(this);
}

void IpcEndpoint::SendMessage(bicycle::function<bool(void*)> generator, UInt64 maximumSize)
{
    KernelBufferMemory *buffer = new KernelBufferMemory(maximumSize);
    KernelBufferMemory::Map *mapping = new KernelBufferMemory::Map(NULL, buffer, false);
    bool send = generator(mapping->LinearBase());
    mapping->Release();
    if (send)
        SendBuffer(buffer);
    buffer->Release();
}

KernelBufferMemory* IpcEndpoint::Read(bool wait)
{
    if (Thread::Active && !_fifoCount && wait) {
        if (_remote == NULL)
            return NULL;
        if (!Thread::Active->BlockOn(this))
            return NULL;
    }
    KernelBufferMemory *result = (KernelBufferMemory*)_fifo->Pop();
    if (result == NULL)
        return NULL;
    _fifoCount--;
    if ((_fifoCount == 0) && (_remote != NULL))
        SignalFor(NULL);
    return result;
}

IpcService::IpcService(KernelString *name, KernelString *service)
{
    _name = name;
    _name->AddRef();
    _type = service;
    _type->AddRef();
    _fifo = new KernelFIFO();
    _fifoCount = 0;
}

IpcService::~IpcService()
{
    _name->Release();
    _type->Release();
    _fifo->Release();
}

IpcEndpoint* IpcService::RequestConnection(void)
{
    IpcEndpoint *local = new IpcEndpoint();
    IpcEndpoint *remote = new IpcEndpoint();
    local->Connect(remote);
    
    bool unSignalled = _fifoCount == 0;
    _fifo->Push(local);
    _fifoCount++;
    if (unSignalled)
        SignalFor(this);
    
    local->Release();
    remote->Autorelease();
    return remote;
}

IpcEndpoint* IpcService::NextConnection(bool wait)
{
    if (Thread::Active && !_fifoCount && wait) {
        if (!Thread::Active->BlockOn(this))
            return NULL;
    }
    IpcEndpoint *local = (IpcEndpoint*)_fifo->Pop();
    if (local == NULL)
        return NULL;
    _fifoCount--;
    if (_fifoCount == 0)
        SignalFor(NULL);
    return local;
}

// TODO: on a queue?
static KernelArray *s_serviceWatchers = NULL;

void IpcServiceList::Register(IpcServiceWatcher *watcher)
{
    if (s_serviceWatchers == NULL)
        s_serviceWatchers = new KernelArray();
    s_serviceWatchers->Add(watcher);
}

void IpcServiceList::Unregister(IpcServiceWatcher *watcher)
{
    if (s_serviceWatchers == NULL)
        return;
    s_serviceWatchers->Remove(watcher);
}

static void NotifyWatchers(bool added, KernelObject *owner, IpcService *service)
{
    int count = s_serviceWatchers->Count();
    for (int i = 0; i < count; i++) {
        IpcServiceWatcher *watcher = (IpcServiceWatcher*)s_serviceWatchers->ObjectAt(i);
        if (service) {
            if (added)
                watcher->ServiceAppeared(owner, service);
            else
                watcher->ServiceRemoved(owner, service);
        } else {
            if(added)
                watcher->ServiceProviderAppeared(owner);
            else
                watcher->ServiceProviderRemoved(owner);
        }
    }
}

IpcServiceList::IpcServiceList(KernelObject *owner)
{
    _owner = owner;
    _services = new KernelDictionary();
    NotifyWatchers(true, _owner, NULL);
}

IpcServiceList::~IpcServiceList()
{
    NotifyWatchers(false, _owner, NULL);
    _services->Release();
}

void IpcServiceList::AddService(IpcService *service)
{
    // TODO: locking
    _services->Set(service->Name(), service);
    NotifyWatchers(true, _owner, service);
}

void IpcServiceList::RemoveService(IpcService *service)
{
    // TODO: locking
    NotifyWatchers(false, _owner, service);
    _services->Set(service->Name(), NULL);
}

KernelArray* IpcServiceList::ServiceList(void)
{
    return _services->AllObjects();
}

IpcService* IpcServiceList::ServiceNamed(KernelString *name)
{
    return (IpcService*)_services->ObjectFor(name);
}
