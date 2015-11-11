#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "Blocking.h"
#include "Locking.h"

class Thread;
class KernelFIFO;
class DispatchGroupHolder;

/************
 * Dispatch group
 *
 * This object allows items to be executed serially, as they attempt to access the group. The thread will block (or timeout) until their turn, and must explicitly exit. A convenience class to do this is provided.
 */
class DispatchGroup : public KernelObject
{
public:
    CLASSNAME(KernelObject, DispatchGroup);
    
    DispatchGroup();
    
    void* EnterOrTimeout(UInt32 microsecondsPassed = 0);    // If successful, returns a token
    void Exit(void *token);
    
    class Block
    {
    public:
        Block(DispatchGroup *group, UInt32 microsecondsPassed = 0)
        {
            _group = group;
            _group->AddRef();
            _token = _group->EnterOrTimeout(microsecondsPassed);
        }
        ~Block()
        {
            _group->Exit(_token);
            _group->Release();
        }
        bool Valid(void)
        {
            return _token != NULL;
        }
    private:
        DispatchGroup *_group;
        void *_token;
    };
    
protected:
    ~DispatchGroup();
    
private:
    HardcoreSpinLock _lock;
    DispatchGroupHolder *_start, *_end;
    DispatchGroupHolder *_current;
};

/************
 * Task queue
 *
 * This object allows you to stick a task on the end. Since lambdas probably won't work here (need to check that) and such, they're little inconvenient objects you need to use.
 */
class DispatchQueue : public BlockableObject
{
public:
    CLASSNAME(BlockableObject, DispatchQueue);
    
    // You may block on these tasks to wait until they're completed
    class Task : public BlockableObject
    {
    public:
        CLASSNAME(BlockableObject, DispatchQueue::Task);
        
        Task();
        
        void Run(void);

    protected:
        ~Task();
        virtual void Execute(void) = 0;
    };
    
    DispatchQueue();
    
    void AddTask(Task *task);
    void AddTask(bicycle::function<int(void)> task);
    
protected:
    ~DispatchQueue();
    
private:
    HardcoreSpinLock _taskLock;
    KernelFIFO *_tasks;
    Thread *_thread;
    SimpleSignal *_threadEnd;
    
    static void WorkThread(void*);
};

#endif // __QUEUE_H__
