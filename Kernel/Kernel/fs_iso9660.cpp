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
    public:
        GenericEntry(DirectoryRecord *record)
        {
            start = SECTOR_SIZE * record->Start.lsb;
            length = record->DataLength.lsb;
            name = new KernelString(record->FileIdentifier, record->FileIdentifierLength);
            _isDirectory = record->FileFlags & FileFlag_Directory;
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
        }
        
    protected:
        ~GenericEntry()
        {
            name->Release();
        }
        
    private:
        bool _isDirectory;
        
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
        static GenericEntry* Create(DirectoryRecord *record)
        {
            if (record->FileFlags & FileFlag_Directory)
                return new DirectoryEntry(record);
            else
                return new FileEntry(record);
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
        
        void Parse(void *data)
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
                        GenericEntry *entry = Create(record);
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
}

FileSystem_ISO9660::FileSystem_ISO9660()
{
    _tasks = new KernelDictionary();
    _identifier = 0;
}

FileSystem_ISO9660::~FileSystem_ISO9660()
{
    _tasks->Release();
}

class FileSystem_ISO9660_Handler : public KernelFunction<int(Interface_Response*)>
{
public:
    FileSystem_ISO9660_Handler(bicycle::function<int(Interface_Response*)> callback) : KernelFunction<int(Interface_Response*)>(callback) {}
    
    static void Handle(KernelDictionary *handlers, KernelBufferMemory *responseMemory, bicycle::function<int(Interface_Response*)> onUnhandled)
    {
        // Map it in
        KernelBufferMemory::Map *mapping = new KernelBufferMemory::Map(NULL, responseMemory, true);
        Interface_Response *response = (Interface_Response*)mapping->LinearBase();
        // Get handler ID
        KernelNumber *number = new KernelNumber(response->originalRequest.identifier);
        // Find task
        FileSystem_ISO9660_Handler *task = (FileSystem_ISO9660_Handler*)handlers->ObjectFor(number);
        if (task == NULL) {
            onUnhandled(response);
        } else {
            task->Pointer()(response);
            handlers->Set(number, NULL);
        }
        // Done
        number->Release();
        mapping->Release();
    }
};

void FileSystem_ISO9660::PerformTask(IpcEndpoint *destination, bicycle::function<int(Interface_Request*)> generate, bicycle::function<int(Interface_Response*)> response)
{
    // Select identifier
    KernelNumber *number = new KernelNumber(_identifier++);
    while (_tasks->ObjectFor(number) != NULL)
        number->Reset(_identifier++);
    UInt32 identifier = number->Value();
    // Save handler
    FileSystem_ISO9660_Handler *handler = new FileSystem_ISO9660_Handler(response);
    _tasks->Set(number, handler);
    handler->Release();
    number->Release();
    // Generate request
    KernelBufferMemory *request = destination->CreateSendBuffer();
    KernelBufferMemory::Map *mapping = new KernelBufferMemory::Map(NULL, request, false);
    Interface_Request *read = (Interface_Request*)mapping->LinearBase();
    generate(read);
    read->identifier = identifier;
    mapping->Release();
    // Send it
    destination->SendBuffer(request);
    request->Release();
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
    PerformTask(input->Link(), [offset](Interface_Request *request){
        BlockRequestRead *read = (BlockRequestRead*)request;
        read->type = BlockRequest::Read;
        read->offset = SECTOR_SIZE * (0x10 + offset);
        read->length = SECTOR_SIZE;
        return 0;
    }, [this, offset](Interface_Response *response){
        BlockResponseRead *blockResponse = (BlockResponseRead*)response;
        // Check it succeeded
        if (blockResponse->status != Interface_Response::Success)
            return /* TODO: Error state: I/O error */;
        // Check we got at least the required sector
        if (blockResponse->length() < SECTOR_SIZE)
            return /* TODO: Error state: read failure */;
        // Get sector
        GenericVolumeDescriptor *gvd = (GenericVolumeDescriptor*)blockResponse->data();
        // Check magic numbers
        if (gvd->StandardVersion != 1)
            return /* TODO: Error state: invalid filesystem */;
        if ((gvd->StandardId[0] != 'C') || (gvd->StandardId[1] != 'D') || (gvd->StandardId[2] != '0') || (gvd->StandardId[3] != '0') || (gvd->StandardId[4] != '1'))
            return /* TODO: Error state: invalid filesystem */;
        if (gvd->VolumeDescriptorType == VolumeDescriptorType_Terminator)
            return /* TODO: Error state: invalid filesystem */;
        if (gvd->VolumeDescriptorType != VolumeDescriptorType_PrimaryVolume) {
            ReadGVD(offset + 1);
            return;
        }
        // Get actual volume info!
        PrimaryVolumeDescriptor *pvd = (PrimaryVolumeDescriptor*)gvd;
        _rootDirectory = new ISO9660Driver::DirectoryEntry((DirectoryRecord*)&pvd->RootDirectoryRecord);
        _rootDirectory->node = NodeRequest::RootNode;
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
    FileSystem_ISO9660_Handler::Handle(_tasks, message, [](Interface_Response *response){
        // TODO: Non-response message from the service
        return 0;
    });
}

void FileSystem_ISO9660::InputConnectionEnd(InputConnection *connection)
{
    // We're done :(
}

GenericProvider::OutputConnection* FileSystem_ISO9660::OutputConnectionStart(Service *source, IpcEndpoint *connection)
{
    return new OutputConnection(this, source, connection);
}

GenericProvider::InputConnection* FileSystem_ISO9660::Input(void)
{
    return (GenericProvider::InputConnection*)_inputs->ObjectFor(INPUT_NAME);
}

#define LOADED_OK       0
#define LOADED_NOTFOUND 1
#define LOADED_NOTDIR   2
#define LOADED_ERROR    3
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
    PerformTask(Input()->Link(), [entry](Interface_Request *request){
        BlockRequestRead *readRequest = (BlockRequestRead*)request;
        readRequest->type = BlockRequest::Read;
        readRequest->offset = entry->start;
        readRequest->length = entry->length;
        return 0;
    }, [onLoaded, dir](Interface_Response *response){
        BlockResponseRead *readResponse = (BlockResponseRead*)response;
        if (readResponse->status != Interface_Response::Success) {
            onLoaded(LOADED_ERROR);
            return -1;
        }
        dir->Parse(readResponse->data());
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

void FileSystem_ISO9660::OutputConnectionMessage(OutputConnection *connection, KernelBufferMemory *message)
{
    KernelBufferMemory::Map *mapping = new KernelBufferMemory::Map(NULL, message, true);
    Interface_Request *request = (Interface_Request*)mapping->LinearBase();
    
    switch (request->type) {
        case NodeRequest::Search:
        {
            DirectoryRequest *readDirectory = (DirectoryRequest*)request;
            // Find the requested node
            ISO9660Driver::GenericEntry *node = _rootDirectory->FindNode(readDirectory->rootNode);
            if (node == NULL) {
                // Return 'not found'
                connection->Link()->SendMessage([readDirectory](void *context){
                    DirectoryResponse *response = (DirectoryResponse*)context;
                    response->status = NodeResponse::NotFound;
                    response->originalRequest = *readDirectory;
                    response->directoryEntries.Initialise();
                    return true;
                }, 4096);
            } else {
                // Request the directory, if necessary
                ParsePath(node, &readDirectory->subpath, 0, [connection, readDirectory](ISO9660Driver::GenericEntry *foundDirectory){
                    // Check it is a directory
                    if (!foundDirectory->IsDirectory()) {
                        connection->Link()->SendMessage([readDirectory, foundDirectory](void *context){
                            DirectoryResponse *response = (DirectoryResponse*)context;
                            response->status = NodeResponse::NotDirectory;
                            response->originalRequest = *readDirectory;
                            response->directoryEntries.Initialise();
                            return true;
                        }, 4096);
                        return 0;
                    }
                    // Got the required directory
                    connection->Link()->SendMessage([readDirectory, foundDirectory](void *context){
                        DirectoryResponse *response = (DirectoryResponse*)context;
                        response->status = NodeResponse::Success;
                        response->originalRequest = *readDirectory;
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
                    return 0;
                }, [connection, readDirectory](UInt32 failedIndex, UInt32 reason){
                    // Return 'not found' or whatever is approriate for reason :(
                    connection->Link()->SendMessage([readDirectory, reason](void *context){
                        DirectoryResponse *response = (DirectoryResponse*)context;
                        switch(reason) {
                            case LOADED_ERROR:
                                response->status = NodeResponse::IOError;
                                break;
                            case LOADED_NOTDIR:
                                response->status = NodeResponse::NotDirectory;
                                break;
                            case LOADED_NOTFOUND:
                                response->status = NodeResponse::NotFound;
                                break;
                            default:
                                response->status = NodeResponse::Unsupported;
                                break;
                        }
                        response->originalRequest = *readDirectory;
                        response->directoryEntries.Initialise();
                        return true;
                    }, 4096);
                    return 0;
                });
            }
        }
            break;
        default:
            break;
    }
    mapping->Release();
}

void FileSystem_ISO9660::OutputConnectionEnd(OutputConnection *oldConnection)
{
    // Nothing to do
}
