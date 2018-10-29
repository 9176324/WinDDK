/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    hid.c

Abstract:

    Code for handling HID related requests

Author:
   Olumuyiwa Durojaiye 12-4-2007

Environment:

    kernel mode only

Revision History:

--*/

#include "pch.h"

#define MODULE_ID                       1



NTSTATUS INTERNAL
NInputReadReport(
    __in  PDEVICE_EXTENSION DevContext,
    __in  WDFREQUEST        Request,
    __out BOOLEAN           *fSentDown
    )
/*++

Routine Description:

   Handle read request from HIDCLASS

Arguments:
   DevContext - Device Context

   Request - Read request

   fSentDown - flag to monitor if the request was sent down the stack

Return Value:

   On success, the function returns STATUS_SUCCESS
   On failure it passes the relevant error code to the caller.
--*/
{


    NTSTATUS status = STATUS_SUCCESS;
    
    TEnter(Func, ("(DevContext=%p)", DevContext));

    
    WdfSpinLockAcquire(DevContext->SpinLock);
    status = ProcessResyncBuffer(DevContext, Request);
    WdfSpinLockRelease(DevContext->SpinLock);


    if (!NT_SUCCESS(status))
    {
        //
        // If we don't have enough bytes in the resync buffer or the packet
        // in the resync buffer is invalid, send an IRP down to read some
        // more.
        //

        //queue the ping pong IRP.
        status = WdfRequestForwardToIoQueue(Request, 
            DevContext->PingPongQueue);
        if(NT_SUCCESS(status))
        {
            //send the read irp down to retrieve data
            
               status = SerialAsyncReadData(DevContext,
                                                  sizeof(OEM_INPUT_REPORT),
                                                  NInputReadReportCompletion,
                                                  DevContext);
               //if the IRP is sent down the stack it should not be touched
               //anymore.
               if(NT_SUCCESS(status))
                {
                    if(NULL != fSentDown)
                    {
                      *fSentDown = TRUE;
                    }
                    else
                    {
                        KdPrint(("fSentDown is NULL!\n"));
                    }
                }
                else
                {
                    KdPrint(("Failed to send serial IRP down\n"));
                }
               
           
        }
    }
    TExit(Func, ("=%x", status));
    return status;
}  


VOID
NInputReadReportCompletion(
    __in WDFREQUEST    Request,
    __in WDFIOTARGET   Target,
    __in PWDF_REQUEST_COMPLETION_PARAMS Params,
    __in WDFCONTEXT    Context
    )
 /*++

Routine Description:

   Read request completion routine.

Arguments:
   Request - WDF request

   Target - IOTarget

   Params - Completion parameters

   Context - Completion routine context

   Return Value:

   On success, the function returns STATUS_SUCCESS
   On failure it passes the relevant error code to the caller.

*/
{
    NTSTATUS            status = STATUS_SUCCESS;
    PHID_INPUT_REPORT   HidReport = NULL;
    PDEVICE_EXTENSION   DevContext = (PDEVICE_EXTENSION)Context;    
    WDFREQUEST          PingPongRequest = {0};
    PCHAR               buffer = NULL;
    size_t              bufferLength = 0;
    size_t              serialBufferLength = 0;
    WDFMEMORY           wdfSerialMem = {0};
    PUCHAR              RawSerialPacket = NULL;
    BOOLEAN             CompleteRequest = TRUE;
    size_t              InfoCount = 0;
    
    
    UNREFERENCED_PARAMETER(Target);
    status = WdfRequestGetStatus(Request);

    if (status == STATUS_CANCELLED)
    {
        TWarn(("ReadReport IRP was cancelled."));
        status = STATUS_SUCCESS;
    }
    else if (!NT_SUCCESS(status))
    {
        TWarn(("Failed to read input data packet (status=%x).", status));
        status = STATUS_SUCCESS;
    }
    else
    {
        
        status = WdfIoQueueRetrieveNextRequest(DevContext->PingPongQueue, 
                                &PingPongRequest);
        if(NT_SUCCESS(status))
        {
            status = WdfRequestRetrieveOutputBuffer(PingPongRequest, 
                                        sizeof(HID_INPUT_REPORT), 
                                        &buffer, 
                                        &bufferLength);

            if(NT_SUCCESS(status))
            {
                HidReport = (PHID_INPUT_REPORT)buffer;
                serialBufferLength = Params->IoStatus.Information;
                wdfSerialMem = Params->Parameters.Read.Buffer;
                RawSerialPacket = WdfMemoryGetBuffer(wdfSerialMem,
                        NULL);
                
                if(serialBufferLength <= sizeof(OEM_INPUT_REPORT))
                {
                    ULONGLONG BeforeTime, AfterTime;
                    BeforeTime = KeQueryInterruptTime();
                    RtlCopyMemory(&HidReport->Report, 
                                RawSerialPacket, 
                                serialBufferLength);
                   
                    status = ProcessInputData(DevContext, 
                                            PingPongRequest, 
                                            HidReport,
                                            serialBufferLength);
                    AfterTime = KeQueryInterruptTime();
                    AfterTime -= BeforeTime;
                    AfterTime /= 10000;
                    TInfo(("Packet Processing Time: %d", (ULONG)AfterTime));                   
                    if(!NT_SUCCESS(status))
                    {
                       //no point in wasting the HIDCLASS IRP. Lets reset its buffer and put it back in the queue
                        RtlZeroMemory(HidReport, sizeof(HID_INPUT_REPORT));
                        KdPrint(("Requeuing request\n"));
                        status = WdfRequestRequeue(PingPongRequest);
                        if(NT_SUCCESS(status))
                        {
                            CompleteRequest = FALSE;
                        }
                        else
                        {
                            //we are unable to requeue the request for some reason. We need to complete the request and report
                            TErr(("Unable to requeue ping pong IRP status=%x", status));
                        } 
                    }
                    else
                    {
                        // we successfully retrived data for delivery to HIDCLASS
                        InfoCount = sizeof(HID_INPUT_REPORT);
                    }
                }                
            }
            if(CompleteRequest)
            {
                    WdfRequestCompleteWithInformation(PingPongRequest,
                                                    status,
                                                    InfoCount);
            }

        }

    }      
    //delete the serial request (no matter what) since its an IRP we created. 
    // look into resuing an IRP isntead.
    WdfObjectDelete( Request);
    TExit(Func, ("=%x", status));    
}       //ReadReportCompletion


NTSTATUS INTERNAL
ProcessResyncBuffer(
    __in PDEVICE_EXTENSION DevContext,
    __in WDFREQUEST        Request
    )
    /*++

    Routine Description:
      This function checks the resync buffer for valid data.

    Arguments:
      DevContext - Device Context

      Request - WDF Request

    Return Value:

     On success, the function returns STATUS_SUCCESS
     On failure it passes the relevant error code to the caller.

     
    --*/
{
    NTSTATUS            status = STATUS_DATA_ERROR;
    PCHAR               buffer  = NULL;
    size_t              bufferLength = 0;
    PHID_INPUT_REPORT   HidReport = NULL;

    TEnter(Func, ("(DevContext=%p,Len=%d,status=%x,xData=%x,yData=%x)",
                  DevContext, DevContext->OemData.BytesInBuff,
                  DevContext->OemData.ResyncData[0].InputReport.bStatus,
                  DevContext->OemData.ResyncData[0].InputReport.wXData,
                  DevContext->OemData.ResyncData[0].InputReport.wYData));

    TAssert(KeGetCurrentIrql() == DISPATCH_LEVEL);
    
    status = WdfRequestRetrieveOutputBuffer(Request,
                                                sizeof(HID_INPUT_REPORT),
                                                &buffer,
                                                &bufferLength);
    if(NT_SUCCESS(status))
    {
        HidReport = (PHID_INPUT_REPORT)buffer;
        //reset failure status so we don't return success if we don't go
        //into the while loop
        status = STATUS_DATA_ERROR;
        while (DevContext->OemData.BytesInBuff >= sizeof(OEM_INPUT_REPORT))
        {
            if (OemIsResyncDataValid(DevContext))
            {
                status = OemNormalizeInputData(DevContext,
                                               &DevContext->OemData.ResyncData[0]);
                if (NT_SUCCESS(status))
                {
                    HidReport->ReportID = REPORTID_PEN;
#ifdef _TIMESTAMP_
                    HidReport->TimeStamp = DevContext->OemData.LastCounter;
#endif
                    RtlCopyMemory(HidReport->Report.RawInput,
                                  &DevContext->OemData.ResyncData[0],
                                  sizeof(OEM_INPUT_REPORT));
                    WdfRequestSetInformation(Request, sizeof(HID_INPUT_REPORT));
                }

                DevContext->OemData.BytesInBuff -= sizeof(OEM_INPUT_REPORT);
                if (DevContext->OemData.BytesInBuff > 0)
                {
                    RtlMoveMemory(&DevContext->OemData.ResyncData[0],
                                  &DevContext->OemData.ResyncData[1],
                                  DevContext->OemData.BytesInBuff);
                }

                if (NT_SUCCESS(status))
                {
                    break;
                }
            }
        }
    }
    TExit(Func, ("=%x", status));
    return status;
}//ProcessResyncBuffer


NTSTATUS INTERNAL
ProcessInputData(
    __in PDEVICE_EXTENSION DevContext,
    __in WDFREQUEST    Request,
    __in PHID_INPUT_REPORT HidReport,
    __in ULONG_PTR     SerialByteCount
    )
    /*++

    Routine Description:
      Validates input from serial driver

    Arguments:
      DevContext - Device Context

      Request - WDF request

      HidReport - report buffer

      SerialByteCount - Number of bytes read from serial driver

    Return Value:

     On success, the function returns STATUS_SUCCESS
     On failure it passes the relevant error code to the caller.

    --*/
{
    NTSTATUS status = STATUS_SUCCESS;
    
    TEnter(Func,("(DevContext=%p)",DevContext));
   

    WdfSpinLockAcquire(DevContext->SpinLock);
    
    if ((DevContext->OemData.BytesInBuff == 0) &&
        (SerialByteCount == sizeof(OEM_INPUT_REPORT)) &&
        IsValidPacket(&HidReport->Report))
    {
        status = OemNormalizeInputData(DevContext, &HidReport->Report);
        if (NT_SUCCESS(status))
        {
            HidReport->ReportID = REPORTID_PEN;
#ifdef _TIMESTAMP_
            HidReport->TimeStamp = DevContext->OemData.LastCounter;
#endif
            
            WdfRequestSetInformation(Request, sizeof(HID_INPUT_REPORT));
        }
    }
    else
    {
        //
        // Either resync buffer already has something in it or packet is
        // partial or invalid, so append data to resync buffer and process
        // it again.
        //

        RtlMoveMemory((PUCHAR)&DevContext->OemData.ResyncData[0] +
                      DevContext->OemData.BytesInBuff,
                      &HidReport->Report,
                      SerialByteCount);
        DevContext->OemData.BytesInBuff += (ULONG)SerialByteCount;
        TAssert(DevContext->OemData.BytesInBuff <= sizeof(DevContext->OemData.ResyncData));
        status = ProcessResyncBuffer(DevContext, Request);
    }

    if (!NT_SUCCESS(status))
    {
        
        status = SerialRequestMoreData(DevContext);
              
    }
    WdfSpinLockRelease(DevContext->SpinLock);

    TExit(Func, ("=%x (status=%x,xData=%x,yData=%x)",
                 status, HidReport->Report.InputReport.bStatus,
                 HidReport->Report.InputReport.wXData,
                 HidReport->Report.InputReport.wYData));
    return status;
}       //ProcessInputData


VOID 
ReadMoreBytes(
    __in WDFWORKITEM WorkItem
    )
    /*
Routine Description:
   Work item function for reading more data from the serial driver

Arguments:

  WorkItem - work item context

Return Value:
   void function.

*/
{
    PWORKITEM_CONTEXT   pItemContext = NULL;
    NTSTATUS            status = STATUS_SUCCESS;
    ULONG               bytesToRead = 0;
    PDEVICE_EXTENSION   devContext = NULL;
    

    pItemContext = GetWorkItemContext(WorkItem);
    devContext = pItemContext->devContext;
    
    WdfSpinLockAcquire(devContext->SpinLock);
    bytesToRead = sizeof(OEM_INPUT_REPORT) - 
                        devContext->OemData.BytesInBuff%sizeof(OEM_INPUT_REPORT);
    WdfSpinLockRelease(devContext->SpinLock);

    KdPrint(("Read %d more bytes", bytesToRead));
    
    status = SerialAsyncReadData(devContext,
                             bytesToRead,
                             NInputReadReportCompletion,
                             NULL);
    if(!NT_SUCCESS(status)){

        TErr(("SerialAsyncReadData failed! staus=0x%x", status));

    }
    WdfObjectDelete(WorkItem);
    return;    
}       //ReadMoreBytes

NTSTATUS INTERNAL
NInputWakeDevice(
   __in WDFDEVICE Device
   )
{
    NTSTATUS status = STATUS_SUCCESS;
    PDEVICE_EXTENSION   DevContext = GetDeviceContext(Device);

    TEnter(Func, ("(Device=%p)", Device));

    status = SerialAsyncReadWritePort(FALSE,
                              DevContext,
                              (PUCHAR)"-1",
                              2);
    if(!NT_SUCCESS(status))
    {
        TWarn(("Failed to wake the digitizer (status=%x).", status));
    }
    TExit(Func, ("=%x", status));
    return status;
}

NTSTATUS INTERNAL
NInputStandbyDevice(
   __in WDFDEVICE Device
   )
{
    NTSTATUS status = STATUS_SUCCESS;
    PDEVICE_EXTENSION   DevContext = GetDeviceContext(Device);

    TEnter(Func, ("(Device=%p)", Device));

    status = SerialAsyncReadWritePort(FALSE,
                              DevContext,
                              (PUCHAR)"+",
                              1);
     if(!NT_SUCCESS(status))
     {
         TWarn(("Failed to send StandBy command (status=%x).", status));
     }
     TExit(Func, ("=%x", status));
    return status;
}

NTSTATUS INTERNAL
NInputSetFeatureReport(
    __in WDFDEVICE Device,
    __in WDFREQUEST Request
    )
/*++

Routine Description:

   Sets feature report

Arguments:
 
   Device - WDF device

   Request - WDF request

Return Value:
  STATUS_NOT_SUPPORTED

--*/

{
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(Request);
    return STATUS_NOT_SUPPORTED;
}

NTSTATUS INTERNAL
NInputGetFeatureReport(
    __in WDFDEVICE Device,
    __in WDFREQUEST Request
    )
/*++

Routine Description:

   Gets feature report

Arguments:

   Device - WDF device

   Request - WDF request

Return Value:
  STATUS_NOT_SUPPORTED

--*/
{
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(Request);
    return STATUS_NOT_SUPPORTED;
}
