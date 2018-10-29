/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_driver2.h

Environment:

    Kernel mode

--*/

#ifdef __cplusplus
extern "C" {
#endif
#include <wdm.h>
#ifdef __cplusplus
}
#endif

#include "fail_library2.h"

typedef struct _DRIVER_DEVICE_EXTENSION
{
	  PDEVICE_OBJECT   DeviceObject;
    PKSPIN_LOCK queueLock;
    KDPC dpc1;
    ULONG ControllerVector;  
    PKINTERRUPT InterruptObject;
}
DRIVER_DEVICE_EXTENSION,*PDRIVER_DEVICE_EXTENSION;

#ifdef __cplusplus
extern "C"
#endif
DRIVER_INITIALIZE DriverEntry;

DRIVER_ADD_DEVICE DriverAddDevice;

__drv_dispatchType(IRP_MJ_CREATE)
DRIVER_DISPATCH DispatchCreate;

__drv_dispatchType(IRP_MJ_READ)
DRIVER_DISPATCH DispatchRead;

__drv_dispatchType(IRP_MJ_WRITE)
DRIVER_DISPATCH DispatchWrite;

__drv_dispatchType(IRP_MJ_POWER)
DRIVER_DISPATCH DispatchPower;

__drv_dispatchType(IRP_MJ_SYSTEM_CONTROL)
DRIVER_DISPATCH DispatchSystemControl;

KSERVICE_ROUTINE InterruptServiceRoutine;

KDEFERRED_ROUTINE CustomDpcRoutine;

DRIVER_UNLOAD DriverUnload;

