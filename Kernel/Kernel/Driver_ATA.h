#ifndef __DRIVER_ATA_H__
#define __DRIVER_ATA_H__

#include "Driver.h"

class SignalOr;
class BlockableObject;
class Timer;
class DMABuffer;
class ATADriverDrive;
class ATADriverDrive_ATAPI;
class ConfigureATA;
class ATADriverDrive_ATA;
class DispatchQueue;
class IpcServiceList;

#define kDriver_Bus_SystemATA      "provider.ata"_ko

// A class a driver can provide to give the ATA driver a place to connect (e.g. PCI)
class ATADriverNode : public Driver
{
public:
    CLASSNAME(Driver, ATADriverNode);
    
    ATADriverNode(const char *name):Driver(name){}
    
    virtual UInt8 inByte(UInt32 address) = 0;
    virtual UInt16 inShort(UInt32 address) = 0;
    virtual UInt32 inLong(UInt32 address) = 0;
    virtual void outByte(UInt32 address, UInt8 byte) = 0;
    virtual void outShort(UInt32 address, UInt16 byte) = 0;
    virtual void outLong(UInt32 address, UInt32 byte) = 0;
    
    virtual void inByteRep(UInt32 address, void *buffer, UInt32 length) = 0;
    virtual void inShortRep(UInt32 address, void *buffer, UInt32 length) = 0;
    virtual void inLongRep(UInt32 address, void *buffer, UInt32 length) = 0;
    virtual void outByteRep(UInt32 address, void *buffer, UInt32 length) = 0;
    virtual void outShortRep(UInt32 address, void *buffer, UInt32 length) = 0;
    virtual void outLongRep(UInt32 address, void *buffer, UInt32 length) = 0;
    
    virtual BlockableObject* Interrupt(void) = 0;
    virtual void ResetInterrupt(void) = 0;
    UInt8 InterruptStatus(void) { return _interruptStatus; }
    UInt8 BMIDEStatus(void) { return _bmStatus; }
    void UpdateBMIDEState(void);

    virtual bool DMAAvailable(void) = 0;
    virtual UInt8 readBusMasterStatus(void) = 0;
    virtual void writeBusMasterStatus(UInt8 x) = 0;
    
protected:
    bool UpdateForInterrupt(void);
    
private:
    UInt8 _interruptStatus;
    UInt8 _bmStatus;
};

// The actual driver
class ATADriver : public Driver
{
public:
    CLASSNAME(Driver, ATADriver);
    
    static void Install(void);
    
    bool Start(Driver *parent);
    void Stop(void);
    
public:
    typedef enum {
        cmdCFA_ERASE_SECTORS               = 0xC0,
        cmdCFA_REQUEST_EXT_ERR_CODE        = 0x03,
        cmdCFA_TRANSLATE_SECTOR            = 0x87,
        cmdCFA_WRITE_MULTIPLE_WO_ERASE     = 0xCD,
        cmdCFA_WRITE_SECTORS_WO_ERASE      = 0x38,
        cmdCHECK_POWER_MODE1               = 0xE5,
        cmdCHECK_POWER_MODE2               = 0x98,
        cmdDEVICE_RESET                    = 0x08,
        cmdEXECUTE_DEVICE_DIAGNOSTIC       = 0x90,
        cmdFLUSH_CACHE                     = 0xE7,
        cmdFLUSH_CACHE_EXT                 = 0xEA,
        cmdFORMAT_TRACK                    = 0x50,
        cmdIDENTIFY_DEVICE                 = 0xEC,
        cmdIDENTIFY_DEVICE_PACKET          = 0xA1,
        cmdIDENTIFY_PACKET_DEVICE          = 0xA1,
        cmdIDLE1                           = 0xE3,
        cmdIDLE2                           = 0x97,
        cmdIDLE_IMMEDIATE1                 = 0xE1,
        cmdIDLE_IMMEDIATE2                 = 0x95,
        cmdINITIALIZE_DRIVE_PARAMETERS     = 0x91,
        cmdINITIALIZE_DEVICE_PARAMETERS    = 0x91,
        cmdNOP                             = 0x00,
        cmdPACKET                          = 0xA0,
        cmdREAD_BUFFER                     = 0xE4,
        cmdREAD_DMA                        = 0xC8,
        cmdREAD_DMA_EXT                    = 0x25,
        cmdREAD_DMA_QUEUED                 = 0xC7,
        cmdREAD_DMA_QUEUED_EXT             = 0x26,
        cmdREAD_MULTIPLE                   = 0xC4,
        cmdREAD_MULTIPLE_EXT               = 0x29,
        cmdREAD_SECTORS                    = 0x20,
        cmdREAD_SECTORS_EXT                = 0x24,
        cmdREAD_VERIFY_SECTORS             = 0x40,
        cmdREAD_VERIFY_SECTORS_EXT         = 0x42,
        cmdRECALIBRATE                     = 0x10,
        cmdSEEK                            = 0x70,
        cmdSET_FEATURES                    = 0xEF,
        cmdSET_MULTIPLE_MODE               = 0xC6,
        cmdSLEEP1                          = 0xE6,
        cmdSLEEP2                          = 0x99,
        cmdSMART                           = 0xB0,
        cmdSTANDBY1                        = 0xE2,
        cmdSTANDBY2                        = 0x96,
        cmdSTANDBY_IMMEDIATE1              = 0xE0,
        cmdSTANDBY_IMMEDIATE2              = 0x94,
        cmdWRITE_BUFFER                    = 0xE8,
        cmdWRITE_DMA                       = 0xCA,
        cmdWRITE_DMA_EXT                   = 0x35,
        cmdWRITE_DMA_QUEUED                = 0xCC,
        cmdWRITE_DMA_QUEUED_EXT            = 0x36,
        cmdWRITE_MULTIPLE                  = 0xC5,
        cmdWRITE_MULTIPLE_EXT              = 0x39,
        cmdWRITE_SECTORS                   = 0x30,
        cmdWRITE_SECTORS_EXT               = 0x34,
        cmdWRITE_VERIFY                    = 0x3C,
    } COMMAND;
    typedef enum {
        lbaCHS = 0, // unsupported
        lba28 = 28,
        lba48 = 48,
    } LBA_SIZE;
    
    typedef enum {
        dcNone = 0,
        dcUnknown = 1,
        dcPATA = 2,
        dcPATAPI = 4,
        dcSATA = 2 + 8,
        dcSATAPI = 4 + 8,
    } DEV_CONFIG;
    
#define IS_ATA(x)       ((x) & 2)
#define IS_ATAPI(x)     ((x) & 4)
    
    typedef union {
        UInt8 raw;
        struct {
            bool check :1;
            bool index :1;  // obsolete
            bool corrected :1;  // obsolete
            bool dataRequest :1;
            bool seekComplete :1;   // or service
            bool deviceFault :1;
            bool ready :1;
            bool busy :1;
        };
    } STAT_REG;
    
    typedef struct {
        // command code
        COMMAND command;
        // command parameters
        UInt32 feature;
        UInt32 secCount;
        UInt32 secNum;
        UInt32 cylLow;
        UInt32 cylHigh;
        UInt8 devHead;
        UInt8 devControl;
        UInt32 sectorCount;
        UInt32 currentMultipleBlock;
        UInt8 lbaSize;
        UInt32 lbaLow;
        UInt32 lbaHigh;
        // status and error regs
        STAT_REG regStatus;
        STAT_REG regAltStatus;
        union {
            UInt8 raw;
            struct {
                bool ataAddressMarkNotFound :1;
                bool ataTrack0NotFound :1;
                bool ataCommandAbort :1;
                bool ataMediaChangeRequest :1;
                bool ataIdNotFound :1;
                bool ataMediaChange :1;
                bool ataUncorrectedError :1;
                bool ataBadBlockOrCRC :1;
            };
            struct {
                bool atapiIllegalLengthIndication :1;
                bool atapiEndOfMedia :1;
                bool atapiCommandAbort :1;
                bool atapiMediaChange :1;
                UInt8 atapiSenseKey :4;
            };
        } regError;
        // driver error
        UInt8 errorCode;
        bool timeOutError;
        // additional result info
        UInt32 totalBytes;
        UInt32 drqPackets;
    } REG_CMD_INFO;
    
    ATADriver();
    
protected:
    ~ATADriver();
    
private:
    friend ConfigureATA;
    
    bool _useInterrupts;
    DMABuffer *_dmaBuffer;
    
    Timer *_timer;
    SignalOr *_waitObject;
    
    IpcServiceList *_serviceList;
    DispatchQueue *_mainQueue;
    ATADriverDrive *_driveHandlers[2];
    
    UInt8 _pioTransferWidth;
    
    REG_CMD_INFO _regCommandInfo;
    
    DEV_CONFIG _configInfo[2];
    
    ATADriverNode* IOPort(void) { return (ATADriverNode*)Parent(); }
    
    // OS support
    void StartTimer(void);
    
    // ATA[PI] API
    friend ATADriverDrive_ATAPI;
    friend ATADriverDrive_ATA;
    int Configure(void);
    bool Reset(UInt32 deviceReturn);
    bool NonDataCommandLBA28(UInt8 dev, COMMAND command, UInt32 sector, UInt32 lba);
    bool NonDataCommandLBA48(UInt8 dev, COMMAND command, UInt32 sector, UInt64 lba);
    bool DataCommandLBA28In(UInt8 device, COMMAND command, UInt32 lba, void *buffer, UInt32 numSectors, UInt32 multiCnt);
    bool DataCommandLBA48In(UInt8 device, COMMAND command, UInt64 lba, void *buffer, UInt32 numSectors, UInt32 multiCnt);
    bool DataCommandLBA28Out(UInt8 device, COMMAND command, UInt32 lba, void *buffer, UInt32 numSectors, UInt32 multiCnt);
    bool DataCommandLBA48Out(UInt8 device, COMMAND command, UInt64 lba, void *buffer, UInt32 numSectors, UInt32 multiCnt);
    bool Packet(UInt8 device, char *controlBuffer, UInt32 controlBufferLength, bool directionOut, void *dataBuffer, UInt32 dataBufferLength);
    
    // Internal gubbins of the internal API
    bool _ExecNonDataCommand(UInt32 device);
    bool _ExecDataCommandIn(UInt32 device, UInt8 *buffer, UInt32 numSectors, UInt32 multiCnt);
    bool _ExecDataCommandOut(UInt32 device, UInt8 *buffer, UInt32 numSectors, UInt32 multiCnt);
    bool _Configure_InitDevice(UInt8 device);
    DEV_CONFIG _Configure_CheckDevice(UInt8 device);
    bool _Select(UInt8 device);
    void _SetupCommand(void);
    void _Wait(UInt8 we, UInt8 pe);
    bool _CompleteCommand(void);
    void _drqBlockOut(UInt8 addressDataRegister, UInt8 *buffer, UInt32 wordCount);
    void _drqBlockIn(UInt8 addressDataRegister, UInt8 *buffer, UInt32 wordCount);
};

#endif // __DRIVER_ATA_H__
