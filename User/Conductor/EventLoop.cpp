#include "EventLoop.h"
#include "Collections.h"

EventLoop::EventLoop()
{
    _waiting = Kernel::Blocking::List::CreateOr();
    _stop = Kernel::Blocking::Simple::Create(false);
    _stopped = true;
    _stop->Set(true);
    _waiting->Add(_stop);
}

EventLoop::~EventLoop()
{
    _stop->Release();
    _waiting->Release();
}

EventLoop::Token EventLoop::AddSource(Kernel::Blocking::Blockable *source, Handler handler)
{
    _waiting->Add(source);
    _sources[source] = handler;
    return Token(source);
}

void EventLoop::RemoveSource(Token source)
{
    Kernel::Blocking::Blockable *blockable = (Kernel::Blocking::Blockable*)source;
    _waiting->Remove(blockable);
    _sources.Reset(blockable);
}

void EventLoop::AddTask(Library::Function<void(void)> handler)
{
    // TODO: this properly
    Kernel::Blocking::Simple *signal = Kernel::Blocking::Simple::Create(false);
    _sources[signal] = [=](Kernel::Blocking::Blockable *trigger, Kernel::Collections::Array *others){
        _waiting->Remove(signal);
        _sources.Reset(signal);
        handler();
    };
    _waiting->Add(signal);
    // finally...
    signal->Set(true);
}
extern "C" void testprint(const char*);

void EventLoop::Run(void)
{
    bool stop = false;
    _stop->Set(false);
    _stopped = false;
    while(!stop) {
        Kernel::Collections::Array *result = _waiting->Block();
        UInt32 count = result->Count();
        for(UInt32 i = 0; i < count; i++) {
            Kernel::Blocking::Blockable *item = (Kernel::Blocking::Blockable*)result->ObjectAt(i);
            if (item == _stop) {
                stop = true;
            } else {
                Handler s = _sources[item];
                s(item, result);
            }
        }
        result->Release();
    }
}

void EventLoop::Stop(void)
{
    if (!_stopped) {
        _stopped = true;
        _stop->Set(true);
    }
}
