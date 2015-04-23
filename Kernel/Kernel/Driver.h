#ifndef __DRIVER_H__
#define __DRIVER_H__

#include "KernelObject.h"

class Interrupts;
class Driver;
class KernelDictionary;

#define kDriver_Property_Name               "name"_ko
#define kDriver_Property_Bus                "bus"_ko

#define kDriver_Bus_System                  "bus.system"_ko
#define kDriver_Bus_PCI                     "bus.pci"_ko

#define kDriver_Property_PCI_ManufacturerID "pci.manufacturer"_ko
#define kDriver_Property_PCI_ProductID      "pci.product"_ko

//#define kDriver_Processor_Interrupts        KS("x86.interrupt")
//#define kDriver_PCI_Interrupts              KS("pci.interrupt")

// A base driver factory - this will return an array of matches to try
class DriverFactory : public KernelObject
{
public:
    class Match : public KernelObject
    {
    public:
        virtual int MatchValue(void) = 0;
        virtual Driver* Instantiate(void) = 0;
        virtual bool MatchMultiple(void) { return false; }  // Allow drivers to match multiple children simultaneously
    };
    
    virtual KernelArray* MatchForParent(Driver *parent) = 0;
};

// A base driver implementation
class Driver : public KernelObject
{
public:
    static void RegisterFactory(DriverFactory *factory);
    static void UnregisterFactory(DriverFactory *factory);
    
    Driver(const char *name);
    
    virtual bool Start(Driver *parent);   // Trigger matching of child drivers, so only call if you're successful
    virtual void Stop(void);
    
    Driver* Parent(void) { return _parent; }
    Driver* Child(int index);
Interrupts* Test(void){return InterruptSource();}
    
    KernelArray* PropertyList(void);
    KernelObject* PropertyFor(KernelObject *property);
protected:
    ~Driver();
    
    virtual bool ConnectChild(Driver *child);
    virtual void DetachChild(Driver *child);

    virtual Interrupts* InterruptSource(void);
    
    void SetProperty(KernelObject *property, KernelObject *value);
    
private:
    KernelDictionary *_properties;
    Driver *_start, *_end;          // Children
    Driver *_previous, *_next;      // Siblings
    Driver *_parent;                // Parent
};

#endif // __DRIVER_H__
