#include "IPC.h"
#include "mem_virtual.h"
#include "mem_logical.h"
#include "Process.h"
#include "Thread.h"
#include "debug.h"
#include "IPC_Manager.h"
#include "CPU_intr.h"   // temp

KernelBufferMemory::KernelBufferMemory(UInt64 maximumSize)
{
    _pageCount = (maximumSize + PAGE_SIZE - 1) / PAGE_SIZE;
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
    UInt64 index = offset / PAGE_SIZE;
    if (_pages[index] == CPhysicalMemory::Invalid) {
        _pagesUsed++;
        _pages[index] = CPhysicalMemory::AllocateContiguousPages(1);
    }
    return _pages[index];
}

UInt64 KernelBufferMemory::MaximumSize(void)
{
    return _pageCount * PAGE_SIZE;
}
UInt64 KernelBufferMemory::Size(void)
{
    return _pagesUsed * PAGE_SIZE;
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
    InterruptDisabler temp; // TODO: clever lock
    if (_remote) {
        IpcEndpoint *remote = _remote;
        _remote = NULL;
        remote->_remote = NULL;
        remote->AddRef();
        remote->SetSignalled(remote, true);
        remote->Release();
    }
    _fifo->Release();
}

void IpcEndpoint::Connect(IpcEndpoint *remote)
{
    InterruptDisabler temp; // TODO: clever lock
    ASSERT(_remote == NULL);
    ASSERT(remote->_remote == NULL);
    _remote = remote;
    remote->_remote = this;
}

bool IpcEndpoint::IsConnected(void)
{
    return _remote != NULL;
}

UInt32 IpcEndpoint::UniqueIdentifier(void)
{
    return UInt32(this) ^ UInt32(_remote);
}

KernelBufferMemory* IpcEndpoint::CreateSendBuffer(UInt64 size)
{
    return new KernelBufferMemory(size);
}

void IpcEndpoint::SendBuffer(KernelBufferMemory *buffer)
{
    InterruptDisabler temp; // TODO: clever lock
    bool unSignalled = _remote->_fifoCount == 0;
    _remote->_fifo->Push(buffer);
    _remote->_fifoCount++;
    if (unSignalled)
        _remote->SetSignalled(_remote, true);
}

void IpcEndpoint::SendMessage(bicycle::function<bool(void*)> generator, UInt64 maximumSize)
{
    KernelBufferMemory *buffer = CreateSendBuffer(maximumSize);
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
        SetSignalled(this, false);
    return result;
}

IpcClient::IpcClient(KernelString *name)
{
    _name = name;
    _name->AddRef();
    _properties = new KernelDictionary();
    _proxy = NULL;
}

IpcClient::~IpcClient()
{
    _name->Release();
    _properties->Release();
}

void IpcClient::Start(IpcServiceProxy *proxy)
{
    _proxy = proxy;
}

void IpcClient::Stop(void)
{
    _proxy = NULL;
}

IpcEndpoint* IpcClient::CompleteConnect(IpcService *service)
{
    IpcEndpoint *endpoint = service->RequestConnection();
    _proxy->StartInput(this, endpoint);
    return endpoint;
}

IpcService::IpcService(KernelString *name, KernelString *service)
{
    _name = name;
    _name->AddRef();
    _type = service;
    _type->AddRef();
    _fifo = new KernelFIFO();
    _fifoCount = 0;
    _properties = new KernelDictionary();
    _proxy = NULL;
}

IpcService::~IpcService()
{
    _name->Release();
    _type->Release();
    _fifo->Release();
    _properties->Release();
}

void IpcService::Start(IpcServiceProxy *proxy)
{
    _proxy = proxy;
}

void IpcService::Stop(void)
{
    _proxy = NULL;
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
        SetSignalled(this, true);
    
    local->Release();
    remote->Autorelease();
    _proxy->StartOutput(this, local);
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
        SetSignalled(this, false);
    return local;
}
