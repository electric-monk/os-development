#ifndef __BLOCKING_H__
#define __BLOCKING_H__

#include "KernelObject.h"
#include "Locking.h"

class SignalWatcher;
class KernelArray;

class BlockableObject : public KernelObject
{
public:
    BlockableObject();
    
    // Register/unregister state
    virtual void RegisterObserver(SignalWatcher *watcher);
    virtual void UnregisterObserver(SignalWatcher *watcher);
    
    // Current state
    virtual BlockableObject* Signalled(void);
    
protected:
    ~BlockableObject();
    
    void SignalFor(BlockableObject *sender);
    
    HardcoreSpinLock _locker;
    
private:
    KernelArray *_watchers;
    BlockableObject *_currentSignal;
};

// The signal watcher is derived from a blockable object to allow objects that do both - many objects (e.g. interrupts, timers) won't ever watch anything, but plenty (e.g. threads and processes) will do both
class SignalWatcher : public BlockableObject
{
public:
    // Actually sensible entry point, that indicates both the root of the tree and the branch that triggered it
    virtual void SignalChanged(BlockableObject *root, BlockableObject *source)
    {
        SignalChanged(source);
    }
protected:
    // TODO: Deprecate this
    virtual void SignalChanged(BlockableObject *source)
    {
        // Legacy implementation, so overrisers of SignalChanged(,) don't need to worry about this
    }
};

class ListSignalWatcher : public SignalWatcher
{
public:
    ListSignalWatcher();
    
    virtual void AddSource(BlockableObject *source);
    virtual void RemoveSource(BlockableObject *source);
    
    void SignalChanged(BlockableObject *source);
    
protected:
    ~ListSignalWatcher();
    
    KernelArray* Sources(void) { return _sources; }
    
    UInt32 Count(void) { return _triggerCount; }
    
private:
    KernelArray *_sources;
    UInt32 _triggerCount;
};

class SignalOr : public ListSignalWatcher
{
public:
    SignalOr();
    
    void SignalChanged(BlockableObject *source);
};

class SignalAnd : public ListSignalWatcher
{
public:
    SignalAnd();
    
    void SignalChanged(BlockableObject *source);
};

#define MICROSECONDS(x)         (x)
#define MILLISECONDS(x)         (MICROSECONDS(x) * 1000)
#define SECONDS(x)              (MILLISECONDS(x) * 1000)

class Timer : public BlockableObject
{
public:
    static void TimerTick(UInt32 microsecondsPassed);
    Timer();
    
    void Reset(UInt32 microseconds = 0, bool repeats = false);   // Reset non-repeating timer, and/or change timeout, 0 == use previous delay
    void Cancel(void);
    
protected:
    ~Timer();
    
private:
    // Linked list
    Timer *_previous, *_next;
    // Settings
    UInt32 _totalTime;
    bool _repeats;
    // State
    bool _running;
    UInt32 _remainder;
};

class SimpleSignal : public BlockableObject
{
public:
    SimpleSignal(bool activeLow);
    
    BlockableObject* Signalled(void);
    
    void Pulse(void) { Set(); Reset(); }
    void Set(void);
    void Reset(void);
    
private:
    bool _activeLow;
    UInt32 _count;
};

#endif // __BLOCKING_H__
