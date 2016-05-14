#include "debug.h"
#include "multiboot.h"
#include "mem_physical.h"
#include "tools.h"
#include "mem_logical.h"
#include "BasicHeap.h"
#include "StandardPC.h"
#include "CPU_intr.h"
#include "Scheduler.h"
#include "mem_virtual.h"
#include "Interface.h"
#include "fs_iso9660.h"
#include "Startup.h"
#include "Video_Multiboot.h"

extern UInt32 kern_start, kern_end;

extern UInt32 virt, phys;

BasicHeap *s_coreHeap;
static unsigned char s_coreMemory[65535*8*2];

#include "Thread.h"
Thread *one, *two;
int threadCount = 0;
static Thread* ThreadAt(int index)
{
    if (index == 0)
        return one;
    else
        return two;
}
static int entered = -1;
int x = 0, y = 0;

#include "Queue.h"
DispatchQueue *testQueue;
class TestTask : public DispatchQueue::Task
{
public:
    TestTask(char c, int x = 50, int y = 22)
    {
        _x = x;
        _y = y;
        _c = c;
    }
    
protected:
    void Execute(void)
    {
        test(_x, _y, _c);
    }
    
private:
    int _x, _y;
    char _c;
};

#include "IPC.h"

class TestServiceWatcher : public IpcServiceWatcher
{
public:
    void ServiceProviderAppeared(KernelObject *provider)
    {
        kprintf("Provider started: %.8x: %s\n", provider, provider->GetClassName(0));
    }
    
    void ServiceAppeared(KernelObject *provider, IpcService *service)
    {
        kprintf("Service started on %.8x: %.8x [%s / %s]\n", provider, service, service->Name()->CString(), service->ServiceType()->CString());
    }

    void ServiceChanged(KernelObject *provider, IpcService *service)
    {
        kprintf("Service changed on %.8x: %.8x\n", provider, service);
    }
    
    void ServiceRemoved(KernelObject *provider, IpcService *service)
    {
        kprintf("Service stopped on %.8x: %.8x [%s / %s]\n", provider, service, service->Name()->CString(), service->ServiceType()->CString());
    }
    
    void ServiceProviderRemoved(KernelObject *provider)
    {
        kprintf("Provider stopped: %.8x\n", provider);
    }
};

#include "Interrupts.h"
int taunt = 0;
static bool KeyboardTestHandler(void *context, void *state)
{
    unsigned char c = inb(0x60);
    kprintf("%.2x ", c);
    if (c == 0x39)
//        ((BasicHeap*)NULL)->AddBlock(0, 0);
        taunt++;
    if (c == 0x14)
        Thread::DebugPrint();
    
    return true;
}

static const char *testchars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
class TestThread : public KernelThread
{
public:
    TestThread(int x, int y, const char *msg, int sleep = 0)
    {
        _x = x;
        _y = y;
        _msg = msg;
        _sleep = sleep;
    }
protected:
    void ThreadMain(void)
    {
        kprintf("%s 0x%.8x\n", _msg, this);
        int i = 1;
        while (1 || (i != 0)) {
            test(_x, _y, testchars[i]);
            i++;
            if (testchars[i] == '\0')
                i = 0;
            if ((_x - 49) == taunt) {
//                kprintf("Lose all marks in exam\n");
//                break;
                s_coreHeap->Test();
            }
            if (_sleep) {
                test(12, 0, '?');
                Thread::Active->Sleep(MILLISECONDS(_sleep));
                test(12, 0, '!');
                
                TestTask *testTask = new TestTask('W');
//                testQueue->AddTask(testTask);
                testTask->Release();
            } else {
                _count++;
                if (_count > 100000) {
                    _count = 0;
                    TestTask *testTask = new TestTask('N');
//                    testQueue->AddTask(testTask);
                    testTask->Release();
                    
                }
            }
        }
    }
private:
    int _x, _y;
    const char *_msg;
    int _sleep;
    int _count;
};

#include "Process.h"
static bool TestHandler(void *context, void *state)
{
    TrapFrame *tf = (TrapFrame*)state;
    test('>');
    test(tf->EBX, tf->ECX, tf->EAX);
    return true;
}
extern "C" void ProcessTestThreadOne(void*);
extern "C" void ProcessTestThreadTwo(void*);
static void ConfigureProcessTest(void (*test)(void*), const char *name)
{
    Process *testProcess = new Process(name);
    PhysicalPointer page = CPhysicalMemory::AllocateContiguousPages();
    void *linearPage = testProcess->pageDirectory.Map(fmWritable | fmUser, pmApplication, page);
    testProcess->pageDirectory.Select();
    CopyMemory(linearPage, (void*)test, 4096);
    new Thread(testProcess, (void(*)(void*))linearPage, NULL);
}

static inline void* FixAddress(void *address)
{
    return ((char*)address) + (UInt32)&virt - (UInt32)&phys;
}
extern UInt32 stack;
extern "C" int k_main(multiboot_info_t* mbd, unsigned int magic)
{
    BasicHeap mainHeap(128);
	multiboot_mmap_entry *map;

    s_coreHeap = &mainHeap;
    mainHeap.AddBlock(s_coreMemory, sizeof(s_coreMemory));

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		kprintf("Multiboot: magic number incorrect\n");
		return -1;
	}

    // Initialise memory management (including paging)
	CPhysicalMemory::Init();
    rootAddressSpace.InitKernel();
    
    // Process Multiboot information
	mbd = (multiboot_info_t*)FixAddress(mbd);
    // Add memory
	for (
		map = (multiboot_mmap_entry*)FixAddress((void*)mbd->mmap_addr);
		map < (multiboot_mmap_entry*)(((char*)FixAddress((void*)mbd->mmap_addr)) + mbd->mmap_length);
		map = (multiboot_mmap_entry*)(((char*)map) + map->size + sizeof(map->size)))
	{
		if (map->type == MULTIBOOT_MEMORY_AVAILABLE)
		{
			// Usable RAM
			CPhysicalMemory::AddChunk((PhysicalPointer)map->addr, map->len);
		}
	}
	CPhysicalMemory::AddReserved((void*)(((UInt32)&phys) & 0xFFC00000), 0x00400000);
    // Initialise video (after memory - it may be required to allocate pages even for just a graphical console)
    Init_Video_Multiboot(FixAddress((void*)mbd->vbe_control_info), FixAddress((void*)mbd->vbe_mode_info), mbd->vbe_mode);

    // Splash
	kprintf("Munro Systems\nCosmOS 1.0\nCopyright (C) 2008-2016 Colin Munro\n\n");
    
    TestServiceWatcher *watcher = new TestServiceWatcher();
    IpcServiceList::Register(watcher);
    
    StandardPC *rootDevice = new StandardPC();
    rootDevice->Start(NULL);

    rootDevice->Test()->RegisterHandler(0x21, KeyboardTestHandler, NULL);
    
    // Do something else
	kprintf("\nStarting!\n");
	test('Z');
    
    bicycle::function<int(void)> test = [rootDevice](){
        kprintf("Lambda %.8x\n", rootDevice);
        return 0;
    };
    test();

    Thread::ConfigureService();
    Collections::ConfigureService();
    BlockableObject::ConfigureService();
    KernelObject::ConfigureService();
    InitStartup();
    
    AutoreleasePool testPool;
    // Thread test
    one = new TestThread(51, 20, "Ahoy");
    two = new TestThread(50, 20, "Hello", 250);
    
    // Process test
    ConfigureProcessTest(ProcessTestThreadOne, "Three");
    ConfigureProcessTest(ProcessTestThreadTwo, "Four");
    rootDevice->Test()->RegisterHandler(0x99, TestHandler, NULL);
    rootDevice->Test()->ConfigureSyscall(0x99);
    
    // Queue test
    testQueue = new DispatchQueue();
    TestTask *testTask = new TestTask('?');
    testQueue->AddTask(testTask);
    testTask->Release();

    CPU_Interrupt_Disable();
    Scheduler::BeginScheduling();
    kprintf("What happen\n");
    
    // Start scheduler?
    while (1) asm("hlt");
    
    // Shut down?
    rootDevice->Stop();
    
	return 0;
}
