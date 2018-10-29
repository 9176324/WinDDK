
/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    storport.h

Abstract:

    These are the structures and defines that are included by STORPORT miniport
    drivers.

Authors:

Revision History:

--*/

#ifdef _NTSCSI_
#error "STORPORT.H must be included instead of SCSI.H"
#endif

#ifdef _NTSRB_
#error "STORPORT.H must be included instead of SRB.H"
#endif

#ifndef _NTSTORPORT_
#define _NTSTORPORT_

#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4200) // array[0] is not a warning for this file
#pragma warning(disable:4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable:4214) // nonstandard extension used : bit field types other than int

#if DBG
#define DebugPrint(x) StorPortDebugPrint x
#else
#define DebugPrint(x)
#endif


//
// Define SCSI maximum configuration parameters.
//

#define SCSI_MAXIMUM_LOGICAL_UNITS 8
#define SCSI_MAXIMUM_TARGETS_PER_BUS 128
#define SCSI_MAXIMUM_LUNS_PER_TARGET 255
#define SCSI_MAXIMUM_BUSES 8
#define SCSI_MINIMUM_PHYSICAL_BREAKS  16
#define SCSI_MAXIMUM_PHYSICAL_BREAKS 255

#define SCSI_COMBINE_BUS_TARGET( Bus, Target ) ( \
    ((((UCHAR) (Target)) & ~(0x20 - 1)) << 8) |        \
    (((UCHAR) (Bus)) << 5) |                     \
    (((UCHAR) (Target)) & (0x20 - 1)))

#define SCSI_DECODE_BUS_TARGET( Value, Bus, Target ) ( \
    Bus = (UCHAR) ((Value) >> 5),                     \
    Target = (UCHAR) ((((Value) >> 8) & ~(0x20 - 1)) | ((Value) & (0x20 - 1))))

//
// This constant is for backward compatibility.
// This use to be the maximum number of targets supported.
//

#define SCSI_MAXIMUM_TARGETS 8


//
// Uninitialized flag value.
//

#define SP_UNINITIALIZED_VALUE ((ULONG) ~0)
#define SP_UNTAGGED ((UCHAR) ~0)

//
// Set asynchronous events.
//

#define SRBEV_BUS_RESET               0x0001
#define SRBEV_SCSI_ASYNC_NOTIFICATION 0x0002

// begin_ntminitape

#define MAXIMUM_CDB_SIZE 12

//
// SCSI I/O Request Block
//

typedef struct _SCSI_REQUEST_BLOCK {
    USHORT Length;                  // offset 0
    UCHAR Function;                 // offset 2
    UCHAR SrbStatus;                // offset 3
    UCHAR ScsiStatus;               // offset 4
    UCHAR PathId;                   // offset 5
    UCHAR TargetId;                 // offset 6
    UCHAR Lun;                      // offset 7
    UCHAR QueueTag;                 // offset 8
    UCHAR QueueAction;              // offset 9
    UCHAR CdbLength;                // offset a
    UCHAR SenseInfoBufferLength;    // offset b
    ULONG SrbFlags;                 // offset c
    ULONG DataTransferLength;       // offset 10
    ULONG TimeOutValue;             // offset 14
    __field_bcount(DataTransferLength) \
    PVOID DataBuffer;               // offset 18
    PVOID SenseInfoBuffer;          // offset 1c
    struct _SCSI_REQUEST_BLOCK *NextSrb; // offset 20
    PVOID OriginalRequest;          // offset 24
    PVOID SrbExtension;             // offset 28
    union {
        ULONG InternalStatus;       // offset 2c
        ULONG QueueSortKey;         // offset 2c
        ULONG LinkTimeoutValue;     // offset 2c
    };

#if defined(_WIN64)

    //
    // Force PVOID alignment of Cdb
    //

    ULONG Reserved;

#endif

    UCHAR Cdb[16];                  // offset 30
} SCSI_REQUEST_BLOCK, *PSCSI_REQUEST_BLOCK;

#define SCSI_REQUEST_BLOCK_SIZE sizeof(SCSI_REQUEST_BLOCK)

//
// SCSI I/O Request Block for WMI Requests
//

typedef struct _SCSI_WMI_REQUEST_BLOCK {
    USHORT Length;
    UCHAR Function;        // SRB_FUNCTION_WMI
    UCHAR SrbStatus;
    UCHAR WMISubFunction;
    UCHAR PathId;          // If SRB_WMI_FLAGS_ADAPTER_REQUEST is set in
    UCHAR TargetId;        // WMIFlags then PathId, TargetId and Lun are
    UCHAR Lun;             // reserved fields.
    UCHAR Reserved1;
    UCHAR WMIFlags;
    UCHAR Reserved2[2];
    ULONG SrbFlags;
    ULONG DataTransferLength;
    ULONG TimeOutValue;
    PVOID DataBuffer;
    PVOID DataPath;
    PVOID Reserved3;
    PVOID OriginalRequest;
    PVOID SrbExtension;
    ULONG Reserved4;

#if (NTDDI_VERSION >= NTDDI_WS03SP1)
#if defined(_WIN64)

    //
    // Force PVOID alignment of Cdb
    //

    ULONG Reserved6;

#endif
#endif

    UCHAR Reserved5[16];
} SCSI_WMI_REQUEST_BLOCK, *PSCSI_WMI_REQUEST_BLOCK;

typedef enum _STOR_DEVICE_POWER_STATE {
    StorPowerDeviceUnspecified = 0,
    StorPowerDeviceD0,
    StorPowerDeviceD1,
    StorPowerDeviceD2,
    StorPowerDeviceD3,
    StorPowerDeviceMaximum
} STOR_DEVICE_POWER_STATE, *PSTOR_DEVICE_POWER_STATE;

typedef enum {
    StorPowerActionNone = 0,
    StorPowerActionReserved,
    StorPowerActionSleep,
    StorPowerActionHibernate,
    StorPowerActionShutdown,
    StorPowerActionShutdownReset,
    StorPowerActionShutdownOff,
    StorPowerActionWarmEject
} STOR_POWER_ACTION, *PSTOR_POWER_ACTION;

typedef struct _SCSI_POWER_REQUEST_BLOCK {
    USHORT Length;                  // offset 0
    UCHAR Function;                 // offset 2
    UCHAR SrbStatus;                // offset 3
    UCHAR SrbPowerFlags;            // offset 4
    UCHAR PathId;                   // offset 5
    UCHAR TargetId;                 // offset 6
    UCHAR Lun;                      // offset 7
    STOR_DEVICE_POWER_STATE DevicePowerState; // offset 8
    ULONG SrbFlags;                 // offset c
    ULONG DataTransferLength;       // offset 10
    ULONG TimeOutValue;             // offset 14
    PVOID DataBuffer;               // offset 18
    PVOID SenseInfoBuffer;          // offset 1c
    struct _SCSI_REQUEST_BLOCK *NextSrb; // offset 20
    PVOID OriginalRequest;          // offset 24
    PVOID SrbExtension;             // offset 28
    STOR_POWER_ACTION PowerAction;       // offset 2c

#if defined(_WIN64)

    //
    // Force PVOID alignment of Cdb
    //

    ULONG Reserved;

#endif

    UCHAR Reserved5[16];              // offset 30
} SCSI_POWER_REQUEST_BLOCK, *PSCSI_POWER_REQUEST_BLOCK;

//
// PNP minor function codes.
//
typedef enum {
    StorStartDevice = 0x0,
    StorRemoveDevice = 0x2,
    StorStopDevice  = 0x4,
    StorQueryCapabilities = 0x9,
    StorQueryResourceRequirements = 0xB,
    StorFilterResourceRequirements = 0xD,
    StorSurpriseRemoval = 0x17
} STOR_PNP_ACTION, *PSTOR_PNP_ACTION;

typedef struct _STOR_DEVICE_CAPABILITIES {
    USHORT Version;
    ULONG  DeviceD1:1;
    ULONG  DeviceD2:1;
    ULONG  LockSupported:1;
    ULONG  EjectSupported:1;
    ULONG  Removable:1;
    ULONG  DockDevice:1;
    ULONG  UniqueID:1;
    ULONG  SilentInstall:1;
    ULONG  SurpriseRemovalOK:1;
    ULONG  NoDisplayInUI:1;

} STOR_DEVICE_CAPABILITIES, *PSTOR_DEVICE_CAPABILITIES;

typedef struct _SCSI_PNP_REQUEST_BLOCK {
    USHORT Length;                  // offset 0
    UCHAR Function;                 // offset 2
    UCHAR SrbStatus;                // offset 3
    UCHAR PnPSubFunction;           // offset 4
    UCHAR PathId;                   // offset 5
    UCHAR TargetId;                 // offset 6
    UCHAR Lun;                      // offset 7
    STOR_PNP_ACTION PnPAction;       // offset 8
    ULONG SrbFlags;                 // offset c
    ULONG DataTransferLength;       // offset 10
    ULONG TimeOutValue;             // offset 14
    PVOID DataBuffer;               // offset 18
    PVOID SenseInfoBuffer;          // offset 1c
    struct _SCSI_REQUEST_BLOCK *NextSrb; // offset 20
    PVOID OriginalRequest;          // offset 24
    PVOID SrbExtension;             // offset 28
    ULONG SrbPnPFlags;              // offset 2c

#if defined(_WIN64)

    //
    // Force PVOID alignment of Cdb
    //

    ULONG Reserved;

#endif
        UCHAR Reserved4[16];            // offset 30
} SCSI_PNP_REQUEST_BLOCK, *PSCSI_PNP_REQUEST_BLOCK;


//
// SRB Functions
//

#define SRB_FUNCTION_EXECUTE_SCSI           0x00
#define SRB_FUNCTION_CLAIM_DEVICE           0x01
#define SRB_FUNCTION_IO_CONTROL             0x02
#define SRB_FUNCTION_RECEIVE_EVENT          0x03
#define SRB_FUNCTION_RELEASE_QUEUE          0x04
#define SRB_FUNCTION_ATTACH_DEVICE          0x05
#define SRB_FUNCTION_RELEASE_DEVICE         0x06
#define SRB_FUNCTION_SHUTDOWN               0x07
#define SRB_FUNCTION_FLUSH                  0x08
#define SRB_FUNCTION_ABORT_COMMAND          0x10
#define SRB_FUNCTION_RELEASE_RECOVERY       0x11
#define SRB_FUNCTION_RESET_BUS              0x12
#define SRB_FUNCTION_RESET_DEVICE           0x13
#define SRB_FUNCTION_TERMINATE_IO           0x14
#define SRB_FUNCTION_FLUSH_QUEUE            0x15
#define SRB_FUNCTION_REMOVE_DEVICE          0x16
#define SRB_FUNCTION_WMI                    0x17
#define SRB_FUNCTION_LOCK_QUEUE             0x18
#define SRB_FUNCTION_UNLOCK_QUEUE           0x19
#define SRB_FUNCTION_RESET_LOGICAL_UNIT     0x20
#define SRB_FUNCTION_SET_LINK_TIMEOUT       0x21
#define SRB_FUNCTION_LINK_TIMEOUT_OCCURRED  0x22
#define SRB_FUNCTION_LINK_TIMEOUT_COMPLETE  0x23
#define SRB_FUNCTION_POWER                  0x24
#define SRB_FUNCTION_PNP                    0x25
#define SRB_FUNCTION_DUMP_POINTERS          0x26
//
// SRB Status
//

#define SRB_STATUS_PENDING                  0x00
#define SRB_STATUS_SUCCESS                  0x01
#define SRB_STATUS_ABORTED                  0x02
#define SRB_STATUS_ABORT_FAILED             0x03
#define SRB_STATUS_ERROR                    0x04
#define SRB_STATUS_BUSY                     0x05
#define SRB_STATUS_INVALID_REQUEST          0x06
#define SRB_STATUS_INVALID_PATH_ID          0x07
#define SRB_STATUS_NO_DEVICE                0x08
#define SRB_STATUS_TIMEOUT                  0x09
#define SRB_STATUS_SELECTION_TIMEOUT        0x0A
#define SRB_STATUS_COMMAND_TIMEOUT          0x0B
#define SRB_STATUS_MESSAGE_REJECTED         0x0D
#define SRB_STATUS_BUS_RESET                0x0E
#define SRB_STATUS_PARITY_ERROR             0x0F
#define SRB_STATUS_REQUEST_SENSE_FAILED     0x10
#define SRB_STATUS_NO_HBA                   0x11
#define SRB_STATUS_DATA_OVERRUN             0x12
#define SRB_STATUS_UNEXPECTED_BUS_FREE      0x13
#define SRB_STATUS_PHASE_SEQUENCE_FAILURE   0x14
#define SRB_STATUS_BAD_SRB_BLOCK_LENGTH     0x15
#define SRB_STATUS_REQUEST_FLUSHED          0x16
#define SRB_STATUS_INVALID_LUN              0x20
#define SRB_STATUS_INVALID_TARGET_ID        0x21
#define SRB_STATUS_BAD_FUNCTION             0x22
#define SRB_STATUS_ERROR_RECOVERY           0x23
#define SRB_STATUS_NOT_POWERED              0x24
#define SRB_STATUS_LINK_DOWN                0x25

//
// This value is used by the port driver to indicate that a non-scsi-related
// error occured.  Miniports must never return this status.
//

#define SRB_STATUS_INTERNAL_ERROR           0x30

//
// Srb status values 0x38 through 0x3f are reserved for internal port driver
// use.
//



//
// SRB Status Masks
//

#define SRB_STATUS_QUEUE_FROZEN             0x40
#define SRB_STATUS_AUTOSENSE_VALID          0x80

#define SRB_STATUS(Status) (Status & ~(SRB_STATUS_AUTOSENSE_VALID | SRB_STATUS_QUEUE_FROZEN))

//
// SRB Flag Bits
//

#define SRB_FLAGS_QUEUE_ACTION_ENABLE       0x00000002
#define SRB_FLAGS_DISABLE_DISCONNECT        0x00000004
#define SRB_FLAGS_DISABLE_SYNCH_TRANSFER    0x00000008

#define SRB_FLAGS_BYPASS_FROZEN_QUEUE       0x00000010
#define SRB_FLAGS_DISABLE_AUTOSENSE         0x00000020
#define SRB_FLAGS_DATA_IN                   0x00000040
#define SRB_FLAGS_DATA_OUT                  0x00000080
#define SRB_FLAGS_NO_DATA_TRANSFER          0x00000000
#define SRB_FLAGS_UNSPECIFIED_DIRECTION      (SRB_FLAGS_DATA_IN | SRB_FLAGS_DATA_OUT)

#define SRB_FLAGS_NO_QUEUE_FREEZE           0x00000100
#define SRB_FLAGS_ADAPTER_CACHE_ENABLE      0x00000200
#define SRB_FLAGS_FREE_SENSE_BUFFER         0x00000400

#define SRB_FLAGS_IS_ACTIVE                 0x00010000
#define SRB_FLAGS_ALLOCATED_FROM_ZONE       0x00020000
#define SRB_FLAGS_SGLIST_FROM_POOL          0x00040000
#define SRB_FLAGS_BYPASS_LOCKED_QUEUE       0x00080000

#define SRB_FLAGS_NO_KEEP_AWAKE             0x00100000
#define SRB_FLAGS_PORT_DRIVER_ALLOCSENSE    0x00200000

#define SRB_FLAGS_PORT_DRIVER_SENSEHASPORT  0x00400000
#define SRB_FLAGS_DONT_START_NEXT_PACKET    0x00800000

#define SRB_FLAGS_PORT_DRIVER_RESERVED      0x0F000000
#define SRB_FLAGS_CLASS_DRIVER_RESERVED     0xF0000000

#if DBG==1
//
// A signature used to validate the scsi port number
// at the end of a sense buffer.
//
#define SCSI_PORT_SIGNATURE                 0x54524f50
#endif

//
// Queue Action
//

#define SRB_SIMPLE_TAG_REQUEST              0x20
#define SRB_HEAD_OF_QUEUE_TAG_REQUEST       0x21
#define SRB_ORDERED_QUEUE_TAG_REQUEST       0x22

#define SRB_WMI_FLAGS_ADAPTER_REQUEST       0x01
#define SRB_POWER_FLAGS_ADAPTER_REQUEST     0x01
#define SRB_PNP_FLAGS_ADAPTER_REQUEST     0x01

// end_ntminitape


//
// Command Descriptor Block. Passed by SCSI controller chip over the SCSI bus
//

#pragma pack(push, cdb, 1)
typedef union _CDB {

    //
    // Generic 6-Byte CDB
    //

    struct _CDB6GENERIC {
       UCHAR  OperationCode;
       UCHAR  Immediate : 1;
       UCHAR  CommandUniqueBits : 4;
       UCHAR  LogicalUnitNumber : 3;
       UCHAR  CommandUniqueBytes[3];
       UCHAR  Link : 1;
       UCHAR  Flag : 1;
       UCHAR  Reserved : 4;
       UCHAR  VendorUnique : 2;
    } CDB6GENERIC;

    //
    // Standard 6-byte CDB
    //

    struct _CDB6READWRITE {
        UCHAR OperationCode;    // 0x08, 0x0A - SCSIOP_READ, SCSIOP_WRITE
        UCHAR LogicalBlockMsb1 : 5;
        UCHAR LogicalUnitNumber : 3;
        UCHAR LogicalBlockMsb0;
        UCHAR LogicalBlockLsb;
        UCHAR TransferBlocks;
        UCHAR Control;
    } CDB6READWRITE;

    //
    // SCSI-1 Inquiry CDB
    //

    struct _CDB6INQUIRY {
        UCHAR OperationCode;    // 0x12 - SCSIOP_INQUIRY
        UCHAR Reserved1 : 5;
        UCHAR LogicalUnitNumber : 3;
        UCHAR PageCode;
        UCHAR IReserved;
        UCHAR AllocationLength;
        UCHAR Control;
    } CDB6INQUIRY;

    //
    // SCSI-3 Inquiry CDB
    //

    struct _CDB6INQUIRY3 {
        UCHAR OperationCode;    // 0x12 - SCSIOP_INQUIRY
        UCHAR EnableVitalProductData : 1;
        UCHAR CommandSupportData : 1;
        UCHAR Reserved1 : 6;
        UCHAR PageCode;
        UCHAR Reserved2;
        UCHAR AllocationLength;
        UCHAR Control;
    } CDB6INQUIRY3;

    struct _CDB6VERIFY {
        UCHAR OperationCode;    // 0x13 - SCSIOP_VERIFY
        UCHAR Fixed : 1;
        UCHAR ByteCompare : 1;
        UCHAR Immediate : 1;
        UCHAR Reserved : 2;
        UCHAR LogicalUnitNumber : 3;
        UCHAR VerificationLength[3];
        UCHAR Control;
    } CDB6VERIFY;

    //
    // SCSI Format CDB
    //

    struct _CDB6FORMAT {
        UCHAR OperationCode;    // 0x04 - SCSIOP_FORMAT_UNIT
        UCHAR FormatControl : 5;
        UCHAR LogicalUnitNumber : 3;
        UCHAR FReserved1;
        UCHAR InterleaveMsb;
        UCHAR InterleaveLsb;
        UCHAR FReserved2;
    } CDB6FORMAT;

    //
    // Standard 10-byte CDB

    struct _CDB10 {
        UCHAR OperationCode;
        UCHAR RelativeAddress : 1;
        UCHAR Reserved1 : 2;
        UCHAR ForceUnitAccess : 1;
        UCHAR DisablePageOut : 1;
        UCHAR LogicalUnitNumber : 3;
        UCHAR LogicalBlockByte0;
        UCHAR LogicalBlockByte1;
        UCHAR LogicalBlockByte2;
        UCHAR LogicalBlockByte3;
        UCHAR Reserved2;
        UCHAR TransferBlocksMsb;
        UCHAR TransferBlocksLsb;
        UCHAR Control;
    } CDB10;

    //
    // Standard 12-byte CDB
    //

    struct _CDB12 {
        UCHAR OperationCode;
        UCHAR RelativeAddress : 1;
        UCHAR Reserved1 : 2;
        UCHAR ForceUnitAccess : 1;
        UCHAR DisablePageOut : 1;
        UCHAR LogicalUnitNumber : 3;
        UCHAR LogicalBlock[4];
        UCHAR TransferLength[4];
        UCHAR Reserved2;
        UCHAR Control;
    } CDB12;



    //
    // Standard 16-byte CDB
    //

    struct _CDB16 {
        UCHAR OperationCode;
        UCHAR Reserved1        : 3;
        UCHAR ForceUnitAccess  : 1;
        UCHAR DisablePageOut   : 1;
        UCHAR Protection       : 3;
        UCHAR LogicalBlock[8];
        UCHAR TransferLength[4];
        UCHAR Reserved2;
        UCHAR Control;
    } CDB16;


    //
    // CD Rom Audio CDBs
    //

    struct _PAUSE_RESUME {
        UCHAR OperationCode;    // 0x4B - SCSIOP_PAUSE_RESUME
        UCHAR Reserved1 : 5;
        UCHAR LogicalUnitNumber : 3;
        UCHAR Reserved2[6];
        UCHAR Action;
        UCHAR Control;
    } PAUSE_RESUME;

    //
    // Read Table of Contents
    //

    struct _READ_TOC {
        UCHAR OperationCode;    // 0x43 - SCSIOP_READ_TOC
        UCHAR Reserved0 : 1;
        UCHAR Msf : 1;
        UCHAR Reserved1 : 3;
        UCHAR LogicalUnitNumber : 3;
        UCHAR Format2 : 4;
        UCHAR Reserved2 : 4;
        UCHAR Reserved3[3];
        UCHAR StartingTrack;
        UCHAR AllocationLength[2];
        UCHAR Control : 6;
        UCHAR Format : 2;
    } READ_TOC;

    struct _READ_DISK_INFORMATION {
        UCHAR OperationCode;    // 0x51 - SCSIOP_READ_DISC_INFORMATION
        UCHAR Reserved1 : 5;
        UCHAR Lun : 3;
        UCHAR Reserved2[5];
        UCHAR AllocationLength[2];
        UCHAR Control;
    } READ_DISK_INFORMATION, READ_DISC_INFORMATION;

    struct _READ_TRACK_INFORMATION {
        UCHAR OperationCode;    // 0x52 - SCSIOP_READ_TRACK_INFORMATION
        UCHAR Track : 2;
        UCHAR Reserved4 : 3;
        UCHAR Lun : 3;
        UCHAR BlockAddress[4];  // or Track Number
        UCHAR Reserved3;
        UCHAR AllocationLength[2];
        UCHAR Control;
    } READ_TRACK_INFORMATION;

    struct _RESERVE_TRACK_RZONE {
        UCHAR OperationCode;    // 0x53 - SCSIOP_RESERVE_TRACK_RZONE
        UCHAR Reserved1[4];
        UCHAR ReservationSize[4];
        UCHAR Control;
    } RESERVE_TRACK_RZONE;

    struct _SEND_OPC_INFORMATION {
        UCHAR OperationCode;    // 0x54 - SCSIOP_SEND_OPC_INFORMATION
        UCHAR DoOpc : 1;        // perform OPC
        UCHAR Reserved1 : 7;
        UCHAR Exclude0 : 1;     // exclude layer 0
        UCHAR Exclude1 : 1;     // exclude layer 1
        UCHAR Reserved2 : 6;
        UCHAR Reserved3[4];
        UCHAR ParameterListLength[2];
        UCHAR Reserved4;
    } SEND_OPC_INFORMATION;

    struct _REPAIR_TRACK {
        UCHAR OperationCode;    // 0x58 - SCSIOP_REPAIR_TRACK
        UCHAR Immediate : 1;
        UCHAR Reserved1 : 7;
        UCHAR Reserved2[2];
        UCHAR TrackNumber[2];
        UCHAR Reserved3[3];
        UCHAR Control;
    } REPAIR_TRACK;

    struct _CLOSE_TRACK {
        UCHAR OperationCode;    // 0x5B - SCSIOP_CLOSE_TRACK_SESSION
        UCHAR Immediate : 1;
        UCHAR Reserved1 : 7;
        UCHAR Track     : 1;
        UCHAR Session   : 1;
        UCHAR Reserved2 : 6;
        UCHAR Reserved3;
        UCHAR TrackNumber[2];
        UCHAR Reserved4[3];
        UCHAR Control;
    } CLOSE_TRACK;

    struct _READ_BUFFER_CAPACITY {
        UCHAR OperationCode;    // 0x5C - SCSIOP_READ_BUFFER_CAPACITY
        UCHAR BlockInfo : 1;
        UCHAR Reserved1 : 7;
        UCHAR Reserved2[5];
        UCHAR AllocationLength[2];
        UCHAR Control;
    } READ_BUFFER_CAPACITY;

    struct _SEND_CUE_SHEET {
        UCHAR OperationCode;    // 0x5D - SCSIOP_SEND_CUE_SHEET
        UCHAR Reserved[5];
        UCHAR CueSheetSize[3];
        UCHAR Control;
    } SEND_CUE_SHEET;

    struct _READ_HEADER {
        UCHAR OperationCode;    // 0x44 - SCSIOP_READ_HEADER
        UCHAR Reserved1 : 1;
        UCHAR Msf : 1;
        UCHAR Reserved2 : 3;
        UCHAR Lun : 3;
        UCHAR LogicalBlockAddress[4];
        UCHAR Reserved3;
        UCHAR AllocationLength[2];
        UCHAR Control;
    } READ_HEADER;

    struct _PLAY_AUDIO {
        UCHAR OperationCode;    // 0x45 - SCSIOP_PLAY_AUDIO
        UCHAR Reserved1 : 5;
        UCHAR LogicalUnitNumber : 3;
        UCHAR StartingBlockAddress[4];
        UCHAR Reserved2;
        UCHAR PlayLength[2];
        UCHAR Control;
    } PLAY_AUDIO;

    struct _PLAY_AUDIO_MSF {
        UCHAR OperationCode;    // 0x47 - SCSIOP_PLAY_AUDIO_MSF
        UCHAR Reserved1 : 5;
        UCHAR LogicalUnitNumber : 3;
        UCHAR Reserved2;
        UCHAR StartingM;
        UCHAR StartingS;
        UCHAR StartingF;
        UCHAR EndingM;
        UCHAR EndingS;
        UCHAR EndingF;
        UCHAR Control;
    } PLAY_AUDIO_MSF;

    struct _BLANK_MEDIA {
        UCHAR OperationCode;    // 0xA1 - SCSIOP_BLANK
        UCHAR BlankType : 3;
        UCHAR Reserved1 : 1;
        UCHAR Immediate : 1;
        UCHAR Reserved2 : 3;
        UCHAR AddressOrTrack[4];
        UCHAR Reserved3[5];
        UCHAR Control;
    } BLANK_MEDIA;

    struct _PLAY_CD {
        UCHAR OperationCode;    // 0xBC - SCSIOP_PLAY_CD
        UCHAR Reserved1 : 1;
        UCHAR CMSF : 1;         // LBA = 0, MSF = 1
        UCHAR ExpectedSectorType : 3;
        UCHAR Lun : 3;

        union {
            struct _LBA {
                UCHAR StartingBlockAddress[4];
                UCHAR PlayLength[4];
            } LBA;

            struct _MSF {
                UCHAR Reserved1;
                UCHAR StartingM;
                UCHAR StartingS;
                UCHAR StartingF;
                UCHAR EndingM;
                UCHAR EndingS;
                UCHAR EndingF;
                UCHAR Reserved2;
            } MSF;
        };

        UCHAR Audio : 1;
        UCHAR Composite : 1;
        UCHAR Port1 : 1;
        UCHAR Port2 : 1;
        UCHAR Reserved2 : 3;
        UCHAR Speed : 1;
        UCHAR Control;
    } PLAY_CD;

    struct _SCAN_CD {
        UCHAR OperationCode;    // 0xBA - SCSIOP_SCAN_CD
        UCHAR RelativeAddress : 1;
        UCHAR Reserved1 : 3;
        UCHAR Direct : 1;
        UCHAR Lun : 3;
        UCHAR StartingAddress[4];
        UCHAR Reserved2[3];
        UCHAR Reserved3 : 6;
        UCHAR Type : 2;
        UCHAR Reserved4;
        UCHAR Control;
    } SCAN_CD;

    struct _STOP_PLAY_SCAN {
        UCHAR OperationCode;    // 0x4E - SCSIOP_STOP_PLAY_SCAN
        UCHAR Reserved1 : 5;
        UCHAR Lun : 3;
        UCHAR Reserved2[7];
        UCHAR Control;
    } STOP_PLAY_SCAN;


    //
    // Read SubChannel Data
    //

    struct _SUBCHANNEL {
        UCHAR OperationCode;    // 0x42 - SCSIOP_READ_SUB_CHANNEL
        UCHAR Reserved0 : 1;
        UCHAR Msf : 1;
        UCHAR Reserved1 : 3;
        UCHAR LogicalUnitNumber : 3;
        UCHAR Reserved2 : 6;
        UCHAR SubQ : 1;
        UCHAR Reserved3 : 1;
        UCHAR Format;
        UCHAR Reserved4[2];
        UCHAR TrackNumber;
        UCHAR AllocationLength[2];
        UCHAR Control;
    } SUBCHANNEL;

    //
    // Read CD. Used by Atapi for raw sector reads.
    //

    struct _READ_CD {
        UCHAR OperationCode;    // 0xBE - SCSIOP_READ_CD
        UCHAR RelativeAddress : 1;
        UCHAR Reserved0 : 1;
        UCHAR ExpectedSectorType : 3;
        UCHAR Lun : 3;
        UCHAR StartingLBA[4];
        UCHAR TransferBlocks[3];
        UCHAR Reserved2 : 1;
        UCHAR ErrorFlags : 2;
        UCHAR IncludeEDC : 1;
        UCHAR IncludeUserData : 1;
        UCHAR HeaderCode : 2;
        UCHAR IncludeSyncData : 1;
        UCHAR SubChannelSelection : 3;
        UCHAR Reserved3 : 5;
        UCHAR Control;
    } READ_CD;

    struct _READ_CD_MSF {
        UCHAR OperationCode;    // 0xB9 - SCSIOP_READ_CD_MSF
        UCHAR RelativeAddress : 1;
        UCHAR Reserved1 : 1;
        UCHAR ExpectedSectorType : 3;
        UCHAR Lun : 3;
        UCHAR Reserved2;
        UCHAR StartingM;
        UCHAR StartingS;
        UCHAR StartingF;
        UCHAR EndingM;
        UCHAR EndingS;
        UCHAR EndingF;
        UCHAR Reserved4 : 1;
        UCHAR ErrorFlags : 2;
        UCHAR IncludeEDC : 1;
        UCHAR IncludeUserData : 1;
        UCHAR HeaderCode : 2;
        UCHAR IncludeSyncData : 1;
        UCHAR SubChannelSelection : 3;
        UCHAR Reserved5 : 5;
        UCHAR Control;
    } READ_CD_MSF;

    //
    // Plextor Read CD-DA
    //

    struct _PLXTR_READ_CDDA {
        UCHAR OperationCode;    // Unknown -- vendor-unique?
        UCHAR Reserved0 : 5;
        UCHAR LogicalUnitNumber :3;
        UCHAR LogicalBlockByte0;
        UCHAR LogicalBlockByte1;
        UCHAR LogicalBlockByte2;
        UCHAR LogicalBlockByte3;
        UCHAR TransferBlockByte0;
        UCHAR TransferBlockByte1;
        UCHAR TransferBlockByte2;
        UCHAR TransferBlockByte3;
        UCHAR SubCode;
        UCHAR Control;
    } PLXTR_READ_CDDA;

    //
    // NEC Read CD-DA
    //

    struct _NEC_READ_CDDA {
        UCHAR OperationCode;    // Unknown -- vendor-unique?
        UCHAR Reserved0;
        UCHAR LogicalBlockByte0;
        UCHAR LogicalBlockByte1;
        UCHAR LogicalBlockByte2;
        UCHAR LogicalBlockByte3;
        UCHAR Reserved1;
        UCHAR TransferBlockByte0;
        UCHAR TransferBlockByte1;
        UCHAR Control;
    } NEC_READ_CDDA;

    //
    // Mode sense
    //

    struct _MODE_SENSE {
        UCHAR OperationCode;    // 0x1A - SCSIOP_MODE_SENSE
        UCHAR Reserved1 : 3;
        UCHAR Dbd : 1;
        UCHAR Reserved2 : 1;
        UCHAR LogicalUnitNumber : 3;
        UCHAR PageCode : 6;
        UCHAR Pc : 2;
        UCHAR Reserved3;
        UCHAR AllocationLength;
        UCHAR Control;
    } MODE_SENSE;

    struct _MODE_SENSE10 {
        UCHAR OperationCode;    // 0x5A - SCSIOP_MODE_SENSE10
        UCHAR Reserved1 : 3;
        UCHAR Dbd : 1;
        UCHAR Reserved2 : 1;
        UCHAR LogicalUnitNumber : 3;
        UCHAR PageCode : 6;
        UCHAR Pc : 2;
        UCHAR Reserved3[4];
        UCHAR AllocationLength[2];
        UCHAR Control;
    } MODE_SENSE10;

    //
    // Mode select
    //

    struct _MODE_SELECT {
        UCHAR OperationCode;    // 0x15 - SCSIOP_MODE_SELECT
        UCHAR SPBit : 1;
        UCHAR Reserved1 : 3;
        UCHAR PFBit : 1;
        UCHAR LogicalUnitNumber : 3;
        UCHAR Reserved2[2];
        UCHAR ParameterListLength;
        UCHAR Control;
    } MODE_SELECT;

    struct _MODE_SELECT10 {
        UCHAR OperationCode;    // 0x55 - SCSIOP_MODE_SELECT10
        UCHAR SPBit : 1;
        UCHAR Reserved1 : 3;
        UCHAR PFBit : 1;
        UCHAR LogicalUnitNumber : 3;
        UCHAR Reserved2[5];
        UCHAR ParameterListLength[2];
        UCHAR Control;
    } MODE_SELECT10;

    struct _LOCATE {
        UCHAR OperationCode;    // 0x2B - SCSIOP_LOCATE
        UCHAR Immediate : 1;
        UCHAR CPBit : 1;
        UCHAR BTBit : 1;
        UCHAR Reserved1 : 2;
        UCHAR LogicalUnitNumber : 3;
        UCHAR Reserved3;
        UCHAR LogicalBlockAddress[4];
        UCHAR Reserved4;
        UCHAR Partition;
        UCHAR Control;
    } LOCATE;

    struct _LOGSENSE {
        UCHAR OperationCode;    // 0x4D - SCSIOP_LOG_SENSE
        UCHAR SPBit : 1;
        UCHAR PPCBit : 1;
        UCHAR Reserved1 : 3;
        UCHAR LogicalUnitNumber : 3;
        UCHAR PageCode : 6;
        UCHAR PCBit : 2;
        UCHAR Reserved2;
        UCHAR Reserved3;
        UCHAR ParameterPointer[2];
        UCHAR AllocationLength[2];
        UCHAR Control;
    } LOGSENSE;

    struct _LOGSELECT {
        UCHAR OperationCode;    // 0x4C - SCSIOP_LOG_SELECT
        UCHAR SPBit : 1;
        UCHAR PCRBit : 1;
        UCHAR Reserved1 : 3;
        UCHAR LogicalUnitNumber : 3;
        UCHAR Reserved : 6;
        UCHAR PCBit : 2;
        UCHAR Reserved2[4];
        UCHAR ParameterListLength[2];
        UCHAR Control;
    } LOGSELECT;

    struct _PRINT {
        UCHAR OperationCode;    // 0x0A - SCSIOP_PRINT
        UCHAR Reserved : 5;
        UCHAR LogicalUnitNumber : 3;
        UCHAR TransferLength[3];
        UCHAR Control;
    } PRINT;

    struct _SEEK {
        UCHAR OperationCode;    // 0x2B - SCSIOP_SEEK
        UCHAR Reserved1 : 5;
        UCHAR LogicalUnitNumber : 3;
        UCHAR LogicalBlockAddress[4];
        UCHAR Reserved2[3];
        UCHAR Control;
    } SEEK;

    struct _ERASE {
        UCHAR OperationCode;    // 0x19 - SCSIOP_ERASE
        UCHAR Long : 1;
        UCHAR Immediate : 1;
        UCHAR Reserved1 : 3;
        UCHAR LogicalUnitNumber : 3;
        UCHAR Reserved2[3];
        UCHAR Control;
    } ERASE;

    struct _START_STOP {
        UCHAR OperationCode;    // 0x1B - SCSIOP_START_STOP_UNIT
        UCHAR Immediate: 1;
        UCHAR Reserved1 : 4;
        UCHAR LogicalUnitNumber : 3;
        UCHAR Reserved2[2];
        UCHAR Start : 1;
        UCHAR LoadEject : 1;
        UCHAR Reserved3 : 6;
        UCHAR Control;
    } START_STOP;

    struct _MEDIA_REMOVAL {
        UCHAR OperationCode;    // 0x1E - SCSIOP_MEDIUM_REMOVAL
        UCHAR Reserved1 : 5;
        UCHAR LogicalUnitNumber : 3;
        UCHAR Reserved2[2];

        UCHAR Prevent : 1;
        UCHAR Persistant : 1;
        UCHAR Reserved3 : 6;

        UCHAR Control;
    } MEDIA_REMOVAL;

    //
    // Tape CDBs
    //

    struct _SEEK_BLOCK {
        UCHAR OperationCode;    // 0x0C - SCSIOP_SEEK_BLOCK
        UCHAR Immediate : 1;
        UCHAR Reserved1 : 7;
        UCHAR BlockAddress[3];
        UCHAR Link : 1;
        UCHAR Flag : 1;
        UCHAR Reserved2 : 4;
        UCHAR VendorUnique : 2;
    } SEEK_BLOCK;

    struct _REQUEST_BLOCK_ADDRESS {
        UCHAR OperationCode;    // 0x02 - SCSIOP_REQUEST_BLOCK_ADDR
        UCHAR Reserved1[3];
        UCHAR AllocationLength;
        UCHAR Link : 1;
        UCHAR Flag : 1;
        UCHAR Reserved2 : 4;
        UCHAR VendorUnique : 2;
    } REQUEST_BLOCK_ADDRESS;

    struct _PARTITION {
        UCHAR OperationCode;    // 0x0D - SCSIOP_PARTITION
        UCHAR Immediate : 1;
        UCHAR Sel: 1;
        UCHAR PartitionSelect : 6;
        UCHAR Reserved1[3];
        UCHAR Control;
    } PARTITION;

    struct _WRITE_TAPE_MARKS {
        UCHAR OperationCode;    // Unknown -- vendor-unique?
        UCHAR Immediate : 1;
        UCHAR WriteSetMarks: 1;
        UCHAR Reserved : 3;
        UCHAR LogicalUnitNumber : 3;
        UCHAR TransferLength[3];
        UCHAR Control;
    } WRITE_TAPE_MARKS;

    struct _SPACE_TAPE_MARKS {
        UCHAR OperationCode;    // Unknown -- vendor-unique?
        UCHAR Code : 3;
        UCHAR Reserved : 2;
        UCHAR LogicalUnitNumber : 3;
        UCHAR NumMarksMSB ;
        UCHAR NumMarks;
        UCHAR NumMarksLSB;
        union {
            UCHAR value;
            struct {
                UCHAR Link : 1;
                UCHAR Flag : 1;
                UCHAR Reserved : 4;
                UCHAR VendorUnique : 2;
            } Fields;
        } Byte6;
    } SPACE_TAPE_MARKS;

    //
    // Read tape position
    //

    struct _READ_POSITION {
        UCHAR Operation;        // 0x43 - SCSIOP_READ_POSITION
        UCHAR BlockType:1;
        UCHAR Reserved1:4;
        UCHAR Lun:3;
        UCHAR Reserved2[7];
        UCHAR Control;
    } READ_POSITION;

    //
    // ReadWrite for Tape
    //

    struct _CDB6READWRITETAPE {
        UCHAR OperationCode;    // Unknown -- vendor-unique?
        UCHAR VendorSpecific : 5;
        UCHAR Reserved : 3;
        UCHAR TransferLenMSB;
        UCHAR TransferLen;
        UCHAR TransferLenLSB;
        UCHAR Link : 1;
        UCHAR Flag : 1;
        UCHAR Reserved1 : 4;
        UCHAR VendorUnique : 2;
    } CDB6READWRITETAPE;

    //
    // Medium changer CDB's
    //

    struct _INIT_ELEMENT_STATUS {
        UCHAR OperationCode;    // 0x07 - SCSIOP_INIT_ELEMENT_STATUS
        UCHAR Reserved1 : 5;
        UCHAR LogicalUnitNubmer : 3;
        UCHAR Reserved2[3];
        UCHAR Reserved3 : 7;
        UCHAR NoBarCode : 1;
    } INIT_ELEMENT_STATUS;

    struct _INITIALIZE_ELEMENT_RANGE {
        UCHAR OperationCode;    // 0xE7 - SCSIOP_INIT_ELEMENT_RANGE
        UCHAR Range : 1;
        UCHAR Reserved1 : 4;
        UCHAR LogicalUnitNubmer : 3;
        UCHAR FirstElementAddress[2];
        UCHAR Reserved2[2];
        UCHAR NumberOfElements[2];
        UCHAR Reserved3;
        UCHAR Reserved4 : 7;
        UCHAR NoBarCode : 1;
    } INITIALIZE_ELEMENT_RANGE;

    struct _POSITION_TO_ELEMENT {
        UCHAR OperationCode;    // 0x2B - SCSIOP_POSITION_TO_ELEMENT
        UCHAR Reserved1 : 5;
        UCHAR LogicalUnitNumber : 3;
        UCHAR TransportElementAddress[2];
        UCHAR DestinationElementAddress[2];
        UCHAR Reserved2[2];
        UCHAR Flip : 1;
        UCHAR Reserved3 : 7;
        UCHAR Control;
    } POSITION_TO_ELEMENT;

    struct _MOVE_MEDIUM {
        UCHAR OperationCode;    // 0xA5 - SCSIOP_MOVE_MEDIUM
        UCHAR Reserved1 : 5;
        UCHAR LogicalUnitNumber : 3;
        UCHAR TransportElementAddress[2];
        UCHAR SourceElementAddress[2];
        UCHAR DestinationElementAddress[2];
        UCHAR Reserved2[2];
        UCHAR Flip : 1;
        UCHAR Reserved3 : 7;
        UCHAR Control;
    } MOVE_MEDIUM;

    struct _EXCHANGE_MEDIUM {
        UCHAR OperationCode;    // 0xA6 - SCSIOP_EXCHANGE_MEDIUM
        UCHAR Reserved1 : 5;
        UCHAR LogicalUnitNumber : 3;
        UCHAR TransportElementAddress[2];
        UCHAR SourceElementAddress[2];
        UCHAR Destination1ElementAddress[2];
        UCHAR Destination2ElementAddress[2];
        UCHAR Flip1 : 1;
        UCHAR Flip2 : 1;
        UCHAR Reserved3 : 6;
        UCHAR Control;
    } EXCHANGE_MEDIUM;

    struct _READ_ELEMENT_STATUS {
        UCHAR OperationCode;    // 0xB8 - SCSIOP_READ_ELEMENT_STATUS
        UCHAR ElementType : 4;
        UCHAR VolTag : 1;
        UCHAR LogicalUnitNumber : 3;
        UCHAR StartingElementAddress[2];
        UCHAR NumberOfElements[2];
        UCHAR Reserved1;
        UCHAR AllocationLength[3];
        UCHAR Reserved2;
        UCHAR Control;
    } READ_ELEMENT_STATUS;

    struct _SEND_VOLUME_TAG {
        UCHAR OperationCode;    // 0xB6 - SCSIOP_SEND_VOLUME_TAG
        UCHAR ElementType : 4;
        UCHAR Reserved1 : 1;
        UCHAR LogicalUnitNumber : 3;
        UCHAR StartingElementAddress[2];
        UCHAR Reserved2;
        UCHAR ActionCode : 5;
        UCHAR Reserved3 : 3;
        UCHAR Reserved4[2];
        UCHAR ParameterListLength[2];
        UCHAR Reserved5;
        UCHAR Control;
    } SEND_VOLUME_TAG;

    struct _REQUEST_VOLUME_ELEMENT_ADDRESS {
        UCHAR OperationCode;    // Unknown -- vendor-unique?
        UCHAR ElementType : 4;
        UCHAR VolTag : 1;
        UCHAR LogicalUnitNumber : 3;
        UCHAR StartingElementAddress[2];
        UCHAR NumberElements[2];
        UCHAR Reserved1;
        UCHAR AllocationLength[3];
        UCHAR Reserved2;
        UCHAR Control;
    } REQUEST_VOLUME_ELEMENT_ADDRESS;

    //
    // Atapi 2.5 Changer 12-byte CDBs
    //

    struct _LOAD_UNLOAD {
        UCHAR OperationCode;    // 0xA6 - SCSIOP_LOAD_UNLOAD_SLOT
        UCHAR Immediate : 1;
        UCHAR Reserved1 : 4;
        UCHAR Lun : 3;
        UCHAR Reserved2[2];
        UCHAR Start : 1;
        UCHAR LoadEject : 1;
        UCHAR Reserved3: 6;
        UCHAR Reserved4[3];
        UCHAR Slot;
        UCHAR Reserved5[3];
    } LOAD_UNLOAD;

    struct _MECH_STATUS {
        UCHAR OperationCode;    // 0xBD - SCSIOP_MECHANISM_STATUS
        UCHAR Reserved : 5;
        UCHAR Lun : 3;
        UCHAR Reserved1[6];
        UCHAR AllocationLength[2];
        UCHAR Reserved2[1];
        UCHAR Control;
    } MECH_STATUS;

    //
    // C/DVD 0.9 CDBs
    //

    struct _SYNCHRONIZE_CACHE10 {

        UCHAR OperationCode;    // 0x35 - SCSIOP_SYNCHRONIZE_CACHE

        UCHAR RelAddr : 1;
        UCHAR Immediate : 1;
        UCHAR Reserved : 3;
        UCHAR Lun : 3;

        UCHAR LogicalBlockAddress[4];   // Unused - set to zero
        UCHAR Reserved2;
        UCHAR BlockCount[2];            // Unused - set to zero
        UCHAR Control;
    } SYNCHRONIZE_CACHE10;

    struct _GET_EVENT_STATUS_NOTIFICATION {
        UCHAR OperationCode;    // 0x4A - SCSIOP_GET_EVENT_STATUS_NOTIFICATION

        UCHAR Immediate : 1;
        UCHAR Reserved : 4;
        UCHAR Lun : 3;

        UCHAR Reserved2[2];
        UCHAR NotificationClassRequest;
        UCHAR Reserved3[2];
        UCHAR EventListLength[2];

        UCHAR Control;
    } GET_EVENT_STATUS_NOTIFICATION;

    struct _GET_PERFORMANCE {
        UCHAR OperationCode;    // 0xAC - SCSIOP_GET_PERFORMANCE
        UCHAR Except    : 2;
        UCHAR Write     : 1;
        UCHAR Tolerance : 2;
        UCHAR Reserved0 : 3;
        UCHAR StartingLBA[4];
        UCHAR Reserved1[2];
        UCHAR MaximumNumberOfDescriptors[2];
        UCHAR Type;
        UCHAR Control;
    } GET_PERFORMANCE;

    struct _READ_DVD_STRUCTURE {
        UCHAR OperationCode;    // 0xAD - SCSIOP_READ_DVD_STRUCTURE
        UCHAR Reserved1 : 5;
        UCHAR Lun : 3;
        UCHAR RMDBlockNumber[4];
        UCHAR LayerNumber;
        UCHAR Format;
        UCHAR AllocationLength[2];
        UCHAR Reserved3 : 6;
        UCHAR AGID : 2;
        UCHAR Control;
    } READ_DVD_STRUCTURE;

    struct _SET_STREAMING {
        UCHAR OperationCode;    // 0xB6 - SCSIOP_SET_STREAMING
        UCHAR Reserved[8];
        UCHAR ParameterListLength[2];
        UCHAR Control;
    } SET_STREAMING;

    struct _SEND_DVD_STRUCTURE {
        UCHAR OperationCode;    // 0xBF - SCSIOP_SEND_DVD_STRUCTURE
        UCHAR Reserved1 : 5;
        UCHAR Lun : 3;
        UCHAR Reserved2[5];
        UCHAR Format;
        UCHAR ParameterListLength[2];
        UCHAR Reserved3;
        UCHAR Control;
    } SEND_DVD_STRUCTURE;

    struct _SEND_KEY {
        UCHAR OperationCode;    // 0xA3 - SCSIOP_SEND_KEY
        UCHAR Reserved1 : 5;
        UCHAR Lun : 3;
        UCHAR Reserved2[6];
        UCHAR ParameterListLength[2];
        UCHAR KeyFormat : 6;
        UCHAR AGID : 2;
        UCHAR Control;
    } SEND_KEY;

    struct _REPORT_KEY {
        UCHAR OperationCode;    // 0xA4 - SCSIOP_REPORT_KEY
        UCHAR Reserved1 : 5;
        UCHAR Lun : 3;
        UCHAR LogicalBlockAddress[4];   // for title key
        UCHAR Reserved2[2];
        UCHAR AllocationLength[2];
        UCHAR KeyFormat : 6;
        UCHAR AGID : 2;
        UCHAR Control;
    } REPORT_KEY;

    struct _SET_READ_AHEAD {
        UCHAR OperationCode;    // 0xA7 - SCSIOP_SET_READ_AHEAD
        UCHAR Reserved1 : 5;
        UCHAR Lun : 3;
        UCHAR TriggerLBA[4];
        UCHAR ReadAheadLBA[4];
        UCHAR Reserved2;
        UCHAR Control;
    } SET_READ_AHEAD;

    struct _READ_FORMATTED_CAPACITIES {
        UCHAR OperationCode;    // 0x23 - SCSIOP_READ_FORMATTED_CAPACITY
        UCHAR Reserved1 : 5;
        UCHAR Lun : 3;
        UCHAR Reserved2[5];
        UCHAR AllocationLength[2];
        UCHAR Control;
    } READ_FORMATTED_CAPACITIES;

    //
    // SCSI-3
    //

    struct _REPORT_LUNS {
        UCHAR OperationCode;    // 0xA0 - SCSIOP_REPORT_LUNS
        UCHAR Reserved1[5];
        UCHAR AllocationLength[4];
        UCHAR Reserved2[1];
        UCHAR Control;
    } REPORT_LUNS;

    struct _PERSISTENT_RESERVE_IN {
        UCHAR OperationCode;    // 0x5E - SCSIOP_PERSISTENT_RESERVE_IN
        UCHAR ServiceAction : 5;
        UCHAR Reserved1 : 3;
        UCHAR Reserved2[5];
        UCHAR AllocationLength[2];
        UCHAR Control;
    } PERSISTENT_RESERVE_IN;

    struct _PERSISTENT_RESERVE_OUT {
        UCHAR OperationCode;    // 0x5F - SCSIOP_PERSISTENT_RESERVE_OUT
        UCHAR ServiceAction : 5;
        UCHAR Reserved1 : 3;
        UCHAR Type : 4;
        UCHAR Scope : 4;
        UCHAR Reserved2[4];
        UCHAR ParameterListLength[2]; // 0x18
        UCHAR Control;
    } PERSISTENT_RESERVE_OUT;

    //
    // MMC / SFF-8090 commands
    //

    struct _GET_CONFIGURATION {
        UCHAR OperationCode;       // 0x46 - SCSIOP_GET_CONFIGURATION
        UCHAR RequestType : 2;     // SCSI_GET_CONFIGURATION_REQUEST_TYPE_*
        UCHAR Reserved1   : 6;     // includes obsolete LUN field
        UCHAR StartingFeature[2];
        UCHAR Reserved2[3];
        UCHAR AllocationLength[2];
        UCHAR Control;
    } GET_CONFIGURATION;

    struct _SET_CD_SPEED {
        UCHAR OperationCode;       // 0xB8 - SCSIOP_SET_CD_SPEED
        union {
            UCHAR Reserved1;
            struct {
                UCHAR RotationControl : 2;
                UCHAR Reserved3       : 6;
            };
        };
        UCHAR ReadSpeed[2];        // 1x == (75 * 2352)
        UCHAR WriteSpeed[2];       // 1x == (75 * 2352)
        UCHAR Reserved2[5];
        UCHAR Control;
    } SET_CD_SPEED;

    struct _READ12 {
        UCHAR OperationCode;      // 0xA8 - SCSIOP_READ12
        UCHAR RelativeAddress   : 1;
        UCHAR Reserved1         : 2;
        UCHAR ForceUnitAccess   : 1;
        UCHAR DisablePageOut    : 1;
        UCHAR LogicalUnitNumber : 3;
        UCHAR LogicalBlock[4];
        UCHAR TransferLength[4];
        UCHAR Reserved2 : 7;
        UCHAR Streaming : 1;
        UCHAR Control;
    } READ12;

    struct _WRITE12 {
        UCHAR OperationCode;      // 0xAA - SCSIOP_WRITE12
        UCHAR RelativeAddress   : 1;
        UCHAR Reserved1         : 1;
        UCHAR EBP               : 1;
        UCHAR ForceUnitAccess   : 1;
        UCHAR DisablePageOut    : 1;
        UCHAR LogicalUnitNumber : 3;
        UCHAR LogicalBlock[4];
        UCHAR TransferLength[4];
        UCHAR Reserved2 : 7;
        UCHAR Streaming : 1;
        UCHAR Control;
    } WRITE12;

    //
    // 16-byte CDBs
    //

    struct _READ16 {
        UCHAR OperationCode;      // 0x88 - SCSIOP_READ16
        UCHAR Reserved1         : 3;
        UCHAR ForceUnitAccess   : 1;
        UCHAR DisablePageOut    : 1;
        UCHAR ReadProtect       : 3;
        UCHAR LogicalBlock[8];
        UCHAR TransferLength[4];
        UCHAR Reserved2         : 7;
        UCHAR Streaming         : 1;
        UCHAR Control;
    } READ16;

    struct _WRITE16 {
        UCHAR OperationCode;      // 0x8A - SCSIOP_WRITE16
        UCHAR Reserved1         : 3;
        UCHAR ForceUnitAccess   : 1;
        UCHAR DisablePageOut    : 1;
        UCHAR WriteProtect      : 3;
        UCHAR LogicalBlock[8];
        UCHAR TransferLength[4];
        UCHAR Reserved2         : 7;
        UCHAR Streaming         : 1;
        UCHAR Control;
    } WRITE16;

    struct _VERIFY16 {
        UCHAR OperationCode;      // 0x8F - SCSIOP_VERIFY16
        UCHAR Reserved1         : 1;
        UCHAR ByteCheck         : 1;
        UCHAR BlockVerify       : 1;
        UCHAR Reserved2         : 1;
        UCHAR DisablePageOut    : 1;
        UCHAR VerifyProtect     : 3;
        UCHAR LogicalBlock[8];
        UCHAR VerificationLength[4];
        UCHAR Reserved3         : 7;
        UCHAR Streaming         : 1;
        UCHAR Control;
    } VERIFY16;

    struct _SYNCHRONIZE_CACHE16 {
        UCHAR OperationCode;      // 0x91 - SCSIOP_SYNCHRONIZE_CACHE16
        UCHAR Reserved1         : 1;
        UCHAR Immediate         : 1;
        UCHAR Reserved2         : 6;
        UCHAR LogicalBlock[8];
        UCHAR BlockCount[4];
        UCHAR Reserved3;
        UCHAR Control;
    } SYNCHRONIZE_CACHE16;

    struct _READ_CAPACITY16 {
        UCHAR OperationCode;      // 0x9E - SCSIOP_READ_CAPACITY16
        UCHAR ServiceAction     : 5;
        UCHAR Reserved1         : 3;
        UCHAR LogicalBlock[8];
        UCHAR BlockCount[4];
        UCHAR PMI               : 1;
        UCHAR Reserved2         : 7;
        UCHAR Control;
    } READ_CAPACITY16;

    ULONG AsUlong[4];
    UCHAR AsByte[16];

} CDB, *PCDB;
#pragma pack(pop, cdb)

////////////////////////////////////////////////////////////////////////////////
//
// GET_EVENT_STATUS_NOTIFICATION
//


#define NOTIFICATION_OPERATIONAL_CHANGE_CLASS_MASK  0x02
#define NOTIFICATION_POWER_MANAGEMENT_CLASS_MASK    0x04
#define NOTIFICATION_EXTERNAL_REQUEST_CLASS_MASK    0x08
#define NOTIFICATION_MEDIA_STATUS_CLASS_MASK        0x10
#define NOTIFICATION_MULTI_HOST_CLASS_MASK          0x20
#define NOTIFICATION_DEVICE_BUSY_CLASS_MASK         0x40


#define NOTIFICATION_NO_CLASS_EVENTS                  0x0
#define NOTIFICATION_OPERATIONAL_CHANGE_CLASS_EVENTS  0x1
#define NOTIFICATION_POWER_MANAGEMENT_CLASS_EVENTS    0x2
#define NOTIFICATION_EXTERNAL_REQUEST_CLASS_EVENTS    0x3
#define NOTIFICATION_MEDIA_STATUS_CLASS_EVENTS        0x4
#define NOTIFICATION_MULTI_HOST_CLASS_EVENTS          0x5
#define NOTIFICATION_DEVICE_BUSY_CLASS_EVENTS         0x6

#pragma pack(push, not_header, 1)
typedef struct _NOTIFICATION_EVENT_STATUS_HEADER {
    UCHAR EventDataLength[2];

    UCHAR NotificationClass : 3;
    UCHAR Reserved : 4;
    UCHAR NEA : 1;

    UCHAR SupportedEventClasses;
#if !defined(__midl)
    UCHAR ClassEventData[0];
#endif
} NOTIFICATION_EVENT_STATUS_HEADER, *PNOTIFICATION_EVENT_STATUS_HEADER;
#pragma pack(pop, not_header)

#define NOTIFICATION_OPERATIONAL_EVENT_NO_CHANGE         0x0
#define NOTIFICATION_OPERATIONAL_EVENT_CHANGE_REQUESTED  0x1
#define NOTIFICATION_OPERATIONAL_EVENT_CHANGE_OCCURRED   0x2

#define NOTIFICATION_OPERATIONAL_STATUS_AVAILABLE        0x0
#define NOTIFICATION_OPERATIONAL_STATUS_TEMPORARY_BUSY   0x1
#define NOTIFICATION_OPERATIONAL_STATUS_EXTENDED_BUSY    0x2

#define NOTIFICATION_OPERATIONAL_OPCODE_NONE             0x0
#define NOTIFICATION_OPERATIONAL_OPCODE_FEATURE_CHANGE   0x1
#define NOTIFICATION_OPERATIONAL_OPCODE_FEATURE_ADDED    0x2
#define NOTIFICATION_OPERATIONAL_OPCODE_UNIT_RESET       0x3
#define NOTIFICATION_OPERATIONAL_OPCODE_FIRMWARE_CHANGED 0x4
#define NOTIFICATION_OPERATIONAL_OPCODE_INQUIRY_CHANGED  0x5

//
// Class event data may be one (or none) of the following:
//

#pragma pack(push, not_op, 1)
typedef struct _NOTIFICATION_OPERATIONAL_STATUS { // event class == 0x1
    UCHAR OperationalEvent : 4;
    UCHAR Reserved1 : 4;
    UCHAR OperationalStatus : 4;
    UCHAR Reserved2 : 3;
    UCHAR PersistentPrevented : 1;
    UCHAR Operation[2];
} NOTIFICATION_OPERATIONAL_STATUS, *PNOTIFICATION_OPERATIONAL_STATUS;
#pragma pack(pop, not_op)


#define NOTIFICATION_POWER_EVENT_NO_CHANGE          0x0
#define NOTIFICATION_POWER_EVENT_CHANGE_SUCCEEDED   0x1
#define NOTIFICATION_POWER_EVENT_CHANGE_FAILED      0x2

#define NOTIFICATION_POWER_STATUS_ACTIVE            0x1
#define NOTIFICATION_POWER_STATUS_IDLE              0x2
#define NOTIFICATION_POWER_STATUS_STANDBY           0x3
#define NOTIFICATION_POWER_STATUS_SLEEP             0x4

#pragma pack(push, not_power, 1)
typedef struct _NOTIFICATION_POWER_STATUS { // event class == 0x2
    UCHAR PowerEvent : 4;
    UCHAR Reserved : 4;
    UCHAR PowerStatus;
    UCHAR Reserved2[2];
} NOTIFICATION_POWER_STATUS, *PNOTIFICATION_POWER_STATUS;
#pragma pack(pop, not_power)

#define NOTIFICATION_MEDIA_EVENT_NO_EVENT           0x0
#define NOTIFICATION_EXTERNAL_EVENT_NO_CHANGE       0x0
#define NOTIFICATION_EXTERNAL_EVENT_BUTTON_DOWN     0x1
#define NOTIFICATION_EXTERNAL_EVENT_BUTTON_UP       0x2
#define NOTIFICATION_EXTERNAL_EVENT_EXTERNAL        0x3 // respond with GET_CONFIGURATION?

#define NOTIFICATION_EXTERNAL_STATUS_READY          0x0
#define NOTIFICATION_EXTERNAL_STATUS_PREVENT        0x1

#define NOTIFICATION_EXTERNAL_REQUEST_NONE          0x0000
#define NOTIFICATION_EXTERNAL_REQUEST_QUEUE_OVERRUN 0x0001
#define NOTIFICATION_EXTERNAL_REQUEST_PLAY          0x0101
#define NOTIFICATION_EXTERNAL_REQUEST_REWIND_BACK   0x0102
#define NOTIFICATION_EXTERNAL_REQUEST_FAST_FORWARD  0x0103
#define NOTIFICATION_EXTERNAL_REQUEST_PAUSE         0x0104
#define NOTIFICATION_EXTERNAL_REQUEST_STOP          0x0106
#define NOTIFICATION_EXTERNAL_REQUEST_ASCII_LOW     0x0200
#define NOTIFICATION_EXTERNAL_REQUEST_ASCII_HIGH    0x02ff

#pragma pack(push, not_extern, 1)
typedef struct _NOTIFICATION_EXTERNAL_STATUS { // event class == 0x3
    UCHAR ExternalEvent : 4;
    UCHAR Reserved1 : 4;
    UCHAR ExternalStatus : 4;
    UCHAR Reserved2 : 3;
    UCHAR PersistentPrevented : 1;
    UCHAR Request[2];
} NOTIFICATION_EXTERNAL_STATUS, *PNOTIFICATION_EXTERNAL_STATUS;
#pragma pack(pop, not_extern)

#define NOTIFICATION_MEDIA_EVENT_NO_CHANGE          0x0
#define NOTIFICATION_MEDIA_EVENT_EJECT_REQUEST      0x1
#define NOTIFICATION_MEDIA_EVENT_NEW_MEDIA          0x2
#define NOTIFICATION_MEDIA_EVENT_MEDIA_REMOVAL      0x3
#define NOTIFICATION_MEDIA_EVENT_MEDIA_CHANGE       0x4

#pragma pack(push, not_media, 1)
typedef struct _NOTIFICATION_MEDIA_STATUS { // event class == 0x4
    UCHAR MediaEvent : 4;
    UCHAR Reserved : 4;

    union {
        UCHAR PowerStatus; // OBSOLETE -- was improperly named in NT5 headers
        UCHAR MediaStatus; // Use this for currently reserved fields
        struct {
            UCHAR DoorTrayOpen : 1;
            UCHAR MediaPresent : 1;
            UCHAR ReservedX    : 6; // do not reference this directly!
        };
    };
    UCHAR StartSlot;
    UCHAR EndSlot;
} NOTIFICATION_MEDIA_STATUS, *PNOTIFICATION_MEDIA_STATUS;
#pragma pack(pop, not_media)

#define NOTIFICATION_BUSY_EVENT_NO_EVENT               0x0
#define NOTIFICATION_MULTI_HOST_EVENT_NO_CHANGE        0x0
#define NOTIFICATION_MULTI_HOST_EVENT_CONTROL_REQUEST  0x1
#define NOTIFICATION_MULTI_HOST_EVENT_CONTROL_GRANT    0x2
#define NOTIFICATION_MULTI_HOST_EVENT_CONTROL_RELEASE  0x3

#define NOTIFICATION_MULTI_HOST_STATUS_READY           0x0
#define NOTIFICATION_MULTI_HOST_STATUS_PREVENT         0x1

#define NOTIFICATION_MULTI_HOST_PRIORITY_NO_REQUESTS   0x0
#define NOTIFICATION_MULTI_HOST_PRIORITY_LOW           0x1
#define NOTIFICATION_MULTI_HOST_PRIORITY_MEDIUM        0x2
#define NOTIFICATION_MULTI_HOST_PRIORITY_HIGH          0x3

#pragma pack(push, not_multi, 1)
typedef struct _NOTIFICATION_MULTI_HOST_STATUS { // event class == 0x5
    UCHAR MultiHostEvent : 4;
    UCHAR Reserved1 : 4;
    UCHAR MultiHostStatus : 4;
    UCHAR Reserved2 : 3;
    UCHAR PersistentPrevented : 1;
    UCHAR Priority[2];
} NOTIFICATION_MULTI_HOST_STATUS, *PNOTIFICATION_MULTI_HOST_STATUS;
#pragma pack(pop, not_multi)

#define NOTIFICATION_BUSY_EVENT_NO_EVENT            0x0
#define NOTIFICATION_BUSY_EVENT_NO_CHANGE           0x0
#define NOTIFICATION_BUSY_EVENT_BUSY                0x1

#define NOTIFICATION_BUSY_STATUS_NO_EVENT           0x0
#define NOTIFICATION_BUSY_STATUS_POWER              0x1
#define NOTIFICATION_BUSY_STATUS_IMMEDIATE          0x2
#define NOTIFICATION_BUSY_STATUS_DEFERRED           0x3

#pragma pack(push, not_busy, 1)
typedef struct _NOTIFICATION_BUSY_STATUS { // event class == 0x6
    UCHAR DeviceBusyEvent : 4;
    UCHAR Reserved : 4;

    UCHAR DeviceBusyStatus;
    UCHAR Time[2];
} NOTIFICATION_BUSY_STATUS, *PNOTIFICATION_BUSY_STATUS;
#pragma pack(pop, not_busy)

////////////////////////////////////////////////////////////////////////////////

//
// Read DVD Structure Definitions and Constants
//

#define DVD_FORMAT_LEAD_IN          0x00
#define DVD_FORMAT_COPYRIGHT        0x01
#define DVD_FORMAT_DISK_KEY         0x02
#define DVD_FORMAT_BCA              0x03
#define DVD_FORMAT_MANUFACTURING    0x04

#pragma pack(push, dvd_struct_header, 1)
typedef struct _READ_DVD_STRUCTURES_HEADER {
    UCHAR Length[2];
    UCHAR Reserved[2];

#if !defined(__midl)
    UCHAR Data[0];
#endif
} READ_DVD_STRUCTURES_HEADER, *PREAD_DVD_STRUCTURES_HEADER;
#pragma pack(pop, dvd_struct_header)

//
// DiskKey, BCA & Manufacturer information will provide byte arrays as their
// data.
//

//
// CDVD 0.9 Send & Report Key Definitions and Structures
//

#define DVD_REPORT_AGID            0x00
#define DVD_CHALLENGE_KEY          0x01
#define DVD_KEY_1                  0x02
#define DVD_KEY_2                  0x03
#define DVD_TITLE_KEY              0x04
#define DVD_REPORT_ASF             0x05
#define DVD_INVALIDATE_AGID        0x3F

#pragma pack(push, dvdstuff, 1)
typedef struct _CDVD_KEY_HEADER {
    UCHAR DataLength[2];
    UCHAR Reserved[2];
#if !defined(__midl)
    UCHAR Data[0];
#endif
} CDVD_KEY_HEADER, *PCDVD_KEY_HEADER;

typedef struct _CDVD_REPORT_AGID_DATA {
    UCHAR Reserved1[3];
    UCHAR Reserved2 : 6;
    UCHAR AGID : 2;
} CDVD_REPORT_AGID_DATA, *PCDVD_REPORT_AGID_DATA;

typedef struct _CDVD_CHALLENGE_KEY_DATA {
    UCHAR ChallengeKeyValue[10];
    UCHAR Reserved[2];
} CDVD_CHALLENGE_KEY_DATA, *PCDVD_CHALLENGE_KEY_DATA;

typedef struct _CDVD_KEY_DATA {
    UCHAR Key[5];
    UCHAR Reserved[3];
} CDVD_KEY_DATA, *PCDVD_KEY_DATA;

typedef struct _CDVD_REPORT_ASF_DATA {
    UCHAR Reserved1[3];
    UCHAR Success : 1;
    UCHAR Reserved2 : 7;
} CDVD_REPORT_ASF_DATA, *PCDVD_REPORT_ASF_DATA;

typedef struct _CDVD_TITLE_KEY_HEADER {
    UCHAR DataLength[2];
    UCHAR Reserved1[1];
    UCHAR Reserved2 : 3;
    UCHAR CGMS : 2;
    UCHAR CP_SEC : 1;
    UCHAR CPM : 1;
    UCHAR Zero : 1;
    CDVD_KEY_DATA TitleKey;
} CDVD_TITLE_KEY_HEADER, *PCDVD_TITLE_KEY_HEADER;
#pragma pack(pop, dvdstuff)


//
// Format Unit Data definitions and structures
//

#pragma pack(push, format_unit, 1)
typedef struct _FORMAT_DESCRIPTOR {
    UCHAR NumberOfBlocks[4];
    UCHAR FormatSubType : 2;
    UCHAR FormatType : 6;
    UCHAR BlockLength[3];
} FORMAT_DESCRIPTOR, *PFORMAT_DESCRIPTOR;

typedef struct _FORMAT_LIST_HEADER {
    UCHAR Reserved;
    UCHAR VendorSpecific : 1;
    UCHAR Immediate : 1;
    UCHAR TryOut : 1;
    UCHAR IP : 1;
    UCHAR STPF : 1;
    UCHAR DCRT : 1;
    UCHAR DPRY : 1;
    UCHAR FOV : 1;
    UCHAR FormatDescriptorLength[2];
#if !defined(__midl)
    FORMAT_DESCRIPTOR Descriptors[0];
#endif
} FORMAT_LIST_HEADER, *PFORMAT_LIST_HEADER;
#pragma pack(pop, format_unit)

//
// Read Formatted Capacity Data - returned in Big Endian Format
//


#pragma pack(push, formatted_capacity, 1)
typedef struct _FORMATTED_CAPACITY_DESCRIPTOR {
    UCHAR NumberOfBlocks[4];
    UCHAR Maximum : 1;
    UCHAR Valid : 1;
    UCHAR FormatType : 6;
    UCHAR BlockLength[3];
} FORMATTED_CAPACITY_DESCRIPTOR, *PFORMATTED_CAPACITY_DESCRIPTOR;

typedef struct _FORMATTED_CAPACITY_LIST {
    UCHAR Reserved[3];
    UCHAR CapacityListLength;
#if !defined(__midl)
    FORMATTED_CAPACITY_DESCRIPTOR Descriptors[0];
#endif
} FORMATTED_CAPACITY_LIST, *PFORMATTED_CAPACITY_LIST;
#pragma pack(pop, formatted_capacity)

//
//      BLANK command blanking type codes
//

#define BLANK_FULL              0x0
#define BLANK_MINIMAL           0x1
#define BLANK_TRACK             0x2
#define BLANK_UNRESERVE_TRACK   0x3
#define BLANK_TAIL              0x4
#define BLANK_UNCLOSE_SESSION   0x5
#define BLANK_SESSION           0x6

//
// PLAY_CD definitions and constants
//

#define CD_EXPECTED_SECTOR_ANY          0x0
#define CD_EXPECTED_SECTOR_CDDA         0x1
#define CD_EXPECTED_SECTOR_MODE1        0x2
#define CD_EXPECTED_SECTOR_MODE2        0x3
#define CD_EXPECTED_SECTOR_MODE2_FORM1  0x4
#define CD_EXPECTED_SECTOR_MODE2_FORM2  0x5

//
// Read Disk Information Definitions and Capabilities
//

#define DISK_STATUS_EMPTY       0x00
#define DISK_STATUS_INCOMPLETE  0x01
#define DISK_STATUS_COMPLETE    0x02
#define DISK_STATUS_OTHERS      0x03

#define LAST_SESSION_EMPTY              0x00
#define LAST_SESSION_INCOMPLETE         0x01
#define LAST_SESSION_RESERVED_DAMAGED   0x02
#define LAST_SESSION_COMPLETE           0x03

#define DISK_TYPE_CDDA          0x00
#define DISK_TYPE_CDI           0x10
#define DISK_TYPE_XA            0x20
#define DISK_TYPE_UNDEFINED     0xFF

//
//  Values for MrwStatus field.
//

#define DISC_BGFORMAT_STATE_NONE        0x0
#define DISC_BGFORMAT_STATE_INCOMPLETE  0x1
#define DISC_BGFORMAT_STATE_RUNNING     0x2
#define DISC_BGFORMAT_STATE_COMPLETE    0x3


#pragma pack(push, discinfo, 1)
typedef struct _OPC_TABLE_ENTRY {
    UCHAR Speed[2];
    UCHAR OPCValue[6];
} OPC_TABLE_ENTRY, *POPC_TABLE_ENTRY;

typedef struct _DISC_INFORMATION {

    UCHAR Length[2];
    UCHAR DiscStatus        : 2;
    UCHAR LastSessionStatus : 2;
    UCHAR Erasable          : 1;
    UCHAR Reserved1         : 3;
    UCHAR FirstTrackNumber;

    UCHAR NumberOfSessionsLsb;
    UCHAR LastSessionFirstTrackLsb;
    UCHAR LastSessionLastTrackLsb;
    UCHAR MrwStatus   : 2;
    UCHAR MrwDirtyBit : 1;
    UCHAR Reserved2   : 2;
    UCHAR URU         : 1;
    UCHAR DBC_V       : 1;
    UCHAR DID_V       : 1;

    UCHAR DiscType;
    UCHAR NumberOfSessionsMsb;
    UCHAR LastSessionFirstTrackMsb;
    UCHAR LastSessionLastTrackMsb;

    UCHAR DiskIdentification[4];
    UCHAR LastSessionLeadIn[4];     // HMSF
    UCHAR LastPossibleLeadOutStartTime[4]; // HMSF
    UCHAR DiskBarCode[8];

    UCHAR Reserved4;
    UCHAR NumberOPCEntries;
    OPC_TABLE_ENTRY OPCTable[ 1 ]; // can be many of these here....

} DISC_INFORMATION, *PDISC_INFORMATION;

// TODO: Deprecate DISK_INFORMATION
//#if PRAGMA_DEPRECATED_DDK
//#pragma deprecated(_DISK_INFORMATION)  // Use DISC_INFORMATION, note size change
//#pragma deprecated( DISK_INFORMATION)  // Use DISC_INFORMATION, note size change
//#pragma deprecated(PDISK_INFORMATION)  // Use DISC_INFORMATION, note size change
//#endif

typedef struct _DISK_INFORMATION {
    UCHAR Length[2];

    UCHAR DiskStatus : 2;
    UCHAR LastSessionStatus : 2;
    UCHAR Erasable : 1;
    UCHAR Reserved1 : 3;

    UCHAR FirstTrackNumber;
    UCHAR NumberOfSessions;
    UCHAR LastSessionFirstTrack;
    UCHAR LastSessionLastTrack;

    UCHAR Reserved2 : 5;
    UCHAR GEN : 1;
    UCHAR DBC_V : 1;
    UCHAR DID_V : 1;

    UCHAR DiskType;
    UCHAR Reserved3[3];

    UCHAR DiskIdentification[4];
    UCHAR LastSessionLeadIn[4];     // MSF
    UCHAR LastPossibleStartTime[4]; // MSF
    UCHAR DiskBarCode[8];

    UCHAR Reserved4;
    UCHAR NumberOPCEntries;
#if !defined(__midl)
    OPC_TABLE_ENTRY OPCTable[0];
#endif
} DISK_INFORMATION, *PDISK_INFORMATION;
#pragma pack(pop, discinfo)


//
// Read Header definitions and structures
//
#pragma pack(push, cdheader, 1)
typedef struct _DATA_BLOCK_HEADER {
    UCHAR DataMode;
    UCHAR Reserved[4];
    union {
        UCHAR LogicalBlockAddress[4];
        struct {
            UCHAR Reserved;
            UCHAR M;
            UCHAR S;
            UCHAR F;
        } MSF;
    };
} DATA_BLOCK_HEADER, *PDATA_BLOCK_HEADER;
#pragma pack(pop, cdheader)


#define DATA_BLOCK_MODE0    0x0
#define DATA_BLOCK_MODE1    0x1
#define DATA_BLOCK_MODE2    0x2

//
// Read TOC Format Codes
//

#define READ_TOC_FORMAT_TOC         0x00
#define READ_TOC_FORMAT_SESSION     0x01
#define READ_TOC_FORMAT_FULL_TOC    0x02
#define READ_TOC_FORMAT_PMA         0x03
#define READ_TOC_FORMAT_ATIP        0x04

// TODO: Deprecate TRACK_INFORMATION structure, use TRACK_INFORMATION2 instead
#pragma pack(push, track_info, 1)
typedef struct _TRACK_INFORMATION {
    UCHAR Length[2];
    UCHAR TrackNumber;
    UCHAR SessionNumber;
    UCHAR Reserved1;
    UCHAR TrackMode : 4;
    UCHAR Copy      : 1;
    UCHAR Damage    : 1;
    UCHAR Reserved2 : 2;
    UCHAR DataMode : 4;
    UCHAR FP       : 1;
    UCHAR Packet   : 1;
    UCHAR Blank    : 1;
    UCHAR RT       : 1;
    UCHAR NWA_V     : 1;
    UCHAR Reserved3 : 7;
    UCHAR TrackStartAddress[4];
    UCHAR NextWritableAddress[4];
    UCHAR FreeBlocks[4];
    UCHAR FixedPacketSize[4];
} TRACK_INFORMATION, *PTRACK_INFORMATION;

// Second Revision Modifies:
// * Longer names for some fields
// * LSB to track/session number fields
// * LRA_V bit
// Second Revision Adds:
// * TrackSize
// * LastRecordedAddress
// * MSB to track/session
// * Two reserved bytes
// Total structure size increased by 12 (0x0C) bytes
typedef struct _TRACK_INFORMATION2 {

    UCHAR Length[2];
    UCHAR TrackNumberLsb;
    UCHAR SessionNumberLsb;

    UCHAR Reserved4;
    UCHAR TrackMode : 4;
    UCHAR Copy      : 1;
    UCHAR Damage    : 1;
    UCHAR Reserved5 : 2;
    UCHAR DataMode      : 4;
    UCHAR FixedPacket   : 1;
    UCHAR Packet        : 1;
    UCHAR Blank         : 1;
    UCHAR ReservedTrack : 1;
    UCHAR NWA_V     : 1;
    UCHAR LRA_V     : 1;
    UCHAR Reserved6 : 6;

    UCHAR TrackStartAddress[4];
    UCHAR NextWritableAddress[4];
    UCHAR FreeBlocks[4];
    UCHAR FixedPacketSize[4]; // blocking factor
    UCHAR TrackSize[4];
    UCHAR LastRecordedAddress[4];

    UCHAR TrackNumberMsb;
    UCHAR SessionNumberMsb;
    UCHAR Reserved7[2];

} TRACK_INFORMATION2, *PTRACK_INFORMATION2;

// Third Revision Adds
// * ReadCompatibilityLBA
// Total structure size increased by 4 bytes
typedef struct _TRACK_INFORMATION3 {

    UCHAR Length[2];
    UCHAR TrackNumberLsb;
    UCHAR SessionNumberLsb;

    UCHAR Reserved4;
    UCHAR TrackMode : 4;
    UCHAR Copy      : 1;
    UCHAR Damage    : 1;
    UCHAR Reserved5 : 2;
    UCHAR DataMode      : 4;
    UCHAR FixedPacket   : 1;
    UCHAR Packet        : 1;
    UCHAR Blank         : 1;
    UCHAR ReservedTrack : 1;
    UCHAR NWA_V     : 1;
    UCHAR LRA_V     : 1;
    UCHAR Reserved6 : 6;

    UCHAR TrackStartAddress[4];
    UCHAR NextWritableAddress[4];
    UCHAR FreeBlocks[4];
    UCHAR FixedPacketSize[4]; // blocking factor
    UCHAR TrackSize[4];
    UCHAR LastRecordedAddress[4];

    UCHAR TrackNumberMsb;
    UCHAR SessionNumberMsb;
    UCHAR Reserved7[2];
    UCHAR ReadCompatibilityLba[4];

} TRACK_INFORMATION3, *PTRACK_INFORMATION3;

#pragma pack(pop, track_info)

#pragma pack(push, perf_descriptor, 1)
typedef struct _PERFORMANCE_DESCRIPTOR {

    UCHAR RandomAccess         : 1;
    UCHAR Exact                : 1;
    UCHAR RestoreDefaults      : 1;
    UCHAR WriteRotationControl : 2;
    UCHAR Reserved1            : 3;

    UCHAR Reserved[3];
    UCHAR StartLba[4];
    UCHAR EndLba[4];
    UCHAR ReadSize[4];
    UCHAR ReadTime[4];
    UCHAR WriteSize[4];
    UCHAR WriteTime[4];

} PERFORMANCE_DESCRIPTOR, *PPERFORMANCE_DESCRIPTOR;
#pragma pack(pop, perf_descriptor)

//
// Command Descriptor Block constants.
//

#define CDB6GENERIC_LENGTH                   6
#define CDB10GENERIC_LENGTH                  10
#define CDB12GENERIC_LENGTH                  12

#define SETBITON                             1
#define SETBITOFF                            0

//
// Mode Sense/Select page constants.
//

#define MODE_PAGE_VENDOR_SPECIFIC       0x00
#define MODE_PAGE_ERROR_RECOVERY        0x01
#define MODE_PAGE_DISCONNECT            0x02
#define MODE_PAGE_FORMAT_DEVICE         0x03 // disk
#define MODE_PAGE_MRW                   0x03 // cdrom
#define MODE_PAGE_RIGID_GEOMETRY        0x04
#define MODE_PAGE_FLEXIBILE             0x05 // disk
#define MODE_PAGE_WRITE_PARAMETERS      0x05 // cdrom
#define MODE_PAGE_VERIFY_ERROR          0x07
#define MODE_PAGE_CACHING               0x08
#define MODE_PAGE_PERIPHERAL            0x09
#define MODE_PAGE_CONTROL               0x0A
#define MODE_PAGE_MEDIUM_TYPES          0x0B
#define MODE_PAGE_NOTCH_PARTITION       0x0C
#define MODE_PAGE_CD_AUDIO_CONTROL      0x0E
#define MODE_PAGE_DATA_COMPRESS         0x0F
#define MODE_PAGE_DEVICE_CONFIG         0x10
#define MODE_PAGE_XOR_CONTROL           0x10 // disk
#define MODE_PAGE_MEDIUM_PARTITION      0x11
#define MODE_PAGE_ENCLOSURE_SERVICES_MANAGEMENT 0x14
#define MODE_PAGE_EXTENDED              0x15
#define MODE_PAGE_EXTENDED_DEVICE_SPECIFIC 0x16
#define MODE_PAGE_CDVD_FEATURE_SET      0x18
#define MODE_PAGE_PROTOCOL_SPECIFIC_LUN 0x18
#define MODE_PAGE_PROTOCOL_SPECIFIC_PORT 0x19
#define MODE_PAGE_POWER_CONDITION       0x1A
#define MODE_PAGE_LUN_MAPPING           0x1B
#define MODE_PAGE_FAULT_REPORTING       0x1C
#define MODE_PAGE_CDVD_INACTIVITY       0x1D // cdrom
#define MODE_PAGE_ELEMENT_ADDRESS       0x1D
#define MODE_PAGE_TRANSPORT_GEOMETRY    0x1E
#define MODE_PAGE_DEVICE_CAPABILITIES   0x1F
#define MODE_PAGE_CAPABILITIES          0x2A // cdrom

#define MODE_SENSE_RETURN_ALL           0x3f

#define MODE_SENSE_CURRENT_VALUES       0x00
#define MODE_SENSE_CHANGEABLE_VALUES    0x40
#define MODE_SENSE_DEFAULT_VAULES       0x80
#define MODE_SENSE_SAVED_VALUES         0xc0


//
// SCSI CDB operation codes
//

// 6-byte commands:
#define SCSIOP_TEST_UNIT_READY          0x00
#define SCSIOP_REZERO_UNIT              0x01
#define SCSIOP_REWIND                   0x01
#define SCSIOP_REQUEST_BLOCK_ADDR       0x02
#define SCSIOP_REQUEST_SENSE            0x03
#define SCSIOP_FORMAT_UNIT              0x04
#define SCSIOP_READ_BLOCK_LIMITS        0x05
#define SCSIOP_REASSIGN_BLOCKS          0x07
#define SCSIOP_INIT_ELEMENT_STATUS      0x07
#define SCSIOP_READ6                    0x08
#define SCSIOP_RECEIVE                  0x08
#define SCSIOP_WRITE6                   0x0A
#define SCSIOP_PRINT                    0x0A
#define SCSIOP_SEND                     0x0A
#define SCSIOP_SEEK6                    0x0B
#define SCSIOP_TRACK_SELECT             0x0B
#define SCSIOP_SLEW_PRINT               0x0B
#define SCSIOP_SET_CAPACITY             0x0B // tape
#define SCSIOP_SEEK_BLOCK               0x0C
#define SCSIOP_PARTITION                0x0D
#define SCSIOP_READ_REVERSE             0x0F
#define SCSIOP_WRITE_FILEMARKS          0x10
#define SCSIOP_FLUSH_BUFFER             0x10
#define SCSIOP_SPACE                    0x11
#define SCSIOP_INQUIRY                  0x12
#define SCSIOP_VERIFY6                  0x13
#define SCSIOP_RECOVER_BUF_DATA         0x14
#define SCSIOP_MODE_SELECT              0x15
#define SCSIOP_RESERVE_UNIT             0x16
#define SCSIOP_RELEASE_UNIT             0x17
#define SCSIOP_COPY                     0x18
#define SCSIOP_ERASE                    0x19
#define SCSIOP_MODE_SENSE               0x1A
#define SCSIOP_START_STOP_UNIT          0x1B
#define SCSIOP_STOP_PRINT               0x1B
#define SCSIOP_LOAD_UNLOAD              0x1B
#define SCSIOP_RECEIVE_DIAGNOSTIC       0x1C
#define SCSIOP_SEND_DIAGNOSTIC          0x1D
#define SCSIOP_MEDIUM_REMOVAL           0x1E

// 10-byte commands
#define SCSIOP_READ_FORMATTED_CAPACITY  0x23
#define SCSIOP_READ_CAPACITY            0x25
#define SCSIOP_READ                     0x28
#define SCSIOP_WRITE                    0x2A
#define SCSIOP_SEEK                     0x2B
#define SCSIOP_LOCATE                   0x2B
#define SCSIOP_POSITION_TO_ELEMENT      0x2B
#define SCSIOP_WRITE_VERIFY             0x2E
#define SCSIOP_VERIFY                   0x2F
#define SCSIOP_SEARCH_DATA_HIGH         0x30
#define SCSIOP_SEARCH_DATA_EQUAL        0x31
#define SCSIOP_SEARCH_DATA_LOW          0x32
#define SCSIOP_SET_LIMITS               0x33
#define SCSIOP_READ_POSITION            0x34
#define SCSIOP_SYNCHRONIZE_CACHE        0x35
#define SCSIOP_COMPARE                  0x39
#define SCSIOP_COPY_COMPARE             0x3A
#define SCSIOP_WRITE_DATA_BUFF          0x3B
#define SCSIOP_READ_DATA_BUFF           0x3C
#define SCSIOP_WRITE_LONG               0x3F
#define SCSIOP_CHANGE_DEFINITION        0x40
#define SCSIOP_WRITE_SAME               0x41
#define SCSIOP_READ_SUB_CHANNEL         0x42
#define SCSIOP_READ_TOC                 0x43
#define SCSIOP_READ_HEADER              0x44
#define SCSIOP_REPORT_DENSITY_SUPPORT   0x44 // tape
#define SCSIOP_PLAY_AUDIO               0x45
#define SCSIOP_GET_CONFIGURATION        0x46
#define SCSIOP_PLAY_AUDIO_MSF           0x47
#define SCSIOP_PLAY_TRACK_INDEX         0x48
#define SCSIOP_PLAY_TRACK_RELATIVE      0x49
#define SCSIOP_GET_EVENT_STATUS         0x4A
#define SCSIOP_PAUSE_RESUME             0x4B
#define SCSIOP_LOG_SELECT               0x4C
#define SCSIOP_LOG_SENSE                0x4D
#define SCSIOP_STOP_PLAY_SCAN           0x4E
#define SCSIOP_XDWRITE                  0x50
#define SCSIOP_XPWRITE                  0x51
#define SCSIOP_READ_DISK_INFORMATION    0x51
#define SCSIOP_READ_DISC_INFORMATION    0x51 // proper use of disc over disk
#define SCSIOP_READ_TRACK_INFORMATION   0x52
#define SCSIOP_XDWRITE_READ             0x53
#define SCSIOP_RESERVE_TRACK_RZONE      0x53
#define SCSIOP_SEND_OPC_INFORMATION     0x54 // optimum power calibration
#define SCSIOP_MODE_SELECT10            0x55
#define SCSIOP_RESERVE_UNIT10           0x56
#define SCSIOP_RESERVE_ELEMENT          0x56
#define SCSIOP_RELEASE_UNIT10           0x57
#define SCSIOP_RELEASE_ELEMENT          0x57
#define SCSIOP_REPAIR_TRACK             0x58
#define SCSIOP_MODE_SENSE10             0x5A
#define SCSIOP_CLOSE_TRACK_SESSION      0x5B
#define SCSIOP_READ_BUFFER_CAPACITY     0x5C
#define SCSIOP_SEND_CUE_SHEET           0x5D
#define SCSIOP_PERSISTENT_RESERVE_IN    0x5E
#define SCSIOP_PERSISTENT_RESERVE_OUT   0x5F

// 12-byte commands
#define SCSIOP_REPORT_LUNS              0xA0
#define SCSIOP_BLANK                    0xA1
#define SCSIOP_ATA_PASSTHROUGH12        0xA1
#define SCSIOP_SEND_EVENT               0xA2
#define SCSIOP_SEND_KEY                 0xA3
#define SCSIOP_MAINTENANCE_IN           0xA3
#define SCSIOP_REPORT_KEY               0xA4
#define SCSIOP_MAINTENANCE_OUT          0xA4
#define SCSIOP_MOVE_MEDIUM              0xA5
#define SCSIOP_LOAD_UNLOAD_SLOT         0xA6
#define SCSIOP_EXCHANGE_MEDIUM          0xA6
#define SCSIOP_SET_READ_AHEAD           0xA7
#define SCSIOP_MOVE_MEDIUM_ATTACHED     0xA7
#define SCSIOP_READ12                   0xA8
#define SCSIOP_GET_MESSAGE              0xA8
#define SCSIOP_SERVICE_ACTION_OUT12     0xA9
#define SCSIOP_WRITE12                  0xAA
#define SCSIOP_SEND_MESSAGE             0xAB
#define SCSIOP_SERVICE_ACTION_IN12      0xAB
#define SCSIOP_GET_PERFORMANCE          0xAC
#define SCSIOP_READ_DVD_STRUCTURE       0xAD
#define SCSIOP_WRITE_VERIFY12           0xAE
#define SCSIOP_VERIFY12                 0xAF
#define SCSIOP_SEARCH_DATA_HIGH12       0xB0
#define SCSIOP_SEARCH_DATA_EQUAL12      0xB1
#define SCSIOP_SEARCH_DATA_LOW12        0xB2
#define SCSIOP_SET_LIMITS12             0xB3
#define SCSIOP_READ_ELEMENT_STATUS_ATTACHED 0xB4
#define SCSIOP_REQUEST_VOL_ELEMENT      0xB5
#define SCSIOP_SEND_VOLUME_TAG          0xB6
#define SCSIOP_SET_STREAMING            0xB6 // C/DVD
#define SCSIOP_READ_DEFECT_DATA         0xB7
#define SCSIOP_READ_ELEMENT_STATUS      0xB8
#define SCSIOP_READ_CD_MSF              0xB9
#define SCSIOP_SCAN_CD                  0xBA
#define SCSIOP_REDUNDANCY_GROUP_IN      0xBA
#define SCSIOP_SET_CD_SPEED             0xBB
#define SCSIOP_REDUNDANCY_GROUP_OUT     0xBB
#define SCSIOP_PLAY_CD                  0xBC
#define SCSIOP_SPARE_IN                 0xBC
#define SCSIOP_MECHANISM_STATUS         0xBD
#define SCSIOP_SPARE_OUT                0xBD
#define SCSIOP_READ_CD                  0xBE
#define SCSIOP_VOLUME_SET_IN            0xBE
#define SCSIOP_SEND_DVD_STRUCTURE       0xBF
#define SCSIOP_VOLUME_SET_OUT           0xBF
#define SCSIOP_INIT_ELEMENT_RANGE       0xE7

// 16-byte commands
#define SCSIOP_XDWRITE_EXTENDED16       0x80 // disk
#define SCSIOP_WRITE_FILEMARKS16        0x80 // tape
#define SCSIOP_REBUILD16                0x81 // disk
#define SCSIOP_READ_REVERSE16           0x81 // tape
#define SCSIOP_REGENERATE16             0x82 // disk
#define SCSIOP_EXTENDED_COPY            0x83
#define SCSIOP_RECEIVE_COPY_RESULTS     0x84
#define SCSIOP_ATA_PASSTHROUGH16        0x85
#define SCSIOP_ACCESS_CONTROL_IN        0x86
#define SCSIOP_ACCESS_CONTROL_OUT       0x87
#define SCSIOP_READ16                   0x88
#define SCSIOP_WRITE16                  0x8A
#define SCSIOP_READ_ATTRIBUTES          0x8C
#define SCSIOP_WRITE_ATTRIBUTES         0x8D
#define SCSIOP_WRITE_VERIFY16           0x8E
#define SCSIOP_VERIFY16                 0x8F
#define SCSIOP_PREFETCH16               0x90
#define SCSIOP_SYNCHRONIZE_CACHE16      0x91
#define SCSIOP_SPACE16                  0x91 // tape
#define SCSIOP_LOCK_UNLOCK_CACHE16      0x92
#define SCSIOP_LOCATE16                 0x92 // tape
#define SCSIOP_WRITE_SAME16             0x93
#define SCSIOP_ERASE16                  0x93 // tape
#define SCSIOP_READ_CAPACITY16          0x9E
#define SCSIOP_SERVICE_ACTION_IN16      0x9E
#define SCSIOP_SERVICE_ACTION_OUT16     0x9F


//
// If the IMMED bit is 1, status is returned as soon
// as the operation is initiated. If the IMMED bit
// is 0, status is not returned until the operation
// is completed.
//

#define CDB_RETURN_ON_COMPLETION   0
#define CDB_RETURN_IMMEDIATE       1

// end_ntminitape

//
// CDB Force media access used in extended read and write commands.
//

#define CDB_FORCE_MEDIA_ACCESS 0x08

//
// Denon CD ROM operation codes
//

#define SCSIOP_DENON_EJECT_DISC    0xE6
#define SCSIOP_DENON_STOP_AUDIO    0xE7
#define SCSIOP_DENON_PLAY_AUDIO    0xE8
#define SCSIOP_DENON_READ_TOC      0xE9
#define SCSIOP_DENON_READ_SUBCODE  0xEB

//
// SCSI Bus Messages
//

#define SCSIMESS_ABORT                0x06
#define SCSIMESS_ABORT_WITH_TAG       0x0D
#define SCSIMESS_BUS_DEVICE_RESET     0X0C
#define SCSIMESS_CLEAR_QUEUE          0X0E
#define SCSIMESS_COMMAND_COMPLETE     0X00
#define SCSIMESS_DISCONNECT           0X04
#define SCSIMESS_EXTENDED_MESSAGE     0X01
#define SCSIMESS_IDENTIFY             0X80
#define SCSIMESS_IDENTIFY_WITH_DISCON 0XC0
#define SCSIMESS_IGNORE_WIDE_RESIDUE  0X23
#define SCSIMESS_INITIATE_RECOVERY    0X0F
#define SCSIMESS_INIT_DETECTED_ERROR  0X05
#define SCSIMESS_LINK_CMD_COMP        0X0A
#define SCSIMESS_LINK_CMD_COMP_W_FLAG 0X0B
#define SCSIMESS_MESS_PARITY_ERROR    0X09
#define SCSIMESS_MESSAGE_REJECT       0X07
#define SCSIMESS_NO_OPERATION         0X08
#define SCSIMESS_HEAD_OF_QUEUE_TAG    0X21
#define SCSIMESS_ORDERED_QUEUE_TAG    0X22
#define SCSIMESS_SIMPLE_QUEUE_TAG     0X20
#define SCSIMESS_RELEASE_RECOVERY     0X10
#define SCSIMESS_RESTORE_POINTERS     0X03
#define SCSIMESS_SAVE_DATA_POINTER    0X02
#define SCSIMESS_TERMINATE_IO_PROCESS 0X11

//
// SCSI Extended Message operation codes
//

#define SCSIMESS_MODIFY_DATA_POINTER  0X00
#define SCSIMESS_SYNCHRONOUS_DATA_REQ 0X01
#define SCSIMESS_WIDE_DATA_REQUEST    0X03

//
// SCSI Extended Message Lengths
//

#define SCSIMESS_MODIFY_DATA_LENGTH   5
#define SCSIMESS_SYNCH_DATA_LENGTH    3
#define SCSIMESS_WIDE_DATA_LENGTH     2

//
// SCSI extended message structure
//

#pragma pack(push, scsi_mess, 1)
typedef struct _SCSI_EXTENDED_MESSAGE {
    UCHAR InitialMessageCode;
    UCHAR MessageLength;
    UCHAR MessageType;
    union _EXTENDED_ARGUMENTS {

        struct {
            UCHAR Modifier[4];
        } Modify;

        struct {
            UCHAR TransferPeriod;
            UCHAR ReqAckOffset;
        } Synchronous;

        struct{
            UCHAR Width;
        } Wide;
    }ExtendedArguments;
}SCSI_EXTENDED_MESSAGE, *PSCSI_EXTENDED_MESSAGE;
#pragma pack(pop, scsi_mess)

//
// SCSI bus status codes.
//

#define SCSISTAT_GOOD                  0x00
#define SCSISTAT_CHECK_CONDITION       0x02
#define SCSISTAT_CONDITION_MET         0x04
#define SCSISTAT_BUSY                  0x08
#define SCSISTAT_INTERMEDIATE          0x10
#define SCSISTAT_INTERMEDIATE_COND_MET 0x14
#define SCSISTAT_RESERVATION_CONFLICT  0x18
#define SCSISTAT_COMMAND_TERMINATED    0x22
#define SCSISTAT_QUEUE_FULL            0x28

//
// Enable Vital Product Data Flag (EVPD)
// used with INQUIRY command.
//

#define CDB_INQUIRY_EVPD           0x01

//
// Defines for format CDB
//

#define LUN0_FORMAT_SAVING_DEFECT_LIST 0
#define USE_DEFAULTMSB  0
#define USE_DEFAULTLSB  0

#define START_UNIT_CODE 0x01
#define STOP_UNIT_CODE  0x00

// begin_ntminitape

//
// Inquiry buffer structure. This is the data returned from the target
// after it receives an inquiry.
//
// This structure may be extended by the number of bytes specified
// in the field AdditionalLength. The defined size constant only
// includes fields through ProductRevisionLevel.
//
// The NT SCSI drivers are only interested in the first 36 bytes of data.
//

#define INQUIRYDATABUFFERSIZE 36

#if (NTDDI_VERSION < NTDDI_WINXP)
typedef struct _INQUIRYDATA {
    UCHAR DeviceType : 5;
    UCHAR DeviceTypeQualifier : 3;
    UCHAR DeviceTypeModifier : 7;
    UCHAR RemovableMedia : 1;
    UCHAR Versions;
    UCHAR ResponseDataFormat : 4;
    UCHAR HiSupport : 1;
    UCHAR NormACA : 1;
    UCHAR ReservedBit : 1;
    UCHAR AERC : 1;
    UCHAR AdditionalLength;
    UCHAR Reserved[2];
    UCHAR SoftReset : 1;
    UCHAR CommandQueue : 1;
    UCHAR Reserved2 : 1;
    UCHAR LinkedCommands : 1;
    UCHAR Synchronous : 1;
    UCHAR Wide16Bit : 1;
    UCHAR Wide32Bit : 1;
    UCHAR RelativeAddressing : 1;
    UCHAR VendorId[8];
    UCHAR ProductId[16];
    UCHAR ProductRevisionLevel[4];
    UCHAR VendorSpecific[20];
    UCHAR Reserved3[40];
} INQUIRYDATA, *PINQUIRYDATA;
#else
#pragma pack(push, inquiry, 1)
typedef struct _INQUIRYDATA {
    UCHAR DeviceType : 5;
    UCHAR DeviceTypeQualifier : 3;
    UCHAR DeviceTypeModifier : 7;
    UCHAR RemovableMedia : 1;
    union {
        UCHAR Versions;
        struct {
            UCHAR ANSIVersion : 3;
            UCHAR ECMAVersion : 3;
            UCHAR ISOVersion : 2;
        };
    };
    UCHAR ResponseDataFormat : 4;
    UCHAR HiSupport : 1;
    UCHAR NormACA : 1;
    UCHAR TerminateTask : 1;
    UCHAR AERC : 1;
    UCHAR AdditionalLength;
    UCHAR Reserved;
    UCHAR Addr16 : 1;               // defined only for SIP devices.
    UCHAR Addr32 : 1;               // defined only for SIP devices.
    UCHAR AckReqQ: 1;               // defined only for SIP devices.
    UCHAR MediumChanger : 1;
    UCHAR MultiPort : 1;
    UCHAR ReservedBit2 : 1;
    UCHAR EnclosureServices : 1;
    UCHAR ReservedBit3 : 1;
    UCHAR SoftReset : 1;
    UCHAR CommandQueue : 1;
    UCHAR TransferDisable : 1;      // defined only for SIP devices.
    UCHAR LinkedCommands : 1;
    UCHAR Synchronous : 1;          // defined only for SIP devices.
    UCHAR Wide16Bit : 1;            // defined only for SIP devices.
    UCHAR Wide32Bit : 1;            // defined only for SIP devices.
    UCHAR RelativeAddressing : 1;
    UCHAR VendorId[8];
    UCHAR ProductId[16];
    UCHAR ProductRevisionLevel[4];
    UCHAR VendorSpecific[20];
    UCHAR Reserved3[40];
} INQUIRYDATA, *PINQUIRYDATA;
#pragma pack(pop, inquiry)
#endif

//
// Inquiry defines. Used to interpret data returned from target as result
// of inquiry command.
//
// DeviceType field
//

#define DIRECT_ACCESS_DEVICE            0x00    // disks
#define SEQUENTIAL_ACCESS_DEVICE        0x01    // tapes
#define PRINTER_DEVICE                  0x02    // printers
#define PROCESSOR_DEVICE                0x03    // scanners, printers, etc
#define WRITE_ONCE_READ_MULTIPLE_DEVICE 0x04    // worms
#define READ_ONLY_DIRECT_ACCESS_DEVICE  0x05    // cdroms
#define SCANNER_DEVICE                  0x06    // scanners
#define OPTICAL_DEVICE                  0x07    // optical disks
#define MEDIUM_CHANGER                  0x08    // jukebox
#define COMMUNICATION_DEVICE            0x09    // network
// 0xA and 0xB are obsolete
#define ARRAY_CONTROLLER_DEVICE         0x0C
#define SCSI_ENCLOSURE_DEVICE           0x0D
#define REDUCED_BLOCK_DEVICE            0x0E    // e.g., 1394 disk
#define OPTICAL_CARD_READER_WRITER_DEVICE 0x0F
#define BRIDGE_CONTROLLER_DEVICE        0x10
#define OBJECT_BASED_STORAGE_DEVICE     0x11    // OSD
#define LOGICAL_UNIT_NOT_PRESENT_DEVICE 0x7F

#define DEVICE_QUALIFIER_ACTIVE         0x00
#define DEVICE_QUALIFIER_NOT_ACTIVE     0x01
#define DEVICE_QUALIFIER_NOT_SUPPORTED  0x03

//
// DeviceTypeQualifier field
//

#define DEVICE_CONNECTED 0x00

//
// Vital Product Data Pages
//

//
// Unit Serial Number Page (page code 0x80)
//
// Provides a product serial number for the target or the logical unit.
//
#pragma pack(push, vpd_media_sn, 1)
typedef struct _VPD_MEDIA_SERIAL_NUMBER_PAGE {
    UCHAR DeviceType : 5;
    UCHAR DeviceTypeQualifier : 3;
    UCHAR PageCode;
    UCHAR Reserved;
    UCHAR PageLength;
#if !defined(__midl)
    UCHAR SerialNumber[0];
#endif
} VPD_MEDIA_SERIAL_NUMBER_PAGE, *PVPD_MEDIA_SERIAL_NUMBER_PAGE;
#pragma pack(pop, vpd_media_sn)

#pragma pack(push, vpd_sn, 1)
typedef struct _VPD_SERIAL_NUMBER_PAGE {
    UCHAR DeviceType : 5;
    UCHAR DeviceTypeQualifier : 3;
    UCHAR PageCode;
    UCHAR Reserved;
    UCHAR PageLength;
#if !defined(__midl)
    UCHAR SerialNumber[0];
#endif
} VPD_SERIAL_NUMBER_PAGE, *PVPD_SERIAL_NUMBER_PAGE;
#pragma pack(pop, vpd_sn)

//
// Device Identification Page (page code 0x83)
// Provides the means to retrieve zero or more identification descriptors
// applying to the logical unit.
//

#pragma pack(push, vpd_stuff, 1)
typedef enum _VPD_CODE_SET {
    VpdCodeSetReserved = 0,
    VpdCodeSetBinary = 1,
    VpdCodeSetAscii = 2,
    VpdCodeSetUTF8 = 3
} VPD_CODE_SET, *PVPD_CODE_SET;

typedef enum _VPD_ASSOCIATION {
    VpdAssocDevice = 0,
    VpdAssocPort = 1,
    VpdAssocTarget = 2,
    VpdAssocReserved1 = 3,
    VpdAssocReserved2 = 4       // bogus, only two bits
} VPD_ASSOCIATION, *PVPD_ASSOCIATION;

typedef enum _VPD_IDENTIFIER_TYPE {
    VpdIdentifierTypeVendorSpecific = 0,
    VpdIdentifierTypeVendorId = 1,
    VpdIdentifierTypeEUI64 = 2,
    VpdIdentifierTypeFCPHName = 3,
    VpdIdentifierTypePortRelative = 4,
    VpdIdentifierTypeTargetPortGroup = 5,
    VpdIdentifierTypeLogicalUnitGroup = 6,
    VpdIdentifierTypeMD5LogicalUnitId = 7,
    VpdIdentifierTypeSCSINameString = 8
} VPD_IDENTIFIER_TYPE, *PVPD_IDENTIFIER_TYPE;

typedef struct _VPD_IDENTIFICATION_DESCRIPTOR {
    UCHAR CodeSet : 4;          // VPD_CODE_SET
    UCHAR Reserved : 4;
    UCHAR IdentifierType : 4;   // VPD_IDENTIFIER_TYPE
    UCHAR Association : 2;
    UCHAR Reserved2 : 2;
    UCHAR Reserved3;
    UCHAR IdentifierLength;
#if !defined(__midl)
    UCHAR Identifier[0];
#endif
} VPD_IDENTIFICATION_DESCRIPTOR, *PVPD_IDENTIFICATION_DESCRIPTOR;

typedef struct _VPD_IDENTIFICATION_PAGE {
    UCHAR DeviceType : 5;
    UCHAR DeviceTypeQualifier : 3;
    UCHAR PageCode;
    UCHAR Reserved;
    UCHAR PageLength;


    //
    // The following field is actually a variable length array of identification
    // descriptors.  Unfortunately there's no C notation for an array of
    // variable length structures so we're forced to just pretend.
    //

#if !defined(__midl)
    // VPD_IDENTIFICATION_DESCRIPTOR Descriptors[0];
    UCHAR Descriptors[0];
#endif
} VPD_IDENTIFICATION_PAGE, *PVPD_IDENTIFICATION_PAGE;

//
// Supported Vital Product Data Pages Page (page code 0x00)
// Contains a list of the vital product data page cods supported by the target
// or logical unit.
//

typedef struct _VPD_SUPPORTED_PAGES_PAGE {
    UCHAR DeviceType : 5;
    UCHAR DeviceTypeQualifier : 3;
    UCHAR PageCode;
    UCHAR Reserved;
    UCHAR PageLength;
#if !defined(__midl)
    UCHAR SupportedPageList[0];
#endif
} VPD_SUPPORTED_PAGES_PAGE, *PVPD_SUPPORTED_PAGES_PAGE;
#pragma pack(pop, vpd_stuff)


#define VPD_MAX_BUFFER_SIZE         0xff

#define VPD_SUPPORTED_PAGES         0x00
#define VPD_SERIAL_NUMBER           0x80
#define VPD_DEVICE_IDENTIFIERS      0x83
#define VPD_MEDIA_SERIAL_NUMBER     0x84
#define VPD_SOFTWARE_INTERFACE_IDENTIFIERS 0x84
#define VPD_NETWORK_MANAGEMENT_ADDRESSES 0x85
#define VPD_EXTENDED_INQUIRY_DATA   0x86
#define VPD_MODE_PAGE_POLICY        0x87
#define VPD_SCSI_PORTS              0x88


//
// Persistent Reservation Definitions.
//

//
// PERSISTENT_RESERVE_* definitions
//

#define RESERVATION_ACTION_READ_KEYS                    0x00
#define RESERVATION_ACTION_READ_RESERVATIONS            0x01

#define RESERVATION_ACTION_REGISTER                     0x00
#define RESERVATION_ACTION_RESERVE                      0x01
#define RESERVATION_ACTION_RELEASE                      0x02
#define RESERVATION_ACTION_CLEAR                        0x03
#define RESERVATION_ACTION_PREEMPT                      0x04
#define RESERVATION_ACTION_PREEMPT_ABORT                0x05
#define RESERVATION_ACTION_REGISTER_IGNORE_EXISTING     0x06

#define RESERVATION_SCOPE_LU                            0x00
#define RESERVATION_SCOPE_ELEMENT                       0x02

#define RESERVATION_TYPE_WRITE_EXCLUSIVE                0x01
#define RESERVATION_TYPE_EXCLUSIVE                      0x03
#define RESERVATION_TYPE_WRITE_EXCLUSIVE_REGISTRANTS    0x05
#define RESERVATION_TYPE_EXCLUSIVE_REGISTRANTS          0x06

//
// Structures for reserve in command.
//

#pragma pack(push, reserve_in_stuff, 1)
typedef struct {
    UCHAR Generation[4];
    UCHAR AdditionalLength[4];
#if !defined(__midl)
    UCHAR ReservationKeyList[0][8];
#endif
} PRI_REGISTRATION_LIST, *PPRI_REGISTRATION_LIST;

typedef struct {
    UCHAR ReservationKey[8];
    UCHAR ScopeSpecificAddress[4];
    UCHAR Reserved;
    UCHAR Type : 4;
    UCHAR Scope : 4;
    UCHAR Obsolete[2];
} PRI_RESERVATION_DESCRIPTOR, *PPRI_RESERVATION_DESCRIPTOR;

typedef struct {
    UCHAR Generation[4];
    UCHAR AdditionalLength[4];
#if !defined(__midl)
    PRI_RESERVATION_DESCRIPTOR Reservations[0];
#endif
} PRI_RESERVATION_LIST, *PPRI_RESERVATION_LIST;
#pragma pack(pop, reserve_in_stuff)

//
// Structures for reserve out command.
//

#pragma pack(push, reserve_out_stuff, 1)
typedef struct {
    UCHAR ReservationKey[8];
    UCHAR ServiceActionReservationKey[8];
    UCHAR ScopeSpecificAddress[4];
    UCHAR ActivatePersistThroughPowerLoss : 1;
    UCHAR Reserved1 : 7;
    UCHAR Reserved2;
    UCHAR Obsolete[2];
} PRO_PARAMETER_LIST, *PPRO_PARAMETER_LIST;
#pragma pack(pop, reserve_out_stuff)


//
// Sense Data Format
//

#pragma pack(push, sensedata, 1)
typedef struct _SENSE_DATA {
    UCHAR ErrorCode:7;
    UCHAR Valid:1;
    UCHAR SegmentNumber;
    UCHAR SenseKey:4;
    UCHAR Reserved:1;
    UCHAR IncorrectLength:1;
    UCHAR EndOfMedia:1;
    UCHAR FileMark:1;
    UCHAR Information[4];
    UCHAR AdditionalSenseLength;
    UCHAR CommandSpecificInformation[4];
    UCHAR AdditionalSenseCode;
    UCHAR AdditionalSenseCodeQualifier;
    UCHAR FieldReplaceableUnitCode;
    UCHAR SenseKeySpecific[3];
} SENSE_DATA, *PSENSE_DATA;
#pragma pack(pop, sensedata)

//
// Default request sense buffer size
//

#define SENSE_BUFFER_SIZE 18

//
// Maximum request sense buffer size
//

#define MAX_SENSE_BUFFER_SIZE 255

//
// Maximum number of additional sense bytes.
//

#define MAX_ADDITIONAL_SENSE_BYTES (MAX_SENSE_BUFFER_SIZE - SENSE_BUFFER_SIZE)

//
// Sense codes
//

#define SCSI_SENSE_NO_SENSE         0x00
#define SCSI_SENSE_RECOVERED_ERROR  0x01
#define SCSI_SENSE_NOT_READY        0x02
#define SCSI_SENSE_MEDIUM_ERROR     0x03
#define SCSI_SENSE_HARDWARE_ERROR   0x04
#define SCSI_SENSE_ILLEGAL_REQUEST  0x05
#define SCSI_SENSE_UNIT_ATTENTION   0x06
#define SCSI_SENSE_DATA_PROTECT     0x07
#define SCSI_SENSE_BLANK_CHECK      0x08
#define SCSI_SENSE_UNIQUE           0x09
#define SCSI_SENSE_COPY_ABORTED     0x0A
#define SCSI_SENSE_ABORTED_COMMAND  0x0B
#define SCSI_SENSE_EQUAL            0x0C
#define SCSI_SENSE_VOL_OVERFLOW     0x0D
#define SCSI_SENSE_MISCOMPARE       0x0E
#define SCSI_SENSE_RESERVED         0x0F

//
// Additional tape bit
//

#define SCSI_ILLEGAL_LENGTH         0x20
#define SCSI_EOM                    0x40
#define SCSI_FILE_MARK              0x80

//
// Additional Sense codes
//

#define SCSI_ADSENSE_NO_SENSE                              0x00
#define SCSI_ADSENSE_NO_SEEK_COMPLETE                      0x02
#define SCSI_ADSENSE_LUN_NOT_READY                         0x04
#define SCSI_ADSENSE_LUN_COMMUNICATION                     0x08
#define SCSI_ADSENSE_WRITE_ERROR                           0x0C
#define SCSI_ADSENSE_TRACK_ERROR                           0x14
#define SCSI_ADSENSE_SEEK_ERROR                            0x15
#define SCSI_ADSENSE_REC_DATA_NOECC                        0x17
#define SCSI_ADSENSE_REC_DATA_ECC                          0x18
#define SCSI_ADSENSE_PARAMETER_LIST_LENGTH                 0x1A
#define SCSI_ADSENSE_ILLEGAL_COMMAND                       0x20
#define SCSI_ADSENSE_ILLEGAL_BLOCK                         0x21
#define SCSI_ADSENSE_INVALID_CDB                           0x24
#define SCSI_ADSENSE_INVALID_LUN                           0x25
#define SCSI_ADSENSE_INVALID_FIELD_PARAMETER_LIST          0x26
#define SCSI_ADSENSE_WRITE_PROTECT                         0x27
#define SCSI_ADSENSE_MEDIUM_CHANGED                        0x28
#define SCSI_ADSENSE_BUS_RESET                             0x29
#define SCSI_ADSENSE_PARAMETERS_CHANGED                    0x2A
#define SCSI_ADSENSE_INSUFFICIENT_TIME_FOR_OPERATION       0x2E
#define SCSI_ADSENSE_INVALID_MEDIA                         0x30
#define SCSI_ADSENSE_NO_MEDIA_IN_DEVICE                    0x3a
#define SCSI_ADSENSE_POSITION_ERROR                        0x3b
#define SCSI_ADSENSE_OPERATING_CONDITIONS_CHANGED          0x3f
#define SCSI_ADSENSE_OPERATOR_REQUEST                      0x5a // see below
#define SCSI_ADSENSE_FAILURE_PREDICTION_THRESHOLD_EXCEEDED 0x5d
#define SCSI_ADSENSE_ILLEGAL_MODE_FOR_THIS_TRACK           0x64
#define SCSI_ADSENSE_COPY_PROTECTION_FAILURE               0x6f
#define SCSI_ADSENSE_POWER_CALIBRATION_ERROR               0x73
#define SCSI_ADSENSE_VENDOR_UNIQUE                         0x80 // and higher
#define SCSI_ADSENSE_MUSIC_AREA                            0xA0
#define SCSI_ADSENSE_DATA_AREA                             0xA1
#define SCSI_ADSENSE_VOLUME_OVERFLOW                       0xA7

// for legacy apps:
#define SCSI_ADWRITE_PROTECT                        SCSI_ADSENSE_WRITE_PROTECT
#define SCSI_FAILURE_PREDICTION_THRESHOLD_EXCEEDED  SCSI_ADSENSE_FAILURE_PREDICTION_THRESHOLD_EXCEEDED


//
// SCSI_ADSENSE_LUN_NOT_READY (0x04) qualifiers
//

#define SCSI_SENSEQ_CAUSE_NOT_REPORTABLE         0x00
#define SCSI_SENSEQ_BECOMING_READY               0x01
#define SCSI_SENSEQ_INIT_COMMAND_REQUIRED        0x02
#define SCSI_SENSEQ_MANUAL_INTERVENTION_REQUIRED 0x03
#define SCSI_SENSEQ_FORMAT_IN_PROGRESS           0x04
#define SCSI_SENSEQ_REBUILD_IN_PROGRESS          0x05
#define SCSI_SENSEQ_RECALCULATION_IN_PROGRESS    0x06
#define SCSI_SENSEQ_OPERATION_IN_PROGRESS        0x07
#define SCSI_SENSEQ_LONG_WRITE_IN_PROGRESS       0x08

//
// SCSI_ADSENSE_LUN_COMMUNICATION (0x08) qualifiers
//

#define SCSI_SENSEQ_COMM_FAILURE                 0x00
#define SCSI_SENSEQ_COMM_TIMEOUT                 0x01
#define SCSI_SENSEQ_COMM_PARITY_ERROR            0x02
#define SCSI_SESNEQ_COMM_CRC_ERROR               0x03
#define SCSI_SENSEQ_UNREACHABLE_TARGET           0x04

//
// SCSI_ADSENSE_WRITE_ERROR (0x0C) qualifiers
//
#define SCSI_SENSEQ_LOSS_OF_STREAMING            0x09
#define SCSI_SENSEQ_PADDING_BLOCKS_ADDED         0x0A


//
// SCSI_ADSENSE_NO_SENSE (0x00) qualifiers
//

#define SCSI_SENSEQ_FILEMARK_DETECTED 0x01
#define SCSI_SENSEQ_END_OF_MEDIA_DETECTED 0x02
#define SCSI_SENSEQ_SETMARK_DETECTED 0x03
#define SCSI_SENSEQ_BEGINNING_OF_MEDIA_DETECTED 0x04

//
// SCSI_ADSENSE_ILLEGAL_BLOCK (0x21) qualifiers
//

#define SCSI_SENSEQ_ILLEGAL_ELEMENT_ADDR 0x01

//
// SCSI_ADSENSE_POSITION_ERROR (0x3b) qualifiers
//

#define SCSI_SENSEQ_DESTINATION_FULL 0x0d
#define SCSI_SENSEQ_SOURCE_EMPTY     0x0e

//
// SCSI_ADSENSE_INVALID_MEDIA (0x30) qualifiers
//

#define SCSI_SENSEQ_INCOMPATIBLE_MEDIA_INSTALLED 0x00
#define SCSI_SENSEQ_UNKNOWN_FORMAT 0x01
#define SCSI_SENSEQ_INCOMPATIBLE_FORMAT 0x02
#define SCSI_SENSEQ_CLEANING_CARTRIDGE_INSTALLED 0x03


//
// SCSI_ADSENSE_OPERATING_CONDITIONS_CHANGED (0x3f) qualifiers
//

#define SCSI_SENSEQ_TARGET_OPERATING_CONDITIONS_CHANGED 0x00
#define SCSI_SENSEQ_MICROCODE_CHANGED                   0x01
#define SCSI_SENSEQ_OPERATING_DEFINITION_CHANGED        0x02
#define SCSI_SENSEQ_INQUIRY_DATA_CHANGED                0x03
#define SCSI_SENSEQ_COMPONENT_DEVICE_ATTACHED           0x04
#define SCSI_SENSEQ_DEVICE_IDENTIFIER_CHANGED           0x05
#define SCSI_SENSEQ_REDUNDANCY_GROUP_MODIFIED           0x06
#define SCSI_SENSEQ_REDUNDANCY_GROUP_DELETED            0x07
#define SCSI_SENSEQ_SPARE_MODIFIED                      0x08
#define SCSI_SENSEQ_SPARE_DELETED                       0x09
#define SCSI_SENSEQ_VOLUME_SET_MODIFIED                 0x0A
#define SCSI_SENSEQ_VOLUME_SET_DELETED                  0x0B
#define SCSI_SENSEQ_VOLUME_SET_DEASSIGNED               0x0C
#define SCSI_SENSEQ_VOLUME_SET_REASSIGNED               0x0D
#define SCSI_SENSEQ_REPORTED_LUNS_DATA_CHANGED          0x0E
#define SCSI_SENSEQ_ECHO_BUFFER_OVERWRITTEN             0x0F
#define SCSI_SENSEQ_MEDIUM_LOADABLE                     0x10
#define SCSI_SENSEQ_MEDIUM_AUXILIARY_MEMORY_ACCESSIBLE  0x11


//
// SCSI_ADSENSE_OPERATOR_REQUEST (0x5a) qualifiers
//

#define SCSI_SENSEQ_STATE_CHANGE_INPUT     0x00 // generic request
#define SCSI_SENSEQ_MEDIUM_REMOVAL         0x01
#define SCSI_SENSEQ_WRITE_PROTECT_ENABLE   0x02
#define SCSI_SENSEQ_WRITE_PROTECT_DISABLE  0x03

//
// SCSI_ADSENSE_COPY_PROTECTION_FAILURE (0x6f) qualifiers
//
#define SCSI_SENSEQ_AUTHENTICATION_FAILURE                          0x00
#define SCSI_SENSEQ_KEY_NOT_PRESENT                                 0x01
#define SCSI_SENSEQ_KEY_NOT_ESTABLISHED                             0x02
#define SCSI_SENSEQ_READ_OF_SCRAMBLED_SECTOR_WITHOUT_AUTHENTICATION 0x03
#define SCSI_SENSEQ_MEDIA_CODE_MISMATCHED_TO_LOGICAL_UNIT           0x04
#define SCSI_SENSEQ_LOGICAL_UNIT_RESET_COUNT_ERROR                  0x05

//
// SCSI_ADSENSE_POWER_CALIBRATION_ERROR (0x73) qualifiers
//

#define SCSI_SENSEQ_POWER_CALIBRATION_AREA_ALMOST_FULL 0x01
#define SCSI_SENSEQ_POWER_CALIBRATION_AREA_FULL        0x02
#define SCSI_SENSEQ_POWER_CALIBRATION_AREA_ERROR       0x03
#define SCSI_SENSEQ_PMA_RMA_UPDATE_FAILURE             0x04
#define SCSI_SENSEQ_PMA_RMA_IS_FULL                    0x05
#define SCSI_SENSEQ_PMA_RMA_ALMOST_FULL                0x06


// end_ntminitape

//
// SCSI IO Device Control Codes
//

#define FILE_DEVICE_SCSI 0x0000001b

#define IOCTL_SCSI_EXECUTE_IN   ((FILE_DEVICE_SCSI << 16) + 0x0011)
#define IOCTL_SCSI_EXECUTE_OUT  ((FILE_DEVICE_SCSI << 16) + 0x0012)
#define IOCTL_SCSI_EXECUTE_NONE ((FILE_DEVICE_SCSI << 16) + 0x0013)

//
// SMART support in atapi
//

#define IOCTL_SCSI_MINIPORT_SMART_VERSION           ((FILE_DEVICE_SCSI << 16) + 0x0500)
#define IOCTL_SCSI_MINIPORT_IDENTIFY                ((FILE_DEVICE_SCSI << 16) + 0x0501)
#define IOCTL_SCSI_MINIPORT_READ_SMART_ATTRIBS      ((FILE_DEVICE_SCSI << 16) + 0x0502)
#define IOCTL_SCSI_MINIPORT_READ_SMART_THRESHOLDS   ((FILE_DEVICE_SCSI << 16) + 0x0503)
#define IOCTL_SCSI_MINIPORT_ENABLE_SMART            ((FILE_DEVICE_SCSI << 16) + 0x0504)
#define IOCTL_SCSI_MINIPORT_DISABLE_SMART           ((FILE_DEVICE_SCSI << 16) + 0x0505)
#define IOCTL_SCSI_MINIPORT_RETURN_STATUS           ((FILE_DEVICE_SCSI << 16) + 0x0506)
#define IOCTL_SCSI_MINIPORT_ENABLE_DISABLE_AUTOSAVE ((FILE_DEVICE_SCSI << 16) + 0x0507)
#define IOCTL_SCSI_MINIPORT_SAVE_ATTRIBUTE_VALUES   ((FILE_DEVICE_SCSI << 16) + 0x0508)
#define IOCTL_SCSI_MINIPORT_EXECUTE_OFFLINE_DIAGS   ((FILE_DEVICE_SCSI << 16) + 0x0509)
#define IOCTL_SCSI_MINIPORT_ENABLE_DISABLE_AUTO_OFFLINE ((FILE_DEVICE_SCSI << 16) + 0x050a)
#define IOCTL_SCSI_MINIPORT_READ_SMART_LOG          ((FILE_DEVICE_SCSI << 16) + 0x050b)
#define IOCTL_SCSI_MINIPORT_WRITE_SMART_LOG         ((FILE_DEVICE_SCSI << 16) + 0x050c)

//
// CLUSTER support
// deliberately skipped some values to allow for expansion above.
//
#define IOCTL_SCSI_MINIPORT_NOT_QUORUM_CAPABLE     ((FILE_DEVICE_SCSI << 16) + 0x0520)
#define IOCTL_SCSI_MINIPORT_NOT_CLUSTER_CAPABLE    ((FILE_DEVICE_SCSI << 16) + 0x0521)


// begin_ntminitape

//
// Read Capacity Data - returned in Big Endian format
//

#pragma pack(push, read_capacity, 1)
typedef struct _READ_CAPACITY_DATA {
    ULONG LogicalBlockAddress;
    ULONG BytesPerBlock;
} READ_CAPACITY_DATA, *PREAD_CAPACITY_DATA;
#pragma pack(pop, read_capacity)


#pragma pack(push, read_capacity_ex, 1)
typedef struct _READ_CAPACITY_DATA_EX {
    LARGE_INTEGER LogicalBlockAddress;
    ULONG BytesPerBlock;
} READ_CAPACITY_DATA_EX, *PREAD_CAPACITY_DATA_EX;
#pragma pack(pop, read_capacity_ex)


//
// Read Block Limits Data - returned in Big Endian format
// This structure returns the maximum and minimum block
// size for a TAPE device.
//

#pragma pack(push, read_block_limits, 1)
typedef struct _READ_BLOCK_LIMITS {
    UCHAR Reserved;
    UCHAR BlockMaximumSize[3];
    UCHAR BlockMinimumSize[2];
} READ_BLOCK_LIMITS_DATA, *PREAD_BLOCK_LIMITS_DATA;
#pragma pack(pop, read_block_limits)

#pragma pack(push, read_buffer_capacity, 1)
typedef struct _READ_BUFFER_CAPACITY_DATA {
    UCHAR DataLength[2];
    UCHAR Reserved1;
    UCHAR BlockDataReturned : 1;
    UCHAR Reserved4         : 7;
    UCHAR TotalBufferSize[4];
    UCHAR AvailableBufferSize[4];
} READ_BUFFER_CAPACITY_DATA, *PREAD_BUFFER_CAPACITY_DATA;
#pragma pack(pop, read_buffer_capacity)

//
// Mode data structures.
//

//
// Define Mode parameter header.
//

#pragma pack(push, mode_params, 1)
typedef struct _MODE_PARAMETER_HEADER {
    UCHAR ModeDataLength;
    UCHAR MediumType;
    UCHAR DeviceSpecificParameter;
    UCHAR BlockDescriptorLength;
}MODE_PARAMETER_HEADER, *PMODE_PARAMETER_HEADER;

typedef struct _MODE_PARAMETER_HEADER10 {
    UCHAR ModeDataLength[2];
    UCHAR MediumType;
    UCHAR DeviceSpecificParameter;
    UCHAR Reserved[2];
    UCHAR BlockDescriptorLength[2];
}MODE_PARAMETER_HEADER10, *PMODE_PARAMETER_HEADER10;
#pragma pack(pop, mode_params)

#define MODE_FD_SINGLE_SIDE     0x01
#define MODE_FD_DOUBLE_SIDE     0x02
#define MODE_FD_MAXIMUM_TYPE    0x1E
#define MODE_DSP_FUA_SUPPORTED  0x10
#define MODE_DSP_WRITE_PROTECT  0x80

//
// Define the mode parameter block.
//

#pragma pack(push, mode_params_block, 1)
typedef struct _MODE_PARAMETER_BLOCK {
    UCHAR DensityCode;
    UCHAR NumberOfBlocks[3];
    UCHAR Reserved;
    UCHAR BlockLength[3];
}MODE_PARAMETER_BLOCK, *PMODE_PARAMETER_BLOCK;
#pragma pack(pop, mode_params_block)

//
// Define Disconnect-Reconnect page.
//


#pragma pack(push, mode_page_disconnect, 1)
typedef struct _MODE_DISCONNECT_PAGE {
    UCHAR PageCode : 6;
    UCHAR Reserved : 1;
    UCHAR PageSavable : 1;
    UCHAR PageLength;
    UCHAR BufferFullRatio;
    UCHAR BufferEmptyRatio;
    UCHAR BusInactivityLimit[2];
    UCHAR BusDisconnectTime[2];
    UCHAR BusConnectTime[2];
    UCHAR MaximumBurstSize[2];
    UCHAR DataTransferDisconnect : 2;
    UCHAR Reserved2[3];
}MODE_DISCONNECT_PAGE, *PMODE_DISCONNECT_PAGE;
#pragma pack(pop, mode_page_disconnect)

//
// Define mode caching page.
//

#pragma pack(push, mode_page_caching, 1)
typedef struct _MODE_CACHING_PAGE {
    UCHAR PageCode : 6;
    UCHAR Reserved : 1;
    UCHAR PageSavable : 1;
    UCHAR PageLength;
    UCHAR ReadDisableCache : 1;
    UCHAR MultiplicationFactor : 1;
    UCHAR WriteCacheEnable : 1;
    UCHAR Reserved2 : 5;
    UCHAR WriteRetensionPriority : 4;
    UCHAR ReadRetensionPriority : 4;
    UCHAR DisablePrefetchTransfer[2];
    UCHAR MinimumPrefetch[2];
    UCHAR MaximumPrefetch[2];
    UCHAR MaximumPrefetchCeiling[2];
}MODE_CACHING_PAGE, *PMODE_CACHING_PAGE;
#pragma pack(pop, mode_page_caching)

//
// Define write parameters cdrom page
//
#pragma pack(push, mode_page_wp2, 1)
typedef struct _MODE_CDROM_WRITE_PARAMETERS_PAGE2 {
    UCHAR PageCode : 6;             // 0x05
    UCHAR Reserved : 1;
    UCHAR PageSavable : 1;
    UCHAR PageLength;               // 0x32 ??
    UCHAR WriteType                 : 4;
    UCHAR TestWrite                 : 1;
    UCHAR LinkSizeValid             : 1;
    UCHAR BufferUnderrunFreeEnabled : 1;
    UCHAR Reserved2                 : 1;
    UCHAR TrackMode                 : 4;
    UCHAR Copy                      : 1;
    UCHAR FixedPacket               : 1;
    UCHAR MultiSession              : 2;
    UCHAR DataBlockType             : 4;
    UCHAR Reserved3                 : 4;
    UCHAR LinkSize;
    UCHAR Reserved4;
    UCHAR HostApplicationCode       : 6;
    UCHAR Reserved5                 : 2;
    UCHAR SessionFormat;
    UCHAR Reserved6;
    UCHAR PacketSize[4];
    UCHAR AudioPauseLength[2];
    UCHAR MediaCatalogNumber[16];
    UCHAR ISRC[16];
    UCHAR SubHeaderData[4];
} MODE_CDROM_WRITE_PARAMETERS_PAGE2, *PMODE_CDROM_WRITE_PARAMETERS_PAGE2;
#pragma pack(pop, mode_page_wp2)

#ifndef DEPRECATE_DDK_FUNCTIONS
// this structure is being retired due to missing fields and overly
// complex data definitions for the MCN and ISRC.
#pragma pack(push, mode_page_wp, 1)
typedef struct _MODE_CDROM_WRITE_PARAMETERS_PAGE {
    UCHAR PageLength;               // 0x32 ??
    UCHAR WriteType                 : 4;
    UCHAR TestWrite                 : 1;
    UCHAR LinkSizeValid             : 1;
    UCHAR BufferUnderrunFreeEnabled : 1;
    UCHAR Reserved2                 : 1;
    UCHAR TrackMode                 : 4;
    UCHAR Copy                      : 1;
    UCHAR FixedPacket               : 1;
    UCHAR MultiSession              : 2;
    UCHAR DataBlockType             : 4;
    UCHAR Reserved3                 : 4;
    UCHAR LinkSize;
    UCHAR Reserved4;
    UCHAR HostApplicationCode       : 6;
    UCHAR Reserved5                 : 2;
    UCHAR SessionFormat;
    UCHAR Reserved6;
    UCHAR PacketSize[4];
    UCHAR AudioPauseLength[2];
    UCHAR Reserved7                 : 7;
    UCHAR MediaCatalogNumberValid   : 1;
    UCHAR MediaCatalogNumber[13];
    UCHAR MediaCatalogNumberZero;
    UCHAR MediaCatalogNumberAFrame;
    UCHAR Reserved8                 : 7;
    UCHAR ISRCValid                 : 1;
    UCHAR ISRCCountry[2];
    UCHAR ISRCOwner[3];
    UCHAR ISRCRecordingYear[2];
    UCHAR ISRCSerialNumber[5];
    UCHAR ISRCZero;
    UCHAR ISRCAFrame;
    UCHAR ISRCReserved;
    UCHAR SubHeaderData[4];
} MODE_CDROM_WRITE_PARAMETERS_PAGE, *PMODE_CDROM_WRITE_PARAMETERS_PAGE;
#pragma pack(pop, mode_page_wp)
#endif //ifndef DEPRECATE_DDK_FUNCTIONS

//
// Define the MRW mode page for CDROM device types
//
#pragma pack(push, mode_page_mrw, 1)
typedef struct _MODE_MRW_PAGE {
    UCHAR PageCode : 6; // 0x03
    UCHAR Reserved : 1;
    UCHAR PageSavable : 1;
    UCHAR PageLength;   //0x06
    UCHAR Reserved1;
    UCHAR LbaSpace  : 1;
    UCHAR Reserved2 : 7;
    UCHAR Reserved3[4];
} MODE_MRW_PAGE, *PMODE_MRW_PAGE;
#pragma pack(pop, mode_page_mrw)

//
// Define mode flexible disk page.
//

#pragma pack(push, mode_page_flex, 1)
typedef struct _MODE_FLEXIBLE_DISK_PAGE {
    UCHAR PageCode : 6;
    UCHAR Reserved : 1;
    UCHAR PageSavable : 1;
    UCHAR PageLength;
    UCHAR TransferRate[2];
    UCHAR NumberOfHeads;
    UCHAR SectorsPerTrack;
    UCHAR BytesPerSector[2];
    UCHAR NumberOfCylinders[2];
    UCHAR StartWritePrecom[2];
    UCHAR StartReducedCurrent[2];
    UCHAR StepRate[2];
    UCHAR StepPluseWidth;
    UCHAR HeadSettleDelay[2];
    UCHAR MotorOnDelay;
    UCHAR MotorOffDelay;
    UCHAR Reserved2 : 5;
    UCHAR MotorOnAsserted : 1;
    UCHAR StartSectorNumber : 1;
    UCHAR TrueReadySignal : 1;
    UCHAR StepPlusePerCyclynder : 4;
    UCHAR Reserved3 : 4;
    UCHAR WriteCompenstation;
    UCHAR HeadLoadDelay;
    UCHAR HeadUnloadDelay;
    UCHAR Pin2Usage : 4;
    UCHAR Pin34Usage : 4;
    UCHAR Pin1Usage : 4;
    UCHAR Pin4Usage : 4;
    UCHAR MediumRotationRate[2];
    UCHAR Reserved4[2];
} MODE_FLEXIBLE_DISK_PAGE, *PMODE_FLEXIBLE_DISK_PAGE;
#pragma pack(pop, mode_page_flex)

//
// Define mode format page.
//

#pragma pack(push, mode_page_format, 1)
typedef struct _MODE_FORMAT_PAGE {
    UCHAR PageCode : 6;
    UCHAR Reserved : 1;
    UCHAR PageSavable : 1;
    UCHAR PageLength;
    UCHAR TracksPerZone[2];
    UCHAR AlternateSectorsPerZone[2];
    UCHAR AlternateTracksPerZone[2];
    UCHAR AlternateTracksPerLogicalUnit[2];
    UCHAR SectorsPerTrack[2];
    UCHAR BytesPerPhysicalSector[2];
    UCHAR Interleave[2];
    UCHAR TrackSkewFactor[2];
    UCHAR CylinderSkewFactor[2];
    UCHAR Reserved2 : 4;
    UCHAR SurfaceFirst : 1;
    UCHAR RemovableMedia : 1;
    UCHAR HardSectorFormating : 1;
    UCHAR SoftSectorFormating : 1;
    UCHAR Reserved3[3];
} MODE_FORMAT_PAGE, *PMODE_FORMAT_PAGE;
#pragma pack(pop, mode_page_format)

//
// Define rigid disk driver geometry page.
//

#pragma pack(push, mode_page_geometry, 1)
typedef struct _MODE_RIGID_GEOMETRY_PAGE {
    UCHAR PageCode : 6;
    UCHAR Reserved : 1;
    UCHAR PageSavable : 1;
    UCHAR PageLength;
    UCHAR NumberOfCylinders[3];
    UCHAR NumberOfHeads;
    UCHAR StartWritePrecom[3];
    UCHAR StartReducedCurrent[3];
    UCHAR DriveStepRate[2];
    UCHAR LandZoneCyclinder[3];
    UCHAR RotationalPositionLock : 2;
    UCHAR Reserved2 : 6;
    UCHAR RotationOffset;
    UCHAR Reserved3;
    UCHAR RoataionRate[2];
    UCHAR Reserved4[2];
}MODE_RIGID_GEOMETRY_PAGE, *PMODE_RIGID_GEOMETRY_PAGE;
#pragma pack(pop, mode_page_geometry)

//
// Define read write recovery page
//

#pragma pack(push, mode_page_rw_recovery, 1)
typedef struct _MODE_READ_WRITE_RECOVERY_PAGE {

    UCHAR PageCode : 6;
    UCHAR Reserved1 : 1;
    UCHAR PSBit : 1;
    UCHAR PageLength;
    UCHAR DCRBit : 1;
    UCHAR DTEBit : 1;
    UCHAR PERBit : 1;
    UCHAR EERBit : 1;
    UCHAR RCBit : 1;
    UCHAR TBBit : 1;
    UCHAR ARRE : 1;
    UCHAR AWRE : 1;
    UCHAR ReadRetryCount;
    UCHAR Reserved4[4];
    UCHAR WriteRetryCount;
    UCHAR Reserved5[3];

} MODE_READ_WRITE_RECOVERY_PAGE, *PMODE_READ_WRITE_RECOVERY_PAGE;
#pragma pack(pop, mode_page_rw_recovery)

//
// Define read recovery page - cdrom
//

#pragma pack(push, mode_page_r_recovery, 1)
typedef struct _MODE_READ_RECOVERY_PAGE {

    UCHAR PageCode : 6;
    UCHAR Reserved1 : 1;
    UCHAR PSBit : 1;
    UCHAR PageLength;
    UCHAR DCRBit : 1;
    UCHAR DTEBit : 1;
    UCHAR PERBit : 1;
    UCHAR Reserved2 : 1;
    UCHAR RCBit : 1;
    UCHAR TBBit : 1;
    UCHAR Reserved3 : 2;
    UCHAR ReadRetryCount;
    UCHAR Reserved4[4];

} MODE_READ_RECOVERY_PAGE, *PMODE_READ_RECOVERY_PAGE;
#pragma pack(pop, mode_page_r_recovery)


//
// Define Informational Exception Control Page. Used for failure prediction
//

#pragma pack(push, mode_page_xcpt, 1)
typedef struct _MODE_INFO_EXCEPTIONS
{
    UCHAR PageCode : 6;
    UCHAR Reserved1 : 1;
    UCHAR PSBit : 1;

    UCHAR PageLength;

    union
    {
        UCHAR Flags;
        struct
        {
            UCHAR LogErr : 1;
            UCHAR Reserved2 : 1;
            UCHAR Test : 1;
            UCHAR Dexcpt : 1;
            UCHAR Reserved3 : 3;
            UCHAR Perf : 1;
        };
    };

    UCHAR ReportMethod : 4;
    UCHAR Reserved4 : 4;

    UCHAR IntervalTimer[4];
    UCHAR ReportCount[4];

} MODE_INFO_EXCEPTIONS, *PMODE_INFO_EXCEPTIONS;
#pragma pack(pop, mode_page_xcpt)

//
// Begin C/DVD 0.9 definitions
//

//
// Power Condition Mode Page Format
//

#pragma pack(push, mode_page_power, 1)
typedef struct _POWER_CONDITION_PAGE {
    UCHAR PageCode : 6;         // 0x1A
    UCHAR Reserved : 1;
    UCHAR PSBit : 1;
    UCHAR PageLength;           // 0x0A
    UCHAR Reserved2;

    UCHAR Standby : 1;
    UCHAR Idle : 1;
    UCHAR Reserved3 : 6;

    UCHAR IdleTimer[4];
    UCHAR StandbyTimer[4];
} POWER_CONDITION_PAGE, *PPOWER_CONDITION_PAGE;
#pragma pack(pop, mode_page_power)

//
// CD-Audio Control Mode Page Format
//

#pragma pack(push, mode_page_cdaudio, 1)
typedef struct _CDDA_OUTPUT_PORT {
    UCHAR ChannelSelection : 4;
    UCHAR Reserved : 4;
    UCHAR Volume;
} CDDA_OUTPUT_PORT, *PCDDA_OUTPUT_PORT;

typedef struct _CDAUDIO_CONTROL_PAGE {
    UCHAR PageCode : 6;     // 0x0E
    UCHAR Reserved : 1;
    UCHAR PSBit : 1;

    UCHAR PageLength;       // 0x0E

    UCHAR Reserved2 : 1;
    UCHAR StopOnTrackCrossing : 1;         // Default 0
    UCHAR Immediate : 1;    // Always 1
    UCHAR Reserved3 : 5;

    UCHAR Reserved4[3];
    UCHAR Obsolete[2];

    CDDA_OUTPUT_PORT CDDAOutputPorts[4];

} CDAUDIO_CONTROL_PAGE, *PCDAUDIO_CONTROL_PAGE;
#pragma pack(pop, mode_page_cdaudio)

#define CDDA_CHANNEL_MUTED      0x0
#define CDDA_CHANNEL_ZERO       0x1
#define CDDA_CHANNEL_ONE        0x2
#define CDDA_CHANNEL_TWO        0x4
#define CDDA_CHANNEL_THREE      0x8

//
// C/DVD Feature Set Support & Version Page
//

#pragma pack(push, mode_page_features, 1)
typedef struct _CDVD_FEATURE_SET_PAGE {
    UCHAR PageCode : 6;     // 0x18
    UCHAR Reserved : 1;
    UCHAR PSBit : 1;

    UCHAR PageLength;       // 0x16

    UCHAR CDAudio[2];
    UCHAR EmbeddedChanger[2];
    UCHAR PacketSMART[2];
    UCHAR PersistantPrevent[2];
    UCHAR EventStatusNotification[2];
    UCHAR DigitalOutput[2];
    UCHAR CDSequentialRecordable[2];
    UCHAR DVDSequentialRecordable[2];
    UCHAR RandomRecordable[2];
    UCHAR KeyExchange[2];
    UCHAR Reserved2[2];
} CDVD_FEATURE_SET_PAGE, *PCDVD_FEATURE_SET_PAGE;
#pragma pack(pop, mode_page_features)

//
// CDVD Inactivity Time-out Page Format
//

#pragma pack(push, mode_page_timeout, 1)
typedef struct _CDVD_INACTIVITY_TIMEOUT_PAGE {
    UCHAR PageCode : 6;     // 0x1D
    UCHAR Reserved : 1;
    UCHAR PSBit : 1;

    UCHAR PageLength;       // 0x08
    UCHAR Reserved2[2];

    UCHAR SWPP : 1;
    UCHAR DISP : 1;
    UCHAR Reserved3 : 6;

    UCHAR Reserved4;
    UCHAR GroupOneMinimumTimeout[2];
    UCHAR GroupTwoMinimumTimeout[2];
} CDVD_INACTIVITY_TIMEOUT_PAGE, *PCDVD_INACTIVITY_TIMEOUT_PAGE;
#pragma pack(pop, mode_page_timeout)

//
// CDVD Capabilities & Mechanism Status Page
//

#define CDVD_LMT_CADDY              0
#define CDVD_LMT_TRAY               1
#define CDVD_LMT_POPUP              2
#define CDVD_LMT_RESERVED1          3
#define CDVD_LMT_CHANGER_INDIVIDUAL 4
#define CDVD_LMT_CHANGER_CARTRIDGE  5
#define CDVD_LMT_RESERVED2          6
#define CDVD_LMT_RESERVED3          7


#pragma pack(push, mode_page_capabilities, 1)
typedef struct _CDVD_CAPABILITIES_PAGE {
    UCHAR PageCode : 6;     // 0x2A
    UCHAR Reserved : 1;
    UCHAR PSBit : 1;                        // offset 0

    UCHAR PageLength;       // >= 0x18      // offset 1

    UCHAR CDRRead : 1;
    UCHAR CDERead : 1;
    UCHAR Method2 : 1;
    UCHAR DVDROMRead : 1;
    UCHAR DVDRRead : 1;
    UCHAR DVDRAMRead : 1;
    UCHAR Reserved2 : 2;                    // offset 2

    UCHAR CDRWrite : 1;
    UCHAR CDEWrite : 1;
    UCHAR TestWrite : 1;
    UCHAR Reserved3 : 1;
    UCHAR DVDRWrite : 1;
    UCHAR DVDRAMWrite : 1;
    UCHAR Reserved4 : 2;                    // offset 3

    UCHAR AudioPlay : 1;
    UCHAR Composite : 1;
    UCHAR DigitalPortOne : 1;
    UCHAR DigitalPortTwo : 1;
    UCHAR Mode2Form1 : 1;
    UCHAR Mode2Form2 : 1;
    UCHAR MultiSession : 1;
    UCHAR BufferUnderrunFree : 1;                    // offset 4

    UCHAR CDDA : 1;
    UCHAR CDDAAccurate : 1;
    UCHAR RWSupported : 1;
    UCHAR RWDeinterleaved : 1;
    UCHAR C2Pointers : 1;
    UCHAR ISRC : 1;
    UCHAR UPC : 1;
    UCHAR ReadBarCodeCapable : 1;           // offset 5

    UCHAR Lock : 1;
    UCHAR LockState : 1;
    UCHAR PreventJumper : 1;
    UCHAR Eject : 1;
    UCHAR Reserved6 : 1;
    UCHAR LoadingMechanismType : 3;         // offset 6

    UCHAR SeparateVolume : 1;
    UCHAR SeperateChannelMute : 1;
    UCHAR SupportsDiskPresent : 1;
    UCHAR SWSlotSelection : 1;
    UCHAR SideChangeCapable : 1;
    UCHAR RWInLeadInReadable : 1;
    UCHAR Reserved7 : 2;                    // offset 7

    union {
        UCHAR ReadSpeedMaximum[2];
        UCHAR ObsoleteReserved[2];          // offset 8
    };

    UCHAR NumberVolumeLevels[2];            // offset 10
    UCHAR BufferSize[2];                    // offset 12

    union {
        UCHAR ReadSpeedCurrent[2];
        UCHAR ObsoleteReserved2[2];         // offset 14
    };
    UCHAR ObsoleteReserved3;                // offset 16

    UCHAR Reserved8 : 1;
    UCHAR BCK : 1;
    UCHAR RCK : 1;
    UCHAR LSBF : 1;
    UCHAR Length : 2;
    UCHAR Reserved9 : 2;                    // offset 17

    union {
        UCHAR WriteSpeedMaximum[2];
        UCHAR ObsoleteReserved4[2];         // offset 18
    };
    union {
        UCHAR WriteSpeedCurrent[2];
        UCHAR ObsoleteReserved11[2];        // offset 20
    };

    //
    // NOTE: This mode page is two bytes too small in the release
    //       version of the Windows2000 DDK.  it also incorrectly
    //       put the CopyManagementRevision at offset 20 instead
    //       of offset 22, so fix that with a nameless union (for
    //       backwards-compatibility with those who "fixed" it on
    //       their own by looking at Reserved10[]).
    //

    union {
        UCHAR CopyManagementRevision[2];    // offset 22
        UCHAR Reserved10[2];
    };
    //UCHAR Reserved12[2];                    // offset 24

} CDVD_CAPABILITIES_PAGE, *PCDVD_CAPABILITIES_PAGE;
#pragma pack(pop, mode_page_capabilities)

#pragma pack(push, lun_list, 1)
typedef struct _LUN_LIST {
    UCHAR LunListLength[4]; // sizeof LunSize * 8
    UCHAR Reserved[4];
#if !defined(__midl)
    UCHAR Lun[0][8];        // 4 level of addressing.  2 bytes each.
#endif
} LUN_LIST, *PLUN_LIST;
#pragma pack(pop, lun_list)


#define LOADING_MECHANISM_CADDY                 0x00
#define LOADING_MECHANISM_TRAY                  0x01
#define LOADING_MECHANISM_POPUP                 0x02
#define LOADING_MECHANISM_INDIVIDUAL_CHANGER    0x04
#define LOADING_MECHANISM_CARTRIDGE_CHANGER     0x05

//
// end C/DVD 0.9 mode page definitions

//
// Mode parameter list block descriptor -
// set the block length for reading/writing
//
//

#define MODE_BLOCK_DESC_LENGTH               8
#define MODE_HEADER_LENGTH                   4
#define MODE_HEADER_LENGTH10                 8

#pragma pack(push, mode_parm_rw, 1)
typedef struct _MODE_PARM_READ_WRITE {

   MODE_PARAMETER_HEADER  ParameterListHeader;  // List Header Format
   MODE_PARAMETER_BLOCK   ParameterListBlock;   // List Block Descriptor

} MODE_PARM_READ_WRITE_DATA, *PMODE_PARM_READ_WRITE_DATA;
#pragma pack(pop, mode_parm_rw)

// end_ntminitape

//
// CDROM audio control (0x0E)
//

#define CDB_AUDIO_PAUSE 0
#define CDB_AUDIO_RESUME 1

#define CDB_DEVICE_START 0x11
#define CDB_DEVICE_STOP 0x10

#define CDB_EJECT_MEDIA 0x10
#define CDB_LOAD_MEDIA 0x01

#define CDB_SUBCHANNEL_HEADER      0x00
#define CDB_SUBCHANNEL_BLOCK       0x01

#define CDROM_AUDIO_CONTROL_PAGE   0x0E
#define MODE_SELECT_IMMEDIATE      0x04
#define MODE_SELECT_PFBIT          0x10

#define CDB_USE_MSF                0x01

#pragma pack(push, audio_output, 1)
typedef struct _PORT_OUTPUT {
    UCHAR ChannelSelection;
    UCHAR Volume;
} PORT_OUTPUT, *PPORT_OUTPUT;

typedef struct _AUDIO_OUTPUT {
    UCHAR CodePage;
    UCHAR ParameterLength;
    UCHAR Immediate;
    UCHAR Reserved[2];
    UCHAR LbaFormat;
    UCHAR LogicalBlocksPerSecond[2];
    PORT_OUTPUT PortOutput[4];
} AUDIO_OUTPUT, *PAUDIO_OUTPUT;
#pragma pack(pop, audio_output)

//
// Multisession CDROM
//

#define GET_LAST_SESSION 0x01
#define GET_SESSION_DATA 0x02;

//
// Atapi 2.5 changer
//

#pragma pack(push, chgr_stuff, 1)
typedef struct _MECHANICAL_STATUS_INFORMATION_HEADER {
    UCHAR CurrentSlot : 5;
    UCHAR ChangerState : 2;
    UCHAR Fault : 1;
    UCHAR Reserved : 5;
    UCHAR MechanismState : 3;
    UCHAR CurrentLogicalBlockAddress[3];
    UCHAR NumberAvailableSlots;
    UCHAR SlotTableLength[2];
} MECHANICAL_STATUS_INFORMATION_HEADER, *PMECHANICAL_STATUS_INFORMATION_HEADER;

typedef struct _SLOT_TABLE_INFORMATION {
    UCHAR DiscChanged : 1;
    UCHAR Reserved : 6;
    UCHAR DiscPresent : 1;
    UCHAR Reserved2[3];
} SLOT_TABLE_INFORMATION, *PSLOT_TABLE_INFORMATION;

typedef struct _MECHANICAL_STATUS {
    MECHANICAL_STATUS_INFORMATION_HEADER MechanicalStatusHeader;
    SLOT_TABLE_INFORMATION SlotTableInfo[1];
} MECHANICAL_STATUS, *PMECHANICAL_STATUS;
#pragma pack(pop, chgr_stuff)


// begin_ntminitape

//
// Tape definitions
//

#pragma pack(push, tape_position, 1)
typedef struct _TAPE_POSITION_DATA {
    UCHAR Reserved1:2;
    UCHAR BlockPositionUnsupported:1;
    UCHAR Reserved2:3;
    UCHAR EndOfPartition:1;
    UCHAR BeginningOfPartition:1;
    UCHAR PartitionNumber;
    USHORT Reserved3;
    UCHAR FirstBlock[4];
    UCHAR LastBlock[4];
    UCHAR Reserved4;
    UCHAR NumberOfBlocks[3];
    UCHAR NumberOfBytes[4];
} TAPE_POSITION_DATA, *PTAPE_POSITION_DATA;
#pragma pack(pop, tape_position)

//
// This structure is used to convert little endian
// ULONGs to SCSI CDB big endians values.
//

#pragma pack(push, byte_stuff, 1)
typedef union _EIGHT_BYTE {

    struct {
        UCHAR Byte0;
        UCHAR Byte1;
        UCHAR Byte2;
        UCHAR Byte3;
        UCHAR Byte4;
        UCHAR Byte5;
        UCHAR Byte6;
        UCHAR Byte7;
    };

    ULONGLONG AsULongLong;
} EIGHT_BYTE, *PEIGHT_BYTE;

typedef union _FOUR_BYTE {

    struct {
        UCHAR Byte0;
        UCHAR Byte1;
        UCHAR Byte2;
        UCHAR Byte3;
    };

    ULONG AsULong;
} FOUR_BYTE, *PFOUR_BYTE;

typedef union _TWO_BYTE {

    struct {
        UCHAR Byte0;
        UCHAR Byte1;
    };

    USHORT AsUShort;
} TWO_BYTE, *PTWO_BYTE;
#pragma pack(pop, byte_stuff)

//
// Byte reversing macro for converting
// between big- and little-endian formats
//

#define REVERSE_BYTES_QUAD(Destination, Source) {           \
    PEIGHT_BYTE d = (PEIGHT_BYTE)(Destination);             \
    PEIGHT_BYTE s = (PEIGHT_BYTE)(Source);                  \
    d->Byte7 = s->Byte0;                                    \
    d->Byte6 = s->Byte1;                                    \
    d->Byte5 = s->Byte2;                                    \
    d->Byte4 = s->Byte3;                                    \
    d->Byte3 = s->Byte4;                                    \
    d->Byte2 = s->Byte5;                                    \
    d->Byte1 = s->Byte6;                                    \
    d->Byte0 = s->Byte7;                                    \
}

#define REVERSE_BYTES(Destination, Source) {                \
    PFOUR_BYTE d = (PFOUR_BYTE)(Destination);               \
    PFOUR_BYTE s = (PFOUR_BYTE)(Source);                    \
    d->Byte3 = s->Byte0;                                    \
    d->Byte2 = s->Byte1;                                    \
    d->Byte1 = s->Byte2;                                    \
    d->Byte0 = s->Byte3;                                    \
}

#define REVERSE_BYTES_SHORT(Destination, Source) {          \
    PTWO_BYTE d = (PTWO_BYTE)(Destination);                 \
    PTWO_BYTE s = (PTWO_BYTE)(Source);                      \
    d->Byte1 = s->Byte0;                                    \
    d->Byte0 = s->Byte1;                                    \
}

//
// Byte reversing macro for converting
// USHORTS from big to little endian in place
//

#define REVERSE_SHORT(Short) {          \
    UCHAR tmp;                          \
    PTWO_BYTE w = (PTWO_BYTE)(Short);   \
    tmp = w->Byte0;                     \
    w->Byte0 = w->Byte1;                \
    w->Byte1 = tmp;                     \
    }

//
// Byte reversing macro for convering
// ULONGS between big & little endian in place
//

#define REVERSE_LONG(Long) {            \
    UCHAR tmp;                          \
    PFOUR_BYTE l = (PFOUR_BYTE)(Long);  \
    tmp = l->Byte3;                     \
    l->Byte3 = l->Byte0;                \
    l->Byte0 = tmp;                     \
    tmp = l->Byte2;                     \
    l->Byte2 = l->Byte1;                \
    l->Byte1 = tmp;                     \
    }

//
// This macro has the effect of Bit = log2(Data)
//

#define WHICH_BIT(Data, Bit) {                      \
    UCHAR tmp;                                      \
    for (tmp = 0; tmp < 32; tmp++) {                \
        if (((Data) >> tmp) == 1) {                 \
            break;                                  \
        }                                           \
    }                                               \
    ASSERT(tmp != 32);                              \
    (Bit) = tmp;                                    \
}


//
// For backwards compatability, use SCSIPORT definitions.
//

typedef PHYSICAL_ADDRESS STOR_PHYSICAL_ADDRESS, *PSTOR_PHYSICAL_ADDRESS;

typedef struct _ACCESS_RANGE {
    STOR_PHYSICAL_ADDRESS RangeStart;
    ULONG RangeLength;
    BOOLEAN RangeInMemory;
} ACCESS_RANGE, *PACCESS_RANGE;

//
// _MEMORY_REGION represents a region of physical contiguous memory.
// Generally, this is used for DMA common buffer regions.
//

typedef struct _MEMORY_REGION {

    //
    // Beginning virtual address of the region.
    //

    PUCHAR VirtualBase;

    //
    // Beginning physical address of the region.
    //

    PHYSICAL_ADDRESS PhysicalBase;

    //
    // Length of the region
    //
    //

    ULONG Length;

} MEMORY_REGION, *PMEMORY_REGION;


typedef enum _STOR_SYNCHRONIZATION_MODEL {
    StorSynchronizeHalfDuplex,
    StorSynchronizeFullDuplex
} STOR_SYNCHRONIZATION_MODEL;

typedef enum _INTERRUPT_SYNCHRONIZATION_MODE {
    InterruptSupportNone,
    InterruptSynchronizeAll,
    InterruptSynchronizePerMessage
} INTERRUPT_SYNCHRONIZATION_MODE;

typedef
BOOLEAN
(*PHW_MESSAGE_SIGNALED_INTERRUPT_ROUTINE) (
    IN PVOID HwDeviceExtension,
    IN ULONG MessageId
    );

//
// Message signalled interrupts support.
//

typedef struct _MESSAGE_INTERRUPT_INFORMATION {
    ULONG MessageId;
    ULONG MessageData;
    STOR_PHYSICAL_ADDRESS MessageAddress;
    ULONG InterruptVector;
    ULONG InterruptLevel;
    KINTERRUPT_MODE InterruptMode;
} MESSAGE_INTERRUPT_INFORMATION, *PMESSAGE_INTERRUPT_INFORMATION;


BOOLEAN
StorPortGetMessageInterruptInformation(
    __in PVOID HwDeviceExtension,
    __in ULONG MessageId,
    __out PMESSAGE_INTERRUPT_INFORMATION InterruptInfo
    );
#define STOR_MAP_NO_BUFFERS             (0)
#define STOR_MAP_ALL_BUFFERS            (1)
#define STOR_MAP_NON_READ_WRITE_BUFFERS (2)

// ExtendedFlags1 flags

#define EXTENDED_FLAG_POWER 0x00000001
//
// Configuration information structure.  Contains the information necessary
// to initialize the adapter. NOTE: This structure must be a multiple of
// quadwords.
//

typedef struct _PORT_CONFIGURATION_INFORMATION {

    //
    // Length of port configuation information strucuture.
    //

    ULONG Length;

    //
    // IO bus number (0 for machines that have only 1 IO bus
    //

    ULONG SystemIoBusNumber;

    //
    // EISA, MCA or ISA
    //

    INTERFACE_TYPE  AdapterInterfaceType;

    //
    // Interrupt request level for device
    //

    ULONG BusInterruptLevel;

    //
    // Bus interrupt vector used with hardware buses which use as vector as
    // well as level, such as internal buses.
    //

    ULONG BusInterruptVector;

    //
    // Interrupt mode (level-sensitive or edge-triggered)
    //

    KINTERRUPT_MODE InterruptMode;

    //
    // Maximum number of bytes that can be transferred in a single SRB
    //

    ULONG MaximumTransferLength;

    //
    // Number of contiguous blocks of physical memory
    //

    ULONG NumberOfPhysicalBreaks;

    //
    // DMA channel for devices using system DMA
    //

    ULONG DmaChannel;
    ULONG DmaPort;
    DMA_WIDTH DmaWidth;
    DMA_SPEED DmaSpeed;

    //
    // Alignment masked required by the adapter for data transfers.
    //

    ULONG AlignmentMask;

    //
    // Number of access range elements which have been allocated.
    //

    ULONG NumberOfAccessRanges;

    //
    // Pointer to array of access range elements.
    //

    ACCESS_RANGE (*AccessRanges)[];

    //
    // Reserved field.
    //

    PVOID Reserved;

    //
    // Number of SCSI buses attached to the adapter.
    //

    UCHAR NumberOfBuses;

    //
    // SCSI bus ID for adapter
    //

    CCHAR InitiatorBusId[8];

    //
    // Indicates that the adapter does scatter/gather
    //

    BOOLEAN ScatterGather;

    //
    // Indicates that the adapter is a bus master
    //

    BOOLEAN Master;

    //
    // Host caches data or state.
    //

    BOOLEAN CachesData;

    //
    // Host adapter scans down for bios devices.
    //

    BOOLEAN AdapterScansDown;

    //
    // Primary at disk address (0x1F0) claimed.
    //

    BOOLEAN AtdiskPrimaryClaimed;

    //
    // Secondary at disk address (0x170) claimed.
    //

    BOOLEAN AtdiskSecondaryClaimed;

    //
    // The master uses 32-bit DMA addresses.
    //

    BOOLEAN Dma32BitAddresses;

    //
    // Use Demand Mode DMA rather than Single Request.
    //

    BOOLEAN DemandMode;

    //
    // Data buffers must be mapped into virtual address space.
    //

    UCHAR MapBuffers;

    //
    // The driver will need to tranlate virtual to physical addresses.
    //

    BOOLEAN NeedPhysicalAddresses;

    //
    // Supports tagged queuing
    //

    BOOLEAN TaggedQueuing;

    //
    // Supports auto request sense.
    //

    BOOLEAN AutoRequestSense;

    //
    // Supports multiple requests per logical unit.
    //

    BOOLEAN MultipleRequestPerLu;

    //
    // Support receive event function.
    //

    BOOLEAN ReceiveEvent;

    //
    // Indicates the real-mode driver has initialized the card.
    //

    BOOLEAN RealModeInitialized;

    //
    // Indicate that the miniport will not touch the data buffers directly.
    //

    BOOLEAN BufferAccessScsiPortControlled;

    //
    // Indicator for wide scsi.
    //

    UCHAR   MaximumNumberOfTargets;

    //
    // Ensure quadword alignment.
    //

    UCHAR   ReservedUchars[2];

    //
    // Adapter slot number
    //

    ULONG SlotNumber;

    //
    // Interrupt information for a second IRQ.
    //

    ULONG BusInterruptLevel2;
    ULONG BusInterruptVector2;
    KINTERRUPT_MODE InterruptMode2;

    //
    // DMA information for a second channel.
    //

    ULONG DmaChannel2;
    ULONG DmaPort2;
    DMA_WIDTH DmaWidth2;
    DMA_SPEED DmaSpeed2;

    //
    // Fields added to allow for the miniport
    // to update these sizes based on requirements
    // for large transfers ( > 64K);
    //

    ULONG DeviceExtensionSize;
    ULONG SpecificLuExtensionSize;
    ULONG SrbExtensionSize;

    //
    // Used to determine whether the system and/or the miniport support
    // 64-bit physical addresses.  See SCSI_DMA64_* flags below.
    //

    UCHAR  Dma64BitAddresses;        /* New */

    //
    // Indicates that the miniport can accept a SRB_FUNCTION_RESET_DEVICE
    // to clear all requests to a particular LUN.
    //

    BOOLEAN ResetTargetSupported;       /* New */

    //
    // Indicates that the miniport can support more than 8 logical units per
    // target (maximum LUN number is one less than this field).
    //

    UCHAR MaximumNumberOfLogicalUnits;  /* New */

    //
    // Supports WMI?
    //

    BOOLEAN WmiDataProvider;

    //
    // STORPORT synchronization model, either half or full duplex
    // depending on whether the driver supports async-with-interrupt
    // model or not.
    //

    STOR_SYNCHRONIZATION_MODEL SynchronizationModel;    // STORPORT New

    PHW_MESSAGE_SIGNALED_INTERRUPT_ROUTINE HwMSInterruptRoutine;

    INTERRUPT_SYNCHRONIZATION_MODE InterruptSynchronizationMode;

    MEMORY_REGION DumpRegion;

    ULONG         RequestedDumpBufferSize;

    BOOLEAN       VirtualDevice;

    ULONG         ExtendedFlags1;

    ULONG         MaxNumberOfIO;


} PORT_CONFIGURATION_INFORMATION, *PPORT_CONFIGURATION_INFORMATION;


//
// Scatter/gather lists
//

typedef struct _STOR_SCATTER_GATHER_ELEMENT {
    STOR_PHYSICAL_ADDRESS PhysicalAddress;
    ULONG Length;
    ULONG_PTR Reserved;
} STOR_SCATTER_GATHER_ELEMENT, *PSTOR_SCATTER_GATHER_ELEMENT;

typedef struct _STOR_SCATTER_GATHER_LIST {
    ULONG NumberOfElements;
    ULONG_PTR Reserved;
    STOR_SCATTER_GATHER_ELEMENT List[];
} STOR_SCATTER_GATHER_LIST, *PSTOR_SCATTER_GATHER_LIST;


typedef enum _GETSGSTATUS{
    SG_ALLOCATED = 0,
    SG_BUFFER_TOO_SMALL
} GETSGSTATUS, *PGETSGSTATUS;

//
// Version control for ConfigInfo structure.
//

#define CONFIG_INFO_VERSION_2 sizeof(PORT_CONFIGURATION_INFORMATION)


//
// Flags for controlling 64-bit DMA use (PORT_CONFIGURATION_INFORMATION field
// Dma64BitAddresses)
//

//
// Set by scsiport on entering HwFindAdapter if the system can support 64-bit
// physical addresses.  The miniport can use this information before calling
// ScsiPortGetUncachedExtension to modify the DeviceExtensionSize,
// SpecificLuExtensionSize & SrbExtensionSize fields to account for the extra
// size of the scatter gather list.
//

#define SCSI_DMA64_SYSTEM_SUPPORTED     0x80

//
// Set by the miniport before returning from HwFindAdapter to tell the port
// driver that we support 64-bit physical addresses on I/O transfers. The
// port driver will still allocate Uncached Extension, SenseInfo and Srb
// Extension below 4GB, but I/O transfers will not be remapped.
//


#define SCSI_DMA64_MINIPORT_SUPPORTED   0x01

#if (NTDDI_VERSION >= NTDDI_WS03SP1)
//
// Set by the miniport before returning from HwFindAdpater to tell the port
// driver that we support full 64-bit addressing. This means I/O requests
// may be handled with > 4GB physical addresses, and uncached extension,
// SenseInof and Srb Extension may all lie above 4GB.
//

#define SCSI_DMA64_MINIPORT_FULL64BIT_SUPPORTED 0x02
#endif

//
// Command type (and parameter) definition(s) for AdapterControl requests.
//

typedef enum _SCSI_ADAPTER_CONTROL_TYPE {
    ScsiQuerySupportedControlTypes = 0,
    ScsiStopAdapter,
    ScsiRestartAdapter,
    ScsiSetBootConfig,
    ScsiSetRunningConfig,
    ScsiAdapterControlMax,
    MakeAdapterControlTypeSizeOfUlong = 0xffffffff
} SCSI_ADAPTER_CONTROL_TYPE, *PSCSI_ADAPTER_CONTROL_TYPE;

//
// Adapter control status values
//

typedef enum _SCSI_ADAPTER_CONTROL_STATUS {
    ScsiAdapterControlSuccess = 0,
    ScsiAdapterControlUnsuccessful
} SCSI_ADAPTER_CONTROL_STATUS, *PSCSI_ADAPTER_CONTROL_STATUS;

//
// Parameters for Adapter Control Functions:
//

//
// ScsiQuerySupportedControlTypes:
//

typedef struct _SCSI_SUPPORTED_CONTROL_TYPE_LIST {

    //
    // Specifies the number of entries in the adapter control type list.
    //

    ULONG MaxControlType;

    //
    // The miniport will set TRUE for each control type it supports.
    // The number of entries in this array is defined by MaxAdapterControlType
    // - the miniport must not attempt to set any AC types beyond the maximum
    // value specified.
    //

    BOOLEAN SupportedTypeList[0];

} SCSI_SUPPORTED_CONTROL_TYPE_LIST, *PSCSI_SUPPORTED_CONTROL_TYPE_LIST;

//
// DPC Data Structure
//

typedef struct _DPC_BUFFER {
    CSHORT Type;
    UCHAR Number;
    UCHAR Importance;
    struct {
        PVOID F;
        PVOID B;
    };
    PVOID DeferredRoutine;
    PVOID DeferredContext;
    PVOID SystemArgument1;
    PVOID SystemArgument2;
    PVOID DpcData;
} DPC_BUFFER;

#define STOR_DPC_BUFFER_SIZE (sizeof (DPC_BUFFER))

typedef struct _STOR_DPC {
    DPC_BUFFER Dpc;
    ULONG_PTR Lock;
} STOR_DPC, *PSTOR_DPC;

typedef enum _STOR_SPINLOCK {
    DpcLock = 1,
    StartIoLock,
    InterruptLock
} STOR_SPINLOCK;

typedef struct _STOR_LOCK_HANDLE {
    STOR_SPINLOCK Lock;
    struct {
        struct {
            PVOID Next;
            PVOID Lock;
        } LockQueue;
        KIRQL OldIrql;
    } Context;
} STOR_LOCK_HANDLE, *PSTOR_LOCK_HANDLE;

#define STOR_PERF_DPC_REDIRECTION 0x00000001
#define STOR_PERF_CONCURRENT_CHANNELS 0x00000002
#define STOR_PERF_INTERRUPT_MESSAGE_RANGES 0x00000004
#define STOR_PERF_ADV_CONFIG_LOCALITY 0x00000008
#define STOR_PERF_OPTIMIZE_FOR_COMPLETION_DURING_STARTIO 0x00000010

#define STOR_PERF_VERSION 0x00000003

typedef struct _PERF_CONFIGURATION_DATA {
    ULONG Version;
    ULONG Size;
    ULONG Flags;
    ULONG ConcurrentChannels;
    ULONG FirstRedirectionMessageNumber, LastRedirectionMessageNumber;
    ULONG DeviceNode;
    ULONG Reserved;
    PGROUP_AFFINITY MessageTargets;
} PERF_CONFIGURATION_DATA, *PPERF_CONFIGURATION_DATA;

typedef struct _STARTIO_PERFORMANCE_PARAMETERS {
    ULONG Version;
    ULONG Size;
    ULONG MessageNumber;
    ULONG ChannelNumber;
} STARTIO_PERFORMANCE_PARAMETERS, *PSTARTIO_PERFORMANCE_PARAMETERS;

//
// SCSI Adapter Dependent Routines
//

typedef
BOOLEAN
(*PHW_INITIALIZE) (
    __in PVOID DeviceExtension
    );

typedef
BOOLEAN
(*PHW_BUILDIO) (
    __in PVOID DeviceExtension,
    __in PSCSI_REQUEST_BLOCK Srb
    );

typedef
BOOLEAN
(*PHW_STARTIO) (
    __in PVOID DeviceExtension,
    __in PSCSI_REQUEST_BLOCK Srb
    );

typedef
BOOLEAN
(*PHW_INTERRUPT) (
    __in PVOID DeviceExtension
    );

typedef
VOID
(*PHW_TIMER) (
    __in PVOID DeviceExtension
    );

typedef
VOID
(*PHW_DMA_STARTED) (
    __in PVOID DeviceExtension
    );

typedef
ULONG
(*PHW_FIND_ADAPTER) (
    __in PVOID DeviceExtension,
    __in PVOID HwContext,
    __in PVOID BusInformation,
    __in PCHAR ArgumentString,
    __inout PPORT_CONFIGURATION_INFORMATION ConfigInfo,
    __out PBOOLEAN Again
    );

typedef
BOOLEAN
(*PHW_RESET_BUS) (
    __in PVOID DeviceExtension,
    __in ULONG PathId
    );

typedef
BOOLEAN
(*PHW_ADAPTER_STATE) (
    __in PVOID DeviceExtension,
    __in PVOID Context,
    __in BOOLEAN SaveState
    );

typedef
SCSI_ADAPTER_CONTROL_STATUS
(*PHW_ADAPTER_CONTROL) (
    __in PVOID DeviceExtension,
    __in SCSI_ADAPTER_CONTROL_TYPE ControlType,
    __in PVOID Parameters
    );

typedef
BOOLEAN
(*PHW_PASSIVE_INITIALIZE_ROUTINE)(
    __in PVOID DeviceExtension
    );

typedef
VOID
(*PHW_DPC_ROUTINE)(
    __in PSTOR_DPC Dpc,
    __in PVOID HwDeviceExtension,
    __in PVOID SystemArgument1,
    __in PVOID SystemArgument2
    );

typedef
BOOLEAN
(*PStorPortGetMessageInterruptInformation)(
    __in PVOID HwDeviceExtension,
    __in ULONG MessageId,
    __out PMESSAGE_INTERRUPT_INFORMATION InterruptInfo
    );
typedef
VOID
(*PStorPortPutScatterGatherList) (
    __in PVOID HwDeviceExtension,
    __in PSTOR_SCATTER_GATHER_LIST ScatterGatherList,
    __in BOOLEAN  WriteToDevice
    );
typedef
VOID
  (*PpostScaterGatherExecute)(
    __in PVOID  *DeviceObject,
    __in PVOID  *Irp,
    __in PSTOR_SCATTER_GATHER_LIST  ScatterGather,
    __in PVOID  Context
    );

typedef
GETSGSTATUS
(*PStorPortBuildScatterGatherList) (
    __in PVOID  HwDeviceExtension,
    __in PVOID  Mdl,
    __in_bcount(Length) PVOID  CurrentVa,
    __in ULONG  Length,
    __in PpostScaterGatherExecute ExecutionRoutine,
    __in PVOID  Context,
    __in BOOLEAN  WriteToDevice,
    __inout_bcount(ScatterGatherBufferLength) PVOID  ScatterGatherBuffer,
    __in ULONG  ScatterGatherBufferLength
    );
typedef

VOID
(*PStorPortFreePool)(
    __in __drv_freesMem(Mem) PVOID PMemory,
    __in PVOID HwDeviceExtension,
    __in_opt __drv_freesMem(Mem) PVOID PMdl
    );
typedef

__drv_allocatesMem(Mem)
__bcount(NumberOfBytes)
PVOID
(*PStorPortAllocatePool)(
    __in ULONG NumberOfBytes,
    __in ULONG Tag,
    __in PVOID HwDeviceExtension,
    __out __deref __drv_when(return==0, __drv_valueIs(==0))
    __deref __drv_when(return!=0, __drv_aliasesMem __drv_allocatesMem(Mem) __drv_valueIs(!=0))
        PVOID *PMdl
    );

typedef
PVOID
(*PStorPortGetSystemAddress)(
    __in PSCSI_REQUEST_BLOCK Srb
    );

typedef
ULONG
(*PStorPortAcquireMSISpinLock)(
    __in PVOID HwDeviceExtension,
    __in ULONG MessageID
    );
typedef
VOID
(*PStorPortReleaseMSISpinLock)(
    __in PVOID HwDeviceExtension,
    __in ULONG MessageID,
    __in ULONG OldIrql
    );

typedef
VOID
(*PStorPortCompleteServiceIrp)(
    __in PVOID HwDeviceExtension,
    __in PVOID  Irp
    );
typedef
PVOID
(*PStorPortGetOriginalMdl)(
    __in PSCSI_REQUEST_BLOCK Srb
    );

typedef struct _STORPORT_EXTENDED_FUNCTIONS {

    ULONG Version;

    // Port extended services
    PStorPortGetMessageInterruptInformation GetMessageInterruptInformation;
    PStorPortPutScatterGatherList           PutScatterGatherList;
    PStorPortBuildScatterGatherList         BuildScatterGatherList;
    PStorPortFreePool                       FreePool;
    PStorPortAllocatePool                   AllocatePool;
    PStorPortGetSystemAddress               GetSystemAddress;
    PStorPortAcquireMSISpinLock             AcquireMSISpinLock;
    PStorPortReleaseMSISpinLock             ReleaseMSISpinLock;
    PStorPortCompleteServiceIrp             CompleteServiceIrp;
    PStorPortGetOriginalMdl                 GetOriginalMdl;
} STORPORT_EXTENDED_FUNCTIONS, *PSTORPORT_EXTENDED_FUNCTIONS;


typedef enum _STORPORT_FUNCTION_CODE {
    ExtFunctionAllocatePool,
    ExtFunctionFreePool,
    ExtFunctionAllocateMdl,
    ExtFunctionFreeMdl,
    ExtFunctionBuildMdlForNonPagedPool,
    ExtFunctionGetSystemAddress,
    ExtFunctionGetOriginalMdl,
    ExtFunctionCompleteServiceIrp,
    ExtFunctionGetDeviceObjects,
    ExtFunctionBuildScatterGatherList,
    ExtFunctionPutScatterGatherList,
    ExtFunctionAcquireMSISpinLock,
    ExtFunctionReleaseMSISpinLock,
    ExtFunctionGetMessageInterruptInformation,
    ExtFunctionInitializePerformanceOptimizations,
    ExtFunctionGetStartIoPerformanceParameters,
    ExtFunctionLogSystemEvent,
    ExtFunctionGetCurrentProcessorNumber,
    ExtFunctionGetActiveGroupCount,
    ExtFunctionGetGroupAffinity,
    ExtFunctionGetActiveNodeCount,
    ExtFunctionGetNodeAffinity,
    ExtFunctionGetHighestNodeNumber,
    ExtFunctionGetLogicalProcessorRelationship,
    ExtFunctionAllocateContiguousMemorySpecifyCacheNode,
    ExtFunctionFreeContiguousMemorySpecifyCache
} STORPORT_FUNCTION_CODE, *PSTORPORT_FUNCTION_CODE;


//
// Storage port driver status codes
// This is the storage equivalent of NTSTATUS
//

#define STOR_STATUS_SUCCESS                     (0x00000000L)
#define STOR_STATUS_UNSUCCESSFUL                (0xC1000001L)
#define STOR_STATUS_NOT_IMPLEMENTED             (0xC1000002L)
#define STOR_STATUS_INSUFFICIENT_RESOURCES      (0xC1000003L)
#define STOR_STATUS_BUFFER_TOO_SMALL            (0xC1000004L)
#define STOR_STATUS_ACCESS_DENIED               (0xC1000005L)
#define STOR_STATUS_INVALID_PARAMETER           (0xC1000006L)
#define STOR_STATUS_INVALID_DEVICE_REQUEST      (0xC1000007L)
#define STOR_STATUS_INVALID_IRQL                (0xC1000008L)
#define STOR_STATUS_INVALID_DEVICE_STATE        (0xC1000009L)
#define STOR_STATUS_INVALID_BUFFER_SIZE         (0xC100000AL)
#define STOR_STATUS_UNSUPPORTED_VERSION         (0xC100000BL)

//
// Port driver error codes
//

#define SP_BUS_PARITY_ERROR          0x0001
#define SP_UNEXPECTED_DISCONNECT     0x0002
#define SP_INVALID_RESELECTION       0x0003
#define SP_BUS_TIME_OUT              0x0004
#define SP_PROTOCOL_ERROR            0x0005
#define SP_INTERNAL_ADAPTER_ERROR    0x0006
#define SP_REQUEST_TIMEOUT           0x0007
#define SP_IRQ_NOT_RESPONDING        0x0008
#define SP_BAD_FW_WARNING            0x0009
#define SP_BAD_FW_ERROR              0x000a
#define SP_LOST_WMI_MINIPORT_REQUEST 0x000b

//
// Port driver version flags
//
#define SP_VER_TRACE_SUPPORT        0x0010

//
// Return values for SCSI_HW_FIND_ADAPTER.
//

#define SP_RETURN_NOT_FOUND     0
#define SP_RETURN_FOUND         1
#define SP_RETURN_ERROR         2
#define SP_RETURN_BAD_CONFIG    3

//
// Notification Event Types
//

typedef enum _SCSI_NOTIFICATION_TYPE {
    RequestComplete,
    NextRequest,
    NextLuRequest,
    ResetDetected,
    _obsolete1,             // STORPORT: CallDisableInterrupts has been removed
    _obsolete2,             // STORPORT: CallEnableInterrupts has been removed
    RequestTimerCall,
    BusChangeDetected,
    WMIEvent,
    WMIReregister,
    LinkUp,
    LinkDown,
    QueryTickCount,
    BufferOverrunDetected,
    TraceNotification,
    GetExtendedFunctionTable,

    EnablePassiveInitialization = 0x1000,
    InitializeDpc,
    IssueDpc,
    AcquireSpinLock,
    ReleaseSpinLock

} SCSI_NOTIFICATION_TYPE, *PSCSI_NOTIFICATION_TYPE;

//
// Structure passed between miniport initialization
// and SCSI port initialization
//

typedef struct _HW_INITIALIZATION_DATA {

    ULONG HwInitializationDataSize;

    //
    // Adapter interface type:
    //
    // Internal
    // Isa
    // Eisa
    // MicroChannel
    // TurboChannel
    // PCIBus
    // VMEBus
    // NuBus
    // PCMCIABus
    // CBus
    // MPIBus
    // MPSABus
    //

    INTERFACE_TYPE  AdapterInterfaceType;

    //
    // Miniport driver routines
    //

    PHW_INITIALIZE HwInitialize;

    PHW_STARTIO HwStartIo;

    PHW_INTERRUPT HwInterrupt;

    PHW_FIND_ADAPTER HwFindAdapter;

    PHW_RESET_BUS HwResetBus;

    PHW_DMA_STARTED HwDmaStarted;

    PHW_ADAPTER_STATE HwAdapterState;

    //
    // Miniport driver resources
    //

    ULONG DeviceExtensionSize;

    ULONG SpecificLuExtensionSize;

    ULONG SrbExtensionSize;

    ULONG NumberOfAccessRanges;

    PVOID Reserved;

    //
    // Data buffers must be mapped into virtual address space.
    //

    UCHAR MapBuffers;

    //
    // The driver will need to tranlate virtual to physical addresses.
    //

    BOOLEAN NeedPhysicalAddresses;

    //
    // Supports tagged queuing
    //

    BOOLEAN TaggedQueuing;

    //
    // Supports auto request sense.
    //

    BOOLEAN AutoRequestSense;

    //
    // Supports multiple requests per logical unit.
    //

    BOOLEAN MultipleRequestPerLu;

    //
    // Support receive event function.
    //

    BOOLEAN ReceiveEvent;

    //
    // Vendor identification length
    //

    USHORT VendorIdLength;

    //
    // Vendor identification
    //

    PVOID VendorId;

    //
    // Pad for alignment and future use.
    //

    union {

        USHORT ReservedUshort;

        //
        // Flags to indicate supported features
        //
        USHORT PortVersionFlags;
    };

    //
    // Device identification length
    //

    USHORT DeviceIdLength;

    //
    // Device identification
    //

    PVOID DeviceId;

    //
    // Stop adapter routine.
    //

    PHW_ADAPTER_CONTROL HwAdapterControl;

    //
    // Initialize to the Build IO routine if one is supported, otherwise
    // should be NULL.
    //

    PHW_BUILDIO HwBuildIo;                      // STORPORT New

} HW_INITIALIZATION_DATA, *PHW_INITIALIZATION_DATA;


// Virtual driver HW_INIT_DATA
typedef
VOID
(*PHW_FREE_ADAPTER_RESOURCES) (
    __in PVOID DeviceExtension
    );

typedef
VOID
(*PHW_PROCESS_SERVICE_REQUEST) (
    __in PVOID DeviceExtension,
    __in PVOID Irp
    );

typedef
VOID
(*PHW_COMPLETE_SERVICE_IRP) (
    __in PVOID DeviceExtension
    );

typedef
VOID
(*PHW_INITIALIZE_TRACING) (
    __in PVOID Arg1,
    __in PVOID Arg2
    );

typedef
VOID
(*PHW_CLEANUP_TRACING) (
    __in PVOID  Arg1
    );
typedef
ULONG
(*PVIRTUAL_HW_FIND_ADAPTER) (
    __in PVOID DeviceExtension,
    __in PVOID HwContext,
    __in PVOID BusInformation,
    __in PVOID LowerDevice,
    __in PCHAR ArgumentString,
    __inout PPORT_CONFIGURATION_INFORMATION ConfigInfo,
    __out PBOOLEAN Again
    );

typedef struct _VIRTUAL_HW_INITIALIZATION_DATA {

    ULONG HwInitializationDataSize;

    //
    // Adapter interface type:
    //
    // Internal
    // Isa
    // Eisa
    // MicroChannel
    // TurboChannel
    // PCIBus
    // VMEBus
    // NuBus
    // PCMCIABus
    // CBus
    // MPIBus
    // MPSABus
    //

    INTERFACE_TYPE  AdapterInterfaceType;

    //
    // Miniport driver routines
    //

    PHW_INITIALIZE HwInitialize;

    PHW_STARTIO HwStartIo;

    PHW_INTERRUPT HwInterrupt;

    PVIRTUAL_HW_FIND_ADAPTER HwFindAdapter;

    PHW_RESET_BUS HwResetBus;

    PHW_DMA_STARTED HwDmaStarted;

    PHW_ADAPTER_STATE HwAdapterState;

    //
    // Miniport driver resources
    //

    ULONG DeviceExtensionSize;

    ULONG SpecificLuExtensionSize;

    ULONG SrbExtensionSize;

    ULONG NumberOfAccessRanges;

    PVOID Reserved;

    //
    // Data buffers must be mapped into virtual address space.
    //

    UCHAR MapBuffers;

    //
    // The driver will need to tranlate virtual to physical addresses.
    //

    BOOLEAN NeedPhysicalAddresses;

    //
    // Supports tagged queuing
    //

    BOOLEAN TaggedQueuing;

    //
    // Supports auto request sense.
    //

    BOOLEAN AutoRequestSense;

    //
    // Supports multiple requests per logical unit.
    //

    BOOLEAN MultipleRequestPerLu;

    //
    // Support receive event function.
    //

    BOOLEAN ReceiveEvent;

    //
    // Vendor identification length
    //

    USHORT VendorIdLength;

    //
    // Vendor identification
    //

    PVOID VendorId;

    //
    // Pad for alignment and future use.
    //

    union {

        USHORT ReservedUshort;

        //
        // Flags to indicate supported features
        //
        USHORT PortVersionFlags;
    };

    //
    // Device identification length
    //

    USHORT DeviceIdLength;

    //
    // Device identification
    //

    PVOID DeviceId;

    //
    // Stop adapter routine.
    //

    PHW_ADAPTER_CONTROL HwAdapterControl;

    //
    // Initialize to the Build IO routine if one is supported, otherwise
    // should be NULL.
    //

    PHW_BUILDIO HwBuildIo;                      // STORPORT New

    PHW_FREE_ADAPTER_RESOURCES HwFreeAdapterResources;

    PHW_PROCESS_SERVICE_REQUEST HwProcessServiceRequest;

    PHW_COMPLETE_SERVICE_IRP HwCompleteServiceIrp;

    //
    // Functions for enabling tracing in miniport
    //
    PHW_INITIALIZE_TRACING HwInitializeTracing;
    PHW_CLEANUP_TRACING    HwCleanupTracing;



} VIRTUAL_HW_INITIALIZATION_DATA, *PVIRTUAL_HW_INITIALIZATION_DATA;

#define DUMP_MINIPORT_VERSION_1         0x0100
#define DUMP_MINIPORT_VERSION           0x0200
#define DUMP_MINIPORT_NAME_LENGTH       15

typedef struct _MINIPORT_MAPPINGS {

    //
    // Structure version
    //
    USHORT Version;
 
    // Pointer to iBF Table
    //
    PVOID IBFTable;

    //
    // Pointer to Nic0 memory map.
    //
    PVOID Nic0Map;
    
    //
    // Pointer to Nic1 memory map.
    //
    PVOID Nic1Map;

    // 
    // Number of additional NICs.
    //
    ULONG NumberOfAdditionalNic;

   //
   // Pointer to memory Map for additional NICs bounded by the number in
   // NumberOfAdditionalNic;
   //
   PVOID NicMap[];

} MINIPORT_MAPPINGS, *PMINIPORT_MAPPINGS;




typedef struct _MINIPORT_DUMP_POINTERS {

    //
    // Structure version
    //
    USHORT Version;

    //
    // Structure size
    //
    USHORT Size;

    //
    // Dump miniport name
    //
    WCHAR DriverName[DUMP_MINIPORT_NAME_LENGTH];

    //
    // Pointer to the DMA adapter object
    //

    struct _ADAPTER_OBJECT *AdapterObject;

    //
    // Register base
    //
    PVOID MappedRegisterBase;

    //
    // Common buffer size, must be <= 64KB
    //
    ULONG CommonBufferSize;

    //
    // Pointer which is passed to dump driver through port configuration.
    //
    PVOID MiniportPrivateDumpData;

    //
    // The following members are part of the
    // PORT_CONFIGURATION_INFORMATION structure
    //

    ULONG SystemIoBusNumber;

    INTERFACE_TYPE AdapterInterfaceType;

    ULONG MaximumTransferLength;

    ULONG NumberOfPhysicalBreaks;

    ULONG AlignmentMask;

    ULONG NumberOfAccessRanges;

    ACCESS_RANGE (*AccessRanges)[];

    UCHAR NumberOfBuses;

    BOOLEAN  Master;

    BOOLEAN MapBuffers;

    UCHAR MaximumNumberOfTargets;

} MINIPORT_DUMP_POINTERS, *PMINIPORT_DUMP_POINTERS;



//
//  Structure used with StorPortLogSystemEvent
//      (and related definitions)
//
//  The structure should be zeroed before use to enable future expansion
//  and backward compatability. The low byte of the interface revision is used to distinguish
//  between minor changes in the interface that are still backward compatible.
//

#define STOR_CURRENT_LOG_INTERFACE_REVISION 0x0100

typedef enum _STOR_EVENT_ASSOCIATION_ENUM {
    StorEventAdapterAssociation = 0,                            // Event is associated with the adapter
    StorEventLunAssociation,                                    // Event is associated with the LUN
    StorEventTargetAssociation,                                 // Event is associated with the target
    StorEventInvalidAssociation                                 // Marks end of valid enumeration range
} STOR_EVENT_ASSOCIATION_ENUM;

typedef struct _STOR_LOG_EVENT_DETAILS {
    ULONG                       InterfaceRevision;              // Revision of this interface (use STOR_CURRENT_LOG_INTERFACE_REVISION)
    ULONG                       Size;                           // Size of this structure (fill in before passing to Storport)
    ULONG                       Flags;                          // Special flags to control this interface (none currently defined)
    STOR_EVENT_ASSOCIATION_ENUM EventAssociation;               // See STOR_EVENT_ASSOCIATION_ENUM enumeration
    ULONG                       PathId;                         // Path portion of SCSI address
    ULONG                       TargetId;                       // Target portion of SCSI address
    ULONG                       LunId;                          // LUN portion of SCSI address
    BOOLEAN                     StorportSpecificErrorCode;      // TRUE if error code is specific to Storport (SP_xxx)
    ULONG                       ErrorCode;                      // Error code, either SP_xxx or standard NTSTATUS code
    ULONG                       UniqueId;                       // Often used to indicate the location of the error
    ULONG                       DumpDataSize;                   // Size in bytes of private data block
    PVOID                       DumpData;                       // Private data block
    ULONG                       StringCount;                    // Count of NUL-terminated Unicode strings
    PWSTR                     * StringList;                     // Unicode strings used for substitution within log messages (%2, %3, %4...)
} STOR_LOG_EVENT_DETAILS, *PSTOR_LOG_EVENT_DETAILS;



typedef
BOOLEAN
(*PSTOR_SYNCHRONIZED_ACCESS)(
    __in PVOID HwDeviceExtension,
    __in PVOID Context
    );


#ifndef _NTDDK_
#define STORPORT_API DECLSPEC_IMPORT
#else
#define STORPORT_API
#endif

//
// Port driver routines called by miniport driver
//

STORPORT_API
ULONG
StorPortInitialize(
    __in PVOID Argument1,
    __in PVOID Argument2,
    __in struct _HW_INITIALIZATION_DATA *HwInitializationData,
    __in_opt PVOID HwContext
    );

STORPORT_API
VOID
StorPortFreeDeviceBase(
    __in PVOID HwDeviceExtension,
    __in PVOID MappedAddress
    );

STORPORT_API
ULONG
StorPortGetBusData(
    __in PVOID DeviceExtension,
    __in ULONG BusDataType,
    __in ULONG SystemIoBusNumber,
    __in ULONG SlotNumber,
    __inout __drv_when(Length!=0, __bcount(Length))
    PVOID Buffer,
    __in ULONG Length
    );

STORPORT_API
ULONG
StorPortSetBusDataByOffset(
    __in PVOID DeviceExtension,
    __in ULONG BusDataType,
    __in ULONG SystemIoBusNumber,
    __in ULONG SlotNumber,
    __in_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    );

STORPORT_API
PVOID
StorPortGetDeviceBase(
    __in PVOID HwDeviceExtension,
    __in INTERFACE_TYPE BusType,
    __in ULONG SystemIoBusNumber,
    __in STOR_PHYSICAL_ADDRESS IoAddress,
    __in ULONG NumberOfBytes,
    __in BOOLEAN InIoSpace
    );

STORPORT_API
PVOID
StorPortGetLogicalUnit(
    __in PVOID HwDeviceExtension,
    __in UCHAR PathId,
    __in UCHAR TargetId,
    __in UCHAR Lun
    );

STORPORT_API
PSTOR_SCATTER_GATHER_LIST
StorPortGetScatterGatherList(
    __in PVOID HwDeviceExtension,
    __in PSCSI_REQUEST_BLOCK Srb
    );

STORPORT_API
STOR_PHYSICAL_ADDRESS
StorPortGetPhysicalAddress(
    __in PVOID HwDeviceExtension,
    __in_opt PSCSI_REQUEST_BLOCK Srb,
    __in PVOID VirtualAddress,
    __out ULONG *Length
    );

STORPORT_API
PVOID
StorPortGetVirtualAddress(
    __in PVOID HwDeviceExtension,
    __in STOR_PHYSICAL_ADDRESS PhysicalAddress
    );

STORPORT_API
PVOID
StorPortGetUncachedExtension(
    __in PVOID HwDeviceExtension,
    __in PPORT_CONFIGURATION_INFORMATION ConfigInfo,
    __in ULONG NumberOfBytes
    );

STORPORT_API
BOOLEAN
StorPortPauseDevice(
    __in PVOID HwDeviceExtension,
    __in UCHAR PathId,
    __in UCHAR TargetId,
    __in UCHAR Lun,
    __in ULONG Timeout
    );

STORPORT_API
BOOLEAN
StorPortResumeDevice(
    __in PVOID HwDeviceExtension,
    __in UCHAR PathId,
    __in UCHAR TargetId,
    __in UCHAR Lun
    );

STORPORT_API
BOOLEAN
StorPortPause(
    __in PVOID HwDeviceExtension,
    __in ULONG Timeout
    );

STORPORT_API
BOOLEAN
StorPortResume(
    __in PVOID HwDeviceExtension
    );

STORPORT_API
BOOLEAN
StorPortDeviceBusy(
    __in PVOID HwDeviceExtension,
    __in UCHAR PathId,
    __in UCHAR TargetId,
    __in UCHAR Lun,
    __in ULONG RequestsToComplete
    );

STORPORT_API
BOOLEAN
StorPortDeviceReady(
    __in PVOID HwDeviceExtension,
    __in UCHAR PathId,
    __in UCHAR TargetId,
    __in UCHAR Lun
    );

STORPORT_API
BOOLEAN
StorPortBusy(
    __in PVOID HwDeviceExtension,
    __in ULONG RequestsToComplete
    );

STORPORT_API
BOOLEAN
StorPortReady(
    __in PVOID HwDeviceExtension
    );

STORPORT_API
BOOLEAN
StorPortSetDeviceQueueDepth(
    __in PVOID HwDeviceExtension,
    __in UCHAR PathId,
    __in UCHAR TargetId,
    __in UCHAR Lun,
    __in ULONG Depth
    );

STORPORT_API
VOID
StorPortNotification(
    __in SCSI_NOTIFICATION_TYPE NotificationType,
    __in PVOID HwDeviceExtension,
    ...
    );

STORPORT_API
VOID
StorPortLogError(
    __in PVOID HwDeviceExtension,
    __in_opt PSCSI_REQUEST_BLOCK Srb,
    __in UCHAR PathId,
    __in UCHAR TargetId,
    __in UCHAR Lun,
    __in ULONG ErrorCode,
    __in ULONG UniqueId
    );

STORPORT_API
VOID
StorPortCompleteRequest(
    __in PVOID HwDeviceExtension,
    __in UCHAR PathId,
    __in UCHAR TargetId,
    __in UCHAR Lun,
    __in UCHAR SrbStatus
    );

STORPORT_API
VOID
StorPortStallExecution(
    __in ULONG Delay
    );

STORPORT_API
BOOLEAN
StorPortSynchronizeAccess(
    __in PVOID HwDeviceExtension,
    __in PSTOR_SYNCHRONIZED_ACCESS SynchronizedAccessRoutine,
    __in_opt PVOID Context
    );

#if defined(_M_AMD64)

#define StorPortReadPortUchar(h, p) READ_PORT_UCHAR(p)
#define StorPortReadPortUshort(h, p) READ_PORT_USHORT(p)
#define StorPortReadPortUlong(h, p) READ_PORT_ULONG(p)

#define StorPortReadPortBufferUchar(h, p, b, c) READ_PORT_BUFFER_UCHAR(p, b, c)
#define StorPortReadPortBufferUshort(h, p, b, c) READ_PORT_BUFFER_USHORT(p, b, c)
#define StorPortReadPortBufferUlong(h, p, b, c) READ_PORT_BUFFER_ULONG(p, b, c)

#define StorPortReadRegisterUchar(h, r) READ_REGISTER_UCHAR(r)
#define StorPortReadRegisterUshort(h, r) READ_REGISTER_USHORT(r)
#define StorPortReadRegisterUlong(h, r) READ_REGISTER_ULONG(r)

#define StorPortReadRegisterBufferUchar(h, r, b, c) READ_REGISTER_BUFFER_UCHAR(r, b, c)
#define StorPortReadRegisterBufferUshort(h, r, b, c) READ_REGISTER_BUFFER_USHORT(r, b, c)
#define StorPortReadRegisterBufferUlong(h, r, b, c) READ_REGISTER_BUFFER_ULONG(r, b, c)

#define StorPortWritePortUchar(h, p, v) WRITE_PORT_UCHAR(p, v)
#define StorPortWritePortUshort(h, p, v) WRITE_PORT_USHORT(p, v)
#define StorPortWritePortUlong(h, p, v) WRITE_PORT_ULONG(p, v)

#define StorPortWritePortBufferUchar(h, p, b, c) WRITE_PORT_BUFFER_UCHAR(p, b, c)
#define StorPortWritePortBufferUshort(h, p, b, c) WRITE_PORT_BUFFER_USHORT(p, b, c)
#define StorPortWritePortBufferUlong(h, p, b, c) WRITE_PORT_BUFFER_ULONG(p, b, c)

#define StorPortWriteRegisterUchar(h, r, v) WRITE_REGISTER_UCHAR(r, v)
#define StorPortWriteRegisterUshort(h, r, v) WRITE_REGISTER_USHORT(r, v)
#define StorPortWriteRegisterUlong(h, r, v) WRITE_REGISTER_ULONG(r, v)

#define StorPortWriteRegisterBufferUchar(h, r, b, c) WRITE_REGISTER_BUFFER_UCHAR(r, b, c)
#define StorPortWriteRegisterBufferUshort(h, r, b, c) WRITE_REGISTER_BUFFER_USHORT(r, b, c)
#define StorPortWriteRegisterBufferUlong(h, r, b, c) WRITE_REGISTER_BUFFER_ULONG(r, b, c)

#define StorPortMoveMemory memmove

#else

STORPORT_API
UCHAR
StorPortReadPortUchar(
    __in PVOID HwDeviceExtension,
    __in PUCHAR Port
    );

STORPORT_API
USHORT
StorPortReadPortUshort(
    __in PVOID HwDeviceExtension,
    __in PUSHORT Port
    );

STORPORT_API
ULONG
StorPortReadPortUlong(
    __in PVOID HwDeviceExtension,
    __in PULONG Port
    );

STORPORT_API
VOID
StorPortReadPortBufferUchar(
    __in PVOID HwDeviceExtension,
    __in PUCHAR Port,
    __in_ecount(Count) PUCHAR Buffer,
    __in ULONG  Count
    );

STORPORT_API
VOID
StorPortReadPortBufferUshort(
    __in PVOID HwDeviceExtension,
    __in PUSHORT Port,
    __in_ecount(Count) PUSHORT Buffer,
    __in ULONG Count
    );

STORPORT_API
VOID
StorPortReadPortBufferUlong(
    __in PVOID HwDeviceExtension,
    __in PULONG Port,
    __in_ecount(Count) PULONG Buffer,
    __in ULONG Count
    );

STORPORT_API
UCHAR
StorPortReadRegisterUchar(
    __in PVOID HwDeviceExtension,
    __in PUCHAR Register
    );

STORPORT_API
USHORT
StorPortReadRegisterUshort(
    __in PVOID HwDeviceExtension,
    __in PUSHORT Register
    );

STORPORT_API
ULONG
StorPortReadRegisterUlong(
    __in PVOID HwDeviceExtension,
    __in PULONG Register
    );

STORPORT_API
VOID
StorPortReadRegisterBufferUchar(
    __in PVOID HwDeviceExtension,
    __in PUCHAR Register,
    __in_ecount(Count) PUCHAR Buffer,
    __in ULONG Count
    );

STORPORT_API
VOID
StorPortReadRegisterBufferUshort(
    __in PVOID HwDeviceExtension,
    __in PUSHORT Register,
    __in_ecount(Count) PUSHORT Buffer,
    __in ULONG Count
    );

STORPORT_API
VOID
StorPortReadRegisterBufferUlong(
    __in PVOID HwDeviceExtension,
    __in PULONG Register,
    __in_ecount(Count) PULONG Buffer,
    __in ULONG Count
    );

STORPORT_API
VOID
StorPortWritePortUchar(
    __in PVOID HwDeviceExtension,
    __in PUCHAR Port,
    __in UCHAR Value
    );

STORPORT_API
VOID
StorPortWritePortUshort(
    __in PVOID HwDeviceExtension,
    __in PUSHORT Port,
    __in USHORT Value
    );

STORPORT_API
VOID
StorPortWritePortUlong(
    __in PVOID HwDeviceExtension,
    __in PULONG Port,
    __in ULONG Value
    );

STORPORT_API
VOID
StorPortWritePortBufferUchar(
    __in PVOID HwDeviceExtension,
    __in PUCHAR Port,
    __in_ecount(Count) PUCHAR Buffer,
    __in ULONG  Count
    );

STORPORT_API
VOID
StorPortWritePortBufferUshort(
    __in PVOID HwDeviceExtension,
    __in PUSHORT Port,
    __in_ecount(Count) PUSHORT Buffer,
    __in ULONG Count
    );

STORPORT_API
VOID
StorPortWritePortBufferUlong(
    __in PVOID HwDeviceExtension,
    __in PULONG Port,
    __in_ecount(Count) PULONG Buffer,
    __in ULONG Count
    );

STORPORT_API
VOID
StorPortWriteRegisterUchar(
    __in PVOID HwDeviceExtension,
    __in PUCHAR Register,
    __in UCHAR Value
    );

STORPORT_API
VOID
StorPortWriteRegisterUshort(
    __in PVOID HwDeviceExtension,
    __in PUSHORT Register,
    __in USHORT Value
    );

STORPORT_API
VOID
StorPortWriteRegisterUlong(
    __in PVOID HwDeviceExtension,
    __in PULONG Register,
    __in ULONG Value
    );

STORPORT_API
VOID
StorPortWriteRegisterBufferUchar(
    __in PVOID HwDeviceExtension,
    __in PUCHAR Register,
    __in_ecount(Count) PUCHAR Buffer,
    __in ULONG  Count
    );

STORPORT_API
VOID
StorPortWriteRegisterBufferUshort(
    __in PVOID HwDeviceExtension,
    __in PUSHORT Register,
    __in_ecount(Count) PUSHORT Buffer,
    __in ULONG Count
    );

STORPORT_API
VOID
StorPortWriteRegisterBufferUlong(
    __in PVOID HwDeviceExtension,
    __in PULONG Register,
    __in_ecount(Count) PULONG Buffer,
    __in ULONG Count
    );

STORPORT_API
VOID
StorPortMoveMemory(
    __in_bcount(Length) PVOID WriteBuffer,
    __in_bcount(Length) PVOID ReadBuffer,
    __in ULONG Length
    );


#endif

#if (NTDDI_VERSION >= NTDDI_WS03SP1)
#pragma intrinsic (memcpy)

#define StorPortCopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#endif

STORPORT_API
STOR_PHYSICAL_ADDRESS
StorPortConvertUlongToPhysicalAddress(
    __in ULONG_PTR UlongAddress
    );

STORPORT_API
ULONG
StorPortConvertPhysicalAddressToUlong(
    __in STOR_PHYSICAL_ADDRESS Address
    );

STORPORT_API
VOID
StorPortQuerySystemTime(
    __out PLARGE_INTEGER CurrentTime
    );

#define StorPortConvertPhysicalAddressToUlong(Address) ((Address).LowPart)
#define StorPortConvertPhysicalAddressToULong64(Address) ((Address).QuadPart)

#define MINIPORT_REG_SZ     1
#define MINIPORT_REG_BINARY 3
#define MINIPORT_REG_DWORD  4

__drv_allocatesMem(Mem)
__bcount(Length)
STORPORT_API
PUCHAR
StorPortAllocateRegistryBuffer(
    __in PVOID HwDeviceExtension,
    __in PULONG Length
    );


STORPORT_API
VOID
StorPortFreeRegistryBuffer(
    __in PVOID HwDeviceExtension,
    __in __drv_freesMem(Mem) PUCHAR Buffer
    );

BOOLEAN
StorPortRegistryRead(
    __in PVOID HwDeviceExtension,
    __in PUCHAR ValueName,
    __in ULONG Global,
    __in ULONG Type,
    __out PUCHAR Buffer,
    __inout PULONG BufferLength
    );

STORPORT_API
BOOLEAN
StorPortRegistryWrite(
    __in PVOID HwDeviceExtension,
    __in PUCHAR ValueName,
    __in ULONG Global,
    __in ULONG Type,
    __in_bcount(BufferLength) PUCHAR Buffer,
    __in ULONG BufferLength
    );

__drv_preferredFunction("(see documentation)", "Obsolete")
STORPORT_API
BOOLEAN
StorPortValidateRange(
    __in PVOID HwDeviceExtension,
    __in INTERFACE_TYPE BusType,
    __in ULONG SystemIoBusNumber,
    __in STOR_PHYSICAL_ADDRESS IoAddress,
    __in ULONG NumberOfBytes,
    __in BOOLEAN InIoSpace
    );

STORPORT_API
VOID
StorPortDebugPrint(
    __in ULONG DebugPrintLevel,
    __in PCCHAR DebugMessage,
    ...
    );

BOOLEAN
FORCEINLINE
StorPortEnablePassiveInitialization(
    __in PVOID DeviceExtension,
    __in PHW_PASSIVE_INITIALIZE_ROUTINE HwPassiveInitializeRoutine
    )
{
    LONG Succ;

    Succ = FALSE;
    StorPortNotification (EnablePassiveInitialization,
                          DeviceExtension,
                          HwPassiveInitializeRoutine,
                          &Succ);

    return (BOOLEAN)Succ;
}

VOID
FORCEINLINE
StorPortInitializeDpc(
    __in PVOID DeviceExtension,
    __out PSTOR_DPC Dpc,
    __in PHW_DPC_ROUTINE HwDpcRoutine
    )
{
    StorPortNotification (InitializeDpc,
                          DeviceExtension,
                          Dpc,
                          HwDpcRoutine);
}

BOOLEAN
FORCEINLINE
StorPortIssueDpc(
    __in PVOID DeviceExtension,
    __in PSTOR_DPC Dpc,
    __in PVOID SystemArgument1,
    __in PVOID SystemArgument2
    )
{
    LONG Succ;

    Succ = FALSE;
    StorPortNotification (IssueDpc,
                          DeviceExtension,
                          Dpc,
                          SystemArgument1,
                          SystemArgument2,
                          &Succ);
    return (BOOLEAN)Succ;
}

// StorPortNotification is a polymorphic function that handles many different types of requests, 
// making it difficult to annotate in a manner that would cover all possible uses.
// Because StorPortNotification returns VOID, the scanning engine should assume the LockHandle was acquired as asked.
// This assumption requires the suppressing the PFD warning generated (28104).
VOID
FORCEINLINE
StorPortAcquireSpinLock(
    __in PVOID DeviceExtension,
    __in STOR_SPINLOCK SpinLock,
    __in PVOID LockContext,
    __inout __deref __drv_acquiresExclusiveResource(LockHandle)
    PSTOR_LOCK_HANDLE LockHandle
    )
#pragma warning (suppress: 28104)
{
    StorPortNotification (AcquireSpinLock,
                          DeviceExtension,
                          SpinLock,
                          LockContext,
                          LockHandle);
}

VOID
FORCEINLINE
StorPortReleaseSpinLock(
    __in PVOID DeviceExtension,
    __inout __deref __drv_releasesExclusiveResource(LockHandle)
    PSTOR_LOCK_HANDLE LockHandle
    )
{
    StorPortNotification (ReleaseSpinLock,
                          DeviceExtension,
                          LockHandle);
}

STORPORT_API
ULONG
StorPortExtendedFunction(
    __in STORPORT_FUNCTION_CODE FunctionCode,
    __in PVOID HwDeviceExtension,
    ...
    );

ULONG
FORCEINLINE
StorPortAllocatePool(
    __in PVOID HwDeviceExtension,
    __in ULONG NumberOfBytes,
    __in ULONG Tag,
    __out __deref __drv_when(return!=0, __drv_valueIs(==0))
    __deref __drv_when(return==0, __drv_aliasesMem __drv_allocatesMem(Mem) __drv_valueIs(!=0) 
        __bcount(NumberOfBytes))
    PVOID *BufferPointer
    )
{
    return StorPortExtendedFunction(ExtFunctionAllocatePool,
                                    HwDeviceExtension,
                                    NumberOfBytes,
                                    Tag,
                                    BufferPointer);
}

ULONG
FORCEINLINE
StorPortFreePool(
    __in PVOID HwDeviceExtension,
    __in __drv_freesMem(Mem) PVOID BufferPointer
    )
{
    return StorPortExtendedFunction(ExtFunctionFreePool,
                                    HwDeviceExtension,
                                    BufferPointer);
}

ULONG
FORCEINLINE
StorPortAllocateMdl(
    __in PVOID HwDeviceExtension,
    __in_bcount(NumberOfBytes) PVOID BufferPointer,
    __in ULONG NumberOfBytes,
    __out __deref __drv_when(return!=0, __drv_valueIs(==0))
    __deref __drv_when(return==0, __drv_aliasesMem __drv_allocatesMem(Mem) __drv_valueIs(!=0))
    PVOID *Mdl
    )
{
    return StorPortExtendedFunction(ExtFunctionAllocateMdl,
                                    HwDeviceExtension,
                                    BufferPointer,
                                    NumberOfBytes,
                                    Mdl);
}

ULONG
FORCEINLINE
StorPortFreeMdl(
    __in PVOID HwDeviceExtension,
    __in __drv_freesMem(Mem) PVOID Mdl
    )
{
    return StorPortExtendedFunction(ExtFunctionFreeMdl,
                                    HwDeviceExtension,
                                    Mdl);
}

ULONG
FORCEINLINE
StorPortBuildMdlForNonPagedPool(
    __in PVOID HwDeviceExtension,
    __inout PVOID Mdl
    )
{
    return StorPortExtendedFunction(ExtFunctionBuildMdlForNonPagedPool,
                                    HwDeviceExtension,
                                    Mdl);
}

ULONG
FORCEINLINE
StorPortGetSystemAddress(
    __in PVOID HwDeviceExtension,
    __in PSCSI_REQUEST_BLOCK Srb,
    __deref_out PVOID *SystemAddress
    )
{
    return StorPortExtendedFunction(ExtFunctionGetSystemAddress,
                                    HwDeviceExtension,
                                    Srb,
                                    SystemAddress);
}

ULONG
FORCEINLINE
StorPortGetOriginalMdl(
    __in PVOID HwDeviceExtension,
    __in PSCSI_REQUEST_BLOCK Srb,
    __deref_out PVOID *Mdl
    )
{
    return StorPortExtendedFunction(ExtFunctionGetOriginalMdl,
                                    HwDeviceExtension,
                                    Srb,
                                    Mdl);
}

ULONG
FORCEINLINE
StorPortCompleteServiceIrp(
    __in PVOID HwDeviceExtension,
    __in PVOID Irp
    )
{
    return StorPortExtendedFunction(ExtFunctionCompleteServiceIrp,
                                    HwDeviceExtension,
                                    Irp);
}

ULONG
FORCEINLINE
StorPortGetDeviceObjects(
    __in PVOID HwDeviceExtension,
    __deref_out PVOID *AdapterDeviceObject,
    __deref_out PVOID *PhysicalDeviceObject,
    __deref_out PVOID *LowerDeviceObject
    )
{
    return StorPortExtendedFunction(ExtFunctionGetDeviceObjects,
                                    HwDeviceExtension,
                                    AdapterDeviceObject,
                                    PhysicalDeviceObject,
                                    LowerDeviceObject);
}

ULONG
FORCEINLINE
StorPortBuildScatterGatherList(
    __in PVOID HwDeviceExtension,
    __in PVOID Mdl,
    __in_bcount(Length) PVOID CurrentVa,
    __in ULONG Length,
    __in PpostScaterGatherExecute ExecutionRoutine,
    __in PVOID Context,
    __in BOOLEAN WriteToDevice,
    __inout_bcount(ScatterGatherBufferLength) PVOID ScatterGatherBuffer,
    __in ULONG ScatterGatherBufferLength
    )
{
    return StorPortExtendedFunction(ExtFunctionBuildScatterGatherList,
                                    HwDeviceExtension,
                                    Mdl,
                                    CurrentVa,
                                    Length,
                                    ExecutionRoutine,
                                    Context,
                                    WriteToDevice,
                                    ScatterGatherBuffer,
                                    ScatterGatherBufferLength);
}

ULONG
FORCEINLINE
StorPortPutScatterGatherList(
    __in PVOID HwDeviceExtension,
    __in PSTOR_SCATTER_GATHER_LIST ScatterGatherList,
    __in BOOLEAN WriteToDevice
    )
{
    return StorPortExtendedFunction(ExtFunctionPutScatterGatherList,
                                    HwDeviceExtension,
                                    ScatterGatherList,
                                    WriteToDevice);
}

ULONG
FORCEINLINE
StorPortAcquireMSISpinLock(
    __in PVOID HwDeviceExtension,
    __in ULONG MessageId,
    __in PULONG OldIrql
    )
{
    return StorPortExtendedFunction(ExtFunctionAcquireMSISpinLock,
                                    HwDeviceExtension,
                                    MessageId,
                                    OldIrql);
}

ULONG
FORCEINLINE
StorPortReleaseMSISpinLock(
    __in PVOID HwDeviceExtension,
    __in ULONG MessageId,
    __in ULONG OldIrql
    )
{
    return StorPortExtendedFunction(ExtFunctionReleaseMSISpinLock,
                                    HwDeviceExtension,
                                    MessageId,
                                    OldIrql);
}

ULONG
FORCEINLINE
StorPortGetMSIInfo(
    __in PVOID HwDeviceExtension,
    __in ULONG MessageId,
    __out PMESSAGE_INTERRUPT_INFORMATION InterruptInfo
    )
{
    return StorPortExtendedFunction(ExtFunctionGetMessageInterruptInformation,
                                    HwDeviceExtension,
                                    MessageId,
                                    InterruptInfo);
}

ULONG
FORCEINLINE
StorPortInitializePerfOpts(
    __in PVOID HwDeviceExtension,
    __in BOOLEAN Query,
    __inout PPERF_CONFIGURATION_DATA PerfConfigData
    )
{
    return StorPortExtendedFunction(ExtFunctionInitializePerformanceOptimizations,
                                    HwDeviceExtension,
                                    Query,
                                    PerfConfigData);
}

ULONG
FORCEINLINE
StorPortGetStartIoPerfParams(
    __in PVOID HwDeviceExtension,
    __in PSCSI_REQUEST_BLOCK Srb,
    __inout PSTARTIO_PERFORMANCE_PARAMETERS StartIoPerfParams
    )
{
    return StorPortExtendedFunction(ExtFunctionGetStartIoPerformanceParameters,
                                    HwDeviceExtension,
                                    Srb,
                                    StartIoPerfParams);
}

ULONG
FORCEINLINE
StorPortLogSystemEvent(
    __in PVOID HwDeviceExtension,
    __inout PSTOR_LOG_EVENT_DETAILS LogDetails,
    __inout PULONG MaximumSize
    )
{
    return StorPortExtendedFunction(ExtFunctionLogSystemEvent,
                                    HwDeviceExtension,
                                    LogDetails,
                                    MaximumSize);
}

ULONG
FORCEINLINE
StorPortGetCurrentProcessorNumber (
    __in PVOID HwDeviceExtension,
    __out PPROCESSOR_NUMBER ProcNumber
    )
{
    return StorPortExtendedFunction(ExtFunctionGetCurrentProcessorNumber,
                                    HwDeviceExtension,
                                    ProcNumber);
}

ULONG
FORCEINLINE
StorPortGetGroupAffinity (
    __in PVOID HwDeviceExtension,
    __in USHORT GroupNumber,
    __out PKAFFINITY GroupAffinityMask
    )
{
    return StorPortExtendedFunction(ExtFunctionGetGroupAffinity,
                                    HwDeviceExtension,
                                    GroupNumber,
                                    GroupAffinityMask);
}

ULONG
FORCEINLINE
StorPortGetActiveGroupCount (
    __in PVOID HwDeviceExtension,
    __out PUSHORT NumberGroups
    )
{
    return StorPortExtendedFunction(ExtFunctionGetActiveGroupCount,
                                    HwDeviceExtension,
                                    NumberGroups);
}

ULONG
FORCEINLINE
StorPortGetNodeAffinity (
    __in PVOID HwDeviceExtension,
    __in ULONG NodeNumber,
    __out PGROUP_AFFINITY NodeAffinityMask
    )
{
     return StorPortExtendedFunction(ExtFunctionGetNodeAffinity,
                                    HwDeviceExtension,
                                    NodeNumber,
                                    NodeAffinityMask);
}

ULONG
FORCEINLINE
StorPortGetActiveNodeCount (
    __in PVOID HwDeviceExtension,
    __out PULONG NumberNodes
    )
{
    return StorPortExtendedFunction(ExtFunctionGetActiveNodeCount,
                                    HwDeviceExtension,
                                    NumberNodes);
}

ULONG
FORCEINLINE
StorPortGetHighestNodeNumber (
    __in PVOID HwDeviceExtension,
    __out PULONG HighestNode
    )
{
    return StorPortExtendedFunction(ExtFunctionGetHighestNodeNumber,
                                    HwDeviceExtension,
                                    HighestNode);
}

__drv_maxIRQL(DISPATCH_LEVEL)
ULONG
FORCEINLINE
StorPortGetLogicalProcessorRelationship (
    __in PVOID HwDeviceExtension,
    __in_opt PPROCESSOR_NUMBER ProcessorNumber,
    __in LOGICAL_PROCESSOR_RELATIONSHIP RelationshipType,
    __out_bcount(*Length) PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX Information,
    __inout PULONG Length
    )
{
    return StorPortExtendedFunction(ExtFunctionGetLogicalProcessorRelationship,
                                    HwDeviceExtension,
                                    ProcessorNumber,
                                    RelationshipType,
                                    Information,
                                    Length);
}

__drv_maxIRQL(DISPATCH_LEVEL)
ULONG
FORCEINLINE
StorPortAllocateContiguousMemorySpecifyCacheNode (
    __in PVOID HwDeviceExtension,
    __in SIZE_T NumberOfBytes,
    __in PHYSICAL_ADDRESS LowestAcceptableAddress,
    __in PHYSICAL_ADDRESS HighestAcceptableAddress,
    __in_opt PHYSICAL_ADDRESS BoundaryAddressMultiple,
    __in MEMORY_CACHING_TYPE CacheType,
    __in NODE_REQUIREMENT PreferredNode,
    __out PVOID* BufferPointer
    )
{
    return StorPortExtendedFunction(ExtFunctionAllocateContiguousMemorySpecifyCacheNode,
                                    HwDeviceExtension,
                                    NumberOfBytes,
                                    LowestAcceptableAddress,
                                    HighestAcceptableAddress,
                                    BoundaryAddressMultiple,
                                    CacheType,
                                    PreferredNode,
                                    BufferPointer);
}

ULONG
FORCEINLINE
StorPortFreeContiguousMemorySpecifyCache (
    __in PVOID HwDeviceExtension,
    __in_bcount (NumberOfBytes) PVOID BaseAddress,
    __in SIZE_T NumberOfBytes,
    __in MEMORY_CACHING_TYPE CacheType
    )
{
    return StorPortExtendedFunction(ExtFunctionFreeContiguousMemorySpecifyCache,
                                    HwDeviceExtension,
                                    BaseAddress,
                                    NumberOfBytes,
                                    CacheType);
}

//
// Include SCSIPORT definitions for backwards compatability.
//

#if defined (STOR_USE_SCSI_ALIASES)

#define ScsiPortInitialize StorPortInitialize
#define ScsiPortFreeDeviceBase StorPortFreeDeviceBase
#define ScsiPortGetBusData StorPortGetBusData
#define ScsiPortSetBusDataByOffset StorPortSetBusDataByOffset
#define ScsiPortGetDeviceBase StorPortGetDeviceBase
#define ScsiPortGetLogicalUnit StorPortGetLogicalUnit
#define ScsiPortGetSrb StorPortGetSrb
#define ScsiPortGetPhysicalAddress StorPortGetPhysicalAddress
#define ScsiPortGetVirtualAddress StorPortGetVirtualAddress
#define ScsiPortGetUncachedExtension StorPortGetUncachedExtension
#define ScsiPortFlushDma StorPortFlushDma
#define ScsiPortIoMapTransfer StorPortIoMapTransfer
#define ScsiPortNotification StorPortNotification
#define ScsiPortLogError StorPortLogError
#define ScsiPortCompleteRequest StorPortCompleteRequest
#define ScsiPortMoveMemory StorPortMoveMemory
#define ScsiPortReadPortUchar(Port) StorPortReadPortUchar(NULL, Port)
#define ScsiPortReadPortUshort(Port) StorPortReadPortUshort(NULL, Port)
#define ScsiPortReadPortUlong(Port) StorPortReadPortUlong(NULL, Port)
#define ScsiPortReadPortBufferUchar(Port, Buffer, Count) StorPortReadPortBufferUchar(NULL, Port, Buffer, Count)
#define ScsiPortReadPortBufferUshort(Port, Buffer, Count) StorPortReadPortBufferUshort(NULL, Port, Buffer, Count)
#define ScsiPortReadPortBufferUlong(Port, Buffer, Count) StorPortReadPortBufferUlong(NULL, Port, Buffer, Count)
#define ScsiPortReadRegisterUchar(Register) StorPortReadRegisterUchar(NULL, Register)
#define ScsiPortReadRegisterUshort(Register) StorPortReadRegisterUshort(NULL, Register)
#define ScsiPortReadRegisterUlong(Register) StorPortReadRegisterUlong(NULL, Register)
#define ScsiPortReadRegisterBufferUchar(Register, Buffer, Count) StorPortReadRegisterBufferUchar(NULL, Register, Buffer, Count)
#define ScsiPortReadRegisterBufferUshort(Register, Buffer, Count) StorPortReadRegisterBufferUshort(NULL, Register, Buffer, Count)
#define ScsiPortReadRegisterBufferUlong(Register, Buffer, Count) StorPortReadRegisterBufferUlong(NULL, Register, Buffer, Count)
#define ScsiPortStallExecution StorPortStallExecution
#define ScsiPortWritePortUchar(Port, Value) StorPortWritePortUchar(NULL, Port, Value)
#define ScsiPortWritePortUshort(Port, Value) StorPortWritePortUshort(NULL, Port, Value)
#define ScsiPortWritePortUlong(Port, Value) StorPortWritePortUlong(NULL, Port, Value)
#define ScsiPortWritePortBufferUchar(Port, Buffer, Count) StorPortWritePortBufferUchar(NULL, Port, Buffer, Count)
#define ScsiPortWritePortBufferUshort(Port, Buffer, Count) StorPortWritePortBufferUshort(NULL, Port, Buffer, Count)
#define ScsiPortWritePortBufferUlong(Port, Buffer, Count) StorPortWritePortBufferUlong(NULL, Port, Buffer, Count)
#define ScsiPortWriteRegisterUchar(Register, Value) StorPortWriteRegisterUchar(NULL, Register, Value)
#define ScsiPortWriteRegisterUshort(Register, Value) StorPortWriteRegisterUshort(NULL, Register, Value)
#define ScsiPortWriteRegisterUlong(Register, Value) StorPortWriteRegisterUlong(NULL, Register, Value)
#define ScsiPortWriteRegisterBufferUchar(Register, Buffer, Count) StorPortWriteRegisterBufferUchar(NULL, Register, Buffer, Count)
#define ScsiPortWriteRegisterBufferUshort(Register, Buffer, Count) StorPortWriteRegisterBufferUshort(NULL, Register, Buffer, Count)
#define ScsiPortWriteRegisterBufferUlong(Register, Buffer, Count) StorPortWriteRegisterBufferUlong(NULL, Register, Buffer, Count)
#define ScsiPortConvertUlongToPhysicalAddress StorPortConvertUlongToPhysicalAddress
#define ScsiPortConvertPhysicalAddressToUlong StorPortConvertPhysicalAddressToUlong
#define ScsiPortQuerySystemTime StorPortQuerySystemTime
#define ScsiPortValidateRange StorPortValidateRange
#define ScsiDebugPrint StorPortDebugPrint

typedef PHYSICAL_ADDRESS SCSI_PHYSICAL_ADDRESS, *PSCSI_PHYSICAL_ADDRESS;

#endif // STOR_USE_SCSI_ALIASES


#if _MSC_VER >= 1200
#pragma warning(pop) // un-sets any local warning changes
#else
#pragma warning(default:4200) // array[0] is not a warning for this file
#pragma warning(default:4201) // nonstandard extension used : nameless struct/union
#pragma warning(default:4214) // nonstandard extension used : bit field types other than int
#endif

#endif // !defined _NTSTORPORT_

