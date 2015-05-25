#include "ImageLoader.h"
#include "Queue.h"
#include "Interface_Block.h"
#include "IPC.h"
#include "Interface_BinaryImage.h"

// ELF base types

typedef UInt32 Elf32_Addr;
typedef UInt16 Elf32_Half;
typedef UInt32 Elf32_Off;
typedef SInt32 Elf32_Sword;
typedef UInt32 Elf32_Word;
typedef UInt8 Elf32_char;

// ELF structures

#define EI_NIDENT   16
typedef struct {
    Elf32_char e_ident[EI_NIDENT];
    Elf32_Half e_type;
#define ET_NONE     0
#define ET_REL      1
#define ET_EXEC     2
#define ET_DYN      3
#define ET_CORE     4
    Elf32_Half e_machine;
#define EM_NONE     0
#define EM_M32      1
#define EM_SPARC    2
#define EM_386      3
#define EM_68K      4
#define EM_88K      5
#define EM_860      7
#define EM_MIPS     8
    Elf32_Word e_version;
#define EV_CURRENT  1
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} __attribute((packed)) Elf32_Ehdr;

// Fields
#define EI_MAG0     0
#define EI_MAG1     1
#define EI_MAG2     2
#define EI_MAG3     3
#define EI_CLASS    4
#define EI_DATA     5
#define EI_VERSION  6
// Values
#define ELFMAG0 0x7F
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'
#define ELFCLASS32      1
#define ELFCLASS64      2
#define ELFDATA2LSB     1
#define ELFDATA2MSB     2

typedef struct {
    Elf32_Word p_type;
#define PT_NULL     0
#define PT_LOAD     1
#define PT_DYNAMIC  2
#define PT_INTERP   3
#define PT_NOTE     4
#define PT_SHLIB    5
#define PT_PHDR     6
#define PT_LOPROC   0x70000000
#define PT_HIPROC   0x7FFFFFFF
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
#define PF_X        0x01
#define PF_W        0x02
#define PF_R        0x04
#define PF_MASKOS   0x0FF00000
#define PF_MASKPROC 0xF0000000
    Elf32_Word p_align;
} Elf32_Phdr;

// Symbol object

class ImageLoader_Symbol : public KernelObject
{
public:
    virtual UInt64 Address(void) = 0;
};

class ImageLoader_Symbol_Main : public ImageLoader_Symbol
{
private:
    UInt64 _main;
public:
    ImageLoader_Symbol_Main(UInt64 main) : _main(main) {}
    
    UInt64 Address(void)
    {
        return _main;
    }
};

// Segment object

class ImageLoader_Segment : public KernelObject
{
private:
    UInt64 _address, _length, _flags, _offset, _fileLength;
public:
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

ImageLoader::ImageLoader()
{
    _tasks = new InterfaceHelper();
    _symbols = new KernelArray();
    _segments = new KernelArray();
}

ImageLoader::~ImageLoader()
{
    _segments->Release();
    _symbols->Release();
    _tasks->Release();
}

UInt32 ImageLoader::InputCount(void)
{
    return 1;
}
KernelDictionary* ImageLoader::Input(UInt32 index)
{
    return NULL;    // TODO
}

#define INPUT_NAME          "file"_ko

GenericProvider::InputConnection* ImageLoader::InputConnectionStart(KernelString *name, IpcEndpoint *connection)
{
    if (!name->IsEqualTo(INPUT_NAME))
        return NULL;
    InputConnection *newConnection = new InputConnection(this, name, connection);
    _queue->AddTask([this, connection](){
        _tasks->PerformTask(connection, [](Interface_Request *request){
            BlockRequestRead *read = (BlockRequestRead*)request;
            read->type = BlockRequest::Read;
            read->offset = 0;
            read->length = sizeof(Elf32_Ehdr);
            return 0;
        }, [this, connection](Interface_Response *response){
            BlockResponseRead *blockResponse = (BlockResponseRead*)response;
            // Check it succeeded
            if (blockResponse->status != Interface_Response::Success)
                return /* TODO: Error state: I/O error */;
            // Check we got at least the required sector
            if (blockResponse->length() < sizeof(Elf32_Ehdr))
                return /* TODO: Error state: read failure */;
            Elf32_Ehdr *header = (Elf32_Ehdr*)blockResponse->data();
            if ((header->e_ident[EI_MAG0] != ELFMAG0) || (header->e_ident[EI_MAG1] != ELFMAG1) || (header->e_ident[EI_MAG2] != ELFMAG2) || (header->e_ident[EI_MAG3] != ELFMAG3)) {
                // TODO: Invalid start
                return 0;
            }
            // TODO: Make this more generic
            if (header->e_ident[EI_CLASS] != ELFCLASS32) {
                // TODO: Not 32-bit
                return 0;
            }
            if (header->e_ident[EI_VERSION] != 1) {
                // TODO: Wrong version
                return 0;
            }
            if (header->e_entry != 0) {
                ImageLoader_Symbol *symbol = new ImageLoader_Symbol_Main(header->e_entry);
                _symbols->Add(symbol);
                symbol->Release();
            }
            // Read the program header
            _tasks->PerformTask(connection, [header](Interface_Request *request){
                BlockRequestRead *read = (BlockRequestRead*)request;
                read->type = BlockRequest::Read;
                read->offset = header->e_phoff;
                read->length = header->e_phnum * header->e_phentsize;
                return 0;
            }, [this, header](Interface_Response *response){
                BlockResponseRead *blockResponse = (BlockResponseRead*)response;
                // Check it succeeded
                if (blockResponse->status != Interface_Response::Success)
                    return /* TODO: Error state: I/O error */0;
                // Read each header
                Elf32_Phdr *programHeader = (Elf32_Phdr*)blockResponse->data();
                for (UInt32 i = 0; i < header->e_phnum; i++) {
                    if (programHeader->p_type & PT_LOAD) {
                        ImageLoader_Segment *segment = new ImageLoader_Segment(programHeader);
                        _segments->Add(segment);
                        segment->Release();
                    }
                    programHeader = (Elf32_Phdr*)(((char*)programHeader) + header->e_phentsize);
                }
                // Done - we can now expose our interface to the outside world!
                IpcService *ipcService = new IpcService("image"_ko, "binaryImage"_ko);
                GenericProvider::Service *service = new GenericProvider::Service(this, ipcService);
                ipcService->Release();
                Launch(service);
                service->Release();
                return 0;
            });
            return 0;
        });
        return 0;
    });
    // Done
    return newConnection;
}

void ImageLoader::InputConnectionReceived(InputConnection *connection, KernelBufferMemory *message)
{
    _tasks->HandleMessage(message, [](Interface_Response *response){
        // TODO: Handle unsolicited message
        return 0;
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
    bicycle::function<int (int error)> onError = [connection, request](int error){
        connection->Link()->SendMessage([request, error](void *context){
            Interface_Response *response = (Interface_Response*)context;
            response->Fill(request);
            response->status = error;
            return true;
        });
        return 0;
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
                bicycle::function<int(void *read, int length)> onRead = [readChunk, connection](void *read, int length){
                    connection->Link()->SendMessage([readChunk, read, length](void *context){
                        Interface_BinaryImage::ReadChunk *response = (Interface_BinaryImage::ReadChunk*)context;
                        response->Fill(readChunk);
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
                    return 0;
                };
                if (segment->FileLength() > readChunk->offset) {
                    mapping->AddRef();  // Needed for onRead
                    // Read file required
                    _tasks->PerformTask(Input()->Link(), [segment, readChunk](Interface_Request *request){
                        BlockRequestRead *read = (BlockRequestRead*)request;
                        read->type = BlockRequest::Read;
                        read->offset = segment->FileOffset() + readChunk->offset;
                        read->length = segment->FileLength();
                        if (read->length > readChunk->length)
                            read->length = readChunk->length;
                        return 0;
                    }, [onRead, onError, mapping](Interface_Response *response){
                        BlockResponseRead *blockResponse = (BlockResponseRead*)response;
                        if (blockResponse->status == BlockResponseRead::Success)
                            onRead(blockResponse->data(), blockResponse->length());
                        else
                            onError(blockResponse->status);
                        mapping->Release();
                        return 0;
                    });
                } else {
                    // It's just empty space
                    onRead(NULL, 0);
                }
            }
        }
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
