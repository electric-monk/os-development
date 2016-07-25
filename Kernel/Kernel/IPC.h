#ifndef __IPC_H__
#define __IPC_H__

#include "Blocking.h"
#include "mem_virtual.h"

class KernelFIFO;

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
    CLASSNAME(BlockableObject, IpcEndpoint);
    
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

class IpcService;

class IpcClient : public KernelObject
{
public:
    CLASSNAME(KernelObject, IpcClient);
    
    IpcClient(KernelString *name);
    
    KernelString* Name(void) { return _name; }
    
    KernelDictionary* Properties(void) { return _properties; }
    
    virtual void Connect(IpcService *service) = 0;
    virtual void Disconnect(void) = 0;
    
private:
    KernelString *_name;
    KernelDictionary *_properties;
};

class IpcService : public BlockableObject
{
public:
    CLASSNAME(BlockableObject, IpcService);
    
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

#endif // __IPC_H__
