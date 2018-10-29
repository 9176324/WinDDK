/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

#ifndef _SDV_WDMSEC_H_
#define _SDV_WDMSEC_H_

#ifdef IoCreateDeviceSecure
#undef IoCreateDeviceSecure
#endif
#define IoCreateDeviceSecure sdv_IoCreateDeviceSecure 
NTSTATUS
sdv_IoCreateDeviceSecure(
    IN  PDRIVER_OBJECT      DriverObject,
    IN  ULONG               DeviceExtensionSize,
    IN  PUNICODE_STRING     DeviceName              OPTIONAL,
    IN  DEVICE_TYPE         DeviceType,
    IN  ULONG               DeviceCharacteristics,
    IN  BOOLEAN             Exclusive,
    IN  PCUNICODE_STRING    DefaultSDDLString,
    IN  LPCGUID             DeviceClassGuid         OPTIONAL,
    OUT PDEVICE_OBJECT     *DeviceObject
    );

#endif
