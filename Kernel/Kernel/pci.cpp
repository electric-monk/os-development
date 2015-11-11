#include "runtime.h"
#include "tools.h"
#include "pci.h"
#include "debug.h"
#include "Driver.h"
#include "Collections.h"
#include "Interrupts.h"
#include "StandardPC.h"

#define CONFIG_ADDRESS      0xCF8
#define CONFIG_DATA         0xCFC

namespace PCI {
    static UInt32 Address(UInt8 bus, UInt8 slot, UInt8 function, UInt8 reg)
    {
        return (UInt32(bus) << 16) | (UInt32(slot) << 11) | (UInt32(function) << 8) | (UInt32(reg) & 0xFC) | 0x80000000;
    }
    
    static UInt32 ReadLong(UInt8 bus, UInt8 slot, UInt8 function, UInt8 reg)
    {
        outl(CONFIG_ADDRESS, Address(bus, slot, function, reg));
        return inl(CONFIG_DATA);
    }
    
    static void WriteLong(UInt8 bus, UInt8 slot, UInt8 function, UInt8 reg, UInt32 value)
    {
        outl(CONFIG_ADDRESS, Address(bus, slot, function, reg));
        outl(CONFIG_DATA, value);
    }
    
    // Convenience accessors
    static UInt16 ReadVendorID(UInt8 bus, UInt8 slot, UInt8 function)
    {
        UInt32 value = ReadLong(bus, slot, function, 0x00);
        return value & 0xFFFF;
    }
    static UInt16 ReadDeviceID(UInt8 bus, UInt8 slot, UInt8 function)
    {
        UInt32 value = ReadLong(bus, slot, function, 0x00);
        return (value & 0xFFFF0000) >> 16;
    }
    static UInt8 ReadHeaderType(UInt8 bus, UInt8 slot, UInt8 function)
    {
        UInt32 value = ReadLong(bus, slot, function, 0x0C);
        return (value & 0x00FF0000) >> 16;
    }
    static UInt8 ReadBaseClass(UInt8 bus, UInt8 slot, UInt8 function)
    {
        UInt32 value = ReadLong(bus, slot, function, 0x08);
        return (value & 0xFF000000) >> 24;
    }
    static UInt8 ReadSubClass(UInt8 bus, UInt8 slot, UInt8 function)
    {
        UInt32 value = ReadLong(bus, slot, function, 0x08);
        return (value & 0x00FF0000) >> 16;
    }
    static UInt8 ReadSecondaryBus(UInt8 bus, UInt8 slot, UInt8 function)
    {
        UInt32 value = ReadLong(bus, slot, function, 0x18);
        return (value & 0x0000FF00) >> 8;
    }
    
    class Controller : public Device
    {
    public:
        CLASSNAME(Device, PCI::Controller);
        
        Controller(const char *name, UInt8 bus)
        :Device(name, bus, 0, 0)
        {
        }
        
        void CheckFunction(UInt8 device, UInt8 function)
        {
            Device *newDevice;
            if ((ReadBaseClass(Bus(), device, function) == 0x06) && (ReadSubClass(Bus(), device, function) == 0x04)) {
                newDevice = new Controller("PCI to PCI bridge", function);
            } else {
                newDevice = new Device("PCI device", Bus(), device, function);
            }
            ConnectChild(newDevice);
            newDevice->Release();
            if (!newDevice->Start(this))
                DetachChild(newDevice);
        }
        
        void CheckDevice(UInt8 device)
        {
            UInt16 vendorID = PCI::ReadVendorID(Bus(), device, 0);
            if (vendorID == 0xFFFF) // Not present
                return;
            CheckFunction(device, 0);
            if (PCI::ReadHeaderType(Bus(), device, 0) & 0x80) {
                // Multi-function device
                for (UInt8 function = 1; function < 8; function++)
                    if (PCI::ReadVendorID(Bus(), device, function) != 0xFFFF)
                        CheckFunction(device, function);
            }
        }
        
        bool Start(Driver *parent)
        {
            for (UInt8 device = 0; device < 32; device++)
                CheckDevice(device);
            return Device::Start(parent);
        }
    };
    
    bool Available(void)
    {
        return PCI::ReadVendorID(0, 0, 0) != 0xFFFF;
    }
}

PCI::Device::Device(const char *name, UInt8 bus, UInt8 device, UInt8 function)
:Driver(name)
{
    KernelNumber *number;
    
    _bus = bus;
    _device = device;
    _function = function;
    SetProperty(kDriver_Property_Bus, kDriver_Bus_PCI);
    number = new KernelNumber(PCI::ReadVendorID(bus, device, function));
    SetProperty(kDriver_Property_PCI_ManufacturerID, number);
    number->Release();
    number = new KernelNumber(PCI::ReadDeviceID(bus, device, function));
    SetProperty(kDriver_Property_PCI_ProductID, number);
    number->Release();
    number = new KernelNumber(PCI::ReadBaseClass(bus, device, function));
    SetProperty(kDriver_Property_PCI_Class, number);
    number->Release();
    number = new KernelNumber(PCI::ReadSubClass(bus, device, function));
    SetProperty(kDriver_Property_PCI_Subclass, number);
    number->Release();
}

UInt16 PCI::Device::VendorID(void)
{
    return (ReadPCIRegister(0x00) & 0xFFFF0000) >> 16;
}
UInt16 PCI::Device::DeviceID(void)
{
    return ReadPCIRegister(0x00) & 0xFFFF;
}
UInt8 PCI::Device::HeaderType(void)
{
    return (ReadPCIRegister(0x0C) & 0x00FF0000) >> 16;
}
UInt8 PCI::Device::BaseClass(void)
{
    return (ReadPCIRegister(0x08) & 0xFF000000) >> 24;
}
UInt8 PCI::Device::SubClass(void)
{
    return (ReadPCIRegister(0x08) & 0x00FF0000) >> 16;
}
UInt32 PCI::Device::ReadBAR(UInt8 bar)
{
    if (bar < 6)
        return ReadPCIRegister(0x10 + (bar * sizeof(UInt32)));
    return 0;
}

UInt32 PCI::Device::ReadPCIRegister(UInt8 reg)
{
    return ReadLong(_bus, _device, _function, reg);
}
void PCI::Device::WritePCIRegister(UInt8 reg, UInt32 value)
{
    WriteLong(_bus, _device, _function, reg, value);
}

class Interrupts_PCI : public Interrupts
{
public:
    CLASSNAME(Interrupts, Interrupts_PCI);
    
    Interrupts_PCI(Interrupts *original, PCI::Device *owner)
    {
        _interrupts = original;
        _interrupts->AddRef();
        _owner = owner;
        _owner->AddRef();
    }

    InterruptHandlerHandle RegisterHandler(int irq, InterruptHandlerBlock handler)
    {
        // We ignore the IRQ and use the correct one
        UInt32 interruptState = _owner->ReadPCIRegister(0x3C);
        return _interrupts->RegisterHandler(PIC_IRQ_OFFSET + (interruptState & 0xFF), handler);
    }
    
    void UnregisterHandler(InterruptHandlerHandle handler)
    {
        _interrupts->UnregisterHandler(handler);
    }
    
    void ConfigureSyscall(int irq)
    {
        // Doesn't make sense for PCI
    }

protected:
    ~Interrupts_PCI()
    {
        _interrupts->Release();
        _owner->Release();
    }
    
private:
    Interrupts *_interrupts;
    PCI::Device *_owner;
};

Interrupts* PCI::Device::InterruptSource(void)
{
    // Return an interrupt source designed for PCI functionality
    Interrupts_PCI *pciInt = new Interrupts_PCI(Driver::InterruptSource(), this);
    pciInt->Autorelease();
    return pciInt;
}

PCI::Root::Root()
:Driver("PCI Root")
{
}

bool PCI::Root::Start(Driver *parent)
{
    UInt8 headerType = PCI::ReadHeaderType(0, 0, 0);
    UInt8 maximumFunction = 1;
    if (headerType & 0x80)
        maximumFunction = 8;
    for (UInt8 function = 0; function < maximumFunction; function++) {
        if (PCI::ReadVendorID(0, 0, function) != 0xFFFF) {
            PCI::Controller *controller = new PCI::Controller("PCI Controller", function);
            ConnectChild(controller);
            controller->Release();
            if (!controller->Start(this))
                DetachChild(controller);
        }
    }
    return Driver::Start(parent);
}
