#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "mem_logical.h"
#include "KernelObject.h"

class Process : public KernelObject
{
public:
    Process(const char *name);
    
    static Process *Active asm("%gs:4");
    
    SPageDirectoryInfo pageDirectory;
    
protected:
    ~Process();
};

#endif // __PROCESS_H__
