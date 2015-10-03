#ifndef __GENERICKEYBOARD_H__
#define __GENERICKEYBOARD_H__

#include "Driver.h"

class IpcServiceList;
class RunloopThread;

// Base class to provide
class GenericKeyboard : public Driver
{
public:
    GenericKeyboard(const char *name);
    
    bool Start(Driver *parent);
    void Stop(void);

protected:
    ~GenericKeyboard();
    
    bool HandleKey(UInt8 event);
    
    IpcServiceList *_serviceList;
    RunloopThread *_thread;
    
    bool _extended;
    
    KernelDictionary *_mapping;
    KernelArray *_pressed;
};

#endif // __GENERICKEYBOARD_H__
