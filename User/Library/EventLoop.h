#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include "Blocking.h"
#include "Library.h"

class EventLoop
{
public:
    typedef UInt64 Token;
    typedef Library::Function<void(Kernel::Blocking::Blockable *trigger, Kernel::Collections::Array *others)> Handler;
    
    EventLoop();
    ~EventLoop();
    
    Token AddSource(Kernel::Blocking::Blockable *source, Handler handler);
    void RemoveSource(Token source);
    
    void AddTask(Library::Function<void(void)> handler);
    
    void Run(void);
    void Stop(void);
    
private:
    Kernel::Blocking::List *_waiting;
    Kernel::Blocking::Simple *_stop;
    Library::Dictionary<Kernel::Blocking::Blockable*, Handler> _sources;
    bool _stopped;
};

#endif // __EVENTLOOP_H__
