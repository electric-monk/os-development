#include "Blocking.h"
#include "Collections.h"
#include "Locking.h"

BlockableObject::BlockableObject()
:_locker(NULL)
{
    _watchers = new KernelArray();
    _currentSignal = NULL;
}

BlockableObject::~BlockableObject()
{
    _watchers->Release();
}

void BlockableObject::RegisterObserver(SignalWatcher *watcher)
{
    GenericLock::Autolock locker(&_locker);
    if (!_watchers->Contains(watcher))
        _watchers->Add(watcher);
}

void BlockableObject::UnregisterObserver(SignalWatcher *watcher)
{
    GenericLock::Autolock locker(&_locker);
    _watchers->Remove(watcher);
}

BlockableObject* BlockableObject::Signalled(void)
{
    return _currentSignal;
}

void BlockableObject::SignalFor(BlockableObject *sender)
{
    GenericLock::Autolock locker(&_locker);
    _currentSignal = sender;    // weak ref
    for (UInt32 i = 0; i < _watchers->Count(); i++)
        ((SignalWatcher*)_watchers->ObjectAt(i))->SignalChanged(sender);
}

ListSignalWatcher::ListSignalWatcher()
{
    _sources = new KernelArray();
    _triggerCount = 0;
}

ListSignalWatcher::~ListSignalWatcher()
{
    _sources->Release();
}

void ListSignalWatcher::AddSource(BlockableObject *source)
{
    GenericLock::Autolock locker(&_locker);
    if (!Sources()->Contains(source)) {
        Sources()->Add(source);
        source->RegisterObserver(this);
        if (source->Signalled()) {
            _triggerCount++;
            if (_triggerCount == 1)
                SignalFor(source);
        }
    }
}

void ListSignalWatcher::RemoveSource(BlockableObject *source)
{
    GenericLock::Autolock locker(&_locker);
    if (Sources()->Contains(source)) {
        if (source->Signalled()) {
            _triggerCount--;
            if (_triggerCount == 0)
                SignalFor(NULL);
        }
        source->UnregisterObserver(this);
        Sources()->Remove(source);
    }
}

void ListSignalWatcher::SignalChanged(BlockableObject *source)
{
    if (source)
        _triggerCount++;
    else
        _triggerCount--;
    // Derived class should do something
}

SignalOr::SignalOr()
{
}

void SignalOr::SignalChanged(BlockableObject *source)
{
    bool wasIdle = Count() == 0;
    ListSignalWatcher::SignalChanged(source);
    bool isIdle = Count() == 0;
    if (wasIdle && !isIdle)
        SignalFor(source);
    else if (!wasIdle && isIdle)
        SignalFor(NULL);
}

SignalAnd::SignalAnd()
{
}

void SignalAnd::SignalChanged(BlockableObject *source)
{
    bool wasActive = Count() == Sources()->Count();
    ListSignalWatcher::SignalChanged(source);
    bool isActive = Count() == Sources()->Count();
    if (!wasActive && isActive)
        SignalFor(this);
    else if (wasActive && !isActive)
        SignalFor(NULL);
}

HardcoreSpinLock s_lock(NULL);
Timer *s_root = NULL;

void Timer::TimerTick(UInt32 microsecondsPassed)
{
    GenericLock::Autolock locker(&s_lock);  // Should be made faster?
    for (Timer *timer = s_root; timer != NULL; timer = timer->_next) {
        if (!timer->_running)
            continue;
        if (microsecondsPassed > timer->_remainder) {
            // Firing timer
            timer->SignalFor(timer);
            if (timer->_repeats) {
                timer->SignalFor(NULL);
                timer->_remainder = timer->_totalTime;
            } else {
                timer->_running = false;
            }
        } else {
            timer->_remainder -= microsecondsPassed;
        }
    }
}

Timer::Timer()
{
    _running = false;
    _totalTime = 0;
    
    GenericLock::Autolock locker(&s_lock);
    _previous = NULL;
    _next = s_root;
    if (_next)
        _next->_previous = this;
    s_root = this;
}

Timer::~Timer()
{
    GenericLock::Autolock locker(&s_lock);
    if (_previous)
        _previous->_next = _next;
    if (_next)
        _next->_previous = _previous;
    if (s_root == this)
        s_root = _next; // Should always be next
}

void Timer::Reset(UInt32 delay, bool repeats)
{
    Cancel();
    if (delay != 0) {
        _totalTime = delay;
        _repeats = repeats;
    }
    if (_totalTime != 0) {
        _remainder = _totalTime;
        _running = true;
    }
}

void Timer::Cancel(void)
{
    _running = false;
    if (Signalled())
        SignalFor(NULL);
}

SimpleSignal::SimpleSignal(bool activeLow)
{
    _activeLow = activeLow;
    _count = 0;
}

void SimpleSignal::Set(void)
{
    if (!(_count++))
        SignalFor(Signalled());
}

void SimpleSignal::Reset(void)
{
    if (_count == 0)
        return;
    if (!(--_count))
        SignalFor(Signalled());
}

BlockableObject* SimpleSignal::Signalled(void)
{
    if (_activeLow)
        return _count ? NULL : this;
    else
        return _count ? this : NULL;
}
