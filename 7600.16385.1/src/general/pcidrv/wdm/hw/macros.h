/****************************************************************************
** COPYRIGHT (C) 1994-1997 INTEL CORPORATION                               **
** DEVELOPED FOR MICROSOFT BY INTEL CORP., HILLSBORO, OREGON               **
** HTTP://WWW.INTEL.COM/                                                   **
** THIS FILE IS PART OF THE INTEL ETHEREXPRESS PRO/100B(TM) AND            **
** ETHEREXPRESS PRO/100+(TM) NDIS 5.0 MINIPORT SAMPLE DRIVER               **
****************************************************************************/

/****************************************************************************
Module Name:
     e100_sup.h     (inlinef.h)

This driver runs on the following hardware:
     - 82558 based PCI 10/100Mb ethernet adapters
     (aka Intel EtherExpress(TM) PRO Adapters)

Environment:
     Kernel Mode - Or whatever is the equivalent on WinNT

*****************************************************************************/

__inline BOOLEAN
WaitScb(
    __in PFDO_DATA FdoData
    );


//-----------------------------------------------------------------------------
// Procedure:   D100IssueScbCommand
//
// Description: This general routine will issue a command to the D100.
//
// Arguments:
//      FdoData - ptr to FdoData object instance.
//      ScbCommand - The command that is to be issued
//      WaitForSCB - A boolean value indicating whether or not a wait for SCB
//                   must be done before the command is issued to the chip
//
// Returns:
//      TRUE if the command was issued to the chip successfully
//      FALSE if the command was not issued to the chip
//-----------------------------------------------------------------------------
__inline NTSTATUS
D100IssueScbCommand(
    __in PFDO_DATA FdoData,
    __in UCHAR ScbCommandLow,
    __in BOOLEAN WaitForScb
    )
{
    if(WaitForScb == TRUE)
    {
        if(!WaitScb(FdoData))
        {
            return(STATUS_DEVICE_DATA_ERROR);
        }
    }

    FdoData->CSRAddress->ScbCommandLow = ScbCommandLow;

    return(STATUS_SUCCESS);
}


__inline NTSTATUS
MP_GET_STATUS_FROM_FLAGS(
    __in PFDO_DATA FdoData
    )
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    if(MP_TEST_FLAG(FdoData, fMP_ADAPTER_RESET_IN_PROGRESS))
    {
        Status = STATUS_DEVICE_NOT_READY;
    }
    else if(MP_TEST_FLAG(FdoData, fMP_ADAPTER_HARDWARE_ERROR))
    {
        Status = STATUS_DEVICE_OFF_LINE;
    }
    else if(MP_TEST_FLAG(FdoData, fMP_ADAPTER_NO_CABLE))
    {
        Status = STATUS_DEVICE_NOT_CONNECTED;
    }

    return Status;
}

__inline VOID
NICDisableInterrupt(
    __in PFDO_DATA FdoData
    )
{
   FdoData->CSRAddress->ScbCommandHigh = SCB_INT_MASK;
}

KSYNCHRONIZE_ROUTINE NICEnableInterrupt;
    
__inline 
BOOLEAN
NICEnableInterrupt(
    PVOID Context
    )
{
    PFDO_DATA FdoData = Context;
    FdoData->CSRAddress->ScbCommandHigh = 0;
    return TRUE;
}

__inline
BOOLEAN
IsPoMgmtSupported(
   __in PFDO_DATA FdoData
   )
{

    if (FdoData->RevsionID  >= E100_82559_A_STEP  &&
         /*FdoData->RevsionID <= E100_82559_C_STEP &&*/
         FdoData->DeviceCaps.SystemWake != PowerSystemUnspecified &&
         FdoData->DeviceCaps.DeviceWake != PowerDeviceUnspecified )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

__inline
USHORT
NICReadPortUShort (
    __in  USHORT * x
    )
{
    return READ_PORT_USHORT (x);
}
__inline
VOID
NICWritePortUShort (
    __in  USHORT * x,
    __in  USHORT   y
    )
{
    WRITE_PORT_USHORT (x,y);
}

__inline
USHORT
NICReadRegisterUShort (
    __in  USHORT * x
    )
{
    return READ_REGISTER_USHORT (x);
}

__inline
VOID
NICWriteRegisterUShort (
    __in  USHORT * x,
    __in  USHORT   y
    )
{
    WRITE_REGISTER_USHORT (x,y);
}



// routines.c

BOOLEAN
MdiRead(
    __in PFDO_DATA Adapter,
    __in ULONG RegAddress,
    __in ULONG PhyAddress,
    __in BOOLEAN  Recoverable,
    __inout PUSHORT DataValue
    );

VOID
MdiWrite(
    __in PFDO_DATA FdoData,
    __in ULONG RegAddress,
    __in ULONG PhyAddress,
    __in USHORT DataValue
    );

MEDIA_STATE
GetMediaState(
    __in PFDO_DATA Adapter
    );

NTSTATUS
D100SubmitCommandBlockAndWait(
    __in PFDO_DATA Adapter
    );

VOID
NICIssueFullReset(
    PFDO_DATA Adapter
    );

VOID
NICIssueSelectiveReset(
    PFDO_DATA Adapter
    );

VOID
DumpStatsCounters(
    __in PFDO_DATA Adapter
    );



// physet.c

VOID
ResetPhy(
    __in PFDO_DATA FdoData
    );

NTSTATUS
PhyDetect(
    __in PFDO_DATA FdoData
    );

NTSTATUS
ScanAndSetupPhy(
    __in PFDO_DATA FdoData
    );

VOID
SelectPhy(
    __in PFDO_DATA FdoData,
    __in UINT SelectPhyAddress,
    __in BOOLEAN WaitAutoNeg
    );

NTSTATUS
SetupPhy(
    __in PFDO_DATA FdoData
    );

VOID
FindPhySpeedAndDpx(
    __in PFDO_DATA FdoData,
    __in UINT PhyId
    );



// eeprom.c
USHORT
GetEEpromAddressSize(
    __in USHORT Size
    );

USHORT
GetEEpromSize(
    __in PFDO_DATA FdoData,
    __in PUCHAR CSRBaseIoAddress
    );

USHORT
ReadEEprom(
    __in PFDO_DATA FdoData,
    __in PUCHAR CSRBaseIoAddress,
    __in USHORT Reg,
    __in USHORT AddressSize
    );

VOID
ShiftOutBits(
    __in PFDO_DATA FdoData,
    __in USHORT data,
    __in USHORT count,
    __in PUCHAR CSRBaseIoAddress
    );

USHORT
ShiftInBits(
    __in PFDO_DATA FdoData,
    __in PUCHAR CSRBaseIoAddress
    );

VOID
RaiseClock(
    __in PFDO_DATA FdoData,
    __inout USHORT *x,
    __in PUCHAR CSRBaseIoAddress
    );

VOID
LowerClock(
    __in PFDO_DATA FdoData,
    __inout USHORT *x,
    __in PUCHAR CSRBaseIoAddress
    );

VOID
EEpromCleanup(
    __in PFDO_DATA FdoData,
    __in PUCHAR CSRBaseIoAddress
    );

