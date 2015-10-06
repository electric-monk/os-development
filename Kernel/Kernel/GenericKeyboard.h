#ifndef __GENERICKEYBOARD_H__
#define __GENERICKEYBOARD_H__

#include "Driver.h"

class IpcServiceList;
class RunloopThread;

// Base class to provide
class GenericKeyboard : public ProviderDriver
{
public:
    GenericKeyboard(const char *name);
    
    bool Start(Driver *parent);
    void Stop(void);

protected:
    ~GenericKeyboard();
    
    bool HandleKey(UInt8 event);
    void SendEvent(int key, bool down);
    
    IpcServiceList *_serviceList;
    RunloopThread *_thread;
    
    bool _extended;
    int _identifierCount;
    
    KernelDictionary *_mapping;
    KernelArray *_pressed;

    // ProviderDriver
    Connection* ConnectionStart(Service *service, IpcEndpoint *endpoint);
    void ConnectionReceive(Connection *connection, KernelBufferMemory *message);
    void ConnectionStop(Connection *connection);
};

#endif // __GENERICKEYBOARD_H__
