#ifndef __BLOCKING_H__
#define __BLOCKING_H__

#include "KernelObject.h"
#include "Locking.h"

class SignalWatcher;
class KernelArray;

class BlockableObject : public KernelObject
{
public:
    static void ConfigureService(void);
    
    BlockableObject();
    
    CLASSNAME(KernelObject, BlockableObject);
    
    // Register/unregister state
    virtual void RegisterObserver(SignalWatcher *watcher);
    virtual void UnregisterObserver(SignalWatcher *watcher);
    
    // Current state
    virtual bool IsSignalled(void);
    virtual KernelArray* CurrentSignals(void);  // Default implementation - all signals, plus their signals
    
protected:
    ~BlockableObject();
    
    KernelArray* ThisCurrentSignals(void);  // Current signals only for this object
    virtual bool CheckSignal(void); // Override this to implement custom signalling logic - default is "any signals mean signalled"
    
    void SetSignalled(BlockableObject *sender, bool active);
    
    HardcoreSpinLock _locker;
    
private:
    KernelArray *_watchers;
    KernelArray *_signals;
    bool _selfSignal;   // Don't add "this" to _signals
    bool _signalled;
    
    void DoSignal(bool active);
};

// The signal watcher is derived from a blockable object to allow objects that do both - many objects (e.g. interrupts, timers) won't ever watch anything, but plenty (e.g. threads and processes) will do both
class SignalWatcher : public BlockableObject
{
public:
    CLASSNAME(BlockableObject, SignalWatcher);
    
    virtual void SignalChanged(BlockableObject *watching, bool active)
    {
        // Nothing by default
    }
};

class ListSignalWatcher : public SignalWatcher
{
public:
    CLASSNAME(SignalWatcher, ListSignalWatcher);
    
    ListSignalWatcher();
    
    virtual void AddSource(BlockableObject *source);
    virtual void RemoveSource(BlockableObject *source);
    
    void SignalChanged(BlockableObject *watching, bool active);
    
protected:
    ~ListSignalWatcher();
    
    KernelArray* Sources(void) { return _sources; }
    
private:
    KernelArray *_sources;
    UInt32 _triggerCount;
};

class SignalOr : public ListSignalWatcher
{
public:
    CLASSNAME(ListSignalWatcher, SignalOr);
    
    SignalOr();

protected:
    bool CheckSignal(void);
};

class SignalAnd : public ListSignalWatcher
{
public:
    CLASSNAME(ListSignalWatcher, SignalAnd);
    
    SignalAnd();
    
protected:
    bool CheckSignal(void);
};

#define MICROSECONDS(x)         (x)
#define MILLISECONDS(x)         (MICROSECONDS(x) * 1000)
#define SECONDS(x)              (MILLISECONDS(x) * 1000)

class Timer : public BlockableObject
{
public:
    CLASSNAME(BlockableObject, Timer);
    
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
    CLASSNAME(BlockableObject, SimpleSignal);
    
    SimpleSignal(bool activeLow);
    
    void Pulse(void) { Set(); Reset(); }
    void Set(void);
    void Reset(void);
    
private:
    bool _activeLow;
    UInt32 _count;
    bool CurrentlySignalled(void);
};

#endif // __BLOCKING_H__
