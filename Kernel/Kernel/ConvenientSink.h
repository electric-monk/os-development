#ifndef __CONVENIENTSINK_H__
#define __CONVENIENTSINK_H__

#include "Provider.h"

class ConvenientSink : public GenericProvider
{
public:
    CLASSNAME(GenericProvider, ConvenientSink);
    
    ConvenientSink(RunloopThread *runloop = NULL, InterfaceHelper *helper = NULL);
    
    bicycle::function<void(IpcEndpoint *source, Interface_Response *response)> unhandledMessageHandler;
    // Only call the following on destinations acquired from CreateInput's handler
    void PerformTask(IpcEndpoint *destination, bicycle::function<void(Interface_Request*)> generate, bicycle::function<void(Interface_Response*)> response);
    void AddTask(bicycle::function<void(void)> task);
    
    void CreateInput(KernelString *name, bicycle::function<bool(IpcEndpoint*)> connectionHandler, bicycle::function<void(IpcClient*)> completion);
    void StopInput(IpcClient *input);
    
    // For GenericProvider
    InputConnection* InputConnectionStart(KernelString *name, IpcEndpoint *connection);
    void InputConnectionReceived(InputConnection *connection, KernelBufferMemory *message);
    void InputConnectionEnd(InputConnection *connection);
    OutputConnection* OutputConnectionStart(Service *source, IpcEndpoint *connection);
    void OutputConnectionMessage(OutputConnection *connection, KernelBufferMemory *message);
    void OutputConnectionEnd(OutputConnection *oldConnection);
    
protected:
    ~ConvenientSink();
    
private:
    KernelDictionary *_inputs;
    InterfaceHelper *_helper;
};

#endif // __CONVENIENTSINK_H__
