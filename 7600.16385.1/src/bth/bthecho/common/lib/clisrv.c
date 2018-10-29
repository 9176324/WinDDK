/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    clisrv.c

Abstract:

    Implementation of functions common to bth echo server and client

Environment:

    Kernel mode

--*/

#include "clisrv.h"

#if defined(EVENT_TRACING)
#include "clisrv.tmh"
#endif


__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
BthEchoSharedDeviceContextHeaderInit(
    PBTHECHOSAMPLE_DEVICE_CONTEXT_HEADER Header,
    WDFDEVICE Device
    )
/*++

Description:

    Initializes the common context header between server and client

Arguments:

    Header - Contex header
    Device - Framework device object

Return Value:

    NTSTATUS Status code.

--*/
{
    NTSTATUS status;
    WDF_OBJECT_ATTRIBUTES attributes;
        
    Header->Device = Device;

    Header->IoTarget = WdfDeviceGetIoTarget(Device);

    //
    // Initialize request object
    //
    
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.ParentObject = Device;

    status = WdfRequestCreate(
        &attributes,
        Header->IoTarget,
        &Header->Request
        );

    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP, 
            "Failed to pre-allocate request in device context, Status code %!STATUS!\n", status);

        goto exit;        
    }

exit:
    return status;
}

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
BthEchoSharedRetrieveLocalBthAddress(
    __in PBTHECHOSAMPLE_DEVICE_CONTEXT_HEADER DevCtxHdr
    )
/*++

Description:

    Retrieves the local bth address.
    
    This address is burnt into device hence doesn't change.

Arguments:

    Header - Contex header

Return Value:

    NTSTATUS Status code.

--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    struct _BRB_GET_LOCAL_BD_ADDR * brb = NULL;
    
    brb = (struct _BRB_GET_LOCAL_BD_ADDR *)
        DevCtxHdr->ProfileDrvInterface.BthAllocateBrb(
            BRB_HCI_GET_LOCAL_BD_ADDR, 
            POOLTAG_BTHECHOSAMPLE
            );

    if(brb == NULL)
    {
        status = STATUS_INSUFFICIENT_RESOURCES;

        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP, 
            "Failed to allocate brb BRB_HCI_GET_LOCAL_BD_ADDR, returning status code %!STATUS!\n", status);        

        goto exit;
    }

    status = BthEchoSharedSendBrbSynchronously(
        DevCtxHdr->IoTarget,
        DevCtxHdr->Request,
        (PBRB) brb,
        sizeof(*brb)
        );

    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP, 
            "Retrieving local bth address failed, Status code %!STATUS!\n", status);        

        goto exit1;        
    }

    DevCtxHdr->LocalBthAddr = brb->BtAddress;
    
exit1:
    DevCtxHdr->ProfileDrvInterface.BthFreeBrb((PBRB)brb);
exit:
    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
BthEchoSharedSendBrbAsync(
    __in WDFIOTARGET IoTarget,
    __in WDFREQUEST Request,
    __in PBRB Brb,
    __in size_t BrbSize,
    __in PFN_WDF_REQUEST_COMPLETION_ROUTINE ComplRoutine,
    __in_opt WDFCONTEXT Context
    )
/*++

Routine Description:

    This routine formats a request with brb and sends it asynchronously

Arguments:

    IoTarget - Target to send the brb to
    Request - request object to be formatted with brb                
    Brb - Brb to be sent
    BrbSize - size of the Brb data structure
    ComplRoutine - WDF completion routine for the request
                   This must be specified because we are formatting the request
                   and hence not using SEND_AND_FORGET flag
    Context - (optional) context to be passed in to the completion routine

Return Value:

    Success implies that request was sent correctly and completion routine will be called
    for it,
    failure implies it was not sent and caller should complete the request

Notes:

    This routine does not call WdfRequestReuse on the Request passed in.
    Caller must do so before passing in the request, if it is reusing the request.

    This routine does not complete the request in case of failure.
    Caller must complete the request in case of failure.

--*/
{
    NTSTATUS status;
    WDF_OBJECT_ATTRIBUTES attributes;
    WDFMEMORY memoryArg1;

    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.ParentObject = Request;

    status = WdfMemoryCreatePreallocated(
        &attributes,
        Brb,
        BrbSize,
        &memoryArg1
        );

    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_UTIL, 
            "Creating preallocted memory for Brb 0x%p failed, Request to be formatted 0x%p, "
            "Status code %!STATUS!\n",
            Brb,
            Request,
            status
            );

        goto exit;
    }

    status = WdfIoTargetFormatRequestForInternalIoctlOthers(
        IoTarget,
        Request,
        IOCTL_INTERNAL_BTH_SUBMIT_BRB,
        memoryArg1,
        NULL, //OtherArg1Offset
        NULL, //OtherArg2
        NULL, //OtherArg2Offset
        NULL, //OtherArg4
        NULL  //OtherArg4Offset
        );

    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_UTIL, 
            "Formatting request 0x%p with Brb 0x%p failed, Status code %!STATUS!\n",
            Request,
            Brb,
            status
            );

        goto exit;
    }

    //
    // Set a CompletionRoutine callback function.
    //
    WdfRequestSetCompletionRoutine(
        Request,
        ComplRoutine,
        Context
        );

    if (FALSE == WdfRequestSend(
        Request,
        IoTarget,
        NULL
        ))
    {
        status = WdfRequestGetStatus(Request);

        TraceEvents(TRACE_LEVEL_ERROR, DBG_UTIL, 
            "Request send failed for request 0x%p, Brb 0x%p, Status code %!STATUS!\n", 
            Request,
            Brb,
            status
            );

        goto exit;
    }

exit:
    return status;
}

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
BthEchoSharedSendBrbSynchronously(
    __in WDFIOTARGET IoTarget,
    __in WDFREQUEST Request,
    __in PBRB Brb,
    __in ULONG BrbSize
    )
/*++

Routine Description:

    This routine formats a request with brb and sends it synchronously

Arguments:

    IoTarget - Target to send the brb to
    Request - request object to be formatted with brb                
    Brb - Brb to be sent
    BrbSize - size of the Brb data structure

Return Value:

    NTSTATUS Status code.

Notes:

    This routine does calls WdfRequestReuse on the Request passed in.
    Caller need not do so before passing in the request.

    This routine does not complete the request in case of failure.
    Caller must complete the request in case of failure.

--*/
{
    NTSTATUS status;
    WDF_REQUEST_REUSE_PARAMS reuseParams;
    WDF_MEMORY_DESCRIPTOR OtherArg1Desc;

    WDF_REQUEST_REUSE_PARAMS_INIT(
        &reuseParams,
        WDF_REQUEST_REUSE_NO_FLAGS, 
        STATUS_NOT_SUPPORTED
        );

    status = WdfRequestReuse(Request, &reuseParams);
    if (!NT_SUCCESS(status))
    {
        goto exit;
    }

    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(
                        &OtherArg1Desc,
                        Brb,
                        BrbSize
                        );
    
    status = WdfIoTargetSendInternalIoctlOthersSynchronously(
        IoTarget,
        Request,
        IOCTL_INTERNAL_BTH_SUBMIT_BRB,
        &OtherArg1Desc,
        NULL, //OtherArg2
        NULL, //OtherArg4
        NULL, //RequestOptions
        NULL  //BytesReturned
        );

exit:
    return status;
}


