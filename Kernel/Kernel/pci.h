void CheckAllBuses(void);
#ifndef __PCI_H__
#define __PCI_H__

#include "Driver.h"

namespace PCI {
    class Device : public Driver
    {
    public:
        CLASSNAME(Driver, PCI::Device);
        
        Device(const char *name, UInt8 bus, UInt8 device, UInt8 function);
        
        UInt8 Bus(void) { return _bus; }
        UInt8 PCIDevice(void) { return _device; }
        UInt8 Function(void) { return _function; }

        UInt16 VendorID(void);
        UInt16 DeviceID(void);
        UInt8 HeaderType(void);
        UInt8 BaseClass(void);
        UInt8 SubClass(void);
        UInt32 ReadBAR(UInt8 bar);
        
        UInt32 ReadPCIRegister(UInt8 reg);
        void WritePCIRegister(UInt8 reg, UInt32 value);
        
        Interrupts* InterruptSource(void);
    private:
        UInt8 _bus, _device, _function;
    };
    
    class Root : public Driver
    {
    public:
        CLASSNAME(Driver, PCI::Root);
        
        Root();
        
        bool Start(Driver *parent);
    };
    
    bool Available(void);
    
    namespace BAR {
        inline bool IsIOMapped(UInt32 bar)
        {
            return bar & 0x01;
        }
        inline bool IsMemoryMapped(UInt32 bar)
        {
            return !IsIOMapped(bar);
        }
        inline UInt16 GetIOAddress(UInt32 bar)
        {
            if (IsIOMapped(bar))
                return bar & ~0x03;
            return 0xFFFF;
        }
        inline void* GetMemoryAddress(UInt32 bar)
        {
            if (IsMemoryMapped(bar))
                return (void*)(bar & ~0x0F);
            return NULL;
        }
    }
}

#endif // __PCI_H__
