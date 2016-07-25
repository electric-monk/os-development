#ifndef __GENERICKEYBOARD_H__
#define __GENERICKEYBOARD_H__

#include "Driver.h"

class RunloopThread;

// Base class to provide
class GenericKeyboard : public ProviderDriver
{
public:
    CLASSNAME(ProviderDriver, GenericKeyboard);
    
    GenericKeyboard(const char *name);
    
    bool Start(Driver *parent);
    void Stop(void);

protected:
    ~GenericKeyboard();
    
    bool HandleKey(UInt8 event);
    void SendEvent(int key, bool down);
    
    RunloopThread *_thread;
    
    bool _extended;
    int _identifierCount;
    
    KernelDictionary *_mapping;
    KernelArray *_pressed;

    // ProviderDriver
    Connection* ConnectionStart(Service *service, IpcEndpoint *endpoint);
    void ConnectionReceive(Connection *connection, KernelBufferMemory *message);
    void ConnectionStop(Connection *connection);
    
private:
    Service *_service;
};

#endif // __GENERICKEYBOARD_H__
