#ifndef __MEM_LOGICAL_H__
#define __MEM_LOGICAL_H__

#include "mem_physical.h"
#include "runtime.h"

typedef enum {
    pmApplication,
    pmKernel,
} MAP_TYPE;

typedef enum {
    fmWritable = 1, // Else read only
    fmUser = 2,     // Else supervisor
    fmPageData = 4, // Memory for the page tables
} MAP_FLAGS;

typedef struct {
    union {
        struct {
            UInt32 Present : 1;     // Present if set
            UInt32 Writable : 1;    // Read-only if clear
            UInt32 User : 1;        // Supervisor if clear
            UInt32 WriteThrough : 1;
            UInt32 CacheDisable : 1;
            UInt32 Accessed : 1;    // Page has been read by CPU
            UInt32 Available : 1;
            UInt32 PageSize : 1;
            UInt32 Global : 1;
            UInt32 UserData : 3;
            UInt32 Frame : 20;      // Address of page, shifted right 12 bits
        };
        UInt32 RawValue;
    };
} SPageDirectory;

typedef struct {
    union {
        struct {
            UInt32 Present : 1;     // Present if set
            UInt32 Writable : 1;    // Read-only if clear
            UInt32 User : 1;        // Supervisor if clear
            UInt32 WriteThough : 1;
            UInt32 CacheDisable : 1;
            UInt32 Accessed : 1;    // Page has been read by CPU
            UInt32 Dirty : 1;       // Page has been written by CPU
            UInt32 PatIndex : 1;
            UInt32 Global : 1;
            UInt32 UserData : 3;
            UInt32 Frame : 20;      // Address of page, shifted right 12 bits
        };
        UInt32 RawValue;
    };
} SPage;

typedef struct {
    SPage pages[1024];
} SPageTable;

class SPageDirectoryInfo {
public:
    // WARNING - ensure to map all pages in as kernel pages, so they can always be accessed from any process
    
    SPageTable *tables[1024];           // Virtual addresses of pages
    SPageDirectory *tablesPhysical;    // Physical addresses of pages, but must be page aligned
    PhysicalPointer directoryPhysical;  // Physical address of tablesPhysical

    void InitKernel(void);  // Initialises this page table to represent the singleton kernel page directory
    
    void Init(void);        // Creates a new process's page directory
    void Release(void);
    void Select(void);
    
    void* Map(/*MAP_FLAGS*/int permissions, MAP_TYPE location, PhysicalPointer physical, size_t count = 1);
    void Map(/*MAP_FLAGS*/int permissions, void *location, PhysicalPointer physical);
    void Unmap(void *logical);
    PhysicalPointer Address(void *logical);
    
private:
    void Unmap(void *logical, bool mapPage);
    void CopyKernel(void);
    
    int revision;
};

extern SPageDirectoryInfo rootAddressSpace; // Process '0', the OS
extern SPageDirectoryInfo *currentAddressSpace; // Set on a switch

/*
class CPagedMemory
{
public:
	static void* MapPage(MAP_TYPE type, PhysicalPointer physical, size_t count = 1);
	static void MapPage(MAP_TYPE type, void *logical, PhysicalPointer physical, size_t count = 1);
	static PhysicalPointer PageAddress(void *logical);
	static void UnmapPage(void *logical);
    
    static void CreateProcessPageTable(void *logical);
};
 */

#endif // __MEM_LOGICAL_H__
