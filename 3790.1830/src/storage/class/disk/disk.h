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
#if defined(JAPAN) && defined(_X86_)
#include "machine.h"
#endif

#include <wmistr.h>

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

typedef
VOID
(*PDISK_UPDATE_PARTITIONS) (
    IN PDEVICE_OBJECT Fdo,
    IN OUT PDRIVE_LAYOUT_INFORMATION_EX PartitionList
    );

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

    };  // unnamed union

    struct {
        //
        // This flag is set when the well known name is created (through
        // DiskCreateSymbolicLinks) and cleared when destroying it
        // (by calling DiskDeleteSymbolicLinks).
        //

        BOOLEAN WellKnownNameCreated : 1;

        //
        // This flag is set when the PhysicalDriveN link is created (through
        // DiskCreateSymbolicLinks) and is cleared when destroying it (through
        // DiskDeleteSymbolicLinks)
        //

        BOOLEAN PhysicalDriveLinkCreated : 1;

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
    // Routine to be called when updating the disk partitions.  This routine
    // is different for removable and non-removable media and is called by
    // (among other things) DiskEnumerateDevice
    //

    PDISK_UPDATE_PARTITIONS UpdatePartitionRoutine;

    //
    // SCSI address used for SMART operations.
    //

    SCSI_ADDRESS ScsiAddress;

    //
    // Event used to synchronize partitioning operations and enumerations.
    //

    KEVENT PartitioningEvent;

    //
    // These unicode strings hold the disk and volume interface strings.  If
    // the interfaces were not registered or could not be set then the string
    // buffer will be NULL.
    //

    UNICODE_STRING DiskInterfaceString;
    UNICODE_STRING PartitionInterfaceString;

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
    // Indicates that the cached partition table is valid when set.
    //

    ULONG CachedPartitionTableValid;

    //
    // The cached partition table - this is only valid if the previous
    // flag is set.  When invalidated the cached partition table will be
    // freed and replaced the next time one of the partitioning functions is
    // called.  This allows the error handling routines to invalidate it by
    // setting the flag and doesn't require that they obtain a lock.
    //

    PDRIVE_LAYOUT_INFORMATION_EX CachedPartitionTable;

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
#define PHYSICAL_EXTENSION_SIZE sizeof(PHYSICAL_DEVICE_EXTENSION) + sizeof(DISK_DATA)

#define MODE_DATA_SIZE      192
#define VALUE_BUFFER_SIZE  2048
#define SCSI_DISK_TIMEOUT    10
#define PARTITION0_LIST_SIZE  4

#define MAX_MEDIA_TYPES 4
typedef struct _DISK_MEDIA_TYPES_LIST {
    PCHAR VendorId;
    PCHAR ProductId;
    PCHAR Revision;
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

#define MAX_SECTORS_PER_VERIFY              0x200

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

//
// Static global lookup tables.
//

extern CLASSPNP_SCAN_FOR_SPECIAL_INFO DiskBadControllers[];
extern const DISK_MEDIA_TYPES_LIST DiskMediaTypes[];

//
// Macros
//

//
// Routine prototypes.
//


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    );

VOID
DiskUnload(
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
DiskAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT Pdo
    );

NTSTATUS
DiskInitFdo(
    IN PDEVICE_OBJECT Fdo
    );

NTSTATUS
DiskInitPdo(
    IN PDEVICE_OBJECT Pdo
    );

NTSTATUS
DiskStartFdo(
    IN PDEVICE_OBJECT Fdo
    );

NTSTATUS
DiskStartPdo(
    IN PDEVICE_OBJECT Pdo
    );

NTSTATUS
DiskStopDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR Type
    );

NTSTATUS
DiskRemoveDevice(
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR Type
    );

NTSTATUS
DiskReadWriteVerification(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

VOID
DiskFdoProcessError(
    PDEVICE_OBJECT DeviceObject,
    PSCSI_REQUEST_BLOCK Srb,
    NTSTATUS *Status,
    BOOLEAN *Retry
    );

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
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN PIRP Irp
    );

NTSTATUS
DiskIoctlSetCacheSetting(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN PIRP Irp
    );

VOID
DisableWriteCache(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIO_WORKITEM WorkItem
    );

VOID
DiskIoctlVerify(
    IN PDEVICE_OBJECT DeviceObject,
    IN PDISK_VERIFY_WORKITEM_CONTEXT Context
    );

VOID
DiskFlushDispatch(
    IN PDEVICE_OBJECT Fdo,
    IN PDISK_GROUP_CONTEXT FlushContext
    );

NTSTATUS
DiskFlushComplete(
    IN PDEVICE_OBJECT Fdo,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
DiskModeSelect(
    IN PDEVICE_OBJECT DeviceObject,
    IN PCHAR ModeSelectBuffer,
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
    PFAILURE_PREDICTION_METHOD FailurePredictCapability
    );

BOOLEAN
EnumerateBusKey(
    IN PFUNCTIONAL_DEVICE_EXTENSION DeviceExtension,
    HANDLE BusKey,
    PULONG DiskNumber
    );

NTSTATUS
DiskCreateFdo(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT LowerDeviceObject,
    IN PULONG DeviceCount,
    IN BOOLEAN DasdAccessOnly
    );

VOID
UpdateDeviceObjects(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
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
DiskEnumerateDevice(
    IN PDEVICE_OBJECT Fdo
    );

NTSTATUS
DiskQueryId(
    IN PDEVICE_OBJECT Pdo,
    IN BUS_QUERY_ID_TYPE IdType,
    IN PUNICODE_STRING UnicodeIdString
    );

NTSTATUS
DiskQueryPnpCapabilities(
    IN PDEVICE_OBJECT DeviceObject,
    IN PDEVICE_CAPABILITIES Capabilities
    );

NTSTATUS
DiskGenerateDeviceName(
    IN BOOLEAN IsFdo,
    IN ULONG DeviceNumber,
    IN OPTIONAL ULONG PartitionNumber,
    IN OPTIONAL PLARGE_INTEGER StartingOffset,
    IN OPTIONAL PLARGE_INTEGER PartitionLength,
    OUT PUCHAR *RawName
    );

VOID
DiskCreateSymbolicLinks(
    IN PDEVICE_OBJECT DeviceObject
    );

VOID
DiskUpdatePartitions(
    IN PDEVICE_OBJECT Fdo,
    IN OUT PDRIVE_LAYOUT_INFORMATION_EX PartitionList
    );

VOID
DiskUpdateRemovablePartitions(
    IN PDEVICE_OBJECT Fdo,
    IN OUT PDRIVE_LAYOUT_INFORMATION_EX PartitionList
    );

NTSTATUS
DiskCreatePdo(
    IN PDEVICE_OBJECT Fdo,
    IN ULONG PartitionOrdinal,
    IN PPARTITION_INFORMATION_EX PartitionEntry,
    IN PARTITION_STYLE PartitionStyle,
    OUT PDEVICE_OBJECT *Pdo
    );

VOID
DiskDeleteSymbolicLinks(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
DiskPdoQueryWmiRegInfo(
    IN PDEVICE_OBJECT DeviceObject,
    OUT ULONG *RegFlags,
    OUT PUNICODE_STRING InstanceName
    );

NTSTATUS
DiskPdoQueryWmiDataBlock(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG BufferAvail,
    OUT PUCHAR Buffer
    );

NTSTATUS
DiskPdoSetWmiDataBlock(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG BufferSize,
    IN PUCHAR Buffer
    );

NTSTATUS
DiskPdoSetWmiDataItem(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG DataItemId,
    IN ULONG BufferSize,
    IN PUCHAR Buffer
    );

NTSTATUS
DiskPdoExecuteWmiMethod(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG MethodId,
    IN ULONG InBufferSize,
    IN ULONG OutBufferSize,
    IN PUCHAR Buffer
    );

NTSTATUS
DiskFdoQueryWmiRegInfo(
    IN PDEVICE_OBJECT DeviceObject,
    OUT ULONG *RegFlags,
    OUT PUNICODE_STRING InstanceName
    );

NTSTATUS
DiskFdoQueryWmiRegInfoEx(
    IN PDEVICE_OBJECT DeviceObject,
    OUT ULONG *RegFlags,
    OUT PUNICODE_STRING InstanceName,
    OUT PUNICODE_STRING MofName
    );

NTSTATUS
DiskFdoQueryWmiDataBlock(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG BufferAvail,
    OUT PUCHAR Buffer
    );

NTSTATUS
DiskFdoSetWmiDataBlock(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG BufferSize,
    IN PUCHAR Buffer
    );

NTSTATUS
DiskFdoSetWmiDataItem(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG DataItemId,
    IN ULONG BufferSize,
    IN PUCHAR Buffer
    );

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

VOID
DiskAcquirePartitioningLock(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension
    );

VOID
DiskReleasePartitioningLock(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension
    );

NTSTATUS DiskInitializeReregistration(
    VOID
    );

extern GUIDREGINFO DiskWmiFdoGuidList[];
extern GUIDREGINFO DiskWmiPdoGuidList[];

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

VOID
DiskConvertPartitionToExtended(
    IN PPARTITION_INFORMATION Partition,
    OUT PPARTITION_INFORMATION_EX PartitionEx
    );

PDRIVE_LAYOUT_INFORMATION_EX
DiskConvertLayoutToExtended(
    IN CONST PDRIVE_LAYOUT_INFORMATION Layout
    );

PDRIVE_LAYOUT_INFORMATION
DiskConvertExtendedToLayout(
    IN CONST PDRIVE_LAYOUT_INFORMATION_EX LayoutEx
    );

NTSTATUS
DiskReadPartitionTableEx(
    IN PFUNCTIONAL_DEVICE_EXTENSION Fdo,
    IN BOOLEAN BypassCache,
    OUT PDRIVE_LAYOUT_INFORMATION_EX* DriveLayout
    );

NTSTATUS
DiskWritePartitionTableEx(
    IN PFUNCTIONAL_DEVICE_EXTENSION Fdo,
    IN PDRIVE_LAYOUT_INFORMATION_EX DriveLayout
    );

NTSTATUS
DiskSetPartitionInformationEx(
    IN PFUNCTIONAL_DEVICE_EXTENSION Fdo,
    IN ULONG PartitionNumber,
    IN struct _SET_PARTITION_INFORMATION_EX* PartitionInfo
    );

NTSTATUS
DiskSetPartitionInformation(
    IN PFUNCTIONAL_DEVICE_EXTENSION Fdo,
    IN ULONG SectorSize,
    IN ULONG PartitionNumber,
    IN ULONG PartitionType
    );

NTSTATUS
DiskVerifyPartitionTable(
    IN PFUNCTIONAL_DEVICE_EXTENSION Fdo,
    IN BOOLEAN FixErrors
    );

BOOLEAN
DiskInvalidatePartitionTable(
    IN PFUNCTIONAL_DEVICE_EXTENSION Fdo,
    IN BOOLEAN PartitionLockHeld
    );

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

