/*++

Copyright (C) Microsoft Corporation. All rights reserved.

Module Name:

    mmc.h

Abstract:

    Functions for MMC area.

Author:

Environment:

    kernel mode only

Notes:


Revision History:

--*/

#ifndef __MMC_H__
#define __MMC_H__

__drv_maxIRQL(APC_LEVEL)
VOID
DeviceDeallocateMmcResources(
    __in WDFDEVICE Device
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceAllocateMmcResources(
    __in WDFDEVICE Device
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceUpdateMmcCapabilities(
    __in WDFDEVICE Device
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceGetConfigurationWithAlloc(
    __in WDFDEVICE                  Device,
    __deref_opt_out_bcount_full(*BytesReturned) 
    PGET_CONFIGURATION_HEADER *     Buffer,
    __out PULONG                    BytesReturned,
    FEATURE_NUMBER const            StartingFeature,
    ULONG const                     RequestedType
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceGetConfiguration(
    __in  WDFDEVICE                 Device,
    __out_bcount_part(BufferSize, *ValidBytes)
          PGET_CONFIGURATION_HEADER Buffer,
    __in  ULONG const               BufferSize,
    __out PULONG                    ValidBytes,
    __in  FEATURE_NUMBER const      StartingFeature,
    __in  ULONG const               RequestedType
    );

__drv_maxIRQL(APC_LEVEL)
VOID
DeviceUpdateMmcWriteCapability(
    __in_bcount(BufferSize) 
        PGET_CONFIGURATION_HEADER   Buffer,
    ULONG const                     BufferSize,
    BOOLEAN const                   CurrentOnly, // TRUE == can drive write now, FALSE == can drive ever write
    __out PBOOLEAN                  Writable,
    __out PFEATURE_NUMBER           ValidationSchema,
    __out PULONG                    BlockingFactor
    );

__drv_maxIRQL(APC_LEVEL)
PVOID
MmcDataFindFeaturePage(
    __in_bcount(Length) 
       PGET_CONFIGURATION_HEADER    FeatureBuffer,
    ULONG const                     Length,
    FEATURE_NUMBER const            Feature
    );

__drv_maxIRQL(APC_LEVEL)
VOID
MmcDataFindProfileInProfiles(
     __in   FEATURE_DATA_PROFILE_LIST const* ProfileHeader,
     __in   FEATURE_PROFILE_TYPE const       ProfileToFind,
     __in   BOOLEAN const                    CurrentOnly,
     __out  PBOOLEAN                         Found
     );

__drv_maxIRQL(APC_LEVEL)
__range(-1,MAXIMUM_RETRY_FOR_SINGLE_IO_IN_100NS_UNITS)
LONGLONG
DeviceRetryTimeGuessBasedOnProfile(
    FEATURE_PROFILE_TYPE const Profile
    );

__drv_maxIRQL(APC_LEVEL)
__range(-1,MAXIMUM_RETRY_FOR_SINGLE_IO_IN_100NS_UNITS)
LONGLONG
DeviceRetryTimeDetectionBasedOnModePage2A(
    __in PCDROM_DEVICE_EXTENSION DeviceExtension
    );

__drv_maxIRQL(APC_LEVEL)
__range(-1,MAXIMUM_RETRY_FOR_SINGLE_IO_IN_100NS_UNITS)
LONGLONG
DeviceRetryTimeDetectionBasedOnGetPerformance(
    __in PCDROM_DEVICE_EXTENSION DeviceExtension,
    __in BOOLEAN                 UseLegacyNominalPerformance
    );

#endif // __MMC_H__

