#ifndef __MEM_LOGICAL_H__
#define __MEM_LOGICAL_H__

#include "mem_physical.h"
#include "runtime.h"

typedef enum {
    pmApplication,
    pmKernel,
} MAP_TYPE;

typedef struct {
    
} SPage;

typedef struct {
    SPage pages[1024];
} SPageTable;

typedef struct {
    SPageTable *tables[1024];           // Virtual addresses of pages
    PhysicalPointer *tablesPhysical;    // Physical addresses of pages, but must be page aligned
    PhysicalPointer directoryPhysical;  // Physical address of tablesPhysical
} SPageDirectory;

class CPagedMemory
{
public:
	static void* MapPage(MAP_TYPE type, PhysicalPointer physical, size_t count = 1);
	static void MapPage(MAP_TYPE type, void *logical, PhysicalPointer physical, size_t count = 1);
	static PhysicalPointer PageAddress(void *logical);
	static void UnmapPage(void *logical);
    
    static void CreateProcessPageTable(void *logical);
};

#endif // __MEM_LOGICAL_H__
