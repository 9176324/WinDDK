/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    fakemodem.h

Environment:

    Kernel mode

--*/

#define INITGUID

#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <ntddk.h>
#include <ntddser.h>

// The Windows 2000 and XP DDK build environments do not include
// ntddmodm.h. Therefore, we need to define GUID_DEVINTERFACE_MODEM
// here

#ifdef BUILD_DOWNLEVEL
DEFINE_GUID(GUID_DEVINTERFACE_MODEM,0x2c7089aa, 0x2e0e,0x11d1,0xb1, 0x14, 0x00, 0xc0, 0x4f, 0xc2, 0xaa, 0xe4);
#else
#include <ntddmodm.h>
#endif

#include <wmistr.h>
#include <wmilib.h>
#include <windef.h>
#include <wmistr.h>
#include <wdmguid.h>
#include <string.h>

#define PNP_DEBUG 1

#define ALLOCATE_PAGED_POOL(_y)  ExAllocatePoolWithTag(PagedPool,_y,'wkaF')

#define ALLOCATE_NONPAGED_POOL(_y) ExAllocatePoolWithTag(NonPagedPool,_y,'wkaF')

#define FREE_POOL(_x) {ExFreePool(_x);_x=NULL;};


#define DO_TYPE_PDO   ' ODP'
#define DO_TYPE_FDO   ' ODF'

#define DO_TYPE_DEL_PDO   'ODPx'
#define DO_TYPE_DEL_FDO   'ODFx'

#define DEVICE_OBJECT_NAME_LENGTH 128

extern ULONG  DebugFlags;

#if DBG
#define DEBUG_FLAG_ERROR  0x0001
#define DEBUG_FLAG_INIT   0x0002
#define DEBUG_FLAG_PNP    0x0004
#define DEBUG_FLAG_POWER  0x0008
#define DEBUG_FLAG_WMI    0x0010
#define DEBUG_FLAG_TRACE  0x0020


#define D_INIT(_x)  if (DebugFlags & DEBUG_FLAG_INIT) {_x}
#define D_PNP(_x)   if (DebugFlags & DEBUG_FLAG_PNP) {_x}
#define D_POWER(_x) if (DebugFlags & DEBUG_FLAG_POWER) {_x}
#define D_TRACE(_x) if (DebugFlags & DEBUG_FLAG_TRACE) {_x}
#define D_ERROR(_x) if (DebugFlags & DEBUG_FLAG_ERROR) {_x}
#define D_WMI(_x) if (DebugFlags & DEBUG_FLAG_WMI) {_x}

#else

#define D_INIT(_x)  {}
#define D_PNP(_x)   {}
#define D_POWER(_x) {}
#define D_TRACE(_x) {}
#define D_ERROR(_x) {}
#define D_WMI(_x) {}

#endif


#define OBJECT_DIRECTORY L"DosDevices"

#define READ_BUFFER_SIZE  128

#define COMMAND_MATCH_STATE_IDLE   0
#define COMMAND_MATCH_STATE_GOT_A  1
#define COMMAND_MATCH_STATE_GOT_T  2

//
// This defines the bit used to control whether the device is sending
// a break.  When this bit is set the device is sending a space (logic 0).
//
// Most protocols will assume that this is a hangup.


#define SERIAL_LCR_BREAK    0x40

//
// These defines are used to define the line control register
//

#define SERIAL_5_DATA       ((UCHAR)0x00)
#define SERIAL_6_DATA       ((UCHAR)0x01)
#define SERIAL_7_DATA       ((UCHAR)0x02)
#define SERIAL_8_DATA       ((UCHAR)0x03)
#define SERIAL_DATA_MASK    ((UCHAR)0x03)

#define SERIAL_1_STOP       ((UCHAR)0x00)
#define SERIAL_1_5_STOP     ((UCHAR)0x04) // Only valid for 5 data bits
#define SERIAL_2_STOP       ((UCHAR)0x04) // Not valid for 5 data bits
#define SERIAL_STOP_MASK    ((UCHAR)0x04)

#define SERIAL_NONE_PARITY  ((UCHAR)0x00)
#define SERIAL_ODD_PARITY   ((UCHAR)0x08)
#define SERIAL_EVEN_PARITY  ((UCHAR)0x18)
#define SERIAL_MARK_PARITY  ((UCHAR)0x28)
#define SERIAL_SPACE_PARITY ((UCHAR)0x38)
#define SERIAL_PARITY_MASK  ((UCHAR)0x38)



typedef struct _DEVICE_EXTENSION 
{

    ULONG            DoType;
    KSPIN_LOCK       SpinLock;
    PDEVICE_OBJECT   DeviceObject;
    LONG             ReferenceCount;
    UNICODE_STRING   InterfaceNameString;
    UNICODE_STRING   SymbolicLink;
    ULONG            OpenCount;
    BOOLEAN          Removing;
    BOOLEAN          SurpriseRemoved;
    BOOLEAN          Started;
    KEVENT           RemoveEvent;
    LIST_ENTRY       HoldList;
    LIST_ENTRY       RestartList;
    KEVENT           PdoStartEvent;
    KDPC             ReadDpc;
    PIRP             CurrentReadIrp;
    LIST_ENTRY       ReadQueue;
    PIRP             CurrentWriteIrp;
    LIST_ENTRY       WriteQueue;
    PIRP             CurrentMaskIrp;
    LIST_ENTRY       MaskQueue;
    ULONG            CurrentMask;
    PDEVICE_OBJECT   Pdo;
    PDEVICE_OBJECT   LowerDevice;
    SERIAL_TIMEOUTS  CurrentTimeouts;

    ULONG            BaudRate;
    UCHAR            LineControl;
    UCHAR            ValidDataMask;

    ULONG            ReadBufferBegin;
    ULONG            ReadBufferEnd;
    ULONG            BytesInReadBuffer;
    UCHAR            CommandMatchState;
    BOOLEAN          ConnectCommand;
    BOOLEAN          IgnoreNextChar;
    BOOLEAN          CapsQueried;
    ULONG            ModemStatus;

    BOOLEAN          CurrentlyConnected;
    BOOLEAN          ConnectionStateChanged;
    UCHAR            ReadBuffer[READ_BUFFER_SIZE];

    DEVICE_POWER_STATE  SystemPowerStateMap[PowerSystemMaximum];
    SYSTEM_POWER_STATE SystemWake;
    DEVICE_POWER_STATE DeviceWake;
    BOOLEAN  WakeOnRingEnabled;
    ERESOURCE OpenCloseResource;

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


extern UNICODE_STRING   DriverEntryRegPath;

#define RemoveReferenceForDispatch  RemoveReference
#define RemoveReferenceForIrp       RemoveReference

// Function prototypes

NTSTATUS
DriverEntry(
        IN PDRIVER_OBJECT DriverObject,
        IN PUNICODE_STRING RegistryPath
           );

VOID
FakeModemUnload(
        IN PDRIVER_OBJECT DriverObject
               );

NTSTATUS
FakeModemUnknown(
        IN PDEVICE_OBJECT pDeviceObject,
        IN PIRP pIrp
               );

NTSTATUS
FakeModemAddDevice(
        IN PDRIVER_OBJECT DriverObject,
        IN PDEVICE_OBJECT Pdo
                  );

NTSTATUS
GetRegistryKeyValue (
        IN HANDLE Handle,
        IN PWCHAR KeyNameString,
        IN ULONG KeyNameStringLength,
        IN PVOID Data,
        IN ULONG DataLength
                    );

NTSTATUS
FakeModemHandleSymbolicLink(
        PDEVICE_OBJECT      Pdo,
        BOOLEAN             Create,
        PDEVICE_EXTENSION   DeviceExtension,
        PDEVICE_OBJECT      Fdo
                           );

NTSTATUS
QueryDeviceCaps(
        PDEVICE_OBJECT    Pdo,
        PDEVICE_CAPABILITIES   Capabilities
               );

NTSTATUS
ModemSetRegistryKeyValue(
        IN PDEVICE_OBJECT   Pdo,
        IN ULONG            DevInstKeyType,
        IN PWCHAR           KeyNameString,
        IN ULONG            DataType,
        IN PVOID            Data,
        IN ULONG            DataLength);

NTSTATUS
ModemGetRegistryKeyValue (
        IN PDEVICE_OBJECT   Pdo,
        IN ULONG            DevInstKeyType,
        IN PWCHAR KeyNameString,
        IN PVOID Data,
        IN ULONG DataLength
                         );

NTSTATUS
FakeModemIoControl(
        IN PDEVICE_OBJECT DeviceObject,
        IN PIRP Irp
                  );

NTSTATUS
FakeModemOpen(
        IN PDEVICE_OBJECT DeviceObject,
        IN PIRP Irp
             );

NTSTATUS
FakeModemClose(
        IN PDEVICE_OBJECT DeviceObject,
        IN PIRP Irp
              );

NTSTATUS
FakeModemCleanup(
        IN PDEVICE_OBJECT DeviceObject,
        IN PIRP Irp
                );

void
FakeModemKillPendingIrps(
        PDEVICE_OBJECT DeviceObject
                        );

NTSTATUS
ForwardIrp(
        PDEVICE_OBJECT   NextDevice,
        PIRP   Irp
          );

NTSTATUS
FakeModemAdapterIoCompletion(
        IN PDEVICE_OBJECT DeviceObject,
        IN PIRP Irp,
        IN PKEVENT pdoIoCompletedEvent
                            );

NTSTATUS
WaitForLowerDriverToCompleteIrp(
        PDEVICE_OBJECT    TargetDeviceObject,
        PIRP              Irp,
        PKEVENT           Event
                               );

NTSTATUS
FakeModemPnP(
        IN PDEVICE_OBJECT DeviceObject,
        IN PIRP Irp
            );

NTSTATUS
FakeModemWmi(
        IN PDEVICE_OBJECT   DeviceObject,
        IN PIRP             Irp
            );

NTSTATUS
FakeModemDealWithResources(
        IN PDEVICE_OBJECT   Fdo,
        IN PIRP             Irp
                          );

VOID
DevicePowerCompleteRoutine(
        PDEVICE_OBJECT    DeviceObject,
        IN UCHAR MinorFunction,
        IN POWER_STATE PowerState,
        IN PVOID Context,
        IN PIO_STATUS_BLOCK IoStatus
                          );

NTSTATUS
FakeModemPower(
        IN PDEVICE_OBJECT DeviceObject,
        IN PIRP Irp
              );

NTSTATUS
FakeModemRead(
        IN PDEVICE_OBJECT DeviceObject,
        IN PIRP Irp
             );

NTSTATUS
FakeModemWrite(
        IN PDEVICE_OBJECT DeviceObject,
        IN PIRP Irp
              );

VOID
WriteIrpWorker(
        IN PDEVICE_OBJECT  DeviceObject
              );

VOID
ProcessWriteBytes(
        PDEVICE_EXTENSION   DeviceExtension,
        PUCHAR              Characters,
        ULONG               Length
                 );

VOID
PutCharInReadBuffer(
        PDEVICE_EXTENSION   DeviceExtension,
        UCHAR               Character
                   );

VOID
ReadIrpWorker(
        PDEVICE_OBJECT  DeviceObject
             );

VOID
TryToSatisfyRead(
        PDEVICE_EXTENSION  DeviceExtension
                );

VOID
WriteCancelRoutine(
        IN PDEVICE_OBJECT DeviceObject,
        IN PIRP Irp
                  );

VOID
ReadCancelRoutine(
        IN PDEVICE_OBJECT DeviceObject,
        IN PIRP Irp
                 );

VOID
ProcessConnectionStateChange(
        IN PDEVICE_OBJECT  DeviceObject
                            );

NTSTATUS
CheckStateAndAddReference(
        PDEVICE_OBJECT   DeviceObject,
        PIRP             Irp
                         );

VOID
RemoveReferenceAndCompleteRequest(
        PDEVICE_OBJECT    DeviceObject,
        PIRP              Irp,
        NTSTATUS          StatusToReturn
                                 );

VOID
RemoveReference(
        PDEVICE_OBJECT    DeviceObject
               );

VOID
FakeModemKillAllReadsOrWrites(
        IN PDEVICE_OBJECT DeviceObject,
        IN PLIST_ENTRY QueueToClean,
        IN PIRP *CurrentOpIrp
                             );

