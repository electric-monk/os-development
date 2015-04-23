#ifndef __IPC_H__
#define __IPC_H__

#include "Blocking.h"
#include "mem_virtual.h"

class KernelFIFO;

class KernelBufferMemory : public KernelObject
{
public:
    class Map : public VirtualMemory
    {
    public:
        Map(Process *process, KernelBufferMemory *memory, bool readonly);
        
    protected:
        ~Map();
        
        void HandlePageFault(void *linearAddress);
        
    private:
        KernelBufferMemory *_memory;
        bool _readonly;
    };
    
public:
    KernelBufferMemory(UInt64 maximumSize);
    
    PhysicalPointer PointerForOffset(UInt64 offset);
    
    UInt64 MaximumSize(void);
    UInt64 Size(void);
    
    void PerformOnBuffer(bool readonly, bicycle::function<int(void*)> operations);
    
protected:
    ~KernelBufferMemory();
    
private:
    UInt64 _pageCount, _pagesUsed;
    PhysicalPointer *_pages;
};

class IpcEndpoint : public BlockableObject
{
public:
    IpcEndpoint();
    
    /* Remote end */
    
    void Connect(IpcEndpoint *remote);
    bool IsConnected(void);
    
    /* Sending */
    
    KernelBufferMemory* CreateSendBuffer(UInt64 size = 65535);
    void SendBuffer(KernelBufferMemory *buffer);
    
    void SendMessage(bicycle::function<bool(void*)> generator, UInt64 maximumSize = 65535);
    
    /* Receiving */
    
    KernelBufferMemory* Read(bool wait);
    
protected:
    ~IpcEndpoint();
    
private:
    IpcEndpoint *_remote;
    KernelFIFO *_fifo;
    UInt64 _fifoCount;
};

class IpcService : public BlockableObject
{
public:
    IpcService(KernelString *name, KernelString *service);

    KernelString* Name(void) { return _name; }
    KernelString* ServiceType(void) { return _type; }
    
    // Create an incoming connection (client end)
    IpcEndpoint* RequestConnection(void);

    // Get the most recent incoming connection (server end)
    IpcEndpoint* NextConnection(bool wait);
    
protected:
    ~IpcService();
    
private:
    KernelString *_name, *_type;
    KernelFIFO *_fifo;
    UInt64 _fifoCount;
};

class IpcServiceWatcher : public KernelObject
{
public:
    virtual void ServiceProviderAppeared(KernelObject *provider) = 0;
    virtual void ServiceAppeared(KernelObject *provider, IpcService *service) = 0;
    virtual void ServiceRemoved(KernelObject *provider, IpcService *service) = 0;
    virtual void ServiceProviderRemoved(KernelObject *provider) = 0;
};

class IpcServiceList : public KernelObject
{
public:
    static void Register(IpcServiceWatcher *watcher);
    static void Unregister(IpcServiceWatcher *watcher);
    
    IpcServiceList(KernelObject *owner);
    
    void RemoveServiceProvider(KernelObject *owner);
    void AddService(IpcService *service);
    void RemoveService(IpcService *service);
    
    KernelArray* ServiceList(void); // Array of IpcService objects
    IpcService* ServiceNamed(KernelString *name);
    
protected:
    ~IpcServiceList();
    
private:
    KernelObject *_owner;
    KernelDictionary *_services;
};

#endif // __IPC_H__
