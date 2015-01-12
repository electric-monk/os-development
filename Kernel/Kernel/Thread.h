#ifndef __THREAD_H__
#define __THREAD_H__

#include "KernelObject.h"
#include "CPU.h"
#include "StandardPC_traps.h"
#include "mem_physical.h"

class Interrupts;
class Process;

#define SERVICE_THREAD          0x70            // interrupt
// EAX parameters
#define SERVICE_THREAD_END      0x00000001      // EBX = ending thread

typedef void (*ThreadCallback)(void *context);

typedef enum {
    tsRunnable,
    tsCompleted
} THREADSTATE;

class Thread : public KernelObject
{
public:
    static void ConfigureService(Interrupts *interruptSource);
    
    Thread(Process *process, void (*entryPoint)(void*), void *context, UInt32 stackSize = 8192);
    
    Process *_process;
    CPU::Context *_context;

    static Thread *Active asm("%gs:8");
    
    void Kill(void);
    
    void Select(CPU::Context **scheduler);
    
    // Thread list control
    static Thread* ThreadCursor(void);
    static void ThreadNext(void);
    static void ThreadLast(void);
    
protected:
    ~Thread();
    
private:
    // The stack used in the kernel
    char *_kernelStack;
    // The stack used in userspace
    char *_stackInProcess;
    PhysicalPointer _processStack;
    
    TrapFrame *_trapFrame;
    UInt32 _stackSize;
    
    // Thread list
    void Attach(void);
    void Detach(void);
    
    static Thread *first, *last, *cursor;
    Thread *_last, *_next;
};

class KernelThread : public Thread
{
public:
    KernelThread(UInt32 stackSize = 8192);
    
protected:
    virtual void ThreadMain(void) = 0;
    
private:
    static void Thunk(void*);
};

#endif // __THREAD_H__
