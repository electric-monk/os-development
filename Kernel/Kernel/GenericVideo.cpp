#include "GenericVideo.h"
#include "Interface_VideoMode.h"
#include "Interface_Console.h"
#include "Interface_Video.h"
#include "Collections.h"
#include "Runloop.h"
#include "IPC.h"
#include "descript.h"

namespace GenericVideo_Internal {
    class VideoBufferMemory : public KernelBufferMemory
    {
    public:
        CLASSNAME(KernelBufferMemory, GenericVideo_Internal::VideoBufferMemory);
        
        VideoBufferMemory(PhysicalPointer physical, UInt64 length)
        :KernelBufferMemory(PAGE_SIZE + length)
        {
            _basePhysical = physical;
            _lengthPhysical = length;
        }
        
        PhysicalPointer PointerForOffset(UInt64 offset)
        {
            UInt64 videoOffset = offset - PAGE_SIZE;
            if ((offset < PAGE_SIZE) || (videoOffset >= _lengthPhysical))
                return KernelBufferMemory::PointerForOffset(offset);
            return (void*)(UInt64(_basePhysical) + offset);
        }
        
    private:
        PhysicalPointer _basePhysical;
        UInt64 _lengthPhysical;
    };
}

GenericVideo::GenericVideo(const char *name)
:ProviderDriver(name)
{
    _ports = new KernelDictionary();
}

GenericVideo::~GenericVideo()
{
    _ports->Release();
}

static void SaveInt(FlatDictionary *dictionary, const char *name, UInt32 value)
{
    FlatString *key = (FlatString*)dictionary->GetNextAddress();
    key->Initialise(name);
    dictionary->CompleteNextItem();
    FlatInteger *val = (FlatInteger*)dictionary->GetNextAddress();
    val->Initialise(value);
    dictionary->CompleteNextItem();
}

static void SaveString(FlatDictionary *dictionary, const char *name, const char *value)
{
    FlatString *key = (FlatString*)dictionary->GetNextAddress();
    key->Initialise(name);
    dictionary->CompleteNextItem();
    FlatString *val = (FlatString*)dictionary->GetNextAddress();
    val->Initialise(value);
    dictionary->CompleteNextItem();
}

bool GenericVideo::Start(Driver *parent)
{
    if (!ProviderDriver::Start(parent))
        return false;
    // We always export a "mode" interface, which allows a client to pick a text or video mode and what resolution it is
    IpcService *ipcService = new IpcService("mode"_ko, SERVICE_TYPE_VIDEOMODE);
    Service *service = new Service(this, ipcService);
    ipcService->Release();
    service->start = [this, service](IpcEndpoint *endpoint){
        Connection *connection = new Connection(this, service, endpoint);
        connection->message = [this, endpoint](KernelBufferMemory *message){
            KernelBufferMemory::Map *map = new KernelBufferMemory::Map(NULL, message, true);
            Interface_Request *packet = (Interface_Request*)map->LinearBase();
            switch (packet->type) {
                case VideoMode::Request::GetModes:
                    endpoint->SendMessage([this, packet](void *buffer){
                        VideoMode::ModeList *list = (VideoMode::ModeList*)buffer;
                        list->Fill(packet);
                        list->ports.Initialise();
                        int count = GetPortCount();
                        for (int i = 0; i < count; i++) {
                            FlatArray *modes = (FlatArray*)list->ports.GetNextAddress();
                            modes->Initialise();
                            int max = GetModes(i, NULL);
                            MODE *availableModes = new MODE[max];
                            GetModes(i, availableModes);
                            for (int j = 0; j < max; j++) {
                                FlatDictionary *modeInfo = (FlatDictionary*)modes->GetNextAddress();
                                modeInfo->Initialise();
                                SaveInt(modeInfo, Mode_Identifier, (UInt32)availableModes[j].identifier);
                                SaveInt(modeInfo, Mode_Width, availableModes[j].width);
                                SaveInt(modeInfo, Mode_Height, availableModes[j].height);
                                SaveString(modeInfo, Mode_Type, availableModes[j].mode == tGraphical ? Mode_Type_Graphical : Mode_Type_Text);
                                // TODO: More fields
                                modes->CompleteNextItem();
                            }
                            delete[] availableModes;
                            list->ports.CompleteNextItem();
                        }
                        return true;
                    });
                    break;
                case VideoMode::Request::SetMode:
                {
                    VideoMode::SetMode *setMode = (VideoMode::SetMode*)packet;
                    SetMode(setMode->port, (MODE_IDENTIFIER)setMode->modeIdentifier);
                }
                    break;
            }
        };
        connection->stop = []{
            // TODO?
        };
        return connection;
    };
    Launch(service);
    service->Release();
    // Start up default modes
    int max = GetPortCount();
    for (int i = 0; i < max; i++)
        UpdatePort(i);
    // Done
    return true;
}

void GenericVideo::Stop(void)
{
    _runloop->AddTask([this]{
        for (int i = 0; i < _services->Count(); i++) {
            Terminate((Service*)_services->ObjectAt(i));
        }
        _ports->Release();
        _ports = new KernelDictionary();
    });
    ProviderDriver::Stop();
}

void GenericVideo::UpdatePort(int index)
{
    KernelNumber *portIndex = new KernelNumber(index);
    // Get old service
    Service *service = (Service*)_ports->ObjectFor(portIndex);
    if (service != NULL) {
        Terminate(service);
        _ports->Set(portIndex, NULL);
    }
    // Create new service
    switch (GetPortMode(index)) {
        case tGraphical:
        {
            IpcService *ipcService = new IpcService(KernelString::Format("%i", index), SERVICE_TYPE_VIDEO);
            service = new Service(this, ipcService);
            ipcService->Release();
            service->start = [this, service, index](IpcEndpoint *endpoint){
                Connection *connection = new Connection(this, service, endpoint);
                FRAMEBUFFER info;
                GetFramebuffer(index, &info);
                KernelBufferMemory *localMemory = new GenericVideo_Internal::VideoBufferMemory(info.address, info.lineSpan * info.height);
                KernelBufferMemory::Map *localMap = new KernelBufferMemory::Map(NULL, localMemory, false);
                void *framebuffer = ((Video::Buffer*)localMap->LinearBase())->Framebuffer();
                connection->message = [this, index, endpoint, info, localMemory, framebuffer](KernelBufferMemory *message){
                    KernelBufferMemory::Map *map = new KernelBufferMemory::Map(NULL, message, true);
                    Interface_Request *packet = (Interface_Request*)map->LinearBase();
                    switch (packet->type) {
                        case Video::Request::GetBuffer:
                        {
                            //Video::GetBuffer *getBuffer = (Video::GetBuffer*)packet;
                            // As a video driver, we currently just ignore the contents of getBuffer
                            KernelBufferMemory *response = new GenericVideo_Internal::VideoBufferMemory(info.address, info.lineSpan * info.height);
                            KernelBufferMemory::Map *responseMap = new KernelBufferMemory::Map(NULL, response, false);
                            Video::Buffer *responseBuffer = (Video::Buffer*)responseMap->LinearBase();
                            responseBuffer->identifier = packet->identifier;
                            responseBuffer->type = Video::Response::Buffer;
                            responseBuffer->width = info.width;
                            responseBuffer->height = info.height;
                            responseBuffer->lineSpan = info.lineSpan;
                            responseBuffer->pixelSpan = info.pixelSpan;
                            responseMap->Release();
                            endpoint->SendBuffer(response);
                            response->Release();
                        }
                            break;
                        case Video::Request::Dirty:
                            // Only do something if it's not memory mapped
                            if (message->PointerForOffset(PAGE_SIZE) != localMemory->PointerForOffset(PAGE_SIZE)) {
                                Video::DirtyRequest *dirtyRequest = (Video::DirtyRequest*)packet;
                                char *toPointer = ((char*)framebuffer) + ((dirtyRequest->y * info.lineSpan) + (dirtyRequest->x * info.pixelSpan));
                                char *fromPointer = ((char*)dirtyRequest->Framebuffer()) + ((dirtyRequest->y * info.lineSpan) + (dirtyRequest->x * info.pixelSpan));
                                int w = dirtyRequest->w * info.pixelSpan;
                                for (int i = dirtyRequest->h; i > 0; i--) {
                                    CopyMemory(toPointer, fromPointer, w);
                                    toPointer += info.lineSpan;
                                    fromPointer += info.lineSpan;
                                }
                            }
                            break;
                    }
                };
                connection->stop = [localMap, localMemory]{
                    localMap->Release();
                    localMemory->Release();
                };
                return connection;
            };
        }
            break;
        case tText:
        {
            IpcService *ipcService = new IpcService(KernelString::Format("%i", index), SERVICE_TYPE_CONSOLE);
            service = new Service(this, ipcService);
            ipcService->Release();
            service->start = [this, service, index](IpcEndpoint *endpoint){
                Console::WriteCharacters::Point outputCursor;
                Console::WriteCharacters::Move direction = Console::WriteCharacters::Move::mRight;
                int width, height;
                GetConsole(index, &width, &height, NULL, NULL);
                bicycle::function<void(Console::WriteCharacters::Move)> updateCursor = [this,&outputCursor,index, width, height](Console::WriteCharacters::Move direction){
                    bicycle::function<void(int,int,int,int)> blank = [this, index](int x, int y, int w, int h){
                        for (int ix = 0; ix < w; ix++)
                            for (int iy = 0; iy < h; iy++)
                                SetCharacter(index, ix + x, iy + y, ' ');
                    };
                    switch (direction) {
                        case Console::WriteCharacters::mNone:
                        default:
                            break;
                        case Console::WriteCharacters::mRight:
                            outputCursor.x++;
                            if (outputCursor.x >= width) {
                                outputCursor.x = 0;
                                outputCursor.y++;
                                if (outputCursor.y >= height) {
                                    MoveCharacters(index, 0, 1, 0, 0, width, height - 1);
                                    blank(0, height - 1, width, 1);
                                }
                            }
                            break;
                    }
                };
                Connection *connection = new Connection(this, service, endpoint);
                connection->message = [this, index, endpoint, &outputCursor, &direction, updateCursor](KernelBufferMemory *message){
                    KernelBufferMemory::Map *map = new KernelBufferMemory::Map(NULL, message, true);
                    Interface_Request *packet = (Interface_Request*)map->LinearBase();
                    switch (packet->type) {
                        case Console::Request::WriteCharacters:
                        {
                            Console::WriteCharacters *request = (Console::WriteCharacters*)packet;
                            FlatObject *command = request->commands.ItemAt(0);
                            for (UInt32 count = request->commands.Count(); count != 0; count--, command = command->NextObject()) {
                                if (command->type == Console::WriteCharacters::ShowCursor::Type()) {
                                    Console::WriteCharacters::ShowCursor *showCursor = (Console::WriteCharacters::ShowCursor*)command;
                                    ShowCursor(index, showCursor->Visible());
                                } else if (command->type == Console::WriteCharacters::SetCursor::Type()) {
                                    Console::WriteCharacters::SetCursor *setCursor = (Console::WriteCharacters::SetCursor*)command;
                                    Console::WriteCharacters::Point location(setCursor->Location());
                                    SetCursor(index, location.x, location.y);
                                } else if (command->type == Console::WriteCharacters::SetPrint::Type()) {
                                    Console::WriteCharacters::SetPrint *setPrint = (Console::WriteCharacters::SetPrint*)command;
                                    outputCursor = setPrint->Location();
                                } else if (command->type == Console::WriteCharacters::SetColour::Type()) {
                                    Console::WriteCharacters::SetColour *setColour = (Console::WriteCharacters::SetColour*)command;
                                    SetColour(index, setColour->Foreground(), setColour->Red(), setColour->Green(), setColour->Blue());
                                } else if (command->type == Console::WriteCharacters::SetDirection::Type()) {
                                    Console::WriteCharacters::SetDirection *setDirection = (Console::WriteCharacters::SetDirection*)command;
                                    direction = setDirection->Direction();
                                } else if (command->type == Console::WriteCharacters::MoveDirection::Type()) {
                                    Console::WriteCharacters::MoveDirection *moveDirection = (Console::WriteCharacters::MoveDirection*)command;
                                    updateCursor(moveDirection->Direction());
                                } else if (command->type == Console::WriteCharacters::MoveText::Type()) {
                                    Console::WriteCharacters::MoveText *moveText = (Console::WriteCharacters::MoveText*)command;
                                    Console::WriteCharacters::Point from(moveText->From());
                                    Console::WriteCharacters::Point to(moveText->To());
                                    Console::WriteCharacters::Point size(moveText->Size());
                                    MoveCharacters(index, from.x, from.y, to.x, to.y, size.x, size.y);
                                } else if (command->type == FlatString::Type()) {
                                    FlatString *output = (FlatString*)command;
                                    const char *data = output->Value();
                                    int length = output->Length();
                                    while (length) {
                                        SetCharacter(index, outputCursor.x, outputCursor.y, *data);
                                        updateCursor(direction);
                                        length--;
                                        data++;
                                    }
                                }
                            }
                        }
                            break;
                        case Console::Request::GetCapabilities:
                            endpoint->SendMessage([this, packet, index](void *buffer){
                                Console::Capabilities *response = (Console::Capabilities*)buffer;
                                response->Fill(packet);
                                GetConsole(index, &response->width, &response->height, &response->colour, &response->direction);
                                response->cursor = response->direction;
                                return true;
                            });
                            break;
                    }
                };
                connection->stop = []{
                    // TODO?
                };
                return connection;
            };
        }
            break;
        default:
            // Do nothing? No video output will appear on the driver
            service = NULL;
            break;
    }
    if (service != NULL) {
        _ports->Set(portIndex, service);
        Launch(service);
        service->Release();
    }
    portIndex->Release();
}
