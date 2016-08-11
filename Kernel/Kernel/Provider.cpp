#include "Provider.h"
#include "Queue.h"
#include "IPC.h"
#include "debug.h"
#include "Interface.h"
#include "IPC_Manager.h"
#include "Runloop.h"

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
    _runloop = new RunloopThread();
    _inputs = new KernelDictionary();
    _services = new KernelArray();
    _connections = new KernelArray();
    _runloop->AddTask([this]{
        _serviceList = new IpcServiceProxy(this);
        return 0;
    });
}

GenericProvider::~GenericProvider()
{
    _serviceList->Release();
    _connections->Release();
    _services->Release();
    _inputs->Release();
    _runloop->Release();
}

void GenericProvider::Launch(Service *service)
{
    _services->Add(service);
    _serviceList->AddOutput(service->ServiceObject());
    service->SetActive(true);
}

void GenericProvider::Kill(Service *service)
{
    service->SetActive(false);
    _serviceList->RemoveOutput(service->ServiceObject());
    _services->Remove(service);
}

GenericProvider::Input::Input(GenericProvider *owner, KernelString *name)
:IpcClient(name)
{
    _owner = owner;
    _owner->AddRef();
    _connection = NULL;
}

GenericProvider::Input::~Input()
{
    kprintf("%.8x input gone\n");
    _owner->Release();
}

void GenericProvider::Input::Connect(IpcService *service)
{
    StrongKernelObject<IpcService> strongService(service);
    _owner->_runloop->AddTask([this, strongService]{
        // Drop old connection
        if (_connection)
            GenericProvider_Thunk::InputDisconnect(_owner, _connection);
        // Initiate connection
        IpcEndpoint *link = CompleteConnect(strongService.Value());
        // Get our new object
        _connection = GenericProvider_Thunk::InputConnect(_owner, Name(), link);
        if (_connection == NULL)
            return 0;
        // Begin monitoring
        StrongKernelObject<GenericProvider::InputConnection> strongConnection(_connection);
        StrongKernelObject<IpcEndpoint> strongEndpoint(_connection->Link());
        _owner->_runloop->AddSource(strongEndpoint.Value(), [this, strongConnection, strongEndpoint, strongService](KernelObject *watching, KernelObject *signals){
            KernelBufferMemory *message = strongEndpoint.Value()->Read(false);
            if (message == NULL) {
                if (_connection == strongConnection.Value())
                    DoDisconnect();
            } else {
                GenericProvider_Thunk::InputMessage(_owner, strongConnection.Value(), message);
            }
            return 0;
        });
        // Add it to the list
        GenericProvider_Thunk::Inputs(_owner)->Set(Name(), _connection);
        _connection->Release();  // Dictionary owns it
        return 0;
    });
}

void GenericProvider::Input::Disconnect(void)
{
    _owner->_runloop->AddTask([=]{
        DoDisconnect();
        return 0;
    });
}

void GenericProvider::Input::DoDisconnect(void)
{
    kprintf("Disconnecting %.8x\n");
    _owner->_runloop->RemoveSource(_connection->Link());
    GenericProvider_Thunk::InputDisconnect(_owner, _connection);
    GenericProvider_Thunk::Inputs(_owner)->Set(_connection->Source(), NULL);
    _connection = NULL;
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

void GenericProvider::Service::SetActive(bool active)
{
    bicycle::function<void*(KernelObject*)> deleteConnection = [this](KernelObject *connection){
        GenericProvider_Thunk::Connections(_owner)->Remove(connection);
        GenericProvider_Thunk::OutputDisconnect(_owner, (GenericProvider::OutputConnection*)connection);
        _owner->_runloop->RemoveSource(((GenericProvider::OutputConnection*)connection)->Link());
        return (void*)NULL;
    };
    if (active) {
        StrongKernelObject<IpcService> strongService(_service);
        _owner->_runloop->AddSource(_service, [this, strongService, deleteConnection](BlockableObject *, KernelArray *){
            IpcEndpoint *link = strongService.Value()->NextConnection(false);
            if (link == NULL)
                return 0;
            // Create the object
            GenericProvider::OutputConnection *connection = GenericProvider_Thunk::OutputConnect(_owner, this, link);
            if (connection == NULL)
                return 0;
            // Set up the monitoring
            StrongKernelObject<GenericProvider::OutputConnection> strongConnnection(connection);
            _owner->_runloop->AddSource(link, [this, link, strongConnnection, deleteConnection](BlockableObject*, KernelArray*){
                KernelBufferMemory *message = strongConnnection.Value()->Link()->Read(false);
                if (message == NULL)
                    deleteConnection(strongConnnection.Value());
                else
                    GenericProvider_Thunk::OutputMessage(_owner, strongConnnection.Value(), message);
                return 0;
            });
            // Add to the connection list
            GenericProvider_Thunk::Connections(_owner)->Add(connection);
            return 0;
        });
    } else {
        KernelArray *toRemove = new KernelArray();
        GenericProvider_Thunk::Connections(_owner)->Enumerate([toRemove, this](KernelObject *object){
            GenericProvider::OutputConnection *connection = (GenericProvider::OutputConnection*)object;
            if (connection->Source() == this)
                toRemove->Add(connection);
            return (void*)NULL;
        });
        toRemove->Enumerate(deleteConnection);
        toRemove->Release();
        _owner->_runloop->RemoveSource(_service);
    }
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
private:
    IpcEndpoint *_connection;
public:
    CLASSNAME(KernelFunction, InterfaceHelper_Handler);
    
    InterfaceHelper_Handler(IpcEndpoint *connection, bicycle::function<int(Interface_Response*)> callback) : KernelFunction<int(Interface_Response*)>(callback)
    {
        _connection = connection;
        _connection->AddRef();
    }
    
protected:
    ~InterfaceHelper_Handler()
    {
        _connection->Release();
    }
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
    InterfaceHelper_Handler *handler = new InterfaceHelper_Handler(destination, response);
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
