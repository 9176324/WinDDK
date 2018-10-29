/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Hw_802_11.h

Abstract:
    Commoon hardware structures
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef HW_802_11_HEADER
#define HW_802_11_HEADER

#define sAckCtsLng      112     // bits in ACK and CTS frames
#define sCrcLng         4       // octets for crc32 (FCS, ICV)
#define aSifsTime       10
#define sMacHdrLng      24      // octets in data header, no WEP

#define Frame_ProtocolVer(f)                ( ((pFrameControl)((f).Octet)) ->Field.ProtocolVersion)
#define Frame_Type(f)                   ( ((pFrameControl)((f).Octet)) ->Field.Type)
#define Frame_Subtype(f)                    ( ((pFrameControl)((f).Octet)) ->Field.Subtype)
#define Frame_ToDS(f)                   ( ((pFrameControl)((f).Octet)) ->Field.ToDS)
#define Frame_FromDS(f)                 ( ((pFrameControl)((f).Octet)) ->Field.FromDS)
#define Frame_MoreFrag(f)               ( ((pFrameControl)((f).Octet)) ->Field.MoreFrag)
#define Frame_Retry(f)                  ( ((pFrameControl)((f).Octet)) ->Field.Retry)
#define Frame_PwrMgt(f)                 ( ((pFrameControl)((f).Octet)) ->Field.PwrMgt)
#define Frame_MoreData(f)               ( ((pFrameControl)((f).Octet)) ->Field.MoreData)
#define Frame_WEP(f)                    ( ((pFrameControl)((f).Octet)) ->Field.WEP)
#define Frame_Order(f)                  ( ((pFrameControl)((f).Octet)) ->Field.Order)
#define Frame_DurID(f)                  ( *((USHORT *)((f).Octet+2)) )
#define Frame_Addr1(f)                  ( (f).Octet+4 )                                 // return pUCHAR
#define Frame_Addr2(f)                  ( (f).Octet+10 )                                // return pUCHAR
#define Frame_Addr3(f)                  ( (f).Octet+16 )                                // return pUCHAR
#define Frame_FragNum(f)                ( ((PSEQUENCE_CONTROL)((f).Octet+22))   ->Field.FragNum)
#define Frame_SeqNum(f)                 ( ((PSEQUENCE_CONTROL)((f).Octet+22))   ->Field.SeqNum)
#define Frame_Addr4(f)                  ( (f).Octet+24 )                                // return pUCHAR
#define Frame_ValidAddr4(f)             ( Frame_ToDS(f)&&Frame_FromDS(f) )
#define Frame_ContainQc(f)              ( (Frame_Type(f) == Type_Data) && (Frame_Subtype(f) & 0x08) )
#define Frame_FrameBody(f)              ( Frame_Addr4(f)+6*Frame_ValidAddr4(f) )    // return pUCHAR
#define Frame_QoSTID(f, offset)         ( ((FrameQoS    *)((f).Octet +offset )) ->field.TID)
#define Frame_IVKeyID(f,offset)         ( *((f).Octet+offset) <<6 & 0x03)

#define Frame_AuthAlgorithmNum(f)       ( *((PUSHORT)( Frame_FrameBody(f) + (Frame_WEP(f)?4:0) )) )
#define Frame_AuthTransactionSeqNum(f)  ( *((PUSHORT)( Frame_FrameBody(f) + 2 + (Frame_WEP(f)?4:0)) ) )
#define Frame_AuthStatusCode(f)         ( *((PUSHORT)( Frame_FrameBody(f) + 4 + (Frame_WEP(f)?4:0)) ) )
#define Frame_AuthChallengeText(f)      ( Frame_FrameBody(f) + 6 + (Frame_WEP(f)?4:0) )                 // return pUCHAR

#define Frame_AssocCap(f)               ( *(    (PUSHORT)(Frame_FrameBody(f)) ) )
#define Frame_AssocStatusCode(f)        ( *(    (PUSHORT)(Frame_FrameBody(f)+2) ) )
#define Frame_AssocAID(f)               ( *(    (PUSHORT)(Frame_FrameBody(f)+4) ) & 0x3FFF )


#define Frame_Addr1OS(a,f)          { CopyMem((a).Octet, (f).Octet+4, 6); (a).Length=6; }
#define Frame_Addr2OS(a,f)          { CopyMem((a).Octet, (f).Octet+10, 6); (a).Length=6; }
#define Frame_Addr3OS(a,f)          { CopyMem((a).Octet, (f).Octet+16, 6); (a).Length=6; }



#define IsFrameTypeMgnt(pframe)         ( ((pframe[0] & 0x0C)==0x00) ? TRUE : FALSE )
#define IsFrameTypeCtrl(pframe)         ( ((pframe[0] & 0x0C)==0x04) ? TRUE : FALSE )
#define IsFrameTypeData(pframe)         ( ((pframe[0] & 0x0C)==0x08) ? TRUE : FALSE )
#define Frame_pDaddr(pduOS)     ( pduOS.Octet + ( Frame_ToDS(pduOS) ? 16 : 4 ) )
#define Frame_pSaddr(pduOS)     ( pduOS.Octet + ( Frame_FromDS(pduOS) ? 16 : 10 ) )
#define Frame_pBssid(pduOS)     ( pduOS.Octet + ( Frame_FromDS(pduOS) ? 10 : ( Frame_ToDS(pduOS) ? 4 : 16 ) ) )

#define PacketGetType(_packet)  ((_packet).Octet[0] & 0xFC)


#define FillOctetString(_os,_octet,_len)        \
    (_os).Octet=(PUCHAR)(_octet);                   \
    (_os).Length=(_len)

typedef struct _OctetString
{
    PUCHAR  Octet;
    USHORT  Length;
}OctetString,*pOctetString;

typedef enum _ElementID{
    EID_SsId = 0,       /* service set identifier (0:32) */
    EID_SupRates = 1,   /* supported rates (1:8) */
    EID_FHParms = 2,    /* FH parameter set (5) */
    EID_DSParms = 3,    /* DS parameter set (1) */
    EID_CFParms = 4,    /* CF parameter set (6) */
    EID_Tim = 5,        /* Traffic Information Map (4:254) */
    EID_IbssParms = 6,  /* IBSS parameter set (2) */
    EID_Ctext = 16,     /* challenge text*/
    EID_ExtSupRates = 50,
}ElementID;

typedef enum _Capability{
    cESS        = 0x01,
    cIBSS       = 0x02,
    cPollable   = 0x04,
    cPollReq    = 0x01,
    cPrivacy    = 0x10,
    cShortPreamble=0x20,
    cPBCC       = 0x40,
}Capability;

typedef enum    _TypeSubtype{
    Type_Asoc_Req       = 0x00,
    Type_Asoc_Rsp       = 0x10,
    Type_Reasoc_Req = 0x20,
    Type_Reasoc_Rsp = 0x30,
    Type_Probe_Req      = 0x40,
    Type_Probe_Rsp      = 0x50,
    Type_Beacon     = 0x80,
    Type_Atim           = 0x90,
    Type_Disasoc        = 0xa0,
    Type_Auth           = 0xb0,
    Type_Deauth     = 0xc0,
    Type_PS_poll        = 0xa4,//= MkString(S8(0,0,1,0,0,1,0,1));
    Type_RTS            = 0xb4,//= MkString(S8(0,0,1,0,1,1,0,1));
    Type_CTS            = 0xc4,//= MkString(S8(0,0,1,0,0,0,1,1));
    Type_Ack            = 0xd4,//= MkString(S8(0,0,1,0,1,0,1,1));
    Type_Cfend          = 0xe4,//= MkString(S8(0,0,1,0,0,1,1,1));
    Type_Cfend_Ack      = 0xf4,//= MkString(S8(0,0,1,0,1,1,1,1));
    Type_Data           = 0x08,//= MkString(S8(0,0,0,1,0,0,0,0));
    Type_Data_Ack       = 0x18,//= MkString(S8(0,0,0,1,1,0,0,0));
    Type_Data_Poll      = 0x28,//= MkString(S8(0,0,0,1,0,1,0,0));
    Type_Data_Poll_Ack = 0x38,//= MkString(S8(0,0,0,1,1,1,0,0));
    Type_Null_Frame = 0x48,//= MkString(S8(0,0,0,1,0,0,1,0));
    Type_Cfack          = 0x58,//= MkString(S8(0,0,0,1,1,0,1,0));
    Type_Cfpoll         = 0x68,//= MkString(S8(0,0,0,1,0,1,1,0));
    Type_Cfpoll_Ack     = 0x78,//= MkString(S8(0,0,0,1,1,1,1,0));

}TypeSubtype;

typedef union _FrameControl
{
    USHORT  ShortData;
    UCHAR   CharData[2];
    struct
    {
        USHORT  ProtocolVersion:2;
        USHORT  Type:2;
        USHORT  Subtype:4;
        USHORT  ToDS:1;
        USHORT  FromDS:1;
        USHORT  MoreFrag:1;
        USHORT  Retry:1;
        USHORT  PwrMgt:1;
        USHORT  MoreData:1;
        USHORT  WEP:1;
        USHORT  Order:1;
    }Field;
}FrameControl, *pFrameControl;

typedef union _SEQUENCE_CONTROL{
    USHORT ShortData;
    struct
    {
        USHORT  FragNum:4;
        USHORT  SeqNum:12;
    }Field;
}SEQUENCE_CONTROL, *PSEQUENCE_CONTROL;


typedef struct _GeneralPacketPartial{
    FrameControl        FrameCtrl;
    USHORT          Duration;
    UCHAR           Addr1[6];
    UCHAR           Addr2[6];
    UCHAR           Addr3[6];
    USHORT          Seq;
}GeneralPacketPartial, *pGeneralPacketPartial;

typedef struct _BeaconProbeRspPartial{
    GeneralPacketPartial    GenPacketPartial;
    ULONGLONG           TimeStamp;
    USHORT              BeaconInterval;
    USHORT              CapabilityInfo;
}BeaconProbeRspPartial, *pBeaconProbeRspPartial;

typedef struct _AuthenticatePacketPartial{
    GeneralPacketPartial    GenPacketPartial;
    USHORT              AuthAlgNum;
    USHORT              AuthSeqNum;
    USHORT              StatusCode;
    UCHAR               ChallengeText[1];
}AuthenticatePacketPartial, *pAuthenticatePacketPartial;

typedef struct _AssociateReqPartial{
    GeneralPacketPartial    GenPacketPartial;
    USHORT              CapabilityInfo;
    USHORT              ListenInterval;
}AssociateReqPartial, *pAssociateReqPartial;

typedef struct _AssociateRspPartial{
    GeneralPacketPartial    GenPacketPartial;
    USHORT              CapabilityInfo;
    USHORT              StatusCode;
    USHORT              AID;
}AssociateRspPartial, *pAssociateRspPartial;

typedef struct _TIM{
    UCHAR   DtimCount;
    UCHAR   DtimPeriod;
    UCHAR   BitmapCtrl;
    UCHAR   PartialVirtualBitmap[251];
}TIM,*pTIM;

// ===============================================
//3 Function declaration
// ===============================================
BOOLEAN 
EqualOS(
    OctetString os1,
    OctetString os2);

OctetString 
PacketGetElement(
    OctetString packet,
    ElementID ID);

void
PacketMakeElement(
    OctetString     *packet,
    ElementID       ID,
    OctetString     info);

void
PacketAppendData(
    OctetString     *packet,
    OctetString     data);

BOOLEAN
TimGetBcMcBit(
    OctetString Tim);

BOOLEAN 
TimGetAIDBit( 
    OctetString     Tim, 
    unsigned short  AID);

USHORT 
ComputeTxTime(
    __in USHORT   FrameLength,            //3in byte
    __in ULONG    DataRate,               //3in 0.5 Mbits
    __in BOOLEAN  bManagementFrame,
    __in BOOLEAN  bShortPreamble);

void
ComputePLCPLength(
    __in USHORT       FrameLength,        //3in byte
    __in ULONG        DataRate,           //3in 0.5 Mbits
    __out USHORT      *FrameTime,
    __out USHORT      *LengExt 
    );

BOOLEAN
CompareSSID(
    PUCHAR ssidbuf1,
    USHORT  ssidlen1,
    PUCHAR ssidbuf2,
    USHORT  ssidlen2
);


#endif

