/*++

Copyright (C) Microsoft Corporation, 1991 - 1999

Module Name:

    classp.h

Abstract:

    Private header file for classpnp.sys modules.  This contains private
    structure and function declarations as well as constant values which do
    not need to be exported.

Author:

Environment:

    kernel mode only

Notes:


Revision History:

--*/


#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ntddk.h>

#include <scsi.h>
#include <wmidata.h>
#include <classpnp.h>

#if CLASS_INIT_GUID
#include <initguid.h>
#endif

#include <mountdev.h>
#include <ioevent.h>


/*
 *  IA64 requires 8-byte alignment for pointers, but the IA64 NT kernel expects 16-byte alignment
 */
#ifdef _WIN64
    #define PTRALIGN                DECLSPEC_ALIGN(16)
#else
    #define PTRALIGN
#endif


extern CLASSPNP_SCAN_FOR_SPECIAL_INFO ClassBadItems[];

extern GUID ClassGuidQueryRegInfoEx;

extern ULONG ClassMaxInterleavePerCriticalIo;

#define CLASSP_REG_SUBKEY_NAME                  (L"Classpnp")

#define CLASSP_REG_HACK_VALUE_NAME              (L"HackMask")
#define CLASSP_REG_MMC_DETECTION_VALUE_NAME     (L"MMCDetectionState")
#define CLASSP_REG_WRITE_CACHE_VALUE_NAME       (L"WriteCacheEnableOverride")
#define CLASSP_REG_PERF_RESTORE_VALUE_NAME      (L"RestorePerfAtCount")
#define CLASSP_REG_REMOVAL_POLICY_VALUE_NAME    (L"UserRemovalPolicy")

#define CLASS_PERF_RESTORE_MINIMUM              (0x10)
#define CLASS_ERROR_LEVEL_1                     (0x4)
#define CLASS_ERROR_LEVEL_2                     (0x8)
#define CLASS_MAX_INTERLEAVE_PER_CRITICAL_IO    (0x4)

#define FDO_HACK_CANNOT_LOCK_MEDIA              (0x00000001)
#define FDO_HACK_GESN_IS_BAD                    (0x00000002)
#define FDO_HACK_NO_SYNC_CACHE                  (0x00000004)
#define FDO_HACK_NO_RESERVE6                    (0x00000008)

#define FDO_HACK_VALID_FLAGS                    (0x0000000F)
#define FDO_HACK_INVALID_FLAGS                  (~FDO_HACK_VALID_FLAGS)

/*
 *  Lots of retries of synchronized SCSI commands that devices may not
 *  even support really slows down the system (especially while booting).
 *  (Even GetDriveCapacity may be failed on purpose if an external disk is powered off).
 *  If a disk cannot return a small initialization buffer at startup
 *  in two attempts (with delay interval) then we cannot expect it to return
 *  data consistently with four retries.
 *  So don't set the retry counts as high here as for data SRBs.
 *
 *  If we find that these requests are failing consecutively,
 *  despite the retry interval, on otherwise reliable media,
 *  then we should either increase the retry interval for
 *  that failure or (by all means) increase these retry counts as appropriate.
 */
#define NUM_LOCKMEDIAREMOVAL_RETRIES    1
#define NUM_MODESENSE_RETRIES           1
#define NUM_DRIVECAPACITY_RETRIES       1

/*
 *  We retry failed I/O requests at 1-second intervals.
 *  In the case of a failure due to bus reset, we want to make sure that we retry after the allowable
 *  reset time.  For SCSI, the allowable reset time is 5 seconds.  ScsiPort queues requests during
 *  a bus reset, which should cause us to retry after the reset is over; but the requests queued in
 *  the miniport are failed all the way back to us immediately.  In any event, in order to make
 *  extra sure that our retries span the allowable reset time, we should retry more than 5 times.
 */
#define NUM_IO_RETRIES      8

#define CLASS_FILE_OBJECT_EXTENSION_KEY     'eteP'
#define CLASSP_VOLUME_VERIFY_CHECKED        0x34

#define CLASS_TAG_PRIVATE_DATA              'CPcS'
#define CLASS_TAG_PRIVATE_DATA_FDO          'FPcS'
#define CLASS_TAG_PRIVATE_DATA_PDO          'PPcS'

//
// Definitions from ntos\rtl\time.c
//

extern CONST LARGE_INTEGER Magic10000;
#define SHIFT10000               13

#define Convert100nsToMilliseconds(LARGE_INTEGER)                       \
    (                                                                   \
    RtlExtendedMagicDivide((LARGE_INTEGER), Magic10000, SHIFT10000)     \
    )

typedef struct _MEDIA_CHANGE_DETECTION_INFO {

    //
    // Mutex to synchronize enable/disable requests and media state changes
    //

    KMUTEX MediaChangeMutex;

    //
    // The current state of the media (present, not present, unknown)
    // protected by MediaChangeSynchronizationEvent
    //

    MEDIA_CHANGE_DETECTION_STATE MediaChangeDetectionState;

    //
    // This is a count of how many time MCD has been disabled.  if it is
    // set to zero, then we'll poll the device for MCN events with the
    // then-current method (ie. TEST UNIT READY or GESN).  this is
    // protected by MediaChangeMutex
    //

    LONG MediaChangeDetectionDisableCount;


    //
    // The timer value to support media change events.  This is a countdown
    // value used to determine when to poll the device for a media change.
    // The max value for the timer is 255 seconds.  This is not protected
    // by an event -- simply InterlockedExchanged() as needed.
    //

    LONG MediaChangeCountDown;

    //
    // recent changes allowed instant retries of the MCN irp.  Since this
    // could cause an infinite loop, keep a count of how many times we've
    // retried immediately so that we can catch if the count exceeds an
    // arbitrary limit.
    //

    LONG MediaChangeRetryCount;

    //
    // use GESN if it's available
    //

    struct {
        BOOLEAN Supported;
        BOOLEAN HackEventMask;
        UCHAR   EventMask;
        UCHAR   NoChangeEventMask;
        PUCHAR  Buffer;
        PMDL    Mdl;
        ULONG   BufferSize;
    } Gesn;

    //
    // If this value is one, then the irp is currently in use.
    // If this value is zero, then the irp is available.
    // Use InterlockedCompareExchange() to set from "available" to "in use".
    // ASSERT that InterlockedCompareExchange() showed previous value of
    //    "in use" when changing back to "available" state.
    // This also implicitly protects the MediaChangeSrb and SenseBuffer
    //

    LONG MediaChangeIrpInUse;

    //
    // Pointer to the irp to be used for media change detection.
    // protected by Interlocked MediaChangeIrpInUse
    //

    PIRP MediaChangeIrp;

    //
    // The srb for the media change detection.
    // protected by Interlocked MediaChangeIrpInUse
    //

    SCSI_REQUEST_BLOCK MediaChangeSrb;
    PUCHAR SenseBuffer;
    ULONG SrbFlags;

    //
    // Second timer to keep track of how long the media change IRP has been
    // in use.  If this value exceeds the timeout (#defined) then we should
    // print out a message to the user and set the MediaChangeIrpLost flag
    // protected by using Interlocked() operations in ClasspSendMediaStateIrp,
    // the only routine which should modify this value.
    //

    LONG MediaChangeIrpTimeInUse;

    //
    // Set by CdRomTickHandler when we determine that the media change irp has
    // been lost
    //

    BOOLEAN MediaChangeIrpLost;

};

typedef enum {
    SimpleMediaLock,
    SecureMediaLock,
    InternalMediaLock
} MEDIA_LOCK_TYPE, *PMEDIA_LOCK_TYPE;

typedef struct _FAILURE_PREDICTION_INFO {
    FAILURE_PREDICTION_METHOD Method;
    ULONG CountDown;                // Countdown timer
    ULONG Period;                   // Countdown period

    PIO_WORKITEM WorkQueueItem;

    KEVENT Event;
} FAILURE_PREDICTION_INFO, *PFAILURE_PREDICTION_INFO;



//
// This struct must always fit within four PVOIDs of info,
// as it uses the irp's "PVOID DriverContext[4]" to store
// this info
//
typedef struct _CLASS_RETRY_INFO {
    struct _CLASS_RETRY_INFO *Next;
} CLASS_RETRY_INFO, *PCLASS_RETRY_INFO;



typedef struct _CSCAN_LIST {

    //
    // The current block which has an outstanding request.
    //

    ULONGLONG BlockNumber;

    //
    // The list of blocks past the CurrentBlock to which we're going to do
    // i/o.  This list is maintained in sorted order.
    //

    LIST_ENTRY CurrentSweep;

    //
    // The list of blocks behind the current block for which we'll have to
    // wait until the next scan across the disk.  This is kept as a stack,
    // the cost of sorting it is taken when it's moved over to be the
    // running list.
    //

    LIST_ENTRY NextSweep;

} CSCAN_LIST, *PCSCAN_LIST;

//
// add to the front of this structure to help prevent illegal
// snooping by other utilities.
//



typedef enum _CLASS_DETECTION_STATE {
    ClassDetectionUnknown = 0,
    ClassDetectionUnsupported = 1,
    ClassDetectionSupported = 2
} CLASS_DETECTION_STATE, *PCLASS_DETECTION_STATE;


typedef struct _CLASS_ERROR_LOG_DATA {
    LARGE_INTEGER TickCount;        // Offset 0x00
    ULONG PortNumber;               // Offset 0x08

    UCHAR ErrorPaging    : 1;       // Offset 0x0c
    UCHAR ErrorRetried   : 1;
    UCHAR ErrorUnhandled : 1;
    UCHAR ErrorReserved  : 5;

    UCHAR Reserved[3];

    SCSI_REQUEST_BLOCK Srb;     // Offset 0x10

    /*
     *  We define the SenseData as the default length.
     *  Since the sense data returned by the port driver may be longer,
     *  SenseData must be at the end of this structure.
     *  For our internal error log, we only log the default length.
     */
    SENSE_DATA SenseData;     // Offset 0x50 for x86 (or 0x68 for ia64) (ULONG32 Alignment required!)
} CLASS_ERROR_LOG_DATA, *PCLASS_ERROR_LOG_DATA;

#define NUM_ERROR_LOG_ENTRIES   16
#define DBG_NUM_PACKET_LOG_ENTRIES (64*2)   // 64 send&receive's

typedef struct _TRANSFER_PACKET {

        LIST_ENTRY AllPktsListEntry;    // entry in fdoData's static AllTransferPacketsList
        SLIST_ENTRY SlistEntry;         // for when in free list (use fast slist)

        PIRP Irp;
        PDEVICE_OBJECT Fdo;

        /*
         *  This is the client IRP that this TRANSFER_PACKET is currently
         *  servicing.
         */
        PIRP OriginalIrp;
        BOOLEAN CompleteOriginalIrpWhenLastPacketCompletes;

        /*
         *  Stuff for retrying the transfer.
         */
        ULONG NumRetries;
        KTIMER RetryTimer;
        KDPC RetryTimerDPC;
        ULONG RetryIntervalSec;

        /*
         *  Event for synchronizing the transfer (optional).
         *  (Note that we can't have the event in the packet itself because
         *  by the time a thread waits on an event the packet may have
         *  been completed and re-issued.
         */
        PKEVENT SyncEventPtr;

        /*
         *  Stuff for retrying during extreme low-memory stress
         *  (when we retry 1 page at a time).
         */
        BOOLEAN InLowMemRetry;
        PUCHAR LowMemRetry_remainingBufPtr;
        ULONG LowMemRetry_remainingBufLen;
        LARGE_INTEGER LowMemRetry_nextChunkTargetLocation;

        /*
         *  Fields used for cancelling the packet.
         */
        // BOOLEAN Cancelled;
        // KEVENT CancelledEvent;

        /*
         *  We keep the buffer and length values here as well
         *  as in the SRB because some miniports return
         *  the transferred length in SRB.DataTransferLength,
         *  and if the SRB failed we need that value again for the retry.
         *  We don't trust the lower stack to preserve any of these values in the SRB.
         */
        PUCHAR BufPtrCopy;
        ULONG BufLenCopy;
        LARGE_INTEGER TargetLocationCopy;

        /*
         *  This is a standard SCSI structure that receives a detailed
         *  report about a SCSI error on the hardware.
         */
        SENSE_DATA SrbErrorSenseData;

        /*
         *  This is the SRB block for this TRANSFER_PACKET.
         *  For IOCTLs, the SRB block includes two DWORDs for
         *  device object and ioctl code; so these must
         *  immediately follow the SRB block.
         */
        SCSI_REQUEST_BLOCK Srb;
        // ULONG SrbIoctlDevObj;        // not handling ioctls yet
        // ULONG SrbIoctlCode;

        #if DBG
            LARGE_INTEGER DbgTimeSent;
            LARGE_INTEGER DbgTimeReturned;
            ULONG DbgPktId;
            IRP DbgOriginalIrpCopy;
            MDL DbgMdlCopy;
        #endif

        BOOLEAN UsePartialMdl;
        PMDL PartialMdl;

} TRANSFER_PACKET, *PTRANSFER_PACKET;

/*
 *  MIN_INITIAL_TRANSFER_PACKETS is the minimum number of packets that
 *  we preallocate at startup for each device (we need at least one packet
 *  to guarantee forward progress during memory stress).
 *  MIN_WORKINGSET_TRANSFER_PACKETS is the number of TRANSFER_PACKETs
 *  we allow to build up and remain for each device;
 *  we _lazily_ work down to this number when they're not needed.
 *  MAX_WORKINGSET_TRANSFER_PACKETS is the number of TRANSFER_PACKETs
 *  that we _immediately_ reduce to when they are not needed.
 *
 *  The absolute maximum number of packets that we will allocate is
 *  whatever is required by the current activity, up to the memory limit;
 *  as soon as stress ends, we snap down to MAX_WORKINGSET_TRANSFER_PACKETS;
 *  we then lazily work down to MIN_WORKINGSET_TRANSFER_PACKETS.
 */
#define MIN_INITIAL_TRANSFER_PACKETS                     1
#define MIN_WORKINGSET_TRANSFER_PACKETS_Consumer      4
#define MAX_WORKINGSET_TRANSFER_PACKETS_Consumer     64
#define MIN_WORKINGSET_TRANSFER_PACKETS_Server        64
#define MAX_WORKINGSET_TRANSFER_PACKETS_Server      1024
#define MIN_WORKINGSET_TRANSFER_PACKETS_Enterprise    256
#define MAX_WORKINGSET_TRANSFER_PACKETS_Enterprise   2048


//
// add to the front of this structure to help prevent illegal
// snooping by other utilities.
//
struct _CLASS_PRIVATE_FDO_DATA {

    /*
     *  Entry in static list used by debug extension to quickly find all class FDOs.
     */
    LIST_ENTRY AllFdosListEntry;

    //
    // this private structure allows us to
    // dynamically re-enable the perf benefits
    // lost due to transient error conditions.
    // in w2k, a reboot was required. :(
    //
    struct {
        ULONG      OriginalSrbFlags;
        ULONG      SuccessfulIO;
        ULONG      ReEnableThreshhold; // 0 means never
    } Perf;

    ULONG_PTR HackFlags;

    STORAGE_HOTPLUG_INFO HotplugInfo;

    // Legacy.  Still used by obsolete legacy code.
    struct {
        LARGE_INTEGER     Delta;       // in ticks
        LARGE_INTEGER     Tick;        // when it should fire
        PCLASS_RETRY_INFO ListHead;    // singly-linked list
        ULONG             Granularity; // static
        KSPIN_LOCK        Lock;        // protective spin lock
        KDPC              Dpc;         // DPC routine object
        KTIMER            Timer;       // timer to fire DPC
    } Retry;

    BOOLEAN TimerStarted;
    BOOLEAN LoggedTURFailureSinceLastIO;
    BOOLEAN LoggedSYNCFailure;

    //
    // privately allocated release queue irp
    // protected by fdoExtension->ReleaseQueueSpinLock
    //
    BOOLEAN ReleaseQueueIrpAllocated;
    PIRP ReleaseQueueIrp;

    /*
     *  Queues for TRANSFER_PACKETs that contextualize the IRPs and SRBs
     *  that we send down to the port driver.
     *  (The free list is an slist so that we can use fast
     *   interlocked operations on it; but the relatively-static
     *   AllTransferPacketsList list has to be
     *   a doubly-linked list since we have to dequeue from the middle).
     */
    LIST_ENTRY AllTransferPacketsList;
    SLIST_HEADER FreeTransferPacketsList;
    ULONG NumFreeTransferPackets;
    ULONG NumTotalTransferPackets;
    ULONG DbgPeakNumTransferPackets;

    /*
     *  Queue for deferred client irps
     */
    LIST_ENTRY DeferredClientIrpList;

    /*
     *  Precomputed maximum transfer length for the hardware.
     */
    ULONG HwMaxXferLen;

    /*
     *  SCSI_REQUEST_BLOCK template preconfigured with the constant values.
     *  This is slapped into the SRB in the TRANSFER_PACKET for each transfer.
     */
    SCSI_REQUEST_BLOCK SrbTemplate;

    KSPIN_LOCK SpinLock;

    /*
     *  For non-removable media, we read the drive capacity at start time and cache it.
     *  This is so that ReadDriveCapacity failures at runtime (e.g. due to memory stress)
     *  don't cause I/O on the paging disk to start failing.
     */
    READ_CAPACITY_DATA_EX LastKnownDriveCapacityData;
    BOOLEAN IsCachedDriveCapDataValid;

    /*
     *  Circular array of timestamped logs of errors that occurred on this device.
     */
    ULONG ErrorLogNextIndex;
    CLASS_ERROR_LOG_DATA ErrorLogs[NUM_ERROR_LOG_ENTRIES];

    //
    // Number of outstanding critical Io requests from Mm
    //
    ULONG NumHighPriorityPagingIo;

    //
    // Maximum number of normal Io requests that can be interleaved with the critical ones
    //
    ULONG MaxInterleavedNormalIo;

    //
    // The timestamp when entering throttle mode
    //
    LARGE_INTEGER ThrottleStartTime;

    //
    // The timestamp when exiting throttle mode
    //
    LARGE_INTEGER ThrottleStopTime;

    //
    // The longest time ever spent in throttle mode
    //
    LARGE_INTEGER LongestThrottlePeriod;

    #if DBG
        ULONG DbgMaxPktId;

        /*
         *  Logging fields for ForceUnitAccess and Flush
         */
        BOOLEAN DbgInitFlushLogging;         // must reset this to 1 for each logging session
        ULONG DbgNumIORequests;
        ULONG DbgNumFUAs;       // num I/O requests with ForceUnitAccess bit set
        ULONG DbgNumFlushes;    // num SRB_FUNCTION_FLUSH_QUEUE
        ULONG DbgIOsSinceFUA;
        ULONG DbgIOsSinceFlush;
        ULONG DbgAveIOsToFUA;      // average number of I/O requests between FUAs
        ULONG DbgAveIOsToFlush;   // ...
        ULONG DbgMaxIOsToFUA;
        ULONG DbgMaxIOsToFlush;
        ULONG DbgMinIOsToFUA;
        ULONG DbgMinIOsToFlush;

        /*
         *  Debug log of previously sent packets (including retries).
         */
        ULONG DbgPacketLogNextIndex;
        TRANSFER_PACKET DbgPacketLogs[DBG_NUM_PACKET_LOG_ENTRIES];
    #endif

};


#define MIN(a, b)   ((a) < (b) ? (a) : (b))
#define MAX(a, b)   ((a) > (b) ? (a) : (b))


#define NOT_READY_RETRY_INTERVAL    10
#define MINIMUM_RETRY_UNITS ((LONGLONG)32)


/*
 *  Simple singly-linked-list queuing macros, with no synchronization.
 */
__inline VOID SimpleInitSlistHdr(SINGLE_LIST_ENTRY *SListHdr)
{
    SListHdr->Next = NULL;
}
__inline VOID SimplePushSlist(SINGLE_LIST_ENTRY *SListHdr, SINGLE_LIST_ENTRY *SListEntry)
{
    SListEntry->Next = SListHdr->Next;
    SListHdr->Next = SListEntry;
}
__inline SINGLE_LIST_ENTRY *SimplePopSlist(SINGLE_LIST_ENTRY *SListHdr)
{
    SINGLE_LIST_ENTRY *sListEntry = SListHdr->Next;
    if (sListEntry){
        SListHdr->Next = sListEntry->Next;
        sListEntry->Next = NULL;
    }
    return sListEntry;
}
__inline BOOLEAN SimpleIsSlistEmpty(SINGLE_LIST_ENTRY *SListHdr)
{
    return (SListHdr->Next == NULL);
}

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    );

VOID
ClassUnload(
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
ClassCreateClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
ClasspCreateClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

VOID
ClasspCleanupProtectedLocks(
    IN PFILE_OBJECT_EXTENSION FsContext
    );

NTSTATUS
ClasspEjectionControl(
    IN PDEVICE_OBJECT Fdo,
    IN PIRP Irp,
    IN MEDIA_LOCK_TYPE LockType,
    IN BOOLEAN Lock
    );

NTSTATUS
ClassReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
ClassDeviceControlDispatch(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    );

NTSTATUS
ClassDeviceControl(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    );

NTSTATUS
ClassDispatchPnp(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    );

NTSTATUS
ClassPnpStartDevice(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
ClassInternalIoControl (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
ClassShutdownFlush(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
ClassSystemControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

//
// Class internal routines
//

NTSTATUS
ClassAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN OUT PDEVICE_OBJECT PhysicalDeviceObject
    );

NTSTATUS
ClasspSendSynchronousCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

VOID
RetryRequest(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp,
    PSCSI_REQUEST_BLOCK Srb,
    BOOLEAN Associated,
    ULONG RetryInterval
    );

NTSTATUS
ClassIoCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
ClassPnpQueryFdoRelations(
    IN PDEVICE_OBJECT Fdo,
    IN PIRP Irp
    );

NTSTATUS
ClassRetrieveDeviceRelations(
    IN PDEVICE_OBJECT Fdo,
    IN DEVICE_RELATION_TYPE RelationType,
    OUT PDEVICE_RELATIONS *DeviceRelations
    );

NTSTATUS
ClassGetPdoId(
    IN PDEVICE_OBJECT Pdo,
    IN BUS_QUERY_ID_TYPE IdType,
    IN PUNICODE_STRING IdString
    );

NTSTATUS
ClassQueryPnpCapabilities(
    IN PDEVICE_OBJECT PhysicalDeviceObject,
    IN PDEVICE_CAPABILITIES Capabilities
    );

VOID
ClasspStartIo(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
ClasspPagingNotificationCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PDEVICE_OBJECT RealDeviceObject
    );

NTSTATUS
ClasspMediaChangeCompletion(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp,
    PVOID Context
    );

PFILE_OBJECT_EXTENSION
ClasspGetFsContext(
    IN PCOMMON_DEVICE_EXTENSION CommonExtension,
    IN PFILE_OBJECT FileObject
    );

NTSTATUS
ClasspMcnControl(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN PIRP Irp,
    IN PSCSI_REQUEST_BLOCK Srb
    );

VOID
ClasspRegisterMountedDeviceInterface(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
ClasspDisableTimer(
    PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
ClasspEnableTimer(
    PDEVICE_OBJECT DeviceObject
    );

//
// routines for dictionary list support
//

VOID
InitializeDictionary(
    IN PDICTIONARY Dictionary
    );

BOOLEAN
TestDictionarySignature(
    IN PDICTIONARY Dictionary
    );

NTSTATUS
AllocateDictionaryEntry(
    IN PDICTIONARY Dictionary,
    IN ULONGLONG Key,
    IN ULONG Size,
    IN ULONG Tag,
    OUT PVOID *Entry
    );

PVOID
GetDictionaryEntry(
    IN PDICTIONARY Dictionary,
    IN ULONGLONG Key
    );

VOID
FreeDictionaryEntry(
    IN PDICTIONARY Dictionary,
    IN PVOID Entry
    );


NTSTATUS
ClasspAllocateReleaseRequest(
    IN PDEVICE_OBJECT Fdo
    );

VOID
ClasspFreeReleaseRequest(
    IN PDEVICE_OBJECT Fdo
    );

NTSTATUS
ClassReleaseQueueCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

VOID
ClasspReleaseQueue(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP ReleaseQueueIrp
    );

VOID
ClasspDisablePowerNotification(
    PFUNCTIONAL_DEVICE_EXTENSION FdoExtension
);

//
// class power routines
//

NTSTATUS
ClassDispatchPower(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
ClassMinimalPowerHandler(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

//
// Child list routines
//

VOID
ClassAddChild(
    IN PFUNCTIONAL_DEVICE_EXTENSION Parent,
    IN PPHYSICAL_DEVICE_EXTENSION Child,
    IN BOOLEAN AcquireLock
    );

PPHYSICAL_DEVICE_EXTENSION
ClassRemoveChild(
    IN PFUNCTIONAL_DEVICE_EXTENSION Parent,
    IN PPHYSICAL_DEVICE_EXTENSION Child,
    IN BOOLEAN AcquireLock
    );

VOID
ClasspRetryDpcTimer(
    IN PCLASS_PRIVATE_FDO_DATA FdoData
    );

VOID
ClasspRetryRequestDpc(
    IN PKDPC Dpc,
    IN PDEVICE_OBJECT DeviceObject,
    IN PVOID Arg1,
    IN PVOID Arg2
    );

VOID
ClassFreeOrReuseSrb(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN PSCSI_REQUEST_BLOCK Srb
    );

VOID
ClassRetryRequest(
    IN PDEVICE_OBJECT SelfDeviceObject,
    IN PIRP           Irp,
    IN LARGE_INTEGER  TimeDelta100ns // in 100ns units
    );

VOID
ClasspBuildRequestEx(
    IN PFUNCTIONAL_DEVICE_EXTENSION Fdo,
    IN PIRP Irp,
    IN PSCSI_REQUEST_BLOCK Srb
    );

NTSTATUS
ClasspAllocateReleaseQueueIrp(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension
    );

NTSTATUS
ClasspInitializeGesn(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN PMEDIA_CHANGE_DETECTION_INFO Info
    );

VOID
ClasspSendNotification(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN const GUID * Guid,
    IN ULONG  ExtraDataSize,
    IN PVOID  ExtraData
    );

VOID
ClassSendEjectionNotification(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension
    );

VOID
ClasspScanForSpecialInRegistry(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension
    );

VOID
ClasspScanForClassHacks(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    IN ULONG_PTR Data
    );

NTSTATUS
ClasspInitializeHotplugInfo(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension
    );

VOID
ClasspPerfIncrementErrorCount(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension
    );
VOID
ClasspPerfIncrementSuccessfulIo(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension
    );

NTSTATUS ClasspUpdateDiskProperties(
    IN PDEVICE_OBJECT Fdo
    );


PTRANSFER_PACKET NewTransferPacket(PDEVICE_OBJECT Fdo);
VOID DestroyTransferPacket(PTRANSFER_PACKET Pkt);
VOID EnqueueFreeTransferPacket(PDEVICE_OBJECT Fdo, PTRANSFER_PACKET Pkt);
PTRANSFER_PACKET DequeueFreeTransferPacket(PDEVICE_OBJECT Fdo, BOOLEAN AllocIfNeeded);
VOID SetupReadWriteTransferPacket(PTRANSFER_PACKET pkt, PVOID Buf, ULONG Len, LARGE_INTEGER DiskLocation, PIRP OriginalIrp);
NTSTATUS SubmitTransferPacket(PTRANSFER_PACKET Pkt);
NTSTATUS TransferPktComplete(IN PDEVICE_OBJECT NullFdo, IN PIRP Irp, IN PVOID Context);
NTSTATUS ServiceTransferRequest(PDEVICE_OBJECT Fdo, PIRP Irp);
VOID TransferPacketRetryTimerDpc(IN PKDPC Dpc, IN PVOID DeferredContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2);
BOOLEAN InterpretTransferPacketError(PTRANSFER_PACKET Pkt);
BOOLEAN RetryTransferPacket(PTRANSFER_PACKET Pkt);
VOID EnqueueDeferredClientIrp(PCLASS_PRIVATE_FDO_DATA FdoData, PIRP Irp);
PIRP DequeueDeferredClientIrp(PCLASS_PRIVATE_FDO_DATA FdoData);
VOID InitLowMemRetry(PTRANSFER_PACKET Pkt, PVOID BufPtr, ULONG Len, LARGE_INTEGER TargetLocation);
BOOLEAN StepLowMemRetry(PTRANSFER_PACKET Pkt);
VOID SetupEjectionTransferPacket(TRANSFER_PACKET *Pkt, BOOLEAN PreventMediaRemoval, PKEVENT SyncEventPtr, PIRP OriginalIrp);
VOID SetupModeSenseTransferPacket(TRANSFER_PACKET *Pkt, PKEVENT SyncEventPtr, PVOID ModeSenseBuffer, UCHAR ModeSenseBufferLen, UCHAR PageMode, PIRP OriginalIrp);
VOID SetupDriveCapacityTransferPacket(TRANSFER_PACKET *Pkt, PVOID ReadCapacityBuffer, ULONG ReadCapacityBufferLen, PKEVENT SyncEventPtr, PIRP OriginalIrp, BOOLEAN Use16ByteCdb);
PMDL BuildDeviceInputMdl(PVOID Buffer, ULONG BufferLen);
VOID FreeDeviceInputMdl(PMDL Mdl);
NTSTATUS InitializeTransferPackets(PDEVICE_OBJECT Fdo);
VOID DestroyAllTransferPackets(PDEVICE_OBJECT Fdo);
VOID InterpretCapacityData(PDEVICE_OBJECT Fdo, PREAD_CAPACITY_DATA_EX ReadCapacityData);


extern LIST_ENTRY AllFdosList;


