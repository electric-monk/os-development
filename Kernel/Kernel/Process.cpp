#include "Process.h"
#include "Runloop.h"
#include "IPC.h"
#include "Interface.h"
#include "Provider.h"
#include "Interface_BinaryImage.h"
#include "CPU_intr.h"
#include "Thread.h"

class BinaryMap : public VirtualMemory
{
public:
    CLASSNAME(VirtualMemory, BinaryMap);
    
    BinaryMap(Process *process, InterfaceHelper *helper, IpcEndpoint *connection, UInt32 chunk, UInt32 length)
    :VirtualMemory(process, length)
    {
        _ownerHelper = helper;
        _ownerConnection = connection;
        _chunk = chunk;
    }
    
    BinaryMap(Process *process, InterfaceHelper *helper, IpcEndpoint *connection, UInt32 chunk, void *linearBase, UInt32 length)
    :VirtualMemory(process, linearBase, length)
    {
        _ownerHelper = helper;
        _ownerConnection = connection;
        _chunk = chunk;
    }
    
protected:
    void HandlePageFault(void *linearAddress)
    {
        _ownerHelper->PerformTask(_ownerConnection, [this, linearAddress](Interface_Request *request){
            Interface_BinaryImage::Read *read = (Interface_BinaryImage::Read*)request;
            read->type = Interface_BinaryImage::Read::ReadChunk;
            read->chunk = _chunk;
            read->offset = (UInt32(linearAddress) & ~(PAGE_SIZE - 1)) - UInt32(LinearBase());
            read->length = PAGE_SIZE;
            return 0;
        }, [this, linearAddress](Interface_Response *response){
            Interface_BinaryImage::ReadChunk *read = (Interface_BinaryImage::ReadChunk*)response;
            if (read->status != Interface_BinaryImage::ReadChunk::Success) {
                // TODO: Failed to read, game's a bogey for this process
                return 0;
            }
            // Get a new page
            Map(fmWritable, linearAddress, CPhysicalMemory::AllocateContiguousPages(), [read](void *address){
                // Fill it in
                read->Populate((UInt8*)((UInt32(address) & ~(PAGE_SIZE - 1))));
                return 0;
            });
            return 0;
        });
    }
    
private:
    InterfaceHelper *_ownerHelper;
    IpcEndpoint *_ownerConnection;
    UInt32 _chunk;
};

Process::Process(const char *name)
{
    pageDirectory.Init();
    
    _binaries = new KernelDictionary();
    _helper = new InterfaceHelper();
    
    // Runloop thread for this process - allows serialised access to resources like images. Since multiple threads might try and map the same pages in, accesses must be serialised.
    _runloop = new RunloopThread(this);
}

Process::~Process()
{
    _runloop->Release();
    _binaries->Release();
    _helper->Release();
    // TODO: Should make sure the thread has stopped! Or we're doomed.
    pageDirectory.Release();
    // TODO: Should this be somewhere more sane?
    if (Process::Active == this)
        rootAddressSpace.Select();
}

void Process::AttachImage(IpcService *binaryImageService)
{
    // Do it on the runloop, for synchronisation
    _runloop->AddTask([this, binaryImageService]{
        // Already mapped? Ignore it!
        if (_binaries->ObjectFor(binaryImageService) != NULL)
            return 0;
        // Connect to it
        IpcEndpoint *connection = binaryImageService->RequestConnection();
        // Connect the service
        _binaries->Set(binaryImageService, connection);
        _runloop->AddSource(connection, [this, connection](BlockableObject *watching, KernelArray *signals){
            _helper->HandleMessage(connection->Read(false), [](Interface_Response *response){
                return 0;
            });
            return 0;
        });
        // Request initial data
        _helper->PerformTask(connection, [](Interface_Request *request){
            Interface_BinaryImage::Request *requestChunks = (Interface_BinaryImage::Request*)request;
            requestChunks->type = Interface_BinaryImage::Request::GetChunks;
            return 0;
        }, [this, binaryImageService, connection](Interface_Response *response){
            Interface_BinaryImage::GotChunks *gotChunks = (Interface_BinaryImage::GotChunks*)response;
            if (gotChunks->status != Interface_BinaryImage::GotChunks::Success) {
                // Failed in some way
                return 0;
            }
            // Add entry
            KernelArray *objects = new KernelArray();
            _binaries->Set(binaryImageService, objects);
            // Map chunks into VM
            for (int i = 0; i < gotChunks->chunkCount; i++) {
                BinaryMap *map = new BinaryMap(this, _helper, connection, i, (void*)gotChunks->chunks[i].virtualAddress, (UInt32)gotChunks->chunks[i].length);
                objects->Add(map);
                map->Release();
            }
            objects->Release();
            // Launch any threads
            _helper->PerformTask(connection, [](Interface_Request *request){
                request->type= Interface_BinaryImage::Request::GetSymbols;
                return 0;
            }, [this](Interface_Response *response){
                Interface_BinaryImage::GotSymbols *symbols = (Interface_BinaryImage::GotSymbols*)response;
                FlatString *launchKey = FlatString::CreateDynamic(Symbol_Launch);
                FlatString *entryKey = FlatString::CreateDynamic(Symbol_Address_Virtual);
                for (UInt32 i = 0; i < symbols->symbols.Count(); i++) {
                    FlatDictionary *symbol = (FlatDictionary*)symbols->symbols.ItemAt(i);
                    FlatInteger *value = (FlatInteger*)symbol->ItemFor(launchKey);
                    if (value && value->Value()) {
                        // This symbol should be launched as an entrypoint
                        FlatInteger *entryPoint = (FlatInteger*)symbol->ItemFor(entryKey);
                        if (entryPoint) {
                            // Actually found entry point, generate new userspace thread!
                            new Thread(this, (void(*)(void*))entryPoint->Value(), NULL);
                        }
                    }
                }
                launchKey->ReleaseDynamic();
                entryKey->ReleaseDynamic();
                return 0;
            });
            return 0;
        });
        return 0;
    });
    // this is all wrongh
    
    // Check if we're inside a thread belonging to Process
    // If so: simply start a runloop, waiting
    // Here, we launch a thread. It'll start in the kernel, but become this image's first user thread if it has any entry points (if there's more than one, it'll spawn new user threads for subsequent ones)
    // This means we have a thread to run the initial runloop on while the image is loaded. Once it is, threads will enter a runloop on a page fault, so the thread no longer needs to be a kernel thread.
    
    // Map segments
    // Find entry points and start threads on them
}
