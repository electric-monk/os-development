#include "Driver_ATA.h"
#include "Blocking.h"
#include "Thread.h"
#include "mem_physical.h"
#include "mem_logical.h"
#include "IPC.h"
#include "Queue.h"
#include "debug.h"
#include "Interrupts.h"
#include "pci.h"
#include "StandardPC.h"
#include "IPC_Manager.h"

#include "Interface_Block.h"

// Internal API based on MINDRVR from http://ata-atapi.com/

#define CB_DH_LBA  0x40    // LBA bit
#define CB_DH_DEV0 0x00    // select device 0
#define CB_DH_DEV1 0x10    // select device 1

#define ATAPI_CMD_EJECT     0x1B
#define ATAPI_CMD_READ      0xA8

#ifdef DEBUG_ATA
#define ATADBG  kprintf
#else
#define ATADBG(f, ...)
#endif

// Register numbers
typedef enum {
    CB_DATA  = 0,   // data reg         in/out cmd_blk_base1+0
    CB_ERR   = 1,   // error            in     cmd_blk_base1+1
    CB_FR    = 1,   // feature reg         out cmd_blk_base1+1
    CB_SC    = 2,   // sector count     in/out cmd_blk_base1+2
    CB_SN    = 3,   // sector number    in/out cmd_blk_base1+3
    CB_CL    = 4,   // cylinder low     in/out cmd_blk_base1+4
    CB_CH    = 5,   // cylinder high    in/out cmd_blk_base1+5
    CB_DH    = 6,   // device head      in/out cmd_blk_base1+6
    CB_STAT  = 7,   // primary status   in     cmd_blk_base1+7
    CB_CMD   = 7,   // command             out cmd_blk_base1+7
    CB_ASTAT = 8,   // alternate status in     ctrl_blk_base2+6
    CB_DC    = 8,   // device control      out ctrl_blk_base2+6
} ATA_Register;
typedef enum {  // CB_ERR
    // ATA
    CB_ER_ICRC = 0x80,    // ATA Ultra DMA bad CRC
    CB_ER_BBK  = 0x80,    // ATA bad block
    CB_ER_UNC  = 0x40,    // ATA uncorrected error
    CB_ER_MC   = 0x20,    // ATA media change
    CB_ER_IDNF = 0x10,    // ATA id not found
    CB_ER_MCR  = 0x08,    // ATA media change request
    CB_ER_ABRT = 0x04,    // ATA command aborted
    CB_ER_NTK0 = 0x02,    // ATA track 0 not found
    CB_ER_NDAM = 0x01,    // ATA address mark not found
    // ATAPI
    CB_ER_P_SNSKEY = 0xf0,   // ATAPI sense key (mask)
    CB_ER_P_MCR    = 0x08,   // ATAPI Media Change Request
    CB_ER_P_ABRT   = 0x04,   // ATAPI command abort
    CB_ER_P_EOM    = 0x02,   // ATAPI End of Media
    CB_ER_P_ILI    = 0x01,   // ATAPI Illegal Length Indication
} ATA_Error;
typedef enum {  // CB_STAT
    CB_STAT_BSY  = 0x80,  // busy
    CB_STAT_RDY  = 0x40,  // ready
    CB_STAT_DF   = 0x20,  // device fault
    CB_STAT_WFT  = 0x20,  // write fault (old name)
    CB_STAT_SKC  = 0x10,  // seek complete (only SEEK command)
    CB_STAT_SERV = 0x10,  // service (overlap/queued commands)
    CB_STAT_DRQ  = 0x08,  // data request
    CB_STAT_CORR = 0x04,  // corrected (obsolete)
    CB_STAT_IDX  = 0x02,  // index (obsolete)
    CB_STAT_ERR  = 0x01,  // error (ATA)
    CB_STAT_CHK  = 0x01,  // check (ATAPI)
} ATA_Status;
typedef enum {  // CB_DC
    CB_DC_HOB   = 0x80,  // High Order Byte (48-bit LBA)
    CB_DC_SRST  = 0x04,  // soft reset
    CB_DC_NIEN  = 0x02,  // disable interrupts
} ATA_DeviceControl;
typedef enum {
    BM_COMMAND_REG    = 0,            // offset to BM command reg
    BM_STATUS_REG     = 2,            // offset to BM status reg
} BM_Register;
typedef enum {
    BM_CR_MASK_READ   = 0x00,           // read from memory
    BM_CR_MASK_WRITE  = 0x08,           // write to memory
    BM_CR_MASK_START  = 0x01,           // start transfer
    BM_CR_MASK_STOP   = 0x00,           // stop transfer
} BM_Command;
typedef enum {
    BM_SR_MASK_SIMPLEX = 0x80,           // simplex only
    BM_SR_MASK_DRV1    = 0x40,           // drive 1 can do dma
    BM_SR_MASK_DRV0    = 0x20,           // drive 0 can do dma
    BM_SR_MASK_INT     = 0x04,           // INTRQ signal asserted
    BM_SR_MASK_ERR     = 0x02,           // error
    BM_SR_MASK_ACT     = 0x01,           // active
} BM_Status;
#define BM_PRD_ADDR_LOW   4            // offset to BM prd addr reg low 16 bits
#define BM_PRD_ADDR_HIGH  6            // offset to BM prd addr reg high 16 bits

static const UInt8 s_devices[] = {CB_DH_DEV0, CB_DH_DEV1};

#define ATA_IO_DELAY    { IOPort()->inByte(CB_ASTAT); IOPort()->inByte(CB_ASTAT); IOPort()->inByte(CB_ASTAT); IOPort()->inByte(CB_ASTAT); }

#define MAX_TRANSFER_SIZE  262144
#define MAX_SEG ((MAX_TRANSFER_SIZE/65536L)+2L)
#define MAX_PRD (MAX_SEG*4L)
#define PRD_BUF_SIZE (48+(2*MAX_PRD*8))
class DMABuffer
{
public:
    DMABuffer()
    {
        // Allocate the required pages
        _allocatedPages = PRD_BUF_SIZE / 4096;
        PhysicalPointer base = CPhysicalMemory::AllocateContiguousPages(_allocatedPages);
        UInt64 start = UInt64(base);
        UInt64 position = (start + 15) & 0xFFFFFFF8;
        if ((position & 0xFFFF0000) != ((position + (MAX_PRD * 8) - 1) & 0xFFFF0000))
            position += (MAX_PRD * 8) & 0xFFFF0000;
        _physicalAddress = PhysicalPointer(position);
        int savedPages = (position - start) / 4096;
        CPhysicalMemory::ReleasePages(base, savedPages);
        _allocatedPages -= savedPages;
        // TODO: The spare pages at the end?
        // Map into memory
        _virtualAddress = rootAddressSpace.Map(fmWritable, pmKernel, _physicalAddress, _allocatedPages);
    }
    
    ~DMABuffer()
    {
        rootAddressSpace.Unmap(_virtualAddress, (size_t)_allocatedPages);
        CPhysicalMemory::ReleasePages(_physicalAddress, _allocatedPages);
    }
    
    PhysicalPointer Physical(void) { return _physicalAddress; }
    void* Virtual(void) { return _virtualAddress; }
private:
    PhysicalPointer _physicalAddress;
    void *_virtualAddress;
    int _allocatedPages;
};

bool ATADriverNode::UpdateForInterrupt(void)
{
    bool dma = true;//DMAAvailable();
    if (dma)
        _bmStatus = readBusMasterStatus();
    bool gotInt = dma && (_bmStatus & BM_SR_MASK_INT);
    if (!dma || gotInt) {
        _interruptStatus = inByte(CB_STAT); // This acknowledges the interrupt for the channel
        if (gotInt)
            writeBusMasterStatus(BM_SR_MASK_INT);
        return true;
    }
    return false;
}

void ATADriverNode::UpdateBMIDEState(void)
{
    _bmStatus = readBusMasterStatus() & 0x60;
}

class ATADriverNode_PCI : public ATADriverNode
{
public:
    CLASSNAME(ATADriverNode, ATADriverNode_PCI);
    
private:
    bool _primary;
    UInt32 _ioPort, _controlPort, _bmPort;
    SimpleSignal *_interrupt;
    InterruptHandlerHandle _interruptToken;
    
    UInt16 GetPort(UInt32 address)
    {
        if (address <= 7)
            return _ioPort + address;
        else if (address == 8)
            return _controlPort;
        else
            return -1;   // Error!
    }

protected:
    ~ATADriverNode_PCI()
    {
        _interrupt->Release();
    }
    
public:
    class Factory : public DriverFactory
    {
    public:
        CLASSNAME(DriverFactory, ATADriverNode_PCI::Factory);
        
    private:
        class Match : public DriverFactory::Match
        {
        private:
            bool _primary;
            
        public:
            CLASSNAME(DriverFactory::Match, ATADriverNode_PCI::Factory::Match);
            
            Match(bool primary)
            {
                _primary = primary;
            }
            int MatchValue(void)
            {
                return 1000000 - (_primary ? 0 : 1);
            }
            Driver* Instantiate(void)
            {
                return new ATADriverNode_PCI(_primary);
            }
            bool MatchMultiple(void)
            {
                return _primary;
            }
        };
    public:
        KernelArray* MatchForParent(Driver *parent)
        {
            KernelString *propertyBus = (KernelString*)parent->PropertyFor(kDriver_Property_Bus);
            if (propertyBus == NULL)
                return NULL;
            if (!propertyBus->IsEqualTo(kDriver_Bus_PCI))
                return NULL;
            KernelNumber *propertyClass = (KernelNumber*)parent->PropertyFor(kDriver_Property_PCI_Class);
            if (propertyClass == NULL)
                return NULL;
            if (propertyClass->Value() != 0x01)
                return NULL;
            KernelNumber *propertySubclass = (KernelNumber*)parent->PropertyFor(kDriver_Property_PCI_Subclass);
            if (propertySubclass == NULL)
                return NULL;
            UInt32 subclass = propertySubclass->Value();
            if ((subclass != 0x01) && (subclass != 0x05) && (subclass != 0x06))
                return NULL;
            KernelArray *output = new KernelArray();
            Match *primary = new Match(true);
            output->Add(primary);
            primary->Release();
            Match *secondary = new Match(false);
            output->Add(secondary);
            secondary->Release();
            output->Autorelease();
            return output;
        }
    };
    
    ATADriverNode_PCI(bool primary)
    :ATADriverNode("PCI ATA device")
    {
        _primary = primary;
        _interrupt = new SimpleSignal(false);
        SetProperty(kDriver_Property_Bus, kDriver_Bus_SystemATA);
    }

    bool Start(Driver *parent)
    {
        ATADBG("%.8x/%s\n", this, __PRETTY_FUNCTION__);
        PCI::Device *parentPCI = (PCI::Device*)parent;
        // Discover the configuration of the device
        UInt32 classInfo = parentPCI->ReadPCIRegister(0x08);
        UInt8 programmingInterface = (classInfo & 0x0000FF00) >> 8;
        bool nativeMode = programmingInterface & (_primary ? 0x01 : 0x04);
        bool configurable = programmingInterface & (_primary ? 0x02 : 0x08);
        // Can it be set to PCI native mode?
        if (configurable && !nativeMode) {
            ATADBG("%.8x/%s: Setting to PCI native mode\n", this, __PRETTY_FUNCTION__);
            nativeMode = true;
            classInfo |= _primary ? 0x01 : 0x04;
            parentPCI->WritePCIRegister(0x08, classInfo);
        }
        // Get the device I/O ports
        int irq = -1;
        _bmPort = parentPCI->ReadBAR(4) + (_primary ? 0 : 8);
        if (!PCI::BAR::IsIOMapped(_bmPort))
            /* Handle error? */;
        _bmPort = PCI::BAR::GetIOAddress(_bmPort);
        if (nativeMode) {
            _ioPort = parentPCI->ReadBAR(_primary ? 0 : 2);
            if (_ioPort == 0) {
                _ioPort = _primary ? 0x1F0 : 0x170;
            } else {
                if (!PCI::BAR::IsIOMapped(_ioPort))
                    /* Handle error? */ kprintf("ERROR: IO is not IO mapped %.8x\n", _ioPort);
                _ioPort = PCI::BAR::GetIOAddress(_ioPort);
            }
            _controlPort = parentPCI->ReadBAR(_primary ? 1 : 3);
            if (_controlPort == 0) {
                _controlPort = _primary ? 0x3F6 : 0x376;
            } else {
                if (!PCI::BAR::IsIOMapped(_controlPort))
                    /* Handle error? */ kprintf("ERROR: control is not IO mapped %.8x\n", _controlPort);
                _controlPort = PCI::BAR::GetIOAddress(_controlPort);
            }
            // Ascertain interrupt
            parentPCI->WritePCIRegister(0x3C, 0xFE);
            if ((parentPCI->ReadPCIRegister(0x3C) & 0xFF) == 0xFE) {
                // IRQ assignment required
                parentPCI->WritePCIRegister(0x3C, 15);
                irq = PIC_IRQ_OFFSET + 15;
            } else {
                UInt8 ProgIF = (parentPCI->ReadPCIRegister(0x08) & 0x0000FF00) >> 8;
                if ((parentPCI->BaseClass() == 0x01) && (parentPCI->SubClass() == 0x01) && ((ProgIF == 0x8A) || (ProgIF == 0x80))) {
                    irq = PIC_IRQ_OFFSET + (_primary ? 14 : 15);
                }
            }
        } else {
            _ioPort = _primary ? 0x1F0 : 0x170;
            _controlPort = _primary ? 0x3F6 : 0x376;
            irq = PIC_IRQ_OFFSET + (_primary ? 14 : 15);
        }
        // Hook up the interrupt
        Interrupts *interruptSource;
        if (irq == -1) {
            // Just use our parent's interrupt source. The interrupt number will be ignored and it'll pick the correct interrupt automatically.
            interruptSource = InterruptSource();
        } else {
            // We need to search for a non-PCI parent to get access to the real system IRQs to support the legacy mode
            Driver *driver = parent;
            do {
                KernelString *bus = (KernelString*)driver->PropertyFor(kDriver_Property_Bus);
                if (bus == NULL)
                    break;
                if (!bus->IsEqualTo(kDriver_Bus_PCI))
                    break;
                driver = driver->Parent();
            } while (driver != NULL);
            if (driver == NULL) {
                // TODO: Serious error
            }
            interruptSource = driver->InterruptSource();
        }
        _interruptToken = interruptSource->RegisterHandler(irq, [this, parentPCI](void *state){
            if (UpdateForInterrupt()) {
                _interrupt->Set();
                return true;
            } else
            return false;
        });
        return ATADriverNode::Start(parent);
    }
    
    void Stop(void)
    {
        InterruptSource()->UnregisterHandler(_interruptToken);
        ATADriverNode::Stop();
    }
    
    UInt8 inByte(UInt32 address)
    {
        return inb(GetPort(address));
    }
    UInt16 inShort(UInt32 address)
    {
        return inw(GetPort(address));
    }
    UInt32 inLong(UInt32 address)
    {
        return inl(GetPort(address));
    }
    void outByte(UInt32 address, UInt8 byte)
    {
        outb(GetPort(address), byte);
    }
    void outShort(UInt32 address, UInt16 byte)
    {
        outw(GetPort(address), byte);
    }
    void outLong(UInt32 address, UInt32 byte)
    {
        outl(GetPort(address), byte);
    }
    
    void inByteRep(UInt32 address, void *buffer, UInt32 length)
    {
        length /= sizeof(UInt8);
        asm volatile("rep insb":"+c"(length), "+D"(buffer):"d"(GetPort(address)));
    }
    void inShortRep(UInt32 address, void *buffer, UInt32 length)
    {
        length /= sizeof(UInt16);
        asm volatile("rep insw":"+c"(length), "+D"(buffer):"d"(GetPort(address)));
    }
    void inLongRep(UInt32 address, void *buffer, UInt32 length)
    {
//        asm("rep insd"::"c"(length / sizeof(UInt32)), "d"(GetPort(address)), "D"(buffer));
        inShortRep(address, buffer, length);
    }
    void outByteRep(UInt32 address, void *buffer, UInt32 length)
    {
        length /= sizeof(UInt8);
        asm volatile("rep outsb":"+c"(length), "+S"(buffer):"d"(GetPort(address)));
    }
    void outShortRep(UInt32 address, void *buffer, UInt32 length)
    {
        length /= sizeof(UInt16);
        asm volatile("rep outsw":"+c"(length), "+S"(buffer):"d"(GetPort(address)));
    }
    void outLongRep(UInt32 address, void *buffer, UInt32 length)
    {
//        asm("rep outsd"::"c"(length / sizeof(UInt32)), "d"(GetPort(address)), "S"(buffer));
        outShortRep(address, buffer, length);
    }
    
    BlockableObject* Interrupt(void)
    {
        return _interrupt;
    }
    void ResetInterrupt(void)
    {
        _interrupt->Reset();
    }
    
    bool DMAAvailable(void)
    {
        return _bmPort != 0;
    }
    UInt8 readBusMasterStatus(void)
    {
        UInt8 b = inb(_bmPort + 0x02);
        return b;
    }
    void writeBusMasterStatus(UInt8 x)
    {
        outb(_bmPort + 0x02, x);
    }
};

class ATADriverDrive : public KernelObject
{
public:
    CLASSNAME(KernelObject, ATADriverDrive);
    
private:
    class GenericHandler : public SignalWatcher
    {
    public:
        CLASSNAME(SignalWatcher, ATADriverDrive::GenericHandler);
        
        GenericHandler(ATADriverDrive *owner)
        {
            _owner = owner;
        }
        
    protected:
        ATADriverDrive *_owner;
    };
    class ConnectionHandler : public GenericHandler
    {
    public:
        CLASSNAME(GenericHandler, ATADriverDrive::ConnectionHandler);
        
        ConnectionHandler(ATADriverDrive *owner) : GenericHandler(owner) { }
        
        void SignalChanged(BlockableObject *watching, bool active)
        {
            if (active)
                _owner->IncomingConnection();
        }
    };
    class RequestHandler : public GenericHandler
    {
    public:
        CLASSNAME(GenericHandler, ATADriverDrive::RequestHandler);
        
        RequestHandler(ATADriverDrive *owner, IpcEndpoint *sender)
        :GenericHandler(owner)
        {
            _sender = sender;
        }
        
        void SignalChanged(BlockableObject *watching, bool active)
        {
            if (active)
                _owner->ReceivedRequest(_sender);
        }
        
    protected:
        IpcEndpoint *_sender;
    };
    class Request : public DispatchQueue::Task
    {
    public:
        CLASSNAME(DispatchQueue::Task, ATADriverDrive::Request);
        
        Request(ATADriverDrive *owner, IpcEndpoint *sender, KernelBufferMemory *request)
        {
            _owner = owner;
            _endpoint = sender;
            _endpoint->AddRef();
            _request = request;
            _request->AddRef();
        }
        
    protected:
        ~Request()
        {
            _request->Release();
            _endpoint->Release();
        }
        
        void Execute(void)
        {
            KernelBufferMemory::Map *mapping = new KernelBufferMemory::Map(NULL, _request, true);
            _owner->ProcessRequest(_endpoint, (BlockRequest*)mapping->LinearBase(), _request->Size());
            mapping->Release();
        }
        
    private:
        ATADriverDrive *_owner;
        IpcEndpoint *_endpoint;
        KernelBufferMemory *_request;
    };
public:
    ATADriverDrive(ATADriver *driver, const char *mode, int index, DispatchQueue *queue)
    {
        _driver = driver;
        _driverIndex = index;
        _queue = queue;
        _service = new IpcService(KernelString::Format("%s%i", mode, index), SERVICE_TYPE_BLOCK);
        ConnectionHandler *handler = new ConnectionHandler(this);
        _service->RegisterObserver(handler);
        handler->Release();
    }
    
    void IncomingConnection(void)
    {
        IpcEndpoint *newConnection = _service->NextConnection(false);
        RequestHandler *handler = new RequestHandler(this, newConnection);
        newConnection->RegisterObserver(handler);
        handler->Release();
        // Retain until disconnected
        newConnection->AddRef();
    }
    
    void ReceivedRequest(IpcEndpoint *connection)
    {
        KernelBufferMemory *data = connection->Read(false);
        if (data == NULL) {
            // If it's signalled and empty, it disconnected
            connection->Release();
        } else {
            // A new request
            Request *requestTask = new Request(this, connection, data);
            _queue->AddTask(requestTask);
            requestTask->Release();
        }
    }
    
    virtual void ProcessRequest(IpcEndpoint *endpoint, BlockRequest *request, UInt64 length) = 0;
    
    IpcService *_service;
    
protected:
    ~ATADriverDrive()
    {
        _service->Release();
    }
    
    ATADriver *_driver;
    UInt32 _driverIndex;
    
    UInt16 _sectorSize;
    
private:
    DispatchQueue *_queue;
    
protected:
    class ResponseHelper
    {
    public:
        ResponseHelper(IpcEndpoint *endpoint, BlockRequest *request, UInt64 maximumLength = 65536)
        {
            _endpoint = endpoint;
            _response = _endpoint->CreateSendBuffer();
            _mapping = new KernelBufferMemory::Map(NULL, _response, false);
            
            Response()->status = BlockResponse::Unsupported;
            Response()->Fill(request);
        }
        
        ~ResponseHelper()
        {
            _mapping->Release();
            _endpoint->SendBuffer(_response);
            _response->Release();
        }
        
        BlockResponse* Response(void)
        {
            return (BlockResponse*)_mapping->LinearBase();
        }
        
    private:
        IpcEndpoint *_endpoint;
        KernelBufferMemory *_response;
        KernelBufferMemory::Map *_mapping;
    };
};

class ATADriverDrive_ATA : public ATADriverDrive
{
public:
    CLASSNAME(ATADriverDrive, ATADriverDrive_ATA);
    
    ATADriverDrive_ATA(ATADriver *driver, int index, DispatchQueue *queue)
    :ATADriverDrive(driver, "ata", index, queue)
    {
        _sectorSize = 512;  // Should query this
    }
    
    void ProcessRequest(IpcEndpoint *endpoint, BlockRequest *request, UInt64 length)
    {
        ResponseHelper response(endpoint, request);
        switch(request->type) {
            case BlockRequest::Read:
            case BlockRequest::Write:
            default:
                // Default response is "unsupported"
                kprintf("Unsupported ATA command %i\n", request->type);
                break;
        }
    }
};

class ATADriverDrive_ATAPI : public ATADriverDrive
{
public:
    CLASSNAME(ATADriverDrive, ATADriverDrive_ATAPI);
    
    ATADriverDrive_ATAPI(ATADriver *driver, int index, DispatchQueue *queue)
    :ATADriverDrive(driver, "atapi", index, queue)
    {
        _sectorSize = 2048; // Should query this
    }
    
    void ProcessRequest(IpcEndpoint *endpoint, BlockRequest *request, UInt64 length)
    {
        ResponseHelper response(endpoint, request);
        switch(request->type) {
            case BlockRequest::Read:
            {
                BlockRequestRead *readRequest = (BlockRequestRead*)request;
                BlockResponseRead *readResponse = (BlockResponseRead*)response.Response();
                readResponse->requestedOffset = readRequest->offset;
                readResponse->requestedLength = readRequest->length;
                readResponse->readOffset = readRequest->offset - (readRequest->offset % _sectorSize);
                readResponse->readLength = readRequest->length + (readRequest->offset - readResponse->readOffset);
                if  (readResponse->readLength > (255 * _sectorSize)) {
                    readResponse->readLength = 255 * _sectorSize;
                } else {
                    UInt64 remainder = readResponse->readLength % _sectorSize;
                    if (remainder != 0)
                        readResponse->readLength += _sectorSize - remainder;
                }
                UInt32 sectorOffset = readResponse->readOffset / _sectorSize;
                UInt32 sectorLength = readResponse->readLength / _sectorSize;
                unsigned char packet[] = {
                    ATAPI_CMD_READ, 0x00,
                    static_cast<unsigned char>((sectorOffset >> 24) & 0xFF), static_cast<unsigned char>((sectorOffset >> 16) & 0xFF), static_cast<unsigned char>((sectorOffset >> 8) & 0xFF), static_cast<unsigned char>(sectorOffset & 0xFF),
                    0x00, 0x00, 0x00,
                    static_cast<unsigned char>(sectorLength),
                    0x00, 0x00
                };
                bool status = _driver->Packet(_driverIndex, (char*)packet, sizeof(packet), false, readResponse->rawData(), readResponse->readLength);
                UInt32 result;  // Can't seem to use ?: here, gcc will leave references to symbol in, and it won't link
                if (status)
                    result = BlockResponse::Success;
                else
                    result = BlockResponse::GeneralFailure;
                response.Response()->status = result;
                break;
            }
            case BlockRequest::Eject:
            {
                char packet[] = {ATAPI_CMD_EJECT, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
                bool status = _driver->Packet(_driverIndex, packet, sizeof(packet), true, NULL, 0);
                UInt32 result;  // Can't seem to use ?: here, gcc will leave references to symbol in, and it won't link
                if (status)
                    result = BlockResponse::Success;
                else
                    result = BlockResponse::GeneralFailure;
                response.Response()->status = result;
                break;
            }
            default:
                kprintf("Unsupported ATAPI command %i\n", request->type);
                // Default response is "unsupported"
                break;
        }
    }
};

class ATADriver_Factory : public DriverFactory
{
private:
    class ATADriver_Match : public DriverFactory::Match
    {
    public:
        CLASSNAME(DriverFactory::Match, ATADriver_Match);
        
        int MatchValue(void)
        {
            return 1000;
        }
        Driver* Instantiate(void)
        {
            return new ATADriver();
        }
    };
public:
    CLASSNAME(DriverFactory, ATADriver_Factory);
    
    KernelArray* MatchForParent(Driver *parent)
    {
        KernelString *propertyBus = (KernelString*)parent->PropertyFor(kDriver_Property_Bus);
        if (propertyBus == NULL)
            return NULL;
        if (!propertyBus->IsEqualTo(kDriver_Bus_SystemATA))
            return NULL;
        KernelArray *result = new KernelArray();
        ATADriver_Match *match = new ATADriver_Match();
        result->Add(match);
        match->Release();
        result->Autorelease();
        return result;
    }
};

static ATADriverDrive* Instantiate(ATADriver *driver, IpcServiceProxy *service, DispatchQueue *queue, int index, ATADriver::DEV_CONFIG type)
{
    ATADriverDrive *result;
    
    switch (type) {
        default:
            result = NULL;
            break;
        case ATADriver::dcPATA:
        case ATADriver::dcSATA:
            result = new ATADriverDrive_ATA(driver, index, queue);
            break;
        case ATADriver::dcPATAPI:
        case ATADriver::dcSATAPI:
            result = new ATADriverDrive_ATAPI(driver, index, queue);
            break;
    }
    if (result != NULL) {
        ATADBG("%.8x/%s: type %i: %.8x\n", driver, __PRETTY_FUNCTION__, type, result);
        service->AddOutput(result->_service);
    }
    return result;
}

class ConfigureATA : public DispatchQueue::Task
{
public:
    CLASSNAME(DispatchQueue::Task, ConfigureATA);
    
    ConfigureATA(ATADriver *driver, bool useDMA)
    {
        _owner = driver;
        _owner->AddRef();
        _useDMA = useDMA;
    }
    
protected:
    ~ConfigureATA()
    {
        _owner->Release();
    }
    
    void Execute(void)
    {
        int count = _owner->Configure();
        if (_useDMA) {
            // Allocate DMA buffer
            _owner->_dmaBuffer = new DMABuffer();
            // Read status register
            _owner->IOPort()->UpdateBMIDEState();
        } else {
            _owner->_dmaBuffer = NULL;
        }
        // Create nubs for child devices
        for (int i = 0; i < 2; i++)
            if (_owner->_configInfo[i] != ATADriver::dcNone)
                _owner->_driveHandlers[i] = Instantiate(_owner, _owner->_serviceList, _owner->_mainQueue, i, _owner->_configInfo[i]);
    }
    
private:
    ATADriver *_owner;
    bool _useDMA;
};

ATADriver::ATADriver()
:Driver("ATA(PI) driver")
{
    _serviceList = new IpcServiceProxy(this);
    // Create waiting stuff
    _timer = new Timer();
    _waitObject = new SignalOr();
    _waitObject->AddSource(_timer);
    // etc.
    _pioTransferWidth = 16;
}

ATADriver::~ATADriver()
{
    _serviceList->Release();
    _waitObject->Release();
    _timer->Release();
}

void ATADriver::Install(void)
{
    ATADriver_Factory *factory = new ATADriver_Factory();
    Driver::RegisterFactory(factory);
    factory->Release();
    ATADriverNode_PCI::Factory *pciFactory = new ATADriverNode_PCI::Factory();
    Driver::RegisterFactory(pciFactory);
    pciFactory->Release();
}

bool ATADriver::Start(Driver *parent)
{
    _serviceList->Start();
    // Check setup
    BlockableObject *interrupt = IOPort()->Interrupt();
    _useInterrupts = interrupt != NULL;
    if (_useInterrupts)
        _waitObject->AddSource(interrupt);
    bool useDMA = false;//_useInterrupts && IOPort()->DMAAvailable();
    // Launch thread
    _mainQueue = new DispatchQueue();
    // Start configure task
    ConfigureATA *configureTask = new ConfigureATA(this, useDMA);
    _mainQueue->AddTask(configureTask);
    configureTask->Release();
    // Configure
    return Driver::Start(parent);
}

void ATADriver::Stop(void)
{
    // Remove child nubs
    for (int i = 0; i < 2; i++) {
        if (_driveHandlers[i] == NULL)
            continue;
        _serviceList->RemoveOutput(_driveHandlers[i]->_service);
        _driveHandlers[i]->Release();
        _driveHandlers[i] = NULL;
    }
    // Stop thread
    _mainQueue->Release();
    // Release memory
    if (_dmaBuffer != NULL)
        delete _dmaBuffer;
    // Clean up setup
    if (_useInterrupts)
        _waitObject->RemoveSource(IOPort()->Interrupt());
    Driver::Stop();
    _serviceList->Stop();
}

void ATADriver::StartTimer(void)
{
    IOPort()->ResetInterrupt();
    _timer->Reset(SECONDS(20));
}

// Fills in _configInfo, returns devices found
int ATADriver::Configure(void)
{
    ATADBG("%.8x/%s\n", this, __PRETTY_FUNCTION__);

    // Reset Bus Master Error bit
    IOPort()->writeBusMasterStatus(BM_SR_MASK_ERR);
    
    // Assume there are no devices
    _configInfo[0] = dcNone;
    _configInfo[1] = dcNone;
    
    // Set up Device Control register
    IOPort()->outByte(CB_DC, _useInterrupts ? 0 : CB_DC_NIEN);
    
    // Look for devices
    if (_Configure_InitDevice(CB_DH_DEV0))
        _configInfo[0] = dcUnknown;
    if ((_configInfo[0] != dcNone) && _Configure_InitDevice(CB_DH_DEV1))
        _configInfo[1] = dcUnknown;
    
    // Soft reset
    IOPort()->outByte(CB_DH, CB_DH_DEV0);
    ATA_IO_DELAY;
    Reset(0);
    
    // Check devices again
    if (_configInfo[0] != dcNone)
        _configInfo[0] = _Configure_CheckDevice(CB_DH_DEV0);
    if (_configInfo[1] != dcNone)
        _configInfo[1] = _Configure_CheckDevice(CB_DH_DEV1);
    
    // Select a device
    int count = 0, index = 1;
    do {
        if (_configInfo[index] != dcNone) {
            IOPort()->outByte(CB_DH, s_devices[index]);
            ATA_IO_DELAY;
            count++;
        }
        index--;
    } while (index >= 0);
    
    // BMIDE Error=1?
    if (IOPort()->readBusMasterStatus() & BM_SR_MASK_ERR)
        ;//
    ATADBG("%.8x/%s found %i devices\n", this, __PRETTY_FUNCTION__, count);
    return count;
}

bool ATADriver::_Configure_InitDevice(UInt8 device)
{
    IOPort()->outByte(CB_DH, device);
    ATA_IO_DELAY;
    IOPort()->outByte(CB_SC, 0x55);
    IOPort()->outByte(CB_SN, 0xAA);
    IOPort()->outByte(CB_SC, 0xAA);
    IOPort()->outByte(CB_SN, 0x55);
    IOPort()->outByte(CB_SC, 0x55);
    IOPort()->outByte(CB_SN, 0xAA);
    UInt8 sc = IOPort()->inByte(CB_SC);
    UInt8 sn = IOPort()->inByte(CB_SN);
    return (sc == 0x55) && (sn == 0xAA);
}

ATADriver::DEV_CONFIG ATADriver::_Configure_CheckDevice(UInt8 device)
{
    IOPort()->outByte(CB_DH, device);
    ATA_IO_DELAY;
    UInt8 sc = IOPort()->inByte(CB_SC);
    UInt8 sn = IOPort()->inByte(CB_SN);
    if ((sc == 0x01) && (sn == 0x01)) {
        UInt8 st = IOPort()->inByte(CB_STAT);
        UInt8 cl = IOPort()->inByte(CB_CL);
        UInt8 ch = IOPort()->inByte(CB_CH);
        if ((cl == 0x14) && (ch == 0xEB))
            return dcPATAPI;
        else if ((cl == 0x69) && (ch == 0x96))
            return dcSATAPI;
        else if (st != 0) {
            if ((cl == 0x00) && (ch == 0x00))
                return dcPATA;
            else if ((cl == 0x3C) && (ch == 0xC3))
                return dcSATA;
        }
    }
    return dcUnknown;
}

bool ATADriver::Reset(UInt32 deviceReturn)
{
    _regCommandInfo.errorCode = 0;
    
    // Reset Bus Master Error bit
    IOPort()->writeBusMasterStatus(BM_SR_MASK_ERR | BM_SR_MASK_INT);
    
    // Initiaise command timeout
    StartTimer();
    
    // Set and then reset soft reset bit
    UInt8 dc = _useInterrupts ? 0 : CB_DC_NIEN;
    IOPort()->outByte(CB_DC, CB_DC_SRST | dc);
    ATA_IO_DELAY;
    IOPort()->outByte(CB_DC, dc);
    ATA_IO_DELAY;
    
    IOPort()->readBusMasterStatus();
    // If there is a device 0, wait for it
    if (_configInfo[0] != dcNone) {
        while (true) {
            UInt8 status = IOPort()->inByte(CB_STAT);
            if (!(status & CB_STAT_BSY))
                break;
            if (_timer->IsSignalled()) {
                _regCommandInfo.timeOutError = true;
                _regCommandInfo.errorCode = 1;
                break;
            }
        }
    }
    
    IOPort()->readBusMasterStatus();
    // If there is a device 1, wait for it
    if (_configInfo[1] != dcNone) {
        while (true) {
            IOPort()->outByte(CB_DH, CB_DH_DEV1);
            ATA_IO_DELAY;
            UInt8 sc = IOPort()->inByte(CB_SC);
            UInt8 sn = IOPort()->inByte(CB_SN);
            if ((sc == 0x01) && (sn == 0x01))
                break;
            if (_timer->IsSignalled()) {
                _regCommandInfo.timeOutError = true;
                _regCommandInfo.errorCode = 2;
                break;
            }
        }
        
        // Now check if it's not busy
        if (_regCommandInfo.errorCode == 0) {
            if (IOPort()->inByte(CB_STAT) & CB_STAT_BSY)
                _regCommandInfo.errorCode = 3;
        }
    }
    
    // Select requested device
    if (_configInfo[deviceReturn] == dcNone)
        deviceReturn = (deviceReturn + 1) % 2;  // Hope there's at least one device
    IOPort()->outByte(CB_DH, s_devices[deviceReturn]);
    ATA_IO_DELAY;
    
    // BMIDE Error=1?
    if (IOPort()->readBusMasterStatus() & BM_SR_MASK_ERR)
        _regCommandInfo.errorCode = 78;
    
    // Done
    return _CompleteCommand();
}

void ATADriver::_SetupCommand(void)
{
    // Output devctrl
    IOPort()->outByte(CB_DC, _regCommandInfo.devControl);
    
    // Output command parameters
    UInt8 dh = (_regCommandInfo.devHead & 0xF0) | ((_regCommandInfo.lbaLow >> 24) & 0x0F);
    switch (_regCommandInfo.lbaSize) {
        case lba48:
            IOPort()->outByte(CB_FR, _regCommandInfo.feature >> 8);
            IOPort()->outByte(CB_SC, _regCommandInfo.secCount >> 8);
            IOPort()->outByte(CB_SN, _regCommandInfo.lbaLow >> 24);
            IOPort()->outByte(CB_CL, _regCommandInfo.lbaHigh);
            IOPort()->outByte(CB_CH, _regCommandInfo.lbaHigh >> 8);
            dh = _regCommandInfo.devHead;
            // Fall through
        case lba28:
            IOPort()->outByte(CB_FR, _regCommandInfo.feature);
            IOPort()->outByte(CB_SC, _regCommandInfo.secCount);
            IOPort()->outByte(CB_SN, _regCommandInfo.lbaLow);
            IOPort()->outByte(CB_CL, _regCommandInfo.lbaLow >> 8);
            IOPort()->outByte(CB_CH, _regCommandInfo.lbaLow >> 16);
            break;
        default:
            IOPort()->outByte(CB_FR, _regCommandInfo.feature);
            IOPort()->outByte(CB_SC, _regCommandInfo.secCount);
            IOPort()->outByte(CB_SN, _regCommandInfo.secNum);
            IOPort()->outByte(CB_CL, _regCommandInfo.cylLow);
            IOPort()->outByte(CB_CH, _regCommandInfo.cylHigh);
            dh = _regCommandInfo.devHead;
            break;
    }
    IOPort()->outByte(CB_DH, dh);
}

void ATADriver::_Wait(UInt8 we, UInt8 pe)
{
    if (we && _useInterrupts) {
        KernelArray *result = Thread::Active->BlockOn(_waitObject);
        if (result) {
            if (result->Contains(_timer)) {
                _regCommandInfo.timeOutError = true;
                _regCommandInfo.errorCode = we;
            } else {
                IOPort()->ResetInterrupt();
            }
        }
    } else {
        while (true) {
            UInt8 status = IOPort()->inByte(CB_STAT);
            if (!(status & CB_STAT_BSY))
                break;
            if (_timer->IsSignalled()) {
                _regCommandInfo.timeOutError = true;
                _regCommandInfo.errorCode = pe;
                break;
            }
        }
    }
}

bool ATADriver::_CompleteCommand(void)
{
    _regCommandInfo.regStatus.raw = IOPort()->inByte(CB_STAT);
    _regCommandInfo.regAltStatus.raw = IOPort()->inByte(CB_ASTAT);
    _regCommandInfo.regError.raw = IOPort()->inByte(CB_ERR);
    if (_regCommandInfo.errorCode != 0)
        kprintf("ATADriver::Generic command Error %i\n", _regCommandInfo.errorCode);
    return _regCommandInfo.errorCode == 0;
}

bool ATADriver::_Select(UInt8 device)
{
    if (_configInfo[device] == dcNone) {
        IOPort()->outByte(CB_DH, s_devices[device]);
        ATA_IO_DELAY;
        return true;
    }
    while (true) {
        UInt8 status = IOPort()->inByte(CB_STAT);
        if (!(status & (CB_STAT_BSY | CB_STAT_DRQ)))
            break;
        if (_timer->IsSignalled()) {
            _regCommandInfo.timeOutError = true;
            _regCommandInfo.errorCode = 11;
            _regCommandInfo.regStatus.raw = status;
            _regCommandInfo.regAltStatus.raw = IOPort()->inByte(CB_ASTAT);
            _regCommandInfo.regError.raw = IOPort()->inByte(CB_ERR);
            return false;
        }
    }
    IOPort()->outByte(CB_DH, s_devices[device]);
    ATA_IO_DELAY;
    while (true) {
        UInt8 status = IOPort()->inByte(CB_STAT);
        if (!(status & (CB_STAT_BSY | CB_STAT_DRQ)))
            break;
        if (_timer->IsSignalled()) {
            _regCommandInfo.timeOutError = true;
            _regCommandInfo.errorCode = 12;
            _regCommandInfo.regStatus.raw = status;
            _regCommandInfo.regAltStatus.raw = IOPort()->inByte(CB_ASTAT);
            _regCommandInfo.regError.raw = IOPort()->inByte(CB_ERR);
            return false;
        }
    }
    if (_regCommandInfo.errorCode != 0)
        kprintf("ATADriver::Select(%i) Error %i\n", device, _regCommandInfo.errorCode);
    return _regCommandInfo.errorCode == 0;
}

bool ATADriver::_ExecNonDataCommand(UInt32 device)
{
    // Reset Bus Master Error bit
    IOPort()->writeBusMasterStatus(BM_SR_MASK_ERR);
    
    // Set command timeout
    StartTimer();

    // Avoid doing things for device reset
    if (_regCommandInfo.command != cmdDEVICE_RESET) {
        if (!_Select(device))
            return false;
        _SetupCommand();
    }
    
    // Send the command
    IOPort()->outByte(CB_CMD, _regCommandInfo.command);
    ATA_IO_DELAY;
    
    // Wait for result
    bool polled = false;
    if ((_regCommandInfo.command = cmdEXECUTE_DEVICE_DIAGNOSTIC) && (_configInfo[0] == dcNone)) {
        polled = true;
        while (true) {
            IOPort()->outByte(CB_DH, CB_DH_DEV1);
            ATA_IO_DELAY;
            UInt8 secCount = IOPort()->inByte(CB_SC);
            UInt8 secNum = IOPort()->inByte(CB_SN);
            if ((secCount == 0x01) && (secNum == 0x01))
                break;
            if (_timer->IsSignalled()) {
                _regCommandInfo.timeOutError = true;
                _regCommandInfo.errorCode = 24;
                break;
            }
        }
    } else {
        if (_regCommandInfo.command == cmdDEVICE_RESET) {
            polled = true;
            _Wait(0, 23);
        } else {
            if (!_useInterrupts)
                polled = true;
            _Wait(22, 23);
        }
    }
    
    // If polled or error, read register
    UInt8 status = (polled || _regCommandInfo.errorCode) ? IOPort()->inByte(CB_STAT) : IOPort()->InterruptStatus();
    
    // Check error
    if (_regCommandInfo.errorCode == 0) {
        if (status & (CB_STAT_BSY | CB_STAT_DF | CB_STAT_DRQ | CB_STAT_ERR))
            _regCommandInfo.errorCode = 21;
    }
    
    // BMIDE Error=1?
    if (IOPort()->readBusMasterStatus() & BM_SR_MASK_ERR)
        _regCommandInfo.errorCode = 78;
    
    // Done
    return _CompleteCommand();
}

bool ATADriver::NonDataCommandLBA28(UInt8 dev, COMMAND command, UInt32 sector, UInt32 lba)
{
    _regCommandInfo.command = command;
    _regCommandInfo.feature = 0;
    _regCommandInfo.secCount = sector;
    _regCommandInfo.devHead = CB_DH_LBA | s_devices[dev];
    _regCommandInfo.devControl = _useInterrupts ? 0 : CB_DC_NIEN;
    _regCommandInfo.sectorCount = sector;
    _regCommandInfo.lbaSize = lba28;
    _regCommandInfo.lbaHigh = 0;
    _regCommandInfo.lbaLow = lba;
    return _ExecNonDataCommand(dev);
}

bool ATADriver::NonDataCommandLBA48(UInt8 dev, COMMAND command, UInt32 sector, UInt64 lba)
{
    _regCommandInfo.command = command;
    _regCommandInfo.feature = 0;
    _regCommandInfo.secCount = sector;
    _regCommandInfo.devHead = CB_DH_LBA | s_devices[dev];
    _regCommandInfo.devControl = _useInterrupts ? 0 : CB_DC_NIEN;
    _regCommandInfo.sectorCount = sector;
    _regCommandInfo.lbaSize = lba48;
    _regCommandInfo.lbaHigh = lba >> 32;
    _regCommandInfo.lbaLow = lba;
    return _ExecNonDataCommand(dev);
}

bool ATADriver::_ExecDataCommandIn(UInt32 device, UInt8 *buffer, UInt32 numSectors, UInt32 multiCnt)
{
    if (multiCnt == 0)
        multiCnt = 1;
    
    // Reset Bus Master Error bit
    IOPort()->writeBusMasterStatus(BM_SR_MASK_ERR);
    
    // Set timeout
    StartTimer();
    
    // Select drive
    if (!_Select(device))
        return false;
    
    // Setup
    _SetupCommand();
    
    // Start command
    IOPort()->outByte(CB_CMD, _regCommandInfo.command);
    ATA_IO_DELAY;
    
    // Loop to read each sector
    while (true) {
        // wait for read
        _Wait(34, 35);
        UInt8 status = (!_useInterrupts || _regCommandInfo.errorCode) ? IOPort()->inByte(CB_STAT) : IOPort()->InterruptStatus();
        if (_regCommandInfo.errorCode)
            break;
        
        // If BSY=0 and DRQ=1, transfer data
        if ((status & (CB_STAT_BSY | CB_STAT_DRQ)) == CB_STAT_DRQ) {
            // Increment packet count
            _regCommandInfo.drqPackets++;
            
            // Determine the number of sectors to transfer
            UInt32 wordCount = multiCnt;
            if (wordCount > numSectors)
                wordCount = numSectors;
            wordCount *= 256;
            
            // Do REP INSW to read for one DRQ block
            _regCommandInfo.totalBytes += wordCount << 1;
            _drqBlockIn(CB_DATA, buffer, wordCount);
            ATA_IO_DELAY;
            
            // Decrement counters
            numSectors -= multiCnt;
            buffer += 512 * multiCnt;
        }
        
        // Error?
        if (!(status & CB_STAT_DRQ)) {
            _regCommandInfo.errorCode = 32;
            break;
        }
        
        // Final checks
        if (numSectors == 0) {
            // Check for error
            status = IOPort()->inByte(CB_STAT);
            if (status & (CB_STAT_BSY | CB_STAT_DF | CB_STAT_DRQ | CB_STAT_ERR)) {
                _regCommandInfo.errorCode = 33;
                break;
            }
            break;
        }
    }
    
    // BMIDE Error=1?
    if (IOPort()->readBusMasterStatus() & BM_SR_MASK_ERR)
        _regCommandInfo.errorCode = 78;
    if (_regCommandInfo.errorCode != 0)
        kprintf("ATADriver::_ExecDataCommandIn Error %i\n", _regCommandInfo.errorCode);
    return _regCommandInfo.errorCode == 0;
}

bool ATADriver::DataCommandLBA28In(UInt8 device, COMMAND command, UInt32 lba, void *buffer, UInt32 numSectors, UInt32 multiCnt)
{
    _regCommandInfo.command = command;
    _regCommandInfo.feature = 0;
    _regCommandInfo.secCount = numSectors;
    _regCommandInfo.devHead = CB_DH_LBA | s_devices[device];
    _regCommandInfo.devControl = _useInterrupts ? 0 : CB_DC_NIEN;
    _regCommandInfo.lbaSize = lba28;
    _regCommandInfo.lbaHigh = 0;
    _regCommandInfo.lbaLow = lba;
    
    if ((command == cmdIDENTIFY_DEVICE) || (command == cmdIDENTIFY_DEVICE_PACKET))
        numSectors = 1;
    
    if (multiCnt & 0x0800) {
        // Some sort of shenanigan
        multiCnt &= 0x00FF;
    } else {
        if (command != cmdREAD_MULTIPLE)
            multiCnt = 1;
    }
    
    _regCommandInfo.sectorCount = numSectors;
    _regCommandInfo.currentMultipleBlock = multiCnt;
    
    return _ExecDataCommandIn(device, (UInt8*)buffer, numSectors, multiCnt);
}

bool ATADriver::DataCommandLBA48In(UInt8 device, COMMAND command, UInt64 lba, void *buffer, UInt32 numSectors, UInt32 multiCnt)
{
    _regCommandInfo.command = command;
    _regCommandInfo.feature = 0;
    _regCommandInfo.secCount = numSectors;
    _regCommandInfo.devHead = CB_DH_LBA | s_devices[device];
    _regCommandInfo.devControl = _useInterrupts ? 0 : CB_DC_NIEN;
    _regCommandInfo.lbaSize = lba48;
    _regCommandInfo.lbaHigh = lba >> 32;
    _regCommandInfo.lbaLow = lba;
    
    if (multiCnt & 0x0800) {
        // Some sort of shenanigan
        multiCnt &= 0x00FF;
    } else {
        if (command != cmdREAD_MULTIPLE_EXT)
            multiCnt = 1;
    }
    
    _regCommandInfo.sectorCount = numSectors;
    _regCommandInfo.currentMultipleBlock = multiCnt;
    
    return _ExecDataCommandIn(device, (UInt8*)buffer, numSectors, multiCnt);
}

bool ATADriver::_ExecDataCommandOut(UInt32 device, UInt8 *buffer, UInt32 numSectors, UInt32 multiCnt)
{
    if (multiCnt == 0)
        multiCnt = 1;
    
    // Reset Bus Master Error bit
    IOPort()->writeBusMasterStatus(BM_SR_MASK_ERR);
    
    // Set timeout
    StartTimer();
    
    // Select drive
    if (!_Select(device))
        return false;
    
    // Setup
    _SetupCommand();
    
    // Start command
    IOPort()->outByte(CB_CMD, _regCommandInfo.command);
    ATA_IO_DELAY;
    
    // Loop to get result
    bool runLoop = true;
    UInt8 status;
    while (true) {
        status = IOPort()->inByte(CB_ASTAT);
        if (!(status & CB_STAT_BSY))
            break;
        if (_timer->IsSignalled()) {
            _regCommandInfo.timeOutError = true;
            _regCommandInfo.errorCode = 47;
            runLoop = false;
            break;
        }
    }
    
    // Loop to write each sector
    while (runLoop) {
        if ((status & (CB_STAT_BSY | CB_STAT_DRQ)) == CB_STAT_DRQ) {
            // Increment DRQ packets
            _regCommandInfo.drqPackets++;
            
            // Determine the number of sectors to transfer
            UInt32 wordCount = multiCnt;
            if (wordCount > numSectors)
                wordCount = numSectors;
            wordCount *= 256;
            
            // Do REP OUTSW to write one DRQ block
            _regCommandInfo.totalBytes += wordCount << 1;
            _drqBlockOut(CB_DATA, buffer, wordCount);
            ATA_IO_DELAY;
            
            // Count sectors
            numSectors -= multiCnt;
            buffer += 512 * multiCnt;
        }
        
        // Error?
        if (status & (CB_STAT_BSY | CB_STAT_DF | CB_STAT_ERR)) {
            _regCommandInfo.errorCode = 41;
            break;
        }
        if (!(status & CB_STAT_DRQ)) {
            _regCommandInfo.errorCode = 42;
            break;
        }
        
        // Wait
        _Wait(44, 45);
        status = (!_useInterrupts || _regCommandInfo.errorCode) ? IOPort()->inByte(CB_STAT) : IOPort()->InterruptStatus();
        if (_regCommandInfo.errorCode)
            break;
        
        // If all transfers complete, check it's all okay
        if (numSectors == 0) {
            // Any errors?
            if (status & (CB_STAT_BSY | CB_STAT_DF | CB_STAT_DRQ | CB_STAT_ERR)) {
                _regCommandInfo.errorCode = 43;
                break;
            }
            break;
        }
    }
    
    // BM_IDE Error=1?
    if (IOPort()->readBusMasterStatus() & BM_SR_MASK_ERR)
        _regCommandInfo.errorCode = 78;
    if (_regCommandInfo.errorCode != 0)
        kprintf("ATADriver::_ExecDataCommandOut Error %i\n", _regCommandInfo.errorCode);
    return _regCommandInfo.errorCode == 0;
}

bool ATADriver::DataCommandLBA28Out(UInt8 device, COMMAND command, UInt32 lba, void *buffer, UInt32 numSectors, UInt32 multiCnt)
{
    _regCommandInfo.command = command;
    _regCommandInfo.feature = 0;
    _regCommandInfo.secCount = numSectors;
    _regCommandInfo.devHead = CB_DH_LBA | s_devices[device];
    _regCommandInfo.devControl = _useInterrupts ? 0 : CB_DC_NIEN;
    _regCommandInfo.lbaSize = lba28;
    _regCommandInfo.lbaHigh = 0;
    _regCommandInfo.lbaLow = lba;
    
    if (multiCnt & 0x0800) {
        multiCnt &= 0x00FF;
    } else {
        if ((command != cmdWRITE_MULTIPLE) && (command != cmdCFA_WRITE_MULTIPLE_WO_ERASE))
            multiCnt = 1;
    }
    
    _regCommandInfo.sectorCount = numSectors;
    _regCommandInfo.currentMultipleBlock = multiCnt;
    
    return _ExecDataCommandOut(device, (UInt8*)buffer, numSectors, multiCnt);
}

bool ATADriver::DataCommandLBA48Out(UInt8 device, COMMAND command, UInt64 lba, void *buffer, UInt32 numSectors, UInt32 multiCnt)
{
    _regCommandInfo.command = command;
    _regCommandInfo.feature = 0;
    _regCommandInfo.secCount = numSectors;
    _regCommandInfo.devHead = CB_DH_LBA | s_devices[device];
    _regCommandInfo.devControl = _useInterrupts ? 0 : CB_DC_NIEN;
    _regCommandInfo.lbaSize = lba48;
    _regCommandInfo.lbaHigh = lba >> 32;
    _regCommandInfo.lbaLow = lba;
    
    if (multiCnt & 0x0800) {
        multiCnt &= 0x00FF;
    } else {
        if (command != cmdWRITE_MULTIPLE_EXT)
            multiCnt = 1;
    }
    
    _regCommandInfo.sectorCount = numSectors;
    _regCommandInfo.currentMultipleBlock = multiCnt;
    
    return _ExecDataCommandOut(device, (UInt8*)buffer, numSectors, multiCnt);
}

bool ATADriver::Packet(UInt8 device, char *controlBuffer, UInt32 controlBufferLength, bool directionOut, void *dataBuffer, UInt32 dataBufferLength)
{
    ClearMemory(&_regCommandInfo, sizeof(_regCommandInfo));
    
    int dir = directionOut ? 1 : 0;
    
    // reset Bus Master Error bit
    IOPort()->writeBusMasterStatus(BM_SR_MASK_ERR);
    
    // Check packet size is between 12 and 16
    controlBufferLength = (controlBufferLength < 12) ? 12 : controlBufferLength;
    controlBufferLength = (controlBufferLength > 12) ? 16 : controlBufferLength;
    
    // Set up command info
    _regCommandInfo.command = cmdPACKET;
    _regCommandInfo.feature = 0;
    _regCommandInfo.secNum = 0;
    _regCommandInfo.secCount = 0;
    _regCommandInfo.cylLow = controlBufferLength & 0xFF;
    _regCommandInfo.cylHigh = (controlBufferLength & 0xFF00) >> 8;
    _regCommandInfo.devHead = s_devices[device];
    _regCommandInfo.devControl = _useInterrupts ? 0 : CB_DC_NIEN;
    _regCommandInfo.errorCode = 0;
    
    // Set timeout
    StartTimer();
    
    // Select device
    if (!_Select(device))
        return false;
    _SetupCommand();
    
    // Send command
    IOPort()->outByte(CB_CMD, cmdPACKET);
    ATA_IO_DELAY;
    
    // Wait for not busy
    while (true) {
        UInt8 status = IOPort()->inByte(CB_ASTAT);
        if (!(status & CB_STAT_BSY))
            break;
        if (_timer->IsSignalled()) {
            _regCommandInfo.timeOutError = true;
            _regCommandInfo.errorCode = 51;
            dir = -1;
            break;
        }
    }
    
    // Command packet transfer
    if (!_regCommandInfo.errorCode) {
        UInt8 status = IOPort()->inByte(CB_STAT);
        if ((status & (CB_STAT_BSY | CB_STAT_DRQ | CB_STAT_ERR)) != CB_STAT_DRQ) {
            _regCommandInfo.errorCode = 52;
            dir = -1;
        } else {
            _drqBlockOut(CB_DATA, (UInt8*)controlBuffer, controlBufferLength >> 1);
            ATA_IO_DELAY;
        }
    }
    
    // Data transfer loop
    int count = 0;
    while (!_regCommandInfo.errorCode) {
        // Wait for something
        _Wait(53, 54);
        if (_regCommandInfo.errorCode) {
            dir = -1;
            break;
        }
        // get status
        UInt8 status = _useInterrupts ? IOPort()->InterruptStatus() : IOPort()->inByte(CB_STAT);
        if ((status & (CB_STAT_BSY | CB_STAT_DRQ)) == 0) {
            dir = -1;
            break;
        }
        if ((status & (CB_STAT_BSY | CB_STAT_DRQ)) != CB_STAT_DRQ) {
            _regCommandInfo.errorCode = 55;
            dir = -1;
            break;
        }
        // get byte count
        UInt16 byteCount = (IOPort()->inByte(CB_CH) << 8) | IOPort()->inByte(CB_CL);
        if (byteCount < 1) {
            _regCommandInfo.errorCode = 59;
            dir = -1;
            break;
        }
        // check overflow
        if ((count + byteCount) > dataBufferLength) {
            _regCommandInfo.errorCode = 61;
            dir = -1;
            break;
        }
        // increment counter
        _regCommandInfo.drqPackets++;
        // transfer data
        UInt16 wordCount = (byteCount >> 1) + (byteCount & 0x0001);
        _regCommandInfo.totalBytes += wordCount << 1;
        count += wordCount << 1;
        if (dir)
            _drqBlockOut(CB_DATA, (UInt8*)dataBuffer, wordCount);
        else
            _drqBlockIn(CB_DATA, (UInt8*)dataBuffer, wordCount);
        dataBuffer = ((char*)dataBuffer) + byteCount;
        ATA_IO_DELAY;
    }
    
    // End of command
    if ((!_regCommandInfo.errorCode) && (dir >= 0))
        _Wait(56, 57);
    
    // Final status check
    if (!_regCommandInfo.errorCode) {
        UInt8 status = _useInterrupts ? IOPort()->InterruptStatus() : IOPort()->inByte(CB_STAT);
        if (status & (CB_STAT_BSY | CB_STAT_DRQ | CB_STAT_ERR))
            _regCommandInfo.errorCode = 58;
    }
    
    // BMIDE Error=1?
    if (IOPort()->readBusMasterStatus() & BM_SR_MASK_ERR)
        _regCommandInfo.errorCode = 78;
    if (_regCommandInfo.errorCode != 0)
        kprintf("ATADriver::Packet Error %i\n", _regCommandInfo.errorCode);
    return _regCommandInfo.errorCode == 0;
}

template<class DRQHandler> void ProcessDRQBlock(ATADriverNode *io, UInt8 dataRegister, UInt8 *buffer, UInt32 wordCount)
{
    while (wordCount > 0) {
        UInt32 count = (wordCount > 16384) ? 16384 : wordCount;
        DRQHandler::Handle(io, dataRegister, buffer, count * 2);
        buffer += count * 2;
        wordCount -= count;
    }
}
template<class Handle8, class Handle16, class Handle32> void RunDRQ(ATADriverNode *io, UInt8 addressDataRegister, UInt8 *buffer, UInt32 wordCount, UInt32 transferWidth)
{
    switch (transferWidth) {
        case 32:
            if (!(wordCount & 0x01)) {
                ProcessDRQBlock<Handle32>(io, addressDataRegister, buffer, wordCount);
                break;
            }
            // fall through
        default:
        case 16:
            ProcessDRQBlock<Handle16>(io, addressDataRegister, buffer, wordCount);
            break;
        case 8:
            ProcessDRQBlock<Handle8>(io, addressDataRegister, buffer, wordCount);
            break;
    }
}
#define DRQHandleClass(Method)      class DRQHandle_##Method { public: static void Handle(ATADriverNode *io, UInt8 reg, UInt8 *buffer, UInt32 length) { io->Method(reg, buffer, length); } }
DRQHandleClass(inByteRep);
DRQHandleClass(inShortRep);
DRQHandleClass(inLongRep);
DRQHandleClass(outByteRep);
DRQHandleClass(outShortRep);
DRQHandleClass(outLongRep);

void ATADriver::_drqBlockOut(UInt8 addressDataRegister, UInt8 *buffer, UInt32 wordCount)
{
    RunDRQ<DRQHandle_outByteRep, DRQHandle_outShortRep, DRQHandle_outLongRep>(IOPort(), addressDataRegister, buffer, wordCount, _pioTransferWidth);
}

void ATADriver::_drqBlockIn(UInt8 addressDataRegister, UInt8 *buffer, UInt32 wordCount)
{
    RunDRQ<DRQHandle_inByteRep, DRQHandle_inShortRep, DRQHandle_inLongRep>(IOPort(), addressDataRegister, buffer, wordCount, _pioTransferWidth);
}

//bool ATADriver::_dmaSetup(bool directionOut, UInt8 *buffer, UInt32 byteCount)
//{
//    // Disable/stop DDMA, clear interrupt and error
//    IOPort()->writeBusMasterCommand(BM_CR_MASK_STOP);
//    IOPort()->writeBusMasterStatus(_BMIDE_Status | BM_SR_MASK_INT | BM_SR_MASK_ERR);
//    
//    // PRD list
//    UInt32* prdPointer = _dmaBuffer->Virtual();
//    UInt64 physAddr = _dmaBuffer->Physical();
//    UInt32 numPRD = 0;
//    while (byteCount > 0) {
//        if (numPRD >= MAX_PRD)
//            return false;
//        // Set PRD address
//        prdPointer[0] = physAddr;
//        // Compute count
//        UInt32 temp = 65536;
//        if (temp > byteCount)
//            temp = byteCount;
//        physAddr = physAddr + temp;
//        if ((physAddr & 0xFFFF0000) != (prdPointer[0] & 0xFFFF0000)) {
//            physAddr &= 0xFFFF0000;
//            temp = physAddr - prdPointer[0];
//        }
//        // Set
//        prdPointer[1] = temp & 0x0000FFFF;
//        // Update byte count
//        byteCount -= temp;
//        if (byteCount == 0)
//            prdPointer[1] |= 0x80000000;    // Set PRD end bit
//        prdPointer += 2;
//        numPRD++;
//    }
//    
//    // *(pio_bmide_base_addr + BM_PRD_ADDR_LOW) = _dmaBuffer->Virtual();    // What?
//    
//    // Set read/write control (PCI read is for ATA write)
//    _BMIDE_Direction_Control = directionOut ? BM_CR_MASK_READ : BM_CR_MASK_WRITE;
//    IOPort()->writeBusMasterCommand(_BMIDE_Direction_Control);
//    return true;
//}
