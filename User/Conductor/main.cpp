#include "Runtime.h"
#include "EventLoop.h"

#include "Blocking.h"
#include "Collections.h"
#include "System.h"
#include "IPC.h"

#include "../../Kernel/Kernel/Interface_Mouse.h"
#include "../../Kernel/Kernel/Interface_Keyboard.h"
#include "../../Kernel/Kernel/Interface_VideoMode.h"
#include "../../Kernel/Kernel/Interface_Video.h"

#include "Squirrel.h"

extern "C" void testprint(const char*);

static char* PrintInt(char *bob, int value)
{
    if (value < 0) {
        bob = PrintInt(bob, -value);
        bob--;
        *bob = '-';
        return bob;
    }
    *bob = '0' + (value % 10);
    if (value != 0) {
        value /= 10;
        if (value != 0)
            return PrintInt(bob - 1, value);
    }
    return bob;
}

static char* PrintHex(char *bob, int value)
{
    *bob = "0123456789ABCDEF"[value & 0xF];
    if (value != 0) {
        value >>= 4;
        if (value != 0)
            return PrintHex(bob - 1, value);
    }
    return bob;
}

static bool IsType(Kernel::Handle *object, const char *name)
{
    char buffer[100];
    object->GetClassName(0, buffer, sizeof(buffer));
    int i;
    for (i = 0; (name[i] != '\0') && (buffer[i] != '\0'); i++)
        if (name[i] != buffer[i])
            return false;
    return name[i] == buffer[i];
}

static void Wait(int time)
{
    Kernel::Blocking::Timer *timer = Kernel::Blocking::Timer::Create();
    timer->Reset(time, true);
    timer->Block()->Release();
    timer->Release();
}

static bool Compare(const char *a, const char *b)
{
    while ((*a == *b) && (*a != '\0') && (*b != '\0')) {
        a++;
        b++;
    }
    return *a == *b;
}

static bool Compare(Kernel::Collections::String *string, const char *other)
{
    char buf[100];
    string->Copy(buf, sizeof(buf));
    return Compare(buf, other);
}

class SimpleSink
{
public:
    SimpleSink()
    {
        _provider = Kernel::IPC::Provider::Create();
        Kernel::Collections::String *name = Kernel::Collections::String::Create("input");
        _input = _provider->CreateClient(name);
        name->Release();
    }
    
    ~SimpleSink()
    {
        _input->Release();
        _provider->Release();
    }
    
    Kernel::IPC::Client* Input(void)
    {
        return _input;
    }
    
    Kernel::IPC::Provider* Provider(void)
    {
        return _provider;
    }
    
private:
    Kernel::IPC::Provider *_provider;
    Kernel::IPC::Client *_input;
};

static void SetPixel24(unsigned char *buffer, int widthSpan, int x, int y, Pixel colour)
{
    UInt32 colour24 = (colour.r << 0) | (colour.g << 8) | (colour.b << 16);
    buffer += (y * widthSpan) + (3 * x);
    buffer[0] = colour24 & 0xFF;
    buffer[1] = (colour24 >> 8) & 0xFF;
    buffer[2] = (colour24 >> 16) & 0xFF;
}

static void SetPixel32(unsigned char *buffer, int widthSpan, int x, int y, Pixel colour)
{
    buffer += y * widthSpan;
    buffer += x * /*pixelSpan*/4;
    ((UInt32*)buffer)[0] =  (colour.r << 0) | (colour.g << 8) | (colour.b << 16);
}

Pixel background = {130, 130, 250, 255};

void(*pixelSet)(unsigned char *buffer, int widthSpan, int x, int y, Pixel colour);
int localX, localY;
bool localSet = false;

static void SetCursor(void *framebuffer, int linespan, int width, int height, int offsetX, int offsetY)
{
    if (localSet) {
        for (int y = 0; y < nat299.h; y++) {
            for (int x = 0; x < nat299.w; x++) {
                pixelSet((unsigned char*)framebuffer, linespan, x + localX, y + localY, background);
            }
        }
        localX += offsetX;
        localY -= offsetY;
        if (localX < 0)
            localX = 0;
        else if (localX > (width - nat299.w))
            localX = width - nat299.w;
        if (localY < 0)
            localY = 0;
        else if (localY > (height - nat299.h))
            localY = height - nat299.h;
    } else {
        localX = offsetX;
        localY = offsetY;
        localSet = true;
    }
    for (int y = 0; y < nat299.h; y++) {
        for (int x = 0; x < nat299.w; x++) {
            int bx = x % nat299.w;
            int by = y % nat299.h;
            int index = (by * nat299.w) + bx;
            if (nat299.data[index].a == 255)
                pixelSet((unsigned char*)framebuffer, linespan, x + localX, y + localY, nat299.data[index]);
        }
    }
}

extern "C" void sysmain(void)
{
    char buf[100];
    int x = 0;
    
    Kernel::System::Monitor *monitor = Kernel::System::Monitor::Create();
    Kernel::Collections::String *stateStr = Kernel::Collections::String::Create("State");
    Kernel::Collections::String *typeStr = Kernel::Collections::String::Create("Type");
    Kernel::Collections::String *connectorStr = Kernel::Collections::String::Create("Connector");
    SimpleSink *mouseSink = NULL;
    SimpleSink *keyboardSink = NULL;
    SimpleSink *videoBuffer = NULL;
    SimpleSink *videoMode = NULL;
    Video::Buffer *screen = NULL;
    
    EventLoop loop;
    
    EventLoop::Handler mouseEvent = [&](Kernel::Blocking::Blockable *trigger, Kernel::Collections::Array *others){
        Kernel::IPC::Connection *connection = NULL;
        Kernel::IPC::Memory *message = NULL;
        Kernel::IPC::ProviderIO *io = NULL;
        Kernel::IPC::Provider::EventType providerEvent = mouseSink->Provider()->GetEvent(&connection, &message, &io);
        if (providerEvent == Kernel::IPC::Provider::eventInputMessage) {
            Kernel::Handle *releaser;
            Interface_Mouse::Event *event = (Interface_Mouse::Event*)message->GetPointer(&releaser);
            switch(event->type) {
                case Interface_Mouse::Event::Button:
                {
                    Interface_Mouse::Button *button = (Interface_Mouse::Button*)event;
                    // BUTAN
                    break;
                }
                case Interface_Mouse::Event::Motion:
                {
                    Interface_Mouse::Motion *motion = (Interface_Mouse::Motion*)event;
                    // MOVE
                    if (screen)
                        SetCursor(screen->Framebuffer(), screen->lineSpan, screen->width, screen->height, motion->x, motion->y);
                    break;
                }
            }
            releaser->Release();
        }
        if (io)
            io->Release();
        if (message)
            message->Release();
        if (connection)
            connection->Release();
    };
    EventLoop::Handler keyboardEvent = [&](Kernel::Blocking::Blockable *trigger, Kernel::Collections::Array *others){
        Kernel::IPC::Connection *connection = NULL;
        Kernel::IPC::Memory *message = NULL;
        Kernel::IPC::ProviderIO *io = NULL;
        Kernel::IPC::Provider::EventType providerEvent = keyboardSink->Provider()->GetEvent(&connection, &message, &io);
        if (providerEvent == Kernel::IPC::Provider::eventInputMessage) {
            Kernel::Handle *releaser;
            Interface_Keyboard::Event *event = (Interface_Keyboard::Event*)message->GetPointer(&releaser);
            switch(event->type) {
                case Interface_Keyboard::Event::KeyEvent:
                {
                    Interface_Keyboard::Event *keyEvent = (Interface_Keyboard::Event*)event;
                    // KEY
                    break;
                }
            }
            releaser->Release();
        }
        if (io)
            io->Release();
        if (message)
            message->Release();
        if (connection)
            connection->Release();
    };
    EventLoop::Handler videoModeEvent = [&](Kernel::Blocking::Blockable *trigger, Kernel::Collections::Array *others){
        Kernel::IPC::Connection *connection = NULL;
        Kernel::IPC::Memory *message = NULL;
        Kernel::IPC::ProviderIO *io = NULL;
        Kernel::IPC::Provider::EventType providerEvent = videoMode->Provider()->GetEvent(&connection, &message, &io);
        switch(providerEvent) {
            case Kernel::IPC::Provider::eventInputConnect:
            {
                Kernel::IPC::Memory *message = Kernel::IPC::Memory::Create(16384);
                Kernel::Handle *releaser;
                VideoMode::Request *getModes = (VideoMode::Request*)message->GetPointer(&releaser);
                getModes->packetClass = Interface_Packet::Request;
                getModes->identifier = 0;
                getModes->type = VideoMode::Request::GetModes;
                releaser->Release();
                connection->Send(message);
                message->Release();
            }
                break;
            case Kernel::IPC::Provider::eventInputMessage:
            {
                Kernel::Handle *releaser;
                Interface_Response *response = (Interface_Response*)message->GetPointer(&releaser);
                FlatString *mw = FlatString::CreateDynamic(Mode_Width);
                FlatString *mh = FlatString::CreateDynamic(Mode_Height);
                FlatString *mt = FlatString::CreateDynamic(Mode_Type);
                FlatString *mi = FlatString::CreateDynamic(Mode_Identifier);
                switch(response->type) {
                    case VideoMode::Request::GetModes:
                    {
                        VideoMode::ModeList *ports = (VideoMode::ModeList*)response;
                        FlatInteger *best = NULL;
                        int bestSize = 0;
                        for (int i = 0; i < ports->ports.Count(); i++) {
                            FlatArray *modes = (FlatArray*)ports->ports.ItemAt(i);
                            for (int j = 0; j < modes->Count(); j++) {
                                FlatDictionary *dictionary = (FlatDictionary*)modes->ItemAt(j);
                                FlatInteger *width = (FlatInteger*)dictionary->ItemFor(mw);
                                FlatInteger *height = (FlatInteger*)dictionary->ItemFor(mh);
                                FlatString *type = (FlatString*)dictionary->ItemFor(mt);
                                if (Compare(type->Value(), Mode_Type_Graphical)) {
                                    int size = width->Value() * height->Value();
                                    if (size > bestSize) {
                                        bestSize = size;
                                        best = (FlatInteger*)dictionary->ItemFor(mi);
                                    }
                                }
                            }
                        }
                        if (best) {
                            Kernel::IPC::Memory *message = Kernel::IPC::Memory::Create(16384);
                            Kernel::Handle *releaser;
                            VideoMode::SetMode *setMode = (VideoMode::SetMode*)message->GetPointer(&releaser);
                            setMode->packetClass = Interface_Packet::Request;
                            setMode->identifier = 0;
                            setMode->type = VideoMode::Request::SetMode;
                            setMode->port = 0;
                            setMode->modeIdentifier = best->Value();
                            releaser->Release();
                            connection->Send(message);
                            message->Release();
                        }
                    }
                        break;
                }
                releaser->Release();
            }
                break;
        }
        if (io)
            io->Release();
        if (message)
            message->Release();
        if (connection)
            connection->Release();
    };
    EventLoop::Handler videoEvent = [&](Kernel::Blocking::Blockable *trigger, Kernel::Collections::Array *others){
        Kernel::IPC::Connection *connection = NULL;
        Kernel::IPC::Memory *message = NULL;
        Kernel::IPC::ProviderIO *io = NULL;
        Kernel::IPC::Provider::EventType providerEvent = videoBuffer->Provider()->GetEvent(&connection, &message, &io);
        switch(providerEvent) {
            case Kernel::IPC::Provider::eventInputConnect:
            {
                Kernel::IPC::Memory *message = Kernel::IPC::Memory::Create(16384);
                Kernel::Handle *releaser;
                Video::GetBuffer *getBuffer = (Video::GetBuffer*)message->GetPointer(&releaser);
                getBuffer->packetClass = Interface_Packet::Request;
                getBuffer->identifier = 0;
                getBuffer->type = Video::Request::GetBuffer;
                getBuffer->requestedWidth = getBuffer->requestedHeight = getBuffer->requestedDepth = 0;
                releaser->Release();
                connection->Send(message);
                message->Release();
            }
                break;
            case Kernel::IPC::Provider::eventInputMessage:
            {
                Kernel::Handle *releaser;
                Video::Response *event = (Video::Response*)message->GetPointer(&releaser);
                switch(event->type) {
                    case Video::Response::Buffer:
                    {
                        Video::Buffer *buffer = (Video::Buffer*)event;
                        if (event->status == Interface_Response::Success) {
                            // SCREEN
                            screen = buffer;
                            releaser->AddRef();
                            
                            switch(screen->pixelSpan) {
                                case 4:
                                default:
                                    pixelSet = SetPixel32;
                                    break;
                                case 3:
                                    pixelSet = SetPixel24;
                                    break;
                            }
                            int amount = 0;
                            for (int y = 0; y < buffer->height; y++) {
                                for (int x = 0; x < buffer->width; x++) {
                                    pixelSet((unsigned char*)buffer->Framebuffer(), buffer->lineSpan, x, y, background);
                                }
                            }
                            SetCursor(screen->Framebuffer(), screen->lineSpan, screen->width, screen->height, 10, 10);
                        }
                        break;
                    }
                }
                releaser->Release();
            }
                break;
        }
        if (io)
            io->Release();
        if (message)
            message->Release();
        if (connection)
            connection->Release();
    };
    
    loop.AddSource(monitor, [&](Kernel::Blocking::Blockable *trigger, Kernel::Collections::Array *others){
        Kernel::Collections::Array *array = monitor->Changes();
        int length = array->Count();
        for (int i = 0; i < length; i++) {
            Kernel::Collections::Dictionary *data = (Kernel::Collections::Dictionary*)array->ObjectAt(i);
            // State==Start; Type==Input/Output; Connector
            if (Compare((Kernel::Collections::String*)data->ObjectFor(stateStr), "Start")) {
                Kernel::Collections::String *typeName = (Kernel::Collections::String*)data->ObjectFor(typeStr);
                if (Compare(typeName, "Input")) {
                }
                if (Compare(typeName, "Output")) {
                    Kernel::IPC::Service *service = (Kernel::IPC::Service*)monitor->ObjectForIdentifier(((Kernel::Collections::Number*)data->ObjectFor(connectorStr))->Value());
                    if (Compare(service->Type(), "core.provider.mouse")) {
                        mouseSink = new SimpleSink();
                        mouseSink->Input()->Connect(service);
                        loop.AddSource(mouseSink->Provider(), mouseEvent);
                    }
                    if (Compare(service->Type(), "core.provider.keyboard")) {
                        keyboardSink = new SimpleSink();
                        keyboardSink->Input()->Connect(service);
                        loop.AddSource(keyboardSink->Provider(), keyboardEvent);
                    }
                    if (Compare(service->Type(), "core.provider.video.mode")) {
                        videoMode = new SimpleSink();
                        videoMode->Input()->Connect(service);
                        loop.AddSource(videoMode->Provider(), videoModeEvent);
                    }
                    if (Compare(service->Type(), "core.provider.video")) {
                        videoBuffer = new SimpleSink();
                        videoBuffer->Input()->Connect(service);
                        loop.AddSource(videoBuffer->Provider(), videoEvent);
                    }
                }
            }
        }
        array->Release();
    });
    
    testprint("Running\n");
    loop.Run();

    // test
    asm("cli");
    
    while(true);
}
