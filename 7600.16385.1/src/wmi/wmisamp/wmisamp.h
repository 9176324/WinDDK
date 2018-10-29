/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
    EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.

Module Name:

    WmiSamp.h

Abstract:

    --

Environment:

    Kernel mode

--*/

#include <ntddk.h>
#include <wdf.h>

#include <initguid.h>   // required for GUID definitions
#include <wdmguid.h>    // required for WMILIB_CONTEXT
#include <wmistr.h>
#include <wmilib.h>
#include "WmiData.h"


//
// A macro for Pointer addition.
//
#define Add2Ptr(P,I)    ((PVOID)((PUCHAR)(P) + (I)))


//
// Define DebugPrint for sending messages to the debugger in checked builds.
//
#if DBG
#define DebugPrint      KdPrint
#else
#define DebugPrint
#endif


typedef struct _WMI_SAMPLE_INSTANCE_CONFIG {

    GUID Guid;
    ULONG MinSize;
    PFN_WDF_WMI_INSTANCE_QUERY_INSTANCE EvtWmiInstanceQueryInstance;
    PFN_WDF_WMI_INSTANCE_SET_INSTANCE EvtWmiInstanceSetInstance;
    PFN_WDF_WMI_INSTANCE_SET_ITEM EvtWmiInstanceSetItem;
    PFN_WDF_WMI_INSTANCE_EXECUTE_METHOD EvtWmiInstanceExecuteMethod;

} WMI_SAMPLE_INSTANCE_CONFIG, *PWMI_SAMPLE_INSTANCE_CONFIG;


#define EC1_COUNT       4
#define EC2_COUNT       4

//
// Data storage for WMI data blocks.
//
typedef struct _WMI_SAMPLE_DEVICE_DATA {

    ULONG Ec1Count;
    ULONG Ec1Length[EC1_COUNT];
    ULONG Ec1ActualLength[EC1_COUNT];
    PEC1 Ec1[EC1_COUNT];
    WDFSPINLOCK Ec1Lock;

    ULONG Ec2Count;
    ULONG Ec2Length[EC2_COUNT];
    ULONG Ec2ActualLength[EC2_COUNT];
    PEC2 Ec2[EC2_COUNT];
    WDFSPINLOCK Ec2Lock;

    WDFWMIINSTANCE DynamicInstance;
    ULONG CreateCount;

} WMI_SAMPLE_DEVICE_DATA, *PWMI_SAMPLE_DEVICE_DATA;


//
// Specify an accessor method for the WMI_SAMPLE_DEVICE_DATA structure.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WMI_SAMPLE_DEVICE_DATA, GetWmiSampleDeviceData)


DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_DEVICE_ADD WmiSampEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_DESTROY WmiSampDeviceEvtDestroyCallback;

NTSTATUS
WmiSampWmiRegistration(
    __in WDFDEVICE Device
    );

NTSTATUS
WmiSampDynamicWmiRegistration(
    __in WDFDEVICE Device
    );

ULONG
WmiSampGetEc1(
    __in    PWMI_SAMPLE_DEVICE_DATA WmiDeviceData,
    __inout PVOID Buffer,
    __in    ULONG Index
    );

VOID
WmiSampSetEc1(
    __in PWMI_SAMPLE_DEVICE_DATA WmiDeviceData,
    __in PVOID Buffer,
    __in ULONG Length,
    __in ULONG Index
    );

ULONG
WmiSampGetEc2(
    __in    PWMI_SAMPLE_DEVICE_DATA WmiDeviceData,
    __inout PVOID Buffer,
    __in    ULONG Index
    );

VOID
WmiSampSetEc2(
    __in PWMI_SAMPLE_DEVICE_DATA WmiDeviceData,
    __in PVOID Buffer,
    __in ULONG Length,
    __in ULONG Index
    );


