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
#include "Thread.h"
#include "IPC_Manager.h"

// Values from the linker
extern UInt32 kern_start, kern_end;
extern UInt32 virt, phys;
// System heap
BasicHeap *s_coreHeap;
static unsigned char s_coreMemory[65535*8*2];
// Global root device driver
Driver *s_rootDevice;

#include "Interrupts.h"
static bool TestPrint(void *context, void *state)
{
    TrapFrame *tf = (TrapFrame*)state;
    kprintf("%s", (char*)tf->EAX);
    return true;
}

static inline void* FixAddress(void *address)
{
    return ((char*)address) + (UInt32)&virt - (UInt32)&phys;
}

extern "C" int k_main(multiboot_info_t* mbd, unsigned int magic)
{
    BasicHeap mainHeap(128);
	multiboot_mmap_entry *map;

    s_coreHeap = &mainHeap;
    mainHeap.AddBlock(s_coreMemory, sizeof(s_coreMemory));

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
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
		map = (multiboot_mmap_entry*)(((char*)map) + map->size + sizeof(map->size))) {
		if (map->type == MULTIBOOT_MEMORY_AVAILABLE) {
			// Usable RAM
			CPhysicalMemory::AddChunk((PhysicalPointer)map->addr, map->len);
		}
	}
	CPhysicalMemory::AddReserved((void*)(((UInt32)&phys) & 0xFFC00000), 0x00400000);
    // Initialise video (after memory - it may be required to allocate pages even for just a graphical console)
    Init_Video_Multiboot(FixAddress((void*)mbd->vbe_control_info), FixAddress((void*)mbd->vbe_mode_info), mbd->vbe_mode);

    // Splash
	kprintf("Munro Systems\nCopyright (C) 2008-2016 Colin Munro\n");
    
    // Start root driver
    s_rootDevice = new StandardPC();
    s_rootDevice->Start(NULL);

    // Configure system services
    Thread::ConfigureService();
    Collections::ConfigureService();
    BlockableObject::ConfigureService();
    KernelObject::ConfigureService();
    Driver::ConfigureService();
    IpcServiceMonitor::ConfigureService();
 
    AutoreleasePool testPool;
    s_rootDevice->Test()->RegisterHandler(0xff, TestPrint, NULL);
    s_rootDevice->Test()->ConfigureSyscall(0xff);
    
    // Configure startup process
    InitStartup();
    
    // Begin running
    CPU_Interrupt_Disable();
    Scheduler::BeginScheduling();
    
    // Shut down?
    while (1) asm("hlt");
    s_rootDevice->Stop();
    
	return 0;
}
