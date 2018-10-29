/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    WMI.C

Abstract:

    This module handle all the WMI Irps.

Environment:

    Kernel mode

--*/


#include "precomp.h"

#if defined(EVENT_TRACING)
#include "wmi.tmh"
#endif


#define MOFRESOURCENAME L"PciDrvWMI"

EVT_WDF_WMI_INSTANCE_QUERY_INSTANCE EvtWmiDeviceInfoQueryInstance;

EVT_WDF_WMI_INSTANCE_SET_INSTANCE EvtWmiDeviceInfoSetInstance;

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, PciDrvWmiRegistration)
#pragma alloc_text(PAGE, EvtWmiDeviceInfoQueryInstance)
#pragma alloc_text(PAGE, EvtWmiDeviceInfoSetInstance)
#endif

NTSTATUS
PciDrvWmiRegistration(
    WDFDEVICE      Device
    )
/*++
Routine Description

    Registers with WMI as a data provider for this
    instance of the device

--*/
{
    WDF_WMI_PROVIDER_CONFIG providerConfig;
    WDF_WMI_INSTANCE_CONFIG instanceConfig;
    NTSTATUS        status;
    DECLARE_CONST_UNICODE_STRING(mofRsrcName, MOFRESOURCENAME);

    PAGED_CODE();

    status = WdfDeviceAssignMofResourceName(Device, &mofRsrcName);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
                     "WdfDeviceAssignMofResourceName failed 0x%x", status);
        return status;
    }

    WDF_WMI_PROVIDER_CONFIG_INIT(&providerConfig, &PCIDRV_WMI_STD_DATA_GUID);
    providerConfig.MinInstanceBufferSize = sizeof(PCIDRV_WMI_STD_DATA);

    WDF_WMI_INSTANCE_CONFIG_INIT_PROVIDER_CONFIG(&instanceConfig, &providerConfig);
    instanceConfig.Register = TRUE;
    instanceConfig.EvtWmiInstanceQueryInstance = EvtWmiDeviceInfoQueryInstance;
    instanceConfig.EvtWmiInstanceSetInstance = EvtWmiDeviceInfoSetInstance;

    //
    // No need to get the newly creawted handle because we just reference data
    // from our device extension directly.
    //
    status = WdfWmiInstanceCreate(Device,
                                  &instanceConfig,
                                  WDF_NO_OBJECT_ATTRIBUTES,
                                  WDF_NO_HANDLE);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
                     "WdfWmiInstanceCreate failed 0x%x", status);
        return status;
    }

    return status;
}

NTSTATUS
EvtWmiDeviceInfoQueryInstance(
    __in  WDFWMIINSTANCE WmiInstance,
    __in  ULONG OutBufferSize,
    __out_bcount_part(OutBufferSize, *BufferUsed) PVOID OutBuffer,
    __out PULONG BufferUsed
    )
{
    PFDO_DATA fdoData;

    PAGED_CODE();

    fdoData = FdoGetData(WdfWmiInstanceGetDevice(WmiInstance));

    //
    // No need to check OutBufferSize since MinInstanceBufferSize was set when we
    // created the instance
    //
    RtlZeroMemory(OutBuffer, OutBufferSize);
    RtlCopyMemory(OutBuffer, fdoData->CurrentAddress, sizeof(fdoData->CurrentAddress));

    *BufferUsed = sizeof(fdoData->CurrentAddress);

    return STATUS_SUCCESS;
}

NTSTATUS
EvtWmiDeviceInfoSetInstance(
    __in  WDFWMIINSTANCE WmiInstance,
    __in  ULONG InBufferSize,
    __in_bcount(InBufferSize) PVOID InBuffer
    )
{
    PFDO_DATA fdoData;

    UNREFERENCED_PARAMETER(InBufferSize);

    PAGED_CODE();

    fdoData = FdoGetData(WdfWmiInstanceGetDevice(WmiInstance));

    //
    // No need to check InBufferSize since MinInstanceBufferSize was set when we
    // created the instance
    //
    ASSERT(InBufferSize >= sizeof(fdoData->CurrentAddress));

    RtlCopyMemory(fdoData->CurrentAddress, InBuffer, sizeof(fdoData->CurrentAddress));

    return STATUS_SUCCESS;
}

