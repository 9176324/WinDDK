#pragma once 

BOOLEAN
SetRFPowerState(
    PNIC                Nic, 
    RT_RF_POWER_STATE   eRFPowerState
    );


// Phy functions
void SwChnlPhy(
    PNIC    pNic,
    UCHAR   channel
    );

VOID
HwInternalStartReqComplete(
    __in  PNIC        pNic
    );

ULONG
HwRSSI(
    PNIC        pNic,
    RX_STATUS_DESC_8187     RxDesc
    );

void
HwRestoreToBeforeScan(
    __in PNIC pNic
    );    

void
HwPeriodTimerHandler(
	__in PNIC pNic
	);	

void
HwPostSendHandler(
    __in PNIC     pNic,
    __in ULONG    sentBufferLength
    );


