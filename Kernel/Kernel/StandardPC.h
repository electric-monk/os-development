#ifndef __STANDARDPC_H__
#define __STANDARDPC_H__

#include "Driver.h"

class CPU;

#define PIC_IRQ_OFFSET      0x20

// System interrupts
typedef enum {  /* 0x00 to 0x1F */
    peDivisionByZero    = 0x00,
    peDebugger          = 0x01,
    peNMI               = 0x02,
    peBreakpoint        = 0x03,
    peOverflow          = 0x04,
    peBounds            = 0x05,
    peInvalidOpcode     = 0x06,
    peCoprocessorAbsent = 0x07,
    peDoubleFault       = 0x08,
    peCoprocessorOverrun= 0x09,
    peInvalidTSS        = 0x0A,
    peSegmentAbsent     = 0x0B,
    peStackFault        = 0x0C,
    peGeneralProtection = 0x0D,
    pePageFault         = 0x0E,
    /* reserved         = 0x0F, */
    peMathFault         = 0x10,
    peAlignmentCheck    = 0x11,
    peMachineCheck      = 0x12,
    peFloatingPoint     = 0x13,
} ProcessorExceptions;

class StandardPC : public Driver
{
public:
    StandardPC();
    
    bool Start(Driver *parent);
    void Stop(void);
    
    CPU* GetCPU(int index);
    
    static const char* NameForTrap(unsigned int trap);
    
protected:
    ~StandardPC();
    
    Interrupts* InterruptSource(void);
};

void StartScheduling(void);

#endif // __STANDARDPC_H__
