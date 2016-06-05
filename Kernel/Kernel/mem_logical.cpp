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

//#define PAGE_DEBUG(...) kprintf(__VA_ARGS__)
#define PAGE_DEBUG(...)

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
    PAGE_DEBUG("Kernel logical 0x%.8x, physical 0x%.8x\n", (int)&phys, (int)&virt);
    
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
    PageMapBootstrap.pages[1022].Frame = ((unsigned int)bootstrapPhysical & 0xFFFFF000) >> 12;
    PageMapBootstrap.pages[1022].Writable = 1;
    PageMapBootstrap.pages[1022].Present = 1;
        // Hook it up
    tablesPhysical[1023].Frame = ((unsigned int)bootstrapPhysical & 0xFFFFF000) >> 12;
    tablesPhysical[1023].Available = 1;
    tablesPhysical[1023].Writable = 1;
    tablesPhysical[1023].Present = 1;
    tables[1023] = (SPageTable*)0xFFFFE000;  // We just set this up
    InvalidateTLB(tables[1022]);
    
    if (Address((void*)0xFFFFE000) == CPhysicalMemory::Invalid) {
        PAGE_DEBUG("Failed to init");
        while(1) asm("hlt");
    }
}

void SPageDirectoryInfo::Init(void)
{
    PAGE_DEBUG("SPageDirectoryInfo(%.8x)::Init\n", this);
    // Create a new directory
    directoryPhysical = CPhysicalMemory::AllocateContiguousPages();
    tablesPhysical = NULL;  // Set this to NULL - Map will try to use it otherwise.
    tablesPhysical = (SPageDirectory*)Map(fmWritable, pmKernel, directoryPhysical);
    
    // Empty structure
    for (int i = 0; i < 1024; i++)
    {
        tables[i] = NULL;
        tablesPhysical[i].RawValue = 0x00000000;
    }
    
    // Invalidate 'kernel version' field so first switch to this address space updates to the right kernel data
    revision = 0;
    CopyKernel();
}

void SPageDirectoryInfo::Release(void)
{
    PAGE_DEBUG("SPageDirectoryInfo(%.8x)::Release\n", this);
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
//    PAGE_DEBUG("SPageDirectoryInfo(%.8x)::Select\n", this);
    CopyKernel();
    asm volatile("mov %0, %%cr3"::"r" (directoryPhysical));
    currentAddressSpace = this; // WARN: Will this be picked up by thread switching?
}

void* SPageDirectoryInfo::Map(int permissions, MAP_TYPE location, PhysicalPointer physical, size_t count)
{
    PAGE_DEBUG("SPageDirectoryInfo(%.8x)::Map(%.4x, %.2x, 0x%.8x, %i)\n", this, permissions, location, (int)physical, (int)count);

    UInt32 min, max;
    bool searchReverse;

    void *newLocation, *currentLocation;
    size_t foundLength;
    
    if ((location == pmKernel) && (this != &rootAddressSpace))
    {
        PAGE_DEBUG("Passing request to kernel address space\n");
        newLocation = rootAddressSpace.Map(permissions, location, physical, count);
        PAGE_DEBUG("Returning 0x%.8x (via kernel)\n", newLocation);
        return newLocation;
    }
    
    // Validate input
    searchReverse = false;
    switch (location)
    {
        case pmApplication:
            if (permissions & fmPageData) {
                PAGE_DEBUG("Return NULL - application asking for page table (kernel only)\n");
                return NULL;
            }
            min = 0x00001000;
            max = ((UInt32)&virt) - 0x1000;
            break;
        case pmKernel:
            if (permissions & fmPageData)
            {
                if (count != 1) {
                    PAGE_DEBUG("Returning NULL - asked for something other than a single page table\n");
                    return NULL;
                }
                searchReverse = true;
                // TODO: Search only present page tables
            }
            min = (UInt32)&virt;
            max = 0xFFFFD000;
            break;
        default:
            PAGE_DEBUG("Returning NULL - unknown location type\n");
            return NULL;
    }
    
    // Do the search
    // TODO: Search properly, using page directory and tables
    PAGE_DEBUG("Searching %s between 0x%.8x and 0x%.8x (kernel base 0x%.8x)\n",
               searchReverse ? "down" : "up",
               (void*)min, (void*)max, (void*)&virt);
    newLocation = NULL;
    currentLocation = (void*)(searchReverse ? max : min);
    foundLength = 0;
    while (foundLength < count)
    {
        if ((currentLocation < (void*)min) || (currentLocation > (void*)max)) {
            PAGE_DEBUG("Returning NULL - couldn't find empty page within range\n");
            return NULL;
        }
        
        if (Address(currentLocation) == CPhysicalMemory::Invalid)
        {
            PAGE_DEBUG("Found free page %.8x (phys %.8x)\n", currentLocation, Address(currentLocation));
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
    if (newLocation == NULL) {
        PAGE_DEBUG("Returning NULL - couldn't find contiguous pages as requested\n");
        return NULL;
    }
    
    if ((permissions & fmPageData) && (availableMappingPages == 1))
    {
        int pageDirEntry;
        
        PAGE_DEBUG("Only one page table left - preparing to map a new set of blank page tables into it\n");
        
        // Find empty directory for it
        for (pageDirEntry = 1023; pageDirEntry >= 0; pageDirEntry--)
        {
            if (!tablesPhysical[pageDirEntry].Present)
                break;
        }
        if (pageDirEntry < 0) {
            PAGE_DEBUG("Returning NULL - failed to find empty directory for blank page table\n");
            return NULL;
        }
        
        // We need to map a new page into this one, then get a new entry from the new page for our result
        PhysicalPointer newPage = CPhysicalMemory::AllocateContiguousPages();
        Map(fmWritable, newLocation, newPage);  // Should succeed, as we're using a 'mapping' entry that shouldn't need a new page
        SPageTable *newTable = (SPageTable*)newLocation;
        
        // Scrub table
        for (int i = 0; i < 1024; i++)
            newTable->pages[i].RawValue = 0x00000000;
        
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
    
    // We found a big enough gap, so fill it in
    for (foundLength = 0, currentLocation = newLocation;
         foundLength < count;
         foundLength++, currentLocation = ((unsigned char*)currentLocation) + 0x1000) {
        Map(permissions, currentLocation, physical);
        if (!(permissions & fmNotPresent))
            physical = ((unsigned char*)physical) + 0x1000;
    }
    
    // Done
    PAGE_DEBUG("Returning 0x%.8x\n", newLocation);
    return newLocation;
}
int tauntcount = 1;
void SPageDirectoryInfo::Map(int permissions, void *location, PhysicalPointer physical)
{
    PAGE_DEBUG("SPageDirectoryInfo(%.8x)::Map(%.4x, 0x%.8x, 0x%.8x)\n", this, permissions, (int)location, (int)physical);
    // Get the indices out of the pointer
    unsigned int dirEntry = (((unsigned int)location) >> 22) & 0x3FF;
    unsigned int pagEntry = (((unsigned int)location) >> 12) & 0x3FF;
    unsigned int pagOffset = ((unsigned int)location) & 0xFFF;
    
    // Get the table directory and check the page is present
    if (!tablesPhysical[dirEntry].Present)
    {
        PAGE_DEBUG("SPageDirectoryInfo: Need a new directory entry for %i, creating...\n", dirEntry);
        PhysicalPointer newPage = CPhysicalMemory::AllocateContiguousPages();
        tables[dirEntry] = (SPageTable*)Map(fmPageData | fmWritable, pmKernel, newPage);
        if (tables[dirEntry] == location)
            PAGE_DEBUG("Something terrible has happened\n");
        
        // Scrub the new table
        for (int i = 0; i < 1024; i++)
            tables[dirEntry]->pages[i].RawValue = 0x00000000;
        
        // Set up the new entry
        SPageDirectory newDirectoryEntry;
        newDirectoryEntry.RawValue = 0x00000000;
        newDirectoryEntry.Frame = ((unsigned int)(((UInt32)newPage) & 0xFFFFF000)) >> 12;
        newDirectoryEntry.Present = 1;
        newDirectoryEntry.Writable = 1;
        newDirectoryEntry.User = 1; // In case this directory contains user pages
        newDirectoryEntry.WriteThrough = 0;
        newDirectoryEntry.CacheDisable = 0;
        newDirectoryEntry.Accessed = 0;
        newDirectoryEntry.Available = 1;
        newDirectoryEntry.PageSize = 0; // 4Kb
        newDirectoryEntry.Global = 0;   // Not global
        tablesPhysical[dirEntry] = newDirectoryEntry;
    }
    
    SPage newPageInfo;
    if (permissions & fmNotPresent) {
        newPageInfo.RawValue = (UInt32)physical;
        newPageInfo.Present = 0;
    } else {
        newPageInfo.RawValue = 0x00000000;
        newPageInfo.Present = 1;
        if (permissions & fmWritable)
            newPageInfo.Writable = 1;
        if (permissions & fmUser)
            newPageInfo.User = 1;
        newPageInfo.Frame = ((unsigned int)(((UInt32)physical) & 0xFFFFF000)) >> 12;
    }

    // Activate the mapping
    PAGE_DEBUG("Loading mapping (dir %i page %i)...", dirEntry, pagEntry);
    if ((dirEntry == 1023) && (pagEntry == 1023)) {
        PAGE_DEBUG("Probably making a mistake. old info: %.8x\n", tables[dirEntry]->pages[pagEntry].RawValue);
    }
    tables[dirEntry]->pages[pagEntry] = newPageInfo;
    availableMappingPages--;
    // Increment 'kernel address space' counter so other processes know to update themselves
    if (this == &rootAddressSpace)
    {
        revision++;
        if (revision == 0)
            revision = 1;
    }
    if (this == currentAddressSpace) {
        InvalidateTLB(location);
    } else if ((this == &rootAddressSpace) && (currentAddressSpace->revision != 0)) {
        // These two may already be set, but just in case, reset
        currentAddressSpace->tables[dirEntry] = tables[dirEntry];
        currentAddressSpace->tablesPhysical[dirEntry] = tablesPhysical[dirEntry];
        
        // Set the actual mapping
        currentAddressSpace->tables[dirEntry]->pages[pagEntry] = newPageInfo;
        currentAddressSpace->revision = revision;   // Update now, as we just refreshed it efficiently
        InvalidateTLB(location);
    }
    PAGE_DEBUG("Mapped 0x%.8x\n", location);
//    if (!tauntcount--)
//        tables[1023]->pages[1023].Writable = 0;
//    InvalidateTLB(tables[1023]);
}

void SPageDirectoryInfo::Unmap(void *logical, size_t count)
{
    while (count != 0) {
        Unmap(logical, false);
        logical = ((char*)logical) + 0x1000;
        count--;
    }
}

void SPageDirectoryInfo::Unmap(void *logical, bool mapPage)
{
    PAGE_DEBUG("Unmap 0x%.8x\n", logical);
    if ((UInt32(logical) & 0xFFFFF000) == 0xFFFFF000) {
        PAGE_DEBUG("MASSIVE MALFUNCTION");
        while(1) asm("hlt");
    }
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
    if (tables[dirEntry]->pages[pagEntry].RawValue == 0x00000000)
        return;
    
    // Remove it
    tables[dirEntry]->pages[pagEntry].RawValue = 0x00000000;
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

UInt32 SPageDirectoryInfo::AddressRaw(void *logical)
{
    // Check the directory table
    
    unsigned int directoryIndex = (((unsigned int)logical) & 0xFFC00000) >> 22;
    if (!tablesPhysical[directoryIndex].Present)
        return 0;
    
    // Check if it's a 4MB page, in which case use the frame directly
    
    if (tablesPhysical[directoryIndex].PageSize)
        return 0;   // Doesn't make sense to return this one
    
    // It's 4kb, so use our cached virtual pointer now
    
    SPageTable *table = tables[directoryIndex];
    unsigned int tableIndex = (((unsigned int)logical) & 0x003FF000) >> 12;
    return table->pages[tableIndex].RawValue;
    
}

PhysicalPointer SPageDirectoryInfo::Address(void *logical)
{
    // Check the directory table
    
//    PAGE_DEBUG("SPageDirectoryInfo::Address(0x%.8x)\n", logical);
    unsigned int directoryIndex = (((unsigned int)logical) & 0xFFC00000) >> 22;
//    PAGE_DEBUG("SPageDirectoryInfo directory index %i: %.8x\n", directoryIndex, tablesPhysical[directoryIndex].RawValue);
    if (!tablesPhysical[directoryIndex].Present)
        return CPhysicalMemory::Invalid;
    
    // Check if it's a 4MB page, in which case use the frame directly
    
    if (tablesPhysical[directoryIndex].PageSize)
        return (PhysicalPointer)(((tablesPhysical[directoryIndex].Frame << 12) & 0xFFC00000) | (((unsigned int)logical) & 0x003FFFFF));
    
    // It's 4kb, so use our cached virtual pointer now
    
    SPageTable *table = tables[directoryIndex];
    unsigned int tableIndex = (((unsigned int)logical) & 0x003FF000) >> 12;
//    PAGE_DEBUG("SPageDirectoryInfo page index %i: %.8x\n", tableIndex, table->pages[tableIndex].RawValue);
    if (!table->pages[tableIndex].Present) {
        // Check for "used" but not present pages
        if (table->pages[tableIndex].RawValue != 0)
            return CPhysicalMemory::Minimum;
        return CPhysicalMemory::Invalid;
    }
    
    return (PhysicalPointer)((table->pages[tableIndex].Frame << 12) | (((unsigned int)logical) & 0x00000FFF));
}

void SPageDirectoryInfo::CopyKernel(void)
{
    if ((&rootAddressSpace == this) || (revision == rootAddressSpace.revision) || (tablesPhysical == NULL))
        return;
//    PAGE_DEBUG("SPageDirectoryInfo(%.8x)::CopyKernel\n", this);
    for (int i = (((UInt32)&virt) >> 22); i < 1024; i++)
    {
        tables[i] = rootAddressSpace.tables[i];
        tablesPhysical[i] = rootAddressSpace.tablesPhysical[i];
    }
    revision = rootAddressSpace.revision;
}
