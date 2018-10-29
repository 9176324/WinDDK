/*++

Copyright (c) Microsoft Corporation

Module Name:

    kmdf_vdev_async.c

Abstract:

--*/

#include "kmdf_vdev.h"

VOID  
kmdf1394_AllocateAddressRangeCompletion (    
    IN WDFREQUEST  Request,    
    IN WDFIOTARGET  Target,    
    IN PWDF_REQUEST_COMPLETION_PARAMS  Params,    
    IN WDFCONTEXT  Context)
/*++

Routine Description:

    Allocate Address Range completion routine.

Arguments:

    Request - UNUSED

    Target - UNUSED

    Params - Completion param struct filled out by the completing driver
    
    Context - Pointer to a context structure

Return Value:

    VOID
--*/
{
    PASYNC_ADDRESS_DATA   AsyncAddrData = NULL;
    PALLOCATE_ADDRESS_RANGE  AllocateAddrRange = NULL;
    PDEVICE_EXTENSION  DeviceExtension = NULL;
    WDFMEMORY Memory; 
    PIRB pIrb = NULL;

    PCONTEXT_BUNDLE ContextBundle = NULL;

    //
    // We can get the request completion status from here
    //
    NTSTATUS ntStatus = Params->IoStatus.Status;

    UNREFERENCED_PARAMETER (Target);
    UNREFERENCED_PARAMETER (Request);

    if (NULL == Context)
    {
        TRACE (TL_ERROR, ("Context is NULL!\n"));
        return;
    }

    ContextBundle = (PCONTEXT_BUNDLE) Context;


    AllocateAddrRange = (PALLOCATE_ADDRESS_RANGE) ContextBundle->Context0;
    AsyncAddrData = (PASYNC_ADDRESS_DATA) ContextBundle->Context1;
    Memory = (WDFMEMORY) ContextBundle->Context2;
    DeviceExtension = ContextBundle->Context3;

    //
    // By using the WdfMemoryCreatePreallocated, we can pull the pointer to our
    // IRB from the WDFMemory Object.
    //
    pIrb = (PIRB) WdfMemoryGetBuffer (Memory, NULL);


    if (NT_SUCCESS (ntStatus)) 
    {
        AsyncAddrData->nAddressesReturned = \
            pIrb->u.AllocateAddressRange.AddressesReturned;
        AsyncAddrData->hAddressRange = \
            pIrb->u.AllocateAddressRange.hAddressRange;

        WdfSpinLockAcquire (DeviceExtension->AsyncSpinLock);

        InsertHeadList(
            &DeviceExtension->AsyncAddressData, 
            &AsyncAddrData->AsyncAddressList);

        WdfSpinLockRelease(DeviceExtension->AsyncSpinLock);

        AsyncAddrData->hAddressRange = \
            pIrb->u.AllocateAddressRange.hAddressRange;

        //
        // This goes back in to our original packet from user mode
        //
        AllocateAddrRange->hAddressRange = \
            pIrb->u.AllocateAddressRange.hAddressRange;

        AllocateAddrRange->Required1394Offset.Off_High = \
            pIrb->u.AllocateAddressRange.p1394AddressRange[0].AR_Off_High;

        AllocateAddrRange->Required1394Offset.Off_Low = \
            pIrb->u.AllocateAddressRange.p1394AddressRange[0].AR_Off_Low;
    }
    else 
    {
        TRACE(TL_ERROR, ("AllocateAddressRange failed = 0x%x\n", ntStatus));

        if (pIrb->u.AllocateAddressRange.Mdl)
        {
            IoFreeMdl (pIrb->u.AllocateAddressRange.Mdl);
        }

        if (AsyncAddrData->Buffer)
        {
            ExFreePoolWithTag (AsyncAddrData->Buffer, POOLTAG_KMDF_VDEV);
        }

        if (AsyncAddrData->AddressRange)
        {
            ExFreePoolWithTag (AsyncAddrData->AddressRange, POOLTAG_KMDF_VDEV);
        }

        ExFreePoolWithTag (AsyncAddrData, POOLTAG_KMDF_VDEV);
    }

    ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);

    WdfObjectDelete (Memory);
}

NTSTATUS
kmdf1394_AllocateAddressRange (
                               IN WDFDEVICE Device,
                               IN WDFREQUEST Request,
                               IN OUT PALLOCATE_ADDRESS_RANGE AllocateAddrRange)
/*++

Routine Description:

    Allocate Address Range routine.

Arguments:

    Device - the current WDFDEVICE Object.

    Request - the current request.

    AllocateAddressRange - the Data buffer from usermode to be worked on.

Return Value:

    VOID
--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension = GetDeviceContext(Device);
    PIRB pIrb = NULL;
    PASYNC_ADDRESS_DATA pAsyncAddressData = NULL;
    ULONG nPages;
    WDFMEMORY Memory;
    CONTEXT_BUNDLE ContextBundle;

    ENTER("kmdf1394_AllocateAddressRange");

    pIrb = ExAllocatePoolWithTag (NonPagedPool, sizeof(IRB), POOLTAG_KMDF_VDEV);
    if (!pIrb)
    {
        TRACE(TL_ERROR, ("Failed to allocate pIrb!\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    } 

    ContextBundle.Context0 = AllocateAddrRange;

    pAsyncAddressData = ExAllocatePoolWithTag (
        NonPagedPool,
        sizeof(ASYNC_ADDRESS_DATA),
        POOLTAG_KMDF_VDEV);
    if (!pAsyncAddressData)
    {
        TRACE(TL_ERROR, ("Failed to allocate pAsyncAddressData!\n"));

        ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    ContextBundle.Context1 = pAsyncAddressData;

    pAsyncAddressData->Buffer = ExAllocatePoolWithTag (
        NonPagedPool, 
        AllocateAddrRange->nLength, 
        POOLTAG_KMDF_VDEV);
    if (!pAsyncAddressData->Buffer) 
    {
        TRACE(TL_ERROR, ("Failed to allocate Buffer!\n"));

        ExFreePoolWithTag (pAsyncAddressData, POOLTAG_KMDF_VDEV);
        ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    if ((0 == AllocateAddrRange->MaxSegmentSize) || 
        (PAGE_SIZE == AllocateAddrRange->MaxSegmentSize))
    {
        nPages = \
            ADDRESS_AND_SIZE_TO_SPAN_PAGES (
            AllocateAddrRange->Data, 
            AllocateAddrRange->nLength);
    }
    else
    {
        nPages = \
            (AllocateAddrRange->nLength % AllocateAddrRange->MaxSegmentSize) ? \
            AllocateAddrRange->nLength / AllocateAddrRange->MaxSegmentSize + 1 : \
            AllocateAddrRange->nLength / AllocateAddrRange->MaxSegmentSize;
    }

    pAsyncAddressData->AddressRange = ExAllocatePoolWithTag (
        NonPagedPool,
        sizeof(ADDRESS_RANGE)*nPages,
        POOLTAG_KMDF_VDEV);
    if (!pAsyncAddressData->AddressRange) 
    {
        TRACE(TL_ERROR, ("Failed to allocate AddressRange!\n"));

        ExFreePoolWithTag(pAsyncAddressData->Buffer, POOLTAG_KMDF_VDEV);
        ExFreePoolWithTag(pAsyncAddressData, POOLTAG_KMDF_VDEV);
        ExFreePoolWithTag(pIrb, POOLTAG_KMDF_VDEV);

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    pAsyncAddressData->pMdl = IoAllocateMdl (
        pAsyncAddressData->Buffer,
        AllocateAddrRange->nLength,
        FALSE,
        FALSE,
        NULL);

    if (!pAsyncAddressData->pMdl) 
    {
        TRACE(TL_ERROR, ("Failed to create pMdl!\n"));

        ExFreePoolWithTag (pAsyncAddressData->AddressRange, POOLTAG_KMDF_VDEV);
        ExFreePoolWithTag (pAsyncAddressData->Buffer, POOLTAG_KMDF_VDEV);
        ExFreePoolWithTag (pAsyncAddressData, POOLTAG_KMDF_VDEV);
        ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    MmBuildMdlForNonPagedPool (pAsyncAddressData->pMdl);

    TRACE(TL_TRACE, ("pMdl = 0x%x\n", pAsyncAddressData->pMdl));

    //
    // copy over the contents of data to our driver buffer
    //
    RtlCopyMemory (
        pAsyncAddressData->Buffer, 
        AllocateAddrRange->Data, 
        AllocateAddrRange->nLength);

    pAsyncAddressData->nLength = AllocateAddrRange->nLength;

    RtlZeroMemory (pIrb, sizeof (IRB));
    pIrb->FunctionNumber = REQUEST_ALLOCATE_ADDRESS_RANGE;
    pIrb->Flags = 0;
    pIrb->u.AllocateAddressRange.Mdl = pAsyncAddressData->pMdl;
    pIrb->u.AllocateAddressRange.fulFlags = AllocateAddrRange->fulFlags;
    pIrb->u.AllocateAddressRange.nLength = AllocateAddrRange->nLength;
    pIrb->u.AllocateAddressRange.MaxSegmentSize = \
        AllocateAddrRange->MaxSegmentSize;
    pIrb->u.AllocateAddressRange.fulAccessType = \
        AllocateAddrRange->fulAccessType;
    pIrb->u.AllocateAddressRange.fulNotificationOptions = \
        AllocateAddrRange->fulNotificationOptions;


    pIrb->u.AllocateAddressRange.Callback = NULL;
    pIrb->u.AllocateAddressRange.Context = NULL;

    pIrb->u.AllocateAddressRange.Required1394Offset = \
        AllocateAddrRange->Required1394Offset;
    pIrb->u.AllocateAddressRange.FifoSListHead = NULL;
    pIrb->u.AllocateAddressRange.FifoSpinLock = NULL;
    pIrb->u.AllocateAddressRange.AddressesReturned = 0;
    pIrb->u.AllocateAddressRange.p1394AddressRange = \
        pAsyncAddressData->AddressRange;
    pIrb->u.AllocateAddressRange.DeviceExtension = deviceExtension;

    //
    // We need to create a WDF Memory object for the IRB to nestle in
    // for an async request.
    //
    ntStatus = WdfMemoryCreatePreallocated (
        WDF_NO_OBJECT_ATTRIBUTES,
        pIrb,
        sizeof (IRB),
        &Memory);
    if (!NT_SUCCESS (ntStatus))
    {
        TRACE (
            TL_ERROR,
            ("Failed WdfMemoryCreate %d\n", 
            ntStatus));

        IoFreeMdl (pAsyncAddressData->pMdl);
        ExFreePoolWithTag (pAsyncAddressData->AddressRange, POOLTAG_KMDF_VDEV);
        ExFreePoolWithTag (pAsyncAddressData->Buffer, POOLTAG_KMDF_VDEV);
        ExFreePoolWithTag (pAsyncAddressData, POOLTAG_KMDF_VDEV);
        ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);

        return ntStatus;
    }

    ContextBundle.Context2 = Memory;
    ContextBundle.Context3 = deviceExtension;

    WdfRequestSetCompletionRoutine (
        Request, 
        kmdf1394_AllocateAddressRangeCompletion, 
        &ContextBundle);

    ntStatus = kmdf1394_SubmitIrpAsync (
        deviceExtension->StackIoTarget, 
        Request, 
        Memory);

    EXIT ("kmdf1394_AllocateAddressRange", ntStatus);
    return ntStatus;
} //kmdf1394_AllocateAddressRange


NTSTATUS
kmdf1394_FreeAddressRange (
                           IN WDFDEVICE Device,
                           IN WDFREQUEST Request,
                           IN HANDLE hAddressRange)
/*++

Routine Description:

    Allocate Address Range routine.

Arguments:

    Device - the current WDFDEVICE Object.

    Request - the current request.

    hAddressRange - The Address Range to be freed.

Return Value:

    VOID
--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension = GetDeviceContext(Device);
    PIRB pIrb = NULL;
    PASYNC_ADDRESS_DATA AsyncAddressData  = NULL;
    PLIST_ENTRY listHead, thisEntry;

    UNREFERENCED_PARAMETER(Request);

    ENTER("kmdf1394_FreeAddressRange");

    //
    // have to find our struct...
    //
    WdfSpinLockAcquire (deviceExtension->AsyncSpinLock);

    listHead = &deviceExtension->AsyncAddressData;

    for (thisEntry = listHead->Flink; 
        thisEntry != listHead; 
        AsyncAddressData = NULL, thisEntry = thisEntry->Flink)
    {
        AsyncAddressData = CONTAINING_RECORD (
            thisEntry, 
            ASYNC_ADDRESS_DATA,
            AsyncAddressList);

        if (AsyncAddressData->hAddressRange == hAddressRange) 
        {
            RemoveEntryList(&AsyncAddressData->AsyncAddressList);
            break;
        }
    }

    WdfSpinLockRelease(deviceExtension->AsyncSpinLock);

    //
    // never found an entry...
    //
    if (!AsyncAddressData) 
    {
        return STATUS_INVALID_PARAMETER;
    }

    // 
    // got it, lets free it...
    //
    pIrb = ExAllocatePoolWithTag (NonPagedPool, sizeof(IRB), POOLTAG_KMDF_VDEV);
    if (!pIrb) 
    {
        TRACE(TL_ERROR, ("Failed to allocate pIrb!\n"));

        //
        // Catasrophic failure, insert the AddressData element back on 
        // the list before existing
        //
        WdfSpinLockAcquire (deviceExtension->AsyncSpinLock);

        InsertHeadList (
            &deviceExtension->AsyncAddressData, 
            &AsyncAddressData->AsyncAddressList);

        WdfSpinLockRelease (deviceExtension->AsyncSpinLock);

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory (pIrb, sizeof (IRB));
    pIrb->FunctionNumber = REQUEST_FREE_ADDRESS_RANGE;
    pIrb->Flags = 0;
    pIrb->u.FreeAddressRange.nAddressesToFree = \
        AsyncAddressData->nAddressesReturned;
    pIrb->u.FreeAddressRange.p1394AddressRange = AsyncAddressData->AddressRange;
    pIrb->u.FreeAddressRange.pAddressRange = &AsyncAddressData->hAddressRange;
    pIrb->u.FreeAddressRange.DeviceExtension = (PVOID)deviceExtension;

    //
    // We're going to send this one synchronously
    //
    ntStatus = kmdf1394_SubmitIrpSynch (
        deviceExtension->StackIoTarget, 
        Request, 
        pIrb);
    if (!NT_SUCCESS (ntStatus))
    {
        //
        // The free request failed, insert the element back on to our tracking 
        // list and either try to free later, or release the resources on 
        // driver tear down.
        //
        WdfSpinLockAcquire (deviceExtension->AsyncSpinLock);

        InsertHeadList (
            &deviceExtension->AsyncAddressData, 
            &AsyncAddressData->AsyncAddressList);

        WdfSpinLockRelease (deviceExtension->AsyncSpinLock);
    }
    else
    {
        // 
        //  need to free up everything associated with this from the allocate...
        //
        IoFreeMdl (AsyncAddressData->pMdl);
        ExFreePoolWithTag (AsyncAddressData->Buffer, POOLTAG_KMDF_VDEV);
        ExFreePoolWithTag (AsyncAddressData->AddressRange, POOLTAG_KMDF_VDEV);
        ExFreePoolWithTag (AsyncAddressData, POOLTAG_KMDF_VDEV);
    }

    ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);

    EXIT("kmdf1394_FreeAddressRange", ntStatus);
    return ntStatus;
} // kmdf1394_FreeAddressRange


NTSTATUS
kmdf1394_GetAddressData (
                         IN WDFDEVICE Device,
                         IN WDFREQUEST Request,
                         IN OUT PGET_ADDRESS_DATA GetAddrData)
/*++

Routine Description:

    Get Address Data routine

Arguments:

    Device - the current WDFDEVICE Object.

    Request - the current request.

    GetAddrData - the Data buffer from usermode to be worked on.

Return Value:

    VOID
--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension  = GetDeviceContext(Device);
    PASYNC_ADDRESS_DATA AsyncAddressData = NULL;
    PLIST_ENTRY listHead, thisEntry;

    UNREFERENCED_PARAMETER(Request);

    ENTER("kmdf1394_GetAddressData");

    // 
    // have to find our struct...
    //
    WdfSpinLockAcquire(deviceExtension->AsyncSpinLock);

    listHead = &deviceExtension->AsyncAddressData;

    for(thisEntry = listHead->Flink; 
        thisEntry != listHead;
        AsyncAddressData = NULL, thisEntry = thisEntry->Flink)
    {
        AsyncAddressData = CONTAINING_RECORD(
            thisEntry, 
            ASYNC_ADDRESS_DATA,
            AsyncAddressList);
        if (AsyncAddressData->hAddressRange == GetAddrData->hAddressRange) 
        {
            PCHAR   pBuffer;

            // 
            // found it, let's copy over the contents to our buffer
            //
            pBuffer = \
                (PCHAR)((ULONG_PTR)AsyncAddressData->Buffer + \
                GetAddrData->ulOffset);

            if(pBuffer && GetAddrData->Data) 
            {
                RtlCopyMemory(GetAddrData->Data, pBuffer, GetAddrData->nLength);
            }
            break;
        }
    }

    WdfSpinLockRelease(deviceExtension->AsyncSpinLock);

    //
    // never found an entry...
    //
    if (!AsyncAddressData) 
    {
        ntStatus = STATUS_INVALID_PARAMETER;
    }

    EXIT("kmdf1394_GetAddressData", ntStatus);
    return ntStatus;
} // kmdf1394_GetAddressData

NTSTATUS
kmdf1394_SetAddressData (
                         IN WDFDEVICE Device,
                         IN WDFREQUEST Request,
                         IN PSET_ADDRESS_DATA SetAddrData)
/*++

Routine Description:

    Set Address Range routine.

Arguments:

    Device - the current WDFDEVICE Object.

    Request - the current request.

    SetAddrData - the Data buffer from usermode to be worked on.

Return Value:

    VOID
--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION  deviceExtension = GetDeviceContext(Device);
    PASYNC_ADDRESS_DATA AsyncAddressData = NULL;
    PLIST_ENTRY listHead, thisEntry;

    UNREFERENCED_PARAMETER(Request);

    ENTER("kmdf1394_SetAddressData");

    //
    // have to find our struct...
    //
    WdfSpinLockAcquire(deviceExtension->AsyncSpinLock);

    listHead = &deviceExtension->AsyncAddressData;

    for(thisEntry = listHead->Flink; thisEntry != listHead;
        AsyncAddressData = NULL, thisEntry = thisEntry->Flink)
    {
        AsyncAddressData = CONTAINING_RECORD(
            thisEntry, 
            ASYNC_ADDRESS_DATA,
            AsyncAddressList);
        if (AsyncAddressData->hAddressRange == SetAddrData->hAddressRange) 
        {
            PULONG  pBuffer;

            // 
            // found it, let's copy over the contents from data...
            //
            pBuffer = \
                (PULONG)((ULONG_PTR)AsyncAddressData->Buffer + \
                SetAddrData->ulOffset);

            TRACE(TL_TRACE, ("pBuffer = 0x%x\n", pBuffer));
            TRACE(TL_TRACE, ("Data = 0x%x\n", SetAddrData->Data));

            if(pBuffer && SetAddrData->Data) 
            {
                RtlCopyMemory(pBuffer, SetAddrData->Data, SetAddrData->nLength);
            }
            break;
        }
    }

    WdfSpinLockRelease(deviceExtension->AsyncSpinLock);

    // 
    // never found an entry...
    //
    if (!AsyncAddressData) 
    {
        ntStatus = STATUS_INVALID_PARAMETER;
    }

    EXIT("kmdf1394_SetAddressData", ntStatus);
    return ntStatus;
} // kmdf1394_SetAddressData


VOID  
kmdf1394_AsyncReadCompletion (    
                              IN WDFREQUEST  Request,    
                              IN WDFIOTARGET  Target,    
                              IN PWDF_REQUEST_COMPLETION_PARAMS  Params,    
                              IN WDFCONTEXT  Context)
/*++

Routine Description:

    Async Read completion routine.

Arguments:

Arguments:

    Request - UNUSED

    Target - UNUSED

    Params - UNUSED
    
    Context - Pointer to a WDFMemory Object

Return Value:

    VOID
--*/
{
    PIRB pIrb = NULL;

    UNREFERENCED_PARAMETER (Request);
    UNREFERENCED_PARAMETER (Target);
    UNREFERENCED_PARAMETER (Params);

    ASSERT (Context);

    pIrb = (PIRB) WdfMemoryGetBuffer ((WDFMEMORY)Context, NULL);

    //
    // Just basic clean up here.
    //
    IoFreeMdl (pIrb->u.AsyncRead.Mdl);
    ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);
    WdfObjectDelete ((WDFMEMORY) Context);
}

NTSTATUS
kmdf1394_AsyncRead (
                    IN WDFDEVICE Device,
                    IN WDFREQUEST Request,
                    IN OUT PASYNC_READ AsyncRead)
/*++

Routine Description:

    Async Read routine.

Arguments:

    Device - the current WDFDEVICE Object.

    Request - the current request.

    AsyncRead - the Data buffer from usermode to be worked on.

Return Value:

    VOID
--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION  deviceExtension = GetDeviceContext(Device);
    PIRB pIrb = NULL;
    PMDL pMdl = NULL;
    WDFIOTARGET ioTarget = NULL;
    WDFMEMORY Memory;

    ENTER("kmdf1394_AsyncRead");

    ioTarget = deviceExtension->StackIoTarget;

    pIrb = ExAllocatePoolWithTag(NonPagedPool, sizeof(IRB), POOLTAG_KMDF_VDEV);
    if (!pIrb) 
    {
        TRACE(TL_ERROR, ("Failed to allocate pIrb!\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory (pIrb, sizeof (IRB));
    pIrb->FunctionNumber = REQUEST_ASYNC_READ;
    pIrb->Flags = 0;
    pIrb->u.AsyncRead.DestinationAddress = AsyncRead->DestinationAddress;
    pIrb->u.AsyncRead.nNumberOfBytesToRead = AsyncRead->nNumberOfBytesToRead;
    pIrb->u.AsyncRead.nBlockSize = AsyncRead->nBlockSize;
    pIrb->u.AsyncRead.fulFlags = AsyncRead->fulFlags;

    if (AsyncRead->bGetGeneration) 
    {
        pIrb->u.AsyncRead.ulGeneration = deviceExtension->GenerationCount;

        TRACE(
            TL_TRACE, 
            ("Retrieved Generation Count = 0x%x\n", 
            pIrb->u.AsyncRead.ulGeneration));
    }
    else 
    {
        pIrb->u.AsyncRead.ulGeneration = AsyncRead->ulGeneration;
    }

    pMdl = IoAllocateMdl (
        AsyncRead->Data,
        AsyncRead->nNumberOfBytesToRead,
        FALSE,
        FALSE,
        NULL);
    if(NULL == pMdl) 
    {
        TRACE(TL_ERROR, ("Failed to allocate mdl!\n"));

        ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    MmBuildMdlForNonPagedPool(pMdl);

    pIrb->u.AsyncRead.Mdl = pMdl;

    //
    // We need to create a WDF Memory object for the IRB to nestle in
    // for an async request.
    //
    ntStatus = WdfMemoryCreatePreallocated (
        WDF_NO_OBJECT_ATTRIBUTES,
        pIrb,
        sizeof (IRB),
        &Memory);
    if (!NT_SUCCESS (ntStatus))
    {
        TRACE (
            TL_ERROR,
            ("Failed WdfMemoryCreate %d\n", 
            ntStatus));

        IoFreeMdl (pIrb->u.AsyncRead.Mdl);
        ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);

        return ntStatus;
    }

    WdfRequestSetCompletionRoutine (
        Request, 
        kmdf1394_AsyncReadCompletion, 
        Memory);


    ntStatus = kmdf1394_SubmitIrpAsync (ioTarget, Request, Memory);
    if (!NT_SUCCESS(ntStatus)) 
    {
        if (STATUS_INVALID_GENERATION == ntStatus) 
        {
            TRACE (TL_ERROR, ("Invalide Generation count\n"));
        }
        else
        {
            TRACE(TL_ERROR, ("SubmitIrpAsync failed = 0x%x\n", ntStatus));
        }

        IoFreeMdl (pIrb->u.AsyncRead.Mdl);
        ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);
        WdfObjectDelete (Memory);
    }

    EXIT("kmdf1394_AsyncRead", ntStatus);

    return ntStatus;
} // kmdf1394_AsyncRead

VOID  
kmdf1394_AsyncWriteCompletion (    
                               IN WDFREQUEST  Request,    
                               IN WDFIOTARGET  Target,    
                               IN PWDF_REQUEST_COMPLETION_PARAMS  Params,    
                               IN WDFCONTEXT  Context)
/*++

Routine Description:

    Async Write completion routine.

Arguments:

Arguments:

    Request - UNUSED

    Target - UNUSED

    Params - UNUSED
    
    Context - Pointer to a WDFMemory Object

Return Value:

    VOID
--*/
{
    PIRB pIrb = NULL;

    UNREFERENCED_PARAMETER (Request);
    UNREFERENCED_PARAMETER (Target);
    UNREFERENCED_PARAMETER (Params);

    ASSERT (Context);

    pIrb = (PIRB) WdfMemoryGetBuffer ((WDFMEMORY)Context, NULL);
    //
    // Just basic clean up here.
    //
    IoFreeMdl (pIrb->u.AsyncWrite.Mdl);
    ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);
    WdfObjectDelete ((WDFMEMORY)Context);
}

NTSTATUS
kmdf1394_AsyncWrite (
                     IN WDFDEVICE             Device,
                     IN WDFREQUEST         Request,
                     IN PASYNC_WRITE       AsyncWrite)
/*++

Routine Description:

    Async Write routine.

Arguments:

    Device - the current WDFDEVICE Object.

    Request - the current request.

    AsyncWrite - the Data buffer from usermode to be worked on.

Return Value:

    VOID
--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension = GetDeviceContext(Device);
    PIRB pIrb = NULL;
    PMDL   pMdl = NULL;
    WDFIOTARGET ioTarget = NULL;
    WDFMEMORY Memory;

    ENTER("kmdf1394_AsyncWrite");

    ioTarget = deviceExtension->StackIoTarget;


    pIrb = ExAllocatePoolWithTag(NonPagedPool, sizeof(IRB), POOLTAG_KMDF_VDEV);
    if (!pIrb) 
    {
        TRACE(TL_ERROR, ("Failed to allocate pIrb!\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory (pIrb, sizeof (IRB));
    pIrb->FunctionNumber = REQUEST_ASYNC_WRITE;
    pIrb->Flags = 0;
    pIrb->u.AsyncWrite.DestinationAddress = AsyncWrite->DestinationAddress;
    pIrb->u.AsyncWrite.nNumberOfBytesToWrite = AsyncWrite->nNumberOfBytesToWrite;
    pIrb->u.AsyncWrite.nBlockSize = AsyncWrite->nBlockSize;
    pIrb->u.AsyncWrite.fulFlags = AsyncWrite->fulFlags;

    if (AsyncWrite->bGetGeneration) 
    {
        TRACE(
            TL_TRACE, 
            ("Retrieved Generation Count = 0x%x\n", 
            pIrb->u.AsyncRead.ulGeneration));

        pIrb->u.AsyncRead.ulGeneration = deviceExtension->GenerationCount;
    }
    else 
    {
        pIrb->u.AsyncRead.ulGeneration = AsyncWrite->ulGeneration;
    }

    pMdl = IoAllocateMdl (
        AsyncWrite->Data,
        AsyncWrite->nNumberOfBytesToWrite,
        FALSE,
        FALSE,
        NULL);
    if(NULL == pMdl) 
    {
        TRACE(TL_ERROR, ("Failed to allocate mdl!\n"));

        ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    MmBuildMdlForNonPagedPool (pMdl);

    pIrb->u.AsyncWrite.Mdl = pMdl;

    //
    // We need to create a WDF Memory object for the IRB to nestle in
    // for an async request.
    //
    ntStatus = WdfMemoryCreatePreallocated (
        WDF_NO_OBJECT_ATTRIBUTES,
        pIrb,
        sizeof (IRB),
        &Memory);

    if (!NT_SUCCESS (ntStatus))
    {
        TRACE (
            TL_ERROR,
            ("Failed WdfMemoryCreate %d\n", 
            ntStatus));

        IoFreeMdl (pIrb->u.AsyncWrite.Mdl);
        ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);

        return ntStatus;
    }

    WdfRequestSetCompletionRoutine (
        Request, 
        kmdf1394_AsyncWriteCompletion, 
        Memory);

    ntStatus = kmdf1394_SubmitIrpAsync (ioTarget, Request, Memory);
    if (!NT_SUCCESS(ntStatus)) 
    {
        if (ntStatus != STATUS_INVALID_GENERATION) 
        {
            TRACE(TL_ERROR, ("SubmitIrpSync failed = 0x%x\n", ntStatus));
        }
        else
        {
            TRACE (TL_ERROR, ("Invalid Generation count\n"));
        }

        IoFreeMdl (pIrb->u.AsyncWrite.Mdl);
        ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);
        WdfObjectDelete (Memory);
    }

    EXIT("kmdf1394_AsyncWrite", ntStatus);
    return ntStatus;
} // kmdf1394_AsyncWrite

VOID  
kmdf1394_AsyncLockCompletion (    
                              IN WDFREQUEST  Request,    
                              IN WDFIOTARGET  Target,    
                              IN PWDF_REQUEST_COMPLETION_PARAMS  Params,    
                              IN WDFCONTEXT  Context)
/*++

Routine Description:

    Async Lock completion routine.

Arguments:

Arguments:

    Request - UNUSED

    Target - UNUSED

    Params - UNUSED
    
    Context - Pointer to a WDFMemory Object

Return Value:

    VOID
--*/
{
    PIRB pIrb = NULL;

    UNREFERENCED_PARAMETER (Request);
    UNREFERENCED_PARAMETER (Target);
    UNREFERENCED_PARAMETER (Params);

    ASSERT (Context);

    pIrb = (PIRB) WdfMemoryGetBuffer ((WDFMEMORY)Context, NULL);

    ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);
    WdfObjectDelete ((WDFMEMORY) Context);
}

NTSTATUS
kmdf1394_AsyncLock (
                    IN WDFDEVICE         Device,
                    IN WDFREQUEST     Request,
                    IN OUT PASYNC_LOCK   AsyncLock)
/*++

Routine Description:

    Async Lock routine.

Arguments:

    Device - the current WDFDEVICE Object.

    Request - the current request.

    AsyncLock - the Data buffer from usermode to be worked on.

Return Value:

    VOID
--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PIRB pIrb = NULL;
    WDFMEMORY Memory;
    PDEVICE_EXTENSION  deviceExtension = GetDeviceContext (Device);
    WDFIOTARGET ioTarget = NULL;

    ENTER ("kmdf1394_AsyncLock");

    ioTarget = deviceExtension->StackIoTarget;

    pIrb = ExAllocatePoolWithTag (NonPagedPool, sizeof(IRB), POOLTAG_KMDF_VDEV);
    if (!pIrb) 
    {
        TRACE(TL_ERROR, ("Failed to allocate pIrb!\n"));
        return STATUS_INSUFFICIENT_RESOURCES;  
    } 

    RtlZeroMemory (pIrb, sizeof (IRB));
    pIrb->FunctionNumber = REQUEST_ASYNC_LOCK;
    pIrb->Flags = 0;
    pIrb->u.AsyncLock.DestinationAddress = AsyncLock->DestinationAddress;
    pIrb->u.AsyncLock.nNumberOfArgBytes = AsyncLock->nNumberOfArgBytes;
    pIrb->u.AsyncLock.nNumberOfDataBytes = AsyncLock->nNumberOfDataBytes;
    pIrb->u.AsyncLock.fulTransactionType = AsyncLock->fulTransactionType;
    pIrb->u.AsyncLock.fulFlags = AsyncLock->fulFlags;
    pIrb->u.AsyncLock.Arguments[0] = AsyncLock->Arguments[0];
    pIrb->u.AsyncLock.Arguments[1] = AsyncLock->Arguments[1];
    pIrb->u.AsyncLock.DataValues[0] = AsyncLock->DataValues[0];
    pIrb->u.AsyncLock.DataValues[1] = AsyncLock->DataValues[1];
    pIrb->u.AsyncLock.pBuffer = AsyncLock->Buffer;

    if (AsyncLock->bGetGeneration) 
    {
        pIrb->u.AsyncLock.ulGeneration = deviceExtension->GenerationCount;

        TRACE(
            TL_TRACE, 
            ("Retrieved Generation Count = 0x%x\n", 
            pIrb->u.AsyncLock.ulGeneration));
    }
    else 
    {
        pIrb->u.AsyncLock.ulGeneration = AsyncLock->ulGeneration;
    }

    //
    // We need to create a WDF Memory object for the IRB to nestle in
    // for an async request.
    //
    ntStatus = WdfMemoryCreatePreallocated (
        WDF_NO_OBJECT_ATTRIBUTES,
        pIrb,
        sizeof (IRB),
        &Memory);
    if (!NT_SUCCESS (ntStatus))
    {
        TRACE (
            TL_ERROR,
            ("Failed WdfMemoryCreate %d\n", 
            ntStatus));

        ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);

        return ntStatus;
    }

    WdfRequestSetCompletionRoutine (
        Request, 
        kmdf1394_AsyncLockCompletion, 
        Memory);


    ntStatus = kmdf1394_SubmitIrpAsync (ioTarget, Request, Memory);
    if (!NT_SUCCESS(ntStatus)) 
    {
        if (STATUS_INVALID_GENERATION == ntStatus) 
        {
            TRACE (TL_ERROR, ("Invalid Geneation count\n"));
        }
        else
        {
            TRACE(TL_ERROR, ("SubmitIrpSync failed = 0x%x\n", ntStatus));
        }

        ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);
        WdfObjectDelete (Memory);
    }

    EXIT("kmdf1394_AsyncLock", ntStatus);
    return ntStatus;
} // kmdf1394_AsyncLock

VOID  
kmdf1394_AsyncStreamCompletion (    
                                IN WDFREQUEST  Request,    
                                IN WDFIOTARGET  Target,    
                                IN PWDF_REQUEST_COMPLETION_PARAMS  Params,    
                                IN WDFCONTEXT  Context)
/*++

Routine Description:

    Async Stream completion routine.

Arguments:

Arguments:

    Request - UNUSED

    Target - UNUSED

    Params - UNUSED
    
    Context - Pointer to a WDFMemory Object

Return Value:

    VOID
--*/
{
    PIRB pIrb = NULL;

    UNREFERENCED_PARAMETER (Request);
    UNREFERENCED_PARAMETER (Target);
    UNREFERENCED_PARAMETER (Params);

    ASSERT (Context);

    pIrb = (PIRB) WdfMemoryGetBuffer ((WDFMEMORY)Context, NULL);
    //
    // Just basic clean up here.
    //
    IoFreeMdl (pIrb->u.AsyncStream.Mdl);
    ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);
    WdfObjectDelete ((WDFMEMORY) Context);
}

NTSTATUS
kmdf1394_AsyncStream (
                      IN WDFDEVICE Device,
                      IN WDFREQUEST Request,
                      IN OUT PASYNC_STREAM AsyncStream)
/*++

Routine Description:

    Async Stream routine.

Arguments:

    Device - the current WDFDEVICE Object.

    Request - the current request.

    AsyncStream - the Data buffer from usermode to be worked on.

Return Value:

    VOID
--*/
{
    NTSTATUS    ntStatus = STATUS_SUCCESS;
    PDEVICE_EXTENSION  deviceExtension = GetDeviceContext (Device);
    PIRB pIrb = NULL;
    WDFIOTARGET ioTarget = deviceExtension->StackIoTarget;
    WDFMEMORY Memory;
    PMDL pMdl = NULL;

    ENTER("kmdf1394_AsyncStream");

    pIrb = ExAllocatePoolWithTag(NonPagedPool, sizeof(IRB), POOLTAG_KMDF_VDEV);
    if (!pIrb)
    {
        TRACE(TL_ERROR, ("Failed to allocate pIrb!\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    } 

    RtlZeroMemory (pIrb, sizeof (IRB));
    pIrb->FunctionNumber = REQUEST_ASYNC_STREAM;
    pIrb->Flags = 0;
    pIrb->u.AsyncStream.nNumberOfBytesToStream = \
        AsyncStream->nNumberOfBytesToStream;
    pIrb->u.AsyncStream.fulFlags = AsyncStream->fulFlags;
    pIrb->u.AsyncStream.ulTag = AsyncStream->ulTag;
    pIrb->u.AsyncStream.nChannel = AsyncStream->nChannel;
    pIrb->u.AsyncStream.ulSynch = AsyncStream->ulSynch;
    pIrb->u.AsyncStream.nSpeed = (UCHAR) AsyncStream->nSpeed;

    pMdl = IoAllocateMdl (
        AsyncStream->Data,
        AsyncStream->nNumberOfBytesToStream,
        FALSE,
        FALSE,
        NULL);
    if (!pMdl) 
    {
        TRACE(TL_ERROR, ("Failed to allocate pMdl!\n"));

        ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    MmBuildMdlForNonPagedPool(pMdl);

    pIrb->u.AsyncStream.Mdl = pMdl;

    //
    // We need to create a WDF Memory object for the IRB to nestle in
    // for an async request.
    //
    ntStatus = WdfMemoryCreatePreallocated (
        WDF_NO_OBJECT_ATTRIBUTES,
        pIrb,
        sizeof (IRB),
        &Memory);
    if (!NT_SUCCESS (ntStatus))
    {
        TRACE (
            TL_ERROR,
            ("Failed WdfMemoryCreate %d\n", 
            ntStatus));

        IoFreeMdl (pIrb->u.AsyncStream.Mdl);
        ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);

        return ntStatus;
    }

    WdfRequestSetCompletionRoutine (
        Request, 
        kmdf1394_AsyncStreamCompletion, 
        Memory);

    ntStatus = kmdf1394_SubmitIrpAsync (ioTarget, Request, Memory);
    if (!NT_SUCCESS(ntStatus)) 
    {
        TRACE(TL_ERROR, ("SubmitIrpAsync failed = 0x%x\n", ntStatus));

        IoFreeMdl (pIrb->u.AsyncStream.Mdl);
        ExFreePoolWithTag (pIrb, POOLTAG_KMDF_VDEV);
        WdfObjectDelete (Memory);
    }

    EXIT("kmdf1394_AsyncStream", ntStatus);
    return ntStatus;
} // kmdf1394_AsyncStream


