#include "IPC_Manager.h"
#include "IPC.h"
#include "Process.h"
#include "Runloop.h"

// TODO: A queue or something, to make it safe

namespace IPC_Manager_Internal {
    class Entry
    {
    public:
        typedef enum {
            typeInvalid = 0,
            typeProvider = 1 << 1,
            typeInput = 1 << 2,
            typeOutput = 1 << 3,
            typeInputConnection = 1 << 4,
            typeOutputConnection = 1 << 5,
            typeStart = 0,
            typeStop = 1,
        } Type;
#define TYPE_ONLY(x)        ((::IPC_Manager_Internal::Entry::Type)((x) & ~0x01))
#define TYPE_START(x)       ((::IPC_Manager_Internal::Entry::Type)(TYPE_ONLY(x) | ::IPC_Manager_Internal::Entry::typeStart))
#define TYPE_STOP(x)        ((::IPC_Manager_Internal::Entry::Type)(TYPE_ONLY(x) | ::IPC_Manager_Internal::Entry::typeStop))
#define TYPE_IS_START(x)    (((x) & 0x01) == ::IPC_Manager_Internal::Entry::typeStart)
#define TYPE_IS_STOP(x)     (((x) & 0x01) == ::IPC_Manager_Internal::Entry::typeStop)
        
        static UInt32 Map(KernelObject *object)
        {
            return Process::Mapper()->Map(object);
        }
        
        Entry(Entry **start, Entry **end, UInt32 provider, UInt32 ioPort, UInt32 connection, Type type)
        {
            _type = type;
            _providerHandle = provider;
            _ioHandle = ioPort;
            _connectionHandle = connection;
            _start = start;
            _end = end;
            _next = NULL;
            _last = *_end;
            if (_last)
                _last->_next = this;
            else
                *_start = this;
            *_end = this;
        }
        ~Entry()
        {
            if (_last)
                _last->_next = _next;
            else
                *_start = _next;
            if (_next)
                _next->_last = _last;
            else
                *_end = _last;
        }
        int CancelIfNecessary(void)
        {
            if (!TYPE_IS_STOP(_type))
                return 0;
            Entry *inputStart = NULL;
            Type desiredStart = TYPE_START(_type);
            for (Entry *cursor = _last; cursor != NULL; cursor = cursor->_last) {
                if (cursor->_type != desiredStart)
                    continue;
                if ((cursor->_providerHandle != _providerHandle) || (cursor->_ioHandle != _ioHandle) || (cursor->_connectionHandle != _connectionHandle))
                    continue;
                inputStart = cursor;
                break;
            }
            int erased = 0;
            if (inputStart) {
                // Erase any history of this object, since the buffer contains its entire life
                while (inputStart != this) {
                    Entry *current = inputStart;
                    inputStart = inputStart->_next;
                    if (current->_providerHandle != _providerHandle)
                        continue;
                    if (!(_type & typeProvider)) {
                        if (current->_ioHandle != _ioHandle)
                            continue;
                        if (!(_type & (typeInput | typeOutput))) {
                            if (current->_connectionHandle != _connectionHandle)
                                continue;
                        }
                    }
                    delete current;
                    erased++;
                }
                delete this;
                erased++;
            }
            return erased;
        }
        
        KernelDictionary* GetInfo(void)
        {
            KernelDictionary *dictionary = new KernelDictionary();
            dictionary->Set("State"_ko, State(_type));
            dictionary->Set("Type"_ko, TypeName(_type));
            dictionary->Set("Provider"_ko, Number(_providerHandle));
            if (TYPE_ONLY(_type) != typeProvider)
                dictionary->Set("Connector"_ko, Number(_ioHandle));
            if (TYPE_ONLY(_type) & (typeInputConnection | typeOutputConnection))
                dictionary->Set("Connection"_ko, Number(_connectionHandle));
            dictionary->Autorelease();
            return dictionary;
        }
        
        Type _type;
        UInt32 _providerHandle;
        UInt32 _ioHandle;   // Input or output
        UInt32 _connectionHandle;
        
        // Linked list connection
        Entry **_start, **_end;
        Entry *_last, *_next;
        
    private:
        static KernelNumber* Number(UInt32 handle)
        {
            KernelNumber *num = new KernelNumber(handle);
            num->Autorelease();
            return num;
        }
        static KernelString* State(Type type)
        {
            return TYPE_IS_START(type) ? "Start"_ko : "Stop"_ko;
        }
        static KernelString* TypeName(Type type)
        {
            switch(TYPE_ONLY(type)) {
                case typeProvider:
                    return "Provider"_ko;
                case typeInput:
                    return "Input"_ko;
                case typeOutput:
                    return "Output"_ko;
                case typeInputConnection:
                    return "InputConnection"_ko;
                case typeOutputConnection:
                    return "OutputConnection"_ko;
                default:
                    return "Invalid"_ko;
            }
        }
    };
    
    RunloopThread *s_runloop = NULL;
    
    class State : public KernelObject
    {
    public:
        State()
        {
            _total = 0;
            _start = NULL;
            _end = NULL;
            
            _next = NULL;
            _last = _stateEnd;
            _stateEnd = this;
            if (_last == NULL)
                _stateStart = this;
            else
                _last->_next = this;
        }
        
        static void PerformOnAll(bicycle::function<int(State*)> handler)
        {
            // TODO: lock here?
            for (State *start = _stateStart; start != NULL; start = start->_next)
                handler(start);
        }
        
        void AddEntry(UInt32 provider, UInt32 ioPort, UInt32 connection, Entry::Type type)
        {
            int start = _total;
            Entry *entry = new Entry(&_start, &_end, provider, ioPort, connection, type);
            _total++;   // For new entry
            _total -= entry->CancelIfNecessary();
            if (start != _total)
                onChanged(_total);
        }
        
        void ReadOut(bicycle::function<int(Entry*)> handler, bool reset)
        {
            int start = _total;
            Entry *current = _start;
            while (current) {
                handler(current);
                Entry *next = current->_next;
                if (reset) {
                    delete current;
                    _total--;
                }
                current = next;
            }
            if (start != _total)
                onChanged(_total);
        }
        
        static State* GlobalState(void)
        {
            if (!_stateGlobal)
                _stateGlobal = new State();
            return _stateGlobal;
        }
        
        bicycle::function<int(int amount)> onChanged;
        
        void AddTask(bicycle::function<int(void)> task)
        {
            s_runloop->AddTask(task);
        }
        void Sync(bicycle::function<int(void)> task)
        {
            s_runloop->Sync(task);
        }
        
    protected:
        ~State()
        {
            while(_start)
                delete _start;
            if (_last)
                _last->_next = _next;
            else
                _stateStart = _next;
            if (_next)
                _next->_last = _last;
            else
                _stateEnd = _last;
            if (_stateGlobal == this)
                _stateGlobal = NULL;
        }
        
        Entry *_start, *_end;
        
    private:
        int _total;
        State *_last, *_next;
        static State *_stateStart, *_stateEnd, *_stateGlobal;
    };
    
    State *State::_stateStart = NULL;
    State *State::_stateEnd = NULL;
    State *State::_stateGlobal = NULL;
    
    
}

IpcServiceProxy::IpcServiceProxy(KernelObject *service)
{
    IPC_Manager_Internal::State::GlobalState();
    _service = service;
    _active = false;
    // Should this be per kernel or per monitor? Leaving it an option by making this an instance method
    if (!IPC_Manager_Internal::s_runloop)
        IPC_Manager_Internal::s_runloop = new RunloopThread();
}

IpcServiceProxy::~IpcServiceProxy()
{
    if (_active)
        Stop();
}

void IpcServiceProxy::Start(void)
{
    StrongKernelObject<IpcServiceProxy> strongSelf(this);
    UInt32 selfHandle = IPC_Manager_Internal::Entry::Map(_service);
    IPC_Manager_Internal::State::GlobalState()->AddTask([strongSelf, selfHandle]{
        IpcServiceProxy *that = strongSelf.Value();
        if (!that->_active) {
            IPC_Manager_Internal::State::PerformOnAll([=](IPC_Manager_Internal::State *state){
                state->AddEntry(selfHandle, NULL, NULL, TYPE_START(IPC_Manager_Internal::Entry::typeProvider));
                return 0;
            });
            that->_active = true;
        }
        return 0;
    });
}

void IpcServiceProxy::Stop(void)
{
    StrongKernelObject<IpcServiceProxy> strongSelf(this);
    UInt32 selfHandle = IPC_Manager_Internal::Entry::Map(_service);
    IPC_Manager_Internal::State::GlobalState()->AddTask([strongSelf, selfHandle]{
        IpcServiceProxy *that = strongSelf.Value();
        if (that->_active) {
            IPC_Manager_Internal::State::PerformOnAll([=](IPC_Manager_Internal::State *state){
                state->AddEntry(selfHandle, NULL, NULL, TYPE_STOP(IPC_Manager_Internal::Entry::typeProvider));
                return 0;
            });
            that->_active = false;
        }
        return 0;
    });
}

void IpcServiceProxy::AddInput(IpcClient *client)
{
    UInt32 selfHandle = IPC_Manager_Internal::Entry::Map(_service);
    UInt32 clientHandle = IPC_Manager_Internal::Entry::Map(client);
    IPC_Manager_Internal::State::GlobalState()->AddTask([selfHandle, clientHandle, client, this]{
        client->Start(this);
        IPC_Manager_Internal::State::PerformOnAll([=](IPC_Manager_Internal::State *state){
            state->AddEntry(selfHandle, clientHandle, NULL, TYPE_START(IPC_Manager_Internal::Entry::typeInput));
            return 0;
        });
        return 0;
    });
}

void IpcServiceProxy::RemoveInput(IpcClient *client)
{
    UInt32 selfHandle = IPC_Manager_Internal::Entry::Map(_service);
    UInt32 clientHandle = IPC_Manager_Internal::Entry::Map(client);
    IPC_Manager_Internal::State::GlobalState()->AddTask([selfHandle, clientHandle, client]{
        IPC_Manager_Internal::State::PerformOnAll([=](IPC_Manager_Internal::State *state){
            state->AddEntry(selfHandle, clientHandle, NULL, TYPE_STOP(IPC_Manager_Internal::Entry::typeInput));
            return 0;
        });
        client->Stop();
        return 0;
    });
}

void IpcServiceProxy::AddOutput(IpcService *output)
{
    UInt32 selfHandle = IPC_Manager_Internal::Entry::Map(_service);
    UInt32 clientHandle = IPC_Manager_Internal::Entry::Map(output);
    IPC_Manager_Internal::State::GlobalState()->AddTask([selfHandle, clientHandle, output, this]{
        output->Start(this);
        IPC_Manager_Internal::State::PerformOnAll([=](IPC_Manager_Internal::State *state){
            state->AddEntry(selfHandle, clientHandle, NULL, TYPE_START(IPC_Manager_Internal::Entry::typeOutput));
            return 0;
        });
        return 0;
    });
}

void IpcServiceProxy::RemoveOutput(IpcService *output)
{
    UInt32 selfHandle = IPC_Manager_Internal::Entry::Map(_service);
    UInt32 clientHandle = IPC_Manager_Internal::Entry::Map(output);
    IPC_Manager_Internal::State::GlobalState()->AddTask([selfHandle, clientHandle, output]{
        IPC_Manager_Internal::State::PerformOnAll([=](IPC_Manager_Internal::State *state){
            state->AddEntry(selfHandle, clientHandle, NULL, TYPE_STOP(IPC_Manager_Internal::Entry::typeOutput));
            return 0;
        });
        output->Stop();
        return 0;
    });
}

void IpcServiceProxy::StartInput(IpcClient *client, IpcEndpoint *endpoint)
{
    UInt32 selfHandle = IPC_Manager_Internal::Entry::Map(_service);
    UInt32 clientHandle = IPC_Manager_Internal::Entry::Map(client);
    UInt32 endpointHandle = IPC_Manager_Internal::Entry::Map(endpoint);
    IPC_Manager_Internal::State::GlobalState()->AddTask([selfHandle, clientHandle, endpointHandle]{
        IPC_Manager_Internal::State::PerformOnAll([=](IPC_Manager_Internal::State *state){
            state->AddEntry(selfHandle, clientHandle, endpointHandle, TYPE_START(IPC_Manager_Internal::Entry::typeInputConnection));
            return 0;
        });
        return 0;
    });
    endpoint->Watch([selfHandle, clientHandle, endpointHandle]{
        IPC_Manager_Internal::State::GlobalState()->AddTask([=]{
            IPC_Manager_Internal::State::PerformOnAll([=](IPC_Manager_Internal::State *state){
                state->AddEntry(selfHandle, clientHandle, endpointHandle, TYPE_STOP(IPC_Manager_Internal::Entry::typeInputConnection));
                return 0;
            });
            return 0;
        });
        return 0;
    });
}

void IpcServiceProxy::StartOutput(IpcService *output, IpcEndpoint *endpoint)
{
    UInt32 selfHandle = IPC_Manager_Internal::Entry::Map(_service);
    UInt32 clientHandle = IPC_Manager_Internal::Entry::Map(output);
    UInt32 endpointHandle = IPC_Manager_Internal::Entry::Map(endpoint);
    IPC_Manager_Internal::State::GlobalState()->AddTask([selfHandle, clientHandle, endpointHandle]{
        IPC_Manager_Internal::State::PerformOnAll([=](IPC_Manager_Internal::State *state){
            state->AddEntry(selfHandle, clientHandle, endpointHandle, TYPE_START(IPC_Manager_Internal::Entry::typeOutputConnection));
            return 0;
        });
        return 0;
    });
    endpoint->Watch([selfHandle, clientHandle, endpointHandle]{
        IPC_Manager_Internal::State::GlobalState()->AddTask([=]{
            IPC_Manager_Internal::State::PerformOnAll([=](IPC_Manager_Internal::State *state){
                state->AddEntry(selfHandle, clientHandle, endpointHandle, TYPE_STOP(IPC_Manager_Internal::Entry::typeOutputConnection));
                return 0;
            });
            return 0;
        });
        return 0;
    });
}

IpcServiceMonitor::IpcServiceMonitor(void)
{
    _state = NULL;
    _triggered = false;
    SetTrigger(true);
}

IpcServiceMonitor::~IpcServiceMonitor()
{
    if (_state)
        _state->Release();
}

KernelArray* IpcServiceMonitor::Changes(void)
{
    KernelArray *result = new KernelArray();
    IPC_Manager_Internal::State::GlobalState()->Sync([=]{
        bool reset;
        IPC_Manager_Internal::State *state;
        
        if (_state) {
            reset = true;
            state = _state;
        } else {
            reset = false;
            state = IPC_Manager_Internal::State::GlobalState();
            _state = new IPC_Manager_Internal::State();
            _state->onChanged = [this](int amount){
                SetTrigger(amount != 0);
                return 0;
            };
            SetTrigger(false);
        }
        state->ReadOut([result](IPC_Manager_Internal::Entry *entry){
            result->Add(entry->GetInfo());
            return 0;
        }, reset);
        return 0;
    });
    result->Autorelease();
    return result;
}

void IpcServiceMonitor::SetTrigger(bool shouldTrigger)
{
    if (shouldTrigger == _triggered)
        return;
    _triggered = shouldTrigger;
    SetSignalled(this, _triggered);
}
