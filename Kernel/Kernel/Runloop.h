#ifndef __RUNLOOP_H__
#define __RUNLOOP_H__

#include "Blocking.h"

class KernelDictionary;
class KernelFIFO;
class Thread;
class Process;

class Runloop : public KernelObject
{
public:
    CLASSNAME(KernelObject, Runloop);
    
    class SelfHandlingBlockableObject : public BlockableObject
    {
    public:
        CLASSNAME(BlockableObject, Runloop::SelfHandlingBlockableObject);
        
        virtual void ProcessSignal(Runloop *runloop, BlockableObject *watching, KernelArray *signals) = 0;
    };
    
public:
    Runloop();
    
    void AddSource(SelfHandlingBlockableObject *blocker);
    void AddSource(BlockableObject *blocker, bicycle::function<int(BlockableObject *watching, KernelArray *signals)> handler);
    void RemoveSource(BlockableObject *blocker);
    
    void Run(void);     // Blocks until stopped
    void Stop(void);
    
    void Sync(bicycle::function<int(void)> synchronousCode);
    
protected:
    ~Runloop();
    
private:
    SignalOr *_sourceList;
    KernelDictionary *_sourceMap;
    SimpleSignal *_stop;
    bool _running;
};

/* This is just DispatchQueue but for runloops */
class TaskQueue : public Runloop::SelfHandlingBlockableObject
{
public:
    CLASSNAME(Runloop::SelfHandlingBlockableObject, TaskQueue);
    
    TaskQueue();
    
    void AddTask(bicycle::function<int(void)> task);
    
protected:
    ~TaskQueue();

    void ProcessSignal(Runloop *runloop, BlockableObject *watching, KernelArray *signals);
    
private:
    HardcoreSpinLock _taskLock;
    KernelFIFO *_tasks;
};

/* Lie DispatchQueue with a thread for convenience */
class RunloopThread : public Runloop
{
public:
    CLASSNAME(Runloop, RunloopThread);
    
    RunloopThread(Process *owner = NULL);
    
    // For convenience, a task queue is also provided
    void AddTask(bicycle::function<int(void)> task) { _tasks->AddTask(task); }
    TaskQueue* Queue(void) { return _tasks; }
    
protected:
    ~RunloopThread();
    
private:
    Thread *_thread;
    TaskQueue *_tasks;
    
    static void WorkThread(void*);
};

#endif // __RUNLOOP_H__
