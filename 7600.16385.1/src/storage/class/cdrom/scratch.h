/*++

Copyright (C) Microsoft Corporation. All rights reserved.

Module Name:

    mmc.h

Abstract:

    Functions/macros for using the scratch buffer and scratch request/SRB
    in the serial I/O queue context

Author:

Environment:

    kernel mode only

Notes:


Revision History:

--*/

#ifndef __SCRATCH_H__
#define __SCRATCH_H__


__drv_maxIRQL(APC_LEVEL)
VOID
ScratchBuffer_Deallocate(
    __inout PCDROM_DEVICE_EXTENSION DeviceExtension
    );

__drv_maxIRQL(APC_LEVEL)
BOOLEAN
ScratchBuffer_Allocate(
    __inout PCDROM_DEVICE_EXTENSION DeviceExtension
    );

VOID
ScratchBuffer_ResetItems(
    __inout PCDROM_DEVICE_EXTENSION DeviceExtension,
    __in BOOLEAN                 ResetRequestHistory
    );

__drv_maxIRQL(APC_LEVEL)
VOID
ScratchBuffer_SetupSrb(
    __inout PCDROM_DEVICE_EXTENSION DeviceExtension,
    __in_opt WDFREQUEST             OriginalRequest,
    __in ULONG                      MaximumTransferLength,
    __in BOOLEAN                    GetDataFromDevice
    );

VOID
ScratchBuffer_SetupReadWriteSrb(
    __inout PCDROM_DEVICE_EXTENSION     DeviceExtension,
    __in    WDFREQUEST                  OriginalRequest,
    __in    LARGE_INTEGER               StartingOffset,
    __in    ULONG                       RequiredLength,
    __out_bcount(RequiredLength) UCHAR* DataBuffer,
    __in    BOOLEAN                     IsReadRequest,
    __in    BOOLEAN                     UsePartialMdl
    );

NTSTATUS
ScratchBuffer_SendSrb(
    __inout     PCDROM_DEVICE_EXTENSION DeviceExtension,
    __in        BOOLEAN                 SynchronousSrb,
    __precond(SynchronousSrb) __null
    __precond(!SynchronousSrb) __in_opt
                PSRB_HISTORY_ITEM       *SrbHistoryItem
    );

NTSTATUS
ScratchBuffer_PerformNextReadWrite(
    __in PCDROM_DEVICE_EXTENSION  DeviceExtension,
    __in BOOLEAN                  FirstTry
    );

#if DBG
    #define ScratchBuffer_BeginUse(context) ScratchBuffer_BeginUseX((context), __FILE__, __LINE__)
#else
    #define ScratchBuffer_BeginUse(context) ScratchBuffer_BeginUseX((context), NULL, (ULONG)-1)
#endif

__inline VOID ScratchBuffer_BeginUseX(__inout PCDROM_DEVICE_EXTENSION DeviceExtension, __in_opt LPCSTR File, ULONG Line)
{
    // NOTE: these are not "real" locks.  They are simply to help
    //       avoid multiple uses of the scratch buffer. Thus, it
    //       is not critical to have atomic operations here.
    PVOID tmp = InterlockedCompareExchangePointer((PVOID)&(DeviceExtension->ScratchContext.ScratchInUse), (PVOID)-1, NULL);
    ASSERT(tmp == NULL);
    UNREFERENCED_PARAMETER(tmp); //defensive coding, avoid PREFAST warning.
    DeviceExtension->ScratchContext.ScratchInUseFileName = File;
    DeviceExtension->ScratchContext.ScratchInUseLineNumber = Line;
    ScratchBuffer_ResetItems(DeviceExtension, TRUE);
    RequestClearSendTime(DeviceExtension->ScratchContext.ScratchRequest);
    return;
}
__inline VOID ScratchBuffer_EndUse(__inout PCDROM_DEVICE_EXTENSION DeviceExtension)
{
    // NOTE: these are not "real" locks.  They are simply to help
    //       avoid multiple uses of the scratch buffer.  Thus, it
    //       is not critical to have atomic operations here.

    // On lock release, we erase ScratchInUseFileName and ScratchInUseLineNumber _before_ releasing ScratchInUse,
    // because otherwise we may erase these after the lock has been acquired again by another thread. We store the
    // old values of ScratchInUseFileName and ScratchInUseLineNumber in local variables to facilitate debugging,
    // if the ASSERT at the end of the function is hit.
    __nullterminated CHAR const*  scratchInUseFileName;
    ULONG  scratchInUseLineNumber;
    PVOID  tmp;

    scratchInUseFileName = DeviceExtension->ScratchContext.ScratchInUseFileName;
    scratchInUseLineNumber = DeviceExtension->ScratchContext.ScratchInUseLineNumber;
    UNREFERENCED_PARAMETER(scratchInUseFileName);
    UNREFERENCED_PARAMETER(scratchInUseLineNumber);
    DeviceExtension->ScratchContext.ScratchInUseFileName = NULL;
    DeviceExtension->ScratchContext.ScratchInUseLineNumber = 0;

    tmp = InterlockedCompareExchangePointer((PVOID)&(DeviceExtension->ScratchContext.ScratchInUse), NULL, (PVOID)-1);
    ASSERT(tmp == ((PVOID)-1));
    UNREFERENCED_PARAMETER(tmp); //defensive coding, avoid PREFAST warning.
    return;
}

VOID
CompressSrbHistoryData(
    __inout  PSRB_HISTORY   RequestHistory
    );

VOID
ValidateSrbHistoryDataPresumptions(
    __in     SRB_HISTORY const* RequestHistory
    );

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
ScratchBuffer_ExecuteCdbEx(
    __inout PCDROM_DEVICE_EXTENSION DeviceExtension,
    __in_opt WDFREQUEST             OriginalRequest,
    __in ULONG                      TransferSize,
    __in BOOLEAN                    GetDataFromDevice,
    __in PCDB                       Cdb,
    __in UCHAR                      OprationLength,
    __in ULONG                      TimeoutValue
    );

__drv_maxIRQL(APC_LEVEL)
__inline
NTSTATUS
ScratchBuffer_ExecuteCdb(
    __inout PCDROM_DEVICE_EXTENSION DeviceExtension,
    __in_opt WDFREQUEST             OriginalRequest,
    __in ULONG                      TransferSize,
    __in BOOLEAN                    GetDataFromDevice,
    __in PCDB                       Cdb,
    __in UCHAR                      OprationLength
    )
{
    return ScratchBuffer_ExecuteCdbEx(DeviceExtension,
                                      OriginalRequest,
                                      TransferSize,
                                      GetDataFromDevice,
                                      Cdb,
                                      OprationLength,
                                      0);
}

KDEFERRED_ROUTINE ScratchBuffer_ReadWriteTimerRoutine;

#endif //__SCRATCH_H__

