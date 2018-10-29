/*++

Copyright (C) Microsoft Corporation, 1991 - 1999

Module Name:

    disk.c

Abstract:

    SCSI disk class driver

Environment:

    kernel mode only

Notes:

Revision History:

--*/

#include "ntddk.h"
#include "scsi.h"
#include <wmidata.h>
#include "classpnp.h"

#include <wmistr.h>
#include "ntstrsafe.h"

//
// Set component ID for DbgPrintEx calls
//
#ifndef DEBUG_COMP_ID
#define DEBUG_COMP_ID   DPFLTR_DISK_ID
#endif

//
// Include header file and setup GUID for tracing
//
#include <storswtr.h>
#define WPP_GUID_DISK           (945186BF, 3DD6, 4f3f, 9C8E, 9EDD3FC9D558)
#ifndef WPP_CONTROL_GUIDS
#define WPP_CONTROL_GUIDS       WPP_CONTROL_GUIDS_NORMAL_FLAGS(WPP_GUID_DISK)
#endif

#ifdef ExAllocatePool
#undef ExAllocatePool
#define ExAllocatePool #assert(FALSE)
#endif

#define DISK_TAG_GENERAL        ' DcS'  // "ScD " - generic tag
#define DISK_TAG_SMART          'aDcS'  // "ScDa" - SMART allocations
#define DISK_TAG_INFO_EXCEPTION 'ADcS'  // "ScDA" - Info Exceptions
#define DISK_TAG_DISABLE_CACHE  'CDcS'  // "ScDC" - disable cache paths
#define DISK_TAG_CCONTEXT       'cDcS'  // "ScDc" - disk allocated completion context
#define DISK_TAG_DISK_GEOM      'GDcS'  // "ScDG" - disk geometry buffer
#define DISK_TAG_UPDATE_GEOM    'gDcS'  // "ScDg" - update disk geometry paths
#define DISK_TAG_SENSE_INFO     'IDcS'  // "ScDI" - sense info buffers
#define DISK_TAG_PNP_ID         'iDcS'  // "ScDp" - pnp ids
#define DISK_TAG_MODE_DATA      'MDcS'  // "ScDM" - mode data buffer
#define DISK_CACHE_MBR_CHECK    'mDcS'  // "ScDM" - mbr checksum code
#define DISK_TAG_NAME           'NDcS'  // "ScDN" - disk name code
#define DISK_TAG_READ_CAP       'PDcS'  // "ScDP" - read capacity buffer
#define DISK_TAG_PART_LIST      'pDcS'  // "ScDp" - disk partition lists
#define DISK_TAG_SRB            'SDcS'  // "ScDS" - srb allocation
#define DISK_TAG_START          'sDcS'  // "ScDs" - start device paths
#define DISK_TAG_UPDATE_CAP     'UDcS'  // "ScDU" - update capacity path
#define DISK_TAG_WI_CONTEXT     'WDcS'  // "ScDW" - work-item context


#if defined(_X86_) || defined(_AMD64_)

//
// Disk device data
//

typedef enum _DISK_GEOMETRY_SOURCE {
    DiskGeometryUnknown,
    DiskGeometryFromBios,
    DiskGeometryFromPort,
    DiskGeometryFromNec98,
    DiskGeometryGuessedFromBios,
    DiskGeometryFromDefault,
    DiskGeometryFromNT4
} DISK_GEOMETRY_SOURCE, *PDISK_GEOMETRY_SOURCE;
#endif

//
// Context for requests that can be combined and sent down
//

typedef struct _DISK_GROUP_CONTEXT
{
    //
    // Queue of requests whose representative is currently outstanding at the port driver
    //
    LIST_ENTRY CurrList;

    //
    // The representative for the above queue
    //
    PIRP CurrIrp;

    //
    // Queue of requests whose representative is waiting to go down
    //
    LIST_ENTRY NextList;

    //
    // The representative for the above queue
    //
    PIRP NextIrp;

    //
    // The srb associated with this group
    //
    SCSI_REQUEST_BLOCK Srb;

    //
    // The mutex that will synchronize access to this context
    //
    KMUTEX Mutex;

    //
    // This event will allow for the requests to be sent down synchronously
    //
    KEVENT Event;

#if DBG

    //
    // This counter maintains the number of requests currently tagged
    // to the request that is waiting to go down
    //
    ULONG DbgTagCount;

    //
    // This counter maintains the number of requests that were avoided
    //
    ULONG DbgSavCount;

    //
    // This counter maintains the total number of times that we combined
    // requests and  the respective number of  requests that were tagged
    //
    ULONG DbgRefCount[64];

#endif

} DISK_GROUP_CONTEXT, *PDISK_GROUP_CONTEXT;

//
// Write cache setting as defined by the user
//
typedef enum _DISK_USER_WRITE_CACHE_SETTING
{
    DiskWriteCacheDisable =  0,
    DiskWriteCacheEnable  =  1,
    DiskWriteCacheDefault = -1

} DISK_USER_WRITE_CACHE_SETTING, *PDISK_USER_WRITE_CACHE_SETTING;

typedef struct _DISK_DATA {

    //
    // This field is the ordinal of a partition as it appears on a disk.
    //

    ULONG PartitionOrdinal;

    //
    // How has this disk been partitioned? Either EFI or MBR.
    //

    PARTITION_STYLE PartitionStyle;

    union {

        struct {

            //
            // Disk signature (from MBR)
            //

            ULONG Signature;

            //
            // MBR checksum
            //

            ULONG MbrCheckSum;

            //
            // Number of hidden sectors for BPB.
            //

            ULONG HiddenSectors;

            //
            // Partition type of this device object
            //
            // This field is set by:
            //
            //     1. Initially set according to the partition list entry
            //        partition type returned by IoReadPartitionTable.
            //
            //     2. Subsequently set by the
            //        IOCTL_DISK_SET_PARTITION_INFORMATION I/O control
            //        function when IoSetPartitionInformation function
            //        successfully updates the partition type on the disk.
            //

            UCHAR PartitionType;

            //
            // Boot indicator - indicates whether this partition is a
            // bootable (active) partition for this device
            //
            // This field is set according to the partition list entry boot
            // indicator returned by IoReadPartitionTable.
            //

            BOOLEAN BootIndicator;

        } Mbr;

        struct {

            //
            // The DiskGUID field from the EFI partition header.
            //

            GUID DiskId;

            //
            // Partition type of this device object.
            //

            GUID PartitionType;

            //
            // Unique partition identifier for this partition.
            //

            GUID PartitionId;

            //
            // EFI partition attributes for this partition.
            //

            ULONG64 Attributes;

            //
            // EFI partition name of this partition.
            //

            WCHAR PartitionName[36];

        } Efi;

#pragma warning(suppress: 4201) //this is intended to be an unnamed union
    };

    struct {
        //
        // This flag is set when the well known name is created (through
        // DiskCreateSymbolicLinks) and cleared when destroying it
        // (by calling DiskDeleteSymbolicLinks).
        //

        unsigned int WellKnownNameCreated : 1;

        //
        // This flag is set when the PhysicalDriveN link is created (through
        // DiskCreateSymbolicLinks) and is cleared when destroying it (through
        // DiskDeleteSymbolicLinks)
        //

        unsigned int PhysicalDriveLinkCreated : 1;

    } LinkStatus;

    //
    // ReadyStatus - STATUS_SUCCESS indicates that the drive is ready for
    // use.  Any error status is to be returned as an explaination for why
    // a request is failed.
    //
    // This was done solely for the zero-length partition case of having no
    // media in a removable disk drive.  When that occurs, and a read is sent
    // to the zero-length non-partition-zero PDO that was created, we had to
    // be able to fail the request with a reasonable value.  This may not have
    // been the best way to do this, but it works.
    //

    NTSTATUS ReadyStatus;

    //
    // SCSI address used for SMART operations.
    //

    SCSI_ADDRESS ScsiAddress;

    //
    // These unicode strings hold the disk and volume interface strings.  If
    // the interfaces were not registered or could not be set then the string
    // buffer will be NULL.
    //

    UNICODE_STRING DiskInterfaceString;

    //
    // What type of failure prediction mechanism is available
    //

    FAILURE_PREDICTION_METHOD FailurePredictionCapability;
    BOOLEAN AllowFPPerfHit;

#if defined(_X86_) || defined(_AMD64_)
    //
    // This flag indiciates that a non-default geometry for this drive has
    // already been determined by the disk driver.  This field is ignored
    // for removable media drives.
    //

    DISK_GEOMETRY_SOURCE GeometrySource;

    //
    // If GeometryDetermined is TRUE this will contain the geometry which was
    // reported by the firmware or by the BIOS.  For removable media drives
    // this will contain the last geometry used when media was present.
    //

    DISK_GEOMETRY RealGeometry;
#endif

    //
    // This mutex prevents more than one IOCTL_DISK_VERIFY from being
    // sent down to the disk. This greatly reduces the possibility of
    // a Denial-of-Service attack
    //

    KMUTEX VerifyMutex;

    //
    // This allows for parallel flush requests to be combined into one so as to
    // reduce the number of outstanding requests that are sent down to the disk
    //

    DISK_GROUP_CONTEXT FlushContext;

    //
    // The user-specified disk write cache setting
    //

    DISK_USER_WRITE_CACHE_SETTING WriteCacheOverride;

} DISK_DATA, *PDISK_DATA;

//
// Define a general structure of identfing disk controllers with bad
// hardware.
//

#define HackDisableTaggedQueuing            (0x01)
#define HackDisableSynchronousTransfers     (0x02)
#define HackDisableSpinDown                 (0x04)
#define HackDisableWriteCache               (0x08)
#define HackCauseNotReportableHack          (0x10)
#define HackRequiresStartUnitCommand        (0x20)


#define DiskDeviceParameterSubkey           L"Disk"
#define DiskDeviceUserWriteCacheSetting     L"UserWriteCacheSetting"
#define DiskDeviceCacheIsPowerProtected     L"CacheIsPowerProtected"


#define FUNCTIONAL_EXTENSION_SIZE sizeof(FUNCTIONAL_DEVICE_EXTENSION) + sizeof(DISK_DATA)

#define MODE_DATA_SIZE      192
#define VALUE_BUFFER_SIZE  2048
#define SCSI_DISK_TIMEOUT    10
#define PARTITION0_LIST_SIZE  4

#define MAX_MEDIA_TYPES 4
typedef struct _DISK_MEDIA_TYPES_LIST {
    PCCHAR VendorId;
    PCCHAR ProductId;
    PCCHAR Revision;
    const ULONG NumberOfTypes;
    const ULONG NumberOfSides;
    const STORAGE_MEDIA_TYPE MediaTypes[MAX_MEDIA_TYPES];
} DISK_MEDIA_TYPES_LIST, *PDISK_MEDIA_TYPES_LIST;

//
// WMI reregistration structures used for reregister work item
//
typedef struct
{
    SINGLE_LIST_ENTRY Next;
    PDEVICE_OBJECT DeviceObject;
    PIRP Irp;
} DISKREREGREQUEST, *PDISKREREGREQUEST;

#define MAX_SECTORS_PER_VERIFY              0x100

//
// This is based off 100ns units
//
#define ONE_MILLI_SECOND   ((ULONGLONG)10 * 1000)

//
// Context for the work-item
//
typedef struct _DISK_VERIFY_WORKITEM_CONTEXT
{
    PIRP Irp;
    PSCSI_REQUEST_BLOCK Srb;
    PIO_WORKITEM WorkItem;

} DISK_VERIFY_WORKITEM_CONTEXT, *PDISK_VERIFY_WORKITEM_CONTEXT;

//
// Poll for Failure Prediction every hour
//
#define DISK_DEFAULT_FAILURE_POLLING_PERIOD 1 * 60 * 60

#define CHECK_IRQL()                                    \
    if (KeGetCurrentIrql() >= DISPATCH_LEVEL) {         \
        ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);    \
        return STATUS_INVALID_LEVEL;                    \
    }

//
// Static global lookup tables.
//

extern CLASSPNP_SCAN_FOR_SPECIAL_INFO DiskBadControllers[];
extern const DISK_MEDIA_TYPES_LIST DiskMediaTypes[];
extern const DISK_MEDIA_TYPES_LIST DiskMediaTypesExclude[];

//
// Macros
//

//
// Routine prototypes.
//


DRIVER_INITIALIZE DriverEntry;

__control_entrypoint(DeviceDriver)
VOID
DiskUnload(
    IN PDRIVER_OBJECT DriverObject
    );

__control_entrypoint(DeviceDriver)
NTSTATUS
DiskAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT Pdo
    );

__control_entrypoint(DeviceDriver)
NTSTATUS
DiskInitFdo(
    IN PDEVICE_OBJECT Fdo
    );

__control_entrypoint(DeviceDriver)
NTSTATUS
DiskStartFdo(
    IN PDEVICE_OBJECT Fdo
    );

__control_entrypoint(DeviceDriver)
NTSTATUS
DiskStopDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR Type
    );

__control_entrypoint(DeviceDriver)
NTSTATUS
DiskRemoveDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR Type
    );

__control_entrypoint(DeviceDriver)
NTSTATUS
DiskReadWriteVerification(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

__control_entrypoint(DeviceDriver)
NTSTATUS
DiskDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

__control_entrypoint(DeviceDriver)
VOID
DiskFdoProcessError(
    PDEVICE_OBJECT DeviceObject,
    PSCSI_REQUEST_BLOCK Srb,
    NTSTATUS *Status,
    BOOLEAN *Retry
    );

__control_entrypoint(DeviceDriver)
NTSTATUS
DiskShutdownFlush(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskGetCacheInformation(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN PDISK_CACHE_INFORMATION CacheInfo
    );

NTSTATUS
DiskSetCacheInformation(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN PDISK_CACHE_INFORMATION CacheInfo
    );

VOID
DiskLogCacheInformation(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN PDISK_CACHE_INFORMATION CacheInfo,
    IN NTSTATUS Status
    );

NTSTATUS
DiskIoctlGetCacheSetting(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskIoctlSetCacheSetting(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

IO_WORKITEM_ROUTINE DisableWriteCache;

IO_WORKITEM_ROUTINE DiskIoctlVerifyThread;

VOID
DiskFlushDispatch(
    IN PDEVICE_OBJECT Fdo,
    IN PDISK_GROUP_CONTEXT FlushContext
    );

IO_COMPLETION_ROUTINE DiskFlushComplete;

NTSTATUS
DiskModeSelect(
    IN PDEVICE_OBJECT DeviceObject,
    __in_bcount(Length) PCHAR ModeSelectBuffer,
    IN ULONG Length,
    IN BOOLEAN SavePage
    );

//
// We need to validate that the self test subcommand is valid and
// appropriate. Right now we allow subcommands 0, 1 and 2 which are non
// captive mode tests. Once we figure out a way to know if it is safe to
// run a captive test then we can allow captive mode tests. Also if the
// atapi 5 spec is ever updated to denote that bit 7 is the captive
// mode bit, we can allow any request that does not have bit 7 set. Until
// that is done we want to be sure
//
#define DiskIsValidSmartSelfTest(Subcommand) \
    ( ((Subcommand) == SMART_OFFLINE_ROUTINE_OFFLINE) || \
      ((Subcommand) == SMART_SHORT_SELFTEST_OFFLINE) || \
      ((Subcommand) == SMART_EXTENDED_SELFTEST_OFFLINE) )


NTSTATUS
DiskPerformSmartCommand(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN ULONG SrbControlCode,
    IN UCHAR Command,
    IN UCHAR Feature,
    IN UCHAR SectorCount,
    IN UCHAR SectorNumber,
    IN OUT PSRB_IO_CONTROL SrbControl,
    OUT PULONG BufferSize
    );

NTSTATUS
DiskGetInfoExceptionInformation(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    OUT PMODE_INFO_EXCEPTIONS ReturnPageData
    );

NTSTATUS
DiskSetInfoExceptionInformation(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN PMODE_INFO_EXCEPTIONS PageData
    );

NTSTATUS
DiskDetectFailurePrediction(
    PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    PFAILURE_PREDICTION_METHOD FailurePredictCapability,
    BOOLEAN ScsiAddressAvailable
    );

NTSTATUS
DiskCreateFdo(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT LowerDeviceObject,
    IN PULONG DeviceCount,
    IN BOOLEAN DasdAccessOnly
    );

VOID
DiskSetSpecialHacks(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN ULONG_PTR Data
    );

VOID
ResetBus(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
DiskGenerateDeviceName(
    IN ULONG DeviceNumber,
    OUT PCCHAR *RawName
    );

VOID
DiskCreateSymbolicLinks(
    IN PDEVICE_OBJECT DeviceObject
    );

VOID
DiskDeleteSymbolicLinks(
    IN PDEVICE_OBJECT DeviceObject
    );

__control_entrypoint(DeviceDriver)
NTSTATUS
DiskFdoQueryWmiRegInfo(
    IN PDEVICE_OBJECT DeviceObject,
    OUT ULONG *RegFlags,
    OUT PUNICODE_STRING InstanceName
    );

__control_entrypoint(DeviceDriver)
NTSTATUS
DiskFdoQueryWmiRegInfoEx(
    IN PDEVICE_OBJECT DeviceObject,
    OUT ULONG *RegFlags,
    OUT PUNICODE_STRING InstanceName,
    OUT PUNICODE_STRING MofName
    );

__control_entrypoint(DeviceDriver)
NTSTATUS
DiskFdoQueryWmiDataBlock(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG BufferAvail,
    OUT PUCHAR Buffer
    );

__control_entrypoint(DeviceDriver)
NTSTATUS
DiskFdoSetWmiDataBlock(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG BufferSize,
    IN PUCHAR Buffer
    );

__control_entrypoint(DeviceDriver)
NTSTATUS
DiskFdoSetWmiDataItem(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG DataItemId,
    IN ULONG BufferSize,
    IN PUCHAR Buffer
    );

__control_entrypoint(DeviceDriver)
NTSTATUS
DiskFdoExecuteWmiMethod(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG MethodId,
    IN ULONG InBufferSize,
    IN ULONG OutBufferSize,
    IN PUCHAR Buffer
    );

__control_entrypoint(DeviceDriver)
NTSTATUS
DiskWmiFunctionControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN CLASSENABLEDISABLEFUNCTION Function,
    IN BOOLEAN Enable
    );

NTSTATUS
DiskReadFailurePredictStatus(
    PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    PSTORAGE_FAILURE_PREDICT_STATUS DiskSmartStatus
    );

NTSTATUS
DiskReadFailurePredictData(
    PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    PSTORAGE_FAILURE_PREDICT_DATA DiskSmartData
    );

NTSTATUS
DiskEnableDisableFailurePrediction(
    PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    BOOLEAN Enable
    );

NTSTATUS
DiskEnableDisableFailurePredictPolling(
    PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    BOOLEAN Enable,
    ULONG PollTimeInSeconds
    );

NTSTATUS DiskInitializeReregistration(
    VOID
    );

extern GUIDREGINFO DiskWmiFdoGuidList[];

#if defined(_X86_) || defined(_AMD64_)
NTSTATUS
DiskReadDriveCapacity(
    IN PDEVICE_OBJECT Fdo
    );
#else
#define DiskReadDriveCapacity(Fdo)  ClassReadDriveCapacity(Fdo)
#endif


#if defined(_X86_) || defined(_AMD64_)

NTSTATUS
DiskSaveDetectInfo(
    PDRIVER_OBJECT DriverObject
    );

VOID
DiskCleanupDetectInfo(
    IN PDRIVER_OBJECT DriverObject
    );

VOID
DiskDriverReinitialization (
    IN PDRIVER_OBJECT DriverObject,
    IN PVOID Nothing,
    IN ULONG Count
    );

#endif

#if defined(_X86_) || defined(_AMD64_)
NTSTATUS
DiskGetDetectInfo(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    OUT PDISK_DETECTION_INFO DetectInfo
    );

NTSTATUS
DiskReadSignature(
    IN PDEVICE_OBJECT Fdo
    );

BOOLEAN
DiskIsNT4Geometry(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension
    );

#else
#define DiskGetDetectInfo(FdoExtension, DetectInfo) (STATUS_UNSUCCESSFUL)
#endif


#define DiskHashGuid(Guid) (((PULONG) &Guid)[0] ^ ((PULONG) &Guid)[0] ^ ((PULONG) &Guid)[0] ^ ((PULONG) &Guid)[0])


NTSTATUS
DiskDetermineMediaTypes(
    IN PDEVICE_OBJECT Fdo,
    IN PIRP     Irp,
    IN UCHAR    MediumType,
    IN UCHAR    DensityCode,
    IN BOOLEAN  MediaPresent,
    IN BOOLEAN  IsWritable
    );

NTSTATUS
DiskIoctlGetLengthInfo(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskIoctlGetDriveGeometry(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

NTSTATUS
DiskIoctlGetDriveGeometryEx(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskIoctlGetCacheInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

NTSTATUS
DiskIoctlSetCacheInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

NTSTATUS
DiskIoctlGetMediaTypesEx(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

NTSTATUS
DiskIoctlPredictFailure(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

NTSTATUS
DiskIoctlVerify(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

NTSTATUS
DiskIoctlReassignBlocks(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

NTSTATUS
DiskIoctlReassignBlocksEx(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

NTSTATUS
DiskIoctlIsWritable(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

NTSTATUS
DiskIoctlSetVerify(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

NTSTATUS
DiskIoctlClearVerify(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

NTSTATUS
DiskIoctlUpdateDriveSize(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

NTSTATUS
DiskIoctlGetVolumeDiskExtents(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

NTSTATUS
DiskIoctlSmartGetVersion(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

NTSTATUS
DiskIoctlSmartReceiveDriveData(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

NTSTATUS
DiskIoctlSmartSendDriveCommand(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );




