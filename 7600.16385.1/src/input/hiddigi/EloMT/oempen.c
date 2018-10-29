/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    oempen.c

Abstract:

    Code for handling read requests

Author:
   Olumuyiwa Durojaiye Sept-20-2007

Environment:

    kernel mode only

Revision History:

--*/

#include "pch.h"

#define MODULE_ID                       1

extern USHORT g_wYMax;

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
*/
{


    NTSTATUS            status = STATUS_SUCCESS;
    
    KdPrint(("received read IRP!\n"));
        
    //queue the ping pong IRP.
    status = WdfRequestForwardToIoQueue(Request, 
        DevContext->PingPongQueue);
    if(NT_SUCCESS(status))
    {
        if(FALSE == DevContext->OemData.CycleStarted)
        {
            KdPrint(("Cycle:%d\n Sending Irp down", DevContext->OemData.CycleStarted));
            DevContext->OemData.CycleStarted = TRUE;
            //send the read irp down to retrieve data
            status = SerialAsyncReadData(DevContext,
                                              OEM_STATUS_CHECK,
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
                KdPrint(("Failed to send serial IRPdown\n"));
            }
        }
        else
        {
            if(NULL != fSentDown)
            {
              *fSentDown = TRUE;
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
    NTSTATUS status = STATUS_SUCCESS;    
    PDEVICE_EXTENSION DevContext = (PDEVICE_EXTENSION)Context;
    ULONGLONG BeforeTime, AfterTime;
    
    
    UNREFERENCED_PARAMETER(Target);
    status = WdfRequestGetStatus(Request);

    if (status == STATUS_CANCELLED)
    {
        KdPrint(("ReadReport IRP was cancelled.\n"));       
    }
    else if (!NT_SUCCESS(status))
    {
        KdPrint(("Failed to read input data packet (status=%x).\n", status));       
    }
    
    if(NT_SUCCESS(status))
    {
        BeforeTime = KeQueryInterruptTime();
        status = ProcessInputData(DevContext, 
                                Params);
        if(!NT_SUCCESS(status))
        {
           KdPrint(("Input processing failed (status=%x).\n", status));
        }
        AfterTime = KeQueryInterruptTime();
        AfterTime -= BeforeTime;
        AfterTime /= 10000;
        TInfo(("Packet Processing Time: %d", (ULONG)AfterTime));
    }
    else
    {
        KdPrint(("Read IRP failed. Read more data\n"));
        status = SerialRequestMoreData(DevContext);
        if(NT_SUCCESS(status))
        {
            KdPrint(("Failed to request more bytes!\n"));
        }
    }          
    //delete the serial request (no matter what) since its an IRP we created. 
    WdfObjectDelete( Request);
    TExit(Func, ("=%x", status));    
}       


NTSTATUS INTERNAL
ProcessInputData(
    __in PDEVICE_EXTENSION DevContext,
    __in PWDF_REQUEST_COMPLETION_PARAMS Params
    )
/*

Routine Description:

  This function runs the received request from the serial driver through some processing

Arguments:
  DevContext - Device context

  Params - Completion parameters

Return Values:
   on success STATUS_SUCCESS is returned
   If more data is needed from the serial driver STATUS_MORE_PROCESSING_REQUIRED is returned
   A failure code acn also be returned if one of the call in the function failes
*/
{
    NTSTATUS                status = STATUS_SUCCESS;
    ULONG_PTR               serialBytes = 0;
    PUCHAR					RawSerialPacket = NULL;
    OEM_RAW_SERIAL_PACKET_2 SerialDataPacket = {0};
    WDFREQUEST              PingPongRequest = {0};
    WDFMEMORY               wdfSerialMem = {0};
    
    
    PoSetSystemState(ES_USER_PRESENT);
    
    serialBytes = Params->IoStatus.Information;

    wdfSerialMem = Params->Parameters.Read.Buffer;
    RawSerialPacket = WdfMemoryGetBuffer(wdfSerialMem,
                        NULL);
    //data is available in the resync buffer append what just came in and see if we have good data
    TInfo(("appending fresh data bytes received: %d\n", (ULONG)serialBytes));
    WdfSpinLockAcquire(DevContext->SpinLock);
    RtlCopyMemory((PUCHAR)&DevContext->OemData.ResyncData[0]+ DevContext->OemData.BytesInBuff, 
                    RawSerialPacket,
                    serialBytes);
    TInfo(("Resync bytes before count add:%d\n", (ULONG)DevContext->OemData.BytesInBuff));
    DevContext->OemData.BytesInBuff += serialBytes;
    TInfo(("Resync bytes after count add:%d\n",(ULONG)DevContext->OemData.BytesInBuff));
    if(ELO_SINGLE_TOUCH_COUNT <= DevContext->OemData.BytesInBuff)
    {
        status = ProcessOEMBuffer(DevContext, &SerialDataPacket);
        if(NT_SUCCESS(status))
        {
            //get ping pong IRP
            status = WdfIoQueueRetrieveNextRequest(DevContext->PingPongQueue, 
                            &PingPongRequest);

            if(!NT_SUCCESS(status))
            {
                TInfo(("No Pending pingpong IRP!\n"));
            }
            else
            {
                status = TranslateRawDataToHID(DevContext,
                                &SerialDataPacket,
                                PingPongRequest);
                if(!NT_SUCCESS(status))
                {
                    TErr(("Failed to translate data status=%x\n", status));
                }
            }
        }
    }
    WdfSpinLockRelease(DevContext->SpinLock);
     
    status = SerialRequestMoreData(DevContext);
    TExit(Func, ("=%x", status));
    return status;
}
VOID INTERNAL
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
    ULONG_PTR           bytesToRead = 0;
    PDEVICE_EXTENSION   devContext = NULL;
    

    pItemContext = GetWorkItemContext(WorkItem);
    devContext = pItemContext->devContext;
    
    WdfSpinLockAcquire(devContext->SpinLock);
    
    KdPrint(("READMOREBYTES buffer Count:%d\n", devContext->OemData.BytesInBuff));
    if(0x02 == devContext->OemData.BytesInBuff)
    {
        PUCHAR   ResyncData = (PUCHAR)&devContext->OemData.ResyncData;
        status = ValidateOEMHeader(ResyncData, (PULONG)&devContext->OemData.BytesInBuff);
        //we have the status field and contact 2 field
        if(NT_SUCCESS(status))
        {
            //both bytes are valid the contact count determins the number of bytes to retrieve.
            bytesToRead = ELO_TWO_TOUCH_COUNT - devContext->OemData.BytesInBuff;
            
            if(0x01 == devContext->OemData.ResyncData[0].RawSerialData.Contacts )
            {
                bytesToRead = ELO_SINGLE_TOUCH_COUNT - devContext->OemData.BytesInBuff;
            }
        }
        else
        {
            //we have 1 or no valid bytes
            KdPrint(("a least on byte is invalid! buffer count:%d\n", devContext->OemData.BytesInBuff));
            bytesToRead = ELO_MT_STATUS_COUNT - devContext->OemData.BytesInBuff;
            if(devContext->OemData.BytesInBuff)
            {
                RtlMoveMemory((PUCHAR)&devContext->OemData.ResyncData[0], 
                    ((PUCHAR)&devContext->OemData.ResyncData[0])+1,
                    devContext->OemData.BytesInBuff);
            }
        }
    }
    else if(0x00 == devContext->OemData.BytesInBuff)
    {
        bytesToRead = ELO_STATUS_COUNT;
    }
    else if(0x01 == devContext->OemData.BytesInBuff)
    {
        bytesToRead = ELO_FINGER_COUNT;
    }
    else if(0x01 == devContext->OemData.ResyncData[0].RawSerialData.Contacts)
    {
        //we have an unexpected amount of data likely due to serial port noise. retrieve enough data for
        //one touch
        bytesToRead = ELO_SINGLE_TOUCH_COUNT - devContext->OemData.BytesInBuff;
    }
    else if(0x02 == devContext->OemData.ResyncData[0].RawSerialData.Contacts)
    {
        //we have an unexpected amount of data likely due to serial port noise. retrieve enough data for
        //2 fingers
        
        bytesToRead = ELO_TWO_TOUCH_COUNT - devContext->OemData.BytesInBuff;
    }
    else{
        //we have an unexpected number of bytes. read enough for single touch but we already know this data won't pass validation.
        //the validation code will shift the bytes until it finds a valid sequence.
        KdPrint(("READMOREBYTES: buffer Count:%d\n", devContext->OemData.BytesInBuff));
        if(devContext->OemData.BytesInBuff > ELO_SINGLE_TOUCH_COUNT && devContext->OemData.BytesInBuff < ELO_TWO_TOUCH_COUNT)
        {            
            bytesToRead = ELO_TWO_TOUCH_COUNT - devContext->OemData.BytesInBuff;
        }
        else if(devContext->OemData.BytesInBuff < ELO_SINGLE_TOUCH_COUNT)
        {
            bytesToRead = ELO_SINGLE_TOUCH_COUNT - devContext->OemData.BytesInBuff;
        }
        else if (devContext->OemData.BytesInBuff == ELO_SINGLE_TOUCH_COUNT)
        {
            devContext->OemData.BytesInBuff--;
            bytesToRead = ELO_SINGLE_TOUCH_COUNT - devContext->OemData.BytesInBuff;
            RtlMoveMemory((PUCHAR)&devContext->OemData.ResyncData[0], 
                    ((PUCHAR)&devContext->OemData.ResyncData[0])+1,
                    devContext->OemData.BytesInBuff);
        }
        else
        {
            //the resync buffer has more bytes than the size of a full multi-touch packet. This should never happen!!
            ASSERT(FALSE);
        }
          
    }
    WdfSpinLockRelease(devContext->SpinLock);

    KdPrint(("Read %d more bytes\n", bytesToRead));
    
    status = SerialAsyncReadData(devContext,
                             (ULONG)bytesToRead,
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

    DevContext->OemData.CycleStarted = FALSE;
    TExit(Func, ("=%x", status));
    return status;
}

NTSTATUS INTERNAL
NInputStandbyDevice(
   __in WDFDEVICE Device
   )
{
    NTSTATUS status = STATUS_SUCCESS;
    
    UNREFERENCED_PARAMETER(Device);
    TEnter(Func, ("(Device=%p)", Device));
    //command to send to device to a lower power state should go here.
    //this particular device doesn't support power states so there's nothing to 
    // do. The Wacom KMDF sample sends in command in this method. 
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
  SUCCESS - Returns STATUS_SUCCESS.
  FAILURE - Returns NT status code.

--*/

{
    NTSTATUS                status = STATUS_SUCCESS;
    PHID_XFER_PACKET        FeaturePacket  = NULL;
    PDEVICE_EXTENSION       deviceInfo = NULL;
    WDF_REQUEST_PARAMETERS  params = {0};

    

    UNREFERENCED_PARAMETER(Device);
    WDF_REQUEST_PARAMETERS_INIT(&params);
    WdfRequestGetParameters(Request, &params);

    KdPrint(("Updating config mode\n"));
    deviceInfo = GetDeviceContext(WdfIoQueueGetDevice(WdfRequestGetIoQueue(Request)));

    //
    // IOCTL_HID_SET_FEATURE & IOCTL_HID_GET_FEATURE are not METHOD_NIEHTER
    // IOCTLs. So you cannot retreive UserBuffer from the IRP using Wdf
    // function. As a result we have to escape out to WDM to get the UserBuffer
    // directly from the IRP. 
    //

    if (params.Parameters.DeviceIoControl.InputBufferLength < sizeof(HID_XFER_PACKET)) {
        status = STATUS_BUFFER_TOO_SMALL;
        KdPrint(("InputBufferLength:%d is too smalll!\n", params.Parameters.DeviceIoControl.InputBufferLength));
        return status;
    }
    
    //
    // This is a kernel buffer so no need for try/except block when accesssing
    // Irp->UserBuffer.
    //
    FeaturePacket = (PHID_XFER_PACKET) WdfRequestWdmGetIrp(Request)->UserBuffer;
    if (FeaturePacket == NULL) {
        status = STATUS_INVALID_DEVICE_REQUEST;
        KdPrint(("FeaturePacket == NULL!\n"));
        return status;
    }
    if(FeaturePacket->reportBufferLen < sizeof(HID_FEATURE_REPORT))
    {
        status = STATUS_INVALID_BUFFER_SIZE;
        KdPrint(("Wrong feature report length expected: %d, actual:%d !\n", sizeof(HID_FEATURE_REPORT), FeaturePacket->reportBufferLen));
    }
    else
    {
        
        PHID_FEATURE_REPORT InputModeReport = (PHID_FEATURE_REPORT)FeaturePacket->reportBuffer;
        
        if(InputModeReport->ReportID == REPORTID_FEATURE)
        {
            KdPrint(("new mode:%d\n", InputModeReport->InputMode));
            KdPrint(("current mode:%d\n", deviceInfo->InputMode));
            if((MODE_MOUSE == InputModeReport->InputMode)
#ifdef MULTITOUCH                 
                ||
                (MODE_SINGLE_TOUCH ==  InputModeReport->InputMode) ||
                (MODE_MULTI_TOUCH == InputModeReport->InputMode) 
#endif
                )
            {
                deviceInfo->InputMode = InputModeReport->InputMode;
            }
            else
            {
                status = STATUS_INVALID_PARAMETER;
                KdPrint(("Invalid input mode"));
            }
        }
        else
        {
            KdPrint(("wrong report id:%d\n", InputModeReport->ReportID));
        }
    } 
   
    TExit(Func, ("=%x", status));
    return status;
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
  SUCCESS - Returns STATUS_SUCCESS.
  FAILURE - Returns NT status code.

--*/
{
    NTSTATUS                 status = STATUS_SUCCESS;
    PHID_XFER_PACKET         FeaturePacket  = NULL;
    PDEVICE_EXTENSION        deviceInfo = NULL;
    WDF_REQUEST_PARAMETERS   params = {0};

    
    UNREFERENCED_PARAMETER(Device);

    WDF_REQUEST_PARAMETERS_INIT(&params);
    WdfRequestGetParameters(Request, &params);

    KdPrint(("Retrieving config mode\n"));
    deviceInfo = GetDeviceContext(WdfIoQueueGetDevice(WdfRequestGetIoQueue(Request)));

    //
    // IOCTL_HID_SET_FEATURE & IOCTL_HID_GET_FEATURE are not METHOD_NIEHTER
    // IOCTLs. So you cannot retreive UserBuffer from the IRP using Wdf
    // function. As a result we have to escape out to WDM to get the UserBuffer
    // directly from the IRP. 
    //

    if (params.Parameters.DeviceIoControl.OutputBufferLength < sizeof(HID_XFER_PACKET)) {
        status = STATUS_BUFFER_TOO_SMALL;
        KdPrint(("Output Buffer:%d is too smalll!\n", params.Parameters.DeviceIoControl.OutputBufferLength));
       
    }
    else {

        FeaturePacket = (PHID_XFER_PACKET) WdfRequestWdmGetIrp(Request)->UserBuffer;
        if (FeaturePacket == NULL) {
            status = STATUS_INVALID_DEVICE_REQUEST;
            KdPrint(("FeaturePacket == NULL!\n"));            
        }
        else
        {        
            UCHAR ReportId = *(PUCHAR)FeaturePacket->reportBuffer;
            if(REPORTID_FEATURE == ReportId &&
                FeaturePacket->reportBufferLen >= sizeof(HID_FEATURE_REPORT))
            {  
               PHID_FEATURE_REPORT InputModeReport = (PHID_FEATURE_REPORT)FeaturePacket->reportBuffer; 
               InputModeReport->InputMode = deviceInfo->InputMode;
                
            }
            else if(REPORTID_MAX_COUNT == ReportId && 
                FeaturePacket->reportBufferLen >= sizeof(HID_MAX_COUNT_REPORT))
            {                
               PHID_MAX_COUNT_REPORT MaxCountReport = (PHID_MAX_COUNT_REPORT)FeaturePacket->reportBuffer;
               MaxCountReport->MaxCount = ELO_MT_MAX_COUNT;
            }
            else
            {
                status = STATUS_INVALID_PARAMETER;
                KdPrint(("Wrong feature report id or length expected ids: Feauture:%d,  MaxCount:%d actual:%d  expected length: Feature:%d, or MaxCount:%d actual:%d!\n", 
                    REPORTID_FEATURE, REPORTID_MAX_COUNT, ReportId, sizeof(HID_FEATURE_REPORT), sizeof(HID_MAX_COUNT_REPORT), FeaturePacket->reportBufferLen));
            }
        } 
        
    }   
    TExit(Func, ("=%x", status));
    return status;
}

NTSTATUS
INTERNAL
ValidateOEMHeader(
    __in_bcount(*BytesToValidate) PUCHAR TouchHeader, 
    __inout PULONG BytesToValidate
    )
    /*++

Routine Description:

   validates device data header button state and number of contacts

Arguments:

   TouchHeader - Pointer to header

   BytesToValidate - Number of bytes to validate(The function will only accept a values 1 and  2 (button status and contact count. returns number of valid bytes in buffer

Return Value:
  SUCCESS - Returns STATUS_SUCCESS.
  FAILURE - Returns NT status code.

--*/
{
    NTSTATUS  status = STATUS_DEVICE_DATA_ERROR;
    
    if(NULL != TouchHeader && NULL != BytesToValidate && *BytesToValidate > 0 && *BytesToValidate < 3)
    {
        if(ISVALIDSTATUS(TouchHeader[0]))   
        {
            //contact count must be one or 2        
            if(*BytesToValidate == 2 && ELO_MT_CONTACT_1 != TouchHeader[1] && ELO_MT_CONTACT_2 != TouchHeader[1])
            {
                //if the second byte isn't valid the validity of the first byte is of no use to us.
                *BytesToValidate = 0;
                KdPrint(("invalid contact count!\n"));
                status = STATUS_DEVICE_DATA_ERROR;  
            }
            else if(*BytesToValidate == 2)
            {
                KdPrint(("both bytes are valid\n"));
                status = STATUS_SUCCESS;
            }
        }
        else if(*BytesToValidate == 2)
        {
            //first byte (buttons status) is invalid. we default to invalidating both bytes.
            //then if the second byte is a valid button status we let the caller know            
            *BytesToValidate = 0;
            if(ISVALIDSTATUS(TouchHeader[1]))
            {
                *BytesToValidate = 1;
                status = STATUS_SUCCESS;
            }
        }
    }
    else
    {
        status = STATUS_INVALID_PARAMETER;
        KdPrint(("received bad function arguments\n"));
    }
    TExit(Func, ("=%x", status));
    return status;
}

