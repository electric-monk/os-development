#include "IPC.h"
#include "Handle_internal.h"
#include "../../Kernel/Kernel/IPC_Service.h"

namespace Kernel {
    
    namespace IPC {

        Memory* Memory::Create(int maximumSize)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(IPC_SYSCALL), "a"(IPC_MEMORY_CREATE), "b"((int)maximumSize)
                          : /* clobbers */);
            CheckResult(IPC_SYSCALL, IPC_MEMORY_CREATE, result);
            return (Memory*)handle;
        }
        
        void* Memory::GetPointer(Handle **mapping)
        {
            int result;
            int handle;
            int pointer;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle), "=c"(pointer)
                          : /* inputs */ [interrupt]"N"(IPC_SYSCALL), "a"(IPC_MEMORY_MAP), "b"((int)this)
                          : /* clobbers */);
            CheckResult(IPC_SYSCALL, IPC_MEMORY_MAP, result);
            *mapping = (Handle*)handle;
            return (void*)pointer;
        }
        
        void Connection::Send(Memory *buffer)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(IPC_SYSCALL), "a"(IPC_CONNECTION_SEND), "b"((int)this), "c"((int)buffer)
                          : /* clobbers */);
            CheckResult(IPC_SYSCALL, IPC_CONNECTION_SEND, result);
        }
        
        Memory* Connection::Receive(void)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(IPC_SYSCALL), "a"(IPC_CONNECTION_RECEIVE), "b"((int)this)
                          : /* clobbers */);
            CheckResult(IPC_SYSCALL, IPC_CONNECTION_RECEIVE, result);
            return (Memory*)handle;
        }
        
        Collections::String* Service::Name(void)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(IPC_SYSCALL), "a"(IPC_SERVICE_GET_NAME), "b"((int)this)
                          : /* clobbers */);
            CheckResult(IPC_SYSCALL, IPC_SERVICE_GET_NAME, result);
            return (Collections::String*)handle;
        }
        
        Collections::String* Service::Type(void)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(IPC_SYSCALL), "a"(IPC_SERVICE_GET_TYPE), "b"((int)this)
                          : /* clobbers */);
            CheckResult(IPC_SYSCALL, IPC_SERVICE_GET_TYPE, result);
            return (Collections::String*)handle;
        }
        
        Collections::Dictionary* Service::Properties(void)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(IPC_SYSCALL), "a"(IPC_SERVICE_GET_PROPERTIES), "b"((int)this)
                          : /* clobbers */);
            CheckResult(IPC_SYSCALL, IPC_SERVICE_GET_PROPERTIES, result);
            return (Collections::Dictionary*)handle;
        }
        
        void Client::Connect(Service *service)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(IPC_SYSCALL), "a"(IPC_PROVIDER_CONNECT), "b"((int)this), "c"((int)service)
                          : /* clobbers */);
            CheckResult(IPC_SYSCALL, IPC_PROVIDER_CONNECT, result);
        }
        
        void Client::Disconnect(void)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(IPC_SYSCALL), "a"(IPC_PROVIDER_DISCONNECT), "b"((int)this)
                          : /* clobbers */);
            CheckResult(IPC_SYSCALL, IPC_PROVIDER_DISCONNECT, result);
        }

        Collections::String* Client::Name(void)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(IPC_SYSCALL), "a"(IPC_INPUT_GET_NAME), "b"((int)this)
                          : /* clobbers */);
            CheckResult(IPC_SYSCALL, IPC_INPUT_GET_NAME, result);
            return (Collections::String*)handle;
        }
        
        Collections::Dictionary* Client::Properties(void)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(IPC_SYSCALL), "a"(IPC_INPUT_GET_PROPERTIES), "b"((int)this)
                          : /* clobbers */);
            CheckResult(IPC_SYSCALL, IPC_INPUT_GET_PROPERTIES, result);
            return (Collections::Dictionary*)handle;
        }
        
        Provider* Provider::Create(void)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(IPC_SYSCALL), "a"(IPC_PROVIDER_CREATE)
                          : /* clobbers */);
            CheckResult(IPC_SYSCALL, IPC_PROVIDER_CREATE, result);
            return (Provider*)handle;
        }
        
        Service* Provider::CreateService(Collections::String *name, Collections::String *type)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(IPC_SYSCALL), "a"(IPC_PROVIDER_OUTPUT_START), "b"((int)name), "c"((int)type)
                          : /* clobbers */);
            CheckResult(IPC_SYSCALL, IPC_PROVIDER_OUTPUT_START, result);
            return (Service*)handle;
        }
        
        void Provider::TerminateService(Service *service)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(IPC_SYSCALL), "a"(IPC_SERVICE_TERMINATE), "b"((int)this), "c"((int)service)
                          : /* clobbers */);
            CheckResult(IPC_SYSCALL, IPC_SERVICE_TERMINATE, result);
        }
        
        Client* Provider::CreateClient(Collections::String *name)
        {
            int result;
            int handle;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(handle)
                          : /* inputs */ [interrupt]"N"(IPC_SYSCALL), "a"(IPC_PROVIDER_INPUT_START), "b"((int)this), "c"((int)name)
                          : /* clobbers */);
            CheckResult(IPC_SYSCALL, IPC_PROVIDER_INPUT_START, result);
            return (Client*)handle;
        }
        
        void Provider::TerminateClient(Client *client)
        {
            int result;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result)
                          : /* inputs */ [interrupt]"N"(IPC_SYSCALL), "a"(IPC_INPUT_TERMINATE), "b"((int)this), "c"((int)client)
                          : /* clobbers */);
            CheckResult(IPC_SYSCALL, IPC_INPUT_TERMINATE, result);
        }
        
        Provider::EventType Provider::GetEvent(Connection **ipcConnection, Memory **message, ProviderIO **providerConnection)
        {
            int result;
            int conn, mess, star;
            asm volatile ("int %[interrupt]"
                          : /* outputs */ "=a"(result), "=b"(conn), "=c"(mess), "=d"(star)
                          : /* inputs */ [interrupt]"N"(IPC_SYSCALL), "a"(IPC_PROVIDER_GET_EVENT), "b"((int)this)
                          : /* clobbers */);
            Provider::EventType returnValue;
            switch (result) {
                case IPC_ERROR_EVENT_INPUT_CONNECT:
                    returnValue = eventInputConnect;
                    break;
                case IPC_ERROR_EVENT_INPUT_MESSAGE:
                    returnValue = eventInputMessage;
                    break;
                case IPC_ERROR_EVENT_INPUT_DISCONNECT:
                    returnValue = eventInputDisconnect;
                    break;
                case IPC_ERROR_EVENT_OUTPUT_CONNECT:
                    returnValue = eventOutputConnect;
                    break;
                case IPC_ERROR_EVENT_OUTPUT_MESSAGE:
                    returnValue = eventOutputMessage;
                    break;
                case IPC_ERROR_EVENT_OUTPUT_DISCONNECT:
                    returnValue = eventOutputDisconnect;
                    break;
                default:
                    // In this case, CheckResult will do the right thing anyway
                    returnValue = eventNone;
                    break;
            }
            CheckResult(IPC_SYSCALL, IPC_PROVIDER_GET_EVENT, (returnValue == eventNone) ? result : IPC_ERROR_SUCCESS);
            if (ipcConnection)
                *ipcConnection = (Connection*)conn;
            if (message)
                *message = (Memory*)mess;
            if (providerConnection)
                *providerConnection = (ProviderIO*)star;
            return returnValue;
        }
        
    }

}
