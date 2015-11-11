#ifndef __IMAGELOADER_H__
#define __IMAGELOADER_H__

#include "Provider.h"

class ImageLoader : public GenericProvider
{
public:
    CLASSNAME(GenericProvider, ImageLoader);
    
    ImageLoader();
    
    UInt32 InputCount(void);
    KernelDictionary* Input(UInt32 index);
protected:
    ~ImageLoader();
    
    InputConnection* InputConnectionStart(KernelString *name, IpcEndpoint *connection);
    void InputConnectionReceived(InputConnection *connection, KernelBufferMemory *message);
    void InputConnectionEnd(InputConnection *connection);
    OutputConnection* OutputConnectionStart(Service *source, IpcEndpoint *connection);
    void OutputConnectionMessage(OutputConnection *connection, KernelBufferMemory *message);
    void OutputConnectionEnd(OutputConnection *oldConnection);
    
private:
    InterfaceHelper *_tasks;
    KernelArray *_symbols;
    KernelArray *_segments;
    
    InputConnection* Input(void);
};

#endif // __IMAGELOADER_H__
