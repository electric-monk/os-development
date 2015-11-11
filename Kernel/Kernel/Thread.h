#ifndef __THREAD_H__
#define __THREAD_H__

#include "KernelObject.h"
#include "CPU.h"
#include "StandardPC_traps.h"
#include "mem_physical.h"
#include "Blocking.h"

class Interrupts;
class Process;
class KernelDictionary;
class GrowableStack;

typedef void (*ThreadCallback)(void *context);

typedef enum {
    tsBlocked,
    tsRunnable,
    tsRunning,
    tsCompleted,
} THREADSTATE;

class Thread : public SignalWatcher // TODO: Implement SignalWatcher mechanisms
{
public:
    CLASSNAME(SignalWatcher, Thread);
    
    typedef enum {
        tmAuto = 0,     // Automatically select user or kernel mode
        tmKernel,
        tmUserspace,
    } ThreadMode;
    
    static void ConfigureService(void);
    
    Thread(Process *process, void (*entryPoint)(void*), void *context, ThreadMode mode = tmAuto, UInt32 stackSize = 8192);
    
    Process *_process;
    CPU::Context *_context;
    THREADSTATE _state;

    static Thread *Active asm("%gs:8");
    
    void Kill(void);
    KernelArray* BlockOn(BlockableObject *source);
    void Sleep(UInt32 microseconds);
    
    void Select(CPU::Context **scheduler);
    
    KernelDictionary* KernelThreadLocalStorage(void) { return _kernelStorage; }
    
    // Thread list control
    static Thread* ThreadCursor(void);
    static void ThreadNext(void);
    static void ThreadLast(void);
    
    static void DebugPrint(void);
    
protected:
    ~Thread();
    
    void SignalChanged(BlockableObject *watching, bool active);
    
private:
    BlockableObject *_blockingObject;
    KernelArray *_blockingResult;
    
    // The stack used in the kernel
    char *_kernelStack;
    UInt32 _kernelStackLength;
    // The stack used in userspace
    GrowableStack *_stackInProcess;
    
    TrapFrame *_trapFrame;
    UInt32 _stackSize;
    
    KernelDictionary *_kernelStorage;
    
    // Thread list
    void Attach(void);
    void Detach(void);
    
    static Thread *first, *last, *cursor;
    Thread *_last, *_next;
};

class KernelThread : public Thread
{
public:
    CLASSNAME(Thread, KernelThread);
    
    KernelThread(UInt32 stackSize = 8192);
    
protected:
    virtual void ThreadMain(void) = 0;
    
private:
    static void Thunk(void*);
};

#endif // __THREAD_H__
