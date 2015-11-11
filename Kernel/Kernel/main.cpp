#include "debug.h"
#include "multiboot.h"
#include "mem_physical.h"
#include "tools.h"
#include "mem_logical.h"
#include "BasicHeap.h"
#include "StandardPC.h"
#include "CPU_intr.h"
#include "Scheduler.h"
#include "mem_virtual.h"
#include "Interface.h"
#include "fs_iso9660.h"
#include "Startup.h"
#include "Video_Multiboot.h"

extern UInt32 kern_start, kern_end;

extern UInt32 virt, phys;

BasicHeap *s_coreHeap;
static unsigned char s_coreMemory[65535*4];

#include "Thread.h"
Thread *one, *two;
int threadCount = 0;
static Thread* ThreadAt(int index)
{
    if (index == 0)
        return one;
    else
        return two;
}
static int entered = -1;
int x = 0, y = 0;

#include "Queue.h"
DispatchQueue *testQueue;
class TestTask : public DispatchQueue::Task
{
public:
    TestTask(char c, int x = 50, int y = 22)
    {
        _x = x;
        _y = y;
        _c = c;
    }
    
protected:
    void Execute(void)
    {
        test(_x, _y, _c);
    }
    
private:
    int _x, _y;
    char _c;
};

#include "IPC.h"

#include "Interface_Block.h"
class TestCDClient : public SignalWatcher
{
private:
    IpcService *_service;
    IpcEndpoint *_ourEnd;
public:
    TestCDClient(IpcService *service)
    {
        _service = service;
        
        _ourEnd = service->RequestConnection();
        _ourEnd->AddRef();
        _ourEnd->RegisterObserver(this);
        
        KernelBufferMemory *request = _ourEnd->CreateSendBuffer();
        KernelBufferMemory::Map *input = new KernelBufferMemory::Map(NULL, request, false);
        BlockRequestRead *readRequest = (BlockRequestRead*)input->LinearBase();
        readRequest->identifier = 1234;
        readRequest->type = BlockRequest::Read;
        readRequest->offset = 0x10 * 2048;
        readRequest->length = 8;
        input->Release();
        kprintf("Request %.8x size %i\n", request, request->Size());
        _ourEnd->SendBuffer(request);
        request->Release();
    }
protected:
    ~TestCDClient()
    {
        _ourEnd->Release();
    }
    void SignalChanged(BlockableObject *watching, bool active)
    {
        if (active) {
            KernelBufferMemory *response = _ourEnd->Read(false);
            KernelBufferMemory::Map *output = new KernelBufferMemory::Map(NULL, response, true);
            BlockResponseRead *readResponse = (BlockResponseRead*)output->LinearBase();
            if (readResponse->type != BlockRequest::Read) {
                kprintf("CD: unexpected type %i\n", readResponse->type);
                return;
            }
            if (readResponse->status != BlockResponse::Success) {
                kprintf("CD: failed to read: %i\n", readResponse->status);
                return;
            }
            kprintf("CD read:");
            unsigned char *bob = (unsigned char*) readResponse->data();
            for (int i = 0; i < 8; i++)
                kprintf(" %.2x", (int)bob[i]);
            kprintf("\n");

            FileSystem_ISO9660 *fs = new FileSystem_ISO9660();
            fs->ConnectInput("cdrom"_ko, _service);
        }
    }
};

#include "Interface_File.h"
class TestFileSystemClient : public SignalWatcher
{
private:
    IpcService *_service;
    IpcEndpoint *_ourEnd;
    
    UInt32 _fileNode;
    UInt32 _handle;
    
    void LaunchTest(int index)
    {
        KernelBufferMemory *request = _ourEnd->CreateSendBuffer();
        KernelBufferMemory::Map *input = new KernelBufferMemory::Map(NULL, request, false);
        bool send = true;
        switch (index) {
            case 0:
            {
                DirectoryRequest *dirRequest = (DirectoryRequest*)input->LinearBase();
                dirRequest->identifier = index;
                dirRequest->type = DirectoryRequest::Search;
                dirRequest->rootNode = DirectoryRequest::RootNode;
                dirRequest->offset = 0;
                dirRequest->length = 0xFFFF;
                dirRequest->subpath.Initialise();
                kprintf("Requesting root directory\n");
            }
                break;
            case 1:
            {
                OpenRequest *openRequest = (OpenRequest*)input->LinearBase();
                openRequest->identifier = index;
                openRequest->type = OpenRequest::OpenFile;
                openRequest->rootNode = _fileNode;
                openRequest->subpath.Initialise();
                kprintf("Opening first file\n");
            }
                break;
            case 2:
            {
                ReadRequest *readRequest = (ReadRequest*)input->LinearBase();
                readRequest->identifier = index;
                readRequest->type = ReadRequest::ReadFile;
                readRequest->handle = _handle;
                readRequest->offset = 0;
                readRequest->length = 25;
                kprintf("Reading %i bytes\n", readRequest->length);
            }
                break;
            case 3:
            {
                FileRequest *closeRequest = (FileRequest*)input->LinearBase();
                closeRequest->identifier = index;
                closeRequest->type = FileRequest::CloseFile;
                closeRequest->handle = _handle;
                kprintf("Closing file\n");
            }
                break;
            default:
                send = false;
                break;
        }
        input->Release();
        if (send)
            _ourEnd->SendBuffer(request);
        request->Release();
    }
    void HandleTest(Interface_Response *response)
    {
        switch (response->identifier) {
            case 0:
            {
                DirectoryResponse *dirResponse = (DirectoryResponse*)response;
                if (dirResponse->type != DirectoryRequest::Search) {
                    kprintf("FS: unexpected type %i\n", dirResponse->type);
                    return;
                }
                if (dirResponse->status != Interface_Response::Success) {
                    kprintf("FS: failed to read: %i\n", dirResponse->status);
                    return;
                }
                kprintf("Directory read %i items:\n", (int)dirResponse->directoryEntries.Count());
                FlatString *nodeKey = FlatString::CreateDynamic(Node_ID);
                FlatString *nameKey = FlatString::CreateDynamic(Node_Name);
                FlatString *typeKey = FlatString::CreateDynamic(Node_Type);
                FlatString *typeDirectory =FlatString::CreateDynamic(NoteType_Directory);
                FlatString *sizeKey = FlatString::CreateDynamic(Node_Size);
                for (UInt32 i = 0; i < dirResponse->directoryEntries.Count(); i++) {
                    FlatDictionary *file = (FlatDictionary*)dirResponse->directoryEntries.ItemAt(i);
                    FlatInteger *node = (FlatInteger*)file->ItemFor(nodeKey);
                    FlatString *name = (FlatString*)file->ItemFor(nameKey);
                    FlatString *type = (FlatString*)file->ItemFor(typeKey);
                    if (type->IsEqual(typeDirectory))
                        kprintf("\t%i: %s   (directory)\n", (int)node->Value(), name->Value());
                    else {
                        FlatInteger *size = (FlatInteger*)file->ItemFor(sizeKey);
                        kprintf("\t%i: %s   %i bytes\n", (int)node->Value(), name->Value(), (int)size->Value());
                        if (!_fileNode)
                            _fileNode = node->Value();
                    }
                }
                sizeKey->ReleaseDynamic();
                typeDirectory->ReleaseDynamic();
                typeKey->ReleaseDynamic();
                nameKey->ReleaseDynamic();
                nodeKey->ReleaseDynamic();
                kprintf("\n");
                
            }
                break;
            case 1:
            {
                OpenResponse *openResponse = (OpenResponse*)response;
                if (openResponse->type != OpenRequest::OpenFile) {
                    kprintf("FS: unexpected type %i\n", openResponse->type);
                    return;
                }
                if (openResponse->status != Interface_Response::Success) {
                    kprintf("FS: failed to open: %i\n", openResponse->status);
                    return;
                }
                kprintf("Opened file: handle %i\n", openResponse->handle);
                _handle = openResponse->handle;
            }
                break;
            case 2:
            {
                ReadResponse *readResponse = (ReadResponse*)response;
                if (readResponse->type != ReadRequest::ReadFile) {
                    kprintf("FS: unexpected type %i\n", readResponse->type);
                    return;
                }
                if (readResponse->status != Interface_Response::Success) {
                    kprintf("FS: failed to read file: %i\n", readResponse->status);
                    return;
                }
                kprintf("Got: %s\n", readResponse->data());
            }
                break;
            case 3:
            {
                NodeResponse *closeResponse = (NodeResponse*)response;
                if (closeResponse->type != NodeRequest::CloseFile) {
                    kprintf("FS: unexpected type %i\n", closeResponse->type);
                    return;
                }
                kprintf("File closed: %i\n", closeResponse->status);
            }
                break;
            default:
                break;
        }
        LaunchTest(response->identifier + 1);
    }
public:
    TestFileSystemClient(IpcService *service)
    {
        _service = service;
        
        _ourEnd = service->RequestConnection();
        _ourEnd->AddRef();
        _ourEnd->RegisterObserver(this);
        
        _fileNode = 0;
        LaunchTest(0);
    }
protected:
    ~TestFileSystemClient()
    {
        _ourEnd->Release();
        
    }
    
    void SignalChanged(BlockableObject *watching, bool active)
    {
        if (active) {
            KernelBufferMemory *response = _ourEnd->Read(false);
            KernelBufferMemory::Map *output = new KernelBufferMemory::Map(NULL, response, true);
            HandleTest((Interface_Response*)output->LinearBase());
            output->Release();
        }
    }
};

IpcService *testService = NULL;
IpcService *cdService = NULL;
class TestServiceWatcher : public IpcServiceWatcher
{
public:
    void ServiceProviderAppeared(KernelObject *provider)
    {
        kprintf("Provider started: %.8x\n", provider);
    }
    
    void ServiceAppeared(KernelObject *provider, IpcService *service)
    {
        kprintf("Service started on %.8x: %.8x [%s / %s]\n", provider, service, service->Name()->CString(), service->ServiceType()->CString());
        if (service->ServiceType()->IsEqualTo("system.test"_ko))
            testService = service;
        if (service->Name()->IsEqualTo("atapi0"_ko)) {
            kprintf("Found CD service %.8x\n", service);
            new TestCDClient(service);
        }
        if (service->ServiceType()->IsEqualTo("filesystem"_ko)) {
            new TestFileSystemClient(service);
        }
    }

    void ServiceChanged(KernelObject *provider, IpcService *service)
    {
        kprintf("Service changed on %.8x: %.8x\n", provider, service);
    }
    
    void ServiceRemoved(KernelObject *provider, IpcService *service)
    {
        kprintf("Service stopped on %.8x: %.8x [%s / %s]\n", provider, service, service->Name()->CString(), service->ServiceType()->CString());
    }
    
    void ServiceProviderRemoved(KernelObject *provider)
    {
        kprintf("Provider stopped: %.8x\n", provider);
    }
};

class TestService : public SignalWatcher
{
private:
    IpcServiceList *_serviceList;
    IpcService *_service;
    
    class Handler : public SignalWatcher
    {
    private:
        IpcEndpoint *_ourEnd;
    public:
        Handler(IpcEndpoint *ourEnd)
        {
            _ourEnd = ourEnd;
            _ourEnd->RegisterObserver(this);
        }
        
        void SignalChanged(BlockableObject *watching, bool active)
        {
            if (!_ourEnd->IsConnected()) {
                _ourEnd->UnregisterObserver(this);
                this->Release();
            } else {
                KernelBufferMemory *data;
                while ((data = _ourEnd->Read(false)) != NULL) {
                    KernelBufferMemory::Map *input = new KernelBufferMemory::Map(NULL, data, true);
                    KernelBufferMemory *reply = _ourEnd->CreateSendBuffer();
                    KernelBufferMemory::Map *output = new KernelBufferMemory::Map(NULL, reply, false);
                    char *inputData = (char*)input->LinearBase();
                    char *outputData = (char*)output->LinearBase();
                    for (UInt64 i = 0; i < data->Size(); i++) {
                        outputData[i] = ~inputData[i];
                    }
                    input->Release();
                    output->Release();
                    _ourEnd->SendBuffer(reply);
                    reply->Release();
                }
            }
        }
    };
public:
    TestService()
    {
        _serviceList = new IpcServiceList(this);
        _service = new IpcService("testNode"_ko, "system.test"_ko);
        _service->RegisterObserver(this);
        _serviceList->AddService(_service);
    }
    
    void SignalChanged(BlockableObject *watching, bool active)
    {
        if (active) {
            kprintf("TestService got new connection\n");
            new Handler(_service->NextConnection(false));
        }
    }
    
protected:
    ~TestService()
    {
        _serviceList->RemoveService(_service);
        _service->Release();
        _serviceList->Release();
    }
};

class TestClient : public SignalWatcher
{
private:
    IpcEndpoint *_ourEnd;
public:
    TestClient(IpcService *service)
    {
        _ourEnd = service->RequestConnection();
        _ourEnd->RegisterObserver(this);
        
        KernelBufferMemory *request = _ourEnd->CreateSendBuffer();
        KernelBufferMemory::Map *input = new KernelBufferMemory::Map(NULL, request, false);
        char *inputBuf = (char*)input->LinearBase();
        kprintf("Test client sending:");
        for (int i = 0; i < 12; i++) {
            inputBuf[i] = i;
            kprintf(" %.2x", inputBuf[i]&0xFF);
        }
        kprintf("\n");
        input->Release();
        _ourEnd->SendBuffer(request);
        request->Release();
    }
    
    void SignalChanged(BlockableObject *watching, bool active)
    {
        if (active) {
            KernelBufferMemory *response = _ourEnd->Read(false);
            KernelBufferMemory::Map *output = new KernelBufferMemory::Map(NULL, response, true);
            char *outputBuf = (char*)output->LinearBase();
            kprintf("Test client received:");
            for (int i = 0; i < 12; i++) {
                kprintf(" %.2x", outputBuf[i]&0xFF);
            }
            kprintf("\n");
            output->Release();
        }
    }
    
protected:
    ~TestClient()
    {
        _ourEnd->Release();
    }
};

#include "Interrupts.h"
int taunt = 0;
static bool KeyboardTestHandler(void *context, void *state)
{
    unsigned char c = inb(0x60);
    kprintf("%.2x ", c);
    if (c == 0x39)
//        ((BasicHeap*)NULL)->AddBlock(0, 0);
        taunt++;
    if (c == 0x14)
        Thread::DebugPrint();
    
    return true;
}

static const char *testchars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
class TestThread : public KernelThread
{
public:
    TestThread(int x, int y, const char *msg, int sleep = 0)
    {
        _x = x;
        _y = y;
        _msg = msg;
        _sleep = sleep;
    }
protected:
    void ThreadMain(void)
    {
        kprintf("%s 0x%.8x\n", _msg, this);
        int i = 1;
        while (1 || (i != 0)) {
            test(_x, _y, testchars[i]);
            i++;
            if (testchars[i] == '\0')
                i = 0;
            if ((_x - 49) == taunt) {
//                kprintf("Lose all marks in exam\n");
//                break;
                s_coreHeap->Test();
            }
            if (_sleep) {
                test(12, 0, '?');
                Thread::Active->Sleep(MILLISECONDS(_sleep));
                test(12, 0, '!');
                
                TestTask *testTask = new TestTask('W');
//                testQueue->AddTask(testTask);
                testTask->Release();
            } else {
                _count++;
                if (_count > 100000) {
                    _count = 0;
                    TestTask *testTask = new TestTask('N');
//                    testQueue->AddTask(testTask);
                    testTask->Release();
                    
                }
            }
        }
    }
private:
    int _x, _y;
    const char *_msg;
    int _sleep;
    int _count;
};

#include "Process.h"
static bool TestHandler(void *context, void *state)
{
    TrapFrame *tf = (TrapFrame*)state;
    test('>');
    test(tf->EBX, tf->ECX, tf->EAX);
    return true;
}
extern "C" void ProcessTestThreadOne(void*);
extern "C" void ProcessTestThreadTwo(void*);
static void ConfigureProcessTest(void (*test)(void*), const char *name)
{
    Process *testProcess = new Process(name);
    PhysicalPointer page = CPhysicalMemory::AllocateContiguousPages();
    void *linearPage = testProcess->pageDirectory.Map(fmWritable | fmUser, pmApplication, page);
    testProcess->pageDirectory.Select();
    CopyMemory(linearPage, (void*)test, 4096);
    new Thread(testProcess, (void(*)(void*))linearPage, NULL);
}

static inline void* FixAddress(void *address)
{
    return ((char*)address) + (UInt32)&virt - (UInt32)&phys;
}
extern UInt32 stack;
extern "C" int k_main(multiboot_info_t* mbd, unsigned int magic)
{
    BasicHeap mainHeap(128);
	multiboot_mmap_entry *map;

    s_coreHeap = &mainHeap;
    mainHeap.AddBlock(s_coreMemory, sizeof(s_coreMemory));

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		kprintf("Multiboot: magic number incorrect\n");
		return -1;
	}

    // Initialise memory management (including paging)
	CPhysicalMemory::Init();
    rootAddressSpace.InitKernel();
    
    // Process Multiboot information
	mbd = (multiboot_info_t*)FixAddress(mbd);
    // Add memory
	for (
		map = (multiboot_mmap_entry*)FixAddress((void*)mbd->mmap_addr);
		map < (multiboot_mmap_entry*)(((char*)FixAddress((void*)mbd->mmap_addr)) + mbd->mmap_length);
		map = (multiboot_mmap_entry*)(((char*)map) + map->size + sizeof(map->size)))
	{
		if (map->type == MULTIBOOT_MEMORY_AVAILABLE)
		{
			// Usable RAM
			CPhysicalMemory::AddChunk((PhysicalPointer)map->addr, map->len);
		}
	}
	CPhysicalMemory::AddReserved((void*)(((UInt32)&phys) & 0xFFC00000), 0x00400000);
    // Initialise video (after memory - it may be required to allocate pages even for just a graphical console)
    Init_Video_Multiboot(FixAddress((void*)mbd->vbe_control_info), FixAddress((void*)mbd->vbe_mode_info), mbd->vbe_mode);

    // Splash
	kprintf("Munro Systems\nCosmOS 1.0\nCopyright (C) 2008-2015 Colin Munro\n\n");
    
    TestServiceWatcher *watcher = new TestServiceWatcher();
    IpcServiceList::Register(watcher);
    
    StandardPC *rootDevice = new StandardPC();
    rootDevice->Start(NULL);

    rootDevice->Test()->RegisterHandler(0x21, KeyboardTestHandler, NULL);
    
    // Do something else
	kprintf("\nStarting!\n");
	test('Z');
    
    bicycle::function<int(void)> test = [rootDevice](){
        kprintf("Lambda %.8x\n", rootDevice);
        return 0;
    };
    test();

    Thread::ConfigureService(rootDevice->Test());
    Collections::ConfigureService();
    BlockableObject::ConfigureService();
    KernelObject::ConfigureService();
    InitStartup();
    
    AutoreleasePool testPool;
    // Thread test
    one = new TestThread(51, 20, "Ahoy");
    two = new TestThread(50, 20, "Hello", 250);
    
    // Process test
    ConfigureProcessTest(ProcessTestThreadOne, "Three");
    ConfigureProcessTest(ProcessTestThreadTwo, "Four");
    rootDevice->Test()->RegisterHandler(0x99, TestHandler, NULL);
    rootDevice->Test()->ConfigureSyscall(0x99);
    
    // Queue test
    testQueue = new DispatchQueue();
    TestTask *testTask = new TestTask('?');
    testQueue->AddTask(testTask);
    testTask->Release();

    // Service/IPC test
    new TestService();
    kprintf("Found service %.8x\n", testService);
    new TestClient(testService);
    
    CPU_Interrupt_Disable();
    Scheduler::BeginScheduling();
    kprintf("What happen\n");
    
    // Start scheduler?
    while (1) asm("hlt");
    
    // Shut down?
    rootDevice->Stop();
    
	return 0;
}
