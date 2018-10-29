/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    pnp.c

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
DispatchPnp (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{   
    //
    // This defect is injected for the "PnpSurpriseRemove"
    // rule only for the x64 code path.
    //

    PIO_STACK_LOCATION stack;
    NTSTATUS           status = STATUS_SUCCESS;

    stack = IoGetCurrentIrpStackLocation(Irp);

    switch (stack->MinorFunction)
    {
    case IRP_MN_SURPRISE_REMOVAL:

        Irp->IoStatus.Status = STATUS_SUCCESS;

        IoSkipCurrentIrpStackLocation (Irp);

        status = IoCallDriver (((PFDO_DATA)DeviceObject->DeviceExtension)->NextLowerDriver, Irp);

        #if defined (_AMD64_)

        IoDetachDevice(((PFDO_DATA)DeviceObject->DeviceExtension)->NextLowerDriver);
       
        #endif

        return status;
    }

    Irp->IoStatus.Status = status;

    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    return status;
}
