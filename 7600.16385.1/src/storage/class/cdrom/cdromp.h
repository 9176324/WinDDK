/*++

Copyright (C) Microsoft Corporation. All rights reserved.

Module Name:

    cdromp.h 

Abstract:

    Private header file for cdrom.sys modules.  This contains private
    structure and function declarations as well as constant values which do
    not need to be exported.

Author:

Environment:

    kernel mode only

Notes:


Revision History:

--*/

#ifndef __CDROMP_H__
#define __CDROMP_H__


#include <scsi.h>
#include <storduid.h>
#include <mountdev.h>
#include <ioevent.h>
#include <ntintsafe.h>

/*
 *  IA64 requires 8-byte alignment for pointers, but the IA64 NT kernel expects 16-byte alignment
 */
#ifdef _WIN64
    #define PTRALIGN                DECLSPEC_ALIGN(16)
#else
    #define PTRALIGN
#endif

// NOTE: Start with a smaller 100 second maximum, due to current assert in CLASSPNP
//       0x0000 00C9'2A69 C000 (864,000,000,000) is 24 hours in 100ns units
//       0x0000 0000'3B9A CA00 (  1,000,000,000) is 100 seconds in 100ns units
#define MAXIMUM_RETRY_FOR_SINGLE_IO_IN_100NS_UNITS (0x3B9ACA00)

// structures to simplify matching devices, ids, and hacks required for
// these ids.
typedef struct _CDROM_SCAN_FOR_SPECIAL_INFO {
    //
    // * NULL pointers indicates that no match is required.
    // * empty string will only match an empty string.  non-existant strings
    //   in the device descriptor are considered empty strings for this match.
    //   (ie. "" will only match "")
    // * all other strings will do partial matches, based upon
    //   string provided (ie. "hi" will match "hitazen" and "higazui")
    // * array must end with all three PCHARs being set to NULL.
    //

    PCHAR      VendorId;
    PCHAR      ProductId;
    PCHAR      ProductRevision;

    //
    // marked as a ULONG_PTR to allow use as either a ptr to a data block
    // or 32 bits worth of flags. (64 bits on 64 bit systems)  no longer a
    // const so that it may be dynamically built.
    //

    ULONG_PTR  Data;

} CDROM_SCAN_FOR_SPECIAL_INFO, *PCDROM_SCAN_FOR_SPECIAL_INFO;

// Define the various states that media can be in for autorun.
typedef enum _MEDIA_CHANGE_DETECTION_STATE {
    MediaUnknown,
    MediaPresent,
    MediaNotPresent,
    MediaUnavailable   // e.g. cd-r media undergoing burn
} MEDIA_CHANGE_DETECTION_STATE, *PMEDIA_CHANGE_DETECTION_STATE;


/*++////////////////////////////////////////////////////////////////////////////

    This structure defines the history kept for a given transfer packet.
    It includes a srb status/sense data structure that is always either valid
    or zero-filled for the full 18 bytes, time sent/completed, and how long
    the retry delay was requested to be.
    
--*/
typedef struct _SRB_HISTORY_ITEM {
    LARGE_INTEGER TickCountSent;             //  0x00..0x07
    LARGE_INTEGER TickCountCompleted;        //  0x08..0x0F
    ULONG         MillisecondsDelayOnRetry;  //  0x10..0x13
    SENSE_DATA    NormalizedSenseData;       //  0x14..0x25 (0x12 bytes)
    UCHAR         SrbStatus;                 //  0x26
    UCHAR         ClassDriverUse;            //  0x27 -- one byte free (alignment)
} SRB_HISTORY_ITEM, *PSRB_HISTORY_ITEM;

typedef struct _SRB_HISTORY {
    ULONG_PTR        ClassDriverUse[4]; // for the class driver to use as they please
    __field_range(1,30000)
    ULONG            TotalHistoryCount;
    __field_range(0,TotalHistoryCount)
    ULONG            UsedHistoryCount;
    __field_ecount_part(TotalHistoryCount, UsedHistoryCount)
    SRB_HISTORY_ITEM History[1];
} SRB_HISTORY, *PSRB_HISTORY;

extern CDROM_SCAN_FOR_SPECIAL_INFO CdRomBadItems[];

/*++////////////////////////////////////////////////////////////////////////////*/

// legacy registry key and values.
#define CLASSP_REG_SUBKEY_NAME                  (L"Classpnp")

#define CLASSP_REG_HACK_VALUE_NAME              (L"HackMask")
#define CLASSP_REG_MMC_DETECTION_VALUE_NAME     (L"MMCDetectionState")
#define CLASSP_REG_WRITE_CACHE_VALUE_NAME       (L"WriteCacheEnableOverride")
#define CLASSP_REG_PERF_RESTORE_VALUE_NAME      (L"RestorePerfAtCount")
#define CLASSP_REG_REMOVAL_POLICY_VALUE_NAME    (L"UserRemovalPolicy")
#define WINPE_REG_KEY_NAME                      (L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\MiniNT")

#define CLASS_PERF_RESTORE_MINIMUM              (0x10)
#define CLASS_ERROR_LEVEL_1                     (0x4)
#define CLASS_ERROR_LEVEL_2                     (0x8)

#define FDO_HACK_CANNOT_LOCK_MEDIA              (0x00000001)
#define FDO_HACK_GESN_IS_BAD                    (0x00000002)
#define FDO_HACK_NO_RESERVE6                    (0x00000008)
#define FDO_HACK_GESN_IGNORE_OPCHANGE           (0x00000010)
#define FDO_HACK_NO_STREAMING                   (0x00000020)

#define FDO_HACK_VALID_FLAGS                    (0x0000003F)
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
//#define NUM_LOCKMEDIAREMOVAL_RETRIES    1
//#define NUM_MODESENSE_RETRIES           1
//#define NUM_DRIVECAPACITY_RETRIES       1

/*
 *  We retry failed I/O requests at 1-second intervals.
 *  In the case of a failure due to bus reset, we want to make sure that we retry after the allowable
 *  reset time.  For SCSI, the allowable reset time is 5 seconds.  ScsiPort queues requests during
 *  a bus reset, which should cause us to retry after the reset is over; but the requests queued in
 *  the miniport are failed all the way back to us immediately.  In any event, in order to make
 *  extra sure that our retries span the allowable reset time, we should retry more than 5 times.
 */
//#define NUM_IO_RETRIES      8

#define CDROM_VOLUME_VERIFY_CHECKED        0x34

#define CDROM_TAG_PRIVATE_DATA             'CPcS'

typedef struct _MEDIA_CHANGE_DETECTION_INFO {

    // Mutex to synchronize enable/disable requests and media state changes
    KMUTEX      MediaChangeMutex;

    // For request erialization use.
    // This irp is used in timer callback routine, will be sent to the device itself.
    // so that the sequentail queue will get request wrapped on this irp. After doing
    // the real work, this irp will be completed to indicate it's ok to send the next
    // MCN request.
    PIRP        MediaChangeSyncIrp;

    // The last known state of the media (present, not present, unknown).
    // Protected by MediaChangeMutex.
    MEDIA_CHANGE_DETECTION_STATE LastKnownMediaDetectionState;

    // The last state of the media (present, not present, unknown) reported to apps
    // via notifications. Protected by MediaChangeMutex
    MEDIA_CHANGE_DETECTION_STATE LastReportedMediaDetectionState;

    // This is a count of how many time MCD has been disabled.  if it is
    // set to zero, then we'll poll the device for MCN events with the
    // then-current method (ie. TEST UNIT READY or GESN).  this is
    // protected by MediaChangeMutex
    LONG        MediaChangeDetectionDisableCount;

    // The timer value to support media change events.  This is a countdown
    // value used to determine when to poll the device for a media change.
    // The max value for the timer is 255 seconds.  This is not protected
    // by an event -- simply InterlockedExchanged() as needed.
    LONG        MediaChangeCountDown;

    // recent changes allowed instant retries of the MCN REQUEST.  Since this
    // could cause an infinite loop, keep a count of how many times we've
    // retried immediately so that we can catch if the count exceeds an
    // arbitrary limit.
    LONG        MediaChangeRetryCount;

    // use GESN if it's available
    struct {
        BOOLEAN Supported;
        BOOLEAN HackEventMask;
        UCHAR   EventMask;
        UCHAR   NoChangeEventMask;
        PUCHAR  Buffer;
        PMDL    Mdl;
        ULONG   BufferSize;
    } Gesn;

    // If this value is one, then the REQUEST is currently in use.
    // If this value is zero, then the REQUEST is available.
    // Use InterlockedCompareExchange() to set from "available" to "in use".
    // ASSERT that InterlockedCompareExchange() showed previous value of
    //    "in use" when changing back to "available" state.
    // This also implicitly protects the MediaChangeSrb and SenseBuffer
    LONG                MediaChangeRequestInUse;

    // Pointer to the REQUEST to be used for media change detection.
    // protected by Interlocked MediaChangeRequestInUse
    WDFREQUEST          MediaChangeRequest;

    // The srb for the media change detection.
    // protected by Interlocked MediaChangeIrpInUse
    SCSI_REQUEST_BLOCK  MediaChangeSrb;
    PUCHAR              SenseBuffer;
    ULONG               SrbFlags;

    // Second timer to keep track of how long the media change REQUEST has been
    // in use.  If this value exceeds the timeout (#defined) then we should
    // print out a message to the user and set the MediaChangeRequestLost flag
    // protected by using Interlocked() operations in RequestSendMcnRequest(),
    // the only routine which should modify this value.
    LONG                MediaChangeRequestTimeInUse;

    // Set by CdRomTickHandler when we determine that the media change REQUEST has
    // been lost
    BOOLEAN             MediaChangeRequestLost;

} MEDIA_CHANGE_DETECTION_INFO, *PMEDIA_CHANGE_DETECTION_INFO;;

typedef enum {
    SimpleMediaLock,
    SecureMediaLock,
    InternalMediaLock
} MEDIA_LOCK_TYPE, *PMEDIA_LOCK_TYPE;


typedef enum _CDROM_DETECTION_STATE {
    CdromDetectionUnknown = 0,
    CdromDetectionUnsupported = 1,
    CdromDetectionSupported = 2
} CDROM_DETECTION_STATE, *PCDROM_DETECTION_STATE;


typedef struct _CDROM_ERROR_LOG_DATA {
    LARGE_INTEGER       TickCount;          // Offset 0x00
    ULONG               PortNumber;         // Offset 0x08

    UCHAR               ErrorPaging    : 1; // Offset 0x0c
    UCHAR               ErrorRetried   : 1;
    UCHAR               ErrorUnhandled : 1;
    UCHAR               ErrorReserved  : 5;

    UCHAR               Reserved[3];

    SCSI_REQUEST_BLOCK  Srb;                // Offset 0x10

    //  We define the SenseData as the default length.
    //  Since the sense data returned by the port driver may be longer,
    //  SenseData must be at the end of this structure.
    //  For our internal error log, we only log the default length.
    SENSE_DATA          SenseData;     // Offset 0x50 for x86 (or 0x68 for ia64) (ULONG32 Alignment required!)
} CDROM_ERROR_LOG_DATA, *PCDROM_ERROR_LOG_DATA;


#define NUM_ERROR_LOG_ENTRIES       16

//
// add to the front of this structure to help prevent illegal
// snooping by other utilities.
//
typedef struct _CDROM_PRIVATE_FDO_DATA {

    // this private structure allows us to
    // dynamically re-enable the perf benefits
    // lost due to transient error conditions.
    // in w2k, a reboot was required. :(
    struct {
        ULONG      OriginalSrbFlags;
        ULONG      SuccessfulIO;
        ULONG      ReEnableThreshhold; // 0 means never
    } Perf;

    ULONG_PTR            HackFlags;

    STORAGE_HOTPLUG_INFO HotplugInfo;

    BOOLEAN              TimerStarted;
    BOOLEAN              LoggedTURFailureSinceLastIO;
    BOOLEAN              LoggedSYNCFailure;

    // not use WDFSPINLOCK to avoid exposing private object creation 
    // in initialization code. (cdrom.sys was in WDK example)
    KSPIN_LOCK           SpinLock;

    // Circular array of timestamped logs of errors that occurred on this device.
    ULONG                ErrorLogNextIndex;
    CDROM_ERROR_LOG_DATA ErrorLogs[NUM_ERROR_LOG_ENTRIES];

}CDROM_PRIVATE_FDO_DATA, *PCDROM_PRIVATE_FDO_DATA;

//
// this is a private structure, but must be kept here
// to properly compile size of FUNCTIONAL_DEVICE_EXTENSION
//
typedef struct _FILE_OBJECT_CONTEXT {
    WDFFILEOBJECT   FileObject;
    WDFDEVICE       DeviceObject;
    ULONG           LockCount;
    ULONG           McnDisableCount;
    BOOLEAN         EnforceStreamingRead;
    BOOLEAN         EnforceStreamingWrite;
} FILE_OBJECT_CONTEXT, *PFILE_OBJECT_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(FILE_OBJECT_CONTEXT, FileObjectGetContext)


#define NOT_READY_RETRY_INTERVAL    10
#define MODE_PAGE_DATA_SIZE         192

#endif // __CDROMP_H__

