#include "IPC_Service.h"
#include "Interrupts.h"
#include "IPC.h"
#include "Process.h"
#include "Provider.h"
#include "IPC_Manager.h"

namespace IPC_Service {
    
    class ActualUserspaceProvider;

    class UserspaceProvider : public BlockableObject
    {
    private:
        ActualUserspaceProvider *_owner;
        KernelFIFO *_events;
        UInt64 _count;
        
    public:
        CLASSNAME(BlockableObject, IPC_Service::UserspaceProvider);
        
        class Data : public KernelObject
        {
        public:
            CLASSNAME(KernelObject, IPC_Service::UserspaceProvider::Data);
            
            Data(int type, IpcEndpoint *connection, KernelBufferMemory *message, KernelObject *owner)
            {
                _type = type;
                _endpoint = connection;
                _endpoint->AddRef();
                _memory = message;
                if (_memory)
                    _memory->AddRef();
                _source = owner;
                if (_source)
                    _source->AddRef();
            }
            
            void Save(UInt64 *output)
            {
                output[0] = _type;
                if (_endpoint)
                    _endpoint->AddRef();
                output[1] = Process::Mapper()->Map(_endpoint);
                if (_memory)
                    _memory->AddRef();
                output[2] = Process::Mapper()->Map(_memory);
                if (_source)
                    _source->AddRef();
                output[3] = Process::Mapper()->Map(_source);
            }
            
        protected:
            ~Data()
            {
                _endpoint->Release();
                if (_memory)
                    _memory->Release();
                if (_source)
                    _source->Release();
            }
            
        private:
            int _type;
            IpcEndpoint *_endpoint;
            KernelBufferMemory *_memory;
            KernelObject *_source;
        };
        
        UserspaceProvider(ActualUserspaceProvider *owner)
        {
            _owner = owner;
            _events = new KernelFIFO();
            _count = 0;
        }
        
        ActualUserspaceProvider* Actual(void)
        {
            return _owner;
        }
        
        void Record(int type, IpcEndpoint *connection, KernelBufferMemory *message, KernelObject *owner = NULL)
        {
            Data *entry = new Data(type, connection, message, owner);
            _events->Push(entry);
            entry->Release();
            if (_count == 0)
                SetSignalled(this, true);
            _count++;
        }
        
        Data* Read(void)
        {
            if (_count == 0)
                return NULL;
            Data *result = (Data*)_events->Pop();
            _count--;
            if (_count == 0)
                SetSignalled(this, false);
            return result;
        }
        
    protected:
        ~UserspaceProvider()
        {
            _events->Release();
        }
    };
    
    class ActualUserspaceProvider : public GenericProvider
    {
    private:
        KernelArray *_clientList;
        KernelDictionary *_serviceMap;
        UserspaceProvider *_monitor;
        
    public:
        CLASSNAME(GenericProvider, IPC_Service::ActualUserspaceProvider);
        
        ActualUserspaceProvider()
        {
            _clientList = new KernelArray();
            _serviceMap = new KernelDictionary();
            _monitor = new UserspaceProvider(this);
        }
        
        UserspaceProvider* Monitor(void)
        {
            return _monitor;
        }
        
        IpcService* LaunchService(KernelString *name, KernelString *type)
        {
            IpcService *service = new IpcService(name, type);
            Service *gpService = new Service(this, service);
            service->Release();
            Launch(gpService);
            IpcService *result = gpService->ServiceObject();
            _serviceMap->Set(result, gpService);
            return result;
        }
        
        bool KillService(IpcService *service)
        {
            Service *gpService = (Service*)_serviceMap->ObjectFor(service);
            if (!gpService)
                return false;
            Kill(gpService);
            gpService->Release();
            return true;
        }
        
        IpcClient* LaunchClient(KernelString *name)
        {
            Input *input = new Input(this, name);
            _clientList->Add(input);
            _serviceList->AddInput(input);
            return input;
        }
        
        bool KillClient(IpcClient *client)
        {
            if (!_clientList->Contains(client))
                return false;
            _serviceList->RemoveInput(client);
            _clientList->Remove(client);
            client->Release();
            return true;
        }
        
    protected:
        ~ActualUserspaceProvider()
        {
            _serviceMap->Release();
            _clientList->Release();
            _monitor->Release();
        }

        InputConnection* InputConnectionStart(KernelString *name, IpcEndpoint *connection)
        {
            _monitor->Record(IPC_ERROR_EVENT_INPUT_CONNECT, connection, NULL, name);
            return new InputConnection(this, name, connection);
        }
        void InputConnectionReceived(InputConnection *connection, KernelBufferMemory *message)
        {
            _monitor->Record(IPC_ERROR_EVENT_INPUT_MESSAGE, connection->Link(), message);
        }
        void InputConnectionEnd(InputConnection *connection)
        {
            _monitor->Record(IPC_ERROR_EVENT_INPUT_DISCONNECT, connection->Link(), NULL);
            connection->Release();
        }
        OutputConnection* OutputConnectionStart(Service *source, IpcEndpoint *connection)
        {
            _monitor->Record(IPC_ERROR_EVENT_OUTPUT_CONNECT, connection, NULL, source->ServiceObject());
            return new OutputConnection(this, source, connection);
        }
        void OutputConnectionMessage(OutputConnection *connection, KernelBufferMemory *message)
        {
            _monitor->Record(IPC_ERROR_EVENT_OUTPUT_MESSAGE, connection->Link(), message);
        }
        void OutputConnectionEnd(OutputConnection *oldConnection)
        {
            _monitor->Record(IPC_ERROR_EVENT_OUTPUT_DISCONNECT, oldConnection->Link(), NULL);
            oldConnection->Release();
        }
    };
    
    class Service : public SystemService
    {
    protected:
        void Handle(UInt64 *parameters)
        {
            switch (parameters[0]) {
                default:
                    parameters[0] = IPC_ERROR_UNKNOWN_FUNCTION;
                    return;
                case IPC_MEMORY_CREATE:
                {
                    KernelBufferMemory *mem = new KernelBufferMemory(parameters[1]);
                    parameters[1] = Process::Mapper()->Map(mem);
                }
                    break;
                case IPC_MEMORY_MAP:
                {
                    KernelBufferMemory *mem = (KernelBufferMemory*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!mem || !mem->IsDerivedFromClass("KernelBufferMemory")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    // TODO: read only?
                    KernelBufferMemory::Map *mapping = new KernelBufferMemory::Map(Process::Active, mem, false);
                    parameters[1] = Process::Mapper()->Map(mapping);
                    parameters[2] = (UInt64)mapping->LinearBase();
                }
                    break;
                case IPC_CONNECTION_SEND:
                {
                    IpcEndpoint *connection = (IpcEndpoint*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!connection || !connection->IsDerivedFromClass("IpcEndpoint")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelBufferMemory *mem = (KernelBufferMemory*)Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!mem || !mem->IsDerivedFromClass("KernelBufferMemory")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    connection->SendBuffer(mem);
                }
                    break;
                case IPC_CONNECTION_RECEIVE:
                {
                    IpcEndpoint *connection = (IpcEndpoint*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!connection || !connection->IsDerivedFromClass("IpcEndpoint")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelBufferMemory *data = connection->Read(false);
                    if (data) {
                        data->AddRef();
                        parameters[1] = Process::Mapper()->Map(data);
                    } else {
                        parameters[0] = IPC_ERROR_NO_BUFFER;
                        return;
                    }
                }
                    break;
                case IPC_PROVIDER_CREATE:
                {
                    ActualUserspaceProvider *provider = new ActualUserspaceProvider();
                    parameters[1] = Process::Mapper()->Map(provider->Monitor());
                }
                    break;
                case IPC_PROVIDER_OUTPUT_START:
                {
                    UserspaceProvider *provider = (UserspaceProvider*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!provider || !provider->IsDerivedFromClass("IPC_Service::UserspaceProvider")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelString *name = (KernelString*)Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!name || !name->IsDerivedFromClass("KernelString")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelString *type = (KernelString*)Process::Mapper()->Find((::Handle)parameters[3]);
                    if (!type || !type->IsDerivedFromClass("KernelString")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    parameters[1] = Process::Mapper()->Map(provider->Actual()->LaunchService(name, type));
                }
                    break;
                case IPC_SERVICE_TERMINATE:
                {
                    UserspaceProvider *provider = (UserspaceProvider*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!provider || !provider->IsDerivedFromClass("IPC_Service::UserspaceProvider")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    IpcService *service = (IpcService*)Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!service || !service->IsDerivedFromClass("IpcService")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    if (!provider->Actual()->KillService(service)) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                }
                    break;
                case IPC_PROVIDER_INPUT_START:
                {
                    UserspaceProvider *provider = (UserspaceProvider*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!provider || !provider->IsDerivedFromClass("IPC_Service::UserspaceProvider")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelString *name = (KernelString*)Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!name || !name->IsDerivedFromClass("KernelString")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    parameters[1] = Process::Mapper()->Map(provider->Actual()->LaunchClient(name));
                }
                    break;
                case IPC_INPUT_TERMINATE:
                {
                    UserspaceProvider *provider = (UserspaceProvider*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!provider || !provider->IsDerivedFromClass("IPC_Service::UserspaceProvider")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    IpcClient *client = (IpcClient*)Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!client || !client->IsDerivedFromClass("IpcClient")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    if (!provider->Actual()->KillClient(client)) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                }
                    break;
                case IPC_INPUT_GET_NAME:
                {
                    IpcClient *client = (IpcClient*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!client || !client->IsDerivedFromClass("IpcClient")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    parameters[1] = Process::Mapper()->Map(client->Name());
                }
                    break;
                case IPC_INPUT_GET_PROPERTIES:
                {
                    IpcClient *client = (IpcClient*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!client || !client->IsDerivedFromClass("IpcClient")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    parameters[1] = Process::Mapper()->Map(client->Properties());
                }
                    break;
                case IPC_SERVICE_GET_NAME:
                {
                    IpcService *service = (IpcService*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!service || !service->IsDerivedFromClass("IpcService")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    parameters[1] = Process::Mapper()->Map(service->Name());
                }
                    break;
                case IPC_SERVICE_GET_TYPE:
                {
                    IpcService *service = (IpcService*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!service || !service->IsDerivedFromClass("IpcService")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    parameters[1] = Process::Mapper()->Map(service->ServiceType());
                }
                    break;
                case IPC_SERVICE_GET_PROPERTIES:
                {
                    IpcService *service = (IpcService*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!service || !service->IsDerivedFromClass("IpcService")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    parameters[1] = Process::Mapper()->Map(service->Properties());
                }
                    break;
                case IPC_PROVIDER_CONNECT:
                {
                    IpcClient *client = (IpcClient*)Process::Mapper()->Find((::Handle)parameters[1]);
                    IpcService *service = (IpcService*)Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!client || !service || !client->IsDerivedFromClass("IpcClient") || !service->IsDerivedFromClass("IpcService")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    client->Connect(service);
                }
                    break;
                case IPC_PROVIDER_DISCONNECT:
                {
                    IpcClient *client = (IpcClient*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!client || !client->IsDerivedFromClass("IpcClient")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    client->Disconnect();
                }
                    break;
                case IPC_PROVIDER_GET_EVENT:
                {
                    UserspaceProvider *provider = (UserspaceProvider*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!provider || !provider->IsDerivedFromClass("IPC_Service::UserspaceProvider")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    UserspaceProvider::Data *data = provider->Read();
                    if (data) {
                        data->Save(parameters);
                        return;
                    }
                }
                    break;
            }
            parameters[0] = IPC_ERROR_SUCCESS;
        }

        int Interrupt(void)
        {
            return IPC_SYSCALL;
        }
    };
}

static IPC_Service::Service service;

void IpcServiceMonitor::ConfigureService(void)
{
    service.Register();
}
