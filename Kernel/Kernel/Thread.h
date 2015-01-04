#ifndef __THREAD_H__
#define __THREAD_H__

#include "KernelObject.h"
#include "CPU.h"
#include "StandardPC_traps.h"

class Interrupts;

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
    
    Thread(UInt32 stackSize = 8192);
    
    CPU::Context *_context;

    static Thread *Active asm("%gs:8");
    
    void Kill(void);
    
    // Thread list control
    static Thread* ThreadCursor(void);
    static void ThreadNext(void);
    static void ThreadLast(void);
    
protected:
    ~Thread();

    virtual void ThreadMain(void) = 0;
    
private:
    char *_stack;
    TrapFrame *_trapFrame;
    UInt32 _stackSize;
    
    static void Thunk(void*);
    
    // Thread list
    void Attach(void);
    void Detach(void);
    
    static Thread *first, *last, *cursor;
    Thread *_last, *_next;
};

#endif // __THREAD_H__
