#include "StandardPC.h"
#include "StandardPC_traps.h"
#include "Interrupts.h"
#include "descript.h"
#include "mem_logical.h"
#include "debug.h"
#include "CPU_intr.h"
#include "CPU.h"
#include "Driver_ATA.h"
#include "Blocking.h"
#include "Scheduler.h"
#include "Collections.h"
#include "mem_virtual.h"
#include "GenericKeyboard.h"
#include "GenericMouse.h"
#include "pci.h"
#include "Console.h"
#include "Runloop.h"
#include "elfsyms.h"

static bool s_schedulerEnabled = false;

void StartScheduling(void)
{
    s_schedulerEnabled = true;
}

static const char *s_trapNames[] = {
    /* 0x00 */ "Division by zero",
    /* 0x01 */ "Debugger",
    /* 0x02 */ "NMI",
    /* 0x03 */ "Breakpoint",
    /* 0x04 */ "Overflow",
    /* 0x05 */ "Bounds",
    /* 0x06 */ "Invalid opcode",
    /* 0x07 */ "Coprocessor not present",
    /* 0x08 */ "Double fault",
    /* 0x09 */ "Coprocessor overrun",
    /* 0x0A */ "Invalid TSS",
    /* 0x0B */ "Segment absent",
    /* 0x0C */ "Stack fault",
    /* 0x0D */ "General Protection Fault",
    /* 0x0E */ "Page fault",
    /* 0x0F */ NULL,
    /* 0x10 */ "Math fault",
    /* 0x11 */ "Alignment check",
    /* 0x12 */ "Machine check",
    /* 0x13 */ "Floating point exception",
};

const char* StandardPC::NameForTrap(unsigned int trap)
{
    if (trap >= (sizeof(s_trapNames) / sizeof(s_trapNames[0])))
        return NULL;
    return s_trapNames[trap];
}

namespace StandardPC_Internal {

    
    class LegacyATA : public ATADriverNode
    {
    public:
        CLASSNAME(ATADriverNode, StandardPC_Internal::LegacyATA);
        
    private:
        class SimplerSignal : public SimpleSignal
        {
        private:
            bool _set;
            
        public:
            CLASSNAME(SimplerSignal, StandardPC_Internal::LegacyATA::SimplerSignal);
            
            SimplerSignal()
            :SimpleSignal(false)
            {
                _set = false;
            }
            
            void Activate(void)
            {
                if (_set)
                    return;
                _set = true;
                Set();
            }
            void Deactivate(void)
            {
                if (!_set)
                    return;
                _set = false;
                Reset();
            }
        };
        
        UInt16 _basePort;
        UInt16 _baseIRQ;
        InterruptHandlerHandle _interruptHandle;
        SimplerSignal *_interrupt;
        
        unsigned short GetPort(UInt32 address)
        {
            static const UInt16 offsets[] = {0, 1, 2, 3, 4, 5, 6, 7, 0x206};
            return _basePort + offsets[address];
        }
        
        static bool InterruptCallback(void *context, void *info)
        {
            StandardPC_Internal::LegacyATA *that = (StandardPC_Internal::LegacyATA*)context;
            that->_interrupt->Activate();
            that->UpdateForInterrupt();
            return true;
        }
        
    protected:
        ~LegacyATA()
        {
            _interrupt->Release();
        }
        
    public:
        LegacyATA(UInt16 basePort, UInt16 baseIRQ)
        :ATADriverNode("ATA Legacy I/O")
        {
            _basePort = basePort;
            _baseIRQ = baseIRQ;
            _interrupt = new SimplerSignal();
            SetProperty(kDriver_Property_Bus, kDriver_Bus_SystemATA);
        }
        
        bool Start(Driver *parent)
        {
            _interruptHandle = InterruptSource()->RegisterHandler(_baseIRQ + PIC_IRQ_OFFSET, InterruptCallback, this);
            return ATADriverNode::Start(parent);
        }
        
        void Stop(void)
        {
            InterruptSource()->UnregisterHandler(_interruptHandle);
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
//            asm("rep outsd"::"c"(length / sizeof(UInt32)), "d"(GetPort(address)), "S"(buffer));
            outShortRep(address, buffer, length);
        }
        
        BlockableObject* Interrupt(void)
        {
            return _interrupt;
        }
        
        void ResetInterrupt(void)
        {
            _interrupt->Deactivate();
        }
        
        bool DMAAvailable(void)
        {
            return false;
        }
        
        UInt8 readBusMasterStatus(void)
        {
            return 0;
        }
        
        void writeBusMasterStatus(UInt8 x)
        {
            // nothing
        }
    };
    
    class Interrupts : public ::Interrupts
    {
    public:
        CLASSNAME(Interrupts, StandardPC_Internal::Interrupts);
        
    private:
        class HandlerHandle
        {
        public:
            HandlerHandle(HandlerHandle **root, InterruptHandlerBlock handler, UInt16 irq)
            {
                _irq = irq;
                _root = root;
                _last = NULL;
                _next = *root;
                (*_root) = this;
                _handler = handler;
            }
            ~HandlerHandle()
            {
                if (_last != NULL)
                    _last->_next = _next;
                if (_next != NULL)
                    _next->_last = _last;
                if ((*_root) == this)
                    (*_root) = (_next != NULL) ? _next : _last;
            }
            
            static void Trigger(HandlerHandle *that, TrapFrame *frame)
            {
                while (that != NULL) {
                    if (that->_handler(frame))
                        return;
                    that = that->_next;
                };
                // TODO: Unhandled trap
                //            kprintf("Unhandled trap: 0x%.2x\n", frame->TrapNumber);
            }
            UInt16 _irq;
        private:
            HandlerHandle **_root;
            HandlerHandle *_last, *_next;
            InterruptHandlerBlock _handler;
        };
    private:
        idt_entry entries[256];
        HandlerHandle *handlers[256];
    public:
        Interrupts()
        {
            void **vectorBase = &vectors;
            for (int i = 0; i < 256; i++) {
                entries[i].Set(false, SEG_KCODE<<3, vectorBase[i], 0);
                handlers[i] = NULL;
            }
        }
        
        // Internal workings
        void Set(void)
        {
            cpu_ptr data;
            data.base = UInt32(entries);
            data.limit = sizeof(entries);
            idt_load(&data);
        }
        
        void Trigger(TrapFrame *frame)
        {
            if ((frame->TrapNumber >= PIC_IRQ_OFFSET) && (frame->TrapNumber < (PIC_IRQ_OFFSET + 16))) {
                int irq = frame->TrapNumber - PIC_IRQ_OFFSET;
                // Spurious interrupt detection
                if (CPU_PIC_IsSpurious(irq)) {
                    kprintf("Got spurious %i\n", irq);
                    return;
                }
                // Acknowledge, as we may not immediately return from triggering the interrupt
                CPU_PIC_SendEOI(irq);
            }
            // Actually handle
            HandlerHandle::Trigger(handlers[frame->TrapNumber], frame);
        }
        
        // External API
        InterruptHandlerHandle RegisterHandler(int irq, InterruptHandlerBlock handler)
        {
            bool wasUsed = handlers[irq] != NULL;
            InterruptHandlerHandle handle = new HandlerHandle(handlers + irq, handler, irq);
            if (!wasUsed) {
                // Any initialisation necessary?
                if ((irq >= PIC_IRQ_OFFSET) && (irq < (PIC_IRQ_OFFSET + 16)))
                    CPU_PIC_Enable(irq - PIC_IRQ_OFFSET, true);
            }
            return handle;
        }
        
        void UnregisterHandler(InterruptHandlerHandle handler)
        {
            HandlerHandle *handle = (HandlerHandle*)handler;
            UInt16 irq = handle->_irq;
            delete handle;
            // Should this happen first?
            if (handlers[irq] == NULL) {
                // Do we need to uninitialise?
                if ((irq >= PIC_IRQ_OFFSET) && (irq < (PIC_IRQ_OFFSET + 16)))
                    CPU_PIC_Enable(irq - PIC_IRQ_OFFSET, false);
            }
        }
        
        void ConfigureSyscall(int irq)
        {
            void **vectorBase = &vectors;
            entries[irq].Set(true, SEG_KCODE<<3, vectorBase[irq], DPL_USER);
            // Tell CPU to reload
            // TODO: multiprocessor
            Set();
        }
    };
    
    class Timer : public Driver
    {
    public:
        CLASSNAME(Driver, StandardPC_Internal::Timer);
        
        Timer()
        :Driver("PC Timer")
        {
        }
        
        bool Start(Driver *parent)
        {
            int divisor = 1193180 / 10000/*Hz*/;
            outb(0x43, 0x36);
            outb(0x40, divisor & 0xFF);
            outb(0x40, divisor >> 8);
            _timerInterrupt = InterruptSource()->RegisterHandler(PIC_IRQ_OFFSET + 0, InterruptCallback, this);
            return Driver::Start(parent);
        }
        
        void Stop(void)
        {
            InterruptSource()->UnregisterHandler(_timerInterrupt);
            Driver::Stop();
        }
        
    private:
        InterruptHandlerHandle _timerInterrupt;
        
        static bool InterruptCallback(void *context, void *state)
        {
            ::Timer::TimerTick(MILLISECONDS(10));
            if (s_schedulerEnabled)
                Scheduler::EnterFromInterrupt();
            return true;
        }
    };
    
    class PS2Controller;
    
    class PS2Keyboard : public GenericKeyboard
    {
    public:
        CLASSNAME(GenericKeyboard, StandardPC_Internal::PS2Keyboard);
        
        PS2Keyboard(void);
        
        void FeedByte(UInt8 c);
        void ResetInput(void);
    };
    
    class PS2Mouse : public GenericMouse
    {
    public:
        CLASSNAME(GenericMouse, StandardPC_Internal::PS2Mouse);
        
        PS2Mouse();
        
        bool Start(Driver *parent);
        void Stop(void);
        
        void FeedByte(UInt8 c);
        void ResetInput(void);
        
    private:
        void Reset(bool retry);
        void SetSampleRate(UInt8 rate, bicycle::function<void(bool success)> onCompletion);
        void GetMouseID(bicycle::function<void(UInt8 result)> onCompletion);
        void RequestSinglePacket(bicycle::function<void(UInt8 *data)> onCompletion);
        void SendReset(void);
        void SetStreaming(bool enabled, bicycle::function<void(bool success)> onCompletion);
        
        UInt8 _mouseID;
        UInt8 _butans;
        
        UInt8 _buffer[10];
        int _received;
    };
    
    class PS2Controller : public Driver
    {
    public:
        CLASSNAME(Driver, StandardPC_Internal::PS2Controller);
        
        PS2Controller()
        :Driver("PS/2 controller")
        {
            _runloop = new RunloopThread();
        }
        
        bool Start(Driver *parent)
        {
            // Initialise controller
            SendByte(true, 0x20 /* Get Compaq Status Byte */);
            UInt8 status = ReadByte(NULL);
            status |= 1 << 1;       // Enable IRQ12
            status &= ~(1 << 5);    // Disable Mouse Clock
            SendByte(true, 0x60 /* Set Compaq Status Byte */);
            SendByte(false, status);
            SendByte(true, 0xA8 /* Enable Auxiliary Device */);
            // Flush any acknowledgements, which we don't currently care about
            while(true) {
                io_wait();
                if (!(ReadStatus() & 0x01))
                    break;
                ReadData();
            }
            // Hook up interrupts
            InterruptHandlerBlock handler = [this](void *state)->bool{
                bool processed = false;
                while (true) {
                    UInt8 status = ReadStatus();
                    if (!(status & 0x01))
                        break;
                    processed = true;
                    UInt8 data = ReadData();
                    bool aux = status & 0x20;
                    KernelArray *array = aux ? _commandMouse : _commandKeyboard;
                    _runloop->AddTask([=]{
                        if (array->Count() == 0) {
                            if (aux) {
                                // Aux (mouse) port
                                _mouse->FeedByte(data);
                            } else {
                                // Keyboard port
                                _keyboard->FeedByte(data);
                            }
                        } else {
                            bool first = true;
                            while (true) {
                                Command *current = (Command*)array->ObjectAt(0);
                                if (first) {
                                    if (!current->AddByte(data))
                                        break;
                                    first = false;
                                } else {
                                    if ((current == NULL) || (!current->Run()))
                                        break;
                                }
                                array->Remove(current);
                            }
                        }
                    });
                }
                return processed;
            };
            _commandKeyboard = new KernelArray();
            _commandMouse = new KernelArray();
            _interruptKeyboard = InterruptSource()->RegisterHandler(PIC_IRQ_OFFSET + 1, handler);
            _interruptMouse = InterruptSource()->RegisterHandler(PIC_IRQ_OFFSET + 12, handler);
            _keyboard = new PS2Keyboard();
            ConnectChild(_keyboard);
            _keyboard->Start(this);
            _mouse = new PS2Mouse();
            ConnectChild(_mouse);
            _mouse->Start(this);
            return Driver::Start(parent);
        }
        
        void Stop(void)
        {
            InterruptSource()->UnregisterHandler(_interruptKeyboard);
            InterruptSource()->UnregisterHandler(_interruptMouse);
            _keyboard->Stop();
            DetachChild(_keyboard);
            _commandKeyboard->Release();
            _keyboard->Release();
            _mouse->Stop();
            DetachChild(_mouse);
            _commandMouse->Release();
            _mouse->Release();
            Driver::Stop();
        }
        
        void SendCommand(bool aux, UInt8 command, UInt8 *data, int datalen, int responselen, bicycle::function<void(UInt8 *response)> response)
        {
            // TODO: ADD TIMEOUT
            UInt8 outdata[10];
            if (data != NULL) {
                for (int i = 0; i < datalen; i++)
                    outdata[i] = data[i];
            } else {
                datalen = 0;
            }
            _runloop->AddTask([=]{
                Command *commandItem = new Command([=]{
                    if (aux)
                        SendByte(true, 0xD4);
                    SendByte(false, command);
                    if (datalen != 0) {
                        int count = datalen;
                        const UInt8 *send = outdata;
                        while (count) {
                            if (aux)
                                SendByte(true, 0xD4);
                            SendByte(false, *send);
                            send++;
                            count--;
                        }
                    }
                }, [=](UInt8 *data){
                    if (aux)
                        _mouse->ResetInput();
                    else
                        _keyboard->ResetInput();
                    response(data);
                }, responselen);
                KernelArray *array = aux ? _commandMouse : _commandKeyboard;
                bool start = array->Count() == 0;
                array->Add(commandItem);
                if (start) {
                    while (true) {
                        Command *current = (Command*)array->ObjectAt(0);
                        if ((current == NULL) || (!current->Run()))
                            break;
                        array->Remove(current);
                    }
                }
                commandItem->Release();
            });
        }
        
        RunloopThread* Runloop(void)
        {
            return _runloop;
        }
        
    protected:
        ~PS2Controller()
        {
            _runloop->Release();
        }
        
    private:
        PS2Keyboard *_keyboard;
        PS2Mouse *_mouse;
        InterruptHandlerHandle _interruptKeyboard, _interruptMouse;
        RunloopThread *_runloop;
        KernelArray *_commandKeyboard, *_commandMouse;
        
        static UInt8 ReadData(void) { return inb(0x60); }
        static UInt8 ReadStatus(void) { return inb(0x64); }
        static void SendByte(bool control, UInt8 command)
        {
            while (ReadStatus() & 0x02);
            outb(control ? 0x64 : 0x60, command);
        }
        static UInt8 ReadByte(bool *aux)
        {
            UInt8 status;
            while (!((status = ReadStatus()) & 0x01));
            if (aux)
                *aux = status & 0x20;
            return ReadData();
        }
        
        class Command : public KernelObject
        {
        public:
            Command(bicycle::function<void(void)> send, bicycle::function<void(UInt8*)> response, int expected)
            {
                _send = send;
                _response = response;
                _bufSize = 0;
                _bufNeed = expected;
            }
            
            bool Run(void)
            {
                _send();
                if (_bufNeed == 0) {
                    _response(NULL);
                    return true;
                }
                return false;
            }
            
            bool AddByte(UInt8 data)
            {
                if ((_bufSize == 0) && (data != 0xFA))
                    return false;
                _buf[_bufSize++] = data;
                if ((_bufSize == _bufNeed) || (_bufSize == sizeof(_buf))) {
                    _response(_buf);
                    return true;
                }
                return false;
            }
            
        private:
            bicycle::function<void(void)> _send;
            bicycle::function<void(UInt8*)> _response;
            UInt8 _buf[10];
            int _bufSize, _bufNeed;
        };
    };
    
    PS2Keyboard::PS2Keyboard()
    :GenericKeyboard("PS/2 keyboard")
    {
    }
    
    void PS2Keyboard::FeedByte(UInt8 c)
    {
        HandleKey(c);
    }

    void PS2Keyboard::ResetInput(void)
    {
    }
    
    PS2Mouse::PS2Mouse()
    :GenericMouse("PS/2 mouse")
    {
    }
    
    void PS2Mouse::Reset(bool retry)
    {
        if (retry) {
            _butans = 0;
            _received = 0;
            // Activate scroll wheel
            SetSampleRate(200, [=](bool success){});
            SetSampleRate(100, [=](bool success){});
            SetSampleRate(80, [=](bool success){});
            GetMouseID([=](UInt8 value){});
            // Activate buttons
            SetSampleRate(200, [=](bool success){});
            SetSampleRate(200, [=](bool success){});
            SetSampleRate(80, [=](bool success){});
            GetMouseID([=](UInt8 value){
                _mouseID = value;
                SetStreaming(true, [=](bool success){});
            });
        }
    }
    
    bool PS2Mouse::Start(Driver *parent)
    {
        if (!GenericMouse::Start(parent))
            return false;
        _received = 0;
        SendReset();
        return true;
    }

    void PS2Mouse::Stop(void)
    {
        Reset(false);
        GenericMouse::Stop();
    }

    void PS2Mouse::FeedByte(UInt8 c)
    {
        if (c == 0xAA) {
            Reset(true);
        } else {
            int packetLength = 3;
            if ((_mouseID == 3) || (_mouseID == 4))
                packetLength++;
            _buffer[_received++] = c;
            if (_received >= packetLength) {
                _received = 0;
                if (!(_buffer[0] & (0x80 | 0x40))) {
                    // Check mouse movement
                    if (_buffer[1] || _buffer[2]) {
                        UInt32 x = _buffer[1];
                        if (_buffer[0] & 0x10)
                            x |= 0xFFFFFF00;
                        UInt32 y = _buffer[2];
                        if (_buffer[0] & 0x20)
                            y |= 0xFFFFFF00;
                        Move(x, y);
                    }
                    // Check normal buttons
                    for (int i = 0; i < 3; i++) {
                        int butan = 1 << i;
                        if ((_butans & butan) ^ (_buffer[0] & butan)) {
                            bool set = _buffer[0] & butan;
                            if (set)
                                _butans |= butan;
                            else
                                _butans &= ~butan;
                            Button(i, set);
                        }
                    }
                    // Check extra buttons
                    if ((_mouseID == 3) || (_mouseID == 4)) {
                        for (int i = 0; i < 2; i++) {
                            int butan = 1 << (i + 4);
                            if ((_butans & butan) ^ (_buffer[3] & butan)) {
                                bool set = _buffer[3] & butan;
                                if (set)
                                    _butans |= butan;
                                else
                                    _butans &= ~butan;
                                Button(i + 3, set);
                            }
                        }
                    }
                    // Check scroll wheel
                    if (_mouseID == 4) {
                        switch (_buffer[3] & 0x0F) {
                            case 0x01:
                                Scroll(0, -1);
                                break;
                            case 0x0F:
                                Scroll(0, 1);
                                break;
                            case 0x02:
                                Scroll(1, 0);
                                break;
                            case 0x0E:
                                Scroll(-1, 0);
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
        }
    }
    
    void PS2Mouse::ResetInput(void)
    {
        _received = 0;
    }

    void PS2Mouse::SendReset(void)
    {
        PS2Controller *controller = (PS2Controller*)Parent();
        controller->SendCommand(true, 0xFF /* Reset */, NULL, 0, 0, [=](UInt8 *response) {});
    }
    
    void PS2Mouse::SetStreaming(bool enabled, bicycle::function<void(bool success)> onCompletion)
    {
        PS2Controller *controller = (PS2Controller*)Parent();
        controller->SendCommand(true, enabled ? 0xF4 : 0xF5, NULL, 0, 1, [=](UInt8 *response){
            onCompletion(response[0] == 0xFA);
        });
    }
    
    void PS2Mouse::SetSampleRate(UInt8 rate, bicycle::function<void(bool success)> onCompletion)
    {
        PS2Controller *controller = (PS2Controller*)Parent();
        controller->SendCommand(true, 0xF3 /* Set Sample Rate */, &rate, sizeof(rate), 2, [=](UInt8 *response){
            onCompletion(response && (response[0] == 0xFA) && (response[1] == 0xFA));
        });
    }
    
    void PS2Mouse::GetMouseID(bicycle::function<void(UInt8 result)> onCompletion)
    {
        PS2Controller *controller = (PS2Controller*)Parent();
        controller->SendCommand(true, 0xF2 /* Get MouseID */, NULL, 0, 2, [=](UInt8 *response){
            onCompletion((response && (response[0] == 0xFA)) ? response[1] : -1);
        });
    }
    
    void PS2Mouse::RequestSinglePacket(bicycle::function<void(UInt8 *data)> onCompletion)
    {
        PS2Controller *controller = (PS2Controller*)Parent();
        int packetLength = 3;
        if ((_mouseID == 3) || (_mouseID == 4))
            packetLength++;
        controller->SendCommand(true, 0xEB, NULL, 0, 1 + packetLength, [=](UInt8 *response){
            onCompletion((response[0] == 0xFA) ? (response + 1) : NULL);
        });
    }
    
    class Factory : public DriverFactory
    {
    public:
        CLASSNAME(DriverFactory, Factory);
        
    private:
        class Match_IDE : public DriverFactory::Match
        {
        public:
            CLASSNAME(DriverFactory::Match, StandardPC_Internal::Factory::Match_IDE);
            
            Match_IDE(UInt16 port, UInt16 irq)
            {
                _port = port;
                _irq = irq;
            }
            
            int MatchValue(void)
            {
                return 99999;
            }
            Driver* Instantiate(void)
            {
                return new LegacyATA(_port, _irq);
            }
            bool MatchMultiple(void)
            {
                return true;
            }
        private:
            UInt16 _port, _irq;
        };
        class Match_Timer : public DriverFactory::Match
        {
        public:
            CLASSNAME(DriverFactory::Match, StandardPC_Internal::Factory::Match_Timer);
            
            Match_Timer(){}
            
            int MatchValue(void)
            {
                return 1000000;
            }
            Driver* Instantiate(void)
            {
                return new Timer();
            }
            bool MatchMultiple(void)
            {
                return true;
            }
        };
        class Match_Keyboard : public DriverFactory::Match
        {
        public:
            CLASSNAME(DriverFactory::Match, StandardPC_Internal::Factory::Match_Keyboard);
            
            Match_Keyboard(){}
            
            int MatchValue(void)
            {
                return 1000000;
            }
            Driver* Instantiate(void)
            {
                return new PS2Controller();
            }
            bool MatchMultiple(void)
            {
                return true;
            }
        };
        class Match_PCI : public DriverFactory::Match
        {
        public:
            CLASSNAME(DriverFactory::Match, StandardPC_Internal::Factory::Match_PCI);
            
            Match_PCI(){}
            
            int MatchValue(void)
            {
                return 1000000;
            }
            Driver* Instantiate(void)
            {
                return new PCI::Root();
            }
            bool MatchMultiple(void)
            {
                return true;
            }
        };
    public:
        KernelArray* MatchForParent(Driver *parent)
        {
            KernelString *propertyBus = (KernelString*)parent->PropertyFor(kDriver_Property_Bus);
            if (propertyBus == NULL)
                return NULL;
            if (!propertyBus->IsEqualTo(kDriver_Bus_System))
                return NULL;
            KernelArray *result = new KernelArray();
            // Add timer
            Match_Timer *timer = new Match_Timer();
            result->Add(timer);
            timer->Release();
            // Add keyboard
            Match_Keyboard *keyboard = new Match_Keyboard();
            result->Add(keyboard);
            keyboard->Release();
            // Try PCI?
            if (PCI::Available()) {
                // Add PCI device
                Match_PCI *pci = new Match_PCI();
                result->Add(pci);
                pci->Release();
            } else {
                // Add legacy (non-PCI) devices that we would otherwise have found via PCI
                Match_IDE *primary = new Match_IDE(0x1F0, 14);
                result->Add(primary);
                primary->Release();
                Match_IDE *secondary = new Match_IDE(0x170, 15);
                result->Add(secondary);
                secondary->Release();
            }
            result->Autorelease();
            return result;
        }
        
        static void Install(void)
        {
            Factory *that = new Factory();
            Driver::RegisterFactory(that);
            that->Release();
        }
    };

}

static StandardPC_Internal::Interrupts s_interrupts;
static CPU s_CPUs[1];

// Assembly code entry point for all interrupts
extern "C" void trap(TrapFrame *frame)
{
    s_interrupts.Trigger(frame);
}

static void CPUMain(void)
{
    // IDT
    s_interrupts.Set();
    // tell everyone ready
    // start scheduler
    CPU_Interrupt_Enable();
}

static void AlternateCPU(void)
{
    // Switch page tables
    // Alternate CPU segments
    // Alternate CPU LAPIC
    // Main
    CPUMain();
}

#include "Process.h"
#include "Thread.h"

static void PrintStack(void **stackBottom, void **stackTop)
{
    kprintf("Stack [ESP %.8x, top %.8x]:", (int)stackBottom, (int)stackTop);
    if (stackTop < stackBottom)
        stackTop = stackBottom + 24;
    for (void **s = stackBottom; s < stackTop; s++) {
        void *addr = *s;
        ElfSymbols::Symbol *sym = s_symbols->Find(addr);
        if (sym)
            kprintf("\n %.8x %s+%i", addr, sym->Name()->CString(), (int)sym->Offset(addr));
        else
            kprintf(" %.8x", addr);
    }
    kprintf("\n");
}

static void PrintRegisters(TrapFrame *tf)
{
    void* esp = ((UInt32*)tf)+14;
    kprintf("EAX=%.8x EBX=%.8x ECX=%.8x EDX=%.8x\n", tf->EAX, tf->EBX, tf->ECX, tf->EDX);
    kprintf("EDI=%.8x ESI=%.8x EBP=%.8x EIP=%.8x\n", tf->EDI, tf->ESI, tf->EBP, tf->EIP);
    ElfSymbols::Symbol *sym = s_symbols->Find((void*)tf->EIP);
    if (sym)
        kprintf("Crash in %s+%i\n", sym->Name()->CString(), (int)sym->Offset((void*)tf->EIP));
    PrintStack((void**)(Thread::Active->IsCurrentlyUserspace() ? (void*)tf->ESP : esp), (Thread::Active ? (void**)Thread::Active->StackTop(Thread::Active->IsCurrentlyUserspace()) : (((void**)esp) + 24)));
    kprintf("CPU %.8x Process %.8x Thread %.8x %s\n", CPU::Active, Process::Active, Thread::Active, Thread::Active ? (Thread::Active->IsUserspace() ? "[userspace]" : "[kernel]") : "");
}

static void HandleUserspaceException(void *state, int number)
{
    TrapFrame *tf = (TrapFrame*)state;
    kprintf("Process %.8x crashed (%.2x: %s)\n", Process::Active, number, StandardPC::NameForTrap(number));
    PrintRegisters(tf);
    Thread *start = Thread::ThreadCursor();
    Thread *current = start;
    do {
        if (current->_process == Process::Active)
            current->_state = tsCompleted;
        Thread::ThreadNext();
        current = Thread::ThreadCursor();
    } while (current != start);
    Scheduler::EnterFromInterrupt();
}

static bool EndFailHandler(TrapFrame *tf)
{
    PrintRegisters(tf);
    while(1) asm("hlt");
    return true;
}

static bool GenericExceptionHandler(void *context, void *state)
{
    CPU_Interrupt_Disable();
    TrapFrame *tf = (TrapFrame*)state;
    if ((tf->CS & DPL_USER) == DPL_USER) {
        HandleUserspaceException(state, tf->TrapNumber);
        return true;
    }
    Console_Panic();
    kprintf("\nPANIC! CPU exception %.2x: %s\n", tf->TrapNumber, StandardPC::NameForTrap(tf->TrapNumber));
    kprintf("Error %.8x; address %.4x:%.8x\n", tf->ERR, tf->CS, tf->EIP);
#ifdef DUMP_THREADS
    Thread::DebugPrint();
#endif
    return EndFailHandler(tf);
}

static bool PageFaultExceptionHandler(void *context, void *state)
{
    CPU_Interrupt_Disable();
    TrapFrame *tf = (TrapFrame*)state;
    UInt32 faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
    Console_Panic();
    kprintf("\nPANIC! CPU exception %.2x: %s\n", tf->TrapNumber, StandardPC::NameForTrap(tf->TrapNumber));
    kprintf("Error %.8x; address %.4x:%.8x\n", tf->ERR, tf->CS, tf->EIP);
    kprintf("Attempting to access %.8x\n", faulting_address);
    if ((tf->CS & DPL_USER) == DPL_USER) {
        HandleUserspaceException(state, tf->TrapNumber);
        return true;
    }
    return EndFailHandler(tf);
}

StandardPC::StandardPC()
:Driver("root")
{
    // TODO: Make these separate drivers
    for (int i = 0; i < 0x20; i++)
        if (i != 0x0E)
            InterruptSource()->RegisterHandler(i, GenericExceptionHandler, NULL);
    InterruptSource()->RegisterHandler(0x0E, PageFaultExceptionHandler, NULL);
}

StandardPC::~StandardPC()
{
}

bool StandardPC::Start(Driver *parent)
{
    // Multiprocessor
    // Main CPU LAPIC
    // Main CPU Segments
    s_CPUs[0].InitSegments();
    // PIC
    CPU_PIC_Init(PIC_IRQ_OFFSET);
    // IOAPIC
    // Begin Main CPU
    CPUMain();
    // Configure driver infrastructure and start
    AutoreleasePool pool;
    StandardPC_Internal::Factory::Install();
    ATADriver::Install();
    VirtualMemory::ConfigureService(InterruptSource());
    SetProperty(kDriver_Property_Bus, kDriver_Bus_System);
    return Driver::Start(parent);
}

void StandardPC::Stop(void)
{
    _ataSecondary->Stop();
    DetachChild(_ataSecondary);
    _ataSecondary->Release();
    _ataPrimary->Stop();
    DetachChild(_ataPrimary);
    _ataPrimary->Release();
    Driver::Stop();
}

Interrupts* StandardPC::InterruptSource(void)
{
    return &s_interrupts;
}

CPU* StandardPC::GetCPU(int index)
{
    // TODO: multiprocessor
    if (index != 0)
        return NULL;
    return s_CPUs + index;
}

void StandardPC::ContextSwitching::Initialise(void)
{
#ifdef FPU_LAZYISH
    s_interrupts.RegisterHandler(peCoprocessorAbsent, [=](void *state)->bool{
        if (!Thread::Active)
            return false;
        UInt32 value;
        asm volatile("mov %%cr0, %0" : "=r" (value));
        value &= ~0x08;
        asm volatile("mov %0, %%cr0" :: "r" (value));
        if (Thread::Active->_fpuContext.active) {
            Thread::Active->_fpuContext.Restore();
        } else {
            Thread::Active->_fpuContext.active = true;
            asm volatile("fninit");
        }
        return true;
    });
#endif // FPU_LAZYISH
}

void StandardPC::ContextSwitching::EnteringThread(Thread *thread)
{
#ifdef FPU_LAZYISH
    UInt32 value;
    asm volatile("mov %%cr0, %0" : "=r" (value));
    if (thread && CPU::Active->lastFPUUser == thread->Hash())
        value &= ~0x08;
    else
        value |= 0x08;
    asm volatile("mov %0, %%cr0" :: "r" (value));
#else // FPU_LAZYISH
    thread->_fpuContext.Restore();
#endif // FPU_LAZYISH
}

void StandardPC::ContextSwitching::ExitingThread(Thread *thread)
{
#ifdef FPU_LAZYISH
    if (thread && thread->_fpuContext.active) {
        thread->_fpuContext.Store();
        CPU::Active->lastFPUUser = thread->Hash();
    }
#else //FPU_LAZYISH
    thread->_fpuContext.Store();
#endif // FPU_LAZYISH
}

void SystemService::Register(void)
{
    int irq = Interrupt();
    _handle = s_interrupts.RegisterHandler(irq, [=](void *state)->bool{
        AutoreleasePool pool;
        TrapFrame *frame = (TrapFrame*)state;
        UInt64 data[] = {frame->EAX, frame->EBX, frame->ECX, frame->EDX, frame->EDI, frame->ESI};
        Handle(data);
        frame->EAX = data[0]; frame->EBX = data[1]; frame->ECX = data[2]; frame->EDX = data[3]; frame->EDI = data[4]; frame->ESI = data[5];
        return true;
    });
    s_interrupts.ConfigureSyscall(irq);
}

void SystemService::Unregister(void)
{
    s_interrupts.UnregisterHandler(_handle);
    _handle = NULL;
}
