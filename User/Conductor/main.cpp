#include "Blocking.h"
#include "Collections.h"
#include "System.h"
#include "IPC.h"
#include "Runtime.h"
#include "../../Kernel/Kernel/Interface_Mouse.h"
#include "../../Kernel/Kernel/Interface_Keyboard.h"
#include "../../Kernel/Kernel/Interface_VideoMode.h"
#include "../../Kernel/Kernel/Interface_Video.h"

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

static void HandleDriver(Kernel::System::Driver *driver, int level)
{
    char buf[100];
    
    for (int i = 1; i < level; i++)
        testprint(" | ");
    if (level != 0)
        testprint(" +-");
    int len = driver->GetClassName(0, buf, sizeof(buf));
    buf[len] = '\0';
    testprint(buf);
    testprint("\n");
    
    Kernel::Collections::Array *children = driver->Children();
    int j = children->Count();
    for (int i = 0; i < j; i++)
        HandleDriver((Kernel::System::Driver*)children->ObjectAt(i), level + 1);
    children->Release();
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

static void Handle(Kernel::Handle *object, int indent = 0)
{
    char buf[100];
    
    buf[sizeof(buf) - 1] = '\0';
    for (int i = 0; i < indent; i++)
        testprint("    ");
    if (IsType(object, "KernelArray")) {
        Kernel::Collections::Array *array = (Kernel::Collections::Array*)object;
        int length = array->Count();
        testprint("<Array: ");
        testprint(PrintInt(buf + sizeof(buf) - 2, length));
        testprint(" items>\n");
        for (int i = 0; i < length; i++) {
            Kernel::Handle *object = array->ObjectAt(i);
            Handle(object, indent + 1);
        }
        Wait(SECONDS(2));
    } else if (IsType(object, "KernelDictionary")) {
        Kernel::Collections::Dictionary *dictionary = (Kernel::Collections::Dictionary*)object;
        testprint("<Dictionary>\n");
        Kernel::Collections::Array *keys = dictionary->AllKeys();
        int length = keys->Count();
        for (int i = 0; i < length; i++) {
            Kernel::Handle *key = keys->ObjectAt(i);
            Handle(key, indent + 1);
            Kernel::Handle *value = dictionary->ObjectFor(key);
            Handle(value, indent + 2);
        }
        keys->Release();
        Wait(SECONDS(2));
    } else if(IsType(object, "KernelNumber")) {
        Kernel::Collections::Number *number = (Kernel::Collections::Number*)object;
        testprint("<Number: ");
        testprint(PrintInt(buf + sizeof(buf) - 2, number->Value()));
        testprint(">\n");
    } else if (IsType(object, "KernelString")) {
        Kernel::Collections::String *string = (Kernel::Collections::String*)object;
        testprint("<String: ");
        string->Copy(buf, sizeof(buf));
        testprint(buf);
        testprint(">\n");
    } else {
        testprint("<");
        object->GetClassName(0, buf, sizeof(buf));
        testprint(buf);
        testprint(">\n");
    }
}

static void funkprint(Kernel::Collections::String *str)
{
    if (!str) {
        testprint("<null>");
        return;
    }
    char buf[100];
    str->Copy(buf, sizeof(buf));
    testprint(buf);
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

extern "C" void sysmain(void)
{
    char buf[100];
    int x = 0;
//
//bob:
//    buf[sizeof(buf) - 1] = '\0';
//    
//    testprint("Userspace taunt! ");
//    testprint(PrintInt(buf - 1, x++));
//    testprint("\n");
//    for (int i = 0; i <100000; i++)
//        for (int j = 0; j <1000; j++);
//    goto bob;
    
//    Kernel::Blocking::Timer *timer = Kernel::Blocking::Timer::Create();
//    testprint("Created timer ");
//    testprint(PrintHex(buf - 1, (int)timer));
//    testprint("\n");
//    timer->Reset(SECONDS(2), true);
//    while(true) {
//        timer->Block()->Release();
//        testprint("Userspace bling! ");
//        testprint(PrintInt(buf - 1, x++));
//        testprint("\n");
//    }

    Kernel::System::Driver *root = Kernel::System::Driver::GetRootDriver();
    HandleDriver(root, 0);
    root->Release();
    
    Kernel::System::Monitor *monitor = Kernel::System::Monitor::Create();
//    Handle(monitor->Changes());
    Kernel::Collections::String *stateStr = Kernel::Collections::String::Create("State");
    Kernel::Collections::String *typeStr = Kernel::Collections::String::Create("Type");
    Kernel::Collections::String *connectorStr = Kernel::Collections::String::Create("Connector");
    SimpleSink *mouseSink = NULL;
    SimpleSink *keyboardSink = NULL;
    SimpleSink *videoBuffer = NULL;
    SimpleSink *videoMode = NULL;
    
    Kernel::Blocking::List *collection = Kernel::Blocking::List::CreateOr();
    collection->Add(monitor);
    
    while(true) {
        Kernel::Collections::Array *result = collection->Block();
        if (result->Find(monitor) != -1) {
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
                        funkprint(service->Name());
                        testprint(" (");
                        funkprint(service->Type());
                        testprint(")\n");
                        if (Compare(service->Type(), "core.provider.mouse")) {
                            mouseSink = new SimpleSink();
                            mouseSink->Input()->Connect(service);
                            collection->Add(mouseSink->Provider());
                        }
                        if (Compare(service->Type(), "core.provider.keyboard")) {
                            keyboardSink = new SimpleSink();
                            keyboardSink->Input()->Connect(service);
                            collection->Add(keyboardSink->Provider());
                        }
                        if (Compare(service->Type(), "core.provider.video.mode")) {
                            videoMode = new SimpleSink();
                            videoMode->Input()->Connect(service);
                            collection->Add(videoMode->Provider());
                        }
                        if (Compare(service->Type(), "core.provider.video")) {
                            videoBuffer = new SimpleSink();
                            videoBuffer->Input()->Connect(service);
                            collection->Add(videoBuffer->Provider());
                        }
                    }
                }
            }
            array->Release();
        }
        if (videoBuffer && result->Find(videoBuffer->Provider()) != -1) {
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
                                testprint("Video buffer acquired\n");
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
        }
        if (mouseSink && result->Find(mouseSink->Provider()) != -1) {
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
                        testprint("Button ");
                        testprint(PrintInt(buf + sizeof(buf) - 2, button->index));
                        testprint(button->down ? " down" : " up");
                        break;
                    }
                    case Interface_Mouse::Event::Motion:
                    {
                        Interface_Mouse::Motion *motion = (Interface_Mouse::Motion*)event;
                        testprint("Motion ");
                        testprint(PrintInt(buf + sizeof(buf) - 2, motion->x));
                        testprint(", ");
                        testprint(PrintInt(buf + sizeof(buf) - 2, motion->y));
                        break;
                    }
                }
                releaser->Release();
            } else testprint(PrintInt(buf + sizeof(buf) - 2, providerEvent));
            if (io)
                io->Release();
            if (message)
                message->Release();
            if (connection)
                connection->Release();
        }
        if (keyboardSink && result->Find(keyboardSink->Provider()) != -1) {
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
                        testprint("Key ");
                        buf[0] = keyEvent->keyValue;
                        buf[1] = '\0';
                        testprint(buf);
                        testprint(keyEvent->keyDown ? " down" : " up");
                        break;
                    }
                }
                releaser->Release();
            } else testprint(PrintInt(buf + sizeof(buf) - 2, providerEvent));
            if (io)
                io->Release();
            if (message)
                message->Release();
            if (connection)
                connection->Release();
        }
        if (videoMode && result->Find(videoMode->Provider()) != -1) {
            Kernel::IPC::Connection *connection = NULL;
            Kernel::IPC::Memory *message = NULL;
            Kernel::IPC::ProviderIO *io = NULL;
            Kernel::IPC::Provider::EventType providerEvent = videoMode->Provider()->GetEvent(&connection, &message, &io);
            switch(providerEvent) {
                case Kernel::IPC::Provider::eventInputConnect:
                {
                    testprint("Sending mode list request\n");
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
                                testprint("Port ");
                                testprint(PrintInt(buf + sizeof(buf) - 2, i));
                                testprint("\n");
                                FlatArray *modes = (FlatArray*)ports->ports.ItemAt(i);
                                for (int j = 0; j < modes->Count(); j++) {
                                    testprint("Mode ");
                                    testprint(PrintInt(buf + sizeof(buf) - 2, j));
                                    testprint(": ");
                                    FlatDictionary *dictionary = (FlatDictionary*)modes->ItemAt(j);
                                    FlatInteger *width = (FlatInteger*)dictionary->ItemFor(mw);
                                    FlatInteger *height = (FlatInteger*)dictionary->ItemFor(mh);
                                    FlatString *type = (FlatString*)dictionary->ItemFor(mt);
                                    testprint(PrintInt(buf + sizeof(buf) - 2, width->Value()));
                                    testprint("x");
                                    testprint(PrintInt(buf + sizeof(buf) - 2, height->Value()));
                                    testprint(" [");
                                    testprint(type->Value());
                                    testprint("]\n");
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
                                testprint("Sending graphics request\n");
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
        }
        result->Release();
        testprint("\n");
    }
    
    asm("cli");
    
    while(true);
}
