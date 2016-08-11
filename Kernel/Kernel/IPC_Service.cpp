#include "IPC_Service.h"
#include "Interrupts.h"
#include "IPC.h"
#include "Process.h"
#include "Provider.h"

namespace IPC_Service {
    class UserspaceProvider : public GenericProvider
    {
    protected:
        KernelArray *_inputs;
        
    public:
        UserspaceProvider(KernelArray *inputs)
        {
            _inputs = inputs;
            // TODO: Copy?
            _inputs->AddRef();
            // TODO: Fix this
        }
        
    protected:
        ~UserspaceProvider()
        {
            _inputs->Release();
        }

        InputConnection* InputConnectionStart(KernelString *name, IpcEndpoint *connection)
        {
            return NULL;
        }
        void InputConnectionReceived(InputConnection *connection, KernelBufferMemory *message)
        {
            
        }
        void InputConnectionEnd(InputConnection *connection)
        {
            
        }
        OutputConnection* OutputConnectionStart(Service *source, IpcEndpoint *connection)
        {
            return NULL;
        }
        void OutputConnectionMessage(OutputConnection *connection, KernelBufferMemory *message)
        {
            
        }
        void OutputConnectionEnd(OutputConnection *oldConnection)
        {
            
        }
    };
    
    class UserspaceProviderContainer : public BlockableObject
    {
    private:
        KernelFIFO *_events;
        
    public:
        
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
                    KernelArray *inputs = (KernelArray*)Process::Mapper()->Find((::Handle)parameters[0]);
                    if (!inputs || !inputs->IsDerivedFromClass("KernelArray")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    UserspaceProvider *provider = new UserspaceProvider(inputs);
                    parameters[1] = Process::Mapper()->Map(provider);
                }
                    break;
                case IPC_PROVIDER_START:
                {
                    UserspaceProvider *provider = (UserspaceProvider*)Process::Mapper()->Find((::Handle)parameters[0]);
                    if (!provider || !provider->IsDerivedFromClass("UserspaceProvider")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelString *name = (KernelString*)Process::Mapper()->Find((::Handle)parameters[1]);
                    if (!name || !name->IsDerivedFromClass("KernelString")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    KernelString *type = (KernelString*)Process::Mapper()->Find((::Handle)parameters[2]);
                    if (!type || !type->IsDerivedFromClass("KernelString")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    // TODO
                }
                    break;
                case IPC_SERVICE_TERMINATE:
                {
                    UserspaceProvider *provider = (UserspaceProvider*)Process::Mapper()->Find((::Handle)parameters[0]);
                    if (!provider || !provider->IsDerivedFromClass("UserspaceProvider")) {
                        parameters[0] = IPC_ERROR_INVALID_HANDLE;
                        return;
                    }
                    // TODO
                }
                    break;
                case IPC_PROVIDER_GET_EVENT:
                case IPC_CONNECTION_GET_IPC:
                case IPC_CONNECTION_GET_NAME:
                case IPC_CONNECTION_GET_TYPE:
                    ;
            }
            parameters[0] = IPC_ERROR_SUCCESS;
        }

        int Interrupt(void)
        {
            return IPC_SYSCALL;
        }
    };
}
