#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "mem_logical.h"
#include "KernelObject.h"

class IpcService;
class RunloopThread;
class KernelDictionary;
class InterfaceHelper;

class Process : public KernelObject
{
public:
    Process(const char *name);
    
    static Process *Active asm("%gs:4");
    
    SPageDirectoryInfo pageDirectory;
    
    void AttachImage(IpcService *binaryImageService);
    
protected:
    ~Process();
    
    RunloopThread *_runloop;
    
private:
    KernelDictionary *_binaries;
    InterfaceHelper *_helper;
};

#endif // __PROCESS_H__
