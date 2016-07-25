#ifndef __IPC_MANAGER_H__
#define __IPC_MANAGER_H__

#include "Blocking.h"

class IpcClient;
class IpcService;
class IpcEndpoint;
namespace IPC_Manager_Internal {
    class State;
}

class IpcServiceProxy : public KernelObject
{
public:
    CLASSNAME(KernelObject, IpcServiceProxy);
    
    // This class represents a real service, but only keeps track of its state. Thus, the real service simply calls the methods here when stuff happens, to notify others.
    IpcServiceProxy(KernelObject *service);
    
    void Start(void);
    void Stop(void);
    
    void AddInput(IpcClient *client);
    void RemoveInput(IpcClient *client);
    
    void AddOutput(IpcService *output);
    void RemoveOutput(IpcService *output);
    
    void StartInput(IpcClient *client, IpcEndpoint *endpoint);
    void StartOutput(IpcService *output, IpcEndpoint *endpoint);
    
protected:
    ~IpcServiceProxy();
    
private:
    bool _active;
    KernelObject *_service;
};

class IpcServiceMonitor : public BlockableObject
{
public:
    CLASSNAME(BlockableObject, IpcServiceMonitor);

    IpcServiceMonitor(void);
    
    KernelArray* Changes(void);
    
protected:
    ~IpcServiceMonitor();
    
private:
    IPC_Manager_Internal::State *_state;
    bool _triggered;
    void SetTrigger(bool shouldTrigger);
};

#endif // __IPC_MANAGER_H__
