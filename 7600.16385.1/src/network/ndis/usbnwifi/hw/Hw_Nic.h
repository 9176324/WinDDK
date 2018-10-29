/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Hw_Nic.c

Abstract:
    HW layer NIC specific function
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#define NIC_DELAY_POST_RESET            20

#define RX_DESC_SIZE    16
#define TX_DESC_SIZE    32

#define DoCoalesceThreshold     8

#define     NUM_11A_CHANNEL         46
#define     NUM_11G_CHANNEL         14

//   
//  regulatory domain
//
#define	DOMAIN_FROM_EEPROM				0x0100
#define	DOMAIN_FROM_COUNTRY				0x0200
#define	DOMAIN_FROM_UI					0x0400
#define	CHANNELPLAN_MASK                0x000f

// Regulatory domain values that are not defined yet in the windot11.h
#define HW_REG_DOMAIN_MKK1          0x00000041
#define HW_REG_DOMAIN_ISRAEL        0x00000042


#define MAX_BEACON_PERIOD       1024
#define LIMIT_BEACON_PERIOD(_Period)        \
    if((_Period)>=MAX_BEACON_PERIOD)        \
        (_Period)=MAX_BEACON_PERIOD-1;

#define	HWPERIOD_TIMER_IN_MS	2000 //ms

#define GetListHeadEntry(ListHead)  ((ListHead)->Flink)
#define GetListTailEntry(ListHead)  ((ListHead)->Blink)


// TODO: Testing Check For Hang hypothesis
/*
// N=1~...
#define HwIncrementNextDescToSend(  _pNic, _Queue, N)       \
    (_pNic)->TxInfo.TxNextDescToSend[_Queue]=((_pNic)->TxInfo.TxNextDescToSend[_Queue]+N)%(_pNic)->TxInfo.TxTotalDescNum[_Queue]

#define HwIncrementTxNextDescToCheck(_pNic, _Queue, N)      \
    (_pNic)->TxInfo.TxNextDescToCheck[_Queue]=((_pNic)->TxInfo.TxNextDescToCheck[_Queue]+N)%(_pNic)->TxInfo.TxTotalDescNum[_Queue]
*/

// N=1~...
#define HwIncrementNextDescToSend(_pNic, _Queue, N)     \
{                                                               \
    (_pNic)->TxInfo.TxNextDescToSend[_Queue]=((_pNic)->TxInfo.TxNextDescToSend[_Queue]+N)%(_pNic)->TxInfo.TxTotalDescNum[_Queue];   \
    InterlockedExchangeAdd(&(_pNic)->TxInfo.TxBusyDescCount[_Queue], (LONG)N);                 \
    MPVERIFY(HwGetTxBusyDescNum(_pNic, _Queue) == (ULONG)(_pNic)->TxInfo.TxBusyDescCount[_Queue]);   \
}

#define HwIncrementTxNextDescToCheck(_pNic, _Queue, N)      \
{                                                               \
    (_pNic)->TxInfo.TxNextDescToCheck[_Queue]=((_pNic)->TxInfo.TxNextDescToCheck[_Queue]+N)%(_pNic)->TxInfo.TxTotalDescNum[_Queue]; \
    InterlockedExchangeAdd(&(_pNic)->TxInfo.TxBusyDescCount[_Queue], (((LONG)N) * -1));                 \
    MPVERIFY(HwGetTxBusyDescNum(_pNic, _Queue) == (ULONG)(_pNic)->TxInfo.TxBusyDescCount[_Queue]);  \
}


// N=0~...
#define HwGetNextDescToSend(_pNic,_Queue, N)        \
    &(_pNic)->TxInfo.TxDesc[_Queue][((_pNic)->TxInfo.TxNextDescToSend[_Queue]+N)%(_pNic)->TxInfo.TxTotalDescNum[_Queue]]

#define HwGetNextDescToCheck(_pNic,_Queue, N)       \
    &(_pNic)->TxInfo.TxDesc[_Queue][((_pNic)->TxInfo.TxNextDescToCheck[_Queue]+N)%(_pNic)->TxInfo.TxTotalDescNum[_Queue]]

#define HwGetHWDESCAdjustedBuffer(Buf) \
           ( (PUCHAR)(Buf) + RTL8187_HWDESC_HEADER_LEN )

PVOID
HwNormalQGetNextToSendBuffer(
    __in  PNIC       Nic
    );

PVOID
HwHighQGetNextToSendBuffer(
    __in  PNIC       Nic
    );

#define HwIsAddr1Match(_TxBuf,_Addr)            \
    (BOOLEAN)(memcmp((PUCHAR)(_TxBuf)+4, _Addr, 6)==0)

#define HwIsBroadcast(_TxBuf)                                       \
    ((BOOLEAN)(                                                 \
                    ((((UCHAR *)(_TxBuf))[4])==(UCHAR)0xff) &&      \
                    ((((UCHAR *)(_TxBuf))[5])==(UCHAR)0xff) &&      \
                    ((((UCHAR *)(_TxBuf))[6])==(UCHAR)0xff) &&      \
                    ((((UCHAR *)(_TxBuf))[7])==(UCHAR)0xff) &&      \
                    ((((UCHAR *)(_TxBuf))[8])==(UCHAR)0xff) &&      \
                    ((((UCHAR *)(_TxBuf))[9])==(UCHAR)0xff)         \
                                                                ))

#define HwIsMulticast(_TxBuf)                   \
    ((BOOLEAN)(((_TxBuf)[4]&0x01)!=0))


typedef struct _TIMER_NIC_CONTEXT {
    PNIC        Nic;
}   TIMER_NIC_CONTEXT, *PTIMER_NIC_CONTEXT;

//
// Specify an accessor method for the WMI_SAMPLE_DEVICE_DATA structure.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(TIMER_NIC_CONTEXT, GetTimerContext)

typedef struct _WORKITEM_CONTEXT {
    PNIC        Nic;
}   WORKITEM_CONTEXT, *PWORKITEM_CONTEXT;

//
// Specify an accessor method for the WMI_SAMPLE_DEVICE_DATA structure.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WORKITEM_CONTEXT, GetWorkItemContext)



void 
HwInitializeVariable(
    __in PNIC     Nic
    );

void 
HwResetVariable(
    __in PNIC     Nic
    );

VOID
HwSetHardwareID(
    __in PNIC     Nic
    );

NDIS_STATUS 
HwSetNICAddress(
    __in  PNIC Nic
    );

BOOLEAN
HwSetRFState(
    __in  PNIC                Nic,
    __in  RT_RF_POWER_STATE   RFPowerState
    );

//
// Called at passive
//
VOID
HwSetRadioState(
    __in  PNIC    Nic,
    __in  BOOLEAN bPowerOn
    );


USHORT 
HwGenerateBeacon(
    __in PNIC        Nic,
    __in PUCHAR      BeaconBuf,
    __in BOOLEAN     APModeFlag
    );

void 
HwSetupBeacon(
    __in  PNIC        Nic,
    __in  PUCHAR      BeaconBuf,
    __in  USHORT      BeaconBufLen,
    __in  ULONG       BeaconBufPaLow,
    __in  USHORT      Rate
    );

UCHAR
HwMRateToHwRate(
    __in  UCHAR   Rate
    );

UCHAR
HwHwRateToMRate(
    __in  UCHAR   Rate
    );

ULONG64
HwDataRateToLinkSpeed(
    UCHAR  Rate
    );

NDIS_STATUS
HwAllocateAlignPhyMemory(
    PNIC                    Nic,
    ULONG                   Length,
    PUCHAR                  *VirtualAddr,
    NDIS_PHYSICAL_ADDRESS   *PhysicalAddr,
    PUCHAR                  *AlignVirtualAddr,
    NDIS_PHYSICAL_ADDRESS   *AlignPhysicalAddr
    );

void
HwLinkTxDesc(
    PUCHAR                  VirtualAddr,
    NDIS_PHYSICAL_ADDRESS   PhysicalAddr,
    USHORT                  DescSize,
    USHORT                  DescNum
    );

ULONG
HwGetTxBusyDescNum(
    __in  PNIC                    Nic,
    __in  int                     QueueIndex
    );

ULONG
HwGetTxFreeDescNum(
    __in  PNIC                    Nic,
    __in  int                     QueueIndex
    );

void 
HwTransmitNextPacketWithLocalBuffer(
    __in PNIC     Nic,
    __in UCHAR    QueueIndex,
    __in USHORT   PktSize,
    __in USHORT   Rate,
    __in BOOLEAN  bAIDContained,
    __in PUCHAR   Desc
    );

void
HwOnProbe(
    __in PNIC     Nic,
    __in PUCHAR   ProbePktBuf,
    __in USHORT   ProbePktLen,
    __in USHORT   ProbePktRate
    );

void
HwSendProbe(
    __in PNIC                 Nic,
    __in PDOT11_MAC_ADDRESS   BSSID,
    __in PDOT11_SSID          SSID,
    __in PUCHAR               ScanAppendIEByteArray,
    __in USHORT               ScanAppendIEByteArrayLength
    );

USHORT
HwGetHighestRate(
    __in PDOT11_RATE_SET  RateSet
    );

LONG
HwGetPositionInBuffer(
    __in  ULONG   PriorLength,
    __in  ULONG   BufferLength,
    __in  ULONG   TotalOffset
    );

ULONG
HwGetPhysicalAddressFromScatterGatherList(
    __in SCATTER_GATHER_LIST  *FragList,
    __in ULONG                Offset
    );

//
// Hardware receive related function
//

VOID
HwReturnFragment(
    __in PNIC             Nic,
    __in PNIC_RX_FRAGMENT NicFragment
    );

VOID
HwResetReceive(
    __in  PNIC    Nic
    );

BOOLEAN
HwFragmentIsDuplicate(
                     __in  PNIC              Nic,
                     __in  PNIC_RX_FRAGMENT  NicFragment
                     );

void
HwHandleTimingCriticalPacket(
    __in PNIC         Nic
    );


VOID
HwResponseToPacket(
    __in PNIC             Nic,
    __in PNIC_RX_FRAGMENT NicFragment
    );

BOOLEAN
HwFilterPacket(
    __in PNIC             Nic,
    __in PNIC_RX_FRAGMENT NicFragment
    );

EVT_WDF_TIMER HwScanCallback;

EVT_WDF_TIMER HwAwakeCallback;

EVT_WDF_WORKITEM HwSwChnlWorkItem;

VOID
HwSwChnlWorker(
    PNIC    Nic 
    );

EVT_WDF_TIMER HwJoinTimeoutCallback;

EVT_WDF_TIMER HwPeriodTimerCallback;

NDIS_STATUS
HwUpdateInfoFromBSSInformation(
    __in PNIC             Nic,
    __in PSTA_BSS_ENTRY   BSSEntry
    );

NDIS_STATUS
HwSetPacketFilter(
    PNIC    Nic,
    ULONG   PacketFilter
    );
    
BOOLEAN
HwAddIE(
    __in PNIC     Nic,
    __in UCHAR    ID,
    __in UCHAR    Size,
    __in PUCHAR   Data
    );

VOID
HwRemoveIE(
    __in PNIC     Nic,
    __in UCHAR    ID,
    __in PUCHAR   Data
    );

PUCHAR
HwGetIE(
    __in PNIC     Nic,
    __in UCHAR    ID,
    __in UCHAR    Index,
    __out PUCHAR  Size
    );

VOID
HwClearAllIE(
    __in PNIC     Nic
    );

BOOLEAN
HwSendNullPkt(
    __in PNIC     Nic
    );          

BOOLEAN
HwSendPSPoll(
    __in PNIC     Nic
    );

ULONG
HwComputeCrc(
    __in PUCHAR   Buffer,
    __in UINT     Length
    );

VOID
HwGetMulticastBit(
    __in UCHAR    Address[ETH_LENGTH_OF_ADDRESS],
    __out UCHAR * Byte,
    __out UCHAR * Value
    );

NDIS_STATUS
HwSetMulticastMask(
    __in PNIC     Nic,
    __in BOOLEAN  bAcceptAll
    );

VOID
HwDoze(
    __in PNIC     Nic,
    __in ULONG    ulDozeTime
    );

BOOLEAN
HwAwake(
    __in PNIC     Nic
    );

void
HwRemoveKeyEntry(
    __in PNIC Nic,
    __in UCHAR Index,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId
    );

void
HwAddKeyEntry(
    __in PNIC Nic,
    __in PNICKEY NicKey,
    __in UCHAR Index);

void
HwDeleteAllKeys(
    __in  PNIC Nic
    );

NDIS_STATUS
HwUpdateInfoFromdot11BSSDescription(
    __in PNIC     Nic,
    __in PDOT11_BSS_DESCRIPTION   Dot11BSSDescription
    );

NDIS_STATUS
HwGetInfoIEWithOUI(
    __in PUCHAR pucInfoBlob,
    __in ULONG uSizeOfBlob,
    __in UCHAR ucInfoId,
    __out PUCHAR pucLength,
    __deref_out_opt PVOID * ppvInfoEle,
    __inout PULONG pOffset
    );

UCHAR
HwTxRetryLimit(
    __in  PNIC    Nic,
    __in  USHORT  DataRate    
    );

USHORT
GetSequenceNumber(
    __in PNIC        Nic
    );

VOID
HwUpdateTxRetryStat(
    __in PNIC                 pNic,
    __in PMP_TX_MSDU          pMpTxd,
    __in USHORT               retryCount
    );

VOID
HwUpdateTxDataRate(
    __in PNIC                     pNic
    );



