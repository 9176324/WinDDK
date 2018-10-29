/*++
    Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

    Module Name:

        Utils.c

    Abstract:
       support functions

    Environment:
        Kernel mode
--*/

#include "VSerial.h"

#ifdef ALLOC_PRAGMA
    #pragma alloc_text( PAGE, RegQueryValue)
    #pragma alloc_text( PAGE, RegSetValue)
#endif

NTSTATUS
QueryDesiredControlResponse(
    __in ULONG_PTR controlCode,
    __in PSERIAL_DEVICE_EXTENSION DevExt
    )
/*++

Routine Description:

    This routine routine reads data from the registry to determine how the
    IRP dispatch routine should respond. It can either simulate success or failure
    based on the value it finds in the registry.


Arguments:

    controlCode - Control code

    DevExt        - Device extension

Return Value:



--*/
{
    NTSTATUS    status = STATUS_SUCCESS;
    ULONG        dwData = 0;
    UNICODE_STRING valueName = {0};

    KdPrint(("Control code:%d\n", controlCode));

    RtlInitUnicodeString(&valueName, CONTROL_RESPONSE);

    status = WdfRegistryQueryULong (DevExt->SimDevKey,
                              &valueName,
                              &dwData);
    if (NT_SUCCESS(status) && dwData == controlCode)
    {
        KdPrint(("Failed request num:%d\n", controlCode));
        status = STATUS_INVALID_PARAMETER;
    }
    else
    {
        status = STATUS_SUCCESS;
        KdPrint(("failed to query key\n"));
    }

    KdPrint(("RegData:%d\n", dwData));

    return status;
}

NTSTATUS
QueryDesiredReadWriteResponse(
    __in UCHAR IrpMajorFunc,
    __in PSERIAL_DEVICE_EXTENSION DevExt
    )
/*++

Routine Description:

    This routine routine reads data from the registry to determine how the
    IRP dispatch routine should respond. It can either simulate success or failure
    based on the value it finds in the registry.


Arguments:
QueryDesiredReadWriteResponse
    IrpMajorFunc - Major function code

    DevExt        - Device extension

Return Value:
    STATUS_INVALID_PARAMETER
    STATUS_SUCCESS


--*/
{
    NTSTATUS                    status = STATUS_SUCCESS;
    ULONG                       dwData = 0;
    UNICODE_STRING              valueName = {0};
    ULONG                       IrpCount = DevExt->WriteCount;
    LARGE_INTEGER               Timeout = {0};
    

    if(IRP_MJ_READ == IrpMajorFunc)
    {
        KdPrint(("Read request\n"));
        RtlInitUnicodeString(&valueName, READ_RESPONSE);
        IrpCount = DevExt->ReadCount;
    } else {
        KdPrint(("Write request\n"));
        RtlInitUnicodeString(&valueName, WRITE_RESPONSE);
    }

    status = WdfRegistryQueryULong (DevExt->SimDevKey,
                              &valueName,
                              &dwData);
    if(NT_SUCCESS(status) && 
        ((dwData == IrpCount) || (0 == dwData))
        )
    {
        //add time delay for possible timeout testing.
        Timeout.QuadPart = IO_TIMEOUT;
        KeDelayExecutionThread(KernelMode, FALSE, (LARGE_INTEGER *)&Timeout);
        //data in the registry asked us to fail the "IrpCount"th IRP
        KdPrint(("Failed request num:%d\n", IrpCount));
        status = STATUS_INVALID_PARAMETER;			
    }
    else
    {
        status = STATUS_SUCCESS;
        KdPrint(("failed to query key\n"));
    }

    KdPrint(("RegData:%d\n", dwData));

    return status;
}

NTSTATUS
ReadDeviceBuffer(
    __out_bcount_full(readLength) CHAR* dataBuffer,
    __in size_t readLength,
    __in PSERIAL_DEVICE_EXTENSION    DevExt
    )
/*++

Routine Description:
    This routine is called to read data from buffer to complete a read Irp.

Arguments:

    DevExt     -- Device extension

Return Value:

    STATUS_SUCCESS
    STATUS_INVALID_PARAMETER
    STATUS_DEVICE_NOT_READY
--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    size_t    bufferPos = 0;
    size_t   fillCount = 0;

    if(NULL == dataBuffer || NULL == DevExt || 0 == readLength)
    {
        KdPrint(("Invalid parameter dataBuffer= %p,DevExt=%p%,readLength=%d\n",dataBuffer, DevExt, readLength));
        status = STATUS_INVALID_PARAMETER;
    }
    else{
    
        // copy data to the OUT buffer until we fullfil the request
        // or we run out of data
        for( ;
            (DevExt->CurrentPos <= DevExt->LastPos) &&
            (fillCount < readLength);
            DevExt->CurrentPos++,
            fillCount++
            )
        {
            dataBuffer[bufferPos++] = DevExt->DeviceData[DevExt->CurrentPos];

        }
        //if we still have not completed the request,
        // we should read more data for the file
        if(fillCount < readLength)
        {
            status = FillDeviceBuffer(DevExt);
            //if loaded buffer is enough to satisfy current read request,
            // continue with the read request.
            if( NT_SUCCESS(status) &&
                (DevExt->LastPos >= readLength-fillCount)
                )
            {
                for( ;
                    (DevExt->CurrentPos <= DevExt->LastPos) &&
                    (fillCount < readLength);
                    DevExt->CurrentPos++,
                    fillCount++
                    )
                {
                    dataBuffer[bufferPos++] = DevExt->DeviceData[DevExt->CurrentPos];

                }
            }
            else
            {
                KdPrint(("Not enough data was available in the file\n"));
            }
        }
    }
    return status;
}

NTSTATUS
FillDeviceBuffer(
    __in  PSERIAL_DEVICE_EXTENSION    DevExt
    )
/*++

Routine Description:
    This routine is called to fill a buffer with data from the file.
    This call help reduce the number of calls to ZwReadFile.

Arguments:

    DevExt     -- Device extension

Return Value:

    STATUS_SUCCESS
    STATUS_INVALID_PARAMETER
    STATUS_DEVICE_NOT_READY
--*/
{
    ULONG                bufferSize = 0;
    IO_STATUS_BLOCK        iostatus = {0};
    NTSTATUS            status = STATUS_SUCCESS;
    if(NULL == DevExt)
    {
        return STATUS_INVALID_PARAMETER;
    }
    bufferSize = sizeof(DevExt->DeviceData);
    status = ZwReadFile(DevExt->hDataFile, NULL, NULL, NULL, &iostatus,
                                    DevExt->DeviceData, bufferSize, NULL, NULL);
    if(NT_SUCCESS(status) && iostatus.Information != 0)
    {
        DevExt->LastPos = iostatus.Information-1;
        DevExt->CurrentPos = 0;
    }
    else
    {
        //file is out of data
        DevExt->LastPos = 0;
        DevExt->CurrentPos = 0;
        status = DeleteDeviceDataFile(DevExt);
        KdPrint(("ZwReadFile failed\n"));
    }
    return status;
}

NTSTATUS
OpenDeviceDataFile(
    __in PSERIAL_DEVICE_EXTENSION    DevExt
    )
/*++

Routine Description:
    This routine is called to open the data file for reading

Arguments:

    DevExt     -- Device extension

Return Value:

    STATUS_SUCCESS
    STATUS_DEVICE_NOT_READY
--*/
{
     NTSTATUS                       status = STATUS_SUCCESS;
    OBJECT_ATTRIBUTES               oa = {0};
    IO_STATUS_BLOCK                 iostatus = {0};
    UNICODE_STRING                  pathName = {0};
    WCHAR*                          fileName = DEFAULT_FILE;
    WDFSTRING                       String;
    BOOLEAN                         UsedCustomString = FALSE;

    DECLARE_CONST_UNICODE_STRING(valueName, CUSTOM_FILE_NAME);
    //open custom data file if it exists
    RtlInitUnicodeString(&pathName, fileName);
    if(NULL != DevExt)
    {
        status = WdfStringCreate(
                         NULL,
                         WDF_NO_OBJECT_ATTRIBUTES,
                         &String);
        if(NT_SUCCESS(status))
        {
            
            status = WdfRegistryQueryString(DevExt->SimDevKey, 
                &valueName, 
                String);
            
            if(NT_SUCCESS(status))
            {
                KdPrint(("Custom Data file found!\n"));
                UsedCustomString = TRUE;
                WdfStringGetUnicodeString(String,
                                  &pathName);  
            }            
        }        
        InitializeObjectAttributes(&oa, &pathName,
            OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 
            NULL, NULL);         
        
        status = ZwCreateFile(&DevExt->hDataFile, GENERIC_READ, &oa, &iostatus, NULL, 0,
                                FILE_SHARE_READ, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
        if(NT_SUCCESS(status))
        {
            //fill the device buffer with data.
            status = FillDeviceBuffer(DevExt);
        }
        if(TRUE == UsedCustomString)
        {
            WdfObjectDelete(String);
        }
    }
    return status;
}

NTSTATUS
DeleteDeviceDataFile(
    __in PSERIAL_DEVICE_EXTENSION    DevExt
    )
/*++

Routine Description:
    This routine is called to delete the data file when there is
    no data left in it

Arguments:

    DevExt     -- Device extension

Return Value:

    STATUS_SUCCESS
    STATUS_DEVICE_NOT_READY
--*/
{

    FILE_DISPOSITION_INFORMATION    FileDisposition = {0};
    IO_STATUS_BLOCK                 iostatus = {0};
    NTSTATUS                        status = STATUS_SUCCESS;
    KdPrint(("Setting file up for deletion\n"));
    FileDisposition.DeleteFile = TRUE;

    status = ZwSetInformationFile(DevExt->hDataFile, &iostatus, &FileDisposition,
        sizeof(FileDisposition), (FILE_INFORMATION_CLASS)FileDispositionInformation);
    if(!NT_SUCCESS(status))
    {
        KdPrint(("failed to set file up for deletion!\n"));
    }
    else
    {
        //simulate device not ready since we nolonger have a data file.
        status = STATUS_DEVICE_NOT_READY;
    }
   //close file handle no matter what.
   ZwClose(DevExt->hDataFile);
   DevExt->hDataFile = NULL;
   return status;
}

NTSTATUS
RegQueryValue(
    __in  WDFKEY  hkey,
    __in  LPCWSTR pszValueName,
    __in  ULONG   dwType,
    __out_bcount_opt(dwcbData) PVOID   lpbData,
    __in  ULONG   dwcbData,
    __out_opt PULONG  pdwcbLen
    )
/*++

Routine Description:

  Read value from registry key

Arguments:

   hkey			- Registry key handle
   
   pszValueName - Value name
   
   dwType		- Value data type

   lpbData		- Data buffer
   
   dwcbData		- size of data buffer

   pdwcbLen		- length of actual data read

Return Value:

      NT Status code

--*/
    
{
    NTSTATUS                        status = STATUS_SUCCESS;
    UNICODE_STRING                  ucstrName = {0};
    ULONG                           dwLen = 0;
    ULONG                           valueType = 0;
    PAGED_CODE ();

    RtlInitUnicodeString(&ucstrName, pszValueName);
    
    
    status = WdfRegistryQueryValue(hkey,
                             &ucstrName,
                             dwcbData,
                             lpbData,
                             &dwLen,
                             &valueType);
    if (NT_SUCCESS(status))
    {
        if ( (dwType != REG_NONE) && 
             (dwType != valueType)
            )
        {
            status = STATUS_OBJECT_TYPE_MISMATCH;
            KdPrint(("Registry data type mismatch (Type=%x,Expected=%x).",
                  valueType, dwType));
        }            
    }
    if ((pdwcbLen != NULL) &&
        ((status == STATUS_SUCCESS) ||
         (status == STATUS_BUFFER_TOO_SMALL))
        )
    {
        *pdwcbLen = dwLen;
    }
   
    return status;
}    

NTSTATUS
RegSetValue(
    __in WDFKEY  hkey,
    __in LPCWSTR pszValueName,
    __in ULONG   dwType,
    __in PUCHAR  lpbData,
    __in ULONG   dwcbData
    )
/*++

Routine Description:

  Writes data to registry

Arguments:

   hkey			- Registry key handle
   
   pszValueName - Value name
   
   dwType		- Value data type

   lpbData		- Data buffer
   
   dwcbData		- size of data buffer
   
Return Value:

      NT Status code

--*/
{
    NTSTATUS		status = STATUS_SUCCESS;
    UNICODE_STRING	ucstrName = {0};

    PAGED_CODE ();
    
    RtlInitUnicodeString(&ucstrName, pszValueName);
    status = WdfRegistryAssignValue(hkey,
                           &ucstrName,
                           dwType,
                           dwcbData,
                           lpbData);

    if (!NT_SUCCESS(status))
    {
       KdPrint(("Failed to write registry value %S (status=%x).",
              pszValueName, status));
    }

    return status;
} //RegSetValue

NTSTATUS
RegOpenDeviceKey(
    __in PSERIAL_DEVICE_EXTENSION devExt
    )
/*++

Routine Description:

  Opens device registry key

Arguments:

   devExt		- Device extension
  
Return Value:

      NT Status code

--*/
{
    NTSTATUS            status = STATUS_SUCCESS;
    UNICODE_STRING      ParametersKeyName = {0};

    RtlInitUnicodeString(&ParametersKeyName, SIMULATED_DEVICE_KEY_NAME);    
    
    status = WdfRegistryOpenKey(NULL, &ParametersKeyName, 
                    KEY_READ, WDF_NO_OBJECT_ATTRIBUTES,
                    &devExt->SimDevKey);	
    if (!NT_SUCCESS(status))
    {
        KdPrint(("Unable to open device key(status=%x).",
                status));
    }
    return status;
}


