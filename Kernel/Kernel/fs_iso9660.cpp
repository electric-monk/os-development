#include "IPC.h"
#include "Queue.h"
#include "Interface_File.h"
#include "Interface_Block.h"
#include "fs_iso9660.h"

typedef struct {
    UInt16 lsb;
    UInt16 msb;
} __attribute__((packed)) Iso9660DualWord;

typedef struct {
    UInt32 lsb;
    UInt32 msb;
} __attribute__((packed)) Iso9660DualLong;

typedef enum {
    vdtBootRecord = 0,
    vdtPrimaryVolumeDescriptor = 1,
    vdtSupplementaryVolumeDescriptor = 2,
    vdtVolumePartitionDescriptor = 3,
    // <reserved>
    vdtVolumeDescriptorSetTerminator = 255,
} VolumeDescriptorTypes;

typedef struct {
    UInt8 VolumeDescriptorType;
    char StandardId[5]; // "CD001"
    UInt8 StandardVersion;  // 1
    char Data[2014];
} __attribute__((packed)) GenericVolumeDescriptor;

typedef struct {
    UInt8 VolumeDescriptorType; // Primary volume descriptor == 1
    char StandardId[5]; // "CD001"
    UInt8 StandardVersion;  // 1
    UInt8 Flags;    // 0 in PVD
    char IdentifierSystem[32];
    char IdentifierVolume[32];
    char _Reserved1[8];
    Iso9660DualLong VolumeSize;
    char EscapeSequences[32];   // 0
    Iso9660DualWord VolumeSetSize;  // Number of volumes in set (1)
    Iso9660DualWord VolumeSetIndex; // Number in volume set (0)
    Iso9660DualWord LogicalBlockSize;   // Likely 2048
    Iso9660DualLong PathTableSize;
    UInt32 lsbPathTable1;   // mandatory
    UInt32 lsbPathTable2;   // optional
    UInt32 msbPathTable1;   // mandatory
    UInt32 msbPathTable2;   // optional
    char RootDirectoryRecord[34];
    char IdentifierVolumeSet[128];
    char IdentifierPublisher[128];
    char IdentifierDataPreparer[128];
    char IdentifierApplication[128];
    char IdentifierCopyrightFile[37];
    char IdentifierAbstractFile[37];
    char IdentifierBibliographicFile[37];
    char VolumeCreation[17];
    char VolumeModification[17];
    char VolumeExperiation[17];
    char VolumeEffective[17];
    char FileStructureStandardVersion;
    char _Reserved2;
    char ApplicationUse[512];
    char _Reserved3[653];
} __attribute__((packed)) PrimaryVolumeDescriptor;

typedef struct {
    UInt8 DirectoryIdentifierLength;
    UInt8 XARLength;
    UInt32 DirectoryLocation;
    UInt16 ParentDirectoryNumber;
    char DirectoryIdentifier[0];
} __attribute__((packed)) PathTableRecord;

typedef struct {
    UInt8 DirectoryRecordLength;
    UInt8 XARLength;
    Iso9660DualLong Start;
    Iso9660DualLong DataLength;
    UInt8 Year; // From 1900
    UInt8 Month;
    UInt8 Day;
    UInt8 Hour;
    UInt8 Minute;
    UInt8 Second;
    UInt8 GMTOffset;    // 15- minute offset from UTC
    UInt8 FileFlags;
    UInt8 InterleaveSize;
    UInt8 InterleaveSkip;
    Iso9660DualWord VolumeSetIndex;
    UInt8 FileIdentifierLength;
    char FileIdentifier[0];
} __attribute__((packed)) DirectoryRecord;

#define VolumeDescriptorType_PrimaryVolume  1
#define VolumeDescriptorType_Terminator     255

#define FileFlag_Hidden     (1 << 0)
#define FileFlag_Directory  (1 << 1)
#define FileFlag_Associated (1 << 2)
#define FileFlag_Extended   (1 << 3)
#define FileFlag_Permiss    (1 << 4)
#define FileFlag_More       (1 << 7)

#define SECTOR_SIZE         2048

namespace ISO9660Driver {
    
    class GenericEntry : public KernelObject
    {
    private:
        class InterimDate
        {
        private:
            UInt8 Year; // From 1900
            UInt8 Month;
            UInt8 Day;
            UInt8 Hour;
            UInt8 Minute;
            UInt8 Second;
        public:
            InterimDate(const DirectoryRecord *record)
            {
                Year = record->Year;
                Month = record->Month;
                Day = record->Day;
                Hour = record->Hour;
                Minute = record->Minute;
                Second = record->Second;
            }
            
            void Save(FlatDictionary *output)
            {
                FlatDate *fd = (FlatDate*)output->GetNextAddress();
                fd->Initialise();
                fd->Year = Year + 1900;
                fd->Month = Month;
                fd->Day = Day;
                fd->Hour = Hour;
                fd->Minute = Minute;
                fd->Second = Second;
                fd->Millisecond = 0;
                output->CompleteNextItem();
            }
        };
    public:
        GenericEntry(const DirectoryRecord *record)
        :_creationDate(record)
        {
            start = SECTOR_SIZE * record->Start.lsb;
            length = record->DataLength.lsb;
            _isDirectory = record->FileFlags & FileFlag_Directory;
            UInt32 nameLength = record->FileIdentifierLength;
            if (!_isDirectory) {
                // Find the ;
                while ((record->FileIdentifier[nameLength - 1] != ';') && (nameLength != 0))
                    nameLength--;
                // Also find a . if it's at the end
                if ((record->FileIdentifier[nameLength - 2] == '.') && (nameLength > 1))
                    nameLength --;
                // If there wasn't one, give up
                if (nameLength == 0)
                    nameLength = record->FileIdentifierLength;
                else {
                    nameLength--;   // We want to remove the last character
                }
            }
            name = new KernelString(record->FileIdentifier, nameLength);
        }
        
        UInt32 node;    // Assigned pretty much at random, as disk contents are found.
        
        UInt32 start, length;
        
        KernelString *name;
        
        bool IsDirectory(void)
        {
            return _isDirectory;
        }
        
        void Encode(FlatDictionary *output)
        {
            // Node ID
            SaveString(output, Node_ID);    SaveInteger(output, node);
            // Name
            SaveString(output, Node_Name);  SaveString(output, name->CString());
            // Type
            SaveString(output, Node_Type);  SaveString(output, _isDirectory ? NoteType_Directory : NoteType_PlainFile);
            // Size
            if (!_isDirectory) {
                SaveString(output, Node_Size);  SaveInteger(output, length);
            }
            // Date
            SaveString(output, Node_Date_Create);   _creationDate.Save(output);
        }
        
    protected:
        ~GenericEntry()
        {
            name->Release();
        }
        
    private:
        bool _isDirectory;
        InterimDate _creationDate;
        
        static void SaveString(FlatArray *output, const char *key)
        {
            FlatString *fs = (FlatString*)output->GetNextAddress();
            fs->Initialise(key);
            output->CompleteNextItem();
        }
        static void SaveInteger(FlatArray *output, UInt64 value)
        {
            FlatInteger *fi = (FlatInteger*)output->GetNextAddress();
            fi->Initialise(value);
            output->CompleteNextItem();
        }
    };

    class FileEntry : public GenericEntry
    {
    public:
        FileEntry(DirectoryRecord *record)
        :GenericEntry(record)
        {
            
        }
    };
    
    class DirectoryEntry : public GenericEntry
    {
    private:
        static GenericEntry* Create(UInt64 &nodeCounter, DirectoryRecord *record)
        {
            GenericEntry *result;
            if (record->FileFlags & FileFlag_Directory)
                result = new DirectoryEntry(record);
            else
                result = new FileEntry(record);
            result->node = nodeCounter++;
            return result;
        }
    public:
        DirectoryEntry(DirectoryRecord *record)
        :GenericEntry(record)
        {
            _entries = NULL;
        }
        
        bool Loaded(void)
        {
            return _entries != NULL;
        }
        
        void Parse(UInt64 &nodeCounter, void *data)
        {
            // Create storage
            if (_entries != NULL)
                _entries->Release();
            _entries = new KernelArray();
            // Set up search
            char *start = (char*)data;
            char *cursor = start;
            while (cursor < (start + length)) {
                // Get entry
                DirectoryRecord *record = (DirectoryRecord*)cursor;
                if (record->DirectoryRecordLength == 0) {
                     // Sectors are padded with 0 to avoid a directory record crossing boundaries. If we find a zero, jump to the start of the next sector
                    cursor += SECTOR_SIZE - (UInt32(cursor - start) % SECTOR_SIZE);
                } else {
                    bool use = true;
                    // Check name - if it's '.' or '..', skip
                    if (record->FileIdentifierLength == 1) {
                        if (record->FileIdentifier[0] == '\0') // ISO9660 representation of '.'
                            use = false;
                        else if (record->FileIdentifier[0] == '\1') // ISO9660 representation of '..'
                            use = false;
                    }
                    // Create entry if necessary
                    if (use) {
                        GenericEntry *entry = Create(nodeCounter, record);
                        _entries->Add(entry);
                        entry->Release();
                    }
                    cursor += record->DirectoryRecordLength;
                }
            }
        }
        
        KernelArray* Entries(void)
        {
            return _entries;
        }
        
        GenericEntry* SearchEntry(UInt32 node)
        {
            UInt32 i, j = _entries->Count();
            for (i = 0; i < j; i++) {
                GenericEntry *entry = (GenericEntry*)_entries->ObjectAt(i);
                if (node == entry->node)
                    return entry;
            }
            return NULL;
        }
        
        GenericEntry* SearchEntry(KernelString *name)
        {
            UInt32 i, j = _entries->Count();
            for (i = 0; i < j; i++) {
                GenericEntry *entry = (GenericEntry*)_entries->ObjectAt(i);
                if (name->IsEqualTo(entry->name))
                    return entry;
            }
            return NULL;
        }
        
        GenericEntry* FindNode(UInt32 node)
        {
            // First, is it us?
            if (this->node == node)
                return this;
            // Otherwise, we ourselves need to be loaded for this to work :(
            if (!Loaded())
                return NULL;
            // Search our contents
            UInt32 i, j = _entries->Count();
            for (i = 0; i < j; i++) {
                GenericEntry *entry = (GenericEntry*)_entries->ObjectAt(i);
                if (entry->node == node)
                    return entry;
                if (entry->IsDirectory()) {
                    DirectoryEntry *dir = (DirectoryEntry*)entry;
                    GenericEntry *result = dir->FindNode(node);
                    if (result != NULL)
                        return result;
                }
            }
            return NULL;
        }
        
    protected:
        ~DirectoryEntry()
        {
            if (_entries != NULL)
                _entries->Release();
        }
        
    private:
        KernelArray *_entries;
    };
    
    class Interface_File_Output : public GenericProvider::OutputConnection
    {
    public:
        Interface_File_Output(FileSystem_ISO9660 *owner, GenericProvider::Service *service, IpcEndpoint *endpoint)
        :GenericProvider::OutputConnection(owner, service, endpoint)
        {
            _handles = new KernelDictionary();
            _handleCount = 0;
        }
        
        UInt32 Open(FileEntry *file)
        {
            KernelNumber *handle = new KernelNumber(_handleCount++);
            _handles->Set(handle, file);
            UInt32 result = handle->Value();
            handle->Release();
            return result;
        }
        
        void Close(UInt32 fileHandle)
        {
            KernelNumber *handle = new KernelNumber(fileHandle);
            _handles->Set(handle, NULL);
            handle->Release();
        }
        
        FileEntry* FileForHandle(UInt32 fileHandle)
        {
            KernelNumber *handle = new KernelNumber(fileHandle);
            FileEntry *file = (FileEntry*)_handles->ObjectFor(handle);
            handle->Release();
            return file;
        }
        
    protected:
        ~Interface_File_Output()
        {
            _handles->Release();
        }
        
    private:
        KernelDictionary *_handles;
        UInt32 _handleCount;
    };
}

FileSystem_ISO9660::FileSystem_ISO9660()
{
    _tasks = new InterfaceHelper();
    _nodeCounter = 0;
}

FileSystem_ISO9660::~FileSystem_ISO9660()
{
    _tasks->Release();
}

UInt32 FileSystem_ISO9660::InputCount(void)
{
    return 1;
}

#define INPUT_NAME          "cdrom"_ko

KernelDictionary* FileSystem_ISO9660::Input(UInt32 index)
{
    return NULL;    // TODO: This
}

void FileSystem_ISO9660::ReadGVD(int offset)
{
    InputConnection *input = (InputConnection*)_inputs->ObjectFor(INPUT_NAME);
    if (input == NULL)
        return; // ???
    _tasks->PerformTask(input->Link(), [offset](Interface_Request *request){
        BlockRequestRead *read = (BlockRequestRead*)request;
        read->type = BlockRequest::Read;
        read->offset = SECTOR_SIZE * (0x10 + offset);
        read->length = SECTOR_SIZE;
        return 0;
    }, [this, offset](Interface_Response *response){
        BlockResponseRead *blockResponse = (BlockResponseRead*)response;
        // Check it succeeded
        if (blockResponse->status != Interface_Response::Success)
            return 0/* TODO: Error state: I/O error */;
        // Check we got at least the required sector
        if (blockResponse->length() < SECTOR_SIZE)
            return 0/* TODO: Error state: read failure */;
        // Get sector
        GenericVolumeDescriptor *gvd = (GenericVolumeDescriptor*)blockResponse->data();
        // Check magic numbers
        if (gvd->StandardVersion != 1)
            return 0/* TODO: Error state: invalid filesystem */;
        if ((gvd->StandardId[0] != 'C') || (gvd->StandardId[1] != 'D') || (gvd->StandardId[2] != '0') || (gvd->StandardId[3] != '0') || (gvd->StandardId[4] != '1'))
            return 0/* TODO: Error state: invalid filesystem */;
        if (gvd->VolumeDescriptorType == VolumeDescriptorType_Terminator)
            return 0/* TODO: Error state: invalid filesystem */;
        if (gvd->VolumeDescriptorType != VolumeDescriptorType_PrimaryVolume) {
            ReadGVD(offset + 1);
            return 0;
        }
        // Get actual volume info!
        PrimaryVolumeDescriptor *pvd = (PrimaryVolumeDescriptor*)gvd;
        _rootDirectory = new ISO9660Driver::DirectoryEntry((DirectoryRecord*)&pvd->RootDirectoryRecord);
        _rootDirectory->node = (UInt32)NodeRequest::RootNode;
        // Launch service
        IpcService *ipcService = new IpcService("iso9660"_ko, "filesystem"_ko);
        GenericProvider::Service *service = new GenericProvider::Service(this, ipcService);
        ipcService->Release();
        Launch(service);
        service->Release();
        return 0;
    });
}

GenericProvider::InputConnection* FileSystem_ISO9660::InputConnectionStart(KernelString *name, IpcEndpoint *connection)
{
    if (!name->IsEqualTo(INPUT_NAME))
        return NULL;
    InputConnection *newConnection = new InputConnection(this, name, connection);
    // Prepare to handle request response
    _queue->AddTask([this](){
        ReadGVD(0);
        return 0;
    });
    // Done
    return newConnection;
}

void FileSystem_ISO9660::InputConnectionReceived(GenericProvider::InputConnection *connection, KernelBufferMemory *message)
{
    _tasks->HandleMessage(message, [](Interface_Response *response){
        // TODO: Non-response message from the service
        return 0;
    });
}

void FileSystem_ISO9660::InputConnectionEnd(InputConnection *connection)
{
    // We're done :(
    connection->Release();
}

GenericProvider::OutputConnection* FileSystem_ISO9660::OutputConnectionStart(Service *source, IpcEndpoint *connection)
{
    return new ISO9660Driver::Interface_File_Output(this, source, connection);
}

GenericProvider::InputConnection* FileSystem_ISO9660::Input(void)
{
    return (GenericProvider::InputConnection*)_inputs->ObjectFor(INPUT_NAME);
}

#define LOADED_OK       0
#define LOADED_NOTFOUND 1
#define LOADED_NOTDIR   2
#define LOADED_ERROR    3
#define LOADED_BADROOT  4

void FileSystem_ISO9660::EnsureEntryLoaded(ISO9660Driver::GenericEntry *entry, bicycle::function<int(int)> onLoaded)
{
    if (!entry->IsDirectory()) {
        onLoaded(LOADED_NOTDIR);
        return;
    }
    ISO9660Driver::DirectoryEntry *dir = (ISO9660Driver::DirectoryEntry*)entry;
    if (dir->Loaded()) {
        onLoaded(LOADED_OK);
        return;
    }
    _tasks->PerformTask(Input()->Link(), [entry](Interface_Request *request){
        BlockRequestRead *readRequest = (BlockRequestRead*)request;
        readRequest->type = BlockRequest::Read;
        readRequest->offset = entry->start;
        readRequest->length = entry->length;
        return 0;
    }, [onLoaded, dir, this](Interface_Response *response){
        BlockResponseRead *readResponse = (BlockResponseRead*)response;
        if (readResponse->status != Interface_Response::Success) {
            onLoaded(LOADED_ERROR);
            return -1;
        }
        dir->Parse(_nodeCounter, readResponse->data());
        onLoaded(LOADED_OK);
        return 0;
    });
}

void FileSystem_ISO9660::ParsePath(ISO9660Driver::GenericEntry *current, FlatArray *path, UInt32 pathIndex, bicycle::function<int(ISO9660Driver::GenericEntry*)> onFound, bicycle::function<int(UInt32 failedIndex, UInt32 reason)> onNotFound)
{
    bool lastItem = path->Count() == pathIndex;
    if (!current->IsDirectory()) {
        if (lastItem)
            onFound(current);
        else
            onNotFound(pathIndex, LOADED_NOTDIR);
        return;
    }
    EnsureEntryLoaded(current, [=](int response){
        if (response != LOADED_OK) {
            onNotFound(pathIndex, response);
            return -1;
        }
        if (!lastItem) {
            ISO9660Driver::DirectoryEntry *dir = (ISO9660Driver::DirectoryEntry*)current;
            ISO9660Driver::GenericEntry *next = NULL;
            FlatObject *entry = path->ItemAt(pathIndex);
            if (entry->type == FlatInteger::Type()) {
                FlatInteger *integ = (FlatInteger*)entry;
                next = dir->SearchEntry(integ->Value());
            } else if (entry->type == FlatString::Type()) {
                FlatString *strang = (FlatString*)entry;
                KernelString *str = new KernelString((char*)strang->Value(), strang->Length());
                next = dir->SearchEntry(str);
                str->Release();
            }
            if (next == NULL) {
                onNotFound(pathIndex, LOADED_NOTFOUND);
                return -1;
            }
            ParsePath(next, path, pathIndex + 1, onFound, onNotFound);
            return 0;
        } else {
            onFound(current);
            return 0;
        }
    });
}

void FileSystem_ISO9660::ParsePath(UInt64 startNode, FlatArray *path, UInt32 pathIndex, bicycle::function<int(ISO9660Driver::GenericEntry*)> onFound, bicycle::function<int(UInt32 failedIndex, UInt32 reason)> onNotFound)
{
    ISO9660Driver::GenericEntry *node = _rootDirectory->FindNode(startNode);
    if (node == NULL)
        onNotFound(0, LOADED_BADROOT);
    else
        ParsePath(node, path, pathIndex, onFound, onNotFound);
}

static UInt32 GetErrorResponse(UInt32 reason)
{
    switch(reason) {
        case LOADED_ERROR:
            return NodeResponse::IOError;
        case LOADED_NOTDIR:
            return NodeResponse::NotDirectory;
        case LOADED_NOTFOUND:
            return NodeResponse::NotFound;
        case LOADED_BADROOT:
            return NodeResponse::InvalidNode;
        default:
            return NodeResponse::Unsupported;
    }
}

void FileSystem_ISO9660::OutputConnectionMessage(OutputConnection *connection, KernelBufferMemory *message)
{
    KernelBufferMemory::Map *mapping = new KernelBufferMemory::Map(NULL, message, true);
    Interface_Request *request = (Interface_Request*)mapping->LinearBase();
    
    switch (request->type) {
        case NodeRequest::Search:
        {
            DirectoryRequest *readDirectory = (DirectoryRequest*)request;
            // Find the right directory
            mapping->AddRef();
            bicycle::function<int(UInt32 failedIndex, UInt32 reason)> errorHandler = [mapping, connection, readDirectory](UInt32 failedIndex, UInt32 reason){
                // Return 'not found' or whatever is approriate for reason :(
                connection->Link()->SendMessage([mapping, readDirectory, reason](void *context){
                    DirectoryResponse *response = (DirectoryResponse*)context;
                    response->Fill(readDirectory);
                    response->status = GetErrorResponse(reason);
                    response->directoryEntries.Initialise();
                    return true;
                }, 4096);
                mapping->Release();
                return 0;
            };
            ParsePath(readDirectory->rootNode, &readDirectory->subpath, 0, [mapping, connection, readDirectory, errorHandler](ISO9660Driver::GenericEntry *foundDirectory){
                if (foundDirectory->IsDirectory()) {
                    // Got the required directory
                    connection->Link()->SendMessage([readDirectory, foundDirectory](void *context){
                        DirectoryResponse *response = (DirectoryResponse*)context;
                        response->status = NodeResponse::Success;
                        response->Fill(readDirectory);
                        response->directoryEntries.Initialise();
                        KernelArray *items = ((ISO9660Driver::DirectoryEntry*)foundDirectory)->Entries();
                        UInt32 max = items->Count();
                        for (UInt32 i = readDirectory->offset; (i < readDirectory->length) && (i < max); i++) {
                            ISO9660Driver::GenericEntry *entry = (ISO9660Driver::GenericEntry*)items->ObjectAt(i);
                            FlatDictionary *dictionary = (FlatDictionary*)response->directoryEntries.GetNextAddress();
                            dictionary->Initialise();
                            entry->Encode(dictionary);
                            response->directoryEntries.CompleteNextItem();
                        }
                        return true;
                    });
                    mapping->Release();
                } else {
                    // It wasn't a directory
                    errorHandler(0, LOADED_NOTDIR);
                }
                return 0;
            }, errorHandler);
        }
            break;
        case NodeRequest::OpenFile:
        {
            OpenRequest *openRequest = (OpenRequest*)request;
            // Find the node
            bicycle::function<int(UInt32 failedIndex, UInt32 reason)> errorHandler = [connection, openRequest](UInt32 failedIndex, UInt32 reason){
                // Return 'not found' or whatever is approriate for reason :(
                connection->Link()->SendMessage([openRequest, reason](void *context){
                    OpenResponse *response = (OpenResponse*)context;
                    response->status = GetErrorResponse(reason);
                    response->Fill(openRequest);
                    return true;
                }, 4096);
                return 0;
            };
            mapping->AddRef();
            ParsePath(openRequest->rootNode, &openRequest->subpath, 0, [mapping, connection, errorHandler, openRequest](ISO9660Driver::GenericEntry *foundNode){
                if (foundNode->IsDirectory()) {
                    // It wasn't a file
                    errorHandler(0, LOADED_NOTFOUND);
                } else {
                    // Got the file, so make a handle
                    ISO9660Driver::Interface_File_Output *outputConnection = (ISO9660Driver::Interface_File_Output*)connection;
                    UInt32 handle = outputConnection->Open((ISO9660Driver::FileEntry*)foundNode);
                    // Reply!
                    outputConnection->Link()->SendMessage([openRequest, handle](void *context){
                        OpenResponse *response = (OpenResponse*)context;
                        response->status = NodeResponse::Success;
                        response->Fill(openRequest);
                        response->handle = handle;
                        return true;
                    });
                }
                mapping->Release();
                return 0;
            }, errorHandler);
        }
            break;
        case NodeRequest::CloseFile:
        {
            FileRequest *closeRequest = (FileRequest*)request;
            // Do it
            ISO9660Driver::Interface_File_Output *outputConnection = (ISO9660Driver::Interface_File_Output*)connection;
            outputConnection->Close(closeRequest->handle);
            // Reply!
            outputConnection->Link()->SendMessage([closeRequest](void *context){
                NodeResponse *response = (NodeResponse*)context;
                response->status = NodeResponse::Success;
                response->Fill(closeRequest);
                return true;
            });
        }
            break;
        case NodeRequest::ReadFile:
        {
            ReadRequest *readRequest = (ReadRequest*)request;
            ISO9660Driver::Interface_File_Output *outputConnection = (ISO9660Driver::Interface_File_Output*)connection;
            mapping->AddRef();
            bicycle::function<int(UInt32 status, void *data, UInt32 length)> replyHandler = [mapping, outputConnection, readRequest](UInt32 status, void *data, UInt32 length){
                outputConnection->Link()->SendMessage([readRequest, status, data, length](void *context){
                    ReadResponse *response = (ReadResponse*)context;
                    response->Fill(readRequest);
                    response->status = status;
                    response->readOffset = 0;
                    response->readLength = length;
                    if (status == Interface_Response::Success)
                        CopyMemory(response->data(), data, length);
                    else
                        response->readLength = 0;
                    return true;
                });
                mapping->Release();
                return 0;
            };
            // Get file
            ISO9660Driver::FileEntry *entry = outputConnection->FileForHandle(readRequest->handle);
            if (entry == NULL) {
                // No file?
                replyHandler(NodeResponse::InvalidHandle, NULL, 0);
            } else {
                // Read file
                _tasks->PerformTask(Input()->Link(), [entry, readRequest](Interface_Request *request){
                    BlockRequestRead *blockRequest = (BlockRequestRead*)request;
                    blockRequest->type = BlockRequest::Read;
                    UInt32 actualOffset = entry->start + readRequest->offset;
                    blockRequest->offset = actualOffset & ~(SECTOR_SIZE - 1);
                    blockRequest->length = readRequest->length + (actualOffset - blockRequest->offset);
                    UInt32 baseSizeRemainder = blockRequest->length % SECTOR_SIZE;
                    if (baseSizeRemainder != 0)
                        blockRequest->length += SECTOR_SIZE - baseSizeRemainder;
                    return 0;
                }, [replyHandler](Interface_Response *response){
                    BlockResponseRead *readResponse = (BlockResponseRead*)response;
                    replyHandler(readResponse->status, readResponse->data(), readResponse->requestedLength);
                    return 0;
                });
            }
        }
            break;
        default:
            connection->Link()->SendMessage([request](void *context){
                Interface_Response *response = (Interface_Response*)context;
                response->Fill(request);
                response->status = Interface_Response::Unsupported;
                return true;
            });
            break;
    }
    mapping->Release();
}

void FileSystem_ISO9660::OutputConnectionEnd(OutputConnection *oldConnection)
{
    oldConnection->Release();
}
