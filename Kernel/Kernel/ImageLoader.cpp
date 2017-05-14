#include "ImageLoader.h"
#include "Queue.h"
#include "Interface_Block.h"
#include "IPC.h"
#include "Interface_BinaryImage.h"
#include "Runloop.h"
#include "IPC_Manager.h"
#include "elf.h"

// Symbol object

class ImageLoader_Symbol : public KernelObject
{
public:
    CLASSNAME(KernelObject, ImageLoader_Symbol);
    
    virtual UInt64 Address(void) = 0;
    
    virtual void Encode(FlatDictionary *output) = 0;
};

class ImageLoader_Symbol_Main : public ImageLoader_Symbol
{
private:
    UInt64 _main;
public:
    CLASSNAME(ImageLoader_Symbol, ImageLoader_Symbol_Main);
    
    ImageLoader_Symbol_Main(UInt64 main) : _main(main) {}
    
    UInt64 Address(void)
    {
        return _main;
    }
    
    void Encode(FlatDictionary *output)
    {
        {
            // Save address
            FlatString *key = (FlatString*)output->GetNextAddress();
            key->Initialise(Symbol_Address_Virtual);
            output->CompleteNextItem();
            FlatInteger *value = (FlatInteger*)output->GetNextAddress();
            value->Initialise(_main);
            output->CompleteNextItem();
        }
        
        {
            // Save "launch" flag
            FlatString *key = (FlatString*)output->GetNextAddress();
            key->Initialise(Symbol_Launch);
            output->CompleteNextItem();
            FlatInteger *value = (FlatInteger*)output->GetNextAddress();
            value->Initialise(1);
            output->CompleteNextItem();
        }
    }
};

class ImageLoader_Symbol_EntryPoint : public ImageLoader_Symbol_Main
{
public:
    CLASSNAME(ImageLoader_Symbol_Main, ImageLoader_Symbol_EntryPoint);
    
    ImageLoader_Symbol_EntryPoint(UInt64 address, KernelString *name)
    :ImageLoader_Symbol_Main(address)
    {
        _name = name;
        _name->AddRef();
    }
    
    void Encode(FlatDictionary *output)
    {
        ImageLoader_Symbol_Main::Encode(output);
        {
            // Save name
            FlatString *key = (FlatString*)output->GetNextAddress();
            key->Initialise(Symbol_Name);
            output->CompleteNextItem();
            FlatString *value = (FlatString*)output->GetNextAddress();
            value->Initialise(_name->CString());
            output->CompleteNextItem();
        }
    }
    
protected:
    ~ImageLoader_Symbol_EntryPoint()
    {
        _name->Release();
    }
    
private:
    KernelString *_name;
};

// Segment object

class ImageLoader_Segment : public KernelObject
{
private:
    UInt64 _address, _length, _flags, _offset, _fileLength;
public:
    CLASSNAME(KernelObject, ImageLoader_Segment);
    
    ImageLoader_Segment(Elf32_Phdr *header)
    {
        _address = header->p_vaddr;
        _length = header->p_memsz;
        _flags = header->p_flags;
        _offset = header->p_offset;
        _fileLength = header->p_filesz;
    }
    
    // Loading info
    UInt64 VirtualAddress(void) { return _address; }
    UInt64 VirtualLength(void) { return _length; }
    UInt64 WriteFlags(void) { return _flags; }
    
    // Location in file
    UInt64 FileOffset(void) { return _offset; }
    UInt64 FileLength(void) { return _fileLength; }
};

// Our code

#define INPUT_NAME          "file"_ko

ImageLoader::ImageLoader()
{
    _tasks = new InterfaceHelper();
    _symbols = new KernelArray();
    _segments = new KernelArray();
    _runloop->AddTask([this]{
        GenericProvider::Input *input = new GenericProvider::Input(this, INPUT_NAME);
        _serviceList->AddInput(input);
//        input->Release();
    });
}

ImageLoader::~ImageLoader()
{
    _segments->Release();
    _symbols->Release();
    _tasks->Release();
}

GenericProvider::InputConnection* ImageLoader::InputConnectionStart(KernelString *name, IpcEndpoint *connection)
{
    if (!name->IsEqualTo(INPUT_NAME))
        return NULL;
    InputConnection *newConnection = new InputConnection(this, name, connection);
    _runloop->AddTask([this, connection](){
        _tasks->PerformTask(connection, [](Interface_Request *request){
            BlockRequestRead *read = (BlockRequestRead*)request;
            read->type = BlockRequest::Read;
            read->offset = 0;
            read->length = sizeof(Elf32_Ehdr);
        }, [this, connection](Interface_Response *response){
            BlockResponseRead *blockResponse = (BlockResponseRead*)response;
            // Check it succeeded
            if (blockResponse->status != Interface_Response::Success)
                return /* TODO: Error state: I/O error */;
            // Check we got at least the required sector
            if (blockResponse->length() < sizeof(Elf32_Ehdr)) {
                return /* TODO: Error state: read failure */;
            }
            Elf32_Ehdr *header = (Elf32_Ehdr*)blockResponse->data();
            if ((header->e_ident[EI_MAG0] != ELFMAG0) || (header->e_ident[EI_MAG1] != ELFMAG1) || (header->e_ident[EI_MAG2] != ELFMAG2) || (header->e_ident[EI_MAG3] != ELFMAG3)) {
                // TODO: Invalid start
                return;
            }
            // TODO: Make this more generic
            if (header->e_ident[EI_CLASS] != ELFCLASS32) {
                // TODO: Not 32-bit
                return;
            }
            if (header->e_ident[EI_VERSION] != 1) {
                // TODO: Wrong version
                return;
            }
            if (header->e_entry != 0) {
                ImageLoader_Symbol *symbol = new ImageLoader_Symbol_Main(header->e_entry);
                _symbols->Add(symbol);
                symbol->Release();
            }
            // Read the program header
            Elf32_Ehdr headerCopy = *header;
            _tasks->PerformTask(connection, [header](Interface_Request *request){
                BlockRequestRead *read = (BlockRequestRead*)request;
                read->type = BlockRequest::Read;
                read->offset = header->e_phoff;
                read->length = header->e_phnum * header->e_phentsize;
            }, [this, headerCopy](Interface_Response *response){
                BlockResponseRead *blockResponse = (BlockResponseRead*)response;
                // Check it succeeded
                if (blockResponse->status != Interface_Response::Success)
                    return /* TODO: Error state: I/O error */;
                // Read each header
                Elf32_Phdr *programHeader = (Elf32_Phdr*)blockResponse->data();
                for (UInt32 i = 0; i < headerCopy.e_phnum; i++) {
                    if (programHeader->p_type == PT_LOAD) {
                        ImageLoader_Segment *segment = new ImageLoader_Segment(programHeader);
                        _segments->Add(segment);
                        segment->Release();
                    }
                    programHeader = (Elf32_Phdr*)(((char*)programHeader) + headerCopy.e_phentsize);
                }
                // Done - we can now expose our interface to the outside world!
                IpcService *ipcService = new IpcService("image"_ko, "binaryImage"_ko);
                GenericProvider::Service *service = new GenericProvider::Service(this, ipcService);
                ipcService->Release();
                Launch(service);
                service->Release();
            });
        });
    });
    // Done
    return newConnection;
}

void ImageLoader::InputConnectionReceived(InputConnection *connection, KernelBufferMemory *message)
{
    _tasks->HandleMessage(message, [](Interface_Response *response){
        // TODO: Handle unsolicited message
    });
}

void ImageLoader::InputConnectionEnd(InputConnection *connection)
{
    // We're done :(
    connection->Release();
}

GenericProvider::InputConnection* ImageLoader::Input(void)
{
    return (GenericProvider::InputConnection*)_inputs->ObjectFor(INPUT_NAME);
}

GenericProvider::OutputConnection* ImageLoader::OutputConnectionStart(Service *source, IpcEndpoint *connection)
{
    return new GenericProvider::OutputConnection(this, source, connection);
}

void ImageLoader::OutputConnectionMessage(OutputConnection *connection, KernelBufferMemory *message)
{
    KernelBufferMemory::Map *mapping = new KernelBufferMemory::Map(NULL, message, true);
    Interface_Request *request = (Interface_Request*)mapping->LinearBase();
    bicycle::function<void(int error)> onError = [connection, request](int error){
        connection->Link()->SendMessage([request, error](void *context){
            Interface_Response *response = (Interface_Response*)context;
            response->Fill(request);
            response->status = error;
            return true;
        });
    };
    switch (request->type) {
        case Interface_BinaryImage::Request::GetChunks:
            connection->Link()->SendMessage([request, this](void *context){
                Interface_BinaryImage::GotChunks *chunksResponse = (Interface_BinaryImage::GotChunks*)context;
                chunksResponse->Fill(request);
                chunksResponse->status = Interface_Response::Success;
                chunksResponse->chunkCount = _segments->Count();
                for (UInt32 i = 0; i < chunksResponse->chunkCount; i++) {
                    ImageLoader_Segment *segment = (ImageLoader_Segment*)_segments->ObjectAt(i);
                    Interface_BinaryImage::GotChunks::Chunk *chunk = chunksResponse->chunks + i;
                    chunk->virtualAddress = segment->VirtualAddress();
                    chunk->length = segment->VirtualLength();
                    UInt64 flags = segment->WriteFlags();
                    chunk->read = flags & PF_R;
                    chunk->write = flags & PF_W;
                    chunk->executable = flags & PF_X;
                }
                return true;
            });
            break;
        case Interface_BinaryImage::Request::ReadChunk:
        {
            Interface_BinaryImage::Read *readChunk = (Interface_BinaryImage::Read*)request;
            if (readChunk->chunk >= _segments->Count()) {
                onError(Interface_BinaryImage::Response::InvalidChunk);
            } else {
                ImageLoader_Segment *segment = (ImageLoader_Segment*)_segments->ObjectAt(readChunk->chunk);
                bicycle::function<void(void *read, int length)> onRead = [readChunk, connection](void *read, int length){
                    connection->Link()->SendMessage([readChunk, read, length](void *context){
                        Interface_BinaryImage::ReadChunk *response = (Interface_BinaryImage::ReadChunk*)context;
                        response->Fill(readChunk);
                        response->status = Interface_BinaryImage::ReadChunk::Success;
                        Interface_BinaryImage::ReadChunk::RLE *position = &response->chunkStart;
                        UInt64 copied = 0;
                        // First, install real data, if necessary
                        if ((read != NULL) && (length != 0)) {
                            position->length = length;
                            position->encoding = Interface_BinaryImage::ReadChunk::RLE::eNormal;
                            CopyMemory(position->Data(), read, length);
                            copied += length;
                            position = position->Next();
                        }
                        // Install any remaining bytes
                        if (copied < readChunk->length) {
                            position->length = readChunk->length - copied;
                            position->encoding = Interface_BinaryImage::ReadChunk::RLE::eZero;
                            copied += position->length;
                            position = position->Next();
                        }
                        // Install end
                        position->length = 0;
                        position->encoding = Interface_BinaryImage::ReadChunk::RLE::eEnd;
                        return true;
                    });
                };
                mapping->AddRef();
                if (segment->FileLength() > readChunk->offset) {
                    // Read file required
                    _tasks->PerformTask(Input()->Link(), [segment, readChunk](Interface_Request *request){
                        BlockRequestRead *read = (BlockRequestRead*)request;
                        read->type = BlockRequest::Read;
                        read->offset = segment->FileOffset() + readChunk->offset;
                        read->length = segment->FileLength();
                        if (read->length > readChunk->length)
                            read->length = readChunk->length;
                    }, [onRead, onError, mapping](Interface_Response *response){
                        BlockResponseRead *blockResponse = (BlockResponseRead*)response;
                        if (blockResponse->status == BlockResponseRead::Success)
                            onRead(blockResponse->data(), blockResponse->length());
                        else
                            onError(blockResponse->status);
                        mapping->Release();
                    });
                } else {
                    // It's just empty space
                    onRead(NULL, 0);
                    mapping->Release();
                }
            }
        }
            break;
        case Interface_BinaryImage::Request::GetSymbols:
            connection->Link()->SendMessage([request, this](void *context){
                Interface_BinaryImage::GotSymbols *symbolsResponse = (Interface_BinaryImage::GotSymbols*)context;
                symbolsResponse->Fill(request);
                symbolsResponse->symbols.Initialise();
                _symbols->Enumerate([symbolsResponse](KernelObject *object){
                    ImageLoader_Symbol *symbol = (ImageLoader_Symbol*)object;
                    FlatDictionary *entry = (FlatDictionary*)symbolsResponse->symbols.GetNextAddress();
                    entry->Initialise();
                    symbol->Encode(entry);
                    symbolsResponse->symbols.CompleteNextItem();
                    return (void*)NULL;
                });
                return true;
            });
            break;
        default:
            onError(Interface_Response::Unsupported);
            break;
    }
    mapping->Release();
}

void ImageLoader::OutputConnectionEnd(OutputConnection *oldConnection)
{
    // Client left
    oldConnection->Release();
}
