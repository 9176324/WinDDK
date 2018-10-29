/*++
    Copyright (c) Microsoft Corporation

    Module Name:
        hidpen.h

    Abstract:
        Contains definitions of all constants and data types for the
        serial pen hid driver.

    Environment:
        Kernel mode

--*/

#ifndef _HIDPEN_H
#define _HIDPEN_H

//
// Constants
//
#define HPEN_POOL_TAG                   'nepH'

//
// Pen Tablet Report IDs
//
#define REPORTID_PEN                    1
#define REPORTID_MOUSE                  2

// dwfHPen flag values
#define HPENF_DEVICE_STARTED            0x00000001
#define HPENF_DIGITIZER_STANDBY         0x00000002

#define ULONG_MAX                       0xffffffffUL
//
// Macros
//
#define GET_MINIDRIVER_DEVICE_EXTENSION(DO) \
    ((PDEVICE_EXTENSION)(((PHID_DEVICE_EXTENSION)(DO)->DeviceExtension)->MiniDeviceExtension))

#define GET_NEXT_DEVICE_OBJECT(DO) \
    (((PHID_DEVICE_EXTENSION)(DO)->DeviceExtension)->NextDeviceObject)

#define GET_PDO(DO) \
    (((PHID_DEVICE_EXTENSION)(DO)->DeviceExtension)->PhysicalDeviceObject)

//
// Type Definitions
//
typedef struct _DEVICE_EXTENSION
{
    ULONG                 dwfHPen;      //flags
    PDEVICE_OBJECT        DevObj;       //fdo
    PDEVICE_OBJECT        pdo;          //pdo of the pen device
    PDEVICE_OBJECT        LowerDevObj;  //points to the serial device object
    IO_REMOVE_LOCK        RemoveLock;   //to protect IRP_MN_REMOVE_DEVICE
    KSPIN_LOCK            SpinLock;     //to protect the resync buffer
    DEVICE_POWER_STATE    PowerState;   //power state of the digitizer
    OEM_DATA              OemData;      //OEM specific data  
 } DEVICE_EXTENSION, *PDEVICE_EXTENSION;

typedef struct _WORKITEM_DATA
{
   POWER_STATE    NewPowerState;
   PIRP		  Irp;
   PIO_WORKITEM   workItem;
}WORKITEM_DATA, *PWORKITEM_DATA;


//
// Function prototypes
//

// hidpen.c

DRIVER_INITIALIZE DriverEntry;

__drv_dispatchType(IRP_MJ_CREATE)
__drv_dispatchType(IRP_MJ_CLOSE)
DRIVER_DISPATCH HpenCreateClose;

__drv_dispatchType(IRP_MJ_SYSTEM_CONTROL)
DRIVER_DISPATCH HpenSystemControl;


DRIVER_ADD_DEVICE HpenAddDevice;


DRIVER_UNLOAD HpenUnload;


NTSTATUS INTERNAL
RegQueryValue(
    __in  HANDLE  hkey,
    __in  LPCWSTR pszValueName,
    __in  ULONG   dwType,
    __out_bcount(dwcbData)  PUCHAR lpbData,
    __in  ULONG   dwcbData,
    __out_opt PULONG  pdwcbLen
    );

NTSTATUS INTERNAL
RegSetValue(
    __in HANDLE  hkey,
    __in LPCWSTR pszValueName,
    __in ULONG   dwType,
    __in_bcount_opt(dwcbData) PUCHAR  lpbData,
    __in ULONG   dwcbData
    );


__drv_dispatchType(IRP_MJ_PNP)
DRIVER_DISPATCH HpenPnp;
// pnp.c


__drv_dispatchType(IRP_MJ_POWER)
DRIVER_DISPATCH HpenPower;



NTSTATUS INTERNAL
SendSyncIrp(
    __in PDEVICE_OBJECT DevObj,
    __in PIRP           Irp,
    __in BOOLEAN        fCopyToNext
    );

IO_COMPLETION_ROUTINE IrpCompletion;


IO_COMPLETION_ROUTINE DevicePowerIrpCompletion;



__drv_dispatchType(IRP_MJ_INTERNAL_DEVICE_CONTROL)
DRIVER_DISPATCH HpenInternalIoctl;
// ioctl.c


NTSTATUS INTERNAL
GetDeviceDescriptor(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp
    );

NTSTATUS INTERNAL
GetReportDescriptor(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp
    );

NTSTATUS INTERNAL
ReadReport(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp,
    __out BOOLEAN          *fSentDown
    );

NTSTATUS INTERNAL
GetString(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp
    );

NTSTATUS INTERNAL
GetAttributes(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp
    );

// OEM specific functions
NTSTATUS EXTERNAL
OemAddDevice(
    __in PDEVICE_EXTENSION DevExt
    );

NTSTATUS INTERNAL
OemStartDevice(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp
    );

NTSTATUS INTERNAL
OemStopDevice(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp
    );

NTSTATUS INTERNAL
OemInitDevice(
    __in PDEVICE_EXTENSION DevExt
    );

NTSTATUS INTERNAL
OemWakeupDevice(
    __in PDEVICE_EXTENSION DevExt
    );

NTSTATUS INTERNAL
OemStandbyDevice(
    __in PDEVICE_EXTENSION DevExt
    );

NTSTATUS INTERNAL
OemInitSerialPort(
    __in PDEVICE_EXTENSION DevExt
    );

NTSTATUS INTERNAL
OemReadReport(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp,
    __out BOOLEAN          *fSentDown
    );

NTSTATUS INTERNAL
OemWriteReport(
    __in PDEVICE_EXTENSION DevExt,
    __in PIRP              Irp
    );

BOOLEAN INTERNAL
OemIsResyncDataValid(
    __in PDEVICE_EXTENSION DevExt
    );

NTSTATUS INTERNAL
OemNormalizeInputData(
    __in     PDEVICE_EXTENSION DevExt,
    __inout POEM_INPUT_REPORT InData
    );

IO_WORKITEM_ROUTINE WakeDeviceWorkItem;


NTSTATUS INTERNAL
SetDeviceReportDescriptor(
     __in PDEVICE_EXTENSION DevExt
     );

#endif  //ifndef _HIDPEN_H

