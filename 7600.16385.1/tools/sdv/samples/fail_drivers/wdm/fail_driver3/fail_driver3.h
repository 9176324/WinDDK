/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_driver3.h

Environment:

    Kernel mode

--*/

#ifdef __cplusplus
extern "C" {
#endif
#include <ntddk.h>
#ifdef __cplusplus
}
#endif

typedef struct _FDO_DATA
{
    PDEVICE_OBJECT      Self;
    PDEVICE_OBJECT      UnderlyingPDO;
    PDEVICE_OBJECT      NextLowerDriver;
}  FDO_DATA, *PFDO_DATA;

#ifdef __cplusplus
extern "C"
#endif
DRIVER_INITIALIZE DriverEntry;

DRIVER_ADD_DEVICE DriverAddDevice;

__drv_dispatchType(IRP_MJ_CREATE)
DRIVER_DISPATCH DispatchCreate;

__drv_dispatchType(IRP_MJ_CLOSE)
DRIVER_DISPATCH DispatchClose;

__drv_dispatchType(IRP_MJ_CLEANUP)
DRIVER_DISPATCH DispatchCleanup;

__drv_dispatchType(IRP_MJ_READ)
DRIVER_DISPATCH DispatchRead;

__drv_dispatchType(IRP_MJ_WRITE)
DRIVER_DISPATCH DispatchWrite;

__drv_dispatchType(IRP_MJ_DEVICE_CONTROL)
DRIVER_DISPATCH DispatchDeviceControl;

__drv_dispatchType(IRP_MJ_INTERNAL_DEVICE_CONTROL)
DRIVER_DISPATCH DispatchInternalDeviceControl;

__drv_dispatchType(IRP_MJ_POWER)
DRIVER_DISPATCH DispatchPower;

__drv_dispatchType(IRP_MJ_SYSTEM_CONTROL)
DRIVER_DISPATCH DispatchSystemControl;

__drv_dispatchType(IRP_MJ_PNP)
DRIVER_DISPATCH DispatchPnp;

DRIVER_CANCEL CancelRoutine;

DRIVER_UNLOAD DriverUnload;

