#ifndef __PROVIDER_H__
#define __PROVIDER_H__

#include "Collections.h"

class GenericProvider_Thunk;
class IpcService;
class IpcEndpoint;
class KernelBufferMemory;
class DispatchQueue;
class IpcServiceList;
class Interface_Request;
class Interface_Response;

/* A generic class which can connect to some sort of service, and provide new services based upon it */
class GenericProvider : public KernelObject
{
public:
    GenericProvider();

    // Hooking up inputs
    virtual UInt32 InputCount(void) = 0;
    virtual KernelDictionary* Input(UInt32 index) = 0;
    void ConnectInput(KernelString *inputName, IpcService *service);
    void DisconnectInput(KernelString *inputName);
    
//protected:
    /* The factory object that creates instances of this provider */
    class Factory : public KernelObject
    {
    public:
        // Information about this provider
        virtual KernelDictionary* Information(void) = 0;
        virtual UInt32 ExpectedInput(void) = 0;
        virtual KernelDictionary* ExpectedInputInfo(UInt32 index) = 0;
        virtual UInt32 ExpectedOutput(void) = 0;
        virtual KernelDictionary* ExpectedOutputInfo(UInt32 index) = 0;
        
        virtual GenericProvider* Create(void) = 0;
    };
    
    /* The services that this provider has detected and is vending */
    class Service : public KernelObject
    {
    public:
        Service(GenericProvider *owner, IpcService *service);
        
        IpcService* ServiceObject(void) { return _service; }
        
    protected:
        ~Service();
        
        GenericProvider* Owner(void) { return _owner; }
        
    private:
        GenericProvider *_owner;
        IpcService *_service;
    };
    
    /* Active clients that this provider currently has */
    class Connection : public KernelObject
    {
    public:
        IpcEndpoint* Link(void) { return _connection; }
        
    protected:
        Connection(GenericProvider *owner, IpcEndpoint *connection);
        ~Connection();
        
    private:
        GenericProvider *_owner;
        IpcEndpoint *_connection;
    };
    
    /* Active inputs */
    class InputConnection : public Connection
    {
    public:
        InputConnection(GenericProvider *owner, KernelString *input, IpcEndpoint *connection);
        
        KernelString* Source(void) { return _source; }
        
    protected:
        ~InputConnection();
        
    private:
        KernelString *_source;
    };
    
    /* Active outputs */
    class OutputConnection : public Connection
    {
    public:
        OutputConnection(GenericProvider *owner, Service *source, IpcEndpoint *connection);

        Service* Source(void) { return _source; }

    protected:
        ~OutputConnection();
        
    private:
        Service *_source;
    };
    
protected:
    friend GenericProvider_Thunk;
    
    ~GenericProvider();
    
    // Attaching new outputs
    void Launch(Service *service);
    void Kill(Service *service);
    
    // Input event handlers
    virtual InputConnection* InputConnectionStart(KernelString *name, IpcEndpoint *connection) = 0;
    virtual void InputConnectionReceived(InputConnection *connection, KernelBufferMemory *message) = 0;
    virtual void InputConnectionEnd(InputConnection *connection) = 0;
    
    // Output event handlers
    virtual OutputConnection* OutputConnectionStart(Service *source, IpcEndpoint *connection) = 0;
    virtual void OutputConnectionMessage(OutputConnection *connection, KernelBufferMemory *message) = 0;
    virtual void OutputConnectionEnd(OutputConnection *oldConnection) = 0;
    
    // The queue to handle events on
    DispatchQueue *_queue;
    
    // Useful information
    KernelDictionary *_inputs;
    KernelArray *_services;
    KernelArray *_connections;
    
    IpcServiceList *_serviceList;
    
public:
    class Stash
    {
    public:
        Stash();
        ~Stash();
        
        void Register(Factory *factory);
        void Unregister(Factory *factory);
    private:
        KernelArray *_factories;
    };
};

/* A class to help with interfaces */
class InterfaceHelper : public KernelObject
{
public:
    InterfaceHelper();
    
    void PerformTask(IpcEndpoint *destination, bicycle::function<int(Interface_Request*)> generate, bicycle::function<int(Interface_Response*)> response);
    void HandleMessage(KernelBufferMemory *responseMemory, bicycle::function<int(Interface_Response*)> onUnhandled);

protected:
    ~InterfaceHelper();
    
private:
    UInt64 _identifier;
    KernelDictionary *_tasks;
};

#endif // __PROVIDER_H__
