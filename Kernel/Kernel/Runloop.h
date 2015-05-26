#ifndef __RUNLOOP_H__
#define __RUNLOOP_H__

#include "Blocking.h"

class KernelDictionary;
class KernelFIFO;
class Thread;

class Runloop : public KernelObject
{
public:
    class SelfHandlingBlockableObject : public BlockableObject
    {
    public:
        virtual void ProcessSignal(Runloop *runloop, BlockableObject *root, BlockableObject *source) = 0;
    };
    
public:
    Runloop();
    
    void AddSource(SelfHandlingBlockableObject *blocker);
    void AddSource(BlockableObject *blocker, bicycle::function<int(BlockableObject *signal, BlockableObject *trigger)> handler);
    void RemoveSource(BlockableObject *blocker);
    
    void Run(void);     // Blocks until stopped
    void Stop(void);
    
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
    TaskQueue();
    
    void AddTask(bicycle::function<int(void)> task);
    
protected:
    ~TaskQueue();

    void ProcessSignal(Runloop *runloop, BlockableObject *root, BlockableObject *source);
    
private:
    HardcoreSpinLock _taskLock;
    KernelFIFO *_tasks;
};

/* Lie DispatchQueue with a thread for convenience */
class RunloopThread : public Runloop
{
public:
    RunloopThread();
    
    // For convenience, a task queue is also provided
    void AddTask(bicycle::function<int(void)> task) { _tasks->AddTask(task); }
    
protected:
    ~RunloopThread();
    
private:
    Thread *_thread;
    TaskQueue *_tasks;
    
    static void WorkThread(void*);
};

#endif // __RUNLOOP_H__
