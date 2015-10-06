#include "GenericKeyboard.h"
#include "IPC.h"
#include "Runloop.h"
#include "debug.h"
#include "Interface_Keyboard.h"

const struct {
    bool extended;
    UInt8 code;
    Interface_Keyboard::KEY key;
} s_keys[] = {
    {false, 0x0B, Interface_Keyboard::key0},
    {false, 0x02, Interface_Keyboard::key1},
    {false, 0x03, Interface_Keyboard::key2},
    {false, 0x04, Interface_Keyboard::key3},
    {false, 0x05, Interface_Keyboard::key4},
    {false, 0x06, Interface_Keyboard::key5},
    {false, 0x07, Interface_Keyboard::key6},
    {false, 0x08, Interface_Keyboard::key7},
    {false, 0x09, Interface_Keyboard::key8},
    {false, 0x0A, Interface_Keyboard::key9},
    {false, 0x1E, Interface_Keyboard::keyA},
    {false, 0x30, Interface_Keyboard::keyB},
    {false, 0x2E, Interface_Keyboard::keyC},
    {false, 0x20, Interface_Keyboard::keyD},
    {false, 0x12, Interface_Keyboard::keyE},
    {false, 0x21, Interface_Keyboard::keyF},
    {false, 0x22, Interface_Keyboard::keyG},
    {false, 0x23, Interface_Keyboard::keyH},
    {false, 0x17, Interface_Keyboard::keyI},
    {false, 0x24, Interface_Keyboard::keyJ},
    {false, 0x25, Interface_Keyboard::keyK},
    {false, 0x26, Interface_Keyboard::keyL},
    {false, 0x32, Interface_Keyboard::keyM},
    {false, 0x31, Interface_Keyboard::keyN},
    {false, 0x18, Interface_Keyboard::keyO},
    {false, 0x19, Interface_Keyboard::keyP},
    {false, 0x10, Interface_Keyboard::keyQ},
    {false, 0x13, Interface_Keyboard::keyR},
    {false, 0x1F, Interface_Keyboard::keyS},
    {false, 0x14, Interface_Keyboard::keyT},
    {false, 0x16, Interface_Keyboard::keyU},
    {false, 0x2F, Interface_Keyboard::keyV},
    {false, 0x11, Interface_Keyboard::keyW},
    {false, 0x2D, Interface_Keyboard::keyX},
    {false, 0x15, Interface_Keyboard::keyY},
    {false, 0x2C, Interface_Keyboard::keyZ},
    {false, 0x39, Interface_Keyboard::keySpace},
    {false, 0x0E, Interface_Keyboard::keyBackspace},
    {false, 0x1C, Interface_Keyboard::keyReturn},
    {false, 0x01, Interface_Keyboard::keyEscape},
    {true,  0x4B, Interface_Keyboard::keyLeft},
    {true,  0x48, Interface_Keyboard::keyUp},
    {true,  0x4D, Interface_Keyboard::keyRight},
    {true,  0x50, Interface_Keyboard::keyDown},
    {false, 0x2A, Interface_Keyboard::keyShiftLeft},
    {false, 0x36, Interface_Keyboard::keyShiftRight},
    {false, 0x1D, Interface_Keyboard::keyControlLeft},
    {true,  0x1D, Interface_Keyboard::keyControlRight},
    {false, 0x38, Interface_Keyboard::keyAltLeft},
    {true,  0x38, Interface_Keyboard::keyAltRight},
    {false, 0x52, Interface_Keyboard::keyPad0},
    {false, 0x4F, Interface_Keyboard::keyPad1},
    {false, 0x50, Interface_Keyboard::keyPad2},
    {false, 0x51, Interface_Keyboard::keyPad3},
    {false, 0x4B, Interface_Keyboard::keyPad4},
    {false, 0x4C, Interface_Keyboard::keyPad5},
    {false, 0x4D, Interface_Keyboard::keyPad6},
    {false, 0x47, Interface_Keyboard::keyPad7},
    {false, 0x48, Interface_Keyboard::keyPad8},
    {false, 0x49, Interface_Keyboard::keyPad9},
    {true,  0x37, Interface_Keyboard::keyPadStar},
    {true,  0x35, Interface_Keyboard::keyPadSlash},
    {false, 0x4E, Interface_Keyboard::keyPadPlus},
    {false, 0x4A, Interface_Keyboard::keyPadMinus},
    {true,  0x1C, Interface_Keyboard::keyPadEnter},
};

KernelDictionary *problemDictionary;

static KernelObject* MakeKey(bool extended, UInt8 code)
{
    UInt32 value = code;
    if (extended)
        value |= 0xFF000000;
    return new KernelNumber(value);
}

static void MakeMap(KernelDictionary *dictionary, bool extended, UInt8 code, Interface_Keyboard::KEY key)
{
    KernelObject *dKey = MakeKey(extended, code);
    KernelNumber *value = new KernelNumber(key);
    dictionary->Set(dKey, value);
    dKey->Release();
    value->Release();
}

GenericKeyboard::GenericKeyboard(const char *name)
:ProviderDriver(name)
{
    _serviceList = new IpcServiceList(this);
    _mapping = new KernelDictionary();
    _pressed = new KernelArray();
    
    // Make map
    for (int i = 0; i < (sizeof(s_keys) / sizeof(s_keys[0])); i++)
        MakeMap(_mapping, s_keys[i].extended, s_keys[i].code, s_keys[i].key);
    
    problemDictionary = _mapping;
}

GenericKeyboard::~GenericKeyboard()
{
    _serviceList->Release();
    _pressed->Release();
    _mapping->Release();
}

bool GenericKeyboard::Start(Driver *parent)
{
    _thread = new RunloopThread(NULL);
    _extended = false;
    IpcService *ipcService = new IpcService("keyboard"_ko, "keyboard"_ko);
    Service *service = new Service(this, ipcService);
    ipcService->Release();
    Launch(service);
    service->Release();
    _identifierCount = 0;
    return Driver::Start(parent);
}

void GenericKeyboard::Stop(void)
{
    Terminate((Service*)_serviceList->ServiceList()->ObjectAt(0));
    _thread->Release();
    Driver::Stop();
}

bool GenericKeyboard::HandleKey(UInt8 event)
{
    if (event == 0xE0) {
        _extended = true;
        return true;
    }
    bool keyUp = event & 0x80;
    event &= ~0x80;
    bool extended = _extended;
    _extended = false;
    _thread->AddTask([this, keyUp, event, extended]{
        KernelObject *key = MakeKey(extended, event);
        KernelNumber *value = (KernelNumber*)_mapping->ObjectFor(key);
        if (value) {
            bool contains = _pressed->Contains(key);
            if (contains && keyUp) {
                _pressed->Remove(key);
                SendEvent(value->Value(), false);
            } else if (!contains && !keyUp) {
                _pressed->Add(key);
                SendEvent(value->Value(), true);
            }
        }
        key->Release();
        return 0;
    });
    return false;
}

void GenericKeyboard::SendEvent(int key, bool down)
{
    int currentIdentifier = _identifierCount++;
    _connections->Enumerate([currentIdentifier, key, down](KernelObject *object){
        Connection *connection = (Connection*)object;
        connection->Link()->SendMessage([currentIdentifier, key, down](void *buffer)->bool{
            Interface_Keyboard::Event *event = (Interface_Keyboard::Event*)buffer;
            event->packetClass = Interface_Packet::Event;
            event->identifier = currentIdentifier;
            event->type = 0/*todo*/;
            event->keyDown = down;
            event->keyValue = (Interface_Keyboard::KEY)key;
            return true;
        });
        return (void*)NULL;
    });
//    kprintf("Key %s: %c\n", down ? "down" : "up", (char)key);
}

class GenericKeyboard_Connection : public ProviderDriver::Connection
{
public:
    GenericKeyboard_Connection(GenericKeyboard *owner, ProviderDriver::Service *service, IpcEndpoint *endpoint)
    :ProviderDriver::Connection(owner, service, endpoint)
    {
    }
};

ProviderDriver::Connection* GenericKeyboard::ConnectionStart(Service *service, IpcEndpoint *endpoint)
{
    return new GenericKeyboard_Connection(this, service, endpoint);
}

void GenericKeyboard::ConnectionReceive(Connection *connection, KernelBufferMemory *message)
{
    // TODO
}

void GenericKeyboard::ConnectionStop(Connection *connection)
{
    connection->Release();
}
