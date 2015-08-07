#include "FileNubbin.h"
#include "Interface_File_Nubbin.h"
#include "Interface_File.h"
#include "Interface_Block.h"
#include "Queue.h"
#include "IPC.h"

FileNubbin::FileNubbin()
{
    _tasks = new InterfaceHelper();
}

FileNubbin::~FileNubbin()
{
    _tasks->Release();
}

UInt32 FileNubbin::InputCount(void)
{
    return 1;
}

#define INPUT_NAME          "filesystem"_ko

KernelDictionary* FileNubbin::Input(UInt32 index)
{
    return NULL;    // TODO: This
}

/* A connection class to indicate the type */
class NubbinConnection : public GenericProvider::OutputConnection
{
public:
    NubbinConnection(FileNubbin *owner, GenericProvider::Service *service, IpcEndpoint *connection)
    :GenericProvider::OutputConnection(owner, service, connection)
    {
    }
    
    virtual void ProcessMessage(Interface_Request *request, bicycle::function<int(bicycle::function<bool(Interface_Response*)>)> sendReply) = 0;
};

/* The service for the command interface - allowing exposition of files in the attached file system */
class NubbinService : public GenericProvider::Service
{
private:
public:
    NubbinService(FileNubbin *owner, IpcService *service)
    :GenericProvider::Service(owner, service)
    {
    }
    
    virtual bool IsCommand(void)
    {
        return true;
    }
};

/* The service for actual exposed files - since every client gets the same file, this service can just open the file itself */
class NubbinHandle : public NubbinService
{
public:
    typedef bicycle::function<int(bicycle::function<int(Interface_Request*)>, bicycle::function<int(Interface_Response*)>)> RequesterFunction;
private:
    RequesterFunction _requester;
    
    bool _exclusive;
    bool _autoUnexpose;
    bool _allowEject;
    UInt32 _openCount;
    
    UInt32 _handle;
    
public:
    NubbinHandle(FileNubbin *owner, IpcService *service, Interface_File_Nubbin::Expose *request, RequesterFunction requester, bicycle::function<int(int, NubbinHandle*)> completion)
    :NubbinService(owner, service)
    {
        _requester = requester;
        _exclusive = request->exclusive;
        _autoUnexpose = request->autoUnexpose;
        _allowEject = false;    // TODO: Also a nice option
        _requester([request](Interface_Request *newRequest){
            OpenRequest *openRequest = (OpenRequest*)newRequest;
            openRequest->type = NodeRequest::OpenFile;
            openRequest->rootNode = request->rootNode;
            openRequest->subpath.CopyFrom(&request->subpath);
            return 0;
        }, [this, completion](Interface_Response *response){
            OpenResponse *openResponse = (OpenResponse*)response;
            if (openResponse->status == Interface_Response::Success)
                _handle = openResponse->handle;
            completion(openResponse->status, this);
            Release();
            return 0;
        });
    }
    
    bool IsCommand(void)
    {
        return false;
    }
    
    bool CanEject(void)
    {
        return _allowEject;
    }
    
    bool TryConnect(void)   // Returns false if you can't open
    {
        if (_exclusive && (_openCount != 0))
            return false;
        _openCount++;
        return true;
    }
    
    bool TryDisconnect(void)    // Returns true if you should release the service
    {
        _openCount--;
        return (_openCount == 0) && _autoUnexpose;
    }
    
    void UpdateConnection(bool connected)
    {
        _openCount += connected ? 1 : -1;
    }
    
    UInt32 Value(void)
    {
        return _handle;
    }
    
protected:
    ~NubbinHandle()
    {
        _requester([this](Interface_Request *newRequest){
            FileRequest *closeRequest = (FileRequest*)newRequest;
            closeRequest->type = NodeRequest::CloseFile;
            closeRequest->handle = _handle;
            return 0;
        }, [](Interface_Response *response){
            NodeResponse *closeResponse = (NodeResponse*)response;
            if (closeResponse->status != Interface_Response::Success)
                /* Whoops, should probably do something */;
            return 0;
        });
    }
};

GenericProvider::InputConnection* FileNubbin::InputConnectionStart(KernelString *name, IpcEndpoint *connection)
{
    if (!name->IsEqualTo(INPUT_NAME))
        return NULL;
    InputConnection *newConnection = new InputConnection(this, name, connection);
    _queue->AddTask([this](){
        // We have a filesystem, so we can export our control service
        IpcService *ipcService = new IpcService("control"_ko, "nubbin"_ko);
        GenericProvider::Service *service = new NubbinService(this, ipcService);
        ipcService->Release();
        Launch(service);
        service->Release();
        return 0;
    });
    return newConnection;
}

void FileNubbin::InputConnectionReceived(GenericProvider::InputConnection *connection, KernelBufferMemory *message)
{
    _tasks->HandleMessage(message, [](Interface_Response *response){
        // TODO: Non-response message from the service
        return 0;
    });
}

void FileNubbin::InputConnectionEnd(GenericProvider::InputConnection *connection)
{
    _queue->AddTask([this](){
        // We lost our file service, so kill all of the files we're exporting along with the control service
        while (_services->Count() != 0)
            Kill((Service*)_services->ObjectAt(0));
        return 0;
    });
    connection->Release();
}

GenericProvider::InputConnection* FileNubbin::Input(void)
{
    return (InputConnection*)_inputs->ObjectFor(INPUT_NAME);
}

GenericProvider::OutputConnection* FileNubbin::OutputConnectionStart(Service *source, IpcEndpoint *connection)
{
    NubbinService *service = (NubbinService*)source;
    if (!service->IsCommand()) {
        NubbinHandle *handleService = (NubbinHandle*)service;
        if (!handleService->TryConnect())
            return NULL;
    }
    return new GenericProvider::OutputConnection(this, service, connection);
}

static GenericProvider::Service* Find(KernelArray *services, const char *name)
{
    KernelString *sName = new KernelString(name);
    KernelObject *result = (KernelObject*)services->Enumerate([sName](KernelObject *object) -> void*{
        GenericProvider::Service *service = (GenericProvider::Service*)object;
        if (service->ServiceObject()->Name()->IsEqualTo(sName))
            return service;
        return NULL;
    });
    sName->Release();
    return (GenericProvider::Service*)result;
}

static GenericProvider::Service* Find(KernelArray *services, UInt64 value)
{
    return (GenericProvider::Service*)services->Enumerate([value](KernelObject *object) -> void*{
        NubbinService *service = (NubbinService*)object;
        if (service->IsCommand())
            return NULL;
        NubbinHandle *handle = (NubbinHandle*)service;
        if (handle->Value() == value)
            return service;
        return NULL;
    });
}

void FileNubbin::OutputConnectionMessage(GenericProvider::OutputConnection *connection, KernelBufferMemory *message)
{
    KernelBufferMemory::Map *mapping = new KernelBufferMemory::Map(NULL, message, true);
    message->AddRef();
    NubbinService *service = (NubbinService*)connection->Source();
    Interface_Request *request = (Interface_Request*)mapping->LinearBase();
    bicycle::function<int(int state)> doError = [connection, request, mapping, message](int state){
        connection->Link()->SendMessage([request, state](void *context){
            Interface_Response *response = (Interface_Response*)context;
            response->Fill(request);
            response->status = state;
            return true;
        });
        mapping->Release();
        message->Release();
        return 0;
    };
    switch (request->type | (service->IsCommand() ? 0xFF000000 : 0xFE000000)) {
        case Interface_File_Nubbin::Command::ExposeFile | 0xFF000000:
        {
            IpcService *service = new IpcService(/*name*/"0"_ko, SERVICE_TYPE_BLOCK);
            new NubbinHandle(this, service, (Interface_File_Nubbin::Expose*)request, [this](bicycle::function<int(Interface_Request*)> request, bicycle::function<int(Interface_Response*)> response){
                _tasks->PerformTask(Input()->Link(), request, response);
                return 0;
            }, [this, connection, request, mapping, message](int state, NubbinHandle *handle){
                if (state == Interface_Response::Success)
                    Launch(handle);
                connection->Link()->SendMessage([request, handle](void *context){
                    Interface_File_Nubbin::ExposeResponse *response = (Interface_File_Nubbin::ExposeResponse*)context;
                    response->Fill(request);
                    response->handle = handle->Value();
                    response->connectionName.Initialise("0");
                    return true;
                });
                mapping->Release();
                message->Release();
                return 0;
            });
        }
            break;
        case Interface_File_Nubbin::Command::UnexposeFile | 0xFF000000:
        {
            Interface_File_Nubbin::Unexpose *request = (Interface_File_Nubbin::Unexpose*)request;
            Service *found = NULL;
            bool invalid = false;
            if (request->exposedFile.type == FlatString::Type())
                found = Find(_services, ((FlatString*)&request->exposedFile)->Value());
            else if (request->exposedFile.type == FlatInteger::Type())
                found = Find(_services, ((FlatInteger*)&request->exposedFile)->Value());
            else
                invalid = true;
            if (invalid) {
                doError(/* invalid type */Interface_Response::Unsupported);
            } else if (found == NULL) {
                doError(/* not found */Interface_Response::Unsupported);
            } else {
                Kill(found);
                doError(Interface_Response::Success);
            }
        }
            break;
        case BlockRequest::Read | 0xFE000000:
            _tasks->PerformTask(Input()->Link(), [request](Interface_Request *newRequest){
                BlockRequestRead *blockReadRequest = (BlockRequestRead*)request;
                ReadRequest *fileReadRequest = (ReadRequest*)newRequest;
                fileReadRequest->type = ReadRequest::ReadFile;
                fileReadRequest->offset = blockReadRequest->offset;
                fileReadRequest->length = blockReadRequest->length;
                return 0;
            }, [connection, request, mapping, message](Interface_Response *response){
                ReadResponse *fileReadResponse = (ReadResponse*)response;
                connection->Link()->SendMessage([request, fileReadResponse](void *context){
                    BlockResponseRead *newResponse = (BlockResponseRead*)context;
                    newResponse->Fill((BlockRequestRead*)request);
                    newResponse->status = fileReadResponse->status;
                    if (fileReadResponse->status == Interface_Response::Success) {
                        newResponse->requestedOffset = fileReadResponse->readOffset = fileReadResponse->readOffset;
                        newResponse->requestedLength = fileReadResponse->readLength = fileReadResponse->readLength;
                        CopyMemory(newResponse->rawData(), fileReadResponse->data(), fileReadResponse->readLength);
                    }
                    return true;
                });
                mapping->Release();
                message->Release();
                return 0;
            });
            break;
//        case BlockRequest::Write | 0xFE000000:
//            break;
        case BlockRequest::Eject | 0xFE000000:
            if (((NubbinHandle*)service)->CanEject()) {
                Kill(service);
                connection->Link()->SendMessage([request](void *context){
                    Interface_Response *response = (Interface_Response*)context;
                    response->Fill(request);
                    response->status = Interface_Response::Success;
                    return true;
                });
                mapping->Release();
                message->Release();
            } else {
                doError(Interface_Response::Unsupported);
            }
            break;
        default:
            doError(Interface_Response::Unsupported);
            break;
    }
}

void FileNubbin::OutputConnectionEnd(GenericProvider::OutputConnection *oldConnection)
{
    NubbinService *service = (NubbinService*)oldConnection->Source();
    if (!service->IsCommand()) {
        NubbinHandle *handleService = (NubbinHandle*)service;
        if (handleService->TryDisconnect()) {
            _queue->AddTask([this, handleService](){
                Kill(handleService);
                return 0;
            });
        }
    }
    oldConnection->Release();
}