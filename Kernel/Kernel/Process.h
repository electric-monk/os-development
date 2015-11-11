#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "mem_logical.h"
#include "KernelObject.h"

class IpcService;
class RunloopThread;
class KernelDictionary;
class InterfaceHelper;
class ObjectMapper;

class Process : public KernelObject
{
public:
    CLASSNAME(KernelObject, Process);
    
    Process(const char *name);
    
    static Process *Active asm("%gs:4");
    
    ObjectMapper* ActiveMapper(void);
    static ObjectMapper* Mapper(void);
    
    SPageDirectoryInfo pageDirectory;
    
    void AttachImage(IpcService *binaryImageService);
    
protected:
    ~Process();
    
    RunloopThread *_runloop;
    
private:
    KernelDictionary *_binaries;
    InterfaceHelper *_helper;
    ObjectMapper *_mapper;
};

#endif // __PROCESS_H__
