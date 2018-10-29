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
   Olumuyiwa Durojaiye Sept-20-2007

Environment:

    kernel mode only

Revision History:

--*/

#include "pch.h"

#define MODULE_ID                       4



#ifdef ALLOC_PRAGMA
  #pragma alloc_text(PAGE, NInputRegQueryValue)
  #pragma alloc_text(PAGE, NInputRegSetValue)
#endif

VOID
NInputInternalDeviceControl(
    __in WDFQUEUE     Queue,
    __in WDFREQUEST   Request,
    __in size_t       OutputBufferLength,
    __in size_t       InputBufferLength,
    __in ULONG        IoControlCode
    )
/*++

Routine Description:

    This event is called when the framework receives 
    IRP_MJ_INTERNAL DEVICE_CONTROL requests from the system.

Arguments:

    Queue - Handle to the framework queue object that is associated
            with the I/O request.
    Request - Handle to a framework request object.

    OutputBufferLength - length of the request's output buffer,
                        if an output buffer is available.
    InputBufferLength - length of the request's input buffer,
                        if an input buffer is available.

    IoControlCode - the driver-defined or system-defined I/O control code
                    (IOCTL) that is associated with the request.
Return Value:

    VOID

--*/

{
    NTSTATUS            status = STATUS_SUCCESS;
    WDFDEVICE           device = NULL;
    PDEVICE_EXTENSION   devContext = NULL;
    BOOLEAN             RequestPending = FALSE;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    device = WdfIoQueueGetDevice(Queue);
    devContext = GetDeviceContext(device);

    TEnter(Func, ("(IoControlCode=%d)", IoControlCode));

    //
    // Please note that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl. So depending on the ioctl code, we will either
    // use retreive function or escape to WDM to get the UserBuffer.
    //

    switch(IoControlCode) {

    case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
        //
        // Retrieves the device's HID descriptor.
        //
        //init serial port if it hasn't already been initialized.
        status = NInputStartDevice(devContext);
        if(NT_SUCCESS(status))
        {            
          status = NInputGetHidDescriptor(device, Request);
        }
        break;

    case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
        //
        //Retrieves a device's attributes in a HID_DEVICE_ATTRIBUTES structure.
        //
        status = NInputGetDeviceAttributes(Request);
        break;

    case IOCTL_HID_GET_REPORT_DESCRIPTOR:
        //
        //Obtains the report descriptor for the HID device.
        //
        status = NInputGetReportDescriptor(device, Request);
        break;

    case IOCTL_HID_GET_STRING:
          status = NInputGetString(device, Request);
          break;

    case IOCTL_HID_READ_REPORT:

        status = NInputReadReport(devContext, Request, &RequestPending);

        break;
    
    case IOCTL_HID_SET_FEATURE:
        //
        // This sends a HID class feature report to a top-level collection of
        // a HID class device.
        //
        KdPrint(("Setting feature report\n"));
        status = NInputSetFeatureReport(device, Request);
        break;
    
    case IOCTL_HID_GET_FEATURE:
        //
        // returns a feature report associated with a top-level collection
        //
        status = NInputGetFeatureReport(device, Request);
        break;
    
    case IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST:    
    
    case IOCTL_HID_WRITE_REPORT:
        //
        //Transmits a class driver-supplied report to the device.
        //    
    case IOCTL_HID_ACTIVATE_DEVICE:
        //
        // Makes the device ready for I/O operations.
        //
    case IOCTL_HID_DEACTIVATE_DEVICE:
        //
        // Causes the device to cease operations and terminate all outstanding
        // I/O requests.
        //
    default:
        status = STATUS_NOT_SUPPORTED;
        break;
    }

    if(!RequestPending)
    {
        WdfRequestComplete(Request, status);
    }
    TExit(Func, ("=%x", status));
    return;
}



NTSTATUS INTERNAL
NInputGetString(
    __in WDFDEVICE Device,
    __in WDFREQUEST Request
    )
    /*++

Routine Description:

    Returns string requested by the HIDCLASS driver

Arguments:

    Device - Handle to WDF Device Object

    Request - Handle to request object

Return Value:

    NT status code.
    success - STATUS_SUCCESS
    failure:
     STATUS_BUFFER_TOO_SMALL - data buffer size is inadequate
     STATUS_INVALID_PARAMETER  - invalid parameter

--*/
{
    NTSTATUS            status = STATUS_SUCCESS;
    PIRP                Irp = NULL;
    PIO_STACK_LOCATION  irpsp = NULL;
    PWSTR               pwstrID = NULL;
    ULONG_PTR               lenID = 0;

    
    UNREFERENCED_PARAMETER(Device);
    
    Irp = WdfRequestWdmGetIrp(Request);
    irpsp = IoGetCurrentIrpStackLocation(Irp);
    switch ((ULONG_PTR)irpsp->Parameters.DeviceIoControl.Type3InputBuffer &
            0xffff)
    {
        case HID_STRING_ID_IMANUFACTURER:
            pwstrID = gpwstrManufacturerID;
            break;

        case HID_STRING_ID_IPRODUCT:
            pwstrID = gpwstrProductID;
            break;

        case HID_STRING_ID_ISERIALNUMBER:
            pwstrID = gpwstrSerialNumber;
            break;

        default:
            pwstrID = NULL;
            break;
    }

    lenID = pwstrID? wcslen(pwstrID)*sizeof(WCHAR) + sizeof(UNICODE_NULL): 0;
    if (pwstrID == NULL)
    {
        status = STATUS_INVALID_PARAMETER;
        LogError(ERRLOG_INVALID_PARAMETER,
                 status,
                 UNIQUE_ERRID(0x70),
                 NULL,
                 NULL);
        TWarn(("Invalid string ID (ID=%p).",
               irpsp->Parameters.DeviceIoControl.Type3InputBuffer));
    }
    else if (irpsp->Parameters.DeviceIoControl.OutputBufferLength < lenID)
    {
        status = STATUS_BUFFER_TOO_SMALL;
        LogError(ERRLOG_BUFFER_TOO_SMALL,
                 status,
                 UNIQUE_ERRID(0x80),
                 NULL,
                 NULL);
        TWarn(("Output buffer too small (len=%d,need=%d).",
               irpsp->Parameters.DeviceIoControl.OutputBufferLength,
               (LONG)lenID));
    }
    else
    {
        RtlCopyMemory(Irp->UserBuffer, pwstrID, lenID);
        Irp->IoStatus.Information = lenID;
    }

    TExit(Func, ("=%x (string=%S)", status, pwstrID? pwstrID: L"Null"));    
    return status;

}
NTSTATUS
NInputGetHidDescriptor(
    __in WDFDEVICE Device,
    __in WDFREQUEST Request
    )
/*++

Routine Description:

    Finds the HID descriptor and copies it into the buffer provided by the 
    Request.

Arguments:

    Device - Handle to WDF Device Object

    Request - Handle to request object

Return Value:

    NT status code.
	 success - STATUS_SUCCESS 
	 failure:
     STATUS_INVALID_PARAMETER - An invalid parameter was detected.

--*/
{
    NTSTATUS            status = STATUS_SUCCESS;
    size_t              bytesToCopy = 0;
    WDFMEMORY           memory = {0};

    UNREFERENCED_PARAMETER(Device);

    //
    // This IOCTL is METHOD_NEITHER so WdfRequestRetrieveOutputMemory
    // will correctly retrieve buffer from Irp->UserBuffer. 
    // Remember that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl.
    //
    status = WdfRequestRetrieveOutputMemory(Request, &memory);
    if (!NT_SUCCESS(status)) {
        TErr(("failed to retrieve output buffer status=0x%x", status));
        return status;
    }

    //
    // Use hardcoded "HID Descriptor" 
    //
    bytesToCopy = sizeof(gHidDescriptor);
    status = WdfMemoryCopyFromBuffer(memory,
                            0, // Offset
                            (PUCHAR) &gHidDescriptor,
                            bytesToCopy);
    if (!NT_SUCCESS(status)) {
        TErr(("failed to copy descriptor to output buffer. status=0x%x", status));
        return status;
    }

    //
    // Report how many bytes were copied
    //
    WdfRequestSetInformation(Request, bytesToCopy);

    TExit(Func, ("=%x", status));
    return status;
}

NTSTATUS
NInputGetReportDescriptor(
    __in WDFDEVICE Device,
    __in WDFREQUEST Request
    )
/*++

Routine Description:

    Finds the Report descriptor and copies it into the buffer provided by the
    Request.

Arguments:

    Device - Handle to WDF Device Object

    Request - Handle to request object

Return Value:

    NT status code.
	 success - STATUS_SUCCESS 
	 failure:
     STATUS_INVALID_PARAMETER - An invalid parameter was detected.

--*/
{
    NTSTATUS            status = STATUS_SUCCESS;
    ULONG_PTR           bytesToCopy = 0;
    WDFMEMORY           memory = {0};

    UNREFERENCED_PARAMETER(Device);

     //
    // This IOCTL is METHOD_NEITHER so WdfRequestRetrieveOutputMemory
    // will correctly retrieve buffer from Irp->UserBuffer. 
    // Remember that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl.
    //
    status = WdfRequestRetrieveOutputMemory(Request, &memory);
    if (!NT_SUCCESS(status)) {
        TErr(("WdfRequestRetrieveOutputMemory failed status=0x%x", status));
        return status;
    }

    //
    // Use hardcoded Report descriptor
    //
    bytesToCopy = gdwcbReportDescriptor;
    status = WdfMemoryCopyFromBuffer(memory,
                            0,
                            (PUCHAR) gReportDescriptor,
                            bytesToCopy);
    if (!NT_SUCCESS(status)) {
        TErr(("WdfMemoryCopyFromBuffer failed status=0x%x", status));
        return status;
    }

    //
    // Report how many bytes were copied
    //
    WdfRequestSetInformation(Request, bytesToCopy);

    TExit(Func, ("=%x", status));
    return status;
}


NTSTATUS
NInputGetDeviceAttributes(
    __in WDFREQUEST Request
    )
/*++

Routine Description:

    Fill in the given struct _HID_DEVICE_ATTRIBUTES

Arguments:

    Request - Pointer to Request object.

Return Value:

    NT status code.
	success - STATUS_SUCCESS 
	 failure:
     STATUS_INVALID_PARAMETER - An invalid parameter was detected.

--*/
{
    NTSTATUS                 status = STATUS_SUCCESS;
    PHID_DEVICE_ATTRIBUTES   deviceAttributes = NULL;
    PDEVICE_EXTENSION        deviceInfo = NULL;

    deviceInfo = GetDeviceContext(WdfIoQueueGetDevice(WdfRequestGetIoQueue(Request)));

    //
    // This IOCTL is METHOD_NEITHER so WdfRequestRetrieveOutputMemory
    // will correctly retrieve buffer from Irp->UserBuffer. 
    // Remember that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl.
    //
    status = WdfRequestRetrieveOutputBuffer(Request,
                                            sizeof (HID_DEVICE_ATTRIBUTES),
                                            &deviceAttributes,
                                            NULL);
    if (!NT_SUCCESS(status)) {
        TErr(("WdfRequestRetrieveOutputBuffer failed 0x%x\n", status));
        return status;
    }

    deviceAttributes->Size = sizeof (HID_DEVICE_ATTRIBUTES);
    deviceAttributes->VendorID = OEM_VENDOR_ID;
    deviceAttributes->ProductID = deviceInfo->OemData.wProductID;
    deviceAttributes->VersionNumber = deviceInfo->OemData.wFirmwareVer;

    //
    // Report how many bytes were copied
    //
    WdfRequestSetInformation(Request, sizeof (HID_DEVICE_ATTRIBUTES));

    TExit(Func, ("=%x", status));
    return status;
}



NTSTATUS INTERNAL
NInputRegQueryValue(
    __in  WDFKEY  hkey,
    __in  LPCWSTR pszValueName,
    __in  ULONG   dwType,
    __out_ecount(dwcbData) PUCHAR  lpbData,
    __in  ULONG   dwcbData,
    __out_opt PULONG  pdwcbLen
    )
/*++
Routine Description:
  registry value reading function

Arguments:
  hkey - Registry handle.

  pszValueName - Points to the value name string.

  dwType - Registry data type.

 lpbData - To hold the value data.

  dwcbData - Specifies the buffer size.

  pdwcbLen - To hold the require data length (optional).

Return Value:   
 SUCCESS | Returns STATUS_SUCCESS.
 FAILURE | Returns NT status code.
--*/
{
    NTSTATUS        status = STATUS_SUCCESS;
    UNICODE_STRING  ucstrName = {0};
    ULONG           Type = 0;
    ULONG           dwLen = 0;

    PAGED_CODE ();
    TEnter(Func, ("(hkey=%p,ValueName=%S,Type=%x,lpbData=%p,Len=%d,pdwLen=%p)",
                  hkey, pszValueName, dwType, lpbData, dwcbData,
                  pdwcbLen));

    TAssert(lpbData != NULL);
    RtlInitUnicodeString(&ucstrName, pszValueName);
    
    status = WdfRegistryQueryValue(hkey,
                             &ucstrName,
                             dwcbData,
                             lpbData,
                             &dwLen,
                             &Type);
    if (NT_SUCCESS(status))
    {
        if ((dwType != REG_NONE) && (dwType != Type))
        {
            status = STATUS_OBJECT_TYPE_MISMATCH;
            LogError(ERRLOG_REGTYPE_MISMATCH,
                     status,
                     UNIQUE_ERRID(0x30),
                     NULL,
                     NULL);
            TErr(("Registry data type mismatch (Type=%x,Expected=%x).",
                  Type, dwType));
        }
		if ((pdwcbLen != NULL) &&
        ((status == STATUS_SUCCESS) ||
         (status == STATUS_BUFFER_TOO_SMALL)))
		{
			*pdwcbLen = dwLen;
		}         
    }
    TExit(Func, ("=%x", status));
    return status;
}       

NTSTATUS INTERNAL
NInputRegSetValue(
    __in WDFKEY  hkey,
    __in LPCWSTR pszValueName,
    __in ULONG   dwType,
    __in PUCHAR  lpbData,
    __in ULONG   dwcbData
    )
/*++
Routine Description:

   Write registry data.

Arguments:
   hkey - Registry handle.

   pszValueName - Points to the value name string.

   dwType - Registry data type.

   lpbData - To hold the value data.

   dwcbData - Specifies the buffer size.      
   
Return Value:
   SUCCESS - Returns STATUS_SUCCESS.
   FAILURE - Returns NT status code.
--*/
{
    NTSTATUS        status = STATUS_SUCCESS;
    UNICODE_STRING  ucstrName = {0};


    PAGED_CODE ();
    TEnter(Func, ("(hkey=%p,ValueName=%S,Type=%x,lpbData=%p,Len=%d)",
                  hkey, pszValueName, dwType, lpbData, dwcbData));

    TAssert(lpbData != NULL);
    RtlInitUnicodeString(&ucstrName, pszValueName);
    status = WdfRegistryAssignValue(hkey,
                           &ucstrName,
                           dwType,
                           dwcbData,
                           lpbData);
    if (!NT_SUCCESS(status))
    {
        LogError(ERRLOG_WRITEREG_FAILED,
                 status,
                 UNIQUE_ERRID(0x50),
                 NULL,
                 NULL);
        TErr(("Failed to write registry value %S (status=%x).",
              pszValueName, status));
    }

    TExit(Func, ("=%x", status));
    return status;
} 





