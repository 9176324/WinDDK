//===========================================================================
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright (c) 1996 - 1998  Microsoft Corporation.  All Rights Reserved.
//
//===========================================================================
/*++

Module Name:

    sonydcam.h

Abstract:

    Header file for sonydcam.c 

Environment:

    Kernel mode only

Revision History:


--*/



//
// Function prototypes
//

DRIVER_INITIALIZE DriverEntry;

VOID 
InitializeDeviceExtension(
    PPORT_CONFIGURATION_INFORMATION ConfigInfo
    );

NTSTATUS
DCamHwUnInitialize(
    IN PHW_STREAM_REQUEST_BLOCK pSrb
    );

NTSTATUS
DCamHwInitialize(
    IN PHW_STREAM_REQUEST_BLOCK pSrb
    );

NTSTATUS
DCamSubmitIrpSynch(
    PDCAM_EXTENSION pDevExt,
    PIRP pIrp,
    PIRB pIrb
    );

IO_COMPLETION_ROUTINE DCamSynchCR;

#define COMPLETE_SRB(pSrb) if(pSrb->Flags & SRB_HW_FLAGS_STREAM_REQUEST)  \
                              StreamClassStreamNotification(StreamRequestComplete, pSrb->StreamObject, pSrb); \
                           else  \
                              StreamClassDeviceNotification(DeviceRequestComplete, pSrb->HwDeviceExtension, pSrb); 
        


//
// External functions
//

extern
VOID
DCamCancelOnePacket(
    IN PHW_STREAM_REQUEST_BLOCK pSrb
    );

extern 
VOID
DCamTimeoutHandler(
    IN PHW_STREAM_REQUEST_BLOCK Srb
    );

extern VOID
DCamBusResetNotification(
    IN PVOID Context
    );


