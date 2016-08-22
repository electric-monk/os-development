#include "GenericMouse.h"
#include "IPC.h"
#include "Runloop.h"
#include "debug.h"
#include "Interface_Mouse.h"

GenericMouse::GenericMouse(const char *name)
:ProviderDriver(name)
{
}

GenericMouse::~GenericMouse()
{
}

bool GenericMouse::Start(Driver *parent)
{
    _thread = new RunloopThread(NULL);
    IpcService *ipcService = new IpcService("mouse"_ko, SERVICE_TYPE_MOUSE);
    _service = new Service(this, ipcService);
    ipcService->Release();
    Launch(_service);
    _identifierCount = 0;
    return ProviderDriver::Start(parent);
}

void GenericMouse::Stop(void)
{
    Terminate(_service);
    _service->Release();
    _thread->Release();
    ProviderDriver::Stop();
}

void GenericMouse::SendMotion(int x, int y, int type)
{
    int currentIdentifier = _identifierCount++;
    _connections->Enumerate([currentIdentifier, x, y, type](KernelObject *object){
        Connection *connection = (Connection*)object;
        connection->Link()->SendMessage([currentIdentifier, x, y, type](void *buffer)->bool{
            Interface_Mouse::Motion *event = (Interface_Mouse::Motion*)buffer;
            event->packetClass = Interface_Packet::Event;
            event->identifier = currentIdentifier;
            event->type = type;
            event->timestamp = 0/*TODO*/;
            event->x = x;
            event->y = y;
            return true;
        });
        return (void*)NULL;
    });
}

void GenericMouse::Move(SInt32 x, SInt32 y)
{
    SendMotion(x, y, Interface_Mouse::Event::Motion);
}

void GenericMouse::Button(UInt32 index, bool down)
{
    int currentIdentifier = _identifierCount++;
    _connections->Enumerate([currentIdentifier, index, down](KernelObject *object){
        Connection *connection = (Connection*)object;
        connection->Link()->SendMessage([currentIdentifier, index, down](void *buffer)->bool{
            Interface_Mouse::Button *event = (Interface_Mouse::Button*)buffer;
            event->packetClass = Interface_Packet::Event;
            event->identifier = currentIdentifier;
            event->type = Interface_Mouse::Event::Button;
            event->timestamp = 0/*TODO*/;
            event->index = index;
            event->down = down;
            return true;
        });
        return (void*)NULL;
    });
}

void GenericMouse::Scroll(SInt32 x, SInt32 y)
{
    SendMotion(x, y, Interface_Mouse::Event::Wheel);
}

class GenericMouse_Connection : public ProviderDriver::Connection
{
public:
    CLASSNAME(ProviderDriver::Connection, GenericMouse_Connection);
    
    GenericMouse_Connection(GenericMouse *owner, ProviderDriver::Service *service, IpcEndpoint *endpoint)
    :ProviderDriver::Connection(owner, service, endpoint)
    {
    }
};

ProviderDriver::Connection* GenericMouse::ConnectionStart(Service *service, IpcEndpoint *endpoint)
{
    return new GenericMouse_Connection(this, service, endpoint);
}

void GenericMouse::ConnectionReceive(Connection *connection, KernelBufferMemory *message)
{
    // TODO
}

void GenericMouse::ConnectionStop(Connection *connection)
{
    connection->Release();
}
