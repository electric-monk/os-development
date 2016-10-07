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

#include "Context.h"
namespace SquirrelTest {
#include "Squirrel.h"
    Graphics::FrameBuffer* Get(void)
    {
        Graphics::Bitmap *output = new Graphics::Bitmap(nat299.w, nat299.h, Graphics::Bitmap::Format32RGBA);
        UInt8 *outputBuffer = output->Buffer();
        for (int i = 0; i < (nat299.w * nat299.h); i++) {
            *(outputBuffer++) = nat299.data[i].r;
            *(outputBuffer++) = nat299.data[i].g;
            *(outputBuffer++) = nat299.data[i].b;
            *(outputBuffer++) = nat299.data[i].a;
        }
        return output;
    }
}
class SystemFramebuffer : public Graphics::FrameBuffer
{
public:
    SystemFramebuffer()
    {
        _handle = NULL;
        _data = NULL;
    }
    ~SystemFramebuffer()
    {
        if (_handle)
            _handle->Release();
    }
    
    void SetBuffer(Kernel::Handle *owner, Video::Buffer *data)
    {
        Kernel::Handle *other = _handle;
        _handle = owner;
        if (_handle)
            _handle->AddRef();
        if (other)
            other->Release();
        _data = _handle ? data : NULL;
    }

    Format Type(void) const
    {
        switch(_data->format) {
            case Video::Buffer::PixelFormat24RGB:
                return Graphics::FrameBuffer::Format24RGB;
            case Video::Buffer::PixelFormat24BGR:
                return Graphics::FrameBuffer::Format24BGR;
            case Video::Buffer::PixelFormat32RGBx:
                return Graphics::FrameBuffer::Format32RGBx;
            case Video::Buffer::PixelFormat32BGRx:
                return Graphics::FrameBuffer::Format32BGRx;
        }
    }
    UInt32 Width(void) const { return _data->width; }
    UInt32 Height(void) const { return _data->height; }
    UInt8* Buffer(void) const { return (UInt8*)_data->Framebuffer(); }
    
private:
    Kernel::Handle *_handle;
    Video::Buffer *_data;
};

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
    SystemFramebuffer screenFramebuffer;
    Graphics::Bitmap *offscreenFramebuffer;
    
    Graphics::FrameBuffer *_squirrel = SquirrelTest::Get();
    int _count = 0;
    Graphics::Path _star((Graphics::Point2D){50, 0});
    _star.LineTo((Graphics::Point2D){65, 100});
    _star.LineTo((Graphics::Point2D){0, 30});
    _star.LineTo((Graphics::Point2D){100, 30});
    _star.LineTo((Graphics::Point2D){35, 100});
    
    Graphics::Path _circle((Graphics::Point2D){50, 50});
    for (Graphics::Unit i = 0; i < 2*3.1415; i+=0.1)
        _circle.LineTo((Graphics::Point2D){50 + 40 * Library::Maths::Cosine(i), 50 + 40 * Library::Maths::Sine(i)});
    
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
                            testprint("Got screen\n");
                            // SCREEN
                            screenFramebuffer.SetBuffer(releaser, buffer);
                            offscreenFramebuffer = new Graphics::Bitmap(screenFramebuffer.Width(), screenFramebuffer.Height(), screenFramebuffer.Type());
                            Kernel::Blocking::Timer *timer = Kernel::Blocking::Timer::Create();
                            loop.AddSource(timer, [&](Kernel::Blocking::Blockable *trigger, Kernel::Collections::Array *others){
                                Graphics::Context context(*offscreenFramebuffer);
                                Graphics::Path rect((Graphics::Point2D){0, 0});
                                rect.LineTo((Graphics::Point2D){screenFramebuffer.Width(), 0});
                                rect.LineTo((Graphics::Point2D){screenFramebuffer.Width(), screenFramebuffer.Height()});
                                rect.LineTo((Graphics::Point2D){0, screenFramebuffer.Height()});
                                context.DrawPolygon(rect, Graphics::Colour::White);
                                context.Push();
                                context.Translate(50, 50);
                                context.Rotate(_count * 0.02);
                                context.Translate(-50, -50);
                                context.DrawLine(_circle, Graphics::Colour::Black, (_count & 0xFFF) / 32.0);
                                context.Pop();
                                context.Translate(200, 200);
                                context.Rotate(_count * 0.01);
                                context.Translate(-50, -50);
                                context.DrawPolygon(_star, (Graphics::Colour){0xff,0x00,0x00,UInt8(_count)});//(Graphics::Colour){static_cast<UInt8>(_count), static_cast<UInt8>(_count >> 8), static_cast<UInt8>(_count >> 16), 0xff});
                                context.DrawLine(_star, Graphics::Colour::Black, 4);
                                context.Rotate(-_count * 0.01);
                                context.Scale(Library::Maths::Cosine(_count * 0.05) + 1.5, Library::Maths::Cosine(_count * 0.05) + 1.5);
                                context.Rotate(Library::Maths::Cosine(_count * 0.1) * 0.5);
                                context.DrawBitmap(*_squirrel);
                                _count += 3;
                                CopyFast(screenFramebuffer.Buffer(), offscreenFramebuffer->Buffer(), screenFramebuffer.Width() * screenFramebuffer.Height() * screenFramebuffer.PixelSize());
                            });
                            timer->Reset(MILLISECONDS(1), true);
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
