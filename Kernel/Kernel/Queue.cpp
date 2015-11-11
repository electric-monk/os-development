#include "Queue.h"
#include "Collections.h"
#include "Thread.h"
#include "debug.h"

class DispatchGroupHolder : public BlockableObject
{
public:
    CLASSNAME(BlockableObject, DispatchGroupHolder);
    
    DispatchGroupHolder(DispatchGroupHolder **start, DispatchGroupHolder **end)
    {
        _start = start;
        _end = end;
        _last = *_end;
        if (_last)
            _last->_next = this;
        else
            *_start = this;
        *_end = this;
        _next = NULL;
    }
    
    BlockableObject* AfterThese(void)
    {
        if (_last == NULL)
            return NULL;
        SignalAnd *allBefore = new SignalAnd();
        for (DispatchGroupHolder *holder = _last; holder != NULL; holder = holder->_last)
            if (!holder->IsSignalled())
                allBefore->AddSource(holder);
        return allBefore;
    }
    
    void Completed(void)
    {
        SetSignalled(this, true);
        Release();
    }
    
protected:
    ~DispatchGroupHolder()
    {
        if (_last != NULL)
            _last->_next = _next;
        if (_next != NULL)
            _next->_last = _last;
        if (*_start == this)
            *_start = _next;
        if (*_end == this)
            *_end = _last;
    }
    
private:
    DispatchGroupHolder **_start, **_end;
    DispatchGroupHolder *_last, *_next;
};

DispatchGroup::DispatchGroup()
:_lock("DispatchGroup")
{
    _start = NULL;
    _end = NULL;
    _current = NULL;
}

DispatchGroup::~DispatchGroup()
{
    _current = NULL;
    while (_start != NULL)
        _start->Release();
}

void* DispatchGroup::EnterOrTimeout(UInt32 microsecondsPassed)
{
    DispatchGroupHolder *holder;
    BlockableObject *waitFor;
    
    // Create our object, and find who to wait on
    {
        GenericLock::Autolock locker(&_lock);
        holder = new DispatchGroupHolder(&_start, &_end);
        waitFor = holder->AfterThese();
    }
    // If we need to wait for the group, wait now
    if (waitFor != NULL) {
        // Work out who to wait on (the object itself, or a timer as well)
        BlockableObject *waiting;
        if (microsecondsPassed != 0) {
            SignalOr *waitingOn = new SignalOr;
            Timer *sleeping = new Timer();
            sleeping->Reset(microsecondsPassed, false);
            waitingOn->AddSource(sleeping);
            sleeping->Release();
            waitingOn->AddSource(waitFor);
            waitFor->Release();
            waiting = waitingOn;
        } else {
            waiting = waitFor;
        }
        // Our own block is now running
        KernelArray *result = Thread::Active->BlockOn(waiting);
        if ((result == NULL) || !result->Contains(waitFor)) {
            GenericLock::Autolock locker(&_lock);
            // If we're here, we failed to wait
            holder->Completed();    // Just in case - someone may already have started waiting on us
            holder = NULL;
        }
        waiting->Release();
    }
    // Done
    return holder;
}

void DispatchGroup::Exit(void *token)
{
    if (token == NULL)
        return;
    GenericLock::Autolock locker(&_lock);
    DispatchGroupHolder *holder = (DispatchGroupHolder*)token;
    holder->Completed();
}

DispatchQueue::Task::Task()
{
    // No initialisation required
//    kprintf("Task create\n");
}

DispatchQueue::Task::~Task()
{
//    kprintf("Task release\n");
}

void DispatchQueue::Task::Run(void)
{
    // Run the actual task
    Execute();
    // Signal that we're done
    SetSignalled(this, true);
}

DispatchQueue::DispatchQueue()
:_taskLock("DispatchQueue")
{
    _tasks = new KernelFIFO();
    _threadEnd = new SimpleSignal(false);
    _thread = new Thread(NULL, WorkThread, this);
}

DispatchQueue::~DispatchQueue()
{
    _tasks->Release();
    _threadEnd->Set();
    // TODO: Wait for thread to end
    _thread->Kill();
    _thread->Release();
    _threadEnd->Release();
}

void DispatchQueue::AddTask(Task *task)
{
//    kprintf("Queue 0x%.8x adding task 0x%.8x\n", this, task);
    GenericLock::Autolock locker(&_taskLock);
    _tasks->Push(task);
    SetSignalled(task, true);
}

class DispatchQueue_LlambdaTask : public DispatchQueue::Task
{
public:
    CLASSNAME(DispatchQueue::Task, DispatchQueue_LlambdaTask);
    
    DispatchQueue_LlambdaTask(bicycle::function<int(void)> task)
    {
        _function = task;
    }
    
protected:
    void Execute(void)
    {
        _function();
    }
    
private:
    bicycle::function<int(void)> _function;
};

void DispatchQueue::AddTask(bicycle::function<int(void)> task)
{
    DispatchQueue_LlambdaTask *newTask = new DispatchQueue_LlambdaTask(task);
    AddTask(newTask);
    newTask->Release();
}

void DispatchQueue::WorkThread(void *context)
{
    AutoreleasePool pool;
    
    DispatchQueue *that = (DispatchQueue*)context;
    Task *task;
    
    SignalOr *watching = new SignalOr();
    watching->AddSource(that->_threadEnd);
    watching->AddSource(that);
    
//    kprintf("Starting queue 0x%.8x\n", that);
    while (!Thread::Active->BlockOn(watching)->Contains(that->_threadEnd)) {
//        kprintf("Waking up queue 0x%.8x\n", that);
        // Get the next task
        {
            GenericLock::Autolock locker(&that->_taskLock);
            task = (Task*)that->_tasks->Pop();
            if (task == NULL) {
//                kprintf("Sleeping queue 0x%.8x\n", that);
                continue;
            }
            that->SetSignalled(task, false);
        }
        // Run it
//        kprintf("Running task on 0x%.8x: 0x%.8x\n", that, task);
        task->Run();
//        kprintf("Task completed for 0x%.8x\n", that);
        pool.Flush();
    }
//    kprintf("Stopping queue 0x%.8x\n", that);
    
    watching->Release();
}
