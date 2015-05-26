#include "Runloop.h"
#include "Collections.h"
#include "Thread.h"

typedef KernelFunction<int(BlockableObject *signal, BlockableObject *trigger)> RunloopFunction;

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
    AddSource(blocker, [this, blocker](BlockableObject *signal, BlockableObject *trigger){
        blocker->ProcessSignal(this, signal, trigger);
        return 0;
    });
}

void Runloop::AddSource(BlockableObject *blocker, bicycle::function<int(BlockableObject *signal, BlockableObject *trigger)> handler)
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
        BlockableObject *result = Thread::Active->BlockOn(_sourceList);
        if (result == _stop)
            break;
        RunloopFunction *function = (RunloopFunction*)_sourceMap->ObjectFor(result);
        function->Pointer()(result, result->Signalled());
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
        if (!Signalled())
            SignalFor(this);
    }
    function->Release();
}

void TaskQueue::ProcessSignal(Runloop *runloop, BlockableObject *root, BlockableObject *source)
{
    GenericLock::Autolock locker(&_taskLock);
    TaskQueueFunction *task = (TaskQueueFunction*)_tasks->Pop();
    if (task == NULL)
        SignalFor(NULL);
    else
        task->Pointer()();
}

RunloopThread::RunloopThread()
{
    _tasks = new TaskQueue();
    AddSource(_tasks);
    _thread = new Thread(NULL, WorkThread, this);
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
