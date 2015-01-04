#ifndef __STANDARDPC_TRAPS_H__
#define __STANDARDPC_TRAPS_H__

#include "runtime.h"

#define CONSTANT(name, val)         static const int name = val;
#include "StandardPC_traps_shared.inc"
#define SEGMENT_COUNT               7

// Eflags register
#define FL_CF           0x00000001      // Carry Flag
#define FL_PF           0x00000004      // Parity Flag
#define FL_AF           0x00000010      // Auxiliary carry Flag
#define FL_ZF           0x00000040      // Zero Flag
#define FL_SF           0x00000080      // Sign Flag
#define FL_TF           0x00000100      // Trap Flag
#define FL_IF           0x00000200      // Interrupt Enable
#define FL_DF           0x00000400      // Direction Flag
#define FL_OF           0x00000800      // Overflow Flag
#define FL_IOPL_MASK    0x00003000      // I/O Privilege Level bitmask
#define FL_IOPL_0       0x00000000      //   IOPL == 0
#define FL_IOPL_1       0x00001000      //   IOPL == 1
#define FL_IOPL_2       0x00002000      //   IOPL == 2
#define FL_IOPL_3       0x00003000      //   IOPL == 3
#define FL_NT           0x00004000      // Nested Task
#define FL_RF           0x00010000      // Resume Flag
#define FL_VM           0x00020000      // Virtual 8086 mode
#define FL_AC           0x00040000      // Alignment Check
#define FL_VIF          0x00080000      // Virtual Interrupt Flag
#define FL_VIP          0x00100000      // Virtual Interrupt Pending
#define FL_ID           0x00200000      // ID flag

// The structure passed in from the assembly
typedef struct {
    // Registers pushed
    UInt32 EDI, ESI, EBP, oesp, EBX, EDX, ECX, EAX; // Apparently oesp is nonsense.
    
    // Trap frame
    UInt16 GS, padding0, FS, padding1, ES, padding2, DS, padding3;
    UInt32 TrapNumber;
    
    // Hardware
    UInt32 ERR, EIP;
    UInt16 CS, padding4;
    UInt32 EFlags;
    
    // Only used for crossing rings
    UInt32 ESP;
    UInt16 SS, padding5;
} TrapFrame;

// The vector array from the assembly
extern void *vectors; // This is the first vector - (&vectors)[1] will be second, etc.

#endif // __STANDARDPC_TRAPS_H__
