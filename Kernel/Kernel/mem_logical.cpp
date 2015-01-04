#include "mem_logical.h"
#include "debug.h"

SPageDirectoryInfo rootAddressSpace;
SPageDirectoryInfo *currentAddressSpace;

// From assembly code
extern SPageDirectory BootPageDirectory;     // Virtual address of the boot page directory
extern SPageTable PageMapBootstrap;      // A page to use to start off mapping new pages

// From the linker script
extern UInt32 virt;                  // "int" at base address of the kernel
extern UInt32 phys;                  // "int" at base address of the kernel

int availableMappingPages;

static inline void InvalidateTLB(void *m)
{
    asm volatile("invlpg %0"
                 :          // no output
                 : "m"(*reinterpret_cast<char*>(m))
                 : "memory"
                 );
}

void SPageDirectoryInfo::InitKernel(void)
{
//    kprintf("Kernel logical 0x%.8x, physical 0x%.8x\n", (int)&phys, (int)&virt);
    
    currentAddressSpace = this;
    tablesPhysical = &BootPageDirectory;
    directoryPhysical = (PhysicalPointer)(reinterpret_cast<UInt32>(&phys) + ((unsigned int)&BootPageDirectory) - reinterpret_cast<UInt32>(&virt));
    
    revision = 1;
    
    // Dump the bootup temporary mapping
    tablesPhysical[0].Present = 0;
    InvalidateTLB(0x00000000);
    
    // Get virtual pointers : assume currently that kernel only uses 4MB pages
    for (int i = 0; i < 1024; i++)
    {
        if (tablesPhysical[i].Present)
        {
            // Page is present
            if (tablesPhysical[i].PageSize)
            {
                // 4MB page
                tables[i] = (SPageTable*)0xFFFFFFFF;
//                kprintf("4MB page found at %i\n", i);
            }
            else
            {
                // WHAT TO DO?
            }

        }
        else
        {
            tables[i] = NULL;
        }
    }
    
    // Stuff in the utility mapping page in the last slot of the address space
    PhysicalPointer bootstrapPhysical = (PhysicalPointer)(reinterpret_cast<UInt32>(&phys) + reinterpret_cast<UInt32>(&PageMapBootstrap) - reinterpret_cast<UInt32>(&virt));
    availableMappingPages = 1023;   // One will be used, to point to itself
    for (int i = 0; i < 1024; i++)
        PageMapBootstrap.pages[i].RawValue = 0x00000000;
    PageMapBootstrap.pages[1023].Frame = ((unsigned int)bootstrapPhysical & 0xFFFFF000) >> 12;
    PageMapBootstrap.pages[1023].Writable = 1;
    PageMapBootstrap.pages[1023].Present = 1;
        // Hook it up
    tablesPhysical[1023].Frame = ((unsigned int)bootstrapPhysical & 0xFFFFF000) >> 12;
    tablesPhysical[1023].Available = 1;
    tablesPhysical[1023].Writable = 1;
    tablesPhysical[1023].Present = 1;
    tables[1023] = (SPageTable*)0xFFFFF000;  // We just set this up
    InvalidateTLB(tables[1023]);
}

void SPageDirectoryInfo::Init(void)
{
    // Create a new directory
    directoryPhysical = CPhysicalMemory::AllocateContiguousPages();
    tablesPhysical = (SPageDirectory*)Map(fmWritable, pmKernel, directoryPhysical);
    
    // Empty structure
    for (int i = 0; i < 1024; i++)
    {
        tables[i] = NULL;
        tablesPhysical[i].RawValue = 0x00000000;
    }
    
    // Invalidate 'kernel version' field so first switch to this address space updates to the right kernel data
    revision = 0;
}

void SPageDirectoryInfo::Release(void)
{
    // Release any mapped pages
    for (int i = 0; i < (((UInt32)&phys) >> 22); i++)
    {
        if (!tablesPhysical[i].Present)
            continue;
        
        if (tablesPhysical[i].PageSize)
        {
            // Weird - 4MB page should only be for kernel?
            continue;
        }
        
        for (int j = 0; j < 1024; j++)
        {
            if (!tables[i]->pages[j].Present)
                continue;
            CPhysicalMemory::ReleasePages((PhysicalPointer)(tables[i]->pages[j].Frame << 12));
        }
    }
    
    // Release the main directory
    Unmap(tablesPhysical);
    CPhysicalMemory::ReleasePages(directoryPhysical);
}

void SPageDirectoryInfo::Select(void)
{
    if (&rootAddressSpace != this)
    {
        if (revision != rootAddressSpace.revision)
        {
            CopyKernel();
        }
    }
    asm volatile("mov %0, %%cr3"::"r" (directoryPhysical));
    currentAddressSpace = this; // WARN: Will this be picked up by thread switching?
}

void* SPageDirectoryInfo::Map(int permissions, MAP_TYPE location, PhysicalPointer physical, size_t count)
{
//    kprintf("SPageDirectoryInfo::Map(%.4x, %.2x, 0x%.8x, %i)\n", permissions, location, (int)physical, (int)count);

    UInt32 min, max;
    bool searchReverse;

    void *newLocation, *currentLocation;
    size_t foundLength;
    
//    test('a');
    if ((location == pmKernel) && (this != &rootAddressSpace))
    {
//        test('b');
        newLocation = rootAddressSpace.Map(permissions, location, physical, count);
        CopyKernel();
        return newLocation;
    }
    
//    test('c');
    // Validate input
    searchReverse = false;
    switch (location)
    {
        case pmApplication:
//            test('d');
            if (permissions & fmPageData)
                return NULL;
            min = 0x00001000;
            max = ((UInt32)&virt) - 0x1000;
            break;
        case pmKernel:
//            test('e');
            if (permissions & fmPageData)
            {
                if (count != 1)
                    return NULL;
                searchReverse = true;
                // TODO: Search only present page tables
            }
            min = (UInt32)&virt;
            max = 0xFFFFF000;
            break;
        default:
            return NULL;
    }
    
    // Do the search
    // TODO: Search properly, using page directory and tables
//    kprintf("Searching %s between 0x%.8x and 0x%.8x (kernel base 0x%.8x)\n",
//            searchReverse ? "down" : "up",
//            (void*)min, (void*)max, (void*)&virt);
    newLocation = NULL;
    currentLocation = (void*)(searchReverse ? max : min);
    foundLength = 0;
//    test('f');
    while (foundLength < count)
    {
        if ((currentLocation < (void*)min) || (currentLocation > (void*)max))
            return NULL;
        
        PhysicalPointer ptr = Address(currentLocation);
        if (Address(currentLocation) == CPhysicalMemory::Invalid)
        {
            if (foundLength == 0)
                newLocation = currentLocation;
            foundLength++;
        }
        else
        {
            foundLength = 0;
        }
        
        if (searchReverse)
            currentLocation = (void*)(((UInt32)currentLocation) - 0x1000);
        else
            currentLocation = (void*)(((UInt32)currentLocation) + 0x1000);
    }
    if (newLocation == NULL)
        return NULL;
    
    if ((permissions & fmPageData) && (availableMappingPages == 1))
    {
//        test('g');
        int pageDirEntry;
        
        // Find empty directory for it
        for (pageDirEntry = 1023; pageDirEntry >= 0; pageDirEntry--)
        {
            if (!tablesPhysical[pageDirEntry].Present)
                break;
        }
        if (pageDirEntry < 0)
            return NULL;
        
//        test('h');
        // We need to map a new page into this one, then get a new entry from the new page for our result
        PhysicalPointer newPage = CPhysicalMemory::AllocateContiguousPages();
        Map(fmWritable, newLocation, newPage);  // Should succeed, as we're using a 'mapping' entry that shouldn't need a new page
        SPageTable *newTable = (SPageTable*)newLocation;
        
//        test('i');
        // Scrub table
        for (int i = 0; i < 1024; i++)
            newTable->pages[i].RawValue = 0x00000000;
        
//        test('j');
        // Stuff page into page directory
        tablesPhysical[pageDirEntry].Frame = ((unsigned int)newPage & 0xFFFFF000) >> 12;
        tablesPhysical[pageDirEntry].Writable = 1;
        tablesPhysical[pageDirEntry].Available = 1;
        tablesPhysical[pageDirEntry].Present = 1;
        tables[pageDirEntry] = newTable;
        
        // Fix up the number of available mapping pages
        availableMappingPages += 1024 - 1;  // 1024 for new empty page, -1 for the one we just used
        
        // Re-compute newLocation (given that in this mode we should always only have a count of 1)
        newLocation = (void*)((pageDirEntry << 22) | (0xFFF << 12));
    }
    
//    test('k');
    // We found a big enough gap, so fill it in
    for (foundLength = 0, currentLocation = newLocation;
         foundLength < count;
         foundLength++, currentLocation = ((unsigned char*)currentLocation) + 0x1000, physical = ((unsigned char*)physical) + 0x1000)
        Map(permissions, currentLocation, physical);
    
//    test('l');
    // Increment 'kernel address space' counter so other processes know to update themselves
    if (this == &rootAddressSpace)
    {
        revision++;
        if (revision == 0)
            revision = 1;
    }
    
//    test('m');
    // Done
//    kprintf("Generated new mapping at 0x%.8x\n", newLocation);
    return newLocation;
}

void SPageDirectoryInfo::Map(int permissions, void *location, PhysicalPointer physical)
{
//    kprintf("SPageDirectoryInfo::Map(%.4x, 0x%.8x, 0x%.8x)\n", permissions, (int)location, (int)physical);
//    test('0');
    // Get the indices out of the pointer
    unsigned int dirEntry = (((unsigned int)location) >> 22) & 0x3FF;
    unsigned int pagEntry = (((unsigned int)location) >> 12) & 0x3FF;
    unsigned int pagOffset = ((unsigned int)location) & 0xFFF;
    
    // Get the table directory and check the page is present
    if (!tablesPhysical[dirEntry].Present)
    {
//        test('1');
        PhysicalPointer newPage = CPhysicalMemory::AllocateContiguousPages();
//        test('2');
        tables[dirEntry] = (SPageTable*)Map(fmPageData | fmWritable, pmKernel, newPage);
        
//        test('3');
        // Scrub the new table
        for (int i = 0; i < 1024; i++) {
            tables[dirEntry]->pages[i].RawValue = 0x00000000;
        }
        
//        test('4');
        // Set up the new entry
        SPageDirectory newDirectoryEntry;
        newDirectoryEntry.RawValue = 0x00000000;
        newDirectoryEntry.Frame = ((unsigned int)(((UInt32)newPage) & 0xFFFFF000)) >> 12;
        newDirectoryEntry.Present = 1;
        newDirectoryEntry.Writable = 1;
        newDirectoryEntry.User = 0;
        newDirectoryEntry.WriteThrough = 0;
        newDirectoryEntry.CacheDisable = 0;
        newDirectoryEntry.Accessed = 0;
        newDirectoryEntry.Available = 1;
        newDirectoryEntry.PageSize = 0; // 4Kb
        newDirectoryEntry.Global = 0;   // Not global
        tablesPhysical[dirEntry] = newDirectoryEntry;
    }
    
//    test('5');
    SPage newPageInfo;
    newPageInfo.RawValue = 0x00000000;
    newPageInfo.Present = 1;
    if (permissions & fmWritable)
        newPageInfo.Writable = 1;
    if (permissions & fmUser)
        newPageInfo.User = 1;
    newPageInfo.Frame = ((unsigned int)(((UInt32)physical) & 0xFFFFF000)) >> 12;

    // Activate the mapping
//    test('6');
    tables[dirEntry]->pages[pagEntry] = newPageInfo;
//    test('7');
    InvalidateTLB(location);
//    kprintf("0x%.8x mapped\n", location);
}

void SPageDirectoryInfo::Unmap(void *logical)
{
    Unmap(logical, false);
}

void SPageDirectoryInfo::Unmap(void *logical, bool mapPage)
{
    // Just in case
    if (logical == &PageMapBootstrap)
        return;
    
    // Get the indices out of the pointer
    unsigned int dirEntry = (((unsigned int)logical) >> 22) & 0x3FF;
    unsigned int pagEntry = (((unsigned int)logical) >> 12) & 0x3FF;
    unsigned int pagOffset = ((unsigned int)logical) & 0xFFF;
    
    // Check it's there
    if (!tablesPhysical[dirEntry].Present)
        return;
    if (tables[dirEntry] == (void*)0xFFFFFFFF)
        return;
    if (!tables[dirEntry]->pages[pagEntry].Present)
        return;
    
    // Remove it
    tables[dirEntry]->pages[pagEntry].Present = 0;
    InvalidateTLB(logical);
    if (mapPage)
        availableMappingPages++;
    
    // See if it's empty
    for (int i = 0; i < 1024; i++)
    {
        if (tables[dirEntry]->pages[i].Present)
            return;
    }
    
    // It's empty, so scrub it
    tablesPhysical[dirEntry].Present = 0;
    Unmap(tables[dirEntry], true);
    tables[dirEntry] = NULL;
    CPhysicalMemory::ReleasePages((PhysicalPointer)(tablesPhysical[dirEntry].Frame << 12));
    if (mapPage)
        availableMappingPages += -1024 + 1; // -1024 for losing the page, +1 for the entry it was using
}

PhysicalPointer SPageDirectoryInfo::Address(void *logical)
{
    // Check the directory table
    
    unsigned int directoryIndex = (((unsigned int)logical) & 0xFFC00000) >> 22;
    if (!tablesPhysical[directoryIndex].Present)
        return CPhysicalMemory::Invalid;
    
    // Check if it's a 4MB page, in which case use the frame directly
    
    if (tablesPhysical[directoryIndex].PageSize)
        return (PhysicalPointer)(((tablesPhysical[directoryIndex].Frame << 12) & 0xFFC00000) | (((unsigned int)logical) & 0x003FFFFF));
    
    // It's 4kb, so use our cached virtual pointer now
    
    SPageTable *table = tables[directoryIndex];
    unsigned int tableIndex = (((unsigned int)logical) & 0x003FF000) >> 12;
    if (!table->pages[tableIndex].Present)
        return CPhysicalMemory::Invalid;
    
    return (PhysicalPointer)((table->pages[tableIndex].Frame << 12) | (((unsigned int)logical) & 0x00000FFF));
}

void SPageDirectoryInfo::CopyKernel(void)
{
    for (int i = (((UInt32)&virt) >> 22); i < 1024; i++)
    {
        tables[i] = rootAddressSpace.tables[i];
        tablesPhysical[i] = rootAddressSpace.tablesPhysical[i];
    }
    revision = rootAddressSpace.revision;
}
