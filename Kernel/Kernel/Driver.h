#ifndef __DRIVER_H__
#define __DRIVER_H__

#include "KernelObject.h"

class Interrupts;
class Driver;
class KernelDictionary;
class IpcServiceList;
class RunloopThread;
class IpcService;
class IpcEndpoint;
class KernelBufferMemory;

#define kDriver_Property_Name               "name"_ko
#define kDriver_Property_Bus                "bus"_ko

#define kDriver_Bus_System                  "bus.system"_ko
#define kDriver_Bus_PCI                     "bus.pci"_ko

#define kDriver_Property_PCI_ManufacturerID "pci.manufacturer"_ko
#define kDriver_Property_PCI_ProductID      "pci.product"_ko
#define kDriver_Property_PCI_Class          "pci.class"_ko
#define kDriver_Property_PCI_Subclass       "pci.subclass"_ko

//#define kDriver_Processor_Interrupts        KS("x86.interrupt")
//#define kDriver_PCI_Interrupts              KS("pci.interrupt")

// A base driver factory - this will return an array of matches to try
class DriverFactory : public KernelObject
{
public:
    class Match : public KernelObject
    {
    public:
        virtual int MatchValue(void) = 0;
        virtual Driver* Instantiate(void) = 0;
        virtual bool MatchMultiple(void) { return false; }  // Allow drivers to match multiple children simultaneously
    };
    
    virtual KernelArray* MatchForParent(Driver *parent) = 0;
};

// A base driver implementation
class Driver : public KernelObject
{
public:
    static void RegisterFactory(DriverFactory *factory);
    static void UnregisterFactory(DriverFactory *factory);
    
    Driver(const char *name);
    
    virtual bool Start(Driver *parent);   // Trigger matching of child drivers, so only call if you're successful
    virtual void Stop(void);
    
    Driver* Parent(void) { return _parent; }
    Driver* Child(int index);
Interrupts* Test(void){return InterruptSource();}
    
    KernelArray* PropertyList(void);
    KernelObject* PropertyFor(KernelObject *property);

    virtual Interrupts* InterruptSource(void);
protected:
    ~Driver();
    
    virtual bool ConnectChild(Driver *child);
    virtual void DetachChild(Driver *child);
    
    void SetProperty(KernelObject *property, KernelObject *value);
    
private:
    KernelDictionary *_properties;
    Driver *_start, *_end;          // Children
    Driver *_previous, *_next;      // Siblings
    Driver *_parent;                // Parent
};

// A driver that exports an IPC interface
class ProviderDriver : public Driver
{
public:
    ProviderDriver(const char *name);

    class Connection;
    
    /* The services that this provider has detected and is vending */
    class Service : public KernelObject
    {
    public:
        Service(ProviderDriver *owner, IpcService *service);
        
        IpcService* ServiceObject(void) { return _service; }
        
        // If you don't override ConnectionStart/ConnectionReceive/ConnectionStop, these blocks can get called.
        bicycle::function<Connection*(IpcEndpoint *endpoint)> start;
        
    protected:
        ~Service();
        
        ProviderDriver* Owner(void) { return _owner; }
        
    private:
        ProviderDriver *_owner;
        IpcService *_service;
    };
    
    /* Active clients that this provider currently has */
    class Connection : public KernelObject
    {
    public:
        Connection(ProviderDriver *owner, Service *service, IpcEndpoint *connection);

        Service* BaseService(void) { return _service; }
        IpcEndpoint* Link(void) { return _connection; }

        // If you don't override ConnectionStart/ConnectionReceive/ConnectionStop, these blocks can get called.
        bicycle::function<int(KernelBufferMemory*)> message;
        bicycle::function<int(void)> stop;
        
    protected:
        ~Connection();
        
    private:
        ProviderDriver *_owner;
        Service *_service;
        IpcEndpoint *_connection;
    };
    
protected:
    ~ProviderDriver();
    
    void Launch(Service *service);
    void Terminate(Service *service);
    
    virtual Connection* ConnectionStart(Service *service, IpcEndpoint *endpoint);
    virtual void ConnectionReceive(Connection *connection, KernelBufferMemory *message);
    virtual void ConnectionStop(Connection *connection);

    IpcServiceList *_serviceList;
    RunloopThread *_runloop;

    KernelArray *_services;
    KernelArray *_connections;
};

#endif // __DRIVER_H__
