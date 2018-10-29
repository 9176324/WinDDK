/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    wmi.c

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
DispatchSystemControl (
    IN  PDEVICE_OBJECT  DeviceObject,
    IN  PIRP            Irp
    )
{   
    //
    // This defect is injected for "IrqlIoPassive2"
    // rule only for the x64 code path.
    //

    PCONTROLLER_OBJECT controllerObject = NULL;
    #if defined (_AMD64_)

    KIRQL oldIrql ;

    #endif    

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    #if defined (_AMD64_)

    KeRaiseIrql(APC_LEVEL, &oldIrql);

    #endif

    controllerObject = IoCreateController(1000);  

    return STATUS_SUCCESS;
}
