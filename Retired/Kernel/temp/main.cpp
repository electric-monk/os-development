#include "debug.h"
#include "multiboot.h"
#include "mem_physical.h"
#include "tools.h"
#include "mem_logical.h"

extern UInt32 kern_start, kern_end;

extern UInt32 virt, phys;

static inline void* FixAddress(void *address)
{
    return ((char*)address) + (UInt32)&virt - (UInt32)&phys;
}
extern UInt32 stack;
extern "C" int k_main(multiboot_info_t* mbd, unsigned int magic)
{
	memory_map_t *map;

	test('A');
	kprintf("Munro Systems\nCosmOS 1.0\nCopyright (C) 2008-2013 Colin Munro\n\n");
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
	kprintf("Multiboot: %iKb low mem, %iKb high mem:\n", mbd->mem_lower, mbd->mem_upper);
	for (
		map = (memory_map_t*)FixAddress((void*)mbd->mmap_addr);
		map < (memory_map_t*)(((char*)FixAddress((void*)mbd->mmap_addr)) + mbd->mmap_length);
		map = (memory_map_t*)(((char*)map) + map->size + sizeof(map->size)))
	{
		if (map->type == 0x01)
		{
			// Usable RAM
			kprintf("\t0x%.8x: %i bytes\n", map->base_addr_low, map->length_low);
			CPhysicalMemory::AddChunk((PhysicalPointer)map->base_addr_low, map->length_low);
		}
	}
	kprintf("Reserving...\n");
	CPhysicalMemory::AddReserved((void*)&kern_start, ((char*)&kern_end) - ((char*)&kern_start));
    kprintf("Stack at 0x%.8x\n", &stack);

    InitDebug();

    // Do something else
	kprintf("\nStarting!\n");
	test('Z');
	return 0;
}
