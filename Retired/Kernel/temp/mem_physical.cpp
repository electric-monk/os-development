#include "mem_physical.h"
#include "mem_logical.h"

const PhysicalPointer CPhysicalMemory::Minimum = (PhysicalPointer)0x00000000;
const PhysicalPointer CPhysicalMemory::Maximum = (PhysicalPointer)0xFFFFFFFF;
const PhysicalPointer CPhysicalMemory::Invalid = (PhysicalPointer)0xFFFFFFFF;

typedef struct
{
    // TODO: Stick address and count in the one field to save space?
	void *pageBase;				// 4 bytes (physical address)
	unsigned int pageCount;		// 4 bytes (number of pages)
}
PAGE_ENTRY;

typedef struct PAGE_STACK
{
	PAGE_STACK  *lastPage;		// 4 bytes (virtual address)
	PAGE_STACK	*nextPage;		// 4 bytes (virtual address)
	unsigned int entryCount;	// 4 bytes (number of entries)
	unsigned int reserved;		// 4 bytes (nothing)
	PAGE_ENTRY entries[510];	// 4080 bytes
}
PAGE_STACK;

static PAGE_STACK basePage;
static PAGE_STACK *latestPage;
static unsigned int usedPages;

void CPhysicalMemory::Init(void)
{
	basePage.lastPage = NULL;
	basePage.nextPage = NULL;
	basePage.entryCount = 0;
	latestPage = &basePage;
	usedPages = 0;
}

static void GetNewPage(void)
{
	PhysicalPointer newPage = CPhysicalMemory::AllocateContiguousPages();
    PAGE_STACK *newObject = (PAGE_STACK*)currentAddressSpace->Map(fmWritable, pmKernel, newPage);
	newObject->lastPage = latestPage;
	newObject->nextPage = NULL;
	newObject->entryCount = 0;
	latestPage->nextPage = newObject;
	latestPage = newObject;
}

static void TidyPage(PAGE_STACK *page)
{
	if (page->entryCount != 0)
    {
        // Attempt to flatten entries
        int x, y;
        
        for (x = y = 0; x < page->entryCount; x++)
        {
            if (page->entries[x].pageCount != 0)
                page->entries[y++] = page->entries[x];
        }
        page->entryCount = y;
        
        // If there's still entries, we're done
        if (y != 0)
            return;
    }
    
    // If it's the statically allocated base page, don't try to release it
	if (page == &basePage)
		return;
    
    // Delink this page
	if (page->nextPage != NULL)
		page->nextPage->lastPage = page->lastPage;
	if (page->lastPage != NULL)
		page->lastPage->nextPage = page->nextPage;
	if (latestPage == page)
		latestPage = page->lastPage;

    // Release the page from real memory
	PhysicalPointer oldPage = currentAddressSpace->Address(page);
    currentAddressSpace->Unmap(page);
	CPhysicalMemory::ReleasePages(oldPage);
}

static void DropPages(void)
{
    PAGE_STACK *current, *next;
    
    // Just tidy every page
    for (current = &basePage; current != NULL; current = next)
    {
        next = current->nextPage;
        TidyPage(current);
    }
}

void CPhysicalMemory::AddChunk(PhysicalPointer address, PhysicalLength length)
{
	PAGE_STACK *current;
	int index, pageCount;
	unsigned char *newStart, *newEnd;
	unsigned char *curStart, *curEnd;

	newStart = (unsigned char*)address;
	newEnd = newStart + length;
	pageCount = length >> 12;
	for (current = &basePage; current != NULL; current = current->nextPage)
	{
		for (index = current->entryCount; index != 0;)
		{
			index--;
			curStart = (unsigned char*)current->entries[index].pageBase;
			curEnd = curStart + (current->entries[index].pageCount << 12);
			if (newEnd == curStart)
			{
				// New data comes directly before this entry
				current->entries[index].pageBase = (void*)address;
				current->entries[index].pageCount += pageCount;
				return;
			}
			if (newStart == curEnd)
			{
				// New data comes directly after this entry
				current->entries[index].pageCount += pageCount;
				return;
			}
		}
	}
	// Nothing found! Add new entry
	current = &basePage;
	while (true)
	{
		for (; current != NULL; current = current->nextPage)
		{
			if (current->entryCount < 510)
			{
				int newEntry = current->entryCount++;
				current->entries[newEntry].pageBase = (void*)address;
				current->entries[newEntry].pageCount = pageCount;
				return;
			}
		}
		GetNewPage();
		current = latestPage;
	}
}

void CPhysicalMemory::AddReserved(PhysicalPointer address, PhysicalLength length)
{
    // Not very efficient just now
    int pageCount = (length + 0x00000FFF) & 0xFFFFF000;
    char* reserveLower = (char*)address;
    char* reserveUpper = reserveLower + (pageCount << 12);
    for (PAGE_STACK *current = &basePage; current != NULL; current = current->nextPage)
    {
        for (int i = 0; i < current->entryCount; i++)
        {
            PAGE_ENTRY *entry = current->entries + i;
            char* entryLower = (char*)entry->pageBase;
            char* entryUpper = entryLower + (entry->pageCount << 12);
            // TODO: Confirm this code is right, and optimise
            if (reserveLower <= entryLower)
            {
                if (reserveUpper < entryLower)
                {
                    // Reserved memory not in this range
                }
                else if (reserveUpper > entryUpper)
                {
                    // Reserved memory completely covers this range
                    entry->pageCount = 0;
                }
                else
                {
                    // Reserved memory covers the start of this range
                    entry->pageBase = reserveUpper;
                    entry->pageCount -= (reserveUpper - entryLower) >> 12;
                }
            }
            else
            {
                if (reserveLower > entryUpper)
                {
                    // Reserved memory not in this range
                }
                else if (reserveUpper < entryUpper)
                {
                    // Reserved memory is entirely within this range
                    entry->pageCount = (entryUpper - reserveLower) >> 12;
                    AddChunk(reserveUpper, entryUpper - reserveUpper);
                }
                else
                {
                    // Reserved memory covers the end of this range
                    entry->pageCount = (entryUpper - reserveLower) >> 12;
                }

            }
        }
    }
    DropPages();
}

PageCount CPhysicalMemory::TotalPages(void)
{
	PageCount count;

	count = 0;
	for (PAGE_STACK *current = &basePage; current != NULL; current = current->nextPage)
	{
		for (int i = 0; i < current->entryCount; i++)
		{
			count += current->entries[i].pageCount;
		}
	}
	return count;
}

PageCount CPhysicalMemory::TotalUsed(void)
{
	return usedPages;
}

PhysicalLength CPhysicalMemory::PageToMemory(PageCount count)
{
	return count << 12;
}

PhysicalPointer CPhysicalMemory::AllocateContiguousPages(PageCount count, PhysicalPointer min, PhysicalPointer max)
{
	PAGE_STACK *current;
	int index;

	for (current = latestPage; current != NULL; current = current->lastPage)
	{
		for (index = current->entryCount; index != 0;)
		{
			index--;
			if ((current->entries[index].pageCount >= count)
            && ((current->entries[index].pageBase >= min) && ((((char*)current->entries[index].pageBase) + current->entries[index].pageCount) < max)))
			{
				PhysicalPointer result;

				result = current->entries[index].pageBase;
				current->entries[index].pageBase = ((char*)current->entries[index].pageBase) + (count << 12);
				current->entries[index].pageCount -= count;
				if (current->entries[index].pageCount == 0)
					TidyPage(current);
				usedPages += count;
				return result;
			}
		}
	}
	return Invalid;
}

void CPhysicalMemory::ReleasePages(PhysicalPointer address, PageCount count)
{
	// Cheating just now
	AddChunk(address, count << 12);
	usedPages -= count;
}
