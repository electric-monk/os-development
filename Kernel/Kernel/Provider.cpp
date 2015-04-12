#include "Provider.h"
#include "Queue.h"
#include "IPC.h"

class GenericProvider_Thunk
{
public:
    static GenericProvider::InputConnection* InputConnect(GenericProvider *provider, KernelString *name, IpcEndpoint *connection)
    {
        return provider->InputConnectionStart(name, connection);
    }
    static void InputMessage(GenericProvider *provider, GenericProvider::InputConnection *connection, KernelBufferMemory *message)
    {
        provider->InputConnectionReceived(connection, message);
    }
    static void InputDisconnect(GenericProvider *provider, GenericProvider::InputConnection *connection)
    {
        provider->InputConnectionEnd(connection);
    }
};

DispatchQueue *_queue;

// Useful information
KernelDictionary *_inputs;
KernelArray *_services;
KernelArray *_connections;

GenericProvider::GenericProvider()
{
    _queue = new DispatchQueue();
    _inputs = new KernelDictionary();
    _services = new KernelArray();
    _connections = new KernelArray();
    _serviceList = new IpcServiceList(this);
}

GenericProvider::~GenericProvider()
{
    _serviceList->Release();
    _connections->Release();
    _services->Release();
    _inputs->Release();
    _queue->Release();
}

void GenericProvider::Launch(Service *service)
{
    _services->Add(service);
    _serviceList->AddService(service->ServiceObject());
}

void GenericProvider::Kill(Service *service)
{
    _serviceList->RemoveService(service->ServiceObject());
    _services->Remove(service);
}

class GenericProvider_Input_Connect : public DispatchQueue::Task
{
public:
    class DisconnectionHandler : public DispatchQueue::Task
    {
    public:
        DisconnectionHandler(GenericProvider *owner, GenericProvider::InputConnection *connection)
        {
            _owner = owner;
            _connection = connection;
        }
        
    protected:
        void Execute(void)
        {
            GenericProvider_Thunk::InputDisconnect(_owner, _connection);
        }
        
    private:
        GenericProvider *_owner; // weak
        GenericProvider::InputConnection *_connection; // weak
    };
private:
    class MessageHandler : public DispatchQueue::Task
    {
    public:
        MessageHandler(GenericProvider *owner, GenericProvider::InputConnection *connection, KernelBufferMemory *message)
        {
            _provider = owner;
            _connection = connection;
            _message = message;
            _message->AddRef();
        }
        
    protected:
        ~MessageHandler()
        {
            _message->Release();
        }
        
        void Execute(void)
        {
            GenericProvider_Thunk::InputMessage(_provider, _connection, _message);
        }
        
    private:
        GenericProvider *_provider; // weak
        GenericProvider::InputConnection *_connection; // weak
        KernelBufferMemory *_message;
    };
    class MessageWatcher : public SignalWatcher
    {
    public:
        MessageWatcher(GenericProvider *owner, GenericProvider::InputConnection *connection)
        {
            _provider = owner;
            _connection = connection;
        }
        
    protected:
        void SignalChanged(BlockableObject *source)
        {
            // TODO: Some runloop thing
            while (source && source->Signalled()) {
                KernelBufferMemory *message = _connection->Link()->Read(false);
                DispatchQueue::Task *task;
                if (message == NULL)
                    task = new DisconnectionHandler(_provider, _connection);
                else
                    task = new MessageHandler(_provider, _connection, message);
                _provider->_queue->AddTask(task);
                task->Release();
            }
        }
        
    private:
        GenericProvider *_provider; // weak
        GenericProvider::InputConnection *_connection; // weak
    };
    
public:
    GenericProvider_Input_Connect(GenericProvider *provider, KernelString *name, IpcService *service)
    {
        _provider = provider;
        _provider->AddRef();
        _name = name;
        _name->AddRef();
        _service = service;
        _service->AddRef();
    }
    
protected:
    ~GenericProvider_Input_Connect()
    {
        _service->Release();
        _name->Release();
        _provider->Release();
    }
    
    void Execute(void)
    {
        // Initiate connection
        IpcEndpoint *link = _service->RequestConnection();
        if (link == NULL)
            return;
        // Get our new object
        GenericProvider::InputConnection *connection = GenericProvider_Thunk::InputConnect(_provider, _name, link);
        if (connection == NULL) {
            link->Release();    // If the above failed, assume it didn't take ownership
            return;
        }
        // Begin monitoring
        MessageWatcher *observer = new MessageWatcher(_provider, connection);
        connection->Link()->RegisterObserver(observer);
        observer->Release();
        // Add it to the list
        _provider->_inputs->Set(_name, connection);
    }
    
private:
    GenericProvider *_provider;
    KernelString *_name;
    IpcService *_service;
};

void GenericProvider::ConnectInput(KernelString *inputName, IpcService *service)
{
    GenericProvider_Input_Connect *task = new GenericProvider_Input_Connect(this, inputName, service);
    _queue->AddTask(task);
    task->Release();
}

void GenericProvider::DisconnectInput(KernelString *inputName)
{
    InputConnection *connection = (InputConnection*)_inputs->ObjectFor(inputName);
    if (connection == NULL)
        return;
    GenericProvider_Input_Connect::DisconnectionHandler *task = new GenericProvider_Input_Connect::DisconnectionHandler(this, connection);
    _queue->AddTask(task);
    task->Release();
}

GenericProvider::Service::Service(GenericProvider *owner, IpcService *service)
{
    _owner = owner;
    _owner->AddRef();
    _service = service;
    _service->AddRef();
}

GenericProvider::Service::~Service()
{
    _service->Release();
    _owner->Release();
}

GenericProvider::Connection::Connection(GenericProvider *owner, IpcEndpoint *connection)
{
    _owner = owner;
    _owner->AddRef();
    _connection = connection;
    _connection->AddRef();
}

GenericProvider::Connection::~Connection()
{
    _connection->Release();
    _owner->Release();
}

GenericProvider::InputConnection::InputConnection(GenericProvider *owner, KernelString *input, IpcEndpoint *connection)
:Connection(owner, connection)
{
    _source = input;
    _source->AddRef();
}

GenericProvider::InputConnection::~InputConnection()
{
    _source->Release();
}

GenericProvider::OutputConnection::OutputConnection(GenericProvider *owner, Service *source, IpcEndpoint *connection)
:Connection(owner, connection)
{
    _source = source;
    _source->AddRef();
}

GenericProvider::OutputConnection::~OutputConnection()
{
    _source->Release();
}

GenericProvider::Stash::Stash()
{
    
}

GenericProvider::Stash::~Stash()
{
    
}

void GenericProvider::Stash::Register(Factory *factory)
{
    
}

void GenericProvider::Stash::Unregister(Factory *factory)
{
    
}
