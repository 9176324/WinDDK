/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_library2.c

Abstract:

    Template library that can be used together with a driver 
    for testing the seamless build feature of SDV via defect
    injection. This library is not functional and not intended
    as a sample for real software development projects. It only
    provides a skeleton for building a defective library.

Environment:

    Kernel mode

--*/

#include "fail_library2.h"

VOID
LibraryRoutine1 (
    VOID
    )
{
}

NTSTATUS
LibraryRoutine2 (
    IN PVOID pointer
    )
{
    ExFreePool(pointer);

    return STATUS_SUCCESS;
}

NTSTATUS
LibraryRoutine3 (
    IN PKSPIN_LOCK queueLock
    )
{
    KIRQL oldIrql;

    KeAcquireSpinLock(queueLock, &oldIrql);

    return STATUS_SUCCESS;
}

NTSTATUS
LibraryRoutine4 (
    IN PDEVICE_OBJECT badObject,
    IN PIRP        Irp
    )
{
    NTSTATUS status = IoCallDriver(badObject,Irp);

    return status;
}

NTSTATUS
LibraryRoutine5 (
    VOID
    )
{
    KIRQL oldIrql;

    IoAcquireCancelSpinLock(&oldIrql);

    return STATUS_SUCCESS;
}

NTSTATUS
LibraryRoutine6 (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
{

    NTSTATUS status = IoCallDriver(DeviceObject,Irp);
    
    return status;
}
