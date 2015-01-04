#ifndef __MEM_PHYSICAL_H__
#define __MEM_PHYSICAL_H__

#include "runtime.h"

typedef void* PhysicalPointer;
typedef unsigned long PhysicalLength;
typedef unsigned long PageCount;

class CPhysicalMemory
{
public:
	// Values to use
	static const PhysicalPointer Minimum;
	static const PhysicalPointer Maximum;
	static const PhysicalPointer Invalid;

	// Functions to note memory during boot
	static void Init(void);
	static void AddChunk(PhysicalPointer address, PhysicalLength length);
	static void AddReserved(PhysicalPointer address, PhysicalLength length);
	static PageCount TotalPages(void);
	static PageCount TotalUsed(void);
	static PhysicalLength PageToMemory(PageCount count);

	// Functions to actually allocate memory
	static PhysicalPointer AllocateContiguousPages(PageCount count = 1, PhysicalPointer min = Minimum, PhysicalPointer max = Maximum);
	static void ReleasePages(PhysicalPointer address, PageCount count = 1);
};

#endif // __MEM_PHYSICAL_H__
