/*++

Copyright (C) Microsoft Corporation. All rights reserved.

Module Name:

    ioctl.h

Abstract:

    Functions to handle IOCTLs.

Author:

Environment:

    kernel mode only 

Notes:


Revision History:

--*/

#ifndef __IOCTL_H__
#define __IOCTL_H__

BOOLEAN
RequestDispatchProcessDirectly(
    __in WDFDEVICE              Device, 
    __in WDFREQUEST             Request, 
    __in WDF_REQUEST_PARAMETERS RequestParameters
    );

BOOLEAN
RequestDispatchToSequentialQueue(
    __in WDFDEVICE              Device, 
    __in WDFREQUEST             Request, 
    __in WDF_REQUEST_PARAMETERS RequestParameters
    );

BOOLEAN
RequestDispatchSyncWithSequentialQueue(
    __in WDFDEVICE              Device, 
    __in WDFREQUEST             Request, 
    __in WDF_REQUEST_PARAMETERS RequestParameters
    );

BOOLEAN
RequestDispatchSpecialIoctls(
    __in WDFDEVICE              Device, 
    __in WDFREQUEST             Request, 
    __in WDF_REQUEST_PARAMETERS RequestParameters
    );

BOOLEAN
RequestDispatchUnknownRequests(
    __in WDFDEVICE              Device, 
    __in WDFREQUEST             Request, 
    __in WDF_REQUEST_PARAMETERS RequestParameters
    );

//
//  I/O Request Handlers
//

// Handlers that are called directly in dispatch routine.

NTSTATUS
RequestHandleGetInquiryData(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestHandleGetMediaTypeEx(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __out size_t *                 DataLength
    );

NTSTATUS
RequestHandleMountQueryUniqueId(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestHandleMountQueryDeviceName(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestHandleMountQuerySuggestedLinkName(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestHandleGetDeviceNumber(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestHandleGetHotPlugInfo(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestHandleSetHotPlugInfo(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

// Handlers that are called in RequestProcessSerializedIoctl in a work item.

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleGetDvdRegion(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __out size_t *                 DataLength
    );

NTSTATUS
RequestHandleQueryPropertyRetrieveCachedData(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );
                            
__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleReadTOC(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleReadTocEx(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );
                            
__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleGetConfiguration(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleGetDriveGeometry(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleDiskVerify(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleCheckVerify(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleFakePartitionInfo(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleEjectionControl(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleLoadEjectMedia(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleMcnControl(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleReserveRelease(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandlePersistentReserve(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleRawRead(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandlePlayAudioMsf(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleReadQChannel(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandlePauseAudio(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleResumeAudio(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleSeekAudioMsf(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleStopAudio(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleGetSetVolume(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleReadDvdStructure(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleDvdEndSession(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleDvdStartSessionReadKey(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleDvdSendKey(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleSetReadAhead(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleSetSpeed(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleAacsReadMediaKeyBlock(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleAacsStartSession(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleAacsEndSession(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleAacsSendCertificate(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleAacsGetCertificate(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleAacsGetChallengeKey(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleSendChallengeKey(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleReadVolumeId(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleAacsReadSerialNumber(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleAacsReadMediaId(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleAacsReadBindingNonce(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
DeviceHandleAacsGenerateBindingNonce(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleEnableStreaming(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleSendOpcInformation(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __out size_t *                 DataLength
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleGetPerformance(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );



// Handlers that will be called by Sync process.

// RequestHandleUnknownIoctl could be called at DISPATCH_LEVEL.
NTSTATUS
RequestHandleUnknownIoctl(
    __in WDFDEVICE    Device,
    __in WDFREQUEST   Request
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleExclusiveAccessQueryLockState(
    __in WDFDEVICE    Device,
    __in WDFREQUEST   Request
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleExclusiveAccessLockDevice(
    __in WDFDEVICE    Device,
    __in WDFREQUEST   Request
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleExclusiveAccessUnlockDevice(
    __in WDFDEVICE    Device,
    __in WDFREQUEST   Request
    );

NTSTATUS
RequestHandleQueryPropertyDeviceUniqueId(
    __in WDFDEVICE    Device, 
    __in WDFREQUEST   Request
    );

NTSTATUS
RequestHandleQueryPropertyWriteCache(
    __in WDFDEVICE    Device, 
    __in WDFREQUEST   Request
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
RequestHandleScsiPassThrough(
    __in WDFDEVICE    Device, 
    __in WDFREQUEST   Request
    );

// Read/write handler called possibly at DISPATCH_LEVEL.

NTSTATUS
RequestHandleReadWrite(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters
    );

//
//  I/O Request Validation helpers
//

NTSTATUS
RequestValidateRawRead(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateReadTocEx(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateReadToc(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateGetLastSession(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateReadQChannel(
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateDvdReadStructure(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateDvdStartSession(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateDvdSendKey(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateDvdReadKey(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateGetConfiguration(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateSetSpeed(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateAacsReadMediaKeyBlock(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateAacsStartSession(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateAacsSendCertificate(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateAacsGetCertificate(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateAacsGetChallengeKey(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateAacsSendChallengeKey(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateAacsReadVolumeId(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateAacsReadSerialNumber(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateAacsReadMediaId(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateAacsBindingNonce(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateExclusiveAccess(
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateEnableStreaming(
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateSendOpcInformation(
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateGetPerformance(
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

BOOLEAN
RequestIsRealtimeStreaming(
    __in  WDFREQUEST               Request,
    __in  BOOLEAN                  IsReadRequest
    );

NTSTATUS
RequestValidateReadWrite(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters
    );

NTSTATUS
RequestValidatePersistentReserve(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateDvdEndSession(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );

NTSTATUS
RequestValidateAacsEndSession(
    __in  PCDROM_DEVICE_EXTENSION  DeviceExtension, 
    __in  WDFREQUEST               Request, 
    __in  WDF_REQUEST_PARAMETERS   RequestParameters,
    __out size_t *                 DataLength
    );


//
// completion routines
//




#endif // __IOCTL_H__

