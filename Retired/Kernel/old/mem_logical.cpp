#include "mem_logical.h"

static inline void* GetPDBR(void)
{
    unsigned int cr3;
    
    asm volatile("mov %%cr3, %0": "=b"(cr3));
    return (void*)cr3;
}

static inline void InvalidateTLB(void *address)
{
    asm volatile("invlpg %0"::"m" (*(char*)address));
}

// TODO: This code is all wrong
static void MapPage(MAP_TYPE type, void *logical, PhysicalPointer physical)
{
    // Get the indices out of the pointer
    unsigned int dirEntry = (((unsigned int)logical) >> 22) & 0x3FF;
    unsigned int pagEntry = (((unsigned int)logical) >> 12) & 0x3FF;
    unsigned int pagOffset = ((unsigned int)logical) & 0xFFF;
    
    // Get the table directory and check the page is present
    unsigned int *pdbr = (unsigned int*)GetPDBR();
    unsigned int dirEntryData = pdbr[dirEntry];
    if (!(dirEntryData & 0x01))
    {
        // Allocate the page table
        dirEntryData = (unsigned int)CPhysicalMemory::AllocateContiguousPages();
        dirEntryData |= 0x01;   // Set 'present' bit
        dirEntryData |= 0x02;   // Set 'writable' bit
        if (type != pmKernel)
            dirEntryData |= 0x04;   // Set 'user' bit
        dirEntryData |= 0x40;   // Set 'available' bit
        
        // Initialise the page table
        unsigned int *pageTable = (unsigned int*)(dirEntryData & 0xFFFFF000);
        for (int i = 0; i < 1024; i++)
            pageTable[i] = 0x00000000;
        
        // Active the page
        pdbr[dirEntry] = dirEntryData;
        InvalidateTLB(pdbr);
    }
    
    // Write into the page
    unsigned int *pageTable = (unsigned int*)(dirEntryData & 0xFFFFF000);
    unsigned int pageEntryData;
    
    pageEntryData = ((unsigned int)physical)  & 0xFFFFF000;
    pageEntryData |= 0x01;  // Set 'present' bit
    pageEntryData |= 0x02;   // Set 'writable' bit
    if (type != pmKernel)
        pageEntryData |= 0x04;   // Set 'user' bit
    
    // Activate the mapping
    pageTable[pagEntry] = pageEntryData;
    InvalidateTLB(pageTable);
}

void* CPagedMemory::MapPage(MAP_TYPE type, PhysicalPointer physical, size_t count)
{
    void *result;
    
    result = /* select page somehow, based on type */NULL;
    MapPage(type, result, physical, count);
    return result;
}

void CPagedMemory::MapPage(MAP_TYPE type, void *logical, PhysicalPointer physical, size_t count)
{
    // TODO: Make more efficient?
    for (int i = 0; i < count; i++)
        MapPage(type, ((char*)logical) + (count * 4096), ((char*)physical) + (count * 4096));
}

// TODO: This code is all wrong
PhysicalPointer CPagedMemory::PageAddress(void *logical)
{
    // Get the indices out of the pointer
    unsigned int dirEntry = (((unsigned int)logical) >> 22) & 0x3FF;
    unsigned int pagEntry = (((unsigned int)logical) >> 12) & 0x3FF;
    unsigned int pagOffset = ((unsigned int)logical) & 0xFFF;

    // Get the table directory and check the page is present
    unsigned int *pdbr = (unsigned int*)GetPDBR();
    unsigned int dirEntryData = pdbr[dirEntry];
    if (!(dirEntryData & 0x01))
        return CPhysicalMemory::Invalid;
    
    // Get the table entry and check the page is present
    unsigned int *pageTable = (unsigned int*)(dirEntryData & 0xFFFFF000);
    unsigned int pageEntryData = pageTable[pagEntry];
    if (!(pageEntryData & 0x01))
        return CPhysicalMemory::Invalid;
    
    // Get the physical pointer
    return (PhysicalPointer)((pageEntryData & 0xFFFFF000) | pagOffset);
}

// TODO: This code is all wrong
void CPagedMemory::UnmapPage(void *logical)
{
    unsigned int dirEntry = (((unsigned int)logical) >> 22) & 0x3FF;
    unsigned int pagEntry = (((unsigned int)logical) >> 12) & 0x3FF;
    unsigned int pagOffset = ((unsigned int)logical) & 0xFFF;
    
    // Get the table pointer, check it's present
    unsigned int *pdbr = (unsigned int*)GetPDBR();
    unsigned int dirEntryData = pdbr[dirEntry];
    if (!(dirEntryData & 0x01))
        return;
    
    // Get the page entry, check it's present
    unsigned int *pageTable = (unsigned int*)(dirEntryData & 0xFFFFF000);
    if (!(pageTable[pagEntry] & 0x01))
        return;

    pageTable[pagEntry] &= ~0x01;   // Reset the 'present' flag
    InvalidateTLB(pageTable);
    
    // See if any pages are still present
    for (int i = 0; i < 1024; i++)
    {
        if (pageTable[i] & 0x01)
            return;
    }
    
    // If we reach here, no page entries are being used, so we can dump it?
    pdbr[dirEntry] &= ~0x01;    // Reset the 'present' flag
    InvalidateTLB(pdbr);
    CPhysicalMemory::ReleasePages(pageTable);
}

// Build up a 'blank' page table (that only refers to the kernel's pages)
void CPagedMemory::CreateProcessPageTable(void *logical)
{
}
