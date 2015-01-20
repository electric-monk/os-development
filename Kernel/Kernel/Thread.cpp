#include "Thread.h"
#include "debug.h"
#include "Interrupts.h"
#include "StandardPC_traps.h"
#include "Scheduler.h"
#include "Process.h"
#include "mem_logical.h"
#include "mem_physical.h"
#include "Collections.h"

#define CURRENT_PAGE_SIZE           4096

// This pointer points into the guts of the interrupt handler - the part that returns from an interrupt
extern "C" void trapret(void);

// This pointer points to an assembler routine to configure the callback
extern "C" void ThreadEntryPoint(void);
extern "C" void KernelThreadEntryPoint(void);
extern "C" UInt32  TestGetEflag(void);

Thread::Thread(Process *process, void (*entryPoint)(void*), void *context, UInt32 stackSize)
{
    _kernelStorage = new KernelDictionary();
    _kernelStack = new char[1024];
    
    // Make a note of the current process (if any - NULL is kernel thread)
    _process = process;
    if (_process) {
        _process->AddRef();
        // Process stack needs to exist in process space
        _processStack = CPhysicalMemory::AllocateContiguousPages();
        _stackInProcess = (char*)_process->pageDirectory.Map(fmUser | fmWritable, pmApplication, _processStack);
    }
    
    // Configure initial stack
    char *stackPointer = _kernelStack + 1024;
    int usedStack = 0;
    
    // For ThreadEntryPoint - first context, second function
    if (_process) {
        char *userStackPointer = (char*)rootAddressSpace.Map(fmWritable, pmKernel, _processStack) + CURRENT_PAGE_SIZE;
        userStackPointer -= 4;
        *((void**)userStackPointer) = (void*)context;
        usedStack += 4;
        rootAddressSpace.Unmap(userStackPointer);
    } else {
        stackPointer -= 4;
        *((void**)stackPointer) = (void*)context;
        stackPointer -= 4;
        *((void**)stackPointer) = (void*)entryPoint;
    }
    
    // Create trap frame
    stackPointer -= sizeof(TrapFrame);
    _trapFrame = (TrapFrame*)stackPointer;
    _trapFrame->CS = _process ? ((SEG_UCODE << 3) | DPL_USER) : (SEG_KCODE << 3);
    _trapFrame->DS = _process ? ((SEG_UDATA << 3) | DPL_USER) : (SEG_KDATA << 3);
    _trapFrame->ES = _trapFrame->DS;
    _trapFrame->EFlags = _process ? FL_IF : TestGetEflag();
    // These two are only used in userspace processes. KernelThreadEntryPoint will strip them out accordingly.
    _trapFrame->ESP = UInt32(_stackInProcess) + CURRENT_PAGE_SIZE - usedStack;
    _trapFrame->SS = _trapFrame->DS;
    // Set up entrypoint
    _trapFrame->EIP = _process ? UInt32(entryPoint) : UInt32(KernelThreadEntryPoint);   // Upon exiting the (pretend) trap, start the thread
    
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
    if (_process) {
        _process->pageDirectory.Unmap(_stackInProcess);
        _process->Release();
        CPhysicalMemory::ReleasePages(_processStack);
    }
    delete[] _kernelStack;
    _kernelStorage->Release();
}

void Thread::Kill(void)
{
    Detach();
    // TODO: Multiprocessor aware
    if (Thread::Active == this)
        Scheduler::EnterFromInterrupt();
}

void Thread::Select(CPU::Context **scheduler)
{
    if (Process::Active != _process) {
        if (_process) {
            CPU::Active->InitTSS(_kernelStack, 4096);
            _process->pageDirectory.Select();
            Process::Active = _process;
        }
    }
    Thread::Active = this;
    test('^');
    _context->SwitchFrom(scheduler);
    test('?');
    Thread::Active = NULL;
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
    interruptSource->ConfigureSyscall(SERVICE_THREAD);
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
    AddRef();   // Keep a reference to ourselves, while we're runnable
    _next = NULL;
    _last = last;
    if (last)
        last->_next = this;
    else
        first = this;
    last = this;
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
    Release();   // No longer runnable, so releasable
}

Thread *Thread::first = NULL;
Thread *Thread::last = NULL;
Thread *Thread::cursor = NULL;

KernelThread::KernelThread(UInt32 stackSize)
:Thread(NULL, KernelThread::Thunk, this, stackSize)
{
}
void KernelThread::Thunk(void *context)
{
    ((KernelThread*)context)->ThreadMain();
}
