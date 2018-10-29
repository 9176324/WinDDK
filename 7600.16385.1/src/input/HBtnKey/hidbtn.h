/*++
    Copyright (c) 2000,2001 Microsoft Corporation

    Module Name:
        HidBtn.h

    Abstract:
        Contains definitions of all constants and data types for the
        serial pen hid driver.

    Environment:
        Kernel mode

--*/

#ifndef _HIDBTN_H
#define _HIDBTN_H

//
// Constants
//
#define HBTN_POOL_TAG                   'ntbH'

// dwfHBtn flag values
#define HBTNF_DEVICE_STARTED            0x00000001

//
// Macros
//
#define GET_MINIDRIVER_DEVICE_EXTENSION(DO) \
    ((PDEVICE_EXTENSION)(((PHID_DEVICE_EXTENSION)(DO)->DeviceExtension)->MiniDeviceExtension))
#define GET_NEXT_DEVICE_OBJECT(DO)          \
    (((PHID_DEVICE_EXTENSION)(DO)->DeviceExtension)->NextDeviceObject)
#define GET_PDO(DO)                         \
    (((PHID_DEVICE_EXTENSION)(DO)->DeviceExtension)->PhysicalDeviceObject)
//#define ARRAYSIZE(a)    (sizeof(a)/sizeof(a[0]))

//
// Type Definitions
//
typedef struct _DEVICE_EXTENSION
{
    ULONG          dwfHBtn;             //flags
    PDEVICE_OBJECT self;                //my device object
    PDEVICE_OBJECT LowerDevObj;         //lower device object
    IO_REMOVE_LOCK RemoveLock;          //to protect IRP_MN_REMOVE_DEVICE
    LIST_ENTRY     PendingIrpList;
    OEM_EXTENSION  OemExtension;
    KSPIN_LOCK     QueueLock;           //lock for cancel safe queue
    KSPIN_LOCK     DataLock;            //lock for protecting device extension access.
    IO_CSQ         IrpQueue;           //cancel safe queue
    
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

//
// Function prototypes
//

// hidbtn.c
DRIVER_INITIALIZE DriverEntry;

__drv_dispatchType(IRP_MJ_CREATE)
__drv_dispatchType(IRP_MJ_CLOSE)
DRIVER_DISPATCH HbtnCreateClose;

DRIVER_ADD_DEVICE HbtnAddDevice;

DRIVER_UNLOAD HbtnUnload;

// pnp.c
__drv_dispatchType(IRP_MJ_PNP)
DRIVER_DISPATCH HbtnPnp;


__drv_dispatchType(IRP_MJ_POWER)
DRIVER_DISPATCH HbtnPower;


NTSTATUS INTERNAL
SendSyncIrp(
    __in PDEVICE_OBJECT DevObj,
    __in PIRP           Irp,
    __in BOOLEAN        fCopyToNext
    );

IO_COMPLETION_ROUTINE IrpCompletion;

// ioctl.c
__drv_dispatchType(IRP_MJ_INTERNAL_DEVICE_CONTROL)
DRIVER_DISPATCH HbtnInternalIoctl;


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
    __in PIRP              Irp
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

// oembtn.c
NTSTATUS INTERNAL
OemAddDevice(
    __in PDEVICE_EXTENSION devext
    );

NTSTATUS INTERNAL
OemStartDevice(
    __in PDEVICE_EXTENSION devext,
    __in PIRP              Irp
    );

NTSTATUS INTERNAL
OemRemoveDevice(
    __in PDEVICE_EXTENSION devext,
    __in PIRP              Irp
    );

NTSTATUS INTERNAL
OemWriteReport(
    __in PDEVICE_EXTENSION devext,
    __in PIRP Irp
    );

VOID
HbtnInsertIrp (
    __in PIO_CSQ   csq,
    __in PIRP      Irp
    );

VOID
HbtnRemoveIrp(
    __in  PIO_CSQ csq,
    __in  PIRP    Irp
    );

PIRP
HbtnPeekNextIrp(
    __in  PIO_CSQ csq,
    __in  PIRP    Irp,
    __in  PVOID  PeekContext
    );

__drv_raisesIRQL(DISPATCH_LEVEL)
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HbtnAcquireLock(
    __in                                   PIO_CSQ csq,
    __out __drv_out_deref(__drv_savesIRQL) PKIRQL  Irql
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
HbtnReleaseLock(
    __in                              PIO_CSQ csq,
    __in __drv_in(__drv_restoresIRQL) KIRQL   Irql
    );

VOID
HbtnCompleteCancelledIrp(
    __in  PIO_CSQ             pCsq,
    __in  PIRP                Irp
    );

#define GET_DEV_EXT(csq) \
	CONTAINING_RECORD(csq, DEVICE_EXTENSION, IrpQueue)

#endif  //ifndef _HIDBTN_H

