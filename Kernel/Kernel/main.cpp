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
//    CPU_Interrupt_Disable();
//    if (entered > 0) {
//        x++;
//        kprintf("Reentry? %i (%i/%i)\n", entered, x, y);
//        return true;
//    } else y++;
//    entered++;
//    Thread *old = ThreadAt(threadCount);
//    threadCount = (threadCount + 1) % 2;
//    ThreadAt(threadCount)->_context->SwitchFrom(&old->_context);
//    entered--;
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
class TestThread : public Thread
{
public:
    TestThread(int x, int y, char *msg)
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
    char *_msg;
};
class FirstThread : public TestThread
{
public:
    FirstThread(int x, int y, char *msg)
    :TestThread(x, y, msg)
    {}
protected:
    void ThreadMain(void)
    {
//        CPU_PIC_Enable(0, true);
        TestThread::ThreadMain();
    }
};

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
	kprintf("Munro Systems\nCosmOS 1.0\nCopyright (C) 2008-2014 Colin Munro\n\n");
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
//	kcls();
//	kprintf("Multiboot: %iKb low mem, %iKb high mem:\n", mbd->mem_lower, mbd->mem_upper);
	for (
		map = (memory_map_t*)FixAddress((void*)mbd->mmap_addr);
		map < (memory_map_t*)(((char*)FixAddress((void*)mbd->mmap_addr)) + mbd->mmap_length);
		map = (memory_map_t*)(((char*)map) + map->size + sizeof(map->size)))
	{
		if (map->type == 0x01)
		{
			// Usable RAM
//			kprintf("\t0x%.8x: %i bytes\n", map->base_addr_low, map->length_low);
			CPhysicalMemory::AddChunk((PhysicalPointer)map->base_addr_low, map->length_low);
		}
	}
//	CPhysicalMemory::AddReserved((void*)(((UInt32)&phys) & 0xFFC00000), ((char*)&kern_end) - ((char*)&kern_start));
	CPhysicalMemory::AddReserved((void*)(((UInt32)&phys) & 0xFFC00000), 0x00400000);
//    kprintf("Stack at 0x%.8x\n", &stack);

//    InitDebug();
    
    StandardPC *rootDevice = new StandardPC();
    rootDevice->Start(NULL);

    rootDevice->Test()->RegisterHandler(0x21, KeyboardTestHandler, NULL);
    CPU_PIC_Enable(1, true);
    
    // Do something else
	kprintf("\nStarting!\n");
	test('Z');

    Thread::ConfigureService(rootDevice->Test());
    
    // Thread test
    one = new FirstThread(51, 20, "Ahoy");
    two = new TestThread(50, 20, "Hello");
        // Set up timer
    int divisor = 1193180 / 1000/*Hz*/;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
    rootDevice->Test()->RegisterHandler(0x20, MultitaskHandler, NULL);
    CPU_PIC_Enable(0, true);
    
    CPU_Interrupt_Disable();
    Scheduler::BeginScheduling();
//    one->_context->SwitchFrom(&rootDevice->GetCPU(0)->scheduler);
    kprintf("What happen\n");
    
    // Start scheduler?
    while (1) asm("hlt");
    
    // Shut down?
    rootDevice->Stop();
    
	return 0;
}
