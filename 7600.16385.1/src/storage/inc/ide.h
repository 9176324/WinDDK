/*++

This is retained to support the old mini ide drivers.

--*/
#pragma once

#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning(disable:4214) // bit field types other than int

#define MAX_IDE_CHANNEL 2
#define MAX_IDE_DEVICE  2
#define MAX_IDE_LUN     8
#define MAX_IDE_LINE    2

#define PIO_MODE0           (1 << 0)
#define PIO_MODE1           (1 << 1)
#define PIO_MODE2           (1 << 2)
#define PIO_MODE3           (1 << 3)
#define PIO_MODE4           (1 << 4)

#define SWDMA_MODE0         (1 << 5)
#define SWDMA_MODE1         (1 << 6)
#define SWDMA_MODE2         (1 << 7)

#define MWDMA_MODE0         (1 << 8)
#define MWDMA_MODE1         (1 << 9)
#define MWDMA_MODE2         (1 << 10)

#define UDMA_MODE0          (1 << 11)
#define UDMA_MODE1          (1 << 12)
#define UDMA_MODE2          (1 << 13)
#define UDMA_MODE3          (1 << 14)
#define UDMA_MODE4          (1 << 15)
#define UDMA_MODE5          (1 << 16)

#define PIO_SUPPORT         (PIO_MODE0      | PIO_MODE1     | PIO_MODE2    | PIO_MODE3     | PIO_MODE4)
#define SWDMA_SUPPORT       (SWDMA_MODE0    | SWDMA_MODE1   | SWDMA_MODE2)
#define MWDMA_SUPPORT       (MWDMA_MODE0    | MWDMA_MODE1   | MWDMA_MODE2)
#define UDMA33_SUPPORT      (UDMA_MODE0     | UDMA_MODE1    | UDMA_MODE2)
#define UDMA66_SUPPORT      (UDMA_MODE3     | UDMA_MODE4)
#define UDMA100_SUPPORT     (UDMA_MODE5 )
#define UDMA_SUPPORT        (UNINITIALIZED_TRANSFER_MODE & (~(PIO_SUPPORT | SWDMA_SUPPORT | MWDMA_SUPPORT)))

#define DMA_SUPPORT         (SWDMA_SUPPORT  | MWDMA_SUPPORT | UDMA_SUPPORT)
#define ALL_MODE_SUPPORT    (PIO_SUPPORT | DMA_SUPPORT)

#define PIO0                        0
#define PIO1                        1
#define PIO2                        2
#define PIO3                        3
#define PIO4                        4
#define SWDMA0                      5
#define SWDMA1                      6
#define SWDMA2                      7
#define MWDMA0                      8
#define MWDMA1                      9
#define MWDMA2                      10
#define UDMA0                       11
#define UDMA1                       12
#define UDMA2                       13
#define UDMA3                       14
#define UDMA4                       15
#define UDMA5                       16
#define UDMA6                       17
#define MAX_XFER_MODE               17
#define UNINITIALIZED_CYCLE_TIME    0xffffffff
#define UNINITIALIZED_TRANSFER_MODE 0x7fffffff
#define IS_DEFAULT(mode)    (!(mode & 0x80000000))

#define GenTransferModeMask(i, mode) {\
    ULONG temp=0xffffffff; \
    mode |= (temp >> (31-(i)));\
}

//
// mode should not be 0
//
#define GetHighestTransferMode(mode, i) {\
    ULONG temp=(mode); \
    ASSERT(temp); \
    i=0; \
    while ( temp) { \
        temp = (temp >> 1);\
        i++;\
    } \
    i--; \
}

#define GetHighestDMATransferMode(mode, i) {\
    ULONG temp=mode >> 5;\
    i=5; \
    while ( temp) { \
        temp = (temp >> 1); \
        i++; \
    } \
    i--; \
}
#define GetHighestPIOTransferMode(mode, i) { \
    ULONG temp = (mode & PIO_SUPPORT); \
    i=0; \
    temp = temp >> 1; \
    while (temp) { \
        temp = temp >> 1; \
        i++; \
    } \
}

#define SetDefaultTiming(timingTable ) {\
    timingTable[0]=PIO_MODE0_CYCLE_TIME; \
    timingTable[1]=PIO_MODE1_CYCLE_TIME; \
    timingTable[2]=PIO_MODE2_CYCLE_TIME; \
    timingTable[3]=PIO_MODE3_CYCLE_TIME; \
    timingTable[4]=PIO_MODE4_CYCLE_TIME; \
    timingTable[5]=SWDMA_MODE0_CYCLE_TIME; \
    timingTable[6]=SWDMA_MODE1_CYCLE_TIME; \
    timingTable[7]=SWDMA_MODE2_CYCLE_TIME; \
    timingTable[8]=MWDMA_MODE0_CYCLE_TIME; \
    timingTable[9]=MWDMA_MODE1_CYCLE_TIME; \
    timingTable[10]=MWDMA_MODE2_CYCLE_TIME; \
    timingTable[11]=UDMA_MODE0_CYCLE_TIME; \
    timingTable[12]=UDMA_MODE1_CYCLE_TIME; \
    timingTable[13]=UDMA_MODE2_CYCLE_TIME; \
    timingTable[14]=UDMA_MODE3_CYCLE_TIME; \
    timingTable[15]=UDMA_MODE4_CYCLE_TIME; \
    timingTable[16]=UDMA_MODE5_CYCLE_TIME; \
}

//
// possible ide channel state
//         
typedef enum {
    ChannelDisabled = 0,
    ChannelEnabled,
    ChannelStateUnknown
} IDE_CHANNEL_STATE;

//
// IDENTIFY data
//
#pragma pack (push, id_data, 1)
typedef struct _IDENTIFY_DATA {
    USHORT GeneralConfiguration;            // 00 00
    USHORT NumCylinders;                    // 02  1
    USHORT Reserved1;                       // 04  2
    USHORT NumHeads;                        // 06  3
    USHORT UnformattedBytesPerTrack;        // 08  4
    USHORT UnformattedBytesPerSector;       // 0A  5
    USHORT NumSectorsPerTrack;              // 0C  6
    USHORT VendorUnique1[3];                // 0E  7-9
    UCHAR  SerialNumber[20];                // 14  10-19
    USHORT BufferType;                      // 28  20
    USHORT BufferSectorSize;                // 2A  21
    USHORT NumberOfEccBytes;                // 2C  22
    UCHAR  FirmwareRevision[8];             // 2E  23-26
    UCHAR  ModelNumber[40];                 // 36  27-46
    UCHAR  MaximumBlockTransfer;            // 5E  47
    UCHAR  VendorUnique2;                   // 5F
    USHORT DoubleWordIo;                    // 60  48
    USHORT Capabilities;                    // 62  49
    USHORT Reserved2;                       // 64  50
    UCHAR  VendorUnique3;                   // 66  51
    UCHAR  PioCycleTimingMode;              // 67
    UCHAR  VendorUnique4;                   // 68  52
    UCHAR  DmaCycleTimingMode;              // 69
    USHORT TranslationFieldsValid:3;        // 6A  53
    USHORT Reserved3:13;
    USHORT NumberOfCurrentCylinders;        // 6C  54
    USHORT NumberOfCurrentHeads;            // 6E  55
    USHORT CurrentSectorsPerTrack;          // 70  56
    ULONG  CurrentSectorCapacity;           // 72  57-58
    USHORT CurrentMultiSectorSetting;       //     59
    ULONG  UserAddressableSectors;          //     60-61
    USHORT SingleWordDMASupport : 8;        //     62
    USHORT SingleWordDMAActive : 8;
    USHORT MultiWordDMASupport : 8;         //     63
    USHORT MultiWordDMAActive : 8;
    USHORT AdvancedPIOModes : 8;            //     64
    USHORT Reserved4 : 8;
    USHORT MinimumMWXferCycleTime;          //     65
    USHORT RecommendedMWXferCycleTime;      //     66
    USHORT MinimumPIOCycleTime;             //     67
    USHORT MinimumPIOCycleTimeIORDY;        //     68
    USHORT Reserved5[11];                   //     69-79
    USHORT MajorRevision;                   //     80
    USHORT MinorRevision;                   //     81
    USHORT Reserved6;                       //     82
    USHORT CommandSetSupport;               //     83
    USHORT Reserved6a[2];                   //     84-85
    USHORT CommandSetActive;                //     86
    USHORT Reserved6b;                      //     87
    USHORT UltraDMASupport : 8;             //     88
    USHORT UltraDMAActive  : 8;             //
    USHORT Reserved7[11];                   //     89-99
    ULONG  Max48BitLBA[2];                  //     100-103
    USHORT Reserved7a[22];                  //     104-125
    USHORT LastLun:3;                       //     126
    USHORT Reserved8:13;
    USHORT MediaStatusNotification:2;       //     127
    USHORT Reserved9:6;
    USHORT DeviceWriteProtect:1;
    USHORT Reserved10:7;
    USHORT Reserved11[128];                  //     128-255
} IDENTIFY_DATA, *PIDENTIFY_DATA;
#pragma pack (pop, id_data)

#define IDENTIFY_DATA_SIZE sizeof(IDENTIFY_DATA)

//
// The structure is passed to pci ide mini driver
// TransferModeSelect callback for selecting
// proper transfer mode the the devices connected
// to the given IDE channel
//
typedef struct _PCIIDE_TRANSFER_MODE_SELECT {

    //
    // Input Parameters
    //          
          
    //
    // IDE Channel Number. 
    //                                       
    ULONG   Channel;

    //
    // Indicate whether devices are present
    //                                  
    BOOLEAN DevicePresent[MAX_IDE_DEVICE * MAX_IDE_LINE];
    
    //
    // Indicate whether devices are ATA harddisk
    //
    BOOLEAN FixedDisk[MAX_IDE_DEVICE * MAX_IDE_LINE];

    //
    // Indicate whether devices support IO Ready Line
    //                                                
    BOOLEAN IoReadySupported[MAX_IDE_DEVICE * MAX_IDE_LINE];

    //
    // Indicate the data transfer modes devices support
    //               
    ULONG DeviceTransferModeSupported[MAX_IDE_DEVICE * MAX_IDE_LINE];

    //
    // Indicate devices' best timings for PIO, single word DMA,
    // multiword DMA, and Ultra DMA modes
    //
    ULONG BestPioCycleTime[MAX_IDE_DEVICE * MAX_IDE_LINE];
    ULONG BestSwDmaCycleTime[MAX_IDE_DEVICE * MAX_IDE_LINE];
    ULONG BestMwDmaCycleTime[MAX_IDE_DEVICE * MAX_IDE_LINE];
    ULONG BestUDmaCycleTime[MAX_IDE_DEVICE * MAX_IDE_LINE];

    //
    // Indicate devices' current data transfer modes
    //
    ULONG DeviceTransferModeCurrent[MAX_IDE_DEVICE * MAX_IDE_LINE];

    //
    // The user's choice. This will allow pciidex to
    // default to a transfer mode indicated by the mini driver
    //
    ULONG UserChoiceTransferMode[MAX_IDE_DEVICE * MAX_IDE_LINE];

    //
    // This enables UDMA66 on the intel chipsets
    //
    ULONG EnableUDMA66;

    //
    //Some miniports need this
    // The miniport will save this data in their deviceExtension
    //
    IDENTIFY_DATA IdentifyData[MAX_IDE_DEVICE];


    //
    // Output Parameters
    //          
                      
    //
    // Indicate devices' data transfer modes chosen by
    // the pcii ide mini drive
    //
    ULONG DeviceTransferModeSelected[MAX_IDE_DEVICE * MAX_IDE_LINE];

    //
    // Transfermode timings
    //
    PULONG TransferModeTimingTable;
    ULONG   TransferModeTableLength;

} PCIIDE_TRANSFER_MODE_SELECT, *PPCIIDE_TRANSFER_MODE_SELECT;

//
// Prototype for different PCI IDE mini driver
// callbacks
//   
typedef IDE_CHANNEL_STATE
    (*PCIIDE_CHANNEL_ENABLED) (
        IN PVOID DeviceExtension,
        IN ULONG Channel
        );

typedef BOOLEAN 
    (*PCIIDE_SYNC_ACCESS_REQUIRED) (
        IN PVOID DeviceExtension
        );

typedef NTSTATUS
    (*PCIIDE_TRANSFER_MODE_SELECT_FUNC) (
        IN     PVOID                     DeviceExtension,
        IN OUT PPCIIDE_TRANSFER_MODE_SELECT TransferModeSelect
        );

typedef    ULONG  
    (*PCIIDE_USEDMA_FUNC)(
        IN PVOID deviceExtension, 
        IN PVOID cdbCmd,
        IN UCHAR targetID
        ); 

typedef    NTSTATUS
    (*PCIIDE_UDMA_MODES_SUPPORTED) (
        IDENTIFY_DATA   IdentifyData,
        PULONG          BestXferMode,
        PULONG          CurrentMode
        );

// 
// This structure is for the PCI IDE mini driver to 
// return its properties
// 
typedef struct _IDE_CONTROLLER_PROPERTIES {

    //
    // sizeof (IDE_CONTROLLER_PROPERTIES)
    //
    ULONG Size;      
    
    //
    // Indicate the amount of memory PCI IDE mini driver
    // needs for its private data
    //
    ULONG ExtensionSize;

    //
    // Indicate all the data transfer modes the PCI IDE
    // controller supports
    //                                 
    ULONG SupportedTransferMode[MAX_IDE_CHANNEL][MAX_IDE_DEVICE];

    //
    // callback to query whether a IDE channel is enabled
    //                          
    PCIIDE_CHANNEL_ENABLED      PciIdeChannelEnabled;
    
    //
    // callback to query whether both IDE channels requires
    // synchronized access.  (one channel at a time)
    //                                                              
    PCIIDE_SYNC_ACCESS_REQUIRED PciIdeSyncAccessRequired;
    
    //
    // callback to select proper transfer modes for the
    // given devices
    //
    PCIIDE_TRANSFER_MODE_SELECT_FUNC PciIdeTransferModeSelect;

    //
    // at the end of a ATA data transfer, ignores busmaster 
    // status active bit.  Normally, it should be FALSE
    //                    
    BOOLEAN IgnoreActiveBitForAtaDevice;

    //
    // always clear the busmaster interrupt on every interrupt
    // generated by the device.  Normnally, it should be FALSE
    //
    BOOLEAN AlwaysClearBusMasterInterrupt;

    //
    // Some DMA engines can not handle odd DWORD sized transfers
    // Do these IO transfers in PIO mode instead
    //
    BOOLEAN UsePioOnOddTransfers;

    //
    // Some DMA engines can not handle 8K*n+m size transfers 
    // where n > 0 and 0 < m < 512 bytes
    // Do these IO transfers in PIO mode instead
    //
    UCHAR UsePioAfter8kTransfers:1;

    //
    // ATI SB700 needs one DMA retry on CRC error.
    // Retrying in PIO can hang a system.
    //
    UCHAR DmaRetryAfterCrcError:1;
    
    UCHAR Reserved:6;

    //
    // callback to determine whether DMA should be used or not
    // called for every IO
    //
    PCIIDE_USEDMA_FUNC PciIdeUseDma;


    //
    // if the miniport needs a different alignment
    //
    ULONG AlignmentRequirement;

    ULONG DefaultPIO;

    //
    // retrieves the supported udma modes from the Identify data
    //
    PCIIDE_UDMA_MODES_SUPPORTED PciIdeUdmaModesSupported;

} IDE_CONTROLLER_PROPERTIES, *PIDE_CONTROLLER_PROPERTIES;

//
// callback to query PCI IDE controller properties
//                            
typedef
NTSTATUS 
    (*PCONTROLLER_PROPERTIES) (
        IN PVOID                      DeviceExtension,
        IN PIDE_CONTROLLER_PROPERTIES ControllerProperties
        );

//
// To initailize PCI IDE mini driver
//                     
NTSTATUS
PciIdeXInitialize(
    IN PDRIVER_OBJECT           DriverObject,
    IN PUNICODE_STRING          RegistryPath,
    IN PCONTROLLER_PROPERTIES   PciIdeGetControllerProperties,
    IN ULONG                    ExtensionSize
    );

//
// To query PCI IDE config space data
//                                    
NTSTATUS
PciIdexGetBusLocation(
    IN PVOID  DeviceExtension,
    IN PULONG Address
    );

NTSTATUS
PciIdeXGetBusData(
    IN PVOID DeviceExtension,
    IN PVOID Buffer,
    IN ULONG ConfigDataOffset,
    IN ULONG BufferLength
    );

//
// To save PCI IDE config space data
//                                    
NTSTATUS
PciIdeXSetBusData(
    IN PVOID DeviceExtension,
    IN PVOID Buffer,
    IN PVOID DataMask,
    IN ULONG ConfigDataOffset,
    IN ULONG BufferLength
    );

                     
#pragma pack(push, config_header, 1)
typedef struct _PCIIDE_CONFIG_HEADER {

    USHORT  VendorID;                   // (ro)
    USHORT  DeviceID;                   // (ro)

    //
    //  Command
    //
    union {

        struct {

            USHORT  IoAccessEnable:1;           // Device control
            USHORT  MemAccessEnable:1;
            USHORT  MasterEnable:1;
            USHORT  SpecialCycle:1;
            USHORT  MemWriteInvalidateEnable:1;
            USHORT  VgaPaletteSnoopEnable:1;
            USHORT  ParityErrorResponse:1;
            USHORT  WaitCycleEnable:1;
            USHORT  SystemErrorEnable:1;
            USHORT  FastBackToBackEnable:1;
            USHORT  CommandReserved:6;
        } b;

        USHORT w;

    } Command;


    USHORT  Status;
    UCHAR   RevisionID;                 // (ro)

    //
    //  Program Interface
    //
    UCHAR   Chan0OpMode:1;
    UCHAR   Chan0Programmable:1;
    UCHAR   Chan1OpMode:1;
    UCHAR   Chan1Programmable:1;
    UCHAR   ProgIfReserved:3;
    UCHAR   MasterIde:1;

    UCHAR   SubClass;                   // (ro)
    UCHAR   BaseClass;                  // (ro)
    UCHAR   CacheLineSize;              // (ro+)
    UCHAR   LatencyTimer;               // (ro+)
    UCHAR   HeaderType;                 // (ro)
    UCHAR   BIST;                       // Built in self test

    struct _PCI_HEADER_TYPE_0 type0;

} PCIIDE_CONFIG_HEADER, *PPCIIDE_CONFIG_HEADER;
#pragma pack(pop, config_header)
                     
//
// Debug Print
//                        
#if DBG

VOID
PciIdeXDebugPrint(
    ULONG DebugPrintLevel,
    PCCHAR DebugMessage,
    ...
    );

#define PciIdeXDebugPrint(x)    PciIdeXDebugPrint x
    
#else
    
#define PciIdeXDebugPrint(x)    

#endif // DBG

#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4214)
#endif

