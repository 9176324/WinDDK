/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    usb_xmit.c

Abstract:
    Bus dependant layer USB specific function
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    01-01-2006    Created

Notes:

--*/


#include "hw_pcomp.h"

#pragma warning(disable:4200)  // nameless struct/union
#pragma warning(disable:4201)  // nameless struct/union
#pragma warning(disable:4214)  // bit field types other than int

#include "hw_main.h"
#include "hw_nic.h"
#include "8187_gen.h"
#include "Usb_main.h"
#include "usb_xmit.h"
#include "usb_recv.h"

#pragma warning(default:4200)
#pragma warning(default:4201)
#pragma warning(default:4214)
#pragma warning(disable:4244)


#define PERIODIC_INTERVAL  10  //10 ms

WDF_REQUEST_SEND_OPTIONS  G_RequestOptions;


VOID
AllocatePendRequestsTimer(
    PNIC    Nic
    );

EVT_WDF_TIMER HwUsbPendRequestsTimer;

VOID
HwUsbPendRequestsTimer(
        __in  WDFTIMER  Timer
        )
/*++

Routine Description
    This routine serves an important purpose. Read below.
    //
    // NOTE: In the PCI driver any Interrupt causes both the Tx and Rx handlers to be invoked.
    // The Tx handler needs to be invoked to flush out pending packets.
    // Beacons beng received every 100 ms cause this flushing in the PCI driver.
    // We simulate this on USB by invoking the Tx Handler.
    //
    

Arguments:
    Timer -- Handle to the WDFTIMER which fires periodically and 
              flushes the Pending Transmit packets.
    
Return Value:

    
--*/

{
    PNIC                Nic;
    PTIMER_NIC_CONTEXT  timerNicContext;

    timerNicContext  = GetTimerContext(Timer);
    Nic = timerNicContext->Nic;
    Mp11HandleSendCompleteInterrupt(Nic->Adapter);
}


VOID
HwUsbFreeXmitMemory(
    PNIC    Nic
    )
{
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;

    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);
    WdfTimerStop(pUsbDeviceContext->PendRequestTimer, TRUE);
    HwUsbFreeWriteRequests(Nic);
    return;
}


NDIS_STATUS
HwUsbXmitStart(
    PNIC    Nic
    )
{
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    NTSTATUS ntStatus;
    NDIS_STATUS ndisStatus;

    ntStatus = STATUS_SUCCESS;
    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);
    WdfWaitLockAcquire(pUsbDeviceContext->PipeStateLock, NULL);
    ntStatus =  WdfIoTargetStart(WdfUsbTargetPipeGetIoTarget(pUsbDeviceContext->OutputPipeHigh));

    if (!NT_SUCCESS(ntStatus)) {        
        WdfWaitLockRelease(pUsbDeviceContext->PipeStateLock);
        MpTrace(COMP_SEND, DBG_SERIOUS,("Failed to start OutputPipeHigh\n"));
        goto error;
    }
    
    ntStatus =  WdfIoTargetStart(WdfUsbTargetPipeGetIoTarget(pUsbDeviceContext->OutputPipeNormal));

    if (!NT_SUCCESS(ntStatus)) {
        WdfWaitLockRelease(pUsbDeviceContext->PipeStateLock);
        MpTrace(COMP_SEND, DBG_SERIOUS,("Failed to start OutputPipeNormal\n"));
        goto error;
    }

    WdfWaitLockRelease(pUsbDeviceContext->PipeStateLock);
    WdfTimerStart(pUsbDeviceContext->PendRequestTimer, WDF_REL_TIMEOUT_IN_MS(200));
    
error:    
    NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
    return ndisStatus;
}

NDIS_STATUS
HwUsbAllocateXmitResources(
    __in PNIC Nic,
    __in  ULONG NumTxd
    )
{
    BYTE  numRequests;

    if (NumTxd < MAX_WRITE_USB_REQUESTS) {
        numRequests =   MAX_WRITE_USB_REQUESTS;
    }
    else {
        numRequests = (BYTE)NumTxd;
    }
        
    HwUsbAllocateWriteRequests(Nic, numRequests);
    AllocatePendRequestsTimer(Nic);
    return  NDIS_STATUS_SUCCESS;
}


VOID
AllocatePendRequestsTimer(
    PNIC    Nic
    )
{
    WDF_OBJECT_ATTRIBUTES   attributes;
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    WDF_TIMER_CONFIG        timerConfig;
    PTIMER_NIC_CONTEXT      nicPendRequestContext;    
    NTSTATUS                ntStatus;
    
    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);
    WDF_TIMER_CONFIG_INIT_PERIODIC(
                               &timerConfig,
                               HwUsbPendRequestsTimer,
                               PERIODIC_INTERVAL
                               );

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(
                                   &attributes,
                                   TIMER_NIC_CONTEXT
                                   );
    attributes.ParentObject = Mp11GetWdfDevice(Nic->Adapter);
    ntStatus = WdfTimerCreate(
                             &timerConfig,
                             &attributes,
                             &(pUsbDeviceContext->PendRequestTimer)     // Output handle
                             );
    if (!NT_SUCCESS(ntStatus)) {
        MpTrace(COMP_SEND, DBG_SERIOUS,("Could not create Pend equest timer: status(0x%08X)", ntStatus));        
    }
    else {
        nicPendRequestContext = GetTimerContext(pUsbDeviceContext->PendRequestTimer);
        nicPendRequestContext->Nic = Nic;
    }
    
    return; 
}

BOOLEAN
AllocateWriteRequestsWorker(
    PNIC                    Nic,
    WDFUSBPIPE              UsbPipe,
    PUSB_WRITE_RESOURCES    WriteResources
    )
{
    WDFIOTARGET             ioTarget;
    WDF_OBJECT_ATTRIBUTES   requestAttributes;
    UCHAR                   requestIndex ;
    NTSTATUS                ntStatus;
    WDFMEMORY               urbMemory;
    WDF_OBJECT_ATTRIBUTES   objectAttribs;
    struct _URB_BULK_OR_INTERRUPT_TRANSFER *urbBuffer;
    CHAR                    dummyBuffer[1];  //Need a buffer to pass to UsbBuildInterruptOrBulkTransferRequest  

    ioTarget = WdfUsbTargetPipeGetIoTarget(UsbPipe);

    WDF_OBJECT_ATTRIBUTES_INIT(&objectAttribs);
    objectAttribs.ParentObject = Mp11GetWdfDevice( Nic->Adapter );
    ntStatus = WdfSpinLockCreate(&objectAttribs, &WriteResources->WriteRequestArrayLock);

    if (!NT_SUCCESS(ntStatus)) {
        MpTrace(COMP_SEND, DBG_SERIOUS,("Could not create request lock: status(0x%08X)", ntStatus));
        return FALSE;
    }

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&requestAttributes, USB_WRITE_REQ_CONTEXT);

    for (requestIndex = 0; requestIndex < WriteResources->MaxOutstandingWrites; requestIndex++)
    {
        WDFREQUEST* pNextRequest; 
        USBD_PIPE_HANDLE        usbdPipeHandle;
        PUSB_WRITE_REQ_CONTEXT writeContext;

   
        pNextRequest = &WriteResources->WriteRequestArray[requestIndex];
        ntStatus = WdfRequestCreate(&requestAttributes, ioTarget, pNextRequest);

        if (!NT_SUCCESS(ntStatus)) {
            MpTrace(COMP_SEND, DBG_SERIOUS,("Could not create request: status(0x%08X)", ntStatus));
            WriteResources->MaxOutstandingWrites = requestIndex;
            WriteResources->NextAvailableRequestIndex = requestIndex;
            return FALSE;
        }
        

        //
        // Preallocate the request timer to prevent the request from failing while trying to send it.
        //
        ntStatus = WdfRequestAllocateTimer(*pNextRequest);

        if (!NT_SUCCESS(ntStatus)){
            MpTrace(COMP_SEND, DBG_SERIOUS,("Could not allocate timer for  request status(0x%08X)", ntStatus));
            WriteResources->MaxOutstandingWrites = requestIndex;
            WriteResources->NextAvailableRequestIndex = requestIndex;
            return FALSE;

        }

        WDF_OBJECT_ATTRIBUTES_INIT(&objectAttribs);
        objectAttribs.ParentObject = *pNextRequest;

        ntStatus = WdfMemoryCreate(
                                &objectAttribs,
                                NonPagedPool,
                                0,
                                sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                                &urbMemory,
                                &urbBuffer);

        if (!NT_SUCCESS(ntStatus)) {
            MpTrace(COMP_SEND, DBG_SERIOUS,("Could not create request lock: status(0x%08X)", ntStatus));

            WriteResources->MaxOutstandingWrites = requestIndex;
            WriteResources->NextAvailableRequestIndex = requestIndex;
            return FALSE;
        }
               
        usbdPipeHandle = WdfUsbTargetPipeWdmGetPipeHandle(UsbPipe);
        //
        // NOTE : call    UsbBuildInterruptOrBulkTransferRequest otherwise 
        //    WdfUsbTargetPipeFormatRequestForUrb    will assert 
        //  with *** Assertion failed: Urb->UrbHeader.Length >= sizeof(_URB_HEADER)
        //
        UsbBuildInterruptOrBulkTransferRequest(
                            (PURB )urbBuffer,
                            sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                            usbdPipeHandle,
                            dummyBuffer,
                            NULL,
                            sizeof(dummyBuffer),
                            USBD_TRANSFER_DIRECTION_OUT | USBD_SHORT_TRANSFER_OK,
                            NULL);


        //
        // By calling  WdfUsbTargetPipeFormatRequestForUrb the frameworks allocate a lot of resources
        // like the underlying IRP for the request and hence it is better to do it at initilization 
        // to prevent an avoidable  failure later. 
        //
        ntStatus = WdfUsbTargetPipeFormatRequestForUrb(
                                            UsbPipe,
                                            *pNextRequest,
                                            urbMemory,
                                            NULL
                                            );

        
        if(!NT_SUCCESS(ntStatus)) {
            MpTrace (COMP_SEND, DBG_SERIOUS, (
                     "WdfUsbTargetPipeFormatRequestForUrb: Failed - 0x%x \n",
                     ntStatus));
            return FALSE;
        }

        //
        // set REQUEST_CONTEXT  parameters.
        //
        writeContext = GetWriteRequestContext(*pNextRequest);
        writeContext->UrbMemory       = urbMemory;
        writeContext->Urb = (PURB )urbBuffer;
        writeContext->UsbdPipeHandle       = usbdPipeHandle;
        writeContext->UsbPipe               = UsbPipe;
        writeContext->NdisContext           = Nic->Adapter;
        writeContext->Nic                   = Nic;
        writeContext->IoTarget              = ioTarget;
        
    }
    return TRUE;
}


BOOL
HwUsbAllocateWriteRequests(
    PNIC    Nic,
    BYTE    MaxOutstanding
    )
/*++

Routine Description
    Do this for low/meduim q and for high q
    Create write requests,
    Allocate         URB Memory
    Allocate         WDFMEMORY objects
    Setup the assocition between them in the Request context.
    TODO : Also, create an association between the NiCTXDU and the request. That way 
        data frames won't starve if there are many management frames as management frames
       can be initiated by the driver and not totally OS driven. 
    Also, preallocate the request timer here. Would it get lost on RequestReuse?

    

Arguments:
    Nic -- Pointer to the NIC data structure.
    MaxOutstanding -- Number of requests to allocate. 
                      Number of Beacon requests are fixed and allocated from a different pool.
     
                         
    
Return Value:

    
--*/
{
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    PUSB_WRITE_RESOURCES    writeResources;
    BOOLEAN                 retValue;
    WDFUSBPIPE              usbPipe;

    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);

    //
    // set a timer for 30 seconds for each request. This includes both data and mgmt. frames
    //
    WDF_REQUEST_SEND_OPTIONS_INIT(
                              &G_RequestOptions,
                              0
                              );
    //
    // Timeout of less than 20 secs is less desirable.
    //
    WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(
                                     &G_RequestOptions,
                                     WDF_REL_TIMEOUT_IN_SEC(30)
                                     );

    //
    // allocate resources for the Beacon/high endpoint for becaon frames 
    // and the low endpoint used for data and remaining mgmt. frames 
    //

    //        
    //low, normal  or high
    //
    writeResources = &pUsbDeviceContext->LowWrites;
    usbPipe = pUsbDeviceContext->OutputPipeNormal;
    writeResources->MaxOutstandingWrites = MaxOutstanding;
    writeResources->NextAvailableRequestIndex = MaxOutstanding;
    retValue = AllocateWriteRequestsWorker(Nic, usbPipe, writeResources);
    if (!retValue) {
        return retValue;
    }
    //
    // beacon 
    //
    writeResources = &pUsbDeviceContext->HighWrites;
    usbPipe = pUsbDeviceContext->OutputPipeHigh;
    writeResources->MaxOutstandingWrites = TOTAL_DESC_BEACON_QUEUE;
    writeResources->NextAvailableRequestIndex = TOTAL_DESC_BEACON_QUEUE;   
    retValue  =  AllocateWriteRequestsWorker(Nic, usbPipe, writeResources);
    
    return  retValue;       
}


VOID
FreeWriteRequestsWorker(
    PNIC    Nic,
    PUSB_WRITE_RESOURCES    WriteResources
    )
{
    USHORT requestIndex;

    UNREFERENCED_PARAMETER(Nic);
    for (requestIndex = 0; requestIndex < WriteResources->MaxOutstandingWrites; requestIndex++) {
        if (WriteResources->WriteRequestArray[requestIndex] != NULL) {
            WdfObjectDelete(WriteResources->WriteRequestArray[requestIndex]);
            WriteResources->WriteRequestArray[requestIndex] = NULL;
        }

    }

    if (WriteResources->WriteRequestArrayLock != NULL) {
        WdfObjectDelete(WriteResources->WriteRequestArrayLock);
        WriteResources->WriteRequestArrayLock = NULL;
    }

}



VOID
HwUsbFreeWriteRequests(
    PNIC    Nic
    )
{
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    PUSB_WRITE_RESOURCES    writeResources;
    
    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);
    //
    //low or normal  or high
    //
    writeResources  = &pUsbDeviceContext->LowWrites;
    FreeWriteRequestsWorker(Nic, writeResources);
    //
    //beacon
    // 
    writeResources  = &pUsbDeviceContext->HighWrites;
    FreeWriteRequestsWorker(Nic, writeResources);
            
}


WDFREQUEST
GetWriteRequest(
    PNIC    Nic,
    UCHAR   QueueType
    )
{
    WDFREQUEST writeRequest = NULL;
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    PUSB_WRITE_RESOURCES    writeResources;

    writeRequest = NULL;
    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);

    if (QueueType <= HIGH_QUEUE) {    //low or normal
            writeResources  = &pUsbDeviceContext->LowWrites;
    }
    else {    //beacon queue
            writeResources  = &pUsbDeviceContext->HighWrites;
    }
    
    WdfSpinLockAcquire(writeResources->WriteRequestArrayLock);

    if (writeResources->NextAvailableRequestIndex != 0) {   // Request is available
        --(writeResources->NextAvailableRequestIndex);
        writeRequest = writeResources->WriteRequestArray[writeResources->NextAvailableRequestIndex];
        writeResources->WriteRequestArray[writeResources->NextAvailableRequestIndex] = NULL;
    }
    
    WdfSpinLockRelease(writeResources->WriteRequestArrayLock);
    //
    // TODO : If we run out of Write requests then allocate on the fly 
    // instead of returning NULL
    //

    return writeRequest;
}

VOID
ReturnWriteRequest(
    __in PNIC     Nic,
    WDFREQUEST Request,
    ULONG      QueueType
    )
/*++

Routine Description

    Returns a request back to the available array

Arguments:
    Nic -- Pointer to the NIC data structure. 
    Request -- Handle of the WDFREQUEST which is being freed back to the available pool. 
    QueueType -- Used to decide to which pool the Request belongs
    
Return Value:

    
--*/
{
    PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
    PUSB_WRITE_RESOURCES    writeResources;
    WDF_REQUEST_REUSE_PARAMS    params;

    pUsbDeviceContext = GetUsbDeviceContext(Nic->UsbDevice);
    WDF_REQUEST_REUSE_PARAMS_INIT(&params, WDF_REQUEST_REUSE_NO_FLAGS, STATUS_SUCCESS);
    WdfRequestReuse(Request, &params);
    if (QueueType <= HIGH_QUEUE) {    //low or normal
        writeResources  = &pUsbDeviceContext->LowWrites;
    }
    else {    //beacon queue
        writeResources  = &pUsbDeviceContext->HighWrites;
    }

    WdfSpinLockAcquire(writeResources->WriteRequestArrayLock);

    ASSERTMSG("Request list is already full!",
              writeResources->NextAvailableRequestIndex < writeResources->MaxOutstandingWrites);
    writeResources->WriteRequestArray[writeResources->NextAvailableRequestIndex++] = Request;

    WdfSpinLockRelease(writeResources->WriteRequestArrayLock);
}


EVT_WDF_REQUEST_COMPLETION_ROUTINE WriteCompletionRoutine;

void
WriteCompletionRoutine(
    __in WDFREQUEST  Request,
    __in WDFIOTARGET  Target,
    __in PWDF_REQUEST_COMPLETION_PARAMS  CompletionParams,
    __in WDFCONTEXT  Context
    )
/*++

Routine Description

    Request - Handle to the WDFREQUEST which was used to send data to the USB target.
    Target - Handle to the Iotarget to which teh Request was sent. COnceptually this
              is the BULK USB __out pipe(on of  Data or beacon) 
    CompletionParams - In case of USB this contains the USB status and amount of bytes transferred 
                       
    Context - This is the COntext we set in WdfRequestSend 


Arguments:

    
Return Value:

    
--*/

{
    PUSB_WRITE_REQ_CONTEXT writeContext;
    PWDF_USB_REQUEST_COMPLETION_PARAMS usbCompletionParams;
    NTSTATUS                    status;
    PNIC_TX_MSDU                nicTxd;
    PNIC                        pNic;

    UNREFERENCED_PARAMETER(Target);
    writeContext = (PUSB_WRITE_REQ_CONTEXT)Context;
    
    status = CompletionParams->IoStatus.Status;
    nicTxd = writeContext->NicTxd;
    //
    // For usb devices, we should look at the Usb.Completion param.
    //
    usbCompletionParams = CompletionParams->Parameters.Usb.Completion;

    if (!NT_SUCCESS(status)) {
        MpTrace( COMP_SEND,DBG_SERIOUS,
            ("Write failed: request 0x%x Status 0x%x UsbdStatus 0x%x \n",
                Request, status, usbCompletionParams->UsbdStatus));

        if (nicTxd != NULL) {
            //
            // Indicate to NDIS that there was an error
            //
            nicTxd->bFailedDuringSend = TRUE;
            nicTxd->WaitSendToComplete = FALSE;
        }        
    }
    else if (nicTxd != NULL) {
        //
        // NOTE: this is used while indicating completion status to NDIS.
        //
        nicTxd->TxSucceeded = TRUE;
        nicTxd->WaitSendToComplete = FALSE;
        nicTxd->bFailedDuringSend = FALSE;
    }

    pNic = writeContext->Nic;

    //
    // inform NDIS
    //
    //3 Check Normal Queue(This should be transparent for upper layer)
    if (( (writeContext->QueueType == NORMAL_QUEUE) || 
            (writeContext->QueueType == HIGH_QUEUE) )  &&
        (HwGetTxBusyDescNum(writeContext->Nic, writeContext->QueueType) > 0)) {
        
       PTX_DESC_8187 pDesc;
       
       pDesc = (PTX_DESC_8187) HwGetNextDescToCheck(pNic, writeContext->QueueType, 0);
       HwIncrementTxNextDescToCheck(pNic, writeContext->QueueType, 1);
       if (pDesc == (PTX_DESC_8187)pNic->ManagementInfo.pPowerMgmtChangePktDesc && 
            (writeContext->QueueType == NORMAL_QUEUE)) {
           pNic->ManagementInfo.bPowerMgmtChangePktSent = TRUE;
           pNic->ManagementInfo.pPowerMgmtChangePktDesc = NULL;
       }

    }

    if (nicTxd != NULL) 
    {
        Mp11SendCompleteNBL(writeContext->NdisContext);
    }

    ReturnWriteRequest(writeContext->Nic, Request, writeContext->QueueType);
}

NTSTATUS
HwUsbSendPacketAsync (
    PNIC            Nic,
    UCHAR           QueueType,
    ULONG           BufferLength,   // Number of bytes in pData buffer
    PVOID           Buffer,
    PNIC_TX_MSDU    NicTxd
    )
/*++

Routine Description
    Nic - Pointer to HW NIC data structure 
    QueueType - This is used in deciding which queue to free the Request to. 
    Buffer - This is the TX buffer which has the TX desc. header in it. 
    NicTxd - This is the TX data structure maintained by the Bus independant HW portion. 
    
Arguments:

    
Return Value:

    
--*/
{
    WDFREQUEST      writeRequest;
    WDFUSBPIPE      wdfUsbWritePipe;
    NTSTATUS        ntStatus = NDIS_STATUS_SUCCESS;
    PUSB_WRITE_REQ_CONTEXT writeContext;    

    //
    // Get a request from the pool
    //
    writeRequest = GetWriteRequest(Nic, QueueType );

    if (writeRequest == NULL) {   // No requests left
        MpTrace(COMP_SEND, DBG_SERIOUS, ("HwUsbSendPacketAsync no req. left!!\n" ));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    writeContext = GetWriteRequestContext(writeRequest);
    writeContext->QueueType =  QueueType;
    writeContext->NicTxd = NicTxd;
    wdfUsbWritePipe = writeContext->UsbPipe;
    UsbBuildInterruptOrBulkTransferRequest(
                        writeContext->Urb,
                        sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                        writeContext->UsbdPipeHandle,
                        Buffer,
                        NULL,
                        BufferLength,
                        USBD_TRANSFER_DIRECTION_OUT | USBD_SHORT_TRANSFER_OK,
                        NULL);
    ntStatus = WdfUsbTargetPipeFormatRequestForUrb(wdfUsbWritePipe,
                                                   writeRequest,
                                                   writeContext->UrbMemory,
                                                   NULL  );
    
    if (!NT_SUCCESS(ntStatus)) {
        MpTrace(COMP_SEND, DBG_SERIOUS, ("HwUsbSendPacketAsync Failed to format requset for urb\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto error;
    }

    WdfRequestSetCompletionRoutine(writeRequest, WriteCompletionRoutine, writeContext);    
   
    if (WdfRequestSend(writeRequest, writeContext->IoTarget, &G_RequestOptions ) == FALSE) {   // Failure - Return request
        MpTrace(COMP_SEND, DBG_SERIOUS, ("HwUsbSendPacketAsync send failed\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;        
        goto error;
    }

    return ntStatus;

    error:
        ReturnWriteRequest(Nic, writeRequest, QueueType);
        MpTrace( COMP_SEND,DBG_LOUD, ("<-- HwUsbSendPacketAsync\n"));


    return ntStatus;
}































