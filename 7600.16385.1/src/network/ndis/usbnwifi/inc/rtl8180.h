/****************************************************************************
** COPYRIGHT (C) 2001-2002 Realtek Semiconductor Corp.,                    **
** THIS FILE IS PART OF THE Realtek RTL8180 Adapter                        **
** NDIS 5.0 MINIPORT DRIVER                                                **
****************************************************************************/

/****************************************************************************
Module Name:
    rtl8180.h

This driver runs on the following hardware:
    - RTL8180 WLAN PCI adapters

Environment:
    Kernel Mode - Or whatever is the equivalent on WinNT

Revision History
    - Sean 4/8/2002 Driver Created
*****************************************************************************/

#ifndef _RTL8180_REG_H
#define _RTL8180_REG_H

#define     IDR0        0x0             /* Ethernet hardware address. */
#define     IDR4        0x4
#define     MAR0        0x8             /* Multicast filter. */
#define     MAR4        0x0c
#define     DTCCR       0x10            /* Dump Tally Counter Command Register (64-byte) */
#define     TSFR        0x18            // Timing synchronization function register
#define     TLPDS       0x20            // Tx Low priority descriptor queue start address
#define     TNPDS       0x24            // Tx Nowmal priority descriptot queue start address
#define     THPDS       0x28            // Tx Hign priority descriptor queue start address
#define     BRSR        0x2C            // Basic rate set
#define     BSSIDR      0x2E            // BSSID
#define     CMDR        0x37            // Command register
#define     IMR         0x3C
#define     ISR         0x3E
#define     TCR         0x40
#define     RCR         0x44
#define     TCTR        0x48                /* A general-purpose counter. */
#define     BDQSAR      0x4C                /* Null function Start Address Register */
#define     TBDR        0x4c            // Beacon descriptor queue start address
#define     CR9346      0x50
#define     CONFIG0     0x51
#define     CONFIG1     0x52
#define     CONFIG2     0x53
#define     AnaParm     0x54
#define     MSR         0x58                //Media status register
#define     CONFIG3     0x59
#define     Config4     0x5A
#define     TestR       0x5B                // Media status register 2
#define     PSR         0x5E                // PageSelect register
#define     SCR         0x5F                // Security configuration register
#define     BcnIntv     0x70                // Beacon interval register(2 bytes)
#define     AtimWnd     0x72                // Atim Window register (2 bytes)
#define     BcnIntTime  0x74                // Time to trigger interrupt before beacon time
#define     LIR         0x76                // Listen interval Register
#define     PhyDelay    0x78                // Phy Delay
#define     CSCounter   0x79                // Carrier Sense Counter
#define     BBAddr      0x7C                // Phy register address register
#define     PhyDataW    0x7D                // Phy register data write
#define     PhyDataR    0x7E                // Phy register data read

#define     DK0         0x90                // default key 0 register 
#define     DK1         0xA0                // default key 0 register 
#define     DK2         0xB0                // default key 0 register 
#define     DK3         0xC0                // default key 0 register 
#define     CONFIG5     0xD8                //
#define     TPPoll      0xD9                // Transmit priority polling register
#define     CWR         0xDC                // Contention window register
#define     RetryCTR    0xDE                // Retry Count register
#define     RDSAR       0xE4                // Receive descriptor queue start address

#define     WolPtrnMsk0 0x84
#define     WolPtrnMsk1 0x8C
#define     WolPtrnMsk2 0x94
#define     WolPtrnMsk3 0xA4
#define     WolPtrnMsk4 0xB4
#define     WolPtrnCRC0 0xC4

#define     WakeupMask0 0x284
#define     WakeupMask1 0x28C
#define     WakeupMask2 0x294
#define     WakeupMask3 0x2A4
#define     WakeupMask4 0x2B4
#define     PatternCRC0 0x2C4
#define     PatternCRC1 0x2C6
#define     PatternCRC2 0x2C8
#define     PatternCRC3 0x2CA
#define     PatternCRC4 0x2CC

//Wakeup pattern related register map,  start at 0x284
typedef struct  _WakeupPattern{
    ULONG   ShortPattern0Mask[2];
    ULONG   ShortPattern1Mask[2];
    ULONG   LongPattern2Mask[4];
    ULONG   LongPattern3Mask[4];
    ULONG   LongPattern4Mask[4];
    USHORT  PatternCRC[5];
}WakeupPatternReg;

typedef struct  _REG_MAP{
    UCHAR       IDR[6];
    UCHAR       resv1[2];
    UCHAR       MAR[8];
    ULONG       DTCC;
    ULONG       resv2;
    ULONG       TSFLow;
    ULONG       TSFHigh;    
    ULONG       Tlpds;      //0x20
    ULONG       Tnpds;
    ULONG       Thpds;
    USHORT      Brsr;
    UCHAR       Bssidr[6];
    USHORT      Erbcr;
    UCHAR       Ersr;
    UCHAR       Cmdr;       //0x37
    UCHAR       Edtr;
    UCHAR       Cstr;
    UCHAR       Rssir;
    UCHAR       Sqr;
    USHORT      Imr;
    USHORT      Isr;
    ULONG       Tcr;        //0x40
    ULONG       Rcr;
    ULONG       Tctr;
    ULONG       Bqsar;
    UCHAR       Cr9346;     //0x50
    UCHAR       Config0;
    UCHAR       Config1;
    UCHAR       Config2;
    ULONG       Timerint;   
    UCHAR       Msr;
    UCHAR       Config3;
    UCHAR       CONFIG4;
    UCHAR       Msr2;
    USHORT      Mulint;
    UCHAR       Cwmvr;
    UCHAR       Scr;
    UCHAR       Ckr[16];
    UCHAR       Dk[4][16];
    USHORT      Bcnintv;
    USHORT      Atimwnd;

}RegMap;

//////////////////////////////////////////////////////////////////////////////
//
// PCI configuration space of RTL8180
//
//////////////////////////////////////////////////////////////////////////////
#define PMID    0x50
#define PMCSR   0x54
#define PMCtl   0x55



#define BIT0    0x1
#define BIT1    0x2
#define BIT2    0x4
#define BIT3    0x8
#define BIT4    0x10
#define BIT5    0x20
#define BIT6    0x40
#define BIT7    0x80
#define BIT8    0x100
#define BIT9    0x200
#define BIT10   0x400
#define BIT11   0x800
#define BIT12   0x1000
#define BIT13   0x2000
#define BIT14   0x4000
#define BIT15   0x8000
#define BIT16   0x10000
#define BIT17   0x20000
#define BIT18   0x40000
#define BIT19   0x80000
#define BIT20   0x100000
#define BIT21   0x200000
#define BIT22   0x400000
#define BIT23   0x800000
#define BIT24   0x1000000
#define BIT25   0x2000000
#define BIT26   0x4000000
#define BIT27   0x8000000
#define BIT28   0x10000000
#define BIT29   0x20000000
#define BIT30   0x40000000
#define BIT31   0x80000000

//-------------------------------------------------------------------
// 8180 Maximum Basic Rate Set Register     (offset 0x2C-0x2D) 
//-------------------------------------------------------------------
#define BRSR_MBRate     0x0000          // Maximum basic rate:bit 0-7
#define BRSR_SPLCP      0x0100          // Short PLCP for response packet

//----------------------------------------------------------------------------
//       8139 (CMD) command register bits   (Offset 0x37, 8 but)
//----------------------------------------------------------------------------
#define CR_RST          0x10
#define CR_RE           0x08
#define CR_TE           0x04
#define CR_MulRW        0x01

//----------------------------------------------------------------------------
//       8139 IMR/ISR bits                  (offset 0x3C/0x3E, 16 bits)
//----------------------------------------------------------------------------
#define IMR_TXFOVW      0x8000
#define IMR_TimeOut     0x4000          // General Purpose Timer Interrupt
#define IMR_BcnInt      0x2000          // Beacon Timer Interrupt
#define IMR_ATIMInt     0x1000          // ATIM Timer Interrupt
#define IMR_TBDER       0x0800          // Tx Beacon descriptor error interrupt
#define IMR_TBDOK       0x0400          // Tx Beacon descriptor ok interrupt
#define IMR_THPDER      0x0200          // Tx high priority descriptor error interrupt
#define IMR_THPDOK      0x0100          // Tx high priority descriptor ok interrupt
#define IMR_TNPDER      0x0080          // Tx normal priority descriptor error interrupt
#define IMR_TNPDOK      0x0040          // Tx normal priority descriptor ok interrupt
#define IMR_RXFOVW      0x0020          // Rx Fifo Overflow
#define IMR_RDU         0x0010          // rx buffer overflow
#define IMR_TLPDER      0x0008          // transmit error
#define IMR_TLPDOK      0x0004          // transmit OK
#define IMR_RER         0x0002          // receive error, collided frame
#define IMR_ROK         0x0001          // Receive OK
#define IMR_EMPTY       0x0000          // no bits set in IMR

#define INTERRUPT_MASK  (IMR_FIFOOVW | IMR_THPDER | IMR_THPDOK | IMR_TNPDER | IMR_TNPDOK | IMR_TLPDER | IMR_TLPDOK | IMR_RER | IMR_ROK)

//----------------------------------------------------------------------------
//       8139 TCR bits                      (Offset 0x40, 32bit)
//----------------------------------------------------------------------------
#define TCR_noCRC       0x00010000      // Supress CRC
#define TCR_LBK         0x00060000      // loopback mode
#define TCR_noICV       0x00080000      // Supress ICV
#define TCR_DisCW       0x00100000      // Disable Contention Window
#define TCR_MXDMA       0x00E00000      // Max DMA Burst size per TX DMA burst  
#define TCR_SAT         0x01000000      // Enable Rate depedent ack timeout timer
#define TCR_HWVERID     0x7E000000      // Hardware Version ID
#define TCR_CWLIMIT8    0x80000000      // CW8
#define TCR_MXDMA_OFFSET    21


//----------------------------------------------------------------------------
//       8139 RCR bits                          (offset 0x44, 32 bits)
//----------------------------------------------------------------------------
#define RCR_AAP         0x00000001      // accept all physical address
#define RCR_APM         0x00000002      // accept physical match
#define RCR_AM          0x00000004      // accept multicast
#define RCR_AB          0x00000008      // accept broadcast
//#define RCR_ACRC16      0x00000010        // accept CRC16 packet
#define RCR_ACRC32      0x00000020      // accept error packet
#define RCR_9356SEL     0x00000040
#define RCR_EU          0x00000080      // exclude unencrypt
#define RCR_MXDMA0      0x00000100      // Max DMA burst size bit 0
//#define RCR_AMIC      0x00000800      // Accept MIC error packet
#define RCR_AICV        0x00001000      // Accept ICV error packet
#define RCR_RXFTH0      0x00002000      // Rx FIFO threshold
#define RCR_ADF         0x00040000      // Accept Data(frame type) frame
#define RCR_ACF         0x00080000      // Accept control frame
#define RCR_AMF         0x00100000      // Accept management frame
#define RCR_ADD3        0x00200000
#define RCR_APWRMGT     0x00400000      // Accept power management packet
#define RCR_ABSSID      0x00800000      // Accept BSSID match packet
#define RCR_ENMARP      0x10000000      // enable mac auto reset phy
#define RCR_EnCS1       0x20000000      // enable carrier sense method 1
#define RCR_EnCS2       0x40000000      // enable carrier sense method 2
#define RCR_OnlyErlPkt  0x80000000      // Rx Early mode is performed for packet size greater than 1536

#define DEFAULT_RX_FIFO_LIMIT       7
#define RCR_MXDMA_OFFSET            8
#define RCR_FIFO_OFFSET             13
//bug?
#define RCR_AR          0x0
//----------------------------------------------------------------------------
//       8139 (CR9346) 9346 command register bits (offset 0x50, 1 byte)
//----------------------------------------------------------------------------
#define CR9346_EEDO     0x01            // 9346 data out
#define CR9346_EEDI     0x02            // 9346 data in
#define CR9346_EESK     0x04            // 9346 serial clock
#define CR9346_EECS     0x08            // 9346 chip select
#define CR9346_EEM0     0x40            // select 8139 operating mode
#define CR9346_EEM1     0x80            // 00: normal
                                        // 01: autoload
                                        // 10: 9346 programming
                                        // 11: config write enable
#define CR9346_CFGRW    0xC0            // Config register write
#define CR9346_NORM     0x0             //                                         
//-------------------------------------------------------------------------
// EEPROM bit definitions
//-------------------------------------------------------------------------
//- EEPROM control register bits
#define EN_TRNF                     0x10    // Enable turnoff
#define EEDO                        CR9346_EEDO    // EEPROM data out
#define EEDI                        CR9346_EEDI    // EEPROM data in (set for writing data)
#define EECS                        CR9346_EECS    // EEPROM chip select (1=high, 0=low)
#define EESK                        CR9346_EESK    // EEPROM shift clock (1=high, 0=low)

//- EEPROM opcodes
#define EEPROM_READ_OPCODE          06
#define EEPROM_WRITE_OPCODE         05
#define EEPROM_ERASE_OPCODE         07
#define EEPROM_EWEN_OPCODE          19      // Erase/write enable
#define EEPROM_EWDS_OPCODE          16      // Erase/write disable

//- EEPROM data locations
#define RTL8180_EEPROM_ID           0x8129
#define EEPROM_RF_CHIP_ID           0x0C
#define EEPROM_NODE_ADDRESS_BYTE_0  0x0E
#define EEPROM_CONFIG2              0x18
#define EEPROM_TX_POWER_LEVEL_0     0x20
#define EEPROM_CHANNEL_PLAN         0x2E
#define EEPROM_CS_THRESHOLD         0x2F
#define EEPROM_RF_PARAM             0x32
#define EEPROM_VERSION              0x3c                
#define EEPROM_CIS_DATA             0x80
#define EEPROM_SSID                 0x08

#define EEPROM_FLAGS_WORD_3         3
#define EEPROM_FLAG_10MC            BIT_0
#define EEPROM_FLAG_100MC           BIT_1

//----------------------------------------------------------------------------
//       8180 Config0 Regsiter          (offset 0x51, 1 byte)
//----------------------------------------------------------------------------
#define Config0_WEP40           0x80            // 40 bit WEP implemented
#define Config0_WEP104          0x40            // 104 bit WEP implemented
#define Config0_AuxStatus       0x08            // Auxiliary power present
#define Config0_GL              0x03            // Geographic location
                                                // 1:Japan,2:Europe,3:USA
//----------------------------------------------------------------------------
//       8180 Config1 Regsiter          (offset 0x52, 1 byte)
//----------------------------------------------------------------------------
#define Config1_LEDS1           0x80            // LED pin definition
#define Config1_LEDS0           0x40            //
#define Config1_CLKRUN_SEL      0x20            // Cardbus CLK_RUN mode select
#define Config1_LWAKE           0x10            // LWAKE active mode
#define Config1_MEMMAP          0x08            // PCI memory map
#define Config1_IOMAP           0x04            // PCI io map
#define Config1_VPD             0x02            // PCI VPD function
#define Config1_PMEn            0x01            // Power management enable

//----------------------------------------------------------------------------
//       8180 Config2 Regsiter          (offset 0x53, 1 byte)
//----------------------------------------------------------------------------
#define Config2_LCK             0x80            // Locked clock
#define Config2_ANT             0x40            // Antenna diversity
#define Config2_TX_STOP         0x10            // Tx stop mode
#define Config2_DPS             0x08            // Descriptor polling state
#define Config2_PAPE_sign       0x04            // PAPE sign
#define Config2_PAPE_time       0x03            // PAPE time

//----------------------------------------------------------------------------
//       8139 MSR:Media Status Regsiter (offset 0x58, 1 byte)
//----------------------------------------------------------------------------
#define MSR_mAP                 0x0C
#define MSR_mAssoc              0x08            // Currnet link speed is 10M
#define MSR_mIbss               0x04            // Current link is dis-connected
#define MSR_mAtimW              0x02            // Tx Pause packet
#define MSR_mCfp                0x01            // Rx Pause packet

//----------------------------------------------------------------------------
//       8180 Config3 Regsiter          (offset 0x59, 1 byte)
//----------------------------------------------------------------------------
#define Config3_GNTSel          0x80
#define Config3_ParmEn          0x40            // enable write to ANA_PARM
                                                //  (0x54) register
#define Config3_Magic           0x20            // Enable Magic Packet Wakeup
#define Config3_CardBEn         0x08            // Cardbus Enable
#define Config3_CLKRUN_En       0x04            // CLKRUN(clock run) Enable
#define Config3_FuncRegEn       0x02            // Function Register Enable
#define Config3_FBtBEn          0x01            // Enable PCI fast-back-to-back

//----------------------------------------------------------------------------
//       8180 Config4 Regsiter          (offset 0x5a, 1 byte)
//----------------------------------------------------------------------------
#define Config4_VCOPowerDown    0x80
#define Config4_PowerOff        0x40            // Turn ON/Off RF Power(RFMD)
#define Config4_PwrMgtMode      0x20            // Adapter in Power Save mode
#define Config4_LWPME           0x10            // LanWake vs PME
#define Config4_LWPTN           0x04            // LanWake pattern
#define Config4_RFtype          0x03            // RF type

//----------------------------------------------------------------------------
//       8180 PSR Regsiter              (offset 0x5e, 1 byte)
//----------------------------------------------------------------------------
#define PSR_UWF                 0x02            // Enable DID match wakeup
#define PSR_PSEn                0x01

#define WOLPatternPage          1
#define WEPKeyPage              0
/* SCR define */
//----------------------------------------------------------------------------
//       8180 Security Control Regsiter (offset 0x5F, 1 byte)
//----------------------------------------------------------------------------
#define SCR_RxSecOn             0x01            // Enable Rx WEP
#define SCR_TxSecOn             0x02            // Enable Tx WEP
#define SCR_WEP104              0x10            // Turn on WEP2 security scheme
#define SCR_WEP40               0x00            // Turn on WEP security scheme


//----------------------------------------------------------------------------
//       8180 Config5 Regsiter          (offset 0xd8, 1 byte)
//----------------------------------------------------------------------------
#define Config5_EACPI           0x04            // Enable WOL

//----------------------------------------------------------------------------
//       8180 TPPool bits               (offset 0xd9, 1 byte)
//----------------------------------------------------------------------------
#define TPPoll_BPQ              BIT7
#define TPPoll_HPQ              BIT6            // High priority queue polling
#define TPPoll_NPQ              BIT5            // normal priority queue polling
#define TPPoll_LPQ              BIT4            // low priority queue polling

#define TPPoll_FSWInt           0x1             // Forced software interrupt


//=============================================================================
// Tx Descriptor status bits
//----------------------------------------------------------------------------
//#define TXS_CC3_0       BIT0|BIT1|BIT2|BIT3
//#define TXS_EXC         BIT4
//#define   TXS_LNKF        BIT5
//#define TXS_OWC           BIT6
//#define TXS_TES           BIT7
//#define TXS_UNF         BIT9
//#define TXS_LGSEN     BIT11
//#define TXS_LS          BIT12
//#define TXS_FS          BIT13
//#define TXS_EOR         BIT14
//#define TXS_OWN         BIT15
#define TXS_SPLCP       BIT0
#define TXS_MOREFRAG    BIT1
#define TXS_BEACON      BIT2
#define TXS_RTSRATE     BIT3|BIT4|BIT5|BIT6
#define TXS_RTSEN       BIT7
#define TXS_TXRATE      BIT8|BIT9|BIT10|BIT11
#define TXS_LS          BIT12
#define TXS_FS          BIT13
#define TXS_EOR         BIT14
#define TXS_OWN         BIT15

#define TXS_LENGEXT     BIT15
#define TXS_RTSRATE_OFFSET  3
#define TXS_TXRATE_OFFSET   8

//TASK Offload bits
#define TXS_TCPCS       BIT0
#define TXS_UDPCS       BIT1
#define TXS_IPCS        BIT2


//=============================================================================
// Tx/Rx Descriptor VLAN control bits
//----------------------------------------------------------------------------
#define Tx_TAGC         BIT1
#define Rx_TAVA         BIT0

//=============================================================================
// Tx Descriptor status bits
//----------------------------------------------------------------------------
#define RXS_OWN      BIT15
#define RXS_EOR      BIT14
#define RXS_FS       BIT13
#define RXS_LS       BIT12


#define RXS_DMAFAIL  BIT11
#define RXS_BOVF     BIT10
#define RXS_SPLCP    BIT9
#define RXS_Resv1    BIT8
#define RXS_RATE3    BIT7
#define RXS_RATE2    BIT6
#define RXS_RATE1    BIT5
#define RXS_RATE     BIT4
#define RXS_MIC      BIT3
#define RXS_MAR      BIT2
#define RXS_PAR      BIT1
#define RXS_BAR      BIT0

//CheckSum Fail bits
#define RXS_ICV     BIT0
#define RXS_CRC32   BIT1
#define RXS_CRC16   BIT2

#define RX_MSDU_ACT_COUNT_MASK  0x1FFF

#pragma pack(1)

//-------------------------------------------------------------------------
// Ethernet Frame Structure
//-------------------------------------------------------------------------
//- Ethernet 6-byte Address
typedef struct _ETH_ADDRESS_STRUC {
    UCHAR       EthNodeAddress[ETHERNET_ADDRESS_LENGTH];
} ETH_ADDRESS_STRUC, *PETH_ADDRESS_STRUC;


//- Ethernet 14-byte Header
typedef struct _ETH_HEADER_STRUC {
    UCHAR       Destination[ETHERNET_ADDRESS_LENGTH];
    UCHAR       Source[ETHERNET_ADDRESS_LENGTH];
    USHORT      TypeLength;
} ETH_HEADER_STRUC, *PETH_HEADER_STRUC;


//- Ethernet Buffer (Including Ethernet Header) for Transmits
typedef struct _ETH_TX_BUFFER_STRUC {
    ETH_HEADER_STRUC    TxMacHeader;
    UCHAR               TxBufferData[(TX_MSDU_BUFFER_SIZE - sizeof(ETH_HEADER_STRUC))];
} ETH_TX_BUFFER_STRUC, *PETH_TX_BUFFER_STRUC;

typedef struct _ETH_RX_BUFFER_STRUC {
    ETH_HEADER_STRUC    RxMacHeader;
    UCHAR               RxBufferData[(RCB_BUFFER_SIZE - sizeof(ETH_HEADER_STRUC))];
} ETH_RX_BUFFER_STRUC, *PETH_RX_BUFFER_STRUC;

//-------------------------------------------------------------------------
// Control/Status Registers (CSR)
//-------------------------------------------------------------------------
typedef struct _REG_STRUC {
    UCHAR       IDR[8];                 // SCB Status register
    UCHAR       MAR[8];                 // SCB Command register (low byte)
    UCHAR       ScbCommandHigh;         // SCB Command register (high byte)
    ULONG       ScbGeneralPointer;      // SCB General pointer
    ULONG       Port;                   // PORT register
    USHORT      FlashControl;           // Flash Control register
    USHORT      EepromControl;          // EEPROM control register
    ULONG       MDIControl;             // MDI Control Register
    ULONG       RxDMAByteCount;         // Receive DMA Byte count register
} REG_STRUC, *PREG_STRUC;



//-------------------
// 802.1q Tag
//-------------------
//#define TYPE_8021qTAG   0x0081
//802.1p tag
typedef struct  TAG_802_1Q{
//     USHORT     TagType;
     USHORT     VLanID1 :  4;
     USHORT     CFI     :  1;
     USHORT     Priority:  3;
     USHORT     VLanID2 :  8;
}TAG_8021q;


//-------------------------------------------------------------------------
// Transmit Command Block (TxCB)
//-------------------------------------------------------------------------
typedef struct _TX_DESC {
    unsigned short      TPktSize:12;
    unsigned short      resv:2;
    unsigned short      keyID:2;
    unsigned short      Status;
    unsigned short      RtsDur;
    unsigned short      Length:15;
    unsigned short      LengExt:1;
    unsigned long           BufferAddress;
    unsigned short      BufferLength;
    unsigned short      Dur;

    unsigned long           NextDescAddress;
    unsigned long           PreviousDescAddress;
    unsigned long           Reserved0;
    unsigned long           Reserved1;

} TX_DESC, *PTX_DESC;


typedef struct _TX_STATUS_DESC {
    unsigned short      RC:8;
    unsigned short      RSVD0:7;
    unsigned short      TOK:1;

    unsigned short      RSVD1:12;
    unsigned short      LS:1;
    unsigned short      FS:1;
    unsigned short      RSVD2:1;
    unsigned short      OWN:1;

    unsigned long           RSVD3;

    unsigned long           BufferAddress;

    unsigned long           FrameLength:12;
    unsigned long           RSVD4:20;

    unsigned long           NextDescAddress;
    unsigned long           PreviousDescAddress;
    unsigned long           RSVD5;
    unsigned long           RSVD6;

} TX_STATUS_DESC, *PTX_STATUS_DESC;


//-------------------------------------------------------------------------
// Receive Buffer Descriptor (RBD)
//-------------------------------------------------------------------------
typedef struct _RX_DESC{
    unsigned short  length:12;
    unsigned short  CRCstatus:4;
    unsigned short  status;
    unsigned char   SignalQuality;
    unsigned char   RSSI;
    union {
        TAG_8021q       VLAN_TAG;
        struct {
            unsigned char AGC;
            unsigned char Decrypted:1;
            unsigned char Wakeup:1;
            unsigned char Reserved:6;
        };
    };
    NDIS_PHYSICAL_ADDRESS   BufferAddress;
} RX_DESC, *PRX_DESC;

#pragma pack()

//-------------------------------------------------------------------------
// 82557 PCI Register Definitions
// Refer To The PCI Specification For Detailed Explanations
//-------------------------------------------------------------------------
//- Register Offsets
#define PCI_VENDOR_ID_REGISTER      0x00    // PCI Vendor ID Register
#define PCI_DEVICE_ID_REGISTER      0x02    // PCI Device ID Register
#define PCI_CONFIG_ID_REGISTER      0x00    // PCI Configuration ID Register
#define PCI_COMMAND_REGISTER        0x04    // PCI Command Register
#define PCI_STATUS_REGISTER         0x06    // PCI Status Register
#define PCI_REV_ID_REGISTER         0x08    // PCI Revision ID Register
#define PCI_CLASS_CODE_REGISTER     0x09    // PCI Class Code Register
#define PCI_CACHE_LINE_REGISTER     0x0C    // PCI Cache Line Register
#define PCI_LATENCY_TIMER           0x0D    // PCI Latency Timer Register
#define PCI_HEADER_TYPE             0x0E    // PCI Header Type Register
#define PCI_BIST_REGISTER           0x0F    // PCI Built-In SelfTest Register
#define PCI_BAR_0_REGISTER          0x10    // PCI Base Address Register 0
#define PCI_BAR_1_REGISTER          0x14    // PCI Base Address Register 1
#define PCI_BAR_2_REGISTER          0x18    // PCI Base Address Register 2
#define PCI_BAR_3_REGISTER          0x1C    // PCI Base Address Register 3
#define PCI_BAR_4_REGISTER          0x20    // PCI Base Address Register 4
#define PCI_BAR_5_REGISTER          0x24    // PCI Base Address Register 5
#define PCI_SUBVENDOR_ID_REGISTER   0x2C    // PCI SubVendor ID Register
#define PCI_SUBDEVICE_ID_REGISTER   0x2E    // PCI SubDevice ID Register
#define PCI_EXPANSION_ROM           0x30    // PCI Expansion ROM Base Register
#define PCI_INTERRUPT_LINE          0x3C    // PCI Interrupt Line Register
#define PCI_INTERRUPT_PIN           0x3D    // PCI Interrupt Pin Register
#define PCI_MIN_GNT_REGISTER        0x3E    // PCI Min-Gnt Register
#define PCI_MAX_LAT_REGISTER        0x3F    // PCI Max_Lat Register
#define PCI_NODE_ADDR_REGISTER      0x40    // PCI Node Address Register



#endif  // _RTL8180_REG_H

