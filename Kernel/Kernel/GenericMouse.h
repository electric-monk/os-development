#ifndef __GENERICMOUSE_H__
#define __GENERICMOUSE_H__

#include "Driver.h"

class RunloopThread;

// Base class to provide
class GenericMouse : public ProviderDriver
{
public:
    CLASSNAME(ProviderDriver, GenericMouse);
    
    GenericMouse(const char *name);
    
    bool Start(Driver *parent);
    void Stop(void);

protected:
    ~GenericMouse();
    
    void Move(SInt32 x, SInt32 y);
    void Button(UInt32 index, bool down);
    void Scroll(SInt32 x, SInt32 y);
    
    RunloopThread *_thread;
    
    // ProviderDriver
    Connection* ConnectionStart(Service *service, IpcEndpoint *endpoint);
    void ConnectionReceive(Connection *connection, KernelBufferMemory *message);
    void ConnectionStop(Connection *connection);
    
private:
    Service *_service;
    int _identifierCount;
    
    void SendMotion(int x, int y, int type);
};

#endif // __GENERICMOUSE_H__
