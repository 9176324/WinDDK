/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    WMI.C

Abstract:

    This module handles WMI support for the featured WDF toaster driver.

Environment:

    Kernel mode

--*/

#include "toaster.h"
#include <wmilib.h>
#include <wmistr.h>
#include <ToasterMof.h>


static
NTSTATUS
GetDeviceFriendlyName(
    __in  WDFDEVICE Device,
    __out WDFMEMORY* DeviceName
    );

static
ULONG
ToasterHelperFunction1(
    __in ULONG InData
    );

static
ULONG
ToasterHelperFunction2(
    __in ULONG InData1,
    __in ULONG InData2
    );

static
VOID
ToasterHelperFunction3(
    __in  ULONG InData1,
    __in  ULONG InData2,
    __out PULONG OutData1,
    __out PULONG OutData2
    );


#define ToasterDeviceInformation_SIZE FIELD_OFFSET(ToasterDeviceInformation, VariableData)

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(ToasterDeviceInformation, ToasterWmiGetData)
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(ToasterControl, ToasterWmiGetControlData)


#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, ToasterWmiRegistration)
#pragma alloc_text(PAGE, EvtWmiInstanceStdDeviceDataQueryInstance)
#pragma alloc_text(PAGE, EvtWmiInstanceStdDeviceDataSetInstance)
#pragma alloc_text(PAGE, EvtWmiInstanceStdDeviceDataSetItem)
#pragma alloc_text(PAGE, EvtWmiInstanceToasterControlQueryInstance)
#pragma alloc_text(PAGE, EvtWmiInstanceToasterControlExecuteMethod)
#pragma alloc_text(PAGE, EvtWmiInstanceToasterControlSetInstance)
#pragma alloc_text(PAGE, ToasterHelperFunction1)
#pragma alloc_text(PAGE, ToasterHelperFunction2)
#pragma alloc_text(PAGE, ToasterHelperFunction3)
#endif

//
// Global debug error level
//

extern ULONG DebugLevel;

NTSTATUS
ToasterWmiRegistration(
    __in WDFDEVICE Device
    )

/*++

Routine Description

    Registers with WMI as a data provider for this instance of the device.

Arguments:

    Device - The Framework device object for which the WMI provider instances
        are to be created and registered. This device object will be the parent
        object of the new WMI instance objects.

Return Value:

    NT Status code.

--*/

{
    NTSTATUS status;
    PFDO_DATA fdoData;
    PToasterDeviceInformation pData;
    PToasterControl controlData;

    WDFWMIINSTANCE instance;
    WDF_OBJECT_ATTRIBUTES woa;
    WDF_WMI_PROVIDER_CONFIG providerConfig;
    WDF_WMI_INSTANCE_CONFIG instanceConfig;
    DECLARE_CONST_UNICODE_STRING(mofResourceName, MOFRESOURCENAME);

    PAGED_CODE();

    fdoData = ToasterFdoGetData(Device);

    //
    // Register the MOF resource names of any customized WMI data providers
    // that are not defined in wmicore.mof.
    //
    status = WdfDeviceAssignMofResourceName(Device, &mofResourceName);
    if (!NT_SUCCESS(status)) {

        KdPrint(("[Toaster] Status = 0x%08x, WdfDeviceAssignMofResourceName failed\n", status));
        return status;
    }

    //
    // Initialize the config structures for the Provider and the Instance and
    // define event callback functions that support a WMI client's request to
    // access the driver's WMI data blocks.
    //

    WDF_WMI_PROVIDER_CONFIG_INIT(&providerConfig, &ToasterDeviceInformation_GUID);

    //
    // Specify minimum expected buffer size for query and set instance requests.
    // Since the query block size is different than the set block size, set it
    // to zero and manually check for the buffer size for each operation.
    //
    providerConfig.MinInstanceBufferSize = 0;

    //
    // The WDFWMIPROVIDER handle is needed if multiple instances for the provider
    // has to be created or if the instances have to be created sometime after
    // the provider is created. In case below, the provider handle is not needed
    // because only one instance is needed and can be created when the provider
    // is created.
    //
    WDF_WMI_INSTANCE_CONFIG_INIT_PROVIDER_CONFIG(&instanceConfig, &providerConfig);

    //
    // Create the Provider object as part of the Instance creation call by setting
    // the Register value in the Instance Config to TRUE. This eliminates the
    // need to call WdfWmiProviderRegister.
    //
    instanceConfig.Register = TRUE;

    instanceConfig.EvtWmiInstanceQueryInstance = EvtWmiInstanceStdDeviceDataQueryInstance;
    instanceConfig.EvtWmiInstanceSetInstance   = EvtWmiInstanceStdDeviceDataSetInstance;
    instanceConfig.EvtWmiInstanceSetItem       = EvtWmiInstanceStdDeviceDataSetItem;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&woa, ToasterDeviceInformation);

    //
    // Create the WMI instance object for this data block.
    //
    status = WdfWmiInstanceCreate(Device,
                                  &instanceConfig,
                                  &woa,
                                  &instance);
    if (!NT_SUCCESS(status)) {

        KdPrint(("[Toaster] Status = 0x%08x, WdfWmiInstanceCreate failed\n", status));
        return status;
    }

    pData = ToasterWmiGetData(instance);

    pData->ConnectorType = TOASTER_WMI_STD_USB;
    pData->Capacity = 2000;
    pData->ErrorCount = 0;
    pData->Controls = 5;
    pData->DebugPrintLevel = DebugLevel;

    WDF_WMI_PROVIDER_CONFIG_INIT(&providerConfig, &TOASTER_NOTIFY_DEVICE_ARRIVAL_EVENT);
    providerConfig.Flags = WdfWmiProviderEventOnly;

    //
    // Specify minimum expected buffer size for query and set instance requests.
    // Since the query block size is different than the set block size, set it
    // to zero and manually check for the buffer size for each operation.
    //
    providerConfig.MinInstanceBufferSize = 0;

    WDF_WMI_INSTANCE_CONFIG_INIT_PROVIDER_CONFIG(&instanceConfig, &providerConfig);
    instanceConfig.Register = TRUE;

    //
    // Create the WMI instance object for this data block.
    //
    status = WdfWmiInstanceCreate(Device,
                                  &instanceConfig,
                                  WDF_NO_OBJECT_ATTRIBUTES,
                                  &fdoData->WmiDeviceArrivalEvent);

    if (!NT_SUCCESS(status)) {

        KdPrint(("[Toaster] Status = 0x%08x, WdfWmiInstanceCreate failed\n", status));
        return status;
    }


    //
    // Register the Toaster Control class.
    //

    //
    // Initialize the config structures for the Provider and the Instance and
    // define event callback functions that support a WMI client's request to
    // access the driver's WMI data blocks.
    //

    WDF_WMI_PROVIDER_CONFIG_INIT(&providerConfig, &ToasterControl_GUID);

    //
    // Specify minimum expected buffer size for query and set instance requests.
    //
    providerConfig.MinInstanceBufferSize = ToasterControl_SIZE;

    //
    // The WDFWMIPROVIDER handle is needed if multiple instances for the provider
    // has to be created or if the instances have to be created sometime after
    // the provider is created. In case below, the provider handle is not needed
    // because only one instance is needed and can be created when the provider
    // is created.
    //
    WDF_WMI_INSTANCE_CONFIG_INIT_PROVIDER_CONFIG(&instanceConfig, &providerConfig);

    //
    // Create the Provider object as part of the Instance creation call by setting
    // the Register value in the Instance Config to TRUE. This eliminates the
    // need to call WdfWmiProviderRegister.
    //
    instanceConfig.Register = TRUE;

    instanceConfig.EvtWmiInstanceQueryInstance = EvtWmiInstanceToasterControlQueryInstance;
    instanceConfig.EvtWmiInstanceSetInstance   = EvtWmiInstanceToasterControlSetInstance;
    instanceConfig.EvtWmiInstanceSetItem       = EvtWmiInstanceToasterControlSetItem;
    instanceConfig.EvtWmiInstanceExecuteMethod = EvtWmiInstanceToasterControlExecuteMethod;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&woa, ToasterControl);

    //
    // Create the WMI instance object for this data block.
    //
    status = WdfWmiInstanceCreate(Device,
                                  &instanceConfig,
                                  &woa,
                                  &instance);
    if (!NT_SUCCESS(status)) {

        KdPrint(("[Toaster] Status = 0x%08x, WdfWmiInstanceCreate failed\n", status));
        return status;
    }

    controlData = ToasterWmiGetControlData(instance);
    controlData->ControlValue = 25;

    return status;
}

//
// WMI System Call back functions
//

NTSTATUS
EvtWmiInstanceStdDeviceDataQueryInstance(
    __in  WDFWMIINSTANCE WmiInstance,
    __in  ULONG OutBufferSize,
    __in  PVOID OutBuffer,
    __out PULONG BufferUsed
    )
{
    PUCHAR          pBuf;
    ULONG           size;
    UNICODE_STRING  string;
    NTSTATUS        status;

    PAGED_CODE();

    string.Buffer = L"Aishwarya\0\0";
    string.Length = (USHORT) (wcslen(string.Buffer) + 1) * sizeof(WCHAR);
    string.MaximumLength = string.Length + sizeof(UNICODE_NULL);

    //
    // A USHORT value is needed to contain the length of the buffer in the data
    //
    size = ToasterDeviceInformation_SIZE + string.Length + sizeof(USHORT);

    *BufferUsed = size;

    if (OutBufferSize < size) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    pBuf = (PUCHAR) OutBuffer;

    //
    // Copy the structure information
    //
    RtlCopyMemory(pBuf,
                  ToasterWmiGetData(WmiInstance),
                  ToasterDeviceInformation_SIZE);

    pBuf += ToasterDeviceInformation_SIZE;

    //
    // Copy the string. Put length of string ahead of string.
    //
    status = WDF_WMI_BUFFER_APPEND_STRING(pBuf,
                                          size - ToasterDeviceInformation_SIZE,
                                          &string,
                                          &size);

    return status;
}


NTSTATUS
EvtWmiInstanceStdDeviceDataSetInstance(
    __in WDFWMIINSTANCE WmiInstance,
    __in ULONG InBufferSize,
    __in PVOID InBuffer
    )
{
    PAGED_CODE();

    if (InBufferSize < ToasterDeviceInformation_SIZE) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // We will update only writable elements.
    //
    DebugLevel = ToasterWmiGetData(WmiInstance)->DebugPrintLevel =
        ((PToasterDeviceInformation)InBuffer)->DebugPrintLevel;

    return STATUS_SUCCESS;
}


NTSTATUS
EvtWmiInstanceStdDeviceDataSetItem(
    __in WDFWMIINSTANCE WmiInstance,
    __in ULONG DataItemId,
    __in ULONG InBufferSize,
    __in PVOID InBuffer
    )
{
    PAGED_CODE();

    if (DataItemId == ToasterDeviceInformation_DebugPrintLevel_ID) {
        if (InBufferSize < sizeof(ULONG)) {
            return STATUS_BUFFER_TOO_SMALL;
        }

        DebugLevel = ToasterWmiGetData(WmiInstance)->DebugPrintLevel =
            *((PULONG)InBuffer);

        return STATUS_SUCCESS;

    } else {
        return STATUS_WMI_READ_ONLY;
    }
}


NTSTATUS
ToasterFireArrivalEvent(
    __in WDFDEVICE Device
    )
{
    WDFMEMORY memory;
    PWNODE_SINGLE_INSTANCE  wnode;
    ULONG                   wnodeSize;
    ULONG                   wnodeDataBlockSize;
    ULONG                   wnodeInstanceNameSize;
    ULONG                   size;
    ULONG                   length;
    UNICODE_STRING          deviceName;
    UNICODE_STRING          modelName;
    NTSTATUS                status;

    //
    // *NOTE*
    // WdfWmiFireEvent only fires single instance events at the moment so
    // continue to use this method of firing events
    // *NOTE*
    //

    RtlInitUnicodeString(&modelName, L"Sonali\0\0");
    //
    // Get the device name.
    //
    status = GetDeviceFriendlyName(Device, &memory);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    RtlInitUnicodeString(&deviceName, (PWSTR) WdfMemoryGetBuffer(memory, NULL));

    //
    // Determine the amount of wnode information we need.
    //
    wnodeSize = sizeof(WNODE_SINGLE_INSTANCE);
    wnodeInstanceNameSize = deviceName.Length + sizeof(USHORT);
    wnodeDataBlockSize = modelName.Length + sizeof(USHORT);

    size = wnodeSize + wnodeInstanceNameSize + wnodeDataBlockSize;

    //
    // Allocate memory for the WNODE from NonPagedPool
    //
    wnode = ExAllocatePoolWithTag(NonPagedPool, size, TOASTER_POOL_TAG);

    if (NULL != wnode) {
        RtlZeroMemory(wnode, size);

        wnode->WnodeHeader.BufferSize = size;
        wnode->WnodeHeader.ProviderId =
                            IoWMIDeviceObjectToProviderId(
                                    WdfDeviceWdmGetDeviceObject(Device));
        wnode->WnodeHeader.Version = 1;
        KeQuerySystemTime(&wnode->WnodeHeader.TimeStamp);

        RtlCopyMemory(&wnode->WnodeHeader.Guid,
                      &TOASTER_NOTIFY_DEVICE_ARRIVAL_EVENT,
                      sizeof(GUID));

        //
        // *NOTE*
        // WdfWmiFireEvent only fires single instance events at the moment so
        // continue to use this method of firing events
        // *NOTE*
        //

        //
        // Set flags to indicate that you are creating dynamic instance names.
        // The reason we chose to do dynamic instance is becuase we can fire
        // the events anytime. If we do static instance names, we can only
        // fire events after WMI queries for IRP_MN_REGINFO, which happens
        // after the device has been started. Another point to note is that if you
        // are firing an event after the device is started, you should
        // check whether the event is enabled, because that
        // indicates that someone is interested in receiving the event.
        // Why fire an event when nobody is interested and waste
        // system resources?
        //

        wnode->WnodeHeader.Flags = WNODE_FLAG_EVENT_ITEM |
                                    WNODE_FLAG_SINGLE_INSTANCE;

        wnode->OffsetInstanceName = wnodeSize;
        wnode->DataBlockOffset = wnodeSize + wnodeInstanceNameSize;
        wnode->SizeDataBlock = wnodeDataBlockSize;

        //
        // Write the instance name
        //
        size -= wnodeSize;
        status = WDF_WMI_BUFFER_APPEND_STRING(
            WDF_PTR_ADD_OFFSET(wnode, wnode->OffsetInstanceName),
            size,
            &deviceName,
            &length
            );

        //
        // Size was precomputed, this should never fail
        //
        ASSERT(NT_SUCCESS(status));


        //
        // Write the data, which is the model name as a string
        //
        size -= wnodeInstanceNameSize;
        WDF_WMI_BUFFER_APPEND_STRING(
            WDF_PTR_ADD_OFFSET(wnode,  wnode->DataBlockOffset),
            size,
            &modelName,
            &length
            );

        //
        // Size was precomputed, this should never fail
        //
        ASSERT(NT_SUCCESS(status));

        //
        // Indicate the event to WMI. WMI will take care of freeing
        // the WMI struct back to pool.
        //
        status = IoWMIWriteEvent(wnode);

        if (!NT_SUCCESS(status)) {
            KdPrint( ("IoWMIWriteEvent failed %x\n", status));
            ExFreePool(wnode);
        }
    }
    else {
        status = STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Free the memory allocated by GetDeviceFriendlyName function.
    //
    WdfObjectDelete(memory);

    return status;
}


NTSTATUS
GetDeviceFriendlyName(
    __in    WDFDEVICE Device,
    __out   WDFMEMORY* DeviceName
    )

/*++

Routine Description:

    Return the friendly name associated with the given device object.

Arguments:

Return Value:

    NT status

--*/

{
    NTSTATUS status;

    //
    // First get the length of the string. If the FriendlyName
    // is not there then get the lenght of device description.
    //
    status = WdfDeviceAllocAndQueryProperty(Device,
                                            DevicePropertyFriendlyName,
                                            NonPagedPool,
                                            WDF_NO_OBJECT_ATTRIBUTES,
                                            DeviceName);

    if (!NT_SUCCESS(status) && status != STATUS_INSUFFICIENT_RESOURCES) {
        status = WdfDeviceAllocAndQueryProperty(Device,
                                                DevicePropertyDeviceDescription,
                                                NonPagedPool,
                                                WDF_NO_OBJECT_ATTRIBUTES,
                                                DeviceName);

    }

    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfDeviceQueryProperty returned %x\n", status));
    }

    return status;
}


NTSTATUS
EvtWmiInstanceToasterControlQueryInstance(
    __in  WDFWMIINSTANCE WmiInstance,
    __in  ULONG OutBufferSize,
    __in  PVOID OutBuffer,
    __out PULONG BufferUsed
    )

/*++

Routine Description:

    This is the callback routine for the WMI Query irp on the Instance representing
    the ToasterControl Class. This method copies the ToasterControl Class data into
    the given buffer.

Arguments:

    WmiInstance - The handle to the WMI instance object.

    OutBufferSize - The size (in bytes) of the output buffer into which the
        instance data is to be copied.

    OutBuffer - Pointer to the output buffer.

    BufferUsed - Pointer to the location that receives the number of bytes that
        were copied into the output buffer.

Return Value:

    NT Status code.

--*/

{
    UNREFERENCED_PARAMETER(OutBufferSize);
    PAGED_CODE();

    //
    // Since the minimum buffer size required for querying the instance data was
    // already specified during the WMI instance setup, the Framework will make
    // sure that the incoming buffer size is large enough for this instance query.
    // There is no need to check the size of the given buffer again. The instance
    // information can be copied directly to the given buffer.
    //
    RtlCopyMemory(OutBuffer,
                  ToasterWmiGetControlData(WmiInstance),
                  ToasterControl_SIZE);
    *BufferUsed = ToasterControl_SIZE;

    return STATUS_SUCCESS;
}


NTSTATUS
EvtWmiInstanceToasterControlSetInstance(
    __in WDFWMIINSTANCE WmiInstance,
    __in ULONG InBufferSize,
    __in PVOID InBuffer
    )

/*++

Routine Description:

    This is the callback routine for the WMI Set instance information irp on the
    Instance representing the ToasterControl Class. This method reads the data
    from the input buffer and updates the writable elements of the Instance.

Arguments:

    WmiInstance - The handle to the WMI instance object.

    InBufferSize - The size (in bytes) of the input buffer.

    InBuffer - Pointer to the input buffer containing the data for the instance
        updates.

Return Value:

    NT Status code.

--*/

{
    UNREFERENCED_PARAMETER(InBufferSize);
    PAGED_CODE();

    //
    // Since the minimum buffer size required for setting the instance data was
    // already specified during the WMI instance setup, the Framework will make
    // sure that the incoming buffer size is large enough for this set requrest.
    // There is no need to check the size of the given buffer again. The instance
    // information can be copied directly from the given buffer. Also, note that
    // only the wriable elements need to be copied.
    //
    ToasterWmiGetControlData(WmiInstance)->ControlValue = ((PToasterControl)InBuffer)->ControlValue;

    return STATUS_SUCCESS;
}


NTSTATUS
EvtWmiInstanceToasterControlSetItem(
    __in WDFWMIINSTANCE WmiInstance,
    __in ULONG DataItemId,
    __in ULONG InBufferSize,
    __in PVOID InBuffer
    )
{
    if (DataItemId == ToasterControl_ControlValue_ID) {
        if (InBufferSize < sizeof(ULONG)) {
            return STATUS_BUFFER_TOO_SMALL;
        }

        ToasterWmiGetControlData(WmiInstance)->ControlValue = *((PULONG)InBuffer);
        return STATUS_SUCCESS;

    } else {
        return STATUS_WMI_READ_ONLY;
    }
}


NTSTATUS
EvtWmiInstanceToasterControlExecuteMethod(
    __in    WDFWMIINSTANCE WmiInstance,
    __in    ULONG MethodId,
    __in    ULONG InBufferSize,
    __in    ULONG OutBufferSize,
    __inout PVOID Buffer,
    __out   PULONG BufferUsed
    )

/*++

Routine Description:

    This routine handles the Execute Method WMI request to the Toaster device.

Arguments:

    WmiInstance - The handle to the WMI instance object.

    MethodId - The method id of the method in ToasterControl class to execute.

    InBufferSize - The size (in bytes) of the input buffer from which the input
        parameter values are read.

    OutBufferSize - The size (in bytes) of the output buffer into which the
        instance data is to be copied.

    Buffer - Pointer to the buffer used for both input and output of data.

    BufferUsed - Pointer to the location that receives the number of bytes that
        were copied into the output buffer.

Return Value:

    NT Status code.

--*/

{
    NTSTATUS status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(WmiInstance);
    PAGED_CODE();

    switch (MethodId) {
    case ToasterControl1:
        {
            PToasterControl1_IN  InBuffer  = (PToasterControl1_IN) Buffer;
            PToasterControl1_OUT OutBuffer = (PToasterControl1_OUT)Buffer;

            //
            // Check the size of the input and output buffers.
            //
            if (InBufferSize < ToasterControl1_IN_SIZE) {
                status = STATUS_INVALID_PARAMETER_MIX;
                break;
            }

            if (OutBufferSize < ToasterControl1_OUT_SIZE) {
                status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            //
            // Call the helper function for the Toaster Method 1.
            //
            OutBuffer->OutData = ToasterHelperFunction1(InBuffer->InData);

            //
            // Update the size of the output buffer used.
            //
            *BufferUsed = ToasterControl1_OUT_SIZE;

        }
        break;

    case ToasterControl2:
        {
            PToasterControl2_IN  InBuffer  = (PToasterControl2_IN) Buffer;
            PToasterControl2_OUT OutBuffer = (PToasterControl2_OUT)Buffer;

            //
            // Check the size of the input and output buffers.
            //
            if (InBufferSize < ToasterControl2_IN_SIZE) {
                status = STATUS_INVALID_PARAMETER_MIX;
                break;
            }

            if (OutBufferSize < ToasterControl2_OUT_SIZE) {
                status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            //
            // Call the helper function for the Toaster Method 2.
            //
            OutBuffer->OutData = ToasterHelperFunction2(InBuffer->InData1, InBuffer->InData2);

            //
            // Update the size of the output buffer used.
            //
            *BufferUsed = ToasterControl2_OUT_SIZE;

        }
        break;

    case ToasterControl3:
        {
            PToasterControl3_IN  InBuffer  = (PToasterControl3_IN) Buffer;
            PToasterControl3_OUT OutBuffer = (PToasterControl3_OUT)Buffer;

            //
            // Check the size of the input and output buffers.
            //
            if (InBufferSize < ToasterControl3_IN_SIZE) {
                status = STATUS_INVALID_PARAMETER_MIX;
                break;
            }

            if (OutBufferSize < ToasterControl3_OUT_SIZE) {
                status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            //
            // Call the helper function for the Toaster Method 3.
            //
            ToasterHelperFunction3(InBuffer->InData1,
                                   InBuffer->InData2,
                                   &(OutBuffer->OutData1),
                                   &(OutBuffer->OutData2));

            //
            // Update the size of the output buffer used.
            //
            *BufferUsed = ToasterControl3_OUT_SIZE;

        }
        break;

    default:
        status = STATUS_WMI_ITEMID_NOT_FOUND;
        break;
    }

    return status;
}


ULONG
ToasterHelperFunction1(
    __in ULONG InData
    )
{
    PAGED_CODE();

    return (InData + 1);
}


ULONG
ToasterHelperFunction2(
    __in ULONG InData1,
    __in ULONG InData2
    )
{
    PAGED_CODE();

    return (InData1 + InData2);
}


VOID
ToasterHelperFunction3(
    __in  ULONG InData1,
    __in  ULONG InData2,
    __out PULONG OutData1,
    __out PULONG OutData2
    )
{
    PAGED_CODE();

    *OutData1 = InData1 + 1;
    *OutData2 = InData2 + 1;

    return;
}

