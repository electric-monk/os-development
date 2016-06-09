#include "Blocking.h"
#include "Collections.h"
#include "System.h"

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
            return PrintInt(bob - 1, value);
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
    
    while(true);
}
