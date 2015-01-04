#include "Thread.h"
#include "debug.h"
#include "Interrupts.h"
#include "StandardPC_traps.h"
#include "Scheduler.h"

// This pointer points into the guts of the interrupt handler - the part that returns from an interrupt
extern "C" void trapret(void);

// This pointer points to an assembler routine to configure the callback
extern "C" void ThreadEntryPoint(void);
extern "C" void KernelThreadEntryPoint(void);
extern "C" UInt32  TestGetEflag(void);

Thread::Thread(UInt32 stackSize)
{
    // Configure things
    _stackSize = stackSize;
    _stack = new char[_stackSize];

    // Configure initial stack
    char *stackPointer = _stack + _stackSize;
    
    // For ThreadEntryPoint - first context, second function
    stackPointer -= 4;
    *((void**)stackPointer) = (void*)this;
    stackPointer -= 4;
    *((void**)stackPointer) = (void*)Thread::Thunk;
    
    // Create trap frame
    stackPointer -= sizeof(TrapFrame);
    _trapFrame = (TrapFrame*)stackPointer;
    _trapFrame->CS = (SEG_KCODE << 3);
    _trapFrame->DS = (SEG_KDATA << 3);
    _trapFrame->ES = _trapFrame->DS;
    _trapFrame->SS = _trapFrame->DS;
    _trapFrame->EFlags = TestGetEflag();//FL_IF;
    _trapFrame->ESP = UInt32(stackPointer + sizeof(TrapFrame));
    _trapFrame->EIP = (UInt32)KernelThreadEntryPoint;   // Upon exiting the (pretend) trap, start the thread
    
    // Configure context
    stackPointer -= sizeof(CPU::Context);
    _context = (CPU::Context*)stackPointer;
    _context->EDI = 0;
    _context->ESI = 0;
    _context->EBX = 0;
    _context->EBP = 0;
    _context->EIP = (UInt32)trapret;    // When this thread first runs, the first thing it'll do is return from the trap
    
    // Connect to list of threads
    Attach();
}

Thread::~Thread()
{
    Detach();   // May already be detached, but shouldn't hurt
    delete[] _stack;
}

void Thread::Kill(void)
{
    Detach();
    Scheduler::EnterFromInterrupt();
}

void Thread::Thunk(void *context)
{
    ((Thread*)context)->ThreadMain();
}

static bool ThreadInterruptHandler(void *context, void *state)
{
    // x86-specific
    TrapFrame *tf = (TrapFrame*)state;
    switch (tf->EAX) {
        case SERVICE_THREAD_END:
            ((Thread*)tf->EBX)->Kill();
            return true;
        default:
            return false;
    }
}

void Thread::ConfigureService(Interrupts *interruptSource)
{
    interruptSource->RegisterHandler(SERVICE_THREAD, ThreadInterruptHandler, NULL);
}

// Thread list
Thread* Thread::ThreadCursor(void)
{
    return cursor ?: first;
}

void Thread::ThreadNext(void)
{
    Thread *temp = ThreadCursor();
    cursor = (temp?temp->_next:NULL)?:first;
}

void Thread::ThreadLast(void)
{
    Thread *temp = ThreadCursor();
    cursor = (temp?temp->_last:NULL)?:last;
}

void Thread::Attach(void)
{
    _next = NULL;
    _last = last;
    if (last)
        last->_next = this;
    else
        first = this;
    last = this;
    kprintf("first 0x%.8x, last 0x%.8x, cursor 0x%.8x\n", first, last, cursor);
}

void Thread::Detach(void)
{
    if (_next)
        _next->_last = _last;
    if (_last)
        _last->_next = _next;
    if (first == this)
        first = _next;
    if (last == this)
        last = _last;
    if (cursor == this)
        cursor = _next ?: first;
    _last = _next = NULL;
    kprintf("first 0x%.8x, last 0x%.8x, cursor 0x%.8x\n", first, last, cursor);
}

Thread *Thread::first = NULL;
Thread *Thread::last = NULL;
Thread *Thread::cursor = NULL;
