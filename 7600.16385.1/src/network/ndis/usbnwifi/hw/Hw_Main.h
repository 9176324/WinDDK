/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Hw_Main.h

Abstract:
    HW layer defines
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _NATIVE_WIFI_HW_MAIN_H
#define _NATIVE_WIFI_HW_MAIN_H

#include <wdf.h>
#include <wdfworkitem.h>
#include <Wdfsync.h>
#include "r8180equ.h"
#include "8185type.h"
#include "8187_reg.h"

#define HW11_MEMORY_TAG                     'HltR'

#define LOW_QUEUE                           0
#define NORMAL_QUEUE                        1
#define HIGH_QUEUE                          2
#define BEACON_QUEUE                        3
#define TX_QUEUE_NUM                        4

#define TOTAL_DESC_LOW_QUEUE                HW11_MAX_TX_MSDUS
#define TOTAL_DESC_NORMAL_QUEUE             20  
#define TOTAL_DESC_HIGH_QUEUE               4
#define TOTAL_DESC_BEACON_QUEUE             4


#define MAX_NUM_PHY_TYPES                    5
#define MAX_NUM_DOT11_REG_DOMAINS_VALUE      10
#define MAX_NUM_DIVERSITY_SELECTION_RX_LIST  256

#define MAX_MPDU_LENGTH                      2346

#define MAX_IE_NUMBER                        10      // Max value is 128 (one bit reserved)

#define KEY_TABLE_SIZE                       16
#define KEY_MAPPING_KEY_TABLE_SIZE           (KEY_TABLE_SIZE - DOT11_MAX_NUM_DEFAULT_KEY)
#define PER_STA_KEY_TABLE_SIZE               KEY_MAPPING_KEY_TABLE_SIZE

#define DUPLICATE_DETECTION_CACHE_LENGTH     8

// 
// If link quality of the AP we are associated with is below this value,
// we will use the lower data rate for communicating with this AP
//
#define LOW_RATE_LINK_QUALITY_THRESHOLD     40 
//
// The lower data rate we would select would be some value below this
// max
//
#define LOW_RATE_MAX_DATA_RATE              24

 
#define NIC_NUM_REG_PARAMS              (((ULONG)sizeof (NICRegTable)) / ((ULONG)sizeof(NIC_REG_ENTRY)))

#define MAX_FRAGMENT_COUNT                  8   // Max number of MPDU per MSDU

#define MAX_PER_PACKET_VIRTUAL_BUF_NUM      24
#define MAX_PER_PACKET_PHYSICAL_BUF_NUM     MAX_PER_PACKET_VIRTUAL_BUF_NUM
#define MAX_PER_PACKET_BUFFER_LIST_LENGTH   MAX_PER_PACKET_PHYSICAL_BUF_NUM+MAX_FRAGMENT_COUNT+3        // +3 is for Header, LLC and trailer, MAX_FRAGMENT_COUNT for max fragment overhead

#define RT_MAX_LD_SLOT_NUM                  10

#define MAX_JOIN_RETRY_COUNT                5

#define CRC_LENGTH                          4

// Since 8187 inherited 8185, this macro is used to perform extra setting on 8185.
#define IS_HARDWARE_TYPE_8185(_Nic) ((_Nic)->CardInfo.HardwareID==HW_ID_8185)
#define IS_HARDWARE_TYPE_8187(_Nic) ((_Nic)->CardInfo.HardwareID==HW_ID_8187)

                          
#define UpdatePortUchar(pNic, Address, UpdateOperation)             \
{                                                                   \
    UCHAR   _tempReadVal_;                                          \
    _tempReadVal_ = HwPlatformEFIORead1Byte(pNic, Address);                   \
    HwPlatformEFIOWrite1Byte(pNic, Address, (_tempReadVal_ UpdateOperation));  \
}


#if NDIS60_MINIPORT

#define HW_DEFAULT_PROBE_DELAY                      0 
#define HW_DEFAULT_ACTIVE_SCAN_CHANNEL_PARK_TIME    60
#define HW_DEFAULT_PASSIVE_SCAN_CHANNEL_PARK_TIME   130

#define RT_MAX_LD_SLOT_NUM                  10

typedef struct _CHANNEL_LIST {
    ULONG   Channel[200];
    ULONG   Len;
} CHANNEL_LIST, *PCHANNEL_LIST;

//
// If we want to send multiple probes per channel, we can add an
// extra state e.g. ScanStatePerformScan_Pass2 and it will happen
//
typedef enum _SCAN_STATE
{                                                                                 
    ScanStateSwChnl = 0,
    ScanStatePerformScan_Pass1,
    ScanStateMaxState
} SCAN_STATE;

#else

typedef struct  _CHANNEL_LIST {
    UCHAR   Channel[200];
    UCHAR   Len;
} CHANNEL_LIST, *PCHANNEL_LIST;

typedef enum _SCAN_STATE
{
    ScanStateSwChnl = 0,
    ScanStateMaxState = 4,
} SCAN_STATE;

#endif

EXTERN CHANNEL_LIST ChannelListAB[];
EXTERN CHANNEL_LIST ChannelListB[];

typedef NDIS_STATUS
(*HW_RECEIVE_PROCESSING_FUNCTION)(
    __in PNIC             pNic,
    __in PNIC_RX_FRAGMENT pNicFragment
    );


#define DESC85_RATE1M           0x00
#define DESC85_RATE2M           0x01
#define DESC85_RATE5_5M         0x02
#define DESC85_RATE11M          0x03
#define DESC85_RATE6M           0x04
#define DESC85_RATE9M           0x05
#define DESC85_RATE12M          0x06
#define DESC85_RATE18M          0x07
#define DESC85_RATE24M          0x08
#define DESC85_RATE36M          0x09
#define DESC85_RATE48M          0x0a
#define DESC85_RATE54M          0x0b
#define DESC85_RATE72M          0x0c

/*
typedef enum _RF_POWER_STATE {
    RF_ON,
    RF_SLEEP,
    RF_OFF,
    RF_SHUT_DOWN
} RF_POWER_STATE, *PRF_POWER_STATE;
*/

typedef enum _NIC_STATE {
    NIC_INIT_STATE,
    NIC_OP_STATE
} NIC_STATE, * PNIC_STATE;

typedef enum _STA_JOIN_STATE {
    STA_JOIN_STATE_IDLE,
    STA_JOIN_STATE_WAIT_BEACON,
    STA_JOIN_STATE_AUTHENTICATE,
    STA_JOIN_STATE_ASSOCIATE,
    STA_JOIN_STATE_SUCCESS
} STA_JOIN_STATE, *PSTA_JOIN_STATE;


typedef struct _IE {
    BOOLEAN     bUse;
    UCHAR       ID;
    ULONG       OUI;
    UCHAR       Size;
    UCHAR       Data[256];
} IE, *pIE;

typedef struct _NIC_REG_ENTRY { //registry
    NDIS_STRING     RegName;        // variable name text
    BOOLEAN         bRequired;      // 1 -> required, 0 -> optional
    UCHAR           Type;           // NdisParameterInteger/NdisParameterHexInteger/NdisParameterStringle/NdisParameterMultiString
    UINT            StructOffset;       // offset of struct to MP_ADAPTER field
    UINT            FieldOffset;        // offset to structure
    UINT            FieldSize;      // size (in bytes) of the field
    UINT            Default;            // default value to use
    UINT            Min;                // minimum value allowed
    UINT            Max;            // maximum value allowed
} NIC_REG_ENTRY, *PNIC_REG_ENTRY;


typedef struct _DUPE_CACHE_ENTRY {
    USHORT          SequenceControl;
    DOT11_MAC_ADDRESS   Address2;
} DUPE_CACHE_ENTRY, *PDUPE_CACHE_ENTRY;

typedef
VOID
(*HW_CHANNEL_SWITCH_COMPLETE_HANDLER)(
    __in  PNIC    pNic
    );
typedef enum _LED_STATE_8187{
    LED_ON = 1,
    LED_OFF = 2,
    LED_BLINK_NORMAL = 3,
    LED_BLINK_SLOWLY = 4,
    LED_POWER_ON_BLINK = 5,
}LED_STATE_8187;

typedef enum _LED_CTL_MODE_8187{
    LED_CTL_POWER_ON = 1,
    LED_CTL_LINK = 2,
    LED_CTL_NO_LINK = 3,
    LED_CTL_TRX = 4,
    LED_CTL_SITE_SURVEY = 5,
    LED_CTL_POWER_OFF = 6
}LED_CTL_MODE_8187;

typedef union _TX_POWER_BASE_OFFSET
{
    struct _TX_POWER_BASE_OFFSET_FIELD
    {
        UCHAR  Cck:4; // Offset of CCK Tx index: 0 is default, e.g. CckTxPwrIdxBaseOffset=m, TxPowerIndex=s =>  CCK Tx power index = m+s.
        UCHAR  Ofdm:4; // Offset of OFDM Tx index: 0 is default, e.g. OfdmTxPwrIdxBaseOffset=n, TxPowerIndex=t =>  OFDM Tx power index = n+t+10.
    }field;

    UCHAR u1bData;
} TX_POWER_BASE_OFFSET, *PTX_POWER_BASE_OFFSET;

typedef enum _INTERFACE_SELECT_8187{
    INTF_SEL0_USB           = 0,        // USB
    INTF_SEL1_MINICARD      = 1,        // Minicard
    INTF_SEL2_RSV           = 2,        // Reserved.
    INTF_SEL3_RSV           = 3,        // Reserved.
} INTERFACE_SELECT_8187, *PINTERFACE_SELECT_8187;

typedef struct _RX_SEQUENCE{
    unsigned short  FragmentNum:4;
    unsigned short  SequenceNum:12;
} RX_SEQUENCE, *PRX_SEQUENCE;

typedef enum _HARDWARE_ID{ 
    HW_ID_8180,
    HW_ID_8185,
    HW_ID_8187
} HARDWARE_ID, *PHARDWARE_ID;

typedef  enum _RF_TYPE_8187{
    RF_TYPE_MIN,
    RF_MAXIM2825 = 4,
    RF_ZEBRA = 5,
    RF_ZEBRA2,      
    RF_TYPE_MAX,
}RF_TYPE_8187,*PRF_TYPE_8187;

typedef enum _VERSION_ID_8187{
    VERSION_8187_B,
    VERSION_8187_D,
}VERSION_ID_8187,*PVERSION_ID_8187;

typedef enum _INITGAIN_STATE_8225{
    autoDIG     = 0,
    m821dBm = 1,
    m822dBm = 2,
    m823dBm = 3,
    m78dBm  = 4,
    m74dBm  = 5,
    m70dBm  = 6,
    m66dBm  = 7,
}INITGAIN_STATE_8225,*PINITGAIN_STATE_8225;


typedef struct _NIC_LED_INFO {
    //2LED control
    UCHAR                  LedCurrentState;
    UCHAR                  bLedBlinkInProgress;
    UCHAR                  BlinkTimes;
    UCHAR                  BlinkingLedState;
    LED_STATE_8187         Ledstate;
    WDFTIMER               LedControlTimer;
    // Workitem for Led. 
    WDFWORKITEM             FlashLedWorkItem;
    NDIS_EVENT              FlashLedEvent;
    UCHAR                   FlashLedEventCount;
     //3 INF Parameters
    UCHAR                   LedCtrl;


}NIC_LED_INFO, *PNIC_LED_INFO;


typedef enum    _AUTH_STATUS{
    AUTH_STATUS_IDLE,           //initialization state
    AUTH_STATUS_IN_PROGRESS,    //authentication packet exchange in progress
    AUTH_STATUS_FAILED,         //last authentication failed
    AUTH_STATUS_SUCCESSFUL,     //last authentication successed
    AUTH_STATUS_METHOD_NOT_MATCH,//last authentication authenation did not started because of method does not match(open vs shared)
}AUTH_STATUS;

typedef enum{
    MLMESTARTREQ_NONE  = 0,
    MLMESTARTREQ_INIT  = 1,
    MLMESTARTREQ_NWTYPE_CHANGE  = 2,
    MLMESTARTREQ_AUTH_CHANGE  = 3,
    MLMESTARTREQ_KEY_CHANGE  = 4,
    MLMESTARTREQ_PRIVACY_CHANGE  = 5,
}RT_MLMESTARTREQ_RSN;

//----------------------------------------------------------------------------
//      802.11 Preamble Mode
//----------------------------------------------------------------------------
typedef enum    _REG_PREAMBLE_MODE{
    PREAMBLE_LONG   = 1,
    PREAMBLE_AUTO   = 2,
    PREAMBLE_SHORT  = 3,
}REG_PREAMBLE_MODE;


typedef enum{
    ROAMINGSTATE_IDLE  = 0,
    ROAMINGSTATE_SCANNING  = 1,
    ROAMINGSTATE_AUTHENTICATION  = 2,
    ROAMINGSTATE_REASSOCIATION  = 3,
}RT_ROAMING_STATE;


typedef struct _NIC_TX_MSDU {
    // Added as dummy for now
    PMP_TX_MSDU                 pMpTxd;
    PUCHAR                      CoalesceBuf;
   
    BOOLEAN                     WaitSendToComplete; // Mark this Tx MSDU as waiting for send completion    
    BOOLEAN                     bFailedDuringSend;
    BOOLEAN                     bMulticastDestAddr;
    BOOLEAN                     RTSEnabled;
    BOOLEAN                     CTSEnabled;
#if NDIS60_MINIPORT
    BOOLEAN                     TxSucceeded;
    ULONG                       TotalMSDULength;
#endif  // NDIS60_MINIPORT
    USHORT                      QueueIDUsed;
    ULONG                       FirstDescUsed;
    ULONG                       TotalDescUsed;
    ULONG                       SucceedFragCount;
    LIST_ENTRY                  WaitLink ;
    
} NIC_TX_MSDU, *PNIC_TX_MSDU;

typedef struct _NIC_RX_MSDU
{
    LIST_ENTRY              Link;
    ULONG                   TotalPacketSize;
    ULONG                   NumFragments;
    PNIC_RX_FRAGMENT        Fragments[16];
} NIC_RX_MSDU, *PNIC_RX_MSDU;



// RF state.
typedef enum _RT_RF_POWER_STATE
{
    eRfOn,
    eRfSleep,
    eRfOff,
    eRfShutdown
}RT_RF_POWER_STATE;


typedef struct _NIC_RX_FRAGMENT{
    LIST_ENTRY              FragmentList;
    USHORT                  PacketLength;       // Total packet length: Must equal to sum of all FragLength
    USHORT                  DataSize;         // DataSize should equal to PacketLength in non-fragment case
    USHORT                  DataOffset;         // Data offset for this fragment
    RX_STATUS_DESC_8187     RxDesc;
    PVOID                   DriverReserved;
    struct _RT_RFD          *NextRfd; //PNIC_RX_FRAGMENT
    PMP_RX_FRAGMENT         MpFragment;   
    BOOLEAN                 Encrypted;  
    UCHAR                   Channel;        // channel number at which the frame was received, valid only for
                                        // beacon and probe response frames.
    PMDL                    Mdl;
    PETH_RX_BUFFER_STRUC    RxBuffer;
    PNICKEY                 NicKey;
    BOOLEAN                 Unicast;        // Not Multicast or broadcast
    ULONGLONG               FrameNumber;
    WDFMEMORY               BufferHdl;
}NIC_RX_FRAGMENT,*PNIC_RX_FRAGMENT;

typedef struct _VIRTUAL_MEMORY{
    PVOID               Ptr;
    ULONG               Length;
}VIRTUAL_MEMORY,*PVIRTUAL_MEMORY;


typedef struct _NICCARDINFO {
    UCHAR                   PermanentAddress[6];
    UCHAR                   CurrentAddress[6];
    UCHAR                   MacAddr[6];
    UCHAR                   StationID[6];   // Not for Tx/Rx
    BOOLEAN                 bOverrideAddress;

    ULONG                   IoBaseAddress;
    PVOID                   CSRAddress;
    PVOID                   PortOffset;
    ULONG                   IoRange;
    UCHAR                   MacVersionId;
    UCHAR                   RevisionID;
    USHORT                  SubVendorID;
    USHORT                  SubSystemID;

    BOOLEAN                 AutoloadFailFlag;
    BOOLEAN                 ResetInProgress;

    BOOLEAN                 SurpriseRemoved;
    BOOLEAN                 MWIEnable;

    HARDWARE_ID             HardwareID;   
    VERSION_ID_8187         VersionID;    
    BOOLEAN                 SoftwareSeq;
    UCHAR                   VerificationType;

    // contention window
    UCHAR                   RegPacketControlCW;

    // retry limit
    UCHAR                   RegPacketControlRL;

    // Tx AGC
    UCHAR                   RegPacketControlAGC;
    UCHAR                   RegAGC;
    UCHAR                   RegCCK_TxAGC;
    UCHAR                   RegOFDM_TxAGC;

    // Tx Antenna
    UCHAR                   RegPacketControlAntenna;
    UCHAR                   RegAntenna;
    UCHAR                   RegCCK_TxAntenna;
    UCHAR                   RegOFDM_TxAntenna;
    UCHAR                   RegFeedbackControlAntenna;
    // auto rate fallback control
    BOOLEAN                             bRegAutoRateFallback;
    UCHAR                               RegAutoRateFallbackStep;

    UCHAR                  EEPROMTxPowerLevelOf11gOFDM[14];    // OFDM Tx power level
    UCHAR                  TxPowerLevelOf11gOFDM[14];  // OFDM Tx power level

    WIRELESS_MODE           CurrentWirelessMode;    //For 8185
    UCHAR                   CurrentChannel;
    UCHAR                   SwChnlStage;
    UCHAR                   SwChnlStep;

    // 2004-04-07 for RTL8185: for modifying "initial gain" by the user
    UCHAR                   InitialGain;
    // Interface selection. 
    INTERFACE_SELECT_8187   InterfaceSel;  
    RT_RF_POWER_STATE       eRFPowerState;

    // Tx power base offset.
    TX_POWER_BASE_OFFSET    TxPowerBaseOffset;

    //2 Counter
    ULONG                  BBWriteFailcount;
   // BRSR_AR_STRUCT         BRSR_AR_Setting;

    //2004.10.29 Andy for Rate Adaptive
    UCHAR                   InitPSR;

    BOOLEAN                 USBIsHigh;
    // For Zebra's bug.
    BOOLEAN         bToUpdateRf0C;
    ULONG           ValueToUpdateRf0C;
    ULONG           ValueOfRf0C;
    int             SmallAgcCnt;
    int             LargeAgcCnt;
    //RT_BSS_SIG        BssSigList[MAX_NUM_BSS_SIG];
    int             nBssSigCnt;

    // "CornerChange"
    UCHAR               RegCornerChange;
    INITGAIN_STATE_8225     InitGainState;
    // "EvmCheck": for EVM check. 
    int                     RegEvmCheck;
    UCHAR           BQREQInitValue;
} NICCARDINFO, *PNICCARDINFO;



typedef struct _NICRXINFO {
    // Multicast Address List
    UINT                    MCAddressCount;
    UCHAR                   MCAddressList[HW11_MAX_MCAST_LIST_SIZE][DOT11_ADDRESS_SIZE];
    UCHAR                   MCRegValues[8];
    ULONG                   ReceiveConfig;                  // This filter is for a Hw filter

    DUPE_CACHE_ENTRY        DupePacketCache[2 * DUPLICATE_DETECTION_CACHE_LENGTH];
    UCHAR                   NextDupeCacheIndexData;         // Next index to put entry in for data packets
    UCHAR                   NextDupeCacheIndexOther;        // Next index to put entry in for other packets

    UCHAR                   NextDupeCacheIndex;             // Next index to put entry in

    USHORT                  RxTotalDescNum;                 // Hw support Desc max_number
    USHORT                  RxAvailableDescNum;             // Num HW_DESC free for hardware to use
    USHORT                  RxNextDescToReturn;             // Hw Desc current using id_number
    USHORT                  RxNextDescToReceive;            // The next desc hardware will fill in

    LIST_ENTRY              RxFragList;
    USHORT                  RxFragListLen;                   //used for normal io
    LIST_ENTRY              RxUnusedFragList;               // List of fragments not currently in use
                                                        //looks like used for overflow
                                                               //available Frag list
    LIST_ENTRY              PendingList;

    NPAGED_LOOKASIDE_LIST   RecvFRAGMENTLookaside;          // lookaside lists for NIC_RX_FRAGMENT

    HW_RECEIVE_PROCESSING_FUNCTION      ReceiveProcessingFunction;
    ULONG                   ReceiveFilter;                  // This filter is got from OID
    NDIS_SPIN_LOCK          ReceiveFilterLock;              // Lock to protect setting of this filter
    WDFSPINLOCK             RecvLock;
    NDIS_SPIN_LOCK          ListLock;    
    NDIS_SPIN_LOCK          PendingListLock;
    
    USHORT                  NumRxDesc;
    USHORT                  NextRxDescToFill;
    USHORT                  NextRxDescToCheck;
                                                                                            
} NICRXINFO, *PNICRXINFO;

typedef struct _NICTXINFO {
    PUCHAR                  TxDescVa[TX_QUEUE_NUM];     // Desc virtual address without alignment

    PTX_DESC_8187           TxDesc[TX_QUEUE_NUM];           // Desc virtual address with alignment

    USHORT                  TxTotalDescNum[TX_QUEUE_NUM];   //==>numTxDesc

    LONG                    TxNextDescToSend[TX_QUEUE_NUM];    //==>nextTxDescToFill
    LONG                    TxNextDescToCheck[TX_QUEUE_NUM];   //==>nextDescToCheck
    // TODO: Test the CheckForHang hypothesis
    LONG                    TxBusyDescCount[TX_QUEUE_NUM];

    PUCHAR                  *pTxNormalQBufVa;               // Pointer array for Normal queue buffer
    PUCHAR                  *pTxHighQBufVa;                 // Pointer array for High queue buffer
    PUCHAR                  TxBeaconBufVa[TOTAL_DESC_BEACON_QUEUE];               // Beacon queue buffer
    ULONG                   TxLowQueueReservedCount;

    ULONG                   TransmitConfig;
    UCHAR                   CwValues;       // For current CW setting. 
    UCHAR                   RegCWinMin;     // For turbo mode CW adaptive. 
    UCHAR                   RegCWinMax;
    
    NDIS_SPIN_LOCK          TxSpinLock;    
    //
    // MSDU that we have been SGed, but we are not able to send
    // due to lack of enough descriptors would be temporarily queued
    // here. The MP portion is lead to believe that these packets
    // actually have been submitted to hardware    
    LIST_ENTRY              WaitMSDUList; // use this as the wait q till resources are available

    NDIS_SPIN_LOCK          WaitListLock;

    LONG                    WaitMSDUListLength;   // Incremented after adding WaitMSDUList

    LONG                    Reserved;
    UCHAR                   TxPowerLevelPercent;    //change the powerlevel of Tx

    ULONG                   MinPacketsSentForTxRateUpdate;
    ULONG                   TxFailureThresholdForRateFallback;
    ULONG                   TxFailureThresholdForRateIncrease;
    ULONG                   TxFailureThresholdForRoam;
    ULONG                   TxDataRateFallbackSkipLevel;
} NICTXINFO, *PNICTXINFO;



typedef struct _NICEEPROMINFO {
    UCHAR                   EepromAddressSize;
    USHORT                  EEPROMVersion;
    BOOLEAN                 AutoloadFailFlag;
    UCHAR                   EEPROMAntennaDiversity;
    UCHAR                   EEPROMDefaultAntennaB;
    USHORT                  EEPROMChannelPlan;
    RF_TYPE_8187            EEPROMRFChipID;
} NICEEPROMINFO, *PNICEEPROMINFO;

typedef struct _NICRFINFO {
    // RF chip information
    RF_TYPE_8187            RFChipID;
    UCHAR                   RFProgType;
    BOOLEAN                 DigitalPhy;
    USHORT                  ChannelPlan;

    // RF power state
    BOOLEAN                 RadioOff;                       // True if we want long-term Radio off 
    RT_RF_POWER_STATE       RFPowerState;                   // Current Radio state
    LONG                    AccessRefCount;
    BOOLEAN                 RadioStateChangeInProgress;

    // Tx Power control
    UCHAR                   TxPowerLevel[200];  // Zero-based channel index
    UCHAR                   TxPowerLevelOFDM[200];

    UCHAR                   EEPROMTxPowerLevel[200];    // Zero-based channel index
    UCHAR                   EEPROMTxPowerLevelOFDM[200];

    // Reg Domain
    USHORT                  RegDomain;

    // Carrier Sense
    USHORT                  CSThreshold;
    ULONG                   CSMethod;

    // Antenna Settings
    BOOLEAN                 AntennaDiversity;
    BOOLEAN                 DefaultAntennaB;
    UCHAR                  GEPRFOffState;
    ULONG                   CurrentTxAntenna;
    ULONG                   CurrentRxAntenna;

    BOOLEAN                 bThreeWireMode;
    UCHAR                   EnergyThreshold;
    UCHAR                   InterfaceDebugMode;
    UCHAR                   EnableGPIO0;
    ULONG                   VersionID;
    BOOLEAN                 b2ndCut;
    UCHAR                   RegZebraVersion;
    UCHAR                   ChargePump;
    UCHAR                   TXChargePump;
    UCHAR                   RXChargePump;
    UCHAR                   CornerChange;
    UCHAR                   TXAntenna;
    UCHAR                   RXAntenna;
    UCHAR                   TXOFDMPower;
    UCHAR                   TXCCKPower;
    UCHAR                   FastFalseAlarm;
    UCHAR                   CCKFastFalseAlarm;
    UCHAR                   EVMCheck;
    UCHAR                   SamplingPhase;
    UCHAR                   AGCmode;
    BOOLEAN                 bDig;
    UCHAR                   DIG_CurrentInitialGain;
    //save_info_before_scan
    UCHAR                   BForScan_DIG_InitialGain;   
    UCHAR                   BForScan_EnergyThreshold;

} NICRFINFO, *PNICRFINFO;

typedef struct _NICSTATISTICSINFO {

    ULONGLONG               RxNoBuf;    // RX no buffer error
    
    DOT11_MAC_FRAME_STATISTICS  UcastCounters;
    DOT11_MAC_FRAME_STATISTICS  McastCounters;
    DOT11_PHY_FRAME_STATISTICS  PhyCounters[MAX_NUM_PHY_TYPES];

} NICSTATISTICSINFO, *PNICSTATISTICSINFO;



typedef struct _NICDUPLICATEINFO {
    USHORT                  EarlyRxThreshold;
    USHORT                  DefaultBeaconRate;  // Default beacon rate for b/g mode
    ULONG                   RTS_Threshold;
    ULONG                   Frag_Threshold;
    ULONG                   ShortRetryLimit;
    ULONG                   LongRetryLimit;
    ULONG                   MaxTxLifeTime;
    ULONG                   MaxRxLifeTime;
    ULONG                   MediaOccupancyLimit;
    ULONG                   CFPPeriod;
    ULONG                   CFPMaxDuration;
    ULONG                   BeaconInterval;
    ULONG                   JoinFailureTimeout;
} NICDUPLICATEINFO, *PNICDUPLICATE;

typedef struct _NICKEY
{
    BOOLEAN                     Persistent;
    BOOLEAN                     Valid;
    DOT11_MAC_ADDRESS           MacAddr;
    DOT11_CIPHER_ALGORITHM      AlgoId;
    UCHAR                       KeyLength;              // length of KeyValue in bytes
    UCHAR                       KeyValue[16];           // 128 bits
    UCHAR                       TxMICKey[8];            // for TKIP only
    UCHAR                       RxMICKey[8];            // for TKIP only
    union {
        struct {
            ULONGLONG           PN:48;                  // for CCMP
            ULONGLONG           PN_unused:16;
        };
        struct {
            ULONGLONG           TSC:48;                 // for TKIP
            ULONGLONG           TSC_unused:16;
        };
        struct {
            ULONG               IV:24;                  // for WEP 
            ULONG               IV_unused:8;
        };
    };
    struct {
        ULONGLONG               ReplayCounter:48;       // for CCMP or TKIP
        ULONGLONG               ReplayCounter_unused:16;
    };
} NICKEY, *PNICKEY;

typedef struct _NIC_PER_STA_KEY
{
    DOT11_MAC_ADDRESS           StaMacAddr;
    BOOLEAN                     Valid;
    NICKEY                      NicKey[DOT11_MAX_NUM_DEFAULT_KEY];
} NIC_PER_STA_KEY, *PNIC_PER_STA_KEY;


//
// PHY specific MIB. The MIB could be different for different vendors.
//
typedef struct _NICPHYMIB       
{
    DOT11_PHY_TYPE                      PhyType;
    ULONG                               PhyID;
    DOT11_RATE_SET                      OperationalRateSet;
    DOT11_RATE_SET                      BasicRateSet;
    DOT11_RATE_SET                      ActiveRateSet;
    UCHAR                               Channel;
    DOT11_SUPPORTED_DATA_RATES_VALUE_V2 SupportedDataRatesValue;
} NICPHYMIB, *PNICPHYMIB;

//
// Encryption info structure
//
typedef struct _NIC_ENCRYPTION_INFO
{
    UCHAR       RetreatedSize;
    BOOLEAN     MICMdlAdded;
    PVOID       MICData;
    ULONG       LastMdlByteCount;
    PMDL        MICMdl;
} NIC_ENCRYPTION_INFO, *PNIC_ENCRYPTION_INFO;

typedef struct _CHANNEL_ACCESS_SETTING
{
    UCHAR   SIFS_Timer;
    UCHAR   DIFS_Timer;
    UCHAR   SlotTimeTimer;
    UCHAR   EIFS_Timer;
    UCHAR   CWminIndex;
    UCHAR   CWmaxIndex;
} CHANNEL_ACCESS_SETTING, *PCHANNEL_ACCESS_SETTING;


typedef struct _PRIV_8185
{
    CHANNEL_ACCESS_SETTING  ChannelAccessSetting;
    UCHAR                      MaxResponseRate;
    UCHAR                      MinResponseRate;

}PRIV_8185, *PPRIV_8185;

typedef struct _NICMANAGEMENTINFO
{
    // switch channel related variable
    BOOLEAN                             SwChnlInProgress;       // indicate if switch channel is in progress
    UCHAR                               DestChannel;            // destination channel in channel switch
    HW_CHANNEL_SWITCH_COMPLETE_HANDLER  ChannelSwitchedHandler;
    WDFWORKITEM                         SwChnlWorkItem;
    UCHAR                               SwChnlStage;
    UCHAR                               SwChnlStep;    

    // Scan related variable
    WDFTIMER                            Timer_Scan;
    BOOLEAN                             bScanInProgress;
    BOOLEAN                             ScanCancelOperation;          // indicates if scan should be stopped
#if     NDIS60_MINIPORT
    PDOT11_SCAN_REQUEST_V2              ScanRequest;
    ULONG                               ScanProbeDelay;
    ULONG                               ScanChannelCount;
    PULONG                              ScanChannelList;
    ULONG                               ScanChannelTime;
    ULONG                               ScanCurrentChannelIndex;
    SCAN_STATE                          ScanCurrentState;
    ULONGLONG                           ScanChannelSwitchTime;
    UCHAR                               ScanCurrentChannel;
#else
    DOT11_MAC_ADDRESS                   ScanBssid;              // Scan Bssid
    DOT11_SSID                          ScanSsid;               // Scan SSID
    DOT11_SCAN_TYPE                     ScanType;               // Scan type
    UCHAR                               ScanAppendIEByteArray[256]; // IE byte array that should be appended at the end of Probe Request
    USHORT                              ScanAppendIEByteArrayLength;    // IE byte array length
    USHORT                              ScanState;              // State of scan
#endif  // NDIS60_MINIPORT

    // 802.11 Power management
    DOT11_POWER_MGMT_MODE               PowerMgmtMode;
    WDFTIMER                            Timer_Awake;
    WDFTIMER                            Timer_PeriodicCallback;
    PTX_DESC_8187                       pPowerMgmtChangePktDesc;
    BOOLEAN                             bPowerMgmtChangePktSent;
    BOOLEAN                             bATIMPacketReceived;
    BOOLEAN                             bUnicastDataAvailable;
    ULONGLONG                           WakeupTime;
    NDIS_SPIN_LOCK                      PowerMgmtLock;
    BOOLEAN                             bSleepOnInterruptComplete;
    NDIS_DEVICE_POWER_STATE             NextDevicePowerState;

    UCHAR                               SSID[33];               // 1 byte for 0
    UCHAR                               BssId[6];
    UCHAR                               APJoinBSSID[6];         //  For AP join mode.

    USHORT                              CapInfo;
    USHORT                              AtimWindow;

    TIM                                 Tim;
    
    USHORT                              DtimCount;
    USHORT                              DtimPeriod;

    CHAR                                IEMap[256];             // Used to indicate if an IE is available(This array size should not be changed)
    IE                                  IEs[MAX_IE_NUMBER];     // Store all information elements
    USHORT                              WPAIECount;

    UCHAR                               NextBeaconIndex;

    WDFTIMER                            Timer_JoinTimeout;
    ULONG                               JoinWaitBeaconToSynchronize;

    ULONG                               CCAModeSupported;
    ULONG                               CurrentCCAMode;

    BOOLEAN                             bScanCompleteIndicated;
    BOOLEAN                             bAdvertiseBSS;

    ULONG                               uMPDUMaxLength;

    LONG                                EDThreshold;
    BOOLEAN                             ShortSlotTimeOptionEnabled;

    LARGE_INTEGER                       DelaySleepTimeStamp;
    ULONG                               DelaySleepValue;

    DOT11_OFFLOAD_CAPABILITY            OffloadCapability;
    DOT11_CURRENT_OFFLOAD_CAPABILITY    CurrentOffloadCapability;

    DOT11_OPERATION_MODE_CAPABILITY     OperationModeCapability;
    DOT11_CURRENT_OPERATION_MODE        CurrentOperationMode;
    DOT11_BSS_TYPE                      CurrentBSSType;

    DOT11_OPTIONAL_CAPABILITY           OptionalCapability;
    DOT11_CURRENT_OPTIONAL_CAPABILITY   CurrentOptionalCapability;

    USHORT                              CurrentBeaconRate;      // Used for Beacon and Probe request
    USHORT                              CurrentTXDataRate;
    USHORT                              CurrentTXMgmtRate;

    DOT11_DIVERSITY_SUPPORT             DiversitySupport;

    DOT11_SUPPORTED_POWER_LEVELS        SupportedPowerLevels;
    ULONG                               CurrentTxPowerLevel; // 1..8

    PDOT11_REG_DOMAINS_SUPPORT_VALUE    pRegDomainsSupportValue;

    PDOT11_DIVERSITY_SELECTION_RX_LIST  pDiversitySelectionRxList;
    BOOLEAN                             SafeModeEnabled;

    // For 8185
    UCHAR                               dot11CurrentWirelessMode;
    UCHAR                               Regdot11OperationalRateBuf[64];
    UCHAR                               RegWirelessMode;
    UCHAR                               RegPacketControlRL;
    UCHAR                               RegPacketControlAGC;
    UCHAR                               RegCCK_TxAGC;
    UCHAR                               RegOFDM_TxAGC;
    UCHAR                               RegPacketControlAntenna;
    UCHAR                               RegCCK_TxAntenna;
    UCHAR                               RegOFDM_TxAntenna;
    UCHAR                               RegFeedbackControlAntenna;

        // auto rate fallback control
    BOOLEAN                             bRegAutoRateFallback;
    UCHAR                               RegAutoRateFallbackStep;
        
     // encryption related info 
    NICKEY                              KeyTable[KEY_TABLE_SIZE];   // 0-3 is default key, 4+ is key mapping key 
    UCHAR                               DefaultKeyID;
    UCHAR                               KeyMappingKeyCount;
    NIC_PER_STA_KEY                     PerStaKey[PER_STA_KEY_TABLE_SIZE];
    UCHAR                               PerStaKeyCount;
    DOT11_AUTH_ALGORITHM                HwAuthAlgorithm;
    DOT11_CIPHER_ALGORITHM              HwUnicastCipher;
    DOT11_CIPHER_ALGORITHM              HwMulticastCipher;
    NPAGED_LOOKASIDE_LIST               EncryptionInfoList;
    NPAGED_LOOKASIDE_LIST               MICDataList;


    // PHY specific MIB
    PDOT11_SUPPORTED_PHY_TYPES          pSupportedPhyTypes;
    ULONG                               OperatingPhyId;         // ID of currently operating PHY
    ULONG                               SelectedPhyId;          // index of PHY that any PHY specific OID is applied to

    NDIS_SPIN_LOCK                      PhyMIBConfigLock;       // currently used when configuring ActiveRateSet in PhyMIB
    NICPHYMIB                           PhyMIB[MAX_NUM_PHY_TYPES];
    PNICPHYMIB                          OperatingPhyMIB;        // for currently operating PHY (by OperatingPhyId)
    PNICPHYMIB                          SelectedPhyMIB;         // for currently selected PHY (by SelectedPhyId)
    BOOLEAN                             RegRfOff;

    //default setting ==================
    OctetString         Regdot11OperationalRateSet;
    UCHAR               dot11CurrentChannelNumber;

    // For turbo mode. 
    // -----------------------------------------
    BOOLEAN             bInTurboMode;           // 8187 Turbo mode: Currently we are in turbo mode or not. (used when bSupportTurboMode is TRUE)
    PRIV_8185           Info8185;
    BOOLEAN             bSetKeyed;
    OctetString         SupportedRates;
    UCHAR               SupportedRatesBuf[128];
    USHORT              SequenceNumber;

    // current device's connection state
    BOOLEAN                             bDeviceConnected;
    
    /** Total data send retry count for Tx rate negotiation*/
    LONG                                TotalRetryCount;
    ULONG                               PacketsSentForTxRateCheck;
    UCHAR                               MinPacketsSentForTxRateUpdate;
    USHORT                              PrevTxDataRate;
    USHORT                              TxRateIncreaseWaitCount;
    USHORT                              TxRateIncreaseWaitRequired;

    BOOLEAN                             bEnableSendCTSToSelf;
} NICMANAGEMENTINFO, *PNICMANAGEMENTINFO;


typedef struct _NIC
{
    /** The handle to be passed to NDIS when calling functions */
    NDIS_HANDLE             MiniportAdapterHandle;
    WDFUSBDEVICE            UsbDevice;

    /** Pointer to the associated Adapter instance */
    PADAPTER                Adapter;
    /** Pointer to the associated Station instance */
    PSTATION                Station;
    //3 Card Information
    NICCARDINFO             CardInfo;
     //3 EEPROM Releated Variable
    NICEEPROMINFO           EEPROMInfo;
    //3 RF releated Variable
    NICRFINFO               RfInfo;

    //3 Rx releated Variable
    NICRXINFO               RxInfo;
    //3 Tx releated Variable
    NICTXINFO               TxInfo;
    
       //3 Statistics releatedVariable
    NICSTATISTICSINFO       StatisticsInfo;
    //3 Managementl releated Variable(switch channel,...)
    NICMANAGEMENTINFO       ManagementInfo;
    
    NIC_LED_INFO            LedInfo;
    NICDUPLICATEINFO        DupCurrentInfo;
    NICDUPLICATEINFO        DupRegistryInfo;
                                                                                                    
} NIC, *PNIC;


#define IS_WIRELESS_MODE_A(Nic)     ((Nic)->ManagementInfo.dot11CurrentWirelessMode == WIRELESS_MODE_A)
#define IS_WIRELESS_MODE_B(Nic)     ((Nic)->ManagementInfo.dot11CurrentWirelessMode == WIRELESS_MODE_B)
#define IS_WIRELESS_MODE_G(Nic)     ((Nic)->ManagementInfo.dot11CurrentWirelessMode == WIRELESS_MODE_G)
//
// Macros to make code more readable
//

#define HW_IS_IN_AP_MODE(_Nic)      \
    (FALSE)

#define HW_IS_IN_STA_INFRA_MODE(_Nic)     \
    (_Nic->ManagementInfo.CurrentOperationMode.uCurrentOpMode == DOT11_OPERATION_MODE_EXTENSIBLE_STATION && _Nic->ManagementInfo.CurrentBSSType == dot11_BSS_type_infrastructure)

#define HW_IS_IN_STA_ADHOC_MODE(_Nic)     \
    (_Nic->ManagementInfo.CurrentOperationMode.uCurrentOpMode == DOT11_OPERATION_MODE_EXTENSIBLE_STATION && _Nic->ManagementInfo.CurrentBSSType == dot11_BSS_type_independent)

#define HW_IS_IN_NETWORK_MONITOR_MODE(_Nic)     \
    (_Nic->ManagementInfo.CurrentOperationMode.uCurrentOpMode == DOT11_OPERATION_MODE_NETWORK_MONITOR)
    
#define HW_WAIT_FOR_BSS_JOIN(_Nic)     (_Nic->ManagementInfo.JoinWaitBeaconToSynchronize = 1)
#define HW_STOP_WAITING_FOR_JOIN(_Nic)      \
    (InterlockedCompareExchange((PLONG)&_Nic->ManagementInfo.JoinWaitBeaconToSynchronize, 0, 1) == 1)
    
#define HW_MDL_BYTECOUNT(_Mdl) ((_Mdl)->ByteCount)


//
// Some functions that are being called from multiple files
//

VOID
HwResetParameters(
    __in  PNIC                pNic,
    __in  DOT11_RESET_TYPE    ResetType
    );

VOID
HwCheckForATIMPacketSendCompletion(
    __in      PNIC    pNic
    );

void
HwSetChannel(
    __in  PNIC        pNic,
    __in  UCHAR       ucChannel,
    __in  HW_CHANNEL_SWITCH_COMPLETE_HANDLER      ChannelSwitchCompleteHandler
    );

void
HwSetChannelSync(
    __in  PNIC pNic,
    __in  UCHAR ucChannel
    );

NDIS_STATUS
HwProcessReceiveAPMode(
    __in PNIC             pNic,
    __in PNIC_RX_FRAGMENT pNicFragment
    );

NDIS_STATUS
HwProcessReceiveSTAMode(
    __in PNIC             pNic,
    __in PNIC_RX_FRAGMENT pNicFragment
    );

NDIS_STATUS
HwProcessReceiveAdhocMode(
    __in PNIC             pNic,
    __in PNIC_RX_FRAGMENT pNicFragment
    );


NDIS_STATUS
HwProcessReceiveAnyModeForIndicateAck(
    __in PNIC             pNic,
    __in PNIC_RX_FRAGMENT pNicFragment
    );

NDIS_STATUS
HwProcessReceiveSTAModeWaitForJoin(
    __in PNIC             pNic,
    __in PNIC_RX_FRAGMENT pNicFragment
    );

NDIS_STATUS
HwProcessReceiveAdHocSTAModeWaitForJoin(
    __in PNIC             pNic,
    __in PNIC_RX_FRAGMENT pNicFragment
    );

VOID
HwAdjustReceiveProcessingFunction(
    __in  PNIC        pNic
    );

void 
HwGetVersion(
    __in  PNIC        pNic
    );

void
HwSetBSSIDAndBcnIntv(
    __in  PNIC pNic
    );

NDIS_STATUS
HwCalculateMIC(
    __in  PNET_BUFFER     NetBuffer,
    __in  UCHAR           Priority,
    __in  PUCHAR          MICKey,
    __out PUCHAR          MICData
    );

PNICKEY
HwFindPerStaKey(
    __in PNIC                 pNic,
    __in DOT11_MAC_ADDRESS    MacAddr,
    __in UCHAR                keyId
    );



#endif  // _NATIVE_WIFI_HW_MAIN_H

