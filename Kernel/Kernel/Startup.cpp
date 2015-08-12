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
#include "debug.h"

class RunloopWatcher : public IpcServiceWatcher
{
public:
    typedef enum {
        Appear,
        Change,
        Disappear,
    } State;

private:
    typedef KernelFunction<int(State, KernelObject*)> RunloopWatcherProviderFunction;
    typedef KernelFunction<int(State, KernelObject*, IpcService*)> RunloopWatcherServiceFunction;
    
    int _count;
    KernelDictionary *_providerWatchers, *_serviceWatchers;
    
    int Store(KernelDictionary *dictionary, KernelObject *object)
    {
        // TODO: Lock?
        KernelNumber *token = new KernelNumber(_count++);
        while (dictionary->ObjectFor(token))
            token->Reset(_count++);
        dictionary->Set(token, object);
        int result = token->Value();
        token->Release();
        return result;
    }
    static void Reset(KernelDictionary *dictionary, int token)
    {
        KernelNumber *tokenNumber = new KernelNumber(token);
        dictionary->Set(tokenNumber, NULL);
        tokenNumber->Release();
    }
    
    TaskQueue *_queue;

    void SendMessage(KernelDictionary *list, bicycle::function<int(KernelObject *callback)> handler, bicycle::function<int(void)> finish)
    {
        if (_queue) {
            _queue->AddTask([handler, finish, list](){
                list->AllObjects()->Enumerate([handler](KernelObject *func)->void*{
                    handler(func);
                    return NULL;
                });
                finish();
                return 0;
            });
        }
    }
    void SendProviderMessage(KernelObject *provider, State event)
    {
        provider->AddRef();
        SendMessage(_providerWatchers, [provider, event](KernelObject *func){
            RunloopWatcherProviderFunction *function = (RunloopWatcherProviderFunction*)func;
            function->Pointer()(event, provider);
            return 0;
        }, [provider]{
            provider->Release();
            return 0;
        });
    }
    void SendServiceMessage(KernelObject *provider, IpcService *service, State event)
    {
        provider->AddRef();
        service->AddRef();
        SendMessage(_serviceWatchers, [provider, service, event](KernelObject *func){
            RunloopWatcherServiceFunction *function = (RunloopWatcherServiceFunction*)func;
            function->Pointer()(event, provider, service);
            return 0;
        }, [provider, service]{
            provider->Release();
            service->Release();
            return 0;
        });
    }
    
public:
    
    RunloopWatcher()
    {
        _providerWatchers = new KernelDictionary();
        _serviceWatchers = new KernelDictionary();
    }
    
    void SetQueue(TaskQueue *queue)
    {
        // TODO: retain?
        _queue = queue;
    }
    
    int AddTaskForProvider(bicycle::function<int(State appear, KernelObject *provider)> task)
    {
        RunloopWatcherProviderFunction *function = new RunloopWatcherProviderFunction(task);
        int token = Store(_providerWatchers, function);
        function->Release();
        return token;
    }
    
    int AddTaskForService(bicycle::function<int(State appear, KernelObject *provider, IpcService *service)> task)
    {
        RunloopWatcherServiceFunction *function = new RunloopWatcherServiceFunction(task);
        int token = Store(_serviceWatchers, function);
        function->Release();
        return token;
    }
    
    void RemoveTaskForProvider(int token)
    {
        Reset(_providerWatchers, token);
    }
    
    void RemoveTaskForService(int token)
    {
        Reset(_serviceWatchers, token);
    }
    
protected:
    ~RunloopWatcher()
    {
        _serviceWatchers->Release();
        _providerWatchers->Release();
    }
    
public: // For IpcServiceWatcher
    void ServiceProviderAppeared(KernelObject *provider)
    {
        SendProviderMessage(provider, Appear);
    }
    
    void ServiceProviderRemoved(KernelObject *provider)
    {
        SendProviderMessage(provider, Disappear);
    }

    void ServiceAppeared(KernelObject *provider, IpcService *service)
    {
        SendServiceMessage(provider, service, Appear);
    }
    
    void ServiceChanged(KernelObject *provider, IpcService *service)
    {
        SendServiceMessage(provider, service, Change);
    }
    
    void ServiceRemoved(KernelObject *provider, IpcService *service)
    {
        SendServiceMessage(provider, service, Disappear);
    }
};

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
private:
    RunloopThread *_runloop;
    RunloopWatcher *_watcher;
    InterfaceHelper *_helper;
    
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
        _helper->PerformTask(nubbinControl, [](Interface_Request *request){
            kprintf("Requesting nubbin\n");
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
            kprintf("Nubbin response: %i\n", exposeResponse->status);
            // TODO: thing?
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
    }
    
public:
    StartupHandler()
    {
        kprintf("STARTUP: Initialising\n");

        _runloop = new RunloopThread();
        _watcher = new RunloopWatcher();
        _watcher->SetQueue(_runloop->Queue());
        _helper = new InterfaceHelper();
        
        // Add a watcher for disk drives appearing
        _watcher->AddTaskForService([this](RunloopWatcher::State state, KernelObject *provider, IpcService *service){
            if (state == RunloopWatcher::Appear) {
                if (service->ServiceType()->IsEqualTo(SERVICE_TYPE_BLOCK)) {
                    if (service->Name()->IsEqualTo("0"_ko)) {
                        kprintf("STARTUP: Found file %x\n", service);
                        FoundPotentialFile(service);
                    } else {
                        kprintf("STARTUP: Found block service %x\n", service);
                        // New disk drive appeared
                        FoundPotentialDiskDrive(service);
                    }
                } else if (service->ServiceType()->IsEqualTo("filesystem"_ko)) {
                    kprintf("STARTUP: Found filesystem %x\n", service);
                    // New filesystem appeared
                    FoundPotentialFileSystem(service);
                } else if (service->ServiceType()->IsEqualTo("binaryImage"_ko)) {
                    kprintf("STARTUP: Found binary %x\n", service);
                    // New binary image!
                    FoundImage(service);
                } else if (service->ServiceType()->IsEqualTo("nubbin"_ko)) {
                    kprintf("STARTUP: Found nubbin %x\n", service);
                    // New nubbin!
                    FoundPotentialNubbin(service);
                }
            }
            return 0;
        });
        
        IpcServiceList::Register(_watcher);
    }
    
protected:
    ~StartupHandler()
    {
        IpcServiceList::Unregister(_watcher);
        _helper->Release();
        _runloop->Release();
        _watcher->Release();
    }
};

void InitStartup(void)
{
    new StartupHandler();
}
