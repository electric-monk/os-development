#include "ConvenientSink.h"
#include "Runloop.h"
#include "IPC_Manager.h"
#include "Thread.h"

namespace ConvenientSink_Internal {
    class Input : public GenericProvider::Input
    {
    public:
        Input(GenericProvider *owner, KernelString *name, bicycle::function<bool(IpcEndpoint*)> connectionHandler)
        :GenericProvider::Input(owner, name)
        {
            _connectionHandler = connectionHandler;
        }
        
        bool ShouldConnect(IpcEndpoint *endpoint)
        {
            return _connectionHandler(endpoint);
        }
        
    private:
        bicycle::function<bool(IpcEndpoint*)> _connectionHandler;
    };
}

ConvenientSink::ConvenientSink(RunloopThread *runloop, InterfaceHelper *helper)
:GenericProvider(runloop)
{
    if (helper) {
        _helper = helper;
        _helper->AddRef();
    } else {
        _helper = new InterfaceHelper();
    }
    _inputs = new KernelDictionary();
}

ConvenientSink::~ConvenientSink()
{
    _inputs->Release();
    _helper->Release();
}

void ConvenientSink::PerformTask(IpcEndpoint *destination, bicycle::function<void(Interface_Request*)> generate, bicycle::function<void(Interface_Response*)> response)
{
    _helper->PerformTask(destination, generate, response);
}

void ConvenientSink::AddTask(bicycle::function<void(void)> task)
{
    _runloop->AddTask(task);
}

// This is all a horrible hack.
static void QueueCheck(Runloop *loop, ConvenientSink_Internal::Input *input, bicycle::function<void(IpcClient*)> completion)
{
    loop->AddDelayedTask(100, [=]{
        if (input->Active())
            completion(input);
        else
            QueueCheck(loop, input, completion);
    });
}

void ConvenientSink::CreateInput(KernelString *name, bicycle::function<bool(IpcEndpoint*)> connectionHandler, bicycle::function<void(IpcClient*)> completion)
{
    ConvenientSink_Internal::Input *input = new ConvenientSink_Internal::Input(this, name, connectionHandler);
    StrongKernelObject<KernelString> strongName(name);
    StrongKernelObject<GenericProvider::Input> strongInput(input);
    _runloop->AddTask([this, strongName, strongInput]{
        _inputs->Set(strongName.Value(), strongInput.Value());
        _serviceList->AddInput(strongInput.Value());
    });
    QueueCheck(_runloop, input, completion);
}

void ConvenientSink::StopInput(IpcClient *input)
{
    StrongKernelObject<IpcClient> strongInput(input);
    _runloop->AddTask([this, strongInput]{
        _serviceList->RemoveInput(strongInput.Value());
        _inputs->Set(strongInput.Value()->Name(), NULL);
    });
    input->Release();
}

GenericProvider::InputConnection* ConvenientSink::InputConnectionStart(KernelString *name, IpcEndpoint *connection)
{
    ConvenientSink_Internal::Input *input = (ConvenientSink_Internal::Input*)_inputs->ObjectFor(name);
    if (input == NULL)
        return NULL;
    if (!input->ShouldConnect(connection))
        return NULL;
    return new GenericProvider::InputConnection(this, name, connection);
}

void ConvenientSink::InputConnectionReceived(InputConnection *connection, KernelBufferMemory *message)
{
    _helper->HandleMessage(message, [=](Interface_Response *response){
        return unhandledMessageHandler(connection->Link(), response);
    });
}

void ConvenientSink::InputConnectionEnd(InputConnection *connection)
{
    _runloop->RemoveSource(connection->Link());
    connection->Release();
}

GenericProvider::OutputConnection* ConvenientSink::OutputConnectionStart(Service *source, IpcEndpoint *connection)
{
    // We're just a sink, no outputs
    return NULL;
}

void ConvenientSink::OutputConnectionMessage(OutputConnection *connection, KernelBufferMemory *message)
{
    // We're just a sink, no outputs
}

void ConvenientSink::OutputConnectionEnd(OutputConnection *oldConnection)
{
    // We're just a sink, no outputs
}
