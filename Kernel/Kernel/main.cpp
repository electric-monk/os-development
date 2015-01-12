#include "debug.h"
#include "multiboot.h"
#include "mem_physical.h"
#include "tools.h"
#include "mem_logical.h"
#include "BasicHeap.h"
#include "StandardPC.h"
#include "CPU_intr.h"
#include "Scheduler.h"

extern UInt32 kern_start, kern_end;

extern UInt32 virt, phys;

BasicHeap *s_coreHeap;
static unsigned char s_coreMemory[65535];

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
static bool MultitaskHandler(void *context, void *state)
{
    Scheduler::EnterFromInterrupt();
    return true;
}

#include "Interrupts.h"
int taunt = 0;
static bool KeyboardTestHandler(void *context, void *state)
{
    unsigned char c = inb(0x60);
    kprintf("%.2x ", c);
    if (c == 0x39)
//        ((BasicHeap*)NULL)->AddBlock(0, 0);
        taunt++;
    return true;
}

static const char *testchars = "0123456789ABCDEFGHIKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
class TestThread : public KernelThread
{
public:
    TestThread(int x, int y, const char *msg)
    {
        _x = x;
        _y = y;
        _msg = msg;
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
                kprintf("Lose all marks in exam\n");
                break;
            }
        }
    }
private:
    int _x, _y;
    const char *_msg;
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
	memory_map_t *map;

    s_coreHeap = &mainHeap;
    mainHeap.AddBlock(s_coreMemory, sizeof(s_coreMemory));

	test('A');
	kprintf("Munro Systems\nCosmOS 1.0\nCopyright (C) 2008-2015 Colin Munro\n\n");
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		kprintf("Multiboot: magic number incorrect\n");
		return -1;
	}

    // Initialise memory management (including paging)
	CPhysicalMemory::Init();
    rootAddressSpace.InitKernel();
    
    // Parse memory from boot description
	mbd = (multiboot_info_t*)FixAddress(mbd);
	for (
		map = (memory_map_t*)FixAddress((void*)mbd->mmap_addr);
		map < (memory_map_t*)(((char*)FixAddress((void*)mbd->mmap_addr)) + mbd->mmap_length);
		map = (memory_map_t*)(((char*)map) + map->size + sizeof(map->size)))
	{
		if (map->type == 0x01)
		{
			// Usable RAM
			CPhysicalMemory::AddChunk((PhysicalPointer)map->base_addr_low, map->length_low);
		}
	}
	CPhysicalMemory::AddReserved((void*)(((UInt32)&phys) & 0xFFC00000), 0x00400000);
    
    StandardPC *rootDevice = new StandardPC();
    rootDevice->Start(NULL);

    rootDevice->Test()->RegisterHandler(0x21, KeyboardTestHandler, NULL);
    CPU_PIC_Enable(1, true);
    
    // Do something else
	kprintf("\nStarting!\n");
	test('Z');

    Thread::ConfigureService(rootDevice->Test());
    
    // Thread test
    one = new TestThread(51, 20, "Ahoy");
    two = new TestThread(50, 20, "Hello");
        // Set up timer
    int divisor = 1193180 / 1000/*Hz*/;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
    rootDevice->Test()->RegisterHandler(0x20, MultitaskHandler, NULL);
    
    // Process test
    ConfigureProcessTest(ProcessTestThreadOne, "Three");
    ConfigureProcessTest(ProcessTestThreadTwo, "Four");
    rootDevice->Test()->RegisterHandler(0x99, TestHandler, NULL);
    rootDevice->Test()->ConfigureSyscall(0x99);
    
    CPU_Interrupt_Disable();
    CPU_PIC_Enable(0, true);
    Scheduler::BeginScheduling();
//    one->_context->SwitchFrom(&rootDevice->GetCPU(0)->scheduler);
    kprintf("What happen\n");
    
    // Start scheduler?
    while (1) asm("hlt");
    
    // Shut down?
    rootDevice->Stop();
    
	return 0;
}