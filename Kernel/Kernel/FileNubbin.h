#ifndef __FILENUBBIN_H__
#define __FILENUBBIN_H__

#include "Provider.h"

class FileNubbin : public GenericProvider
{
public:
    FileNubbin();
    
    // Hooking up inputs
    UInt32 InputCount(void);
    KernelDictionary* Input(UInt32 index);
    
protected:
    ~FileNubbin();
    
    InputConnection* InputConnectionStart(KernelString *name, IpcEndpoint *connection);
    void InputConnectionReceived(InputConnection *connection, KernelBufferMemory *message);
    void InputConnectionEnd(InputConnection *connection);
    OutputConnection* OutputConnectionStart(Service *source, IpcEndpoint *connection);
    void OutputConnectionMessage(OutputConnection *connection, KernelBufferMemory *message);
    void OutputConnectionEnd(OutputConnection *oldConnection);
    
private:
    InterfaceHelper *_tasks;
    
    InputConnection* Input(void);
};

#endif // __FILENUBBIN_H__
