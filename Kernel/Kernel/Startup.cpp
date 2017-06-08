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
#include "ConvenientSink.h"
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

namespace Startup_Internal {
    class Handler
    {
    public:
        Handler(bicycle::function<void(Handler*)> updater)
        {
            _updater = updater;
        }
        
        virtual ~Handler()
        {
        }
        
        virtual void FoundInput(KernelObject *provider, IpcClient *client) = 0;
        
        virtual void FoundOutput(KernelObject *provider, IpcService *service) = 0;
        
    protected:
        bicycle::function<void(Handler*)> _updater;
    };
    
    class WaitingForImage : public Handler
    {
    public:
        WaitingForImage(bicycle::function<void(Handler*)> updater, KernelObject *loader)
        :Handler(updater)
        {
            START_LOG("STARTUP: Loading binary image\n");
            _loader = loader;
        }
        
        void FoundInput(KernelObject *provider, IpcClient *client)
        {
        }
        
        void FoundOutput(KernelObject *provider, IpcService *service)
        {
            if ((provider == _loader) && service->ServiceType()->IsEqualTo("binaryImage"_ko)) {
                Process *process = new Process("Conductor");
                process->AttachImage(service);
                _updater(NULL);
            }
        }
        
    private:
        KernelObject *_loader;
    };
    
    class WaitingForFile : public Handler
    {
    public:
        WaitingForFile(bicycle::function<void(Handler*)> updater, KernelObject *nubbin)
        :Handler(updater)
        {
            START_LOG("STARTUP: Waiting for file\n");
            _nubbin = nubbin;
            _imageLoader = new ImageLoader();
            _fileOutput = NULL;
            _imageInput = NULL;
        }
        
        void FoundInput(KernelObject *provider, IpcClient *client)
        {
            if (provider == _imageLoader) {
                _imageInput = client;
                Check();
            }
        }

        void FoundOutput(KernelObject *provider, IpcService *service)
        {
            if ((provider == _nubbin) && service->ServiceType()->IsEqualTo(SERVICE_TYPE_BLOCK)) {
                _fileOutput = service;
                Check();
            }
        }
        
    private:
        KernelObject *_nubbin;
        ImageLoader *_imageLoader;
        IpcService *_fileOutput;
        IpcClient *_imageInput;
        
        void Check(void)
        {
            if (!_imageInput || !_fileOutput)
                return;
            _imageInput->Connect(_fileOutput);
            _updater(new WaitingForImage(_updater, _imageLoader));
        }
    };
    
    class WaitingForNubbin : public Handler
    {
    public:
        WaitingForNubbin(bicycle::function<void(Handler*)> updater, KernelObject *nubbin)
        :Handler(updater)
        {
            START_LOG("STARTUP: Creating nubbin\n");
            _nubbin = nubbin;
        }
        
        void FoundInput(KernelObject *provider, IpcClient *client)
        {
        }
        
        void FoundOutput(KernelObject *provider, IpcService *service)
        {
            if ((provider == _nubbin) && service->ServiceType()->IsEqualTo("nubbin"_ko)) {
                ConvenientSink *sink = new ConvenientSink();
                sink->CreateInput("input"_ko, [=](IpcEndpoint *connection){
                    sink->PerformTask(connection, [](Interface_Request *request){
                        Interface_File_Nubbin::Expose *exposeRequest = (Interface_File_Nubbin::Expose*)request;
                        exposeRequest->type = Interface_File_Nubbin::Expose::ExposeFile;
                        exposeRequest->exclusive = true;
                        exposeRequest->autoUnexpose = true;
                        exposeRequest->rootNode = NodeRequest::RootNode;
                        ConvertPath(&exposeRequest->subpath, "boot"_ko, "Conducto.tas"_ko, NULL);
                    }, [](Interface_Response *response){
                        Interface_File_Nubbin::ExposeResponse *exposeResponse = (Interface_File_Nubbin::ExposeResponse*)response;
                        if (exposeResponse->status != Interface_Response::Success) {
                            kprintf("STARTUP: Couldn't find task! Error %i\n", exposeResponse->status);
                            // TODO: Kernel doomed
                        }
                    });
                    return true;
                }, [=](IpcClient *client){
                    sink->AddTask([=]{
                        client->Connect(service);
                    });
                    _updater(new WaitingForFile(_updater, _nubbin));
                });
            }
        }

    private:
        KernelObject *_nubbin;
    };
    
    class WaitingForFilesystem : public Handler
    {
    public:
        WaitingForFilesystem(bicycle::function<void(Handler*)> updater)
        :Handler(updater)
        {
            START_LOG("STARTUP: Connecting filesystem\n");
            _nubbin = new FileNubbin();
            _nubbinInput = NULL;
            _filesystem = NULL;
        }
        
        void FoundInput(KernelObject *provider, IpcClient *client)
        {
            if (provider == _nubbin) {
                _nubbinInput = client;
                Check();
            }
        }
        
        void FoundOutput(KernelObject *provider, IpcService *service)
        {
            if (service->ServiceType()->IsEqualTo("filesystem"_ko)) {
                _filesystem = service;
                Check();
            }
        }
        
    private:
        IpcService *_filesystem;
        KernelObject *_nubbin;
        IpcClient *_nubbinInput;
        
        void Check(void)
        {
            if (!_nubbinInput || !_filesystem)
                return;
            _nubbinInput->Connect(_filesystem);
            _updater(new WaitingForNubbin(_updater, _nubbin));
        }
    };
    
    class WaitingForDisk : public Handler
    {
    public:
        WaitingForDisk(bicycle::function<void(Handler*)> updater)
        :Handler(updater)
        {
            START_LOG("STARTUP: Waiting for boot device\n");
            _filesystem = new FileSystem_ISO9660();
            _filesystemInput = NULL;
            _disk = NULL;
        }
        
        void FoundInput(KernelObject *provider, IpcClient *client)
        {
            if (provider == _filesystem) {
                _filesystemInput = client;
                Check();
            }
        }
        
        void FoundOutput(KernelObject *provider, IpcService *service)
        {
            if (service->Name()->IsEqualTo("atapi0"_ko) && service->ServiceType()->IsEqualTo(SERVICE_TYPE_BLOCK)) {
                _disk = service;
                Check();
            }
        }
        
    private:
        IpcService *_disk;
        KernelObject *_filesystem;
        IpcClient *_filesystemInput;
        
        void Check(void)
        {
            if (!_disk || !_filesystemInput)
                return;
            _filesystemInput->Connect(_disk);
            _updater(new WaitingForFilesystem(_updater));
        }
    };

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
        Startup_Internal::Handler *_state;
        
    public:
        StartupHandler()
        {
            START_LOG("STARTUP: Initialising\n");
            
            AutoreleasePool pool;
            
            _runloop = new RunloopThread();
            _monitor = new IpcServiceMonitor();
            _helper = new InterfaceHelper();
            
            _state = new Startup_Internal::WaitingForDisk([=](Startup_Internal::Handler *state){
                Handler *oldState = _state;
                if (oldState) {
                    _runloop->AddTask([oldState]{
                        delete oldState;
                    });
                }
                _state = state;
                if (!_state) {
                    _runloop->AddTask([this]{
                        Release();
                    });
                }
            });
            
            _runloop->AddSource(_monitor, [this](BlockableObject *object, KernelObject *other){
                KernelArray *changes = _monitor->Changes();
                if (!_state)
                    return;
//                report(changes);
                changes->Enumerate([this](KernelObject *object){
                    KernelDictionary *info = (KernelDictionary*)object;
                    KernelString *state = (KernelString*)info->ObjectFor("State"_ko);
                    KernelString *type = (KernelString*)info->ObjectFor("Type"_ko);
                    KernelNumber *handle = (KernelNumber*)info->ObjectFor("Connector"_ko);
                    KernelNumber *providerHandle = (KernelNumber*)info->ObjectFor("Provider"_ko);
                    KernelObject *provider = handle ? (IpcClient*)_monitor->ObjectForIdentifier(providerHandle->Value()) : NULL;
                    if (provider && state && type && state->IsEqualTo("Start"_ko)) {
                        if (type->IsEqualTo("Input"_ko)) {
                            IpcClient *client = handle ? (IpcClient*)_monitor->ObjectForIdentifier(handle->Value()) : NULL;
                            _state->FoundInput(provider, client);
                        } else if (type->IsEqualTo("Output"_ko)) {
                            IpcService *service = handle ? (IpcService*)_monitor->ObjectForIdentifier(handle->Value()) : NULL;
                            _state->FoundOutput(provider, service);
                        }
                    }
                    return (void*)NULL;
                });
            });
        }
        
    protected:
        ~StartupHandler()
        {
            START_LOG("STARTUP: Task complete! Stopping!\n");
            _runloop->RemoveSource(_monitor);   // TODO: Find out why you must manually remove it or it'll crash, and can't just rely on _runloop retaining it
            _helper->Release();
            _runloop->Release();
            _monitor->Release();
        }
    };
}

void InitStartup(void)
{
    new Startup_Internal::StartupHandler();
}
