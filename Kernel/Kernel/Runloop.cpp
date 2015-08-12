#include "Runloop.h"
#include "Collections.h"
#include "Thread.h"

typedef KernelFunction<int(BlockableObject *watching, KernelArray *signals)> RunloopFunction;

Runloop::Runloop()
{
    _running = false;
    _sourceList = new SignalOr();
    _sourceMap = new KernelDictionary();
    _stop = new SimpleSignal(false);
    
    // Initialise the list with the stop signal
    _sourceList->AddSource(_stop);
}

Runloop::~Runloop()
{
    _sourceMap->Release();
    _sourceList->Release();
    _stop->Release();
}

void Runloop::AddSource(SelfHandlingBlockableObject *blocker)
{
    AddSource(blocker, [this, blocker](BlockableObject *watching, KernelArray *signals){
        blocker->ProcessSignal(this, watching, signals);
        return 0;
    });
}

void Runloop::AddSource(BlockableObject *blocker, bicycle::function<int(BlockableObject *watching, KernelArray *signals)> handler)
{
    RunloopFunction *function = new RunloopFunction(handler);
    _sourceMap->Set(blocker, function);
    function->Release();
    _sourceList->AddSource(blocker);
}

void Runloop::RemoveSource(BlockableObject *blocker)
{
    _sourceList->RemoveSource(blocker);
    _sourceMap->Set(blocker, NULL);
}

void Runloop::Run(void)
{
    AutoreleasePool pool;
    
    AddRef();
    while (true) {
        pool.Flush();
        KernelArray *result = Thread::Active->BlockOn(_sourceList);
        if (result && result->Contains(_stop))
            break;
        BlockableObject *blocker = NULL;
        RunloopFunction *function = (RunloopFunction*)result->Enumerate([this, &blocker](KernelObject *blockerObject){
            blocker = (BlockableObject*)blockerObject;
            return _sourceMap->ObjectFor(blocker);
        });
        if (function == NULL) {
            kprintf("%.8x: Failed to find function\n", this);
            continue;
        }
        function->Pointer()(blocker, result);
    }
    _stop->Reset();
    Release();
}

void Runloop::Stop(void)
{
    _stop->Set();
}

typedef KernelFunction<int(void)> TaskQueueFunction;

TaskQueue::TaskQueue()
:_taskLock("TaskQueue")
{
    _tasks = new KernelFIFO();
}

TaskQueue::~TaskQueue()
{
    _tasks->Release();
}

void TaskQueue::AddTask(bicycle::function<int(void)> task)
{
    TaskQueueFunction *function = new TaskQueueFunction(task);
    {
        GenericLock::Autolock locker(&_taskLock);
        _tasks->Push(function);
        SetSignalled(this, true);
    }
    function->Release();
}

void TaskQueue::ProcessSignal(Runloop *runloop, BlockableObject *watching, KernelArray *signals)
{
    GenericLock::Autolock locker(&_taskLock);
    TaskQueueFunction *task = (TaskQueueFunction*)_tasks->Pop();
    if (task == NULL)
        SetSignalled(this, false);
    else
        task->Pointer()();
}

RunloopThread::RunloopThread(Process *owner)
{
    _tasks = new TaskQueue();
    AddSource(_tasks);
    _thread = new Thread(owner, WorkThread, this, Thread::tmKernel);
}

RunloopThread::~RunloopThread()
{
    _thread->Kill();
    _thread->Release();
    _tasks->Release();
}

void RunloopThread::WorkThread(void *context)
{
    ((RunloopThread*)context)->Run();
}
