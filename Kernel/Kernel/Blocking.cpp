#include "Blocking.h"
#include "Collections.h"
#include "Locking.h"

BlockableObject::BlockableObject()
:_locker(NULL)
{
    _watchers = new KernelArray();
    _signals = new KernelArray();
    _signalled = false;
    _selfSignal = false;
}

BlockableObject::~BlockableObject()
{
    _signals->Release();
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

void BlockableObject::SetSignalled(BlockableObject *sender, bool active)
{
    GenericLock::Autolock locker(&_locker);
    if (sender == this) {
        // "this" is sadly a special case, to avoid retain loops
        if (_selfSignal == active)
            return;
        _selfSignal = active;
    } else {
        bool contains = _signals->Contains(sender);
        if (active) {
            if (contains)
                return;
            _signals->Add(sender);
        } else {
            if (!contains)
                return;
            _signals->Remove(sender);
        }
    }
    DoSignal(CheckSignal());
}

void BlockableObject::DoSignal(bool active)
{
    if (active == _signalled)
        return;
    AutoreleasePool pool;
    _signalled = active;
    _watchers->Enumerate([this, active](KernelObject *object){
        SignalWatcher *watcher = (SignalWatcher*)object;
        watcher->SignalChanged(this, active);
        return (void*)NULL;
    });
}

KernelArray* BlockableObject::CurrentSignals(void)
{
    KernelArray *output = new KernelArray();
    output->Add(this);  // We're signalled, so we should include ourselves, even if we're not in the list
    _signals->Enumerate([this, output](KernelObject *signal){
        BlockableObject *blockingSignal = (BlockableObject*)signal;
        if (blockingSignal != this)
            output->AddArray(blockingSignal->CurrentSignals());
        return (BlockableObject*)NULL;
    });
    output->Autorelease();
    return output;
}

bool BlockableObject::IsSignalled(void)
{
    return _signalled;
}

KernelArray* BlockableObject::ThisCurrentSignals(void)
{
    KernelArray *result = new KernelArray();
    result->AddArray(_signals);
    if (_selfSignal)
        result->Add(this);
    result->Autorelease();
    return result;
}

bool BlockableObject::CheckSignal(void)
{
    return (_signals->Count() + (_selfSignal ? 1 : 0)) != 0;
}

ListSignalWatcher::ListSignalWatcher()
{
    _sources = new KernelArray();
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
        SetSignalled(source, source->IsSignalled());
    }
}

void ListSignalWatcher::RemoveSource(BlockableObject *source)
{
    GenericLock::Autolock locker(&_locker);
    if (Sources()->Contains(source)) {
        SetSignalled(source, false);
        source->UnregisterObserver(this);
        Sources()->Remove(source);
    }
}

void ListSignalWatcher::SignalChanged(BlockableObject *watching, bool active)
{
    // Base class provides empty implementation for convenience, no need to call it
    SetSignalled(watching, active);
}

SignalOr::SignalOr()
{
}

bool SignalOr::CheckSignal(void)
{
    return ThisCurrentSignals()->Count() != 0;
}

SignalAnd::SignalAnd()
{
}

bool SignalAnd::CheckSignal(void)
{
    return ThisCurrentSignals()->Count() == Sources()->Count();
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
            timer->SetSignalled(timer, true);
            if (timer->_repeats) {
                timer->SetSignalled(timer, false);
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
    if (IsSignalled())
        SetSignalled(this, false);
}

SimpleSignal::SimpleSignal(bool activeLow)
{
    _activeLow = activeLow;
    _count = 0;
}

void SimpleSignal::Set(void)
{
    if (!(_count++))
        SetSignalled(this, CurrentlySignalled());
}

void SimpleSignal::Reset(void)
{
    if (_count == 0)
        return;
    if (!(--_count))
        SetSignalled(this, CurrentlySignalled());
}

bool SimpleSignal::CurrentlySignalled(void)
{
    if (_activeLow)
        return _count == 0;
    else
        return _count != 0;
}
