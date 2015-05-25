#include "Provider.h"
#include "Queue.h"
#include "IPC.h"
#include "debug.h"
#include "Interface.h"

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
    static GenericProvider::OutputConnection* OutputConnect(GenericProvider *provider, GenericProvider::Service *source, IpcEndpoint *connection)
    {
        return provider->OutputConnectionStart(source, connection);
    }
    static void OutputMessage(GenericProvider *provider, GenericProvider::OutputConnection *connection, KernelBufferMemory *message)
    {
        provider->OutputConnectionMessage(connection, message);
    }
    static void OutputDisconnect(GenericProvider *provider, GenericProvider::OutputConnection *connection)
    {
        provider->OutputConnectionEnd(connection);
    }
    static void AddTask(GenericProvider *provider, DispatchQueue::Task *task)
    {
        provider->_queue->AddTask(task);
    }
    static KernelDictionary* Inputs(GenericProvider *provider)
    {
        return provider->_inputs;
    }
    static KernelArray* Connections(GenericProvider *provider)
    {
        return provider->_connections;
    }
};

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

class GenericProvider_Output_Watch : public SignalWatcher
{
private:
    class MessageHandler : public DispatchQueue::Task
    {
    public:
        MessageHandler(GenericProvider *provider, GenericProvider::OutputConnection *connection, KernelBufferMemory *message)
        {
            _provider = provider;
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
            GenericProvider_Thunk::OutputMessage(_provider, _connection, _message);
        }
        
    private:
        GenericProvider *_provider;
        GenericProvider::OutputConnection *_connection;
        KernelBufferMemory *_message;
    };
    class DisconnectionHandler : public DispatchQueue::Task
    {
    public:
        DisconnectionHandler(GenericProvider *provider, GenericProvider::OutputConnection *connection)
        {
            _provider = provider;
            _connection = connection;
        }
        
    protected:
        void Execute(void)
        {
            GenericProvider_Thunk::OutputDisconnect(_provider, _connection);
        }
        
    private:
        GenericProvider *_provider;
        GenericProvider::OutputConnection *_connection;
    };
public:
    GenericProvider_Output_Watch(GenericProvider *provider, GenericProvider::Service *service)
    {
        _provider = provider;
        _service = service;
    }
    
protected:
    class Message : public SignalWatcher
    {
    public:
        Message(GenericProvider *provider, GenericProvider::OutputConnection *connection)
        {
            _provider = provider;
            _connection = connection;
        }
        
    protected:
        void SignalChanged(BlockableObject *source) // On message
        {
            if (source) {
//            while (source && source->Signalled()) { // TODO: Some runloop thing
                KernelBufferMemory *message = _connection->Link()->Read(false);
                DispatchQueue::Task *task;
                if (message == NULL)
                    task = new DisconnectionHandler(_provider, _connection);
                else
                    task = new MessageHandler(_provider, _connection, message);
                GenericProvider_Thunk::AddTask(_provider, task);
                task->Release();
            }
        }
        
    private:
        GenericProvider *_provider;
        GenericProvider::OutputConnection *_connection;
    };
    
    void SignalChanged(BlockableObject *source) // On output connection
    {
        if (source) {
//            while (source && source->Signalled()) { // TODO: Some runloop thing
            // Get the new connection
            IpcEndpoint *link = _service->ServiceObject()->NextConnection(false);
            if (link == NULL)
                return;
            // Create the object
            GenericProvider::OutputConnection *connection = GenericProvider_Thunk::OutputConnect(_provider, _service, link);
            if (connection == NULL)
                return;
            // Set up the monitoring
            Message *observer = new Message(_provider, connection);
            link->RegisterObserver(observer);
            observer->Release();
            // Add to the connection list
            GenericProvider_Thunk::Connections(_provider)->Add(connection);
            kprintf("Got connection %.8x\n", connection);
        }
    }
private:
    GenericProvider *_provider;
    GenericProvider::Service *_service;
};

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
        void SignalChanged(BlockableObject *source) // On input message
        {
            if (source) {
//            while (source && source->Signalled()) { // TODO: Some runloop thing
                KernelBufferMemory *message = _connection->Link()->Read(false);
                DispatchQueue::Task *task;
                if (message == NULL)
                    task = new DisconnectionHandler(_provider, _connection);
                else
                    task = new MessageHandler(_provider, _connection, message);
                GenericProvider_Thunk::AddTask(_provider, task);
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
        if (connection == NULL)
            return;
        // Begin monitoring
        MessageWatcher *observer = new MessageWatcher(_provider, connection);
        connection->Link()->RegisterObserver(observer);
        observer->Release();
        // Add it to the list
        GenericProvider_Thunk::Inputs(_provider)->Set(_name, connection);
        connection->Release();  // Dictionary owns it
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
    
    GenericProvider_Output_Watch *watcher = new GenericProvider_Output_Watch(owner, this);
    _service->RegisterObserver(watcher);
    watcher->Release();
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

class InterfaceHelper_Handler : public KernelFunction<int(Interface_Response*)>
{
public:
    InterfaceHelper_Handler(bicycle::function<int(Interface_Response*)> callback) : KernelFunction<int(Interface_Response*)>(callback) {}
};

InterfaceHelper::InterfaceHelper()
{
    _identifier = 0;
    _tasks = new KernelDictionary();
}

InterfaceHelper::~InterfaceHelper()
{
    _tasks->Release();
}

void InterfaceHelper::PerformTask(IpcEndpoint *destination, bicycle::function<int(Interface_Request*)> generate, bicycle::function<int(Interface_Response*)> response)
{
    // Select identifier
    KernelNumber *number = new KernelNumber(_identifier++);
    while (_tasks->ObjectFor(number) != NULL)
        number->Reset(_identifier++);
    UInt32 identifier = number->Value();
    // Save handler
    InterfaceHelper_Handler *handler = new InterfaceHelper_Handler(response);
    _tasks->Set(number, handler);
    handler->Release();
    number->Release();
    // Generate request
    KernelBufferMemory *request = destination->CreateSendBuffer();
    KernelBufferMemory::Map *mapping = new KernelBufferMemory::Map(NULL, request, false);
    Interface_Request *read = (Interface_Request*)mapping->LinearBase();
    generate(read);
    read->identifier = identifier;
    mapping->Release();
    // Send it
    destination->SendBuffer(request);
    request->Release();
}

void InterfaceHelper::HandleMessage(KernelBufferMemory *responseMemory, bicycle::function<int(Interface_Response*)> onUnhandled)
{
    // Map it in
    KernelBufferMemory::Map *mapping = new KernelBufferMemory::Map(NULL, responseMemory, true);
    Interface_Response *response = (Interface_Response*)mapping->LinearBase();
    // Get handler ID
    KernelNumber *number = new KernelNumber(response->identifier);
    // Find task
    InterfaceHelper_Handler *task = (InterfaceHelper_Handler*)_tasks->ObjectFor(number);
    if (task == NULL) {
        onUnhandled(response);
    } else {
        task->Pointer()(response);
        _tasks->Set(number, NULL);
    }
    // Done
    number->Release();
    mapping->Release();
}
