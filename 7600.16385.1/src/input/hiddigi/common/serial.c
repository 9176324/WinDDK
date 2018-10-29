/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    serial.c

Abstract:

    serial interface functions

Author:
   Olumuyiwa Durojaiye Sept-20-2007

Environment:

    kernel mode only

Revision History:

--*/

#include "pch.h"
#define MODULE_ID                       3


NTSTATUS INTERNAL
SerialSyncSendIoctl(
    __in ULONG          IoctlCode,
    __in WDFIOTARGET    nextLowerDriver,
    __in_bcount_opt(InBufferLen) PVOID      InBuffer,
    __in ULONG          InBufferLen,
    __out_bcount_opt(OutBufferLen) PVOID     OutBuffer,
    __in ULONG          OutBufferLen,
    __in BOOLEAN        fInternal
    )
/*
Routine Description:
  This function is used to send IOCTL codes to the serial driver

Arguments:
 
  IoctlCode - Code to send to the serial driver
  
  nextLowerDriver - IOTarget for IOCTL

  InBuffer - Input buffer

  InBufferLen - Input buffer length

  OutBuffer - Input buffer

  OutBufferLen - Input buffer length

Return Value:
 On success, the function return STATUS_SUCCESS
 On failure, it returns the error code received from the serial driver
*/
{
    
    NTSTATUS                   status = STATUS_SUCCESS;
    PWDF_MEMORY_DESCRIPTOR     inputMemoryDescriptorLoc = NULL;
    PWDF_MEMORY_DESCRIPTOR     outputMemoryDescriptorLoc = NULL;
    WDF_MEMORY_DESCRIPTOR      inputMemoryDescriptor = {0};
    WDF_MEMORY_DESCRIPTOR      outputMemoryDescriptor = {0};
    WDF_REQUEST_SEND_OPTIONS   options = {0};
       
    if(NULL != InBuffer)
    {

        WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&inputMemoryDescriptor,
                                            InBuffer,
                                            InBufferLen);
        inputMemoryDescriptorLoc = &inputMemoryDescriptor;
    }
    if(NULL != OutBuffer)
    {
        WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&outputMemoryDescriptor,
                                            OutBuffer,
                                            OutBufferLen);
        outputMemoryDescriptorLoc = &outputMemoryDescriptor;
    }

    WDF_REQUEST_SEND_OPTIONS_INIT(
                                &options,
                                WDF_REQUEST_SEND_OPTION_SYNCHRONOUS 
                                );

    WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(
                                    &options,
                                    WDF_REL_TIMEOUT_IN_SEC(1)
                                    );


    if(fInternal)
    {
        status = WdfIoTargetSendInternalIoctlSynchronously(
                                                    nextLowerDriver,
                                                    NULL,
                                                    IoctlCode,
                                                    inputMemoryDescriptorLoc,
                                                    outputMemoryDescriptorLoc,
                                                    &options,
                                                     NULL);
        
    }
    else
    {
        status = WdfIoTargetSendIoctlSynchronously(
                                                nextLowerDriver,
                                                NULL,
                                                IoctlCode,
                                                inputMemoryDescriptorLoc,
                                                outputMemoryDescriptorLoc,
                                                &options,
                                                NULL);
    }
    return status;
}


NTSTATUS INTERNAL
SerialAsyncReadData(
    __in PDEVICE_EXTENSION      DevContext,
    __in ULONG                  BuffLen,
    __in PFN_WDF_REQUEST_COMPLETION_ROUTINE CompletionRoutine,
    __in_opt PVOID                  Context
    )
/*++
Routine Description:
  This function sends an asynchornous read request to the serial driver 

Argument:
  DevContext - Device Context

  BuffLen - Size of read request buffer

  CompletionRoutine - Completion routine to set on the request

  Context - Context to send to completion routine.

Return Value:
  On success, the function return STATUS_SUCCESS
  On failure, it returns the error code received from the serial driver
*/
{
    NTSTATUS                    status = STATUS_SUCCESS;
    WDF_OBJECT_ATTRIBUTES       RequestAttributes = {0};
    WDFREQUEST                  SerialRequest = {0};
    WDFMEMORY                   MemBuffer = NULL;
    PUCHAR                      ReadBuffer = NULL;
    BOOLEAN                     SentRequest = FALSE;
        
    
    UNREFERENCED_PARAMETER(Context);

    status = WdfRequestCreate(WDF_NO_OBJECT_ATTRIBUTES,
                               DevContext->IoTarget,
                               &SerialRequest);

    if(!NT_SUCCESS(status)){
        
        TErr(("WdfRequestCreate failed.status=0x%x!", status));
        return status;
    }

    WDF_OBJECT_ATTRIBUTES_INIT(&RequestAttributes);
    RequestAttributes.ParentObject = SerialRequest;

    status = WdfMemoryCreate(&RequestAttributes,
                    NonPagedPool,
                    HPEN_POOL_TAG,
                    (size_t)BuffLen,
                    &MemBuffer,
                    &ReadBuffer);
    
    if (NT_SUCCESS(status)) {
        
        RtlZeroMemory(ReadBuffer, BuffLen);
        status = WdfIoTargetFormatRequestForRead(
                                         DevContext->IoTarget,
                                         SerialRequest,
                                         MemBuffer,
                                         NULL,
                                         NULL
                                         );        
        if (NT_SUCCESS(status)) {
            
            WdfRequestSetCompletionRoutine(
                                       SerialRequest,
                                       CompletionRoutine,
                                       DevContext
                                       );

            SentRequest = WdfRequestSend(
                               SerialRequest,
                               DevContext->IoTarget,
                               WDF_NO_SEND_OPTIONS
                               );
            if (FALSE == SentRequest) {

                status = WdfRequestGetStatus(SerialRequest);
                TErr(("WdfRequestSend failed.status=0x%x!", status));
            }            
        }
        else{
            
            TErr(("WdfIoTargetFormatRequestForRead failed.status=0x%x!", status));
        }        
    }
    else{        
        
        TErr(("WdfMemoryCreate failed.status=0x%x!", status)); 
    }
    
    //if we didn't send the reqeust it must be deleted. The memory doesn't need to be deleted because its parent is the 
    //the request
    if(FALSE == SentRequest)
    {
        WdfObjectDelete(SerialRequest);
    }
    
    TExit(Func, ("=%x", status));
    return status;              
}

NTSTATUS INTERNAL
SerialAsyncReadWritePort(
    __in BOOLEAN           fRead,
    __in PDEVICE_EXTENSION DevContext,
    __in PUCHAR            Buffer,
    __in ULONG             BuffLen
    )
/*++
Routine Description:
  This function is used to asynchronously configure the serial port

Arguments:
   fRead - flag for configuration direction. TRUE for read.

   DevContext - Device Context

   Buffer - Configuration buffer

   BufferLen - Configuration buffer lenth
Return Value:
   On success, the function return STATUS_SUCCESS
   On failure, it returns the error code received from the serial driver
*/
{
   
    
    NTSTATUS                   status = STATUS_SUCCESS;
    WDF_OBJECT_ATTRIBUTES      RequestAttributes = {0};
    WDFREQUEST                 SerialRequest = {0};
    WDFMEMORY                  MemBuffer = NULL;
    BOOLEAN                    SentRequest = FALSE;
   
    WDF_OBJECT_ATTRIBUTES_INIT(&RequestAttributes);
    
    status = WdfRequestCreate(&RequestAttributes,
                               DevContext->IoTarget,
                               &SerialRequest);

    if(!NT_SUCCESS(status)){
        
        TErr(("WdfRequestCreate failed status=0x%x", status));
        return status;
    }

    if (Buffer != NULL) 
    {
        WDF_OBJECT_ATTRIBUTES  attributes;

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        
        status = WdfMemoryCreatePreallocated(&attributes,
                                   Buffer,
                                   (size_t)BuffLen,
                                   &MemBuffer);

        if (NT_SUCCESS(status)) {
            
            if(TRUE == fRead)
            {
                status = WdfIoTargetFormatRequestForRead(
                                                 DevContext->IoTarget,
                                                 SerialRequest,
                                                 MemBuffer,
                                                 NULL,
                                                 NULL
                                                 );
            }
            else
            {
                status = WdfIoTargetFormatRequestForWrite(
                                                 DevContext->IoTarget,
                                                 SerialRequest,
                                                 MemBuffer,
                                                 NULL,
                                                 NULL
                                                 );
            }
            if (NT_SUCCESS(status)) {
                
                WdfRequestSetCompletionRoutine(
                                           SerialRequest,
                                           AsyncReadWriteCompletion,
                                           DevContext
                                           );
                
                SentRequest = WdfRequestSend(
                                   SerialRequest,
                                   DevContext->IoTarget,
                                   WDF_NO_SEND_OPTIONS
                                   );

                if (FALSE == SentRequest) {
                     
                    status = WdfRequestGetStatus(SerialRequest);
                    TErr(("WdfRequestSend failed status=0x%x", status)); 
                }
            }
            else {
                 
                TErr(("WdfIoTargetFormatRequestForRead/Write failed status=0x%x", status));
            }
        }
        else{
             
           TErr(("WdfMemoryCreatePreallocated failed status=0x%x", status));
        }
    } 

    //if we didn't send the reqeust it must be deleted. The memory for this is pre-allocated 
    if(FALSE == SentRequest)
    {
        WdfObjectDelete(SerialRequest);
    }
    TExit(Func, ("=%x", status));
    return status;
}


VOID
AsyncReadWriteCompletion(
    IN WDFREQUEST    Request,
    IN WDFIOTARGET   Target,
    IN PWDF_REQUEST_COMPLETION_PARAMS Params,
    IN WDFCONTEXT    Context
    )
/*++
 Routine Description:
   Completion routine for handling Asynchronous I/O.
 
 Argument:

   Request: I/O Request

   Target: IOTarget for request

   Params: Completion parameters

   Context: Context set for completion routine

Return Value:
  On success, the function return STATUS_SUCCESS
  On failure, it returns the error code received from the serial driver
*/
{
   NTSTATUS status =  WdfRequestGetStatus(Request);

   UNREFERENCED_PARAMETER(Context);
   UNREFERENCED_PARAMETER(Params);
   UNREFERENCED_PARAMETER(Target);
   
   if (status == STATUS_CANCELLED)
    {
        KdPrint(("AsyncReadWrite IRP was cancelled."));
    }
    else if (!NT_SUCCESS(status))
    {
        KdPrint(("AsyncReadWrite irp failed (status=%x).", status));
    }

    WdfObjectDelete( Request);
    TExit(Func, ("=%x", status));
    return;
}

NTSTATUS INTERNAL
SerialSyncReadWritePort(
    __in BOOLEAN           fRead,
    __in PDEVICE_EXTENSION DevContext,
    __in_bcount(BuffLen) PUCHAR  Buffer,
    __in ULONG             BuffLen,
    __in_opt PWDF_REQUEST_SEND_OPTIONS   RequestOptions,
    __out_opt PULONG_PTR           BytesAccessed
    )
/*++
Routine Description:
  This function is used to synchronously configure the serial port

Arguments:
   fRead - flag for configuration direction. TRUE for read.

   DevContext - Device Context

   Buffer - Configuration buffer

   BufferLen - Configuration buffer lenth

   RequestOptions - this will include the time limit on the request and that the request should be synchronous.
                   if this parameter is NULL the function sets a default timeout of 1 second.

   BytesAccessed - Bytes read or written
Return Value:
  On success, the function return STATUS_SUCCESS
  On failure, it returns the error code received from the serial driver
*/
{
    
    NTSTATUS                   status = STATUS_INVALID_PARAMETER;    
    WDF_MEMORY_DESCRIPTOR      MemoryDescriptor = {0};
    WDF_REQUEST_SEND_OPTIONS   options = {0};
     
   
    if(NULL == RequestOptions)
    {
        //if the caller does not set the request options then
        // we set a default.
        WDF_REQUEST_SEND_OPTIONS_INIT(
                                &options,
                                WDF_REQUEST_SEND_OPTION_SYNCHRONOUS 
                                );
        WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(
                             &options,
                             WDF_REL_TIMEOUT_IN_SEC(1)
                             );
        
        RequestOptions = &options;
    }
    if (Buffer != NULL) 
   {
        WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&MemoryDescriptor,
                                  Buffer,
                                  BuffLen);

        if(fRead)
        {
            status = WdfIoTargetSendReadSynchronously(
                                    DevContext->IoTarget,
                                    NULL,
                                    &MemoryDescriptor,
                                    NULL,
                                    RequestOptions,
                                    BytesAccessed);


        }
        else{

            status = WdfIoTargetSendWriteSynchronously(
                                    DevContext->IoTarget,
                                    NULL,
                                    &MemoryDescriptor,
                                    NULL,
                                    RequestOptions,
                                    BytesAccessed);
        }
    }
    return status;
}

NTSTATUS INTERNAL
SerialClosePort(
    __in PDEVICE_EXTENSION DevContext
    )
    /*
    Routine Description:
      This function closes the connection to the serial port. The serial port is opened directly because
      HIDCLASS does not allow direcly opening and closing 

    Argument:

      DevContext - Device context

   Return Values:
      Returns the result of the attempt to close the serial port connection
    */
{
   //send clean up
    NTSTATUS status = STATUS_SUCCESS;

    status = SerialSendInitIRP(DevContext, IRP_MJ_CLEANUP);
    if(!NT_SUCCESS(status))
    {
        KdPrint(("failed to clean up serial port\n"));
    }
    else
    {
        status = SerialSendInitIRP(DevContext, IRP_MJ_CLOSE);
        if(!NT_SUCCESS(status))
        {
            KdPrint(("failed to clean up serial port\n"));
        }
    }
   TExit(Func, ("=%x", status));
   return status;
}


NTSTATUS INTERNAL
NInputInitSerialPort(
    __in PDEVICE_EXTENSION DevContext
    )
/*++

Routine Description:
  This routine prepares the serial port for communication with the host

Arguments:
  DevContext - Device context

Return Value:
  On success, the function return STATUS_SUCCESS
  On failure, it returns the error code received from the serial driver
*/
{
    NTSTATUS        status = STATUS_SUCCESS;
    SERIAL_BAUD_RATE sbr = {0};

    KdPrint(("Initiatlizing COM PORT\n"));
    sbr.BaudRate = OEM_SERIAL_BAUDRATE;
    status = SerialSyncSendIoctl(IOCTL_SERIAL_SET_BAUD_RATE,
                                 DevContext->IoTarget,
                                 (PVOID)&sbr,
                                 sizeof(sbr),
                                 NULL,
                                 0,
                                 FALSE);
    if (!NT_SUCCESS(status))
    {
        LogError(ERRLOG_INIT_SERIAL_FAILED,
                 status,
                 UNIQUE_ERRID(0x60),
                 NULL,
                 NULL);
        TErr(("Failed to set COM baudrate (status=%x).", status));
    }
    else
    {
        SERIAL_LINE_CONTROL slc;

        slc.WordLength = OEM_SERIAL_WORDLEN;
        slc.Parity = OEM_SERIAL_PARITY;
        slc.StopBits = OEM_SERIAL_STOPBITS;
        status = SerialSyncSendIoctl(IOCTL_SERIAL_SET_LINE_CONTROL,
                                     DevContext->IoTarget,
                                     (PVOID)&slc,
                                     sizeof(slc),
                                     NULL,
                                     0,
                                     FALSE);
        if (!NT_SUCCESS(status))
        {
            LogError(ERRLOG_INIT_SERIAL_FAILED,
                     status,
                     UNIQUE_ERRID(0x70),
                     NULL,
                     NULL);
            TErr(("Failed to set COM LineControl (status=%x).", status));
        }
        else
        {
            SERIAL_HANDFLOW HandFlow;

            status = SerialSyncSendIoctl(IOCTL_SERIAL_GET_HANDFLOW,
                                         DevContext->IoTarget,
                                         NULL,
                                         0,
                                         (PVOID)&HandFlow,
                                         sizeof(HandFlow),
                                         FALSE);
            if (!NT_SUCCESS(status))
            {
                LogError(ERRLOG_INIT_SERIAL_FAILED,
                         status,
                         UNIQUE_ERRID(0x80),
                         NULL,
                         NULL);
                TErr(("Failed to get HandFlow (status=%x).", status));
            }
            else
            {
                HandFlow.ControlHandShake &= ~SERIAL_DTR_MASK;
                HandFlow.ControlHandShake |= SERIAL_DTR_CONTROL;
                HandFlow.FlowReplace &= ~SERIAL_RTS_MASK;
                HandFlow.FlowReplace |= SERIAL_RTS_CONTROL;
                status = SerialSyncSendIoctl(IOCTL_SERIAL_SET_HANDFLOW,
                                             DevContext->IoTarget,
                                             (PVOID)&HandFlow,
                                             sizeof(HandFlow),
                                             NULL,
                                             0,
                                             FALSE);
                if (!NT_SUCCESS(status))
                {
                    LogError(ERRLOG_INIT_SERIAL_FAILED,
                             status,
                             UNIQUE_ERRID(0x90),
                             NULL,
                             NULL);
                    TErr(("Failed to set HandFlow (status=%x).", status));
                }
                else
                {
                    //
                    // Enable FIFO receive trigger at 14 bytes
                    //
                    ULONG Data = SERIAL_IOC_FCR_FIFO_ENABLE |
                                 SERIAL_IOC_FCR_RCVR_RESET |
                                 SERIAL_IOC_FCR_XMIT_RESET |
                                 SERIAL_IOC_FCR_RCVR_TRIGGER_08_BYTES;
                    status = SerialSyncSendIoctl(IOCTL_SERIAL_SET_FIFO_CONTROL,
                                                 DevContext->IoTarget,
                                                 (PVOID)&Data,
                                                 sizeof(Data),
                                                 NULL,
                                                 0,
                                                 FALSE);
                    if (!NT_SUCCESS(status))
                    {
                        LogError(ERRLOG_INIT_SERIAL_FAILED,
                                 status,
                                 UNIQUE_ERRID(0xa0),
                                 NULL,
                                 NULL);
                        TErr(("Failed to set COM FIFO control (status=%x).",
                              status));
                    }
                    else
                    {
                        Data = SERIAL_PURGE_RXCLEAR;
                        status = SerialSyncSendIoctl(IOCTL_SERIAL_PURGE,
                                                     DevContext->IoTarget,
                                                     (PVOID)&Data,
                                                     sizeof(Data),
                                                     NULL,
                                                     0,
                                                     FALSE);
                        if (!NT_SUCCESS(status))
                        {
                            LogError(ERRLOG_INIT_SERIAL_FAILED,
                                     status,
                                     UNIQUE_ERRID(0xb0),
                                     NULL,
                                     NULL);
                            TErr(("Failed to flush COM receive buffer (status=%x).",
                                  status));
                        }
                    }
                }
            }
        }
    }

    TExit(Func, ("=%x", status));
    return status;
}       




NTSTATUS INTERNAL
SerialSendInitIRP(
    __in PDEVICE_EXTENSION DevContext,
    __in UCHAR           MajorFunction
    )
/*++
Routine Description:
   This function is used for sending CREATE and CLOSE IRPs to the serial driver since HIDLCASS prevents this IRPs from getting to the function driver.

Argument:
   DevContext - Device context

Return Value:
   On success, the function returns STATUS_SUCCESS
   On failure it passes the relevant error code to the caller.
*/
{
    
    
    NTSTATUS                   status = STATUS_SUCCESS;
    WDF_REQUEST_SEND_OPTIONS   RequestOptions = {0};
    WDF_OBJECT_ATTRIBUTES      RequestAttributes = {0};
    WDFREQUEST                 SerialRequest = {0};
    WDFMEMORY                  MemBuffer = NULL;
    PIRP                       pIrp = NULL;
    PIO_STACK_LOCATION         pNextStack = NULL;
    
    WDF_OBJECT_ATTRIBUTES_INIT(&RequestAttributes);
    
    status = WdfRequestCreate(&RequestAttributes,
                               DevContext->IoTarget,
                               &SerialRequest);

    if(!NT_SUCCESS(status))
    {
        return status;
    }

    RequestAttributes.ParentObject = SerialRequest;

    status = WdfMemoryCreate(&RequestAttributes,
                                   NonPagedPool,
                                   HPEN_POOL_TAG,
                                   1,
                                   &MemBuffer,
                                   NULL);
     if (!NT_SUCCESS(status)) {
        TErr(("WdfMemoryCreate failed status=0x%x", status));
    }
    else {
        status = WdfIoTargetFormatRequestForRead(
                                         DevContext->IoTarget,
                                         SerialRequest,
                                         MemBuffer,
                                         NULL,
                                         NULL
                                         );
        
        if (!NT_SUCCESS(status)) {
            
            TErr(("WdfIoTargetFormatRequestForRead failed status=0x%x", status));    
            
        }
        else
        {
            pIrp = WdfRequestWdmGetIrp(SerialRequest);
            pNextStack = IoGetNextIrpStackLocation(pIrp);
            pNextStack->MajorFunction = MajorFunction; 
            
            WDF_REQUEST_SEND_OPTIONS_INIT(
                                        &RequestOptions,
                                        WDF_REQUEST_SEND_OPTION_SYNCHRONOUS 
                                        );
            
            WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(
                                     &RequestOptions,
                                     WDF_REL_TIMEOUT_IN_SEC(SERIAL_TIME_OUT)
                                     );


            KdPrint(("about to send %d request\n", MajorFunction));
            if (WdfRequestSend(
                               SerialRequest,
                               DevContext->IoTarget,
                               &RequestOptions
                               ) == FALSE) {
                status = WdfRequestGetStatus(SerialRequest);

            }
        }
    }
    WdfObjectDelete( SerialRequest);
    TExit(Func, ("=%x", status));
    return status;    
}

NTSTATUS
SerialRequestMoreData(
         __in PDEVICE_EXTENSION   DevContext          
         )
{
    PWORKITEM_CONTEXT       context = NULL;
    WDF_OBJECT_ATTRIBUTES   attributes = {0};
    WDF_WORKITEM_CONFIG     workitemConfig = {0};
    WDFWORKITEM             hWorkItem = {0};
    NTSTATUS                status = STATUS_SUCCESS;

    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&attributes, WORKITEM_CONTEXT);
    attributes.ParentObject = DevContext->Device;

    WDF_WORKITEM_CONFIG_INIT(&workitemConfig, ReadMoreBytes);

    status = WdfWorkItemCreate( &workitemConfig,
                                &attributes,
                                &hWorkItem);

    if(NT_SUCCESS(status)) 
    {
       context = GetWorkItemContext(hWorkItem);
       context->devContext = DevContext;
       WdfWorkItemEnqueue(hWorkItem);
    }
    TExit(Func, ("=%x", status));
    return status;
}


