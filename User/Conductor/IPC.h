#ifndef __IPC_H__
#define __IPC_H__

#include "Handle.h"
#include "Blocking.h"

namespace Kernel {
    
    namespace Collections {
        class String;
        class Dictionary;
    }
    
    namespace IPC {
        
        class Memory : public Handle
        {
        public:
            static Memory* Create(int maximumSize);
            
            void* GetPointer(Handle **mapping); // Release "mapping" to release pointer
        };
        
        class Connection : public Handle
        {
        public:
            void Send(Memory *buffer);
            Memory* Receive(void);
        };
        
        class ProviderIO : public Handle
        {
        };
        
        class Service : public ProviderIO
        {
        public:
            Collections::String* Name(void);
            Collections::String* Type(void);
            Collections::Dictionary* Properties(void);
        };
        
        class Client : public ProviderIO
        {
        public:
            Collections::String* Name(void);
            Collections::Dictionary* Properties(void);
            void Connect(Service *service);
            void Disconnect(void);
        };
        
        class Provider : public Blocking::Blockable
        {
        public:
            static Provider* Create(void);
            
            Service* CreateService(Collections::String *name, Collections::String *type);
            void TerminateService(Service *service);
            
            Client* CreateClient(Collections::String *name);
            void TerminateClient(Client *service);
            
            typedef enum {
                eventNone,
                eventInputConnect,
                eventInputMessage,
                eventInputDisconnect,
                eventOutputConnect,
                eventOutputMessage,
                eventOutputDisconnect,
            } EventType;
            
            EventType GetEvent(Connection **ipcConnection, Memory **message, ProviderIO **providerConnection);
        };
        
    }
    
}

#endif // __IPC_H__
