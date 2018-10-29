/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    fail_library2.h

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

VOID
LibraryRoutine1 (
    VOID
    );

NTSTATUS
LibraryRoutine2 (
    PVOID pointer
    );

NTSTATUS
LibraryRoutine3 ( 
    IN PKSPIN_LOCK queueLock
    );

NTSTATUS
LibraryRoutine4 (
    IN PDEVICE_OBJECT badObject,
    IN PIRP        Irp
    );

NTSTATUS
LibraryRoutine5 (
    VOID
    );

NTSTATUS
LibraryRoutine6 (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );
