/*++

Copyright (C) Microsoft Corporation, 1991 - 1999

Module Name:

    utils.c

Abstract:

    SCSI class driver routines

Environment:

    kernel mode only

Notes:


Revision History:

--*/

#include "classp.h"
#include "debug.h"

#ifdef DEBUG_USE_WPP
#include "utils.tmh"
#endif

BOOLEAN ClasspMyStringMatches(__in_z PCHAR StringToMatch, __in_z PCHAR TargetString);

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, ClassGetDeviceParameter)
    #pragma alloc_text(PAGE, ClassScanForSpecial)
    #pragma alloc_text(PAGE, ClassSetDeviceParameter)
    #pragma alloc_text(PAGE, ClasspMyStringMatches)
#endif



// custom string match -- careful!
BOOLEAN ClasspMyStringMatches(__in_z PCHAR StringToMatch, __in_z PCHAR TargetString)
{
    ULONG length;  // strlen returns an int, not size_t (!)
    PAGED_CODE();
    ASSERT(TargetString);
    // if no match requested, return TRUE
    if (StringToMatch == NULL) {
        return TRUE;
    }
    // cache the string length for efficiency
    length = strlen(StringToMatch);
    // ZERO-length strings may only match zero-length strings
    if (length == 0) {
        return (strlen(TargetString) == 0);
    }
    // strncmp returns zero if the strings match
    return (strncmp(StringToMatch, TargetString, length) == 0);
}


__drv_maxIRQL(PASSIVE_LEVEL)
VOID ClassGetDeviceParameter(
    __in PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    __in_opt PWSTR SubkeyName,
    __in PWSTR ParameterName,
    __inout PULONG ParameterValue  // also default value
    )
{
    NTSTATUS                 status;
    RTL_QUERY_REGISTRY_TABLE queryTable[2] = {0};
    HANDLE                   deviceParameterHandle;
    HANDLE                   deviceSubkeyHandle;
    ULONG                    defaultParameterValue;

    PAGED_CODE();

    //
    // open the given parameter
    //

    status = IoOpenDeviceRegistryKey(FdoExtension->LowerPdo,
                                     PLUGPLAY_REGKEY_DEVICE,
                                     KEY_READ,
                                     &deviceParameterHandle);

    if (NT_SUCCESS(status) && (SubkeyName != NULL)) {

        UNICODE_STRING subkeyName;
        OBJECT_ATTRIBUTES objectAttributes = {0};

        RtlInitUnicodeString(&subkeyName, SubkeyName);
        InitializeObjectAttributes(&objectAttributes,
                                   &subkeyName,
                                   OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                   deviceParameterHandle,
                                   NULL);

        status = ZwOpenKey(&deviceSubkeyHandle,
                           KEY_READ,
                           &objectAttributes);
        if (!NT_SUCCESS(status)) {
            ZwClose(deviceParameterHandle);
        }

    }

    if (NT_SUCCESS(status)) {

        defaultParameterValue = *ParameterValue;

        queryTable->Flags         = RTL_QUERY_REGISTRY_DIRECT | RTL_QUERY_REGISTRY_REQUIRED;
        queryTable->Name          = ParameterName;
        queryTable->EntryContext  = ParameterValue;
        queryTable->DefaultType   = REG_DWORD;
        queryTable->DefaultData   = NULL;
        queryTable->DefaultLength = 0;

        status = RtlQueryRegistryValues(RTL_REGISTRY_HANDLE,
                                        (PWSTR)(SubkeyName ?
                                                deviceSubkeyHandle :
                                                deviceParameterHandle),
                                        queryTable,
                                        NULL,
                                        NULL);
        if (!NT_SUCCESS(status)) {
            *ParameterValue = defaultParameterValue; // use default value
        }

        //
        // close what we open
        //

        if (SubkeyName) {
            ZwClose(deviceSubkeyHandle);
        }

        ZwClose(deviceParameterHandle);
    }

    if (!NT_SUCCESS(status)) {

        //
        // Windows 2000 SP3 uses the driver-specific key, so look in there
        //

        status = IoOpenDeviceRegistryKey(FdoExtension->LowerPdo,
                                         PLUGPLAY_REGKEY_DRIVER,
                                         KEY_READ,
                                         &deviceParameterHandle);

        if (NT_SUCCESS(status) && (SubkeyName != NULL)) {

            UNICODE_STRING subkeyName;
            OBJECT_ATTRIBUTES objectAttributes = {0};

            RtlInitUnicodeString(&subkeyName, SubkeyName);
            InitializeObjectAttributes(&objectAttributes,
                                       &subkeyName,
                                       OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                       deviceParameterHandle,
                                       NULL);

            status = ZwOpenKey(&deviceSubkeyHandle, KEY_READ, &objectAttributes);

            if (!NT_SUCCESS(status)) {
                ZwClose(deviceParameterHandle);
            }
        }

        if (NT_SUCCESS(status)) {

            defaultParameterValue = *ParameterValue;

            queryTable->Flags         = RTL_QUERY_REGISTRY_DIRECT | RTL_QUERY_REGISTRY_REQUIRED;
            queryTable->Name          = ParameterName;
            queryTable->EntryContext  = ParameterValue;
            queryTable->DefaultType   = REG_DWORD;
            queryTable->DefaultData   = NULL;
            queryTable->DefaultLength = 0;

            status = RtlQueryRegistryValues(RTL_REGISTRY_HANDLE,
                                            (PWSTR)(SubkeyName ?
                                                    deviceSubkeyHandle :
                                                    deviceParameterHandle),
                                            queryTable,
                                            NULL,
                                            NULL);
            if (NT_SUCCESS(status)) {

                //
                // Migrate the value over to the device-specific key
                //

                ClassSetDeviceParameter(FdoExtension, SubkeyName, ParameterName, *ParameterValue);

            } else {

                //
                // Use the default value
                //

                *ParameterValue = defaultParameterValue;
            }

            if (SubkeyName) {
                ZwClose(deviceSubkeyHandle);
            }

            ZwClose(deviceParameterHandle);
        }
    }

    return;

} // end ClassGetDeviceParameter()

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS ClassSetDeviceParameter(
    __in PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    __in_opt PWSTR SubkeyName,
    __in PWSTR ParameterName,
    __in ULONG ParameterValue)
{
    NTSTATUS                 status;
    HANDLE                   deviceParameterHandle;
    HANDLE                   deviceSubkeyHandle;

    PAGED_CODE();

    //
    // open the given parameter
    //

    status = IoOpenDeviceRegistryKey(FdoExtension->LowerPdo,
                                     PLUGPLAY_REGKEY_DEVICE,
                                     KEY_READ | KEY_WRITE,
                                     &deviceParameterHandle);

    if (NT_SUCCESS(status) && (SubkeyName != NULL)) {

        UNICODE_STRING subkeyName;
        OBJECT_ATTRIBUTES objectAttributes;

        RtlInitUnicodeString(&subkeyName, SubkeyName);
        InitializeObjectAttributes(&objectAttributes,
                                   &subkeyName,
                                   OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                   deviceParameterHandle,
                                   NULL);

        status = ZwCreateKey(&deviceSubkeyHandle,
                             KEY_READ | KEY_WRITE,
                             &objectAttributes,
                             0, NULL, 0, NULL);
        if (!NT_SUCCESS(status)) {
            ZwClose(deviceParameterHandle);
        }

    }

    if (NT_SUCCESS(status)) {

        status = RtlWriteRegistryValue(
            RTL_REGISTRY_HANDLE,
            (PWSTR) (SubkeyName ?
                     deviceSubkeyHandle :
                     deviceParameterHandle),
            ParameterName,
            REG_DWORD,
            &ParameterValue,
            sizeof(ULONG));

        //
        // close what we open
        //

        if (SubkeyName) {
            ZwClose(deviceSubkeyHandle);
        }

        ZwClose(deviceParameterHandle);
    }

    return status;

} // end ClassSetDeviceParameter()


/*
 *  ClassScanForSpecial
 *
 *      This routine was written to simplify scanning for special
 *      hardware based upon id strings.  it does not check the registry.
 */

__drv_maxIRQL(PASSIVE_LEVEL)
VOID ClassScanForSpecial(
    __in PFUNCTIONAL_DEVICE_EXTENSION FdoExtension,
    __in CLASSPNP_SCAN_FOR_SPECIAL_INFO DeviceList[],
    __in PCLASS_SCAN_FOR_SPECIAL_HANDLER Function)
{
    PSTORAGE_DEVICE_DESCRIPTOR deviceDescriptor;
    PUCHAR vendorId;
    PUCHAR productId;
    PUCHAR productRevision;
    UCHAR nullString[] = "";
    ULONG j;

    PAGED_CODE();
    ASSERT(DeviceList);
    ASSERT(Function);

    deviceDescriptor = FdoExtension->DeviceDescriptor;

    if (DeviceList == NULL) {
        return;
    }
    if (Function == NULL) {
        return;
    }

    //
    // SCSI sets offsets to -1, ATAPI sets to 0.  check for both.
    //

    if (deviceDescriptor->VendorIdOffset != 0 &&
        deviceDescriptor->VendorIdOffset != -1) {
        vendorId = ((PUCHAR)deviceDescriptor);
        vendorId += deviceDescriptor->VendorIdOffset;
    } else {
        vendorId = nullString;
    }
    if (deviceDescriptor->ProductIdOffset != 0 &&
        deviceDescriptor->ProductIdOffset != -1) {
        productId = ((PUCHAR)deviceDescriptor);
        productId += deviceDescriptor->ProductIdOffset;
    } else {
        productId = nullString;
    }
    if (deviceDescriptor->ProductRevisionOffset != 0 &&
        deviceDescriptor->ProductRevisionOffset != -1) {
        productRevision = ((PUCHAR)deviceDescriptor);
        productRevision += deviceDescriptor->ProductRevisionOffset;
    } else {
        productRevision = nullString;
    }

    //
    // loop while the device list is valid (not null-filled)
    //

    for (;(DeviceList->VendorId        != NULL ||
           DeviceList->ProductId       != NULL ||
           DeviceList->ProductRevision != NULL);DeviceList++) {

        if (ClasspMyStringMatches(DeviceList->VendorId,        vendorId) &&
            ClasspMyStringMatches(DeviceList->ProductId,       productId) &&
            ClasspMyStringMatches(DeviceList->ProductRevision, productRevision)
            ) {

            TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_INIT, "ClasspScanForSpecialByInquiry: Found matching "
                        "controller Ven: %s Prod: %s Rev: %s\n",
                        vendorId, productId, productRevision));

            //
            // pass the context to the call back routine and exit
            //

            (Function)(FdoExtension, DeviceList->Data);

            //
            // for CHK builds, try to prevent wierd stacks by having a debug
            // print here. it's a hack, but i know of no other way to prevent
            // the stack from being wrong.
            //

            TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_INIT, "ClasspScanForSpecialByInquiry: "
                        "completed callback\n"));
            return;

        } // else the strings did not match

    } // none of the devices matched.

    TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_INIT, "ClasspScanForSpecialByInquiry: no match found for %p\n",
                FdoExtension->DeviceObject));
    return;

} // end ClasspScanForSpecialByInquiry()


//
// In order to provide better performance without the need to reboot,
// we need to implement a self-adjusting method to set and clear the
// srb flags based upon current performance.
//
// whenever there is an error, immediately grab the spin lock.  the
// MP perf hit here is acceptable, since we're in an error path.  this
// is also neccessary because we are guaranteed to be modifying the
// SRB flags here, setting SuccessfulIO to zero, and incrementing the
// actual error count (which is always done within this spinlock).
//
// whenever there is no error, increment a counter.  if there have been
// errors on the device, and we've enabled dynamic perf, *and* we've
// just crossed the perf threshhold, then grab the spin lock and
// double check that the threshhold has, indeed been hit(*). then
// decrement the error count, and if it's dropped sufficiently, undo
// some of the safety changes made in the SRB flags due to the errors.
//
// * this works in all cases.  even if lots of ios occur after the
//   previous guy went in and cleared the successfulio counter, that
//   just means that we've hit the threshhold again, and so it's proper
//   to run the inner loop again.
//

VOID
ClasspPerfIncrementErrorCount(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension
    )
{
    PCLASS_PRIVATE_FDO_DATA fdoData = FdoExtension->PrivateFdoData;
    KIRQL oldIrql;
    ULONG errors;

    KeAcquireSpinLock(&fdoData->SpinLock, &oldIrql);

    fdoData->Perf.SuccessfulIO = 0; // implicit interlock
    errors = InterlockedIncrement(&FdoExtension->ErrorCount);

    if (errors >= CLASS_ERROR_LEVEL_1) {

        //
        // If the error count has exceeded the error limit, then disable
        // any tagged queuing, multiple requests per lu queueing
        // and sychronous data transfers.
        //
        // Clearing the no queue freeze flag prevents the port driver
        // from sending multiple requests per logical unit.
        //

        CLEAR_FLAG(FdoExtension->SrbFlags, SRB_FLAGS_NO_QUEUE_FREEZE);
        CLEAR_FLAG(FdoExtension->SrbFlags, SRB_FLAGS_QUEUE_ACTION_ENABLE);

        SET_FLAG(FdoExtension->SrbFlags, SRB_FLAGS_DISABLE_SYNCH_TRANSFER);

        TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_GENERAL, "ClasspPerfIncrementErrorCount: "
                    "Too many errors; disabling tagged queuing and "
                    "synchronous data tranfers.\n"));

    }

    if (errors >= CLASS_ERROR_LEVEL_2) {

        //
        // If a second threshold is reached, disable disconnects.
        //

        SET_FLAG(FdoExtension->SrbFlags, SRB_FLAGS_DISABLE_DISCONNECT);
        TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_GENERAL, "ClasspPerfIncrementErrorCount: "
                    "Too many errors; disabling disconnects.\n"));
    }

    KeReleaseSpinLock(&fdoData->SpinLock, oldIrql);
    return;
}

VOID
ClasspPerfIncrementSuccessfulIo(
    IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension
    )
{
    PCLASS_PRIVATE_FDO_DATA fdoData = FdoExtension->PrivateFdoData;
    KIRQL oldIrql;
    ULONG errors;
    ULONG succeeded = 0;

    //
    // don't take a hit from the interlocked op unless we're in
    // a degraded state and we've got a threshold to hit.
    //

    if (FdoExtension->ErrorCount == 0) {
        return;
    }

    if (fdoData->Perf.ReEnableThreshhold == 0) {
        return;
    }

    succeeded = InterlockedIncrement(&fdoData->Perf.SuccessfulIO);
    if (succeeded < fdoData->Perf.ReEnableThreshhold) {
        return;
    }

    //
    // if we hit the threshold, grab the spinlock and verify we've
    // actually done so.  this allows us to ignore the spinlock 99%
    // of the time.
    //

    KeAcquireSpinLock(&fdoData->SpinLock, &oldIrql);

    //
    // re-read the value, so we don't run this multiple times
    // for a single threshhold being hit.  this keeps errorcount
    // somewhat useful.
    //

    succeeded = fdoData->Perf.SuccessfulIO;

    if ((FdoExtension->ErrorCount != 0) &&
        (fdoData->Perf.ReEnableThreshhold <= succeeded)
        ) {

        fdoData->Perf.SuccessfulIO = 0; // implicit interlock

        ASSERT(FdoExtension->ErrorCount > 0);
        errors = InterlockedDecrement(&FdoExtension->ErrorCount);

        //
        // note: do in reverse order of the sets "just in case"
        //

        if (errors < CLASS_ERROR_LEVEL_2) {
            if (errors == CLASS_ERROR_LEVEL_2 - 1) {
                TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_GENERAL, "ClasspPerfIncrementSuccessfulIo: "
                            "Error level 2 no longer required.\n"));
            }
            if (!TEST_FLAG(fdoData->Perf.OriginalSrbFlags,
                           SRB_FLAGS_DISABLE_DISCONNECT)) {
                CLEAR_FLAG(FdoExtension->SrbFlags,
                           SRB_FLAGS_DISABLE_DISCONNECT);
            }
        }

        if (errors < CLASS_ERROR_LEVEL_1) {
            if (errors == CLASS_ERROR_LEVEL_1 - 1) {
                TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_GENERAL, "ClasspPerfIncrementSuccessfulIo: "
                            "Error level 1 no longer required.\n"));
            }
            if (!TEST_FLAG(fdoData->Perf.OriginalSrbFlags,
                           SRB_FLAGS_DISABLE_SYNCH_TRANSFER)) {
                CLEAR_FLAG(FdoExtension->SrbFlags,
                           SRB_FLAGS_DISABLE_SYNCH_TRANSFER);
            }
            if (TEST_FLAG(fdoData->Perf.OriginalSrbFlags,
                          SRB_FLAGS_QUEUE_ACTION_ENABLE)) {
                SET_FLAG(FdoExtension->SrbFlags,
                         SRB_FLAGS_QUEUE_ACTION_ENABLE);
            }
            if (TEST_FLAG(fdoData->Perf.OriginalSrbFlags,
                          SRB_FLAGS_NO_QUEUE_FREEZE)) {
                SET_FLAG(FdoExtension->SrbFlags,
                         SRB_FLAGS_NO_QUEUE_FREEZE);
            }
        }
    } // end of threshhold definitely being hit for first time

    KeReleaseSpinLock(&fdoData->SpinLock, oldIrql);
    return;
}


PMDL BuildDeviceInputMdl(PVOID Buffer, ULONG BufferLen)
{
    PMDL mdl;

    mdl = IoAllocateMdl(Buffer, BufferLen, FALSE, FALSE, NULL);
    if (mdl){
        try {
            /*
             *  We are reading from the device.
             *  Therefore, the device is WRITING to the locked memory.
             *  So we request IoWriteAccess.
             */
            MmProbeAndLockPages(mdl, KernelMode, IoWriteAccess);

        } except(EXCEPTION_EXECUTE_HANDLER) {
            NTSTATUS status = GetExceptionCode();

            TracePrint((TRACE_LEVEL_WARNING, TRACE_FLAG_INIT, "BuildReadMdl: MmProbeAndLockPages failed with %xh.", status));
            IoFreeMdl(mdl);
            mdl = NULL;
        }
    }
    else {
        TracePrint((TRACE_LEVEL_WARNING, TRACE_FLAG_INIT, "BuildReadMdl: IoAllocateMdl failed"));
    }

    return mdl;
}


VOID FreeDeviceInputMdl(PMDL Mdl)
{
    MmUnlockPages(Mdl);
    IoFreeMdl(Mdl);
}


#if 0
    VOID
    ClasspPerfResetCounters(
        IN PFUNCTIONAL_DEVICE_EXTENSION FdoExtension
        )
    {
        PCLASS_PRIVATE_FDO_DATA fdoData = FdoExtension->PrivateFdoData;
        KIRQL oldIrql;

        KeAcquireSpinLock(&fdoData->SpinLock, &oldIrql);
        TracePrint((TRACE_LEVEL_ERROR, TRACE_FLAG_GENERAL, "ClasspPerfResetCounters: "
                    "Resetting all perf counters.\n"));
        fdoData->Perf.SuccessfulIO = 0;
        FdoExtension->ErrorCount = 0;

        if (!TEST_FLAG(fdoData->Perf.OriginalSrbFlags,
                       SRB_FLAGS_DISABLE_DISCONNECT)) {
            CLEAR_FLAG(FdoExtension->SrbFlags,
                       SRB_FLAGS_DISABLE_DISCONNECT);
        }
        if (!TEST_FLAG(fdoData->Perf.OriginalSrbFlags,
                       SRB_FLAGS_DISABLE_SYNCH_TRANSFER)) {
            CLEAR_FLAG(FdoExtension->SrbFlags,
                       SRB_FLAGS_DISABLE_SYNCH_TRANSFER);
        }
        if (TEST_FLAG(fdoData->Perf.OriginalSrbFlags,
                      SRB_FLAGS_QUEUE_ACTION_ENABLE)) {
            SET_FLAG(FdoExtension->SrbFlags,
                     SRB_FLAGS_QUEUE_ACTION_ENABLE);
        }
        if (TEST_FLAG(fdoData->Perf.OriginalSrbFlags,
                      SRB_FLAGS_NO_QUEUE_FREEZE)) {
            SET_FLAG(FdoExtension->SrbFlags,
                     SRB_FLAGS_NO_QUEUE_FREEZE);
        }
        KeReleaseSpinLock(&fdoData->SpinLock, oldIrql);
        return;
    }
#endif


/*++

ClasspDuidGetDeviceIdProperty

Routine Description:

    Add StorageDeviceIdProperty to the device unique ID structure.

Arguments:

    DeviceObject - a pointer to the device object
    Irp - a pointer to the I/O request packet

Return Value:

    Status Code

--*/
NTSTATUS
ClasspDuidGetDeviceIdProperty(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    PCOMMON_DEVICE_EXTENSION commonExtension = DeviceObject->DeviceExtension;
    PSTORAGE_DEVICE_ID_DESCRIPTOR deviceIdDescriptor = NULL;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PSTORAGE_DESCRIPTOR_HEADER descHeader;
    PSTORAGE_DEVICE_UNIQUE_IDENTIFIER storageDuid;
    PUCHAR dest;

    STORAGE_PROPERTY_ID propertyId = StorageDeviceIdProperty;

    NTSTATUS status;

    ULONG queryLength;
    ULONG offset;

    //
    // Get the VPD page 83h data.
    //

    status = ClassGetDescriptor(commonExtension->LowerDeviceObject,
                                &propertyId,
                                &deviceIdDescriptor);

    if (!NT_SUCCESS(status) || !deviceIdDescriptor) {
        goto FnExit;
    }

    queryLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
    descHeader = Irp->AssociatedIrp.SystemBuffer;

    //
    // Adjust required size and potential destination location.
    //

    offset = descHeader->Size;
    dest = (PUCHAR)descHeader + offset;

    descHeader->Size += deviceIdDescriptor->Size;

    if (queryLength < descHeader->Size) {

        //
        // Output buffer is too small.  Return error and make sure
        // the caller gets info about required buffer size.
        //

        Irp->IoStatus.Information = sizeof(STORAGE_DESCRIPTOR_HEADER);
        status = STATUS_BUFFER_OVERFLOW;
        goto FnExit;
    }

    storageDuid = Irp->AssociatedIrp.SystemBuffer;
    storageDuid->StorageDeviceIdOffset = offset;

    RtlCopyMemory(dest,
                  deviceIdDescriptor,
                  deviceIdDescriptor->Size);

    Irp->IoStatus.Information = storageDuid->Size;
    status = STATUS_SUCCESS;

FnExit:

    FREE_POOL(deviceIdDescriptor);

    return status;
}



/*++

ClasspDuidGetDeviceProperty

Routine Description:

    Add StorageDeviceProperty to the device unique ID structure.

Arguments:

    DeviceObject - a pointer to the device object
    Irp - a pointer to the I/O request packet

Return Value:

    Status Code

--*/
NTSTATUS
ClasspDuidGetDeviceProperty(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )
{
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = DeviceObject->DeviceExtension;
    PSTORAGE_DEVICE_DESCRIPTOR deviceDescriptor = fdoExtension->DeviceDescriptor;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PSTORAGE_DESCRIPTOR_HEADER descHeader;
    PSTORAGE_DEVICE_UNIQUE_IDENTIFIER storageDuid;
    PUCHAR dest;

    NTSTATUS status = STATUS_NOT_FOUND;

    ULONG queryLength;
    ULONG offset;

    //
    // Use the StorageDeviceProperty already cached in the device extension.
    //

    if (!deviceDescriptor) {
        goto FnExit;
    }

    queryLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
    descHeader = Irp->AssociatedIrp.SystemBuffer;

    //
    // Use this info only if serial number is available.
    //

    if (deviceDescriptor->SerialNumberOffset == 0) {
        goto FnExit;
    }

    //
    // Adjust required size and potential destination location.
    //

    offset = descHeader->Size;
    dest = (PUCHAR)descHeader + offset;

    descHeader->Size += deviceDescriptor->Size;

    if (queryLength < descHeader->Size) {

        //
        // Output buffer is too small.  Return error and make sure
        // the caller get info about required buffer size.
        //

        Irp->IoStatus.Information = sizeof(STORAGE_DESCRIPTOR_HEADER);
        status = STATUS_BUFFER_OVERFLOW;
        goto FnExit;
    }

    storageDuid = Irp->AssociatedIrp.SystemBuffer;
    storageDuid->StorageDeviceOffset = offset;

    RtlCopyMemory(dest,
                  deviceDescriptor,
                  deviceDescriptor->Size);

    Irp->IoStatus.Information = storageDuid->Size;
    status = STATUS_SUCCESS;

FnExit:

    return status;
}


/*++

ClasspDuidGetDriveLayout

Routine Description:

    Add drive layout signature to the device unique ID structure.
    Layout signature is only added for disk-type devices.

Arguments:

    DeviceObject - a pointer to the device object
    Irp - a pointer to the I/O request packet

Return Value:

    Status Code

--*/
NTSTATUS
ClasspDuidGetDriveLayout(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )
{
    PDRIVE_LAYOUT_INFORMATION_EX layoutEx = NULL;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PSTORAGE_DESCRIPTOR_HEADER descHeader;
    PSTORAGE_DEVICE_UNIQUE_IDENTIFIER storageDuid;
    PSTORAGE_DEVICE_LAYOUT_SIGNATURE driveLayoutSignature;

    NTSTATUS status = STATUS_NOT_FOUND;

    ULONG queryLength;
    ULONG offset;

    //
    // Only process disk-type devices.
    //

    if (DeviceObject->DeviceType != FILE_DEVICE_DISK) {
        goto FnExit;
    }

    //
    // Get current partition table and process only if GPT
    // or MBR layout.
    //

    status = IoReadPartitionTableEx(DeviceObject, &layoutEx);

    if (!NT_SUCCESS(status)) {
        status = STATUS_NOT_FOUND;
        goto FnExit;
    }

    if (layoutEx->PartitionStyle != PARTITION_STYLE_GPT &&
        layoutEx->PartitionStyle != PARTITION_STYLE_MBR) {
        status = STATUS_NOT_FOUND;
        goto FnExit;
    }

    queryLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
    descHeader = Irp->AssociatedIrp.SystemBuffer;

    //
    // Adjust required size and potential destination location.
    //

    offset = descHeader->Size;
    driveLayoutSignature = (PSTORAGE_DEVICE_LAYOUT_SIGNATURE)((PUCHAR)descHeader + offset);

    descHeader->Size += sizeof(STORAGE_DEVICE_LAYOUT_SIGNATURE);

    if (queryLength < descHeader->Size) {

        //
        // Output buffer is too small.  Return error and make sure
        // the caller get info about required buffer size.
        //

        Irp->IoStatus.Information = sizeof(STORAGE_DESCRIPTOR_HEADER);
        status = STATUS_BUFFER_OVERFLOW;
        goto FnExit;
    }

    storageDuid = Irp->AssociatedIrp.SystemBuffer;

    driveLayoutSignature->Size = sizeof(STORAGE_DEVICE_LAYOUT_SIGNATURE);
    driveLayoutSignature->Version = DUID_VERSION_1;

    if (layoutEx->PartitionStyle == PARTITION_STYLE_MBR) {

        driveLayoutSignature->Mbr = TRUE;

        RtlCopyMemory(&driveLayoutSignature->DeviceSpecific.MbrSignature,
                      &layoutEx->Mbr.Signature,
                      sizeof(layoutEx->Mbr.Signature));

    } else {

        driveLayoutSignature->Mbr = FALSE;

        RtlCopyMemory(&driveLayoutSignature->DeviceSpecific.GptDiskId,
                      &layoutEx->Gpt.DiskId,
                      sizeof(layoutEx->Gpt.DiskId));
    }

    storageDuid->DriveLayoutSignatureOffset = offset;

    Irp->IoStatus.Information = storageDuid->Size;
    status = STATUS_SUCCESS;


FnExit:

    FREE_POOL(layoutEx);

    return status;
}


/*++

ClasspDuidQueryProperty

Routine Description:

    Handles IOCTL_STORAGE_QUERY_PROPERTY for device unique ID requests
    (when PropertyId is StorageDeviceUniqueIdProperty).

Arguments:

    DeviceObject - a pointer to the device object
    Irp - a pointer to the I/O request packet

Return Value:

    Status Code

--*/
NTSTATUS
ClasspDuidQueryProperty(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )
{
    PSTORAGE_PROPERTY_QUERY query =  Irp->AssociatedIrp.SystemBuffer;
    PSTORAGE_DESCRIPTOR_HEADER descHeader;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);

    NTSTATUS status;

    ULONG outLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    BOOLEAN includeOptionalIds;
    BOOLEAN overflow = FALSE;
    BOOLEAN infoFound = FALSE;
    BOOLEAN useStatus = TRUE;   // Use the status directly instead of relying on overflow and infoFound flags.

    //
    // Must run at less then dispatch.
    //

    if (KeGetCurrentIrql() >= DISPATCH_LEVEL) {

        ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);
        status = STATUS_INVALID_LEVEL;
        goto FnExit;
    }

    //
    // Check proper query type.
    //

    if (query->QueryType == PropertyExistsQuery) {
        Irp->IoStatus.Information = 0;
        status = STATUS_SUCCESS;
        goto FnExit;
    }

    if (query->QueryType != PropertyStandardQuery) {
        status = STATUS_NOT_SUPPORTED;
        goto FnExit;
    }

    //
    // Check AdditionalParameters validity.
    //

    if (query->AdditionalParameters[0] == DUID_INCLUDE_SOFTWARE_IDS) {
        includeOptionalIds = TRUE;
    } else if (query->AdditionalParameters[0] == DUID_HARDWARE_IDS_ONLY) {
        includeOptionalIds = FALSE;
    } else {
        status = STATUS_INVALID_PARAMETER;
        goto FnExit;
    }

    //
    // Verify output parameters.
    //

    if (outLength < sizeof(STORAGE_DESCRIPTOR_HEADER)) {

        status = STATUS_INFO_LENGTH_MISMATCH;
        goto FnExit;
    }

    //
    // From this point forward the status depends on the overflow
    // and infoFound flags.
    //

    useStatus = FALSE;

    descHeader = Irp->AssociatedIrp.SystemBuffer;
    RtlZeroMemory(descHeader, outLength);

    descHeader->Version = DUID_VERSION_1;
    descHeader->Size = sizeof(STORAGE_DEVICE_UNIQUE_IDENTIFIER);

    //
    // Try to build device unique id from StorageDeviceIdProperty.
    //

    status = ClasspDuidGetDeviceIdProperty(DeviceObject,
                                           Irp);

    if (status == STATUS_BUFFER_OVERFLOW) {
        overflow = TRUE;
    }

    if (NT_SUCCESS(status)) {
        infoFound = TRUE;
    }

    //
    // Try to build device unique id from StorageDeviceProperty.
    //

    status = ClasspDuidGetDeviceProperty(DeviceObject,
                                         Irp);

    if (status == STATUS_BUFFER_OVERFLOW) {
        overflow = TRUE;
    }

    if (NT_SUCCESS(status)) {
        infoFound = TRUE;
    }

    //
    // The following portion is optional and only included if the
    // caller requested software IDs.
    //

    if (!includeOptionalIds) {
        goto FnExit;
    }

    //
    // Try to build device unique id from drive layout signature (disk
    // devices only).
    //

    status = ClasspDuidGetDriveLayout(DeviceObject,
                                      Irp);

    if (status == STATUS_BUFFER_OVERFLOW) {
        overflow = TRUE;
    }

    if (NT_SUCCESS(status)) {
        infoFound = TRUE;
    }

FnExit:

    if (!useStatus) {

        //
        // Return overflow, success, or a generic error.
        //

        if (overflow) {

            //
            // If output buffer is STORAGE_DESCRIPTOR_HEADER, then return
            // success to the user.  Otherwise, send an error so the user
            // knows a larger buffer is required.
            //

            if (outLength == sizeof(STORAGE_DESCRIPTOR_HEADER)) {
                status = STATUS_SUCCESS;
                Irp->IoStatus.Information = sizeof(STORAGE_DESCRIPTOR_HEADER);
            } else {
                status = STATUS_BUFFER_OVERFLOW;
            }

        } else if (infoFound) {
            status = STATUS_SUCCESS;

            //
            // Exercise the compare routine.  This should always succeed.
            //

            ASSERT(DuidExactMatch == CompareStorageDuids(Irp->AssociatedIrp.SystemBuffer,
                                                         Irp->AssociatedIrp.SystemBuffer));

        } else {
            status = STATUS_NOT_FOUND;
        }
    }

    Irp->IoStatus.Status = status;

    ClassReleaseRemoveLock(DeviceObject, Irp);
    ClassCompleteRequest(DeviceObject, Irp, IO_NO_INCREMENT);

    return status;
}

/*++////////////////////////////////////////////////////////////////////////////

ClasspWriteCacheProperty()

Routine Description:

    This routine reads the caching mode page from the device to
    build the Write Cache property page.

Arguments:

    DeviceObject - The device object to handle this irp

    Irp - The IRP for this request

    Srb - SRB allocated by the dispatch routine

Return Value:

--*/

NTSTATUS ClasspWriteCacheProperty(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PSCSI_REQUEST_BLOCK Srb
    )
{
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = DeviceObject->DeviceExtension;
    PSTORAGE_WRITE_CACHE_PROPERTY writeCache;
    PSTORAGE_PROPERTY_QUERY query =  Irp->AssociatedIrp.SystemBuffer;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PMODE_PARAMETER_HEADER modeData = NULL;
    PMODE_CACHING_PAGE pageData = NULL;
    ULONG length, information = 0;
    NTSTATUS status;

    //
    // Must run at less then dispatch.
    //

    if (KeGetCurrentIrql() >= DISPATCH_LEVEL) {

        ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);
        status = STATUS_INVALID_LEVEL;
        goto WriteCacheExit;
    }

    //
    // Check proper query type.
    //

    if (query->QueryType == PropertyExistsQuery) {
        status = STATUS_SUCCESS;
        goto WriteCacheExit;
    }

    if (query->QueryType != PropertyStandardQuery) {
        status = STATUS_NOT_SUPPORTED;
        goto WriteCacheExit;
    }

    length = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    if (length < sizeof(STORAGE_DESCRIPTOR_HEADER)) {
        status = STATUS_INFO_LENGTH_MISMATCH;
        goto WriteCacheExit;
    }

    writeCache = (PSTORAGE_WRITE_CACHE_PROPERTY) Irp->AssociatedIrp.SystemBuffer;
    RtlZeroMemory(writeCache, length);

    //
    // Set version and required size.
    //

    writeCache->Version = sizeof(STORAGE_WRITE_CACHE_PROPERTY);
    writeCache->Size = sizeof(STORAGE_WRITE_CACHE_PROPERTY);

    if (length < sizeof(STORAGE_WRITE_CACHE_PROPERTY)) {
        information = sizeof(STORAGE_DESCRIPTOR_HEADER);
        status = STATUS_SUCCESS;
        goto WriteCacheExit;
    }

    //
    // Set known values
    //

    writeCache->NVCacheEnabled = FALSE;
    writeCache->UserDefinedPowerProtection = TEST_FLAG(fdoExtension->DeviceFlags, DEV_POWER_PROTECTED);

    //
    // Check for flush cache support by sending a sync cache command
    // to the device.
    //

    //
    // Set timeout value and mark the request as not being a tagged request.
    //
    Srb->Length = SCSI_REQUEST_BLOCK_SIZE;
    Srb->TimeOutValue = fdoExtension->TimeOutValue * 4;
    Srb->QueueTag = SP_UNTAGGED;
    Srb->QueueAction = SRB_SIMPLE_TAG_REQUEST;
    Srb->SrbFlags = fdoExtension->SrbFlags;

    Srb->Function = SRB_FUNCTION_EXECUTE_SCSI;
    Srb->CdbLength = 10;

    Srb->Cdb[0] = SCSIOP_SYNCHRONIZE_CACHE;

    status = ClassSendSrbSynchronous(DeviceObject,
                                     Srb,
                                     NULL,
                                     0,
                                     TRUE);
    if (NT_SUCCESS(status)) {
        writeCache->FlushCacheSupported = TRUE;
    } else {
        //
        // Device does not support sync cache
        //

        TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_IOCTL, "ClasspWriteCacheProperty: Synchronize cache failed with status 0x%X\n", status));
        writeCache->FlushCacheSupported = FALSE;
        //
        // Reset the status if there was any failure
        //
        status = STATUS_SUCCESS;
    }

    modeData = ExAllocatePoolWithTag(NonPagedPoolCacheAligned,
                                     MODE_PAGE_DATA_SIZE,
                                     CLASS_TAG_MODE_DATA);

    if (modeData == NULL) {
        TracePrint((TRACE_LEVEL_WARNING, TRACE_FLAG_IOCTL, "ClasspWriteCacheProperty: Unable to allocate mode data buffer\n"));
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto WriteCacheExit;
    }

    RtlZeroMemory(modeData, MODE_PAGE_DATA_SIZE);

    length = ClassModeSense(DeviceObject,
                            (PUCHAR) modeData,
                            MODE_PAGE_DATA_SIZE,
                            MODE_PAGE_CACHING);

    if (length < sizeof(MODE_PARAMETER_HEADER)) {

        //
        // Retry the request in case of a check condition.
        //

        length = ClassModeSense(DeviceObject,
                                (PUCHAR) modeData,
                                MODE_PAGE_DATA_SIZE,
                                MODE_PAGE_CACHING);

        if (length < sizeof(MODE_PARAMETER_HEADER)) {
            TracePrint((TRACE_LEVEL_WARNING, TRACE_FLAG_IOCTL, "ClasspWriteCacheProperty: Mode Sense failed\n"));
            status = STATUS_IO_DEVICE_ERROR;
            goto WriteCacheExit;
        }
    }

    //
    // If the length is greater than length indicated by the mode data reset
    // the data to the mode data.
    //

    if (length > (ULONG) (modeData->ModeDataLength + 1)) {
        length = modeData->ModeDataLength + 1;
    }

    //
    // Look for caching page in the returned mode page data.
    //

    pageData = ClassFindModePage((PUCHAR) modeData,
                                 length,
                                 MODE_PAGE_CACHING,
                                 TRUE);

    //
    // Check if valid caching page exists.
    //

    if (pageData == NULL) {

        TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_IOCTL, "ClasspWriteCacheProperty: Unable to find caching mode page.\n"));
        //
        // Set write cache value as unknown.
        //
        writeCache->WriteCacheEnabled = WriteCacheEnableUnknown;
        writeCache->WriteCacheType = WriteCacheTypeUnknown;
    } else {
        writeCache->WriteCacheEnabled = pageData->WriteCacheEnable ?
                                            WriteCacheEnabled : WriteCacheDisabled;

        writeCache->WriteCacheType = pageData->WriteCacheEnable ?
                                            WriteCacheTypeWriteBack : WriteCacheTypeUnknown;
    }

    //
    // Check write through support. If the device previously failed a write request
    // with FUA bit is set, then CLASS_SPECIAL_FUA_NOT_SUPPORTED will be set,
    // which means write through is not support by the device.
    //

    if ((modeData->DeviceSpecificParameter & MODE_DSP_FUA_SUPPORTED) &&
        (!TEST_FLAG(fdoExtension->ScanForSpecialFlags, CLASS_SPECIAL_FUA_NOT_SUPPORTED))) {
        writeCache->WriteThroughSupported = WriteThroughSupported;
    } else {
        writeCache->WriteThroughSupported = WriteThroughNotSupported;
    }

    //
    // Get the changeable caching mode page and check write cache is changeable.
    //

    RtlZeroMemory(modeData, MODE_PAGE_DATA_SIZE);

    length = ClassPageControlModeSense(DeviceObject,
                            (PUCHAR) modeData,
                            MODE_PAGE_DATA_SIZE,
                            MODE_PAGE_CACHING,
                            MODE_SENSE_CHANGEABLE_VALUES);

    if (length < sizeof(MODE_PARAMETER_HEADER)) {

        //
        // Retry the request in case of a check condition.
        //

        length = ClassPageControlModeSense(DeviceObject,
                                (PUCHAR) modeData,
                                MODE_PAGE_DATA_SIZE,
                                MODE_PAGE_CACHING,
                                MODE_SENSE_CHANGEABLE_VALUES);

        if (length < sizeof(MODE_PARAMETER_HEADER)) {

            TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_IOCTL, "ClasspWriteCacheProperty: Mode Sense failed\n"));

            //
            // If the device fails to return changeable pages, then
            // set the write cache changeable value to unknown.
            //

            writeCache->WriteCacheChangeable = WriteCacheChangeUnknown;
            information = sizeof(STORAGE_WRITE_CACHE_PROPERTY);
            goto WriteCacheExit;
        }
    }

    //
    // If the length is greater than length indicated by the mode data reset
    // the data to the mode data.
    //

    if (length > (ULONG) (modeData->ModeDataLength + 1)) {
        length = modeData->ModeDataLength + 1;
    }

    //
    // Look for caching page in the returned mode page data.
    //

    pageData = ClassFindModePage((PUCHAR) modeData,
                                 length,
                                 MODE_PAGE_CACHING,
                                 TRUE);
    //
    // Check if valid caching page exists.
    //

    if (pageData == NULL) {
        TracePrint((TRACE_LEVEL_INFORMATION, TRACE_FLAG_IOCTL, "ClasspWriteCacheProperty: Unable to find caching mode page.\n"));
        //
        // Set write cache changeable value to unknown.
        //
        writeCache->WriteCacheChangeable = WriteCacheChangeUnknown;
    } else {
        writeCache->WriteCacheChangeable = pageData->WriteCacheEnable ?
                                            WriteCacheChangeable : WriteCacheNotChangeable;
    }

    information = sizeof(STORAGE_WRITE_CACHE_PROPERTY);

WriteCacheExit:

    FREE_POOL(modeData);

    //
    // Set the size and status in IRP
    //
    Irp->IoStatus.Information = information;;
    Irp->IoStatus.Status = status;

    ClassReleaseRemoveLock(DeviceObject, Irp);
    ClassCompleteRequest(DeviceObject, Irp, IO_NO_INCREMENT);

    return status;
}


__inline
BOOLEAN
ValidPersistentReserveScope(
    UCHAR Scope)
{
    switch (Scope) {
    case RESERVATION_SCOPE_LU:
    case RESERVATION_SCOPE_ELEMENT:

        return TRUE;

    default:

        return FALSE;
    }

    return FALSE;
}

__inline
ValidPersistentReserveType(
    UCHAR Type)
{
    switch (Type) {
    case RESERVATION_TYPE_WRITE_EXCLUSIVE:
    case RESERVATION_TYPE_EXCLUSIVE:
    case RESERVATION_TYPE_WRITE_EXCLUSIVE_REGISTRANTS:
    case RESERVATION_TYPE_EXCLUSIVE_REGISTRANTS:

        return TRUE;

    default:

        return FALSE;
    }

    return FALSE;
}


/*++

ClasspPersistentReserve

Routine Description:

    Handles IOCTL_STORAGE_PERSISTENT_RESERVE_IN and IOCTL_STORAGE_PERSISTENT_RESERVE_OUT.

Arguments:

    DeviceObject - a pointer to the device object
    Irp - a pointer to the I/O request packet
    Srb - pointer to preallocated SCSI_REQUEST_BLOCK.

Return Value:

    Status Code

--*/
NTSTATUS
ClasspPersistentReserve(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp,
    PSCSI_REQUEST_BLOCK Srb
    )
{
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PCDB cdb = (PCDB)Srb->Cdb;
    PPERSISTENT_RESERVE_COMMAND prCommand = Irp->AssociatedIrp.SystemBuffer;
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = DeviceObject->DeviceExtension;

    NTSTATUS status;

    ULONG dataBufLen;
    ULONG controlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

    BOOLEAN writeToDevice;

    //
    // Check common input buffer parameters.
    //

    if (irpStack->Parameters.DeviceIoControl.InputBufferLength <
            sizeof(PERSISTENT_RESERVE_COMMAND) ||
        prCommand->Size < sizeof(PERSISTENT_RESERVE_COMMAND)) {

        status = STATUS_INFO_LENGTH_MISMATCH;
        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;

        FREE_POOL(Srb);

        ClassReleaseRemoveLock(DeviceObject, Irp);
        ClassCompleteRequest(DeviceObject, Irp, IO_NO_INCREMENT);
        goto ClasspPersistentReserve_Exit;
    }

    //
    // Check buffer alignment. Only an issue if another kernel mode component
    // (not the I/O manager) allocates the buffer.
    //

    if ((ULONG_PTR)prCommand & fdoExtension->AdapterDescriptor->AlignmentMask) {

        status = STATUS_INVALID_USER_BUFFER;
        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;

        FREE_POOL(Srb);

        ClassReleaseRemoveLock(DeviceObject, Irp);
        ClassCompleteRequest(DeviceObject, Irp, IO_NO_INCREMENT);
        goto ClasspPersistentReserve_Exit;
    }

    //
    // Check additional parameters.
    //

    status = STATUS_SUCCESS;

    if (controlCode == IOCTL_STORAGE_PERSISTENT_RESERVE_IN) {

        //
        // Check output buffer for PR In.
        //

        if (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
                        prCommand->PR_IN.AllocationLength) {

            status = STATUS_INVALID_PARAMETER;
        }

        switch (prCommand->PR_IN.ServiceAction) {

        case RESERVATION_ACTION_READ_KEYS:

            if (prCommand->PR_IN.AllocationLength < sizeof(PRI_REGISTRATION_LIST)) {

                status = STATUS_INVALID_PARAMETER;
            }

            break;

        case RESERVATION_ACTION_READ_RESERVATIONS:

            if (prCommand->PR_IN.AllocationLength < sizeof(PRI_RESERVATION_LIST)) {

                status = STATUS_INVALID_PARAMETER;
            }

            break;

        default:

            status = STATUS_INVALID_PARAMETER;
            break;
        }

        if (!NT_SUCCESS(status)) {

            Irp->IoStatus.Status = status;
            Irp->IoStatus.Information = 0;

            FREE_POOL(Srb);

            ClassReleaseRemoveLock(DeviceObject, Irp);
            ClassCompleteRequest(DeviceObject, Irp, IO_NO_INCREMENT);
            goto ClasspPersistentReserve_Exit;
        }

        //
        // Fill in the CDB.
        //

        cdb->PERSISTENT_RESERVE_IN.OperationCode    = SCSIOP_PERSISTENT_RESERVE_IN;
        cdb->PERSISTENT_RESERVE_IN.ServiceAction    = prCommand->PR_IN.ServiceAction;

        REVERSE_BYTES_SHORT(&(cdb->PERSISTENT_RESERVE_IN.AllocationLength),
                            &(prCommand->PR_IN.AllocationLength));

        dataBufLen = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
        writeToDevice = FALSE;


    } else {

        //
        // Verify ServiceAction, Scope, and Type
        //

        switch (prCommand->PR_OUT.ServiceAction) {

        case RESERVATION_ACTION_REGISTER:
        case RESERVATION_ACTION_REGISTER_IGNORE_EXISTING:
        case RESERVATION_ACTION_CLEAR:

            // Scope and type ignored.

            break;

        case RESERVATION_ACTION_RESERVE:
        case RESERVATION_ACTION_RELEASE:
        case RESERVATION_ACTION_PREEMPT:
        case RESERVATION_ACTION_PREEMPT_ABORT:

            if (!ValidPersistentReserveScope(prCommand->PR_OUT.Scope) ||
                !ValidPersistentReserveType(prCommand->PR_OUT.Type)) {

                status = STATUS_INVALID_PARAMETER;

            }

            break;

        default:

            status = STATUS_INVALID_PARAMETER;

            break;
        }

        //
        // Check input buffer for PR Out.
        // Caller must include the PR parameter list.
        //

        if (NT_SUCCESS(status)) {

            if (irpStack->Parameters.DeviceIoControl.InputBufferLength <
                    (sizeof(PERSISTENT_RESERVE_COMMAND) +
                     sizeof(PRO_PARAMETER_LIST)) ||
                prCommand->Size <
                    irpStack->Parameters.DeviceIoControl.InputBufferLength) {

            status = STATUS_INVALID_PARAMETER;

            }
        }


        if (!NT_SUCCESS(status)) {

            Irp->IoStatus.Status = status;
            Irp->IoStatus.Information = 0;

            FREE_POOL(Srb);

            ClassReleaseRemoveLock(DeviceObject, Irp);
            ClassCompleteRequest(DeviceObject, Irp, IO_NO_INCREMENT);
            goto ClasspPersistentReserve_Exit;
        }

        //
        // Fill in the CDB.
        //

        cdb->PERSISTENT_RESERVE_OUT.OperationCode   = SCSIOP_PERSISTENT_RESERVE_OUT;
        cdb->PERSISTENT_RESERVE_OUT.ServiceAction   = prCommand->PR_OUT.ServiceAction;
        cdb->PERSISTENT_RESERVE_OUT.Scope           = prCommand->PR_OUT.Scope;
        cdb->PERSISTENT_RESERVE_OUT.Type            = prCommand->PR_OUT.Type;

        cdb->PERSISTENT_RESERVE_OUT.ParameterListLength[1] = (UCHAR)sizeof(PRO_PARAMETER_LIST);

        //
        // Move the parameter list to the beginning of the data buffer (so it is aligned
        // correctly and that the MDL describes it correctly).
        //

        RtlMoveMemory(prCommand,
                      prCommand->PR_OUT.ParameterList,
                      sizeof(PRO_PARAMETER_LIST));

        dataBufLen = sizeof(PRO_PARAMETER_LIST);
        writeToDevice = TRUE;
    }

    //
    // Fill in the SRB
    //

    Srb->CdbLength = 10;

    //
    // Set timeout value.
    //

    Srb->TimeOutValue = fdoExtension->TimeOutValue;

    //
    // Send as a tagged request.
    //

    Srb->QueueAction = SRB_HEAD_OF_QUEUE_TAG_REQUEST;
    SET_FLAG(Srb->SrbFlags, SRB_FLAGS_NO_QUEUE_FREEZE);
    SET_FLAG(Srb->SrbFlags, SRB_FLAGS_QUEUE_ACTION_ENABLE);

    status = ClassSendSrbAsynchronous(DeviceObject,
                                      Srb,
                                      Irp,
                                      prCommand,
                                      dataBufLen,
                                      writeToDevice);

ClasspPersistentReserve_Exit:

    return status;

}

/*++

ClasspPriorityHint

Routine Description:

    Handles IOCTL_STORAGE_CHECK_PRIORITY_HINT_SUPPORT.

Arguments:

    DeviceObject - a pointer to the device object
    Irp - a pointer to the I/O request packet

Return Value:

    Status Code

--*/
NTSTATUS
ClasspPriorityHint(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )
{
    PFUNCTIONAL_DEVICE_EXTENSION fdoExtension = DeviceObject->DeviceExtension;
    PCOMMON_DEVICE_EXTENSION commonExtension = DeviceObject->DeviceExtension;
    PCLASS_PRIVATE_FDO_DATA fdoData = fdoExtension->PrivateFdoData;
    PSTORAGE_PRIORITY_HINT_SUPPORT priSupport = Irp->AssociatedIrp.SystemBuffer;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;

    Irp->IoStatus.Information = 0;

    //
    // Check whether this device supports idle priority.
    //
    if (!fdoData->IdlePrioritySupported) {
        status = STATUS_NOT_SUPPORTED;
        goto PriorityHintExit;
    }

    if (irpStack->Parameters.DeviceIoControl.OutputBufferLength <
        sizeof(STORAGE_PRIORITY_HINT_SUPPORT)) {
        status = STATUS_BUFFER_TOO_SMALL;
        goto PriorityHintExit;
    }

    RtlZeroMemory(priSupport, sizeof(STORAGE_PRIORITY_HINT_SUPPORT));

    status = ClassForwardIrpSynchronous(commonExtension, Irp);
    if (!NT_SUCCESS(status)) {
        //
        // If I/O priority is not supported by lower drivers, just set the
        // priorities supported by class driver.
        //
        TracePrint((TRACE_LEVEL_FATAL, TRACE_FLAG_IOCTL, "ClasspPriorityHint: I/O priority not supported by port driver.\n"));
        priSupport->SupportFlags = 0;
        status = STATUS_SUCCESS;
    }

    TracePrint((TRACE_LEVEL_FATAL, TRACE_FLAG_IOCTL, "ClasspPriorityHint: I/O priorities supported by port driver: %X\n", priSupport->SupportFlags));

    priSupport->SupportFlags |= (1 << IoPriorityVeryLow) |
                                (1 << IoPriorityLow) |
                                (1 << IoPriorityNormal) ;

    TracePrint((TRACE_LEVEL_FATAL, TRACE_FLAG_IOCTL, "ClasspPriorityHint: I/O priorities supported: %X\n", priSupport->SupportFlags));
    Irp->IoStatus.Information = sizeof(STORAGE_PRIORITY_HINT_SUPPORT);

PriorityHintExit:

    Irp->IoStatus.Status = status;
    ClassReleaseRemoveLock(DeviceObject, Irp);
    ClassCompleteRequest(DeviceObject, Irp, IO_NO_INCREMENT);
    return status;
}


