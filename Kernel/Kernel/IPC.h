#ifndef __IPC_H__
#define __IPC_H__

#include "Blocking.h"
#include "mem_virtual.h"

class KernelFIFO;
class IpcEndpoint;
class IpcServiceProxy;

class KernelBufferMemory : public KernelObject
{
public:
    CLASSNAME(KernelObject, KernelBufferMemory);
    
    class Map : public VirtualMemory
    {
    public:
        CLASSNAME(VirtualMemory, KernelBufferMemory::Map);
        
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
    
    virtual PhysicalPointer PointerForOffset(UInt64 offset);
    
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
    CLASSNAME(BlockableObject, IpcEndpoint);
    
    IpcEndpoint();
    
    /* Remote end */
    
    void Connect(IpcEndpoint *remote);
    bool IsConnected(void);
    UInt32 UniqueIdentifier(void);
    
    /* Sending */
    
    KernelBufferMemory* CreateSendBuffer(UInt64 size = 65535);
    void SendBuffer(KernelBufferMemory *buffer);
    
    void SendMessage(bicycle::function<bool(void*)> generator, UInt64 maximumSize = 65535);
    
    /* Receiving */
    
    KernelBufferMemory* Read(bool wait);
    // TODO: ReadComplete, for buffer pooling
    
protected:
    ~IpcEndpoint();
    
private:
    IpcEndpoint *_remote;
    KernelFIFO *_fifo;
    UInt64 _fifoCount;
};

class IpcService;

class IpcClient : public KernelObject
{
public:
    CLASSNAME(KernelObject, IpcClient);
    
    IpcClient(KernelString *name);
    
    KernelString* Name(void) { return _name; }
    
    KernelDictionary* Properties(void) { return _properties; }

    void Start(IpcServiceProxy *proxy);
    void Stop(void);
    
    bool Active(void) { return _proxy != NULL; }
    
    virtual void Connect(IpcService *service) = 0;
    virtual void Disconnect(void) = 0;
    
protected:
    ~IpcClient();
    
    IpcEndpoint* CompleteConnect(IpcService *service);
    
private:
    KernelString *_name;
    KernelDictionary *_properties;
    IpcServiceProxy *_proxy;
};

class IpcClientBlock : public IpcClient
{
public:
    CLASSNAME(IpcClient, IpcClientBlock);
    
    IpcClientBlock(KernelString *name, bicycle::function<int(IpcService*, bicycle::function<IpcEndpoint*(void)>)> onConnect, bicycle::function<int(void)> onDisconnect)
    :IpcClient(name)
    {
        _connect = onConnect;
        _disconnect = onDisconnect;
    }
    
    void Connect(IpcService *service)
    {
        _connect(service, [=]{
            return CompleteConnect(service);
        });
    }
    
    void Disconnect(void)
    {
        _disconnect();
    }
    
private:
    bicycle::function<int(IpcService*, bicycle::function<IpcEndpoint*(void)>)> _connect;
    bicycle::function<int(void)> _disconnect;
};

class IpcService : public BlockableObject
{
public:
    CLASSNAME(BlockableObject, IpcService);
    
    IpcService(KernelString *name, KernelString *service);

    KernelString* Name(void) { return _name; }
    KernelString* ServiceType(void) { return _type; }
    
    KernelDictionary* Properties(void) { return _properties; }  // TODO: Should this work another way?
    
    // Get the most recent incoming connection (server end)
    IpcEndpoint* NextConnection(bool wait);
    
    void Start(IpcServiceProxy *proxy);
    void Stop(void);
    
protected:
    ~IpcService();
    
private:
    friend IpcClient;
    // Create an incoming connection (client end)
    IpcEndpoint* RequestConnection(void);
    IpcServiceProxy *_proxy;
    
private:
    KernelDictionary *_properties;
    KernelString *_name, *_type;
    KernelFIFO *_fifo;
    UInt64 _fifoCount;
};

#endif // __IPC_H__
