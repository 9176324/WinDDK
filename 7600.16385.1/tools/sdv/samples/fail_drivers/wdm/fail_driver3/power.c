/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    power.c

Abstract:

    This is a sample driver that contains intentionally placed
    code defects in order to illustrate how Static Driver Verifier
    works. This driver is not functional and not intended as a 
    sample for real driver development projects.

Environment:

    Kernel mode

--*/

#include "fail_driver3.h"

NTSTATUS
DispatchPower (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{  
    //
    // This defect is injected for the "MarkIrpPending" 
    // rule only for the x64 code path.
    // 
    #if defined (_AMD64_)

    IoMarkIrpPending(Irp);

    #endif

    IoSkipCurrentIrpStackLocation(Irp);

    IoCallDriver(((PFDO_DATA) DeviceObject->DeviceExtension)->NextLowerDriver, Irp);

    return STATUS_SUCCESS;
}
