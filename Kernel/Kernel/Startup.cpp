/*
 * Startup functionality!
 *
 * Concept behind this file:
 * 1) Create a startup task queue
 * 2) As there is no Conductor.task yet, do this:
 *  2.1) Wait for the startup disk to appear
 *  2.2) Find System directory on startup disk
 *  2.3) Load Conductor.task and launch it
 * 3) Destroy startup task queue
 */

#include "KernelObject.h"
#include "Runloop.h"
#include "IPC.h"
#include "Interface_Block.h"
#include "Interface_File_Nubbin.h"
#include "Interface_File.h"
#include "Provider.h"
#include "FileNubbin.h"
#include "fs_iso9660.h"
#include "ImageLoader.h"
#include "Process.h"
#include "IPC_Manager.h"
#include "debug.h"

#define DEBUG_STARTUP

#ifdef DEBUG_STARTUP
#define START_LOG(...) kprintf(__VA_ARGS__)
#else
#define START_LOG(...)
#endif

static void ConvertPath(FlatArray *output, ...)
{
	va_list vl;
    
    output->Initialise();
	va_start(vl, output);
    while (true) {
        KernelString *item = va_arg(vl, KernelString*);
        if (item == NULL)
            break;
        FlatString *str = (FlatString*)output->GetNextAddress();
        str->Initialise(item->CString());
        output->CompleteNextItem();
    }
    va_end(vl);
}

class StartupHandler : public KernelObject
{
public:
    CLASSNAME(KernelObject, StartupHandler);
    
private:
    // To make it work
    RunloopThread *_runloop;
    IpcServiceMonitor *_monitor;
    InterfaceHelper *_helper;
    // For launching the startup process
    bool _starting;
    
    void FoundPotentialDiskDrive(IpcService *service)
    {
        // Attempt to mount the disk drive
        FileSystem_ISO9660 *fs = new FileSystem_ISO9660();
        fs->ConnectInput("cdrom"_ko, service);
    }
    
    void FoundPotentialFileSystem(IpcService *service)
    {
        // Start up a nubbin to allow us to read the file
        FileNubbin *nubbin = new FileNubbin();
        nubbin->ConnectInput("filesystem"_ko, service);
    }
    
    void FoundPotentialNubbin(IpcService *service)
    {
        // Tell the nubbin what file to open
        IpcEndpoint *nubbinControl = service->RequestConnection();
        _runloop->AddSource(nubbinControl, [=](BlockableObject *object, KernelArray *signals){
            _helper->HandleMessage(nubbinControl->Read(false), [this](Interface_Response *response){
                kprintf("STARTUP: nubbin unhandled message\n");
                return 0;
            });
            return 0;
        });
        _helper->PerformTask(nubbinControl, [](Interface_Request *request){
            Interface_File_Nubbin::Expose *exposeRequest = (Interface_File_Nubbin::Expose*)request;
            exposeRequest->type = Interface_File_Nubbin::Expose::ExposeFile;
            exposeRequest->exclusive = true;
            exposeRequest->autoUnexpose = true;
            exposeRequest->rootNode = NodeRequest::RootNode;
            exposeRequest->subpath.Initialise();
            FlatString *dir = (FlatString*)exposeRequest->subpath.GetNextAddress();
            dir->Initialise("boot");
            exposeRequest->subpath.CompleteNextItem();
            FlatString *file = (FlatString*)exposeRequest->subpath.GetNextAddress();
//            file->Initialise("Conductor.task");
            file->Initialise("Conducto.tas");
            exposeRequest->subpath.CompleteNextItem();
            return 0;
        }, [](Interface_Response *response){
            Interface_File_Nubbin::ExposeResponse *exposeResponse = (Interface_File_Nubbin::ExposeResponse*)response;
            if (exposeResponse->status != Interface_Response::Success) {
                kprintf("STARTUP: Couldn't find task! Error %i\n", exposeResponse->status);
                // TODO: Kernel doomed
            }
            return 0;
        });
    }
    
    void FoundPotentialFile(IpcService *service)    // TODO: We know the name of the file as the return to the nubbin, don't just guess
    {
        // Attempt to open the initial process
        ImageLoader *loader = new ImageLoader();
        loader->ConnectInput("file"_ko, service);
    }
    
    void FoundImage(IpcService *service)
    {
        Process *process = new Process("Conductor");
        process->AttachImage(service);
        _starting = false;
    }
    
    class ProviderContainer : public KernelObject
    {
    public:
        CLASSNAME(KernelObject, StartupHandler::ProviderContainer);
        
        ProviderContainer(KernelObject *provider)
        {
            
        }
    };
    
public:
    StartupHandler()
    {
        START_LOG("STARTUP: Initialising\n");

        _runloop = new RunloopThread();
        _monitor = new IpcServiceMonitor();
        _helper = new InterfaceHelper();
        _starting = true;
        
        _runloop->AddSource(_monitor, [this](BlockableObject *object, KernelObject *other){
            KernelArray *changes = _monitor->Changes();
            if (!_starting)
                return 0;
            changes->Enumerate([this](KernelObject *object){
                KernelDictionary *info = (KernelDictionary*)object;
                KernelString *state = (KernelString*)info->ObjectFor("State"_ko);
                KernelString *type = (KernelString*)info->ObjectFor("Type"_ko);
                if (state && type && state->IsEqualTo("Start"_ko) && type->IsEqualTo("Output"_ko)) {
                    KernelNumber *handle = (KernelNumber*)info->ObjectFor("Connector"_ko);
                    IpcService *service = handle ? (IpcService*)Process::Mapper()->Find(handle->Value()) : NULL;
                    if (service) {
                        if (service->ServiceType()->IsEqualTo(SERVICE_TYPE_BLOCK)) {
                            if (service->Name()->IsEqualTo("0"_ko)) {
                                START_LOG("STARTUP: Found file %x\n", service);
                                FoundPotentialFile(service);
                            } else {
                                START_LOG("STARTUP: Found block service %x\n", service);
                                // New disk drive appeared
                                FoundPotentialDiskDrive(service);
                            }
                        } else if (service->ServiceType()->IsEqualTo("filesystem"_ko)) {
                            START_LOG("STARTUP: Found filesystem %x\n", service);
                            // New filesystem appeared
                            FoundPotentialFileSystem(service);
                        } else if (service->ServiceType()->IsEqualTo("binaryImage"_ko)) {
                            START_LOG("STARTUP: Found binary %x\n", service);
                            // New binary image!
                            FoundImage(service);
                        } else if (service->ServiceType()->IsEqualTo("nubbin"_ko)) {
                            START_LOG("STARTUP: Found nubbin %x\n", service);
                            // New nubbin!
                            FoundPotentialNubbin(service);
                        }
                    }
                }
                return (void*)NULL;
            });
            return 0;
        });
    }
    
protected:
    ~StartupHandler()
    {
        _helper->Release();
        _runloop->Release();
        _monitor->Release();
    }
};

void InitStartup(void)
{
    new StartupHandler();
}
