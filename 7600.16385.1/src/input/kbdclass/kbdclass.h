/*++

Copyright (c) 1990, 1991, 1992, 1993, 1994 - 1998  Microsoft Corporation

Module Name:

    kbdclass.h

Abstract:

    These are the structures and defines that are used in the
    keyboard class driver.

Revision History:

--*/

#ifndef _KBDCLASS_
#define _KBDCLASS_

#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4200) // zero-sized array in struct/union

#include <ntddkbd.h>

#include "wmilib.h"

// {09281f1f-f66e-485a-99a2-91638f782c49}
#define WPP_CONTROL_GUIDS \
    WPP_DEFINE_CONTROL_GUID(KbdClassTraceGuid, (09281f1f, f66e, 485a, 99a2, 91638f782c49),\
        WPP_DEFINE_BIT(0)               /* bit  0 */ \
        WPP_DEFINE_BIT(1)               /* bit  1 */ \
        WPP_DEFINE_BIT(2)               /* bit  2 */ \
        WPP_DEFINE_BIT(3)               /* bit  3 */ \
        WPP_DEFINE_BIT(4)               /* bit  4 */ \
        WPP_DEFINE_BIT(DBG_UNUSED05)    /* bit  5 */ \
        WPP_DEFINE_BIT(DBG_UNUSED06)    /* bit  6 */ \
        WPP_DEFINE_BIT(DBG_UNUSED07)    /* bit  7 */ \
        WPP_DEFINE_BIT(DBG_UNUSED08)    /* bit  8 */ \
        WPP_DEFINE_BIT(DBG_UNUSED09)    /* bit  9 */ \
        WPP_DEFINE_BIT(DBG_UNUSED10)    /* bit 10 */ \
        WPP_DEFINE_BIT(DBG_UNUSED11)    /* bit 11 */ \
        WPP_DEFINE_BIT(DBG_UNUSED12)    /* bit 12 */ \
        WPP_DEFINE_BIT(DBG_UNUSED13)    /* bit 13 */ \
        WPP_DEFINE_BIT(DBG_UNUSED14)    /* bit 14 */ \
        WPP_DEFINE_BIT(DBG_UNUSED15)    /* bit 15 */ \
        WPP_DEFINE_BIT(DBG_UNUSED16)    /* bit 16 */ \
        WPP_DEFINE_BIT(DBG_UNUSED17)    /* bit 17 */ \
        WPP_DEFINE_BIT(DBG_UNUSED18)    /* bit 18 */ \
        WPP_DEFINE_BIT(DBG_UNUSED19)    /* bit 19 */ \
        WPP_DEFINE_BIT(DBG_UNUSED20)    /* bit 20 */ \
        WPP_DEFINE_BIT(DBG_UNUSED21)    /* bit 21 */ \
        WPP_DEFINE_BIT(DBG_UNUSED22)    /* bit 22 */ \
        WPP_DEFINE_BIT(DBG_UNUSED23)    /* bit 23 */ \
        WPP_DEFINE_BIT(DBG_UNUSED24)    /* bit 24 */ \
        WPP_DEFINE_BIT(DBG_UNUSED25)    /* bit 25 */ \
        WPP_DEFINE_BIT(DBG_UNUSED26)    /* bit 26 */ \
        WPP_DEFINE_BIT(DBG_UNUSED27)    /* bit 27 */ \
        WPP_DEFINE_BIT(DBG_UNUSED28)    /* bit 28 */ \
        WPP_DEFINE_BIT(DBG_UNUSED29)    /* bit 29 */ \
        WPP_DEFINE_BIT(DBG_UNUSED30)    /* bit 30 */ \
       )



#define KEYBOARD_POOL_TAG 'CdbK'
#undef ExAllocatePool
#define ExAllocatePool(Type, Bytes) ExAllocatePoolWithTag(Type, Bytes, KEYBOARD_POOL_TAG)

//
// Define the default number of elements in the class input data queue.
//

#define DATA_QUEUE_SIZE 100
#define MAXIMUM_PORTS_SERVICED 10
#define NAME_MAX 256
#define DUMP_COUNT 4

#define MAX(a,b) (((a) < (b)) ? (b) : (a))

#define KEYBOARD_POWER_LIGHT_TIME L"PowerLightTime"
#define KEYBOARD_WAIT_WAKE_ENABLE L"WaitWakeEnabled"
#define KEYBOARD_ALLOW_DISABLE L"AllowDisable"

#define IS_TRUSTED_FILE_FOR_READ(x) (&DriverEntry == (x)->FsContext2)
#define SET_TRUSTED_FILE_FOR_READ(x) ((x)->FsContext2 = &DriverEntry)
#define CLEAR_TRUSTED_FILE_FOR_READ(x) ((x)->FsContext2 = NULL)

#define ALLOW_OVERFLOW TRUE

//
// Port description
//
// Used only with the
// allforoneandoneforall turned off (AKA ConnectOneClassToOnePort
// turned on).  This is the file sent to the ports.
//
typedef struct _PORT {
    //
    // The file Pointer to the port;
    //
    PFILE_OBJECT    File;

    //
    // The port itself
    //
    struct _DEVICE_EXTENSION * Port;

    //
    // Port flags
    //
    BOOLEAN     Enabled;
    BOOLEAN     Reserved [2];
    BOOLEAN     Free;
} PORT, *PPORT;

#define PORT_WORKING(port) ((port)->Enabled && !(port)->Free)

//
// Class device extension.
//
typedef struct _DEVICE_EXTENSION {

    //
    // Back pointer to the Device Object created for this port.
    //
    PDEVICE_OBJECT  Self;

    //
    // Pointer to the active Class DeviceObject;
    // If the AFOAOFA (all for one and one for all) switch is on then this
    // points to the device object named as the first keyboard.
    //
    PDEVICE_OBJECT  TrueClassDevice;

    //
    // The Target port device Object to which all IRPs are sent.
    //
    PDEVICE_OBJECT  TopPort;

    //
    // The PDO if applicable.
    //
    PDEVICE_OBJECT  PDO;

    //
    // A remove lock to keep track of outstanding I/Os to prevent the device
    // object from leaving before such time as all I/O has been completed.
    //
    IO_REMOVE_LOCK  RemoveLock;

    //
    // If this port a Plug and Play port
    //
    BOOLEAN         PnP;
    BOOLEAN         Started;
    BOOLEAN         AllowDisable;

    KSPIN_LOCK WaitWakeSpinLock;

    //
    // Is the Trusted Subsystem Connected
    //
    ULONG TrustedSubsystemCount;

    //
    // Number of input data items currently in the InputData queue.
    //
    ULONG InputCount;

    //
    // A Unicode string pointing to the symbolic link for the Device Interface
    // of this device object.
    //
    UNICODE_STRING  SymbolicLinkName;

    //
    // Start of the class input data queue (really a circular buffer).
    //
    PKEYBOARD_INPUT_DATA InputData;

    //
    // Insertion pointer for InputData.
    //
    PKEYBOARD_INPUT_DATA DataIn;

    //
    // Removal pointer for InputData.
    //
    PKEYBOARD_INPUT_DATA DataOut;

    //
    // Keyboard attributes.
    //
    KEYBOARD_ATTRIBUTES  KeyboardAttributes;

    //
    // A saved state of indicator lights
    //
    KEYBOARD_INDICATOR_PARAMETERS   IndicatorParameters;

    //
    // Spinlock used to synchronize access to the input data queue and its
    // insertion/removal pointers.
    //
    KSPIN_LOCK SpinLock;

    //
    // Queue of pended read requests sent to this port.  Access to this queue is
    // guarded by SpinLock
    //
    LIST_ENTRY ReadQueue;

    //
    // Request sequence number (used for error logging).
    //
    ULONG SequenceNumber;

    //
    // The "D" and "S" states of the current device
    //
    DEVICE_POWER_STATE DeviceState;
    SYSTEM_POWER_STATE SystemState;

    ULONG UnitId;

    //
    // WMI Information
    //
    WMILIB_CONTEXT WmiLibInfo;

    //
    // Mapping of system to device states when a wait wake irp is active
    //
    DEVICE_POWER_STATE SystemToDeviceState[PowerSystemHibernate];

    //
    // Minimum amount of power needed to wake the device
    //
    DEVICE_POWER_STATE MinDeviceWakeState;

    //
    // Lowest system state that the machine can be in and have the device wake it up
    //
    SYSTEM_POWER_STATE MinSystemWakeState;

    //
    // Actual wait wake irp
    //
    PIRP WaitWakeIrp;

    //
    // Duplicate wait wake irp getting completed because another was queued.
    //
    PIRP ExtraWaitWakeIrp;

    //
    // Target Device Notification Handle
    //
    PVOID TargetNotifyHandle;

    //
    // Only used for a legacy port device
    //
    LIST_ENTRY Link;

    //
    // Used only for a legacy port device when grand master mode is off
    //
    PFILE_OBJECT File;

    //
    // Used for a legacy port device
    //
    BOOLEAN Enabled;

    //
    // Indicates whether it is okay to log overflow errors.
    //
    BOOLEAN OkayToLogOverflow;

    //
    // Indicates whether it is okay to send wait wake irps down the stack
    // (does NOT reflect if the bus can implement or not)
    //
    BOOLEAN WaitWakeEnabled;

    //
    // Indicates whether we have received a surprise removed irp
    //
    BOOLEAN SurpriseRemoved;

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

//
// On some busses, we can power down the bus, but not the system, in this case
// we still need to allow the device to wake said bus, therefore
// waitwake-supported should not rely on systemstate.
//
// #define WAITWAKE_SUPPORTED(port) ((port)->MinDeviceWakeState > PowerDeviceUnspecified) && \
//                                  (port)->MinSystemWakeState > PowerSystemWorking)
#define WAITWAKE_SUPPORTED(port) ((port)->MinDeviceWakeState > PowerDeviceD0 && \
                                  (port)->MinSystemWakeState > PowerSystemWorking)

// #define WAITWAKE_ON(port)        ((port)->WaitWakeIrp != 0)
#define WAITWAKE_ON(port) \
       (InterlockedCompareExchangePointer(&(port)->WaitWakeIrp, NULL, NULL) != NULL)

#define SHOULD_SEND_WAITWAKE(port) (WAITWAKE_SUPPORTED(port) && \
                                    !WAITWAKE_ON(port)       && \
                                    KeyboardClassCheckWaitWakeEnabled(port))

//
// Global shared data
//

typedef struct _GLOBALS {

    //
    // If ConnectOneClassToOnePort is off aka we want "All for one and one for
    // all behavior" then we need to create the one Master DO to which all
    // the goods go.
    //
    PDEVICE_EXTENSION   GrandMaster;

    //
    // List of ClassDevices that associated with the same name
    // aka the all for one and one for all flag is set
    //
    PPORT       AssocClassList;
    ULONG       NumAssocClass;
    LONG        Opens;
    ULONG       NumberLegacyPorts;
    FAST_MUTEX  Mutex;

    //
    // Specifies the type of class-port connection to make.  A '1'
    // indicates a 1:1 relationship between class device objects and
    // port device objects.  A '0' indicates a 1:many relationship.
    //
    ULONG ConnectOneClassToOnePort;

    //
    // When kbdclass receives an output command (EG set LEDs) this flag
    // instructs it to transmit that command to all attached ports,
    // regardless of the unit ID which was specified.
    //
    ULONG SendOutputToAllPorts;

    //
    // Number of port drivers serviced by this class driver.
    //
    ULONG PortsServiced;

    //
    //
    // IntialDevice Extension
    //
    DEVICE_EXTENSION    InitExtension;

    //
    // A list of the registry path to the service parameters.
    //
    UNICODE_STRING      RegistryPath;

    //
    // The base name for all class objects created as mice.
    //
    UNICODE_STRING      BaseClassName;
    WCHAR               BaseClassBuffer[NAME_MAX];

    //
    // Linked list of all the legacy device objects that were created in
    // DriverEntry or FindMorePorts.  We maintain this list so we can delete
    // them when we unload.
    //
    LIST_ENTRY LegacyDeviceList;
} GLOBALS, *PGLOBALS;

typedef struct _KBD_CALL_ALL_PORTS {
    //
    // Number of ports to call
    //
    ULONG   Len;

    //
    // Current Called port;
    //
    ULONG   Current;

    //
    // Array of Ports to call
    //
    PORT    Port[];

} KBD_CALL_ALL_PORTS, *PKBD_CALL_ALL_PORTS;

//
// Keyboard configuration information.
//

typedef struct _KEYBOARD_CONFIGURATION_INFORMATION {

    //
    // Maximum size of class input data queue, in bytes.
    //

    ULONG  DataQueueSize;

} KEYBOARD_CONFIGURATION_INFORMATION, *PKEYBOARD_CONFIGURATION_INFORMATION;

typedef struct _KEYBOARD_WORK_ITEM_DATA {
    PIRP                Irp;
    PDEVICE_EXTENSION   Data;
    PIO_WORKITEM        Item;
    BOOLEAN             WaitWakeState;
} KEYBOARD_WORK_ITEM_DATA, *PKEYBOARD_WORK_ITEM_DATA;

#define KeyboardClassDeleteLegacyDevice(de)                 \
{                                                           \
    if (de->InputData) {                                    \
        ExFreePool (de->InputData);                         \
        de->InputData = de->DataIn = de->DataOut = NULL;    \
    }                                                       \
    IoDeleteDevice (de->Self);                              \
    de = NULL;                                              \
}

//
// Function Declarations
//

NTSTATUS
KeyboardAddDeviceEx(
    IN PDEVICE_EXTENSION NewDeviceObject,
    __in PWSTR            FullClassName,
    IN PFILE_OBJECT      File
    );

DRIVER_ADD_DEVICE
KeyboardAddDevice;

void
KeyboardClassGetWaitWakeEnableState(
    IN PDEVICE_EXTENSION Data
    );

DRIVER_INITIALIZE
DriverEntry;

__drv_dispatchType(IRP_MJ_INTERNAL_DEVICE_CONTROL)
DRIVER_DISPATCH
KeyboardClassPassThrough;

DRIVER_CANCEL
KeyboardClassCancel;

__drv_dispatchType(IRP_MJ_CLEANUP)
DRIVER_DISPATCH
KeyboardClassCleanup;

__drv_dispatchType(IRP_MJ_DEVICE_CONTROL)
DRIVER_DISPATCH
KeyboardClassDeviceControl;

__drv_dispatchType(IRP_MJ_FLUSH_BUFFERS)
DRIVER_DISPATCH
KeyboardClassFlush;

__drv_dispatchType(IRP_MJ_CREATE)
DRIVER_DISPATCH
KeyboardClassCreate;

__drv_dispatchType(IRP_MJ_CLOSE)
DRIVER_DISPATCH
KeyboardClassClose;

__drv_dispatchType(IRP_MJ_READ)
DRIVER_DISPATCH
KeyboardClassRead;

NTSTATUS
KeyboardClassReadCopyData(
    IN PDEVICE_EXTENSION DeviceExtension,
    IN PIRP Irp
    );

PIRP
KeyboardClassDequeueRead(
    IN PDEVICE_EXTENSION DeviceExtension
    );

BOOLEAN
KeyboardClassCheckWaitWakeEnabled(
    IN PDEVICE_EXTENSION Data
    );

BOOLEAN
KeyboardClassCreateWaitWakeIrp (
    IN PDEVICE_EXTENSION Data
    );

REQUEST_POWER_COMPLETE
KeyboardClassWaitWakeComplete;

REQUEST_POWER_COMPLETE
KeyboardClassWWPowerUpComplete;

IO_COMPLETION_ROUTINE
KbdSyncComplete;

IO_COMPLETION_ROUTINE
KeyboardClassPowerComplete;

REQUEST_POWER_COMPLETE
KeyboardClassPoRequestComplete;

__drv_dispatchType(IRP_MJ_POWER)
DRIVER_DISPATCH
KeyboardClassPower;

NTSTATUS
KeyboardSendIrpSynchronously (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN BOOLEAN          CopyToNext
    );

__drv_dispatchType(IRP_MJ_PNP)
DRIVER_DISPATCH
KeyboardPnP;

VOID
KeyboardClassServiceCallback(
    IN PDEVICE_OBJECT DeviceObject,
    IN PKEYBOARD_INPUT_DATA InputDataStart,
    IN PKEYBOARD_INPUT_DATA InputDataEnd,
    IN OUT PULONG InputDataConsumed
    );

DRIVER_UNLOAD
KeyboardClassUnload;

BOOLEAN
KbdCancelRequest(
    IN PVOID Context
    );

VOID
KbdConfiguration();

__drv_valueIs(==0;<0)
NTSTATUS
KbdCreateClassObject(
    __in  PDRIVER_OBJECT      DriverObject,
    __in  PDEVICE_EXTENSION   TmpDeviceExtension,
    __out __deref __drv_when(return<0, __drv_valueIs(==0))
    __deref __drv_when(return==0, __drv_allocatesMem(Mem) __drv_valueIs(!=0))
         PDEVICE_OBJECT     * ClassDeviceObject,
    __deref_out_opt PWSTR   * FullDeviceName,
    __in  BOOLEAN             Legacy
    );

NTSTATUS
KbdDeterminePortsServiced(
    IN PUNICODE_STRING BasePortName,
    IN OUT PULONG NumberPortsServiced
    );

RTL_QUERY_REGISTRY_ROUTINE
KbdDeviceMapQueryCallback;

NTSTATUS
KbdEnableDisablePort(
    IN BOOLEAN EnableFlag,
    IN PIRP    Irp,
    IN PDEVICE_EXTENSION Port,
    IN PFILE_OBJECT * File
    );

NTSTATUS
KbdSendConnectRequest(
    IN PDEVICE_EXTENSION ClassData,
    IN PVOID ServiceCallback
    );

VOID
KbdInitializeDataQueue(
    IN PVOID Context
    );

IO_COMPLETION_ROUTINE
KeyboardCallAllPorts;

IO_COMPLETION_ROUTINE
KeyboardClassSetLedsComplete;

NTSTATUS
KeyboardClassEnableGlobalPort(
    IN PDEVICE_EXTENSION Port,
    IN BOOLEAN Enabled
    );

DRIVER_NOTIFICATION_CALLBACK_ROUTINE
KeyboardClassPlugPlayNotification;

VOID
KeyboardClassLogError(
    PVOID Object,
    NTSTATUS ErrorCode,
    ULONG UniqueErrorValue,
    NTSTATUS FinalStatus,
    ULONG DumpCount,
    ULONG *DumpData,
    UCHAR MajorFunction
    );

BOOLEAN
KeyboardClassCreateWaitWakeIrp (
    IN PDEVICE_EXTENSION Data
    );

IO_WORKITEM_ROUTINE
KeyboardClassCreateWaitWakeIrpWorker;

NTSTATUS
KeyboardToggleWaitWake(
    PDEVICE_EXTENSION Data,
    BOOLEAN           WaitWakeState
    );

IO_WORKITEM_ROUTINE
KeyboardToggleWaitWakeWorker;

NTSTATUS
KeyboardQueryDeviceKey (
    IN  HANDLE  Handle,
    __in PWSTR  ValueNameString,
    __out_bcount(DataLength) PVOID   Data,
    IN  ULONG   DataLength
    );

VOID
KeyboardClassFindMorePorts(
    PDRIVER_OBJECT  DriverObject,
    PVOID           Context,
    ULONG           Count
    );

__drv_dispatchType(IRP_MJ_SYSTEM_CONTROL)
DRIVER_DISPATCH
KeyboardClassSystemControl;

WMI_SET_DATAITEM_CALLBACK
KeyboardClassSetWmiDataItem;

WMI_SET_DATABLOCK_CALLBACK
KeyboardClassSetWmiDataBlock;

WMI_QUERY_DATABLOCK_CALLBACK
KeyboardClassQueryWmiDataBlock;

WMI_QUERY_REGINFO_CALLBACK
KeyboardClassQueryWmiRegInfo;

#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4200)
#endif

#endif // _KBDCLASS_

