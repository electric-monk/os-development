#include "Blocking.h"
#include "Collections.h"
#include "System.h"
#include "IPC.h"

extern "C" void testprint(const char*);

static char* PrintInt(char *bob, int value)
{
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

bool Compare(Kernel::Collections::String *string, const char *other)
{
    char buf[100];
    string->Copy(buf, sizeof(buf));
    int i = 0;
    while ((buf[i] == other[i]) && (buf[i] != '\0') && (other[i] != '\0'))
        i++;
    return buf[i] == other[i];
}

extern "C" void sysmain(void)
{
    char buf[10];
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
    Kernel::Collections::Array *array = monitor->Changes();
    int length = array->Count();
    Kernel::Collections::String *stateStr = Kernel::Collections::String::Create("State");
    Kernel::Collections::String *typeStr = Kernel::Collections::String::Create("Type");
    Kernel::Collections::String *connectorStr = Kernel::Collections::String::Create("Connector");
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
            }
        }
    }
    monitor->Release();
    
    asm("cli");
    
    while(true);
}
