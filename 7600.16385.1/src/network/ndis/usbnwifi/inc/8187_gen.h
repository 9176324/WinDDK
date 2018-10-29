#ifndef __INC_8187_GEN_H
#define __INC_8187_GEN_H

#define RESET_DELAY_8187                20
#define RT_IMR_BcnInt                   IMR_BcnInt
#define RT_IMR_ATIMInt                  IMR_ATIMInt
#define MIN_DESC_BUFFER_LENGTH          5
#define RTL8187_HWDESC_HEADER_LEN       12
#define RTL8187_HWRXDESC_HEADER_LEN     16
#define MAX_PER_PACKET_BUFFER_LIST_LENGTH   MAX_PER_PACKET_PHYSICAL_BUF_NUM+MAX_FRAGMENT_COUNT+3        // +3 is for Header, LLC and trailer, MAX_FRAGMENT_COUNT for max fragment overhead

extern EEPROM_OFFSET EEPROMOffset8187;


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

// Power save mode configured. 
typedef enum _RT_PS_MODE    
{
    eActive,    // Active/Continuous access.
    eMaxPs,     // Max power save mode.
    eFastPs     // Fast power save mode.
}RT_PS_MODE;

ULONGLONG
PlatformGetCurrentTime(
    VOID);

//3 =============================================================================
   //3 Put all function prototypes below
//3 =============================================================================
UCHAR
GetEEPROMSize8187(
    IN PNIC         Nic
    );

NDIS_STATUS
ReadNicInfo8187(
    IN PNIC         Nic
    );

VOID
InitializeVariables8187(
    IN PNIC         Nic
    );
   /*
VOID
CancelAllTimer8187(
    PNIC            Nic
    );
    */

NDIS_STATUS    
InitializeNic8187(
    IN PNIC         Nic,
    IN UCHAR           Channel
    );

VOID
HaltNic8187(
    IN PNIC         Nic,
    IN BOOLEAN          bReset
    );

BOOLEAN
IsSwChnlInProgress8187(
    IN PNIC         Nic
    );

BOOLEAN
IsUsbTxFreeContextAvailable(
    IN  PNIC        Nic,
    IN  UCHAR          QueueIndex
);

VOID
TxFillDescriptor8187(
    IN  PNIC        Nic,
    IN  PTX_DESC_8187   TxDesc,
    IN  UCHAR           QueueIndex,
    IN  PUCHAR          pHeader,
    IN  BOOLEAN         bFirstSeg,
    IN  BOOLEAN         bLastSeg,
    IN  PUCHAR          VirtualAddress,
    IN  ULONG_PTR           BufferLen,
    IN  ULONG           PktLen,
    IN  USHORT          Rate,
    IN  BOOLEAN         bMulticast,
    IN  BOOLEAN         bUseShortPreamble,
    IN  BOOLEAN         bMoreFrag,
    IN  BOOLEAN         bCTSEnable,
    IN  BOOLEAN         bRTSEnable,
    IN  USHORT          RTSDuration,
    IN  USHORT          RTSRate,
    IN  BOOLEAN         bEncrypt,
    IN  UCHAR           RetryLimit,
    IN  PVOID           Reserved   
    );



BOOLEAN
SetupBeaconQueue8187(
    IN  PNIC Nic
    );


VOID
Init8185(
    IN  PNIC        pNic
    );

//1 Function prototypes for Hal8187PhyCfg.c
NDIS_STATUS
PhyConfig8187(
    IN  PNIC            Nic
    );

VOID
SwChnlByDelay8187(  // Only called during initialize
    PNIC            Nic,
    UCHAR              channel
    );

VOID
SwChnlByTimer8187(  // Call after initialization
    PNIC            Nic,
    UCHAR              channel
    );

VOID
GetHwReg8187(
    IN PNIC             Nic,
    IN UCHAR               variable,
    OUT PUCHAR             val
    );

VOID
SetHwReg8187(
    IN PNIC             Nic,
    IN UCHAR               variable,
    IN PUCHAR              val
    );

VOID 
SetZebraOfdmTxPower(
    IN  PNIC    Nic,
    IN  int         TxPower
    );

void
SetTxPowerLevel8187(
    PNIC    Nic,
    UCHAR      channel
);

VOID
GetTxPowerLevel8187(
    PNIC        Nic,
    PULONG     channel
    );

void
SetTxAntenna8187(
    IN PNIC             Nic,
    IN UCHAR               SelectedAntenna
    );


void
SetRxAntenna8187(
    IN PNIC             Nic,
    IN UCHAR               SelectedAntenna
    );


void
GetAntennaNumber8187(
    IN PNIC             Nic,
    OUT PUCHAR                 AntennaNumber
    );

VOID
ResetStateMachine8187(
    IN PNIC         Nic
    );

ULONG
GetChannelList8187(
    IN  PNIC            Nic,
    OUT PULONG             *ppList
    );

USHORT
GetChannelPlanIndex8187(
    IN  PNIC            Nic
);

// Annie, 2004-12-28
void
SetChannelPlanIndex8187(
    IN  PNIC            Nic,
    IN  USHORT              ChannelPlan
);

UCHAR
GetChannelNumber8187(
    IN  PNIC            Nic
    );

VOID
SetRegCWSetting8187(
    IN  PNIC            Nic,
    IN  UCHAR              CWinMaxMin
);

VOID
GetRegCWSetting8187(
    IN  PNIC            Nic,
    OUT PUCHAR             pRegCWMin,
    OUT PUCHAR             pRegCWMax
);

VOID
DisableContentionWindow8187(
    IN  PNIC            Nic,
    IN  BOOLEAN             bDisableCW
);

VOID
AllowAllDestAddr8187(
    IN  PNIC            Nic,
    IN  BOOLEAN             bAllowAllDA,
    IN  BOOLEAN             WriteIntoReg
    );

VOID
ActUpdateChannelAccessSetting(
    PNIC                Nic,
    WIRELESS_MODE           WirelessMode,
    PCHANNEL_ACCESS_SETTING ChnlAccessSetting
    );

VOID
ActSetWirelessMode8187(
    PNIC            Nic,
    UCHAR              bWirelessMode
    );

UCHAR
GetSupportedWirelessMode8187(
    PNIC            Nic
);

VOID
QueryRxDescStatus8187(
    IN  PNIC        Nic,
    IN  PRX_STATUS_DESC_8187    pDesc,
    OUT PRX_STATUS_DESC_8187  pRtRfdStatus
    );

UCHAR
GetInterfaceSelection8187(
    PNIC            Nic
);

EVT_WDF_TIMER LedControlCallback8187;

VOID
LedControl8187(
    PNIC            Nic,
    LED_CTL_MODE_8187   mode
);

VOID
HwSetNicPSMode(
    PNIC    Nic,
    RT_PS_MODE  PowerSaveMode
);

BOOLEAN
SetupBeaconQueue8187(
    PNIC    Nic
);


VOID
SetupRFEInitialTiming(
    PNIC        Nic
);

RT_RF_POWER_STATE
GetRFPowerState8187(
    PNIC            Nic
);

BOOLEAN
SetRFPowerState8187(
    PNIC            Nic, 
    RT_RF_POWER_STATE   eRFPowerState
);

VOID
MgntSetRegdot11OperationalRateSet(
    PNIC            Nic
    );

void
HwSaveAdjustBeforeScan(
    IN PNIC pNic
    );

VOID
HwDumpAllRegisters(
    IN PNIC Nic
    );

void
SetEncryption8187(
    IN PNIC  pNic,
    IN DOT11_CIPHER_ALGORITHM algoId,
    IN BOOLEAN useDefaultKey
    );

#endif // #ifndef __INC_8187_GEN_H


