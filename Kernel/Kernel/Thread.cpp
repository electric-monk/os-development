#include "Thread.h"
#include "debug.h"
#include "Interrupts.h"
#include "StandardPC_traps.h"
#include "Scheduler.h"
#include "Process.h"
#include "mem_logical.h"
#include "mem_physical.h"
#include "Collections.h"
#include "CPU_intr.h"
#include "mem_virtual.h"

#define KERNEL_STACK_PAGES          2

// This pointer points into the guts of the interrupt handler - the part that returns from an interrupt
extern "C" void trapret(void);

// This pointer points to an assembler routine to configure the callback
extern "C" void ThreadEntryPoint(void);
extern "C" void KernelThreadEntryPoint(void);

Thread::Thread(Process *process, void (*entryPoint)(void*), void *context, ThreadMode mode, UInt32 stackSize)
{
    _state = tsRunnable;
    _blockingObject = NULL;
    _blockingResult = NULL;
    
    if (mode == tmAuto) {
        // Select actual mode
        mode = (process == NULL) ? tmKernel : tmUserspace;
    } else if ((mode == tmUserspace) && (process == NULL)) {
        // TODO: Error state: requested userspace, no process
    }
    bool isUserspace = mode == tmUserspace;
    if (!isUserspace && (mode != tmKernel)) {
        // TODO: Error state: nonsense value in mode
    }
    
    _kernelStorage = new KernelDictionary();
    int kernelStackSize = KERNEL_STACK_PAGES;
    int userStackSize = stackSize / PAGE_SIZE;
    if (!isUserspace)
        kernelStackSize += userStackSize;
    _kernelStackLength = kernelStackSize * PAGE_SIZE;
    _kernelStack = new char[_kernelStackLength];
    
    // Make a note of the current process (if any - NULL is kernel thread)
    _process = process;
    if (_process)
        _process->AddRef();
    if (isUserspace) {
        // Process stack needs to exist in process space
        _stackInProcess = new GrowableStack(_process, userStackSize);
    } else {
        _stackInProcess = NULL;
    }
    
    // Configure initial stack
    char *stackPointer = _kernelStack + _kernelStackLength;
    int usedStack = 0;
    
    // For ThreadEntryPoint - first context, second function
    if (isUserspace) {
        AutoreleasePool pool;
        VirtualMemory *userStack = _stackInProcess->MapIntoKernel();
        char *userStackTop = (char*)userStack->LinearBase() + userStack->LinearLength();
        userStackTop -= 4;
        *((void**)userStackTop) = (void*)context;
        usedStack += 4;
    } else {
        stackPointer -= 4;
        *((void**)stackPointer) = (void*)context;
        stackPointer -= 4;
        *((void**)stackPointer) = (void*)entryPoint;
    }
    
    // Create trap frame
    stackPointer -= sizeof(TrapFrame);
    _trapFrame = (TrapFrame*)stackPointer;
    ClearMemory(_trapFrame, sizeof(TrapFrame));
    _trapFrame->CS = isUserspace ? ((SEG_UCODE << 3) | DPL_USER) : (SEG_KCODE << 3);
    _trapFrame->DS = isUserspace ? ((SEG_UDATA << 3) | DPL_USER) : (SEG_KDATA << 3);
    _trapFrame->ES = _trapFrame->DS;
    _trapFrame->EFlags = isUserspace ? FL_IF : GetEflag();
    _trapFrame->GS = isUserspace ? 0 : (SEG_KCPU << 3);    // Set up the TLS
    // These two are only used in userspace processes. KernelThreadEntryPoint will strip them out accordingly.
    _trapFrame->ESP = UInt32(isUserspace ? _stackInProcess->StackTop() : 0) - usedStack;
    _trapFrame->SS = _trapFrame->DS;
    // Set up entrypoint
    _trapFrame->EIP = isUserspace ? UInt32(entryPoint) : UInt32(KernelThreadEntryPoint);   // Upon exiting the (pretend) trap, start the thread
    
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
    BlockOn(NULL);
    if (_stackInProcess)
        _stackInProcess->Release();
    if (_process)
        _process->Release();
    delete[] _kernelStack;
    _kernelStorage->Release();
}

void Thread::Kill(void)
{
    Detach();
    // TODO: Multiprocessor aware
    if (Thread::Active == this) {
        InterruptDisabler disabler;
        Scheduler::EnterFromInterrupt();
    }
}

KernelArray* Thread::BlockOn(BlockableObject *source)
{
//    kprintf("Thread %.8x blocking on %.8x\n", this, source);
    // Release any previous object
    if (_blockingObject) {
        _blockingObject->UnregisterObserver(this);
        _blockingObject->Release();
        _blockingObject = NULL;
        _state = tsRunnable;
    }
    if (_blockingResult) {
        _blockingResult->Release();
        _blockingResult = NULL;
    }
    // If the new object is signalled, don't do anything
    if (!source)
        return NULL;
    if (source->IsSignalled())
        return source->CurrentSignals();
    // Start blocking
    _blockingObject = source;
    _blockingObject->AddRef();
    _blockingObject->RegisterObserver(this);
    _state = tsBlocked;
    // TODO: Multiprocessor?
    if (Thread::Active == this) {
        // TODO: Disable interrupt earlier?
        {
            InterruptDisabler disabler;
            Scheduler::EnterFromInterrupt();
        }
        KernelArray *result = _blockingResult;
        if (result) {
            _blockingResult = NULL;
            result->Autorelease();
        }
        return result;
    }
    return NULL;
}

void Thread::SignalChanged(BlockableObject *watching, bool active)
{
//    kprintf("Thread %.8x got signal %.8x\n", this, signal);
    if (active) {
        // Make a note of who set us off again
        _blockingResult = watching->CurrentSignals();
        _blockingResult->AddRef();
        // Stop blocking
        _blockingObject->UnregisterObserver(this);
        _blockingObject->Release();
        _blockingObject = NULL;
        // Resume running
        _state = tsRunnable;
    }
}

void Thread::Sleep(UInt32 microseconds)
{
    AutoreleasePool pool;
    Timer *timer = new Timer();
    timer->Reset(microseconds, false);
    BlockOn(timer);
    timer->Release();
}

void Thread::Select(CPU::Context **scheduler)
{
    if (_process && _stackInProcess)
        CPU::Active->InitTSS(_kernelStack, _kernelStackLength);
    if (Process::Active != _process) {
        if (_process) {
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

void Thread::DebugPrint(void)
{
    InterruptDisabler disabler;
    kprintf("Threads:\n");
    int i = 0;
    for (Thread *cursor = first; cursor != NULL; cursor = cursor->_next, i++) {
        kprintf("\t%i: %s %x [%x] ", i, cursor->_stackInProcess ? "User" : "Kernel", cursor, cursor->_process);
        switch (cursor->_state) {
            case tsBlocked:
                kprintf("Blocked: %x", cursor->_blockingObject);
                break;
            case tsCompleted:
                kprintf("Completed");
                break;
            case tsRunnable:
                kprintf("Runnable");
                break;
            case tsRunning:
                kprintf("Running");
                break;
            default:
                kprintf("Unknown: %i", cursor->_state);
                break;
        }
        kprintf("\n");
    }
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
:Thread(NULL, KernelThread::Thunk, this, tmKernel, stackSize)
{
}
void KernelThread::Thunk(void *context)
{
    ((KernelThread*)context)->ThreadMain();
}
