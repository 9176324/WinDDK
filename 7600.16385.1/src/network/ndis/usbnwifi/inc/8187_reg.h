#ifndef __INC_HAL8187REG_H
#define __INC_HAL8187REG_H

//----------------------------------------------------------------------------
//       8187 Regsiter offset definition
//----------------------------------------------------------------------------
#define IDR0                0xFF00          /* Ethernet hardware address. */
#define IDR4                0xFF04
#define MAR0                0xFF08          /* Multicast filter. */
#define MAR4                0xFF0c
#define RFPC                0xFF10          /* Receive FIFO Packet Count Register */
#define TFPC                0xFF12          /* Transmit FIFO Packet Count Register */
#define BQREQ           0xFF13          /* Beacon Queue Request Register */
#define TSFR                0xFF18          // Timing synchronization function register

#define BRSR                0xFF2C          // Basic rate set
#define BSSIDR          0xFF2E          // BSSID
#define RR              0xFF34          // Response Rate
#define EIFS                0xFF35          // 
#define CMDR                0xFF37          // Command register
#define IMR             0xFF3C
#define ISR             0xFF3E

#define TCR             0xFF40
#define RCR             0xFF44
#define TINT                0xFF48          /* A general-purpose counter. */

#define CR9346          0xFF50
#define CONFIG0         0xFF51
#define CONFIG1         0xFF52
#define CONFIG2         0xFF53
#define ANA_PARM        0xFF54
#define MSR             0xFF58          //Media status register
#define CONFIG3         0xFF59
#define Config4         0xFF5A
#define TestR           0xFF5B          // Media status register 2
#define PSR             0xFF5E          // PageSelect register
#define SCR             0xFF5F          // Security configuration register
#define BcnIntv         0xFF70          // Beacon interval register(2 bytes)
#define AtimWnd         0xFF72          // Atim Window register (2 bytes)
#define BcnIntTime      0xFF74          // Time to trigger interrupt before beacon time
//2004/08/23, kcwu
#define AtimtrItv           0xFF76
//
#define LIR             0xFF76          // Listen interval Register
#define CSCounter       0xFF79          // Carrier Sense Counter
#define BBAddr          0xFF7C          // Phy register address register
#define PhyDataW        0xFF7D          // Phy register data write
#define PhyDataR            0xFF7E          // Phy register data read

#define RFPinsOutput        0xFF80      
#define RFPinsEnable        0xFF82
#define RFPinsSelect        0xFF84
#define RFPinsInput     0xFF86

#define GPIOPinsOutput      0xFF90
#define GPIOPinsEnable      0xFF91
#define GPIOPinsInput       0xFF92



#define DK0             0xFF90          // default key 0 register 
#define DK1             0xFFA0          // default key 0 register       
#define DK2             0xFFB0          // default key 0 register 
#define DK3             0xFFC0          // default key 0 register 
#define CONFIG5         0xFFD8          //
#define CWR             0xFFDC          // Contention window register
#define RetryCTR            0xFFDE          // Retry Count register

#define WolPtrnMsk0     0xFF84
#define WolPtrnMsk1     0xFF8C
#define WolPtrnMsk2     0xFF94
#define WolPtrnMsk3     0xFFA4
#define WolPtrnMsk4     0xFFB4
#define WolPtrnCRC0     0xFFC4


#define TXAGC_CTL       0xFF9C
#define CCK_TXAGC       0xFF9D
#define OFDM_TXAGC      0xFF9E
#define ANTSEL          0xFF9F
#define SIFS                0xFFB4
#define DIFS                0xFFB5
#define SLOT_TIME       0xFFB6
#define usTIME          0xFFB7
#define CW_CONFIG       0xFFBC
#define CW_VALUE        0xFFBD
#define RATE_FALLBACK_CTL   0xFFBE
#define CCK_FALSE_ALARM 0xFFD0
#define OFDM_FALSE_ALARM    0xFFD2

#define Tally_Cnt           0xFFFA
#define Tally_Sel           0xFFFC

// slot time for 11g
#define SHORT_SLOT_TIME         9
#define NON_SHORT_SLOT_TIME     20

// DIFS under different wireless mode. Value given by PJ. Added by Annie, 2005-12-02.
#define DIFS_SHORT_SLOT_TIME        14  // 0xE
#define DIFS_LONG_SLOT_TIME_BG  36  // 0x24
#define DIFS_A                      20  // 0x14

// EIFS under different wireless mode. Value given by PJ and sync with 8185. Added by Annie, 2005-12-02.
#define EIFS_CutOneDIFS_ShortSlot   79  // 14=0x0E: Will cause throughput down to 2M with 8186 if CCK rate is not 6M.(1Mor11M?). To prevent this case, we modify it to 79. Annie, 2005-12-16.
#define EIFS_CutOneDIFS_LongSlot    79  // 0x4F
//----------------------------------------------------------------------------
//       8187 BQREQ bits                        (Offset 0xFF13, 8bit)
//----------------------------------------------------------------------------
#define BQREQ_Bqchg_req     BIT7
#define BQREQ_ER            BIT6

//----------------------------------------------------------------------------
//       8187 BRSR bits                     (Offset 0xFF2c, 16bit)
//----------------------------------------------------------------------------
#define BRSR85_1M           BIT0
#define BRSR85_2M           BIT1
#define BRSR85_5_5M         BIT2
#define BRSR85_11M          BIT3
#define BRSR85_6M           BIT4
#define BRSR85_9M           BIT5
#define BRSR85_12M          BIT6
#define BRSR85_18M          BIT7
#define BRSR85_24M          BIT8
#define BRSR85_36M          BIT9
#define BRSR85_48M          BIT10
#define BRSR85_54M          BIT11


//----------------------------------------------------------------------------
//       8187 RR bits                       (Offset 0xFF34, 8bit)
//----------------------------------------------------------------------------
#define RR85_1M             0x00
#define RR85_2M             0x01
#define RR85_5_5M           0x02
#define RR85_11M            0x03
#define RR85_6M             0x04
#define RR85_9M             0x05
#define RR85_12M            0x06
#define RR85_18M            0x07
#define RR85_24M            0x08
#define RR85_36M            0x09
#define RR85_48M            0x0a
#define RR85_54M            0x0b



//----------------------------------------------------------------------------
//       8187 TCR bits                      (Offset 0xFF40, 32bit)
//----------------------------------------------------------------------------
#define TCR_noCRC           BIT16       // Supress CRC
#define TCR_LBK             BIT17       // loopback mode
#define TCR_noICV           BIT19       // Supress ICV
#define TCR_DisCW           BIT20       // Disable Contention Window
#define TCR_SAT             BIT24       // Enable Rate depedent ack timeout timer
#define TCR_HWVERID     (BIT25|BIT26|BIT27)     // Hardware Version ID
#define TCR_MXDMA_OFFSET    21


//----------------------------------------------------------------------------
//       8187 RCR bits                          (offset 0xFF44, 32 bits)
//----------------------------------------------------------------------------
#define RCR_AAP             BIT0                    // accept all physical address
#define RCR_APM             BIT1                    // accept physical match
#define RCR_AM              BIT2                    // accept multicast
#define RCR_AB              BIT3                    // accept broadcast
#define RCR_ACRC32          BIT5                    // accept error packet
#define RCR_9356SEL         BIT6
#define RCR_AICV                BIT12               // Accept ICV error packet
#define RCR_RXFTH0          (BIT13|BIT14|BIT15)     // Rx FIFO threshold
#define RCR_ADF             BIT18               // Accept Data(frame type) frame
#define RCR_ACF             BIT19               // Accept control frame
#define RCR_AMF             BIT20               // Accept management frame
#define RCR_ADD3            BIT21
#define RCR_APWRMGT     BIT22               // Accept power management packet
#define RCR_CBSSID          BIT23               // Accept BSSID match packet
#define RCR_ENMARP          BIT28               // enable mac auto reset phy
#define RCR_EnCS1           BIT29               // enable carrier sense method 1
#define RCR_EnCS2           BIT30               // enable carrier sense method 2
#define RCR_OnlyErlPkt      BIT31               // Rx Early mode is performed for packet size greater than 1536


//----------------------------------------------------------------------------
//       8187 Config1 Regsiter          (offset 0xFF52, 1 byte)
//----------------------------------------------------------------------------
#define Config1_LEDS1           BIT7            // LED pin definition
#define Config1_LEDS0           BIT6            //

//----------------------------------------------------------------------------
//       8187 Config2 Regsiter          (offset 0xFF53, 1 byte)
//----------------------------------------------------------------------------
#define Config2_LCK             BIT7            // Locked clock
#define Config2_ANT             BIT6            // Antenna diversity
#define Config2_PAPE_sign       BIT2            // PAPE sign
#define Config2_PAPE_time       (BIT1|BIT0) // PAPE time

//----------------------------------------------------------------------------
//       8187 MSR Regsiter          (offset 0xFF58, 1 byte)
//----------------------------------------------------------------------------
#define MSR_AP                  0x0C
#define MSR_INFRA               0x08
#define MSR_ADHOC               0x04
#define MSR_NOLINK              0x00

//----------------------------------------------------------------------------
//       8187 Config3 Regsiter          (offset 0xFF59, 1 byte)
//----------------------------------------------------------------------------
#define Config3_ParmEn          BIT6            // enable write to ANA_PARM
#define Config3_Magic           BIT5            // Enable Magic Packet Wakeup

//----------------------------------------------------------------------------
//       8187 Config4 Regsiter          (offset 0xFF5a, 1 byte)
//----------------------------------------------------------------------------
#define Config4_VCOPowerDown    BIT7
#define Config4_PowerOff            BIT6            // Turn ON/Off RF Power(RFMD)
#define Config4_PwrMgtMode      BIT5            // Nic in Power Save mode


#define DEFAULT_RX_FIFO_LIMIT           7
#define RCR_MXDMA_OFFSET                8
#define RCR_FIFO_OFFSET             13
#define RCR_AR                          0x0

// RTL8187_TODO: Check EEPROM data locations

//- EEPROM data locations
#define RTL8187_EEPROM_ID               0x8187
#define EEPROM_CHANNEL_PLAN         0x06
#define EEPROM_INTERFACE_SELECT     0x09
#define EEPROM_TX_POWER_BASE_OFFSET 0x0A
#define EEPROM_RF_CHIP_ID               0x0C
#define EEPROM_TX_POWER_LEVEL_0     0x2C
#define EEPROM_TX_POWER_LEVEL_1     0x7A
#define EEPROM_TX_POWER_LEVEL_2     0x36

#define EEPROM_CONFIG2                  0x18
#define EEPROM_RF_PARAM             0x08

#if 0
#define EEPROM_NODE_ADDRESS_BYTE_0  0x0E
#define EEPROM_CS_THRESHOLD         0x2F
#define EEPROM_VERSION                  0x3c                
#define EEPROM_CIS_DATA             0x80

#define EEPROM_FLAGS_WORD_3         3
#define EEPROM_FLAG_10MC                BIT0
#define EEPROM_FLAG_100MC               BIT1
#endif

//----------------------------------------------------------------------------
//       8187 (CMD) command register bits   (Offset 0xFF37, 8 bit)
//----------------------------------------------------------------------------
#define CR_RST          BIT4
#define CR_RE           BIT3
#define CR_TE           BIT2
#define CR_MulRW        BIT1

//----------------------------------------------------------------------------
//       8187 IMR/ISR bits                  (offset 0xFF3C/0xFF3E, 16 bits)
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
#define IMR_RDU         0x0010               // rx buffer overflow
#define IMR_TLPDER      0x0008          // transmit error
#define IMR_TLPDOK      0x0004          // transmit OK
#define IMR_RER         0x0002          // receive error, collided frame
#define IMR_ROK         0x0001          // Receive OK
#define IMR_EMPTY       0x0000          // no bits set in IMR

//----------------------------------------------------------------------------
//       8185 TXAGC_CTL bits                (Offset 0x9C, 8bit)
//----------------------------------------------------------------------------
#define TXAGC_CTL_PER_PACKET_TXAGC  0x01
#define TXAGC_CTL_PER_PACKET_ANT_SEL    0x02
#define TXAGC_CTL_FEEDBACK_ANT      0x04


//----------------------------------------------------------------------------
//       8185 CW_CONFIG bits                (Offset 0xBC, 8bit)
//----------------------------------------------------------------------------
#define CW_CONFIG_PER_PACKET_CW 0x01
#define CW_CONFIG_PER_PACKET_RL 0x02


//----------------------------------------------------------------------------
//       8185 RATE_FALLBACK_CTL bits                (Offset 0xBE, 8bit)
//----------------------------------------------------------------------------
#define RATE_FALLBACK_CTL_AUTO_STEP0    0x00
#define RATE_FALLBACK_CTL_ENABLE        0x80

//2004/08/18, kcwu, Security Registers ==>
//----------------------------------------------------------------------------
//       8185 WPA Config Regsiter (offset 0xb0, 1 byte)
//----------------------------------------------------------------------------
#define SCR_UseDK               0x01
#define SCR_TxSecEnable         0x02    
#define SCR_RxSecEnable         0x04
#define AESMSK1_DEFAULT         0x8f
#define AESMSK2_DEFAULT         0xC7    // [cam_access.txt] it should be 0xC7, not 0x7c. Annie, 2005-11-07.

//8185  Security
#define     RWCAM       0xffa0          // Software read/write CAM config
#define     WCAMI       0xffa4          // Software write CAM input content
#define     RCAMO       0xffa8          // Output value from CAM according to 0xa0 setting
#define     DCAM        0xffac          // Debug CAM Interface
#define     SECR        0xffb0          // Security configuration register
#define     AESMSK1 0xffb2          // AES Mask for frame control when computing AES MIC ,default = 0x8f
#define     AESMSK2 0xffb3          // AES Mask for frame control when computing AES MIC, default = 0xC7
#define     AESMSK3 0xffe2          // AES Mask for sequence control when computing AES MIC ,default = 0x00
#define     AESMSK4 0xffe3          // AES Mask for sequence control when computing AES MIC, default = 0x0f


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

typedef struct _TX_DESC_8187 {   //12 bytes
    // DWORD 1
    unsigned short      TPKTSIZE:12;
    unsigned short      RSVD0:3;
    unsigned short      NoEncrypt:1;
    unsigned short      SPLCP:1;
    unsigned short      MOREFRAG:1;
    unsigned short      CTSEN:1;
    unsigned short      RTSRATE:4;
    unsigned short      RTSEN:1;
    unsigned short      TXRATE:4;
    unsigned short      RSVD1:4;
    // DWORD 2
    unsigned short      RtsDur;
    unsigned short      Length:15;
    unsigned short      LengExt:1;
    // DWORD 3
    unsigned short      CWMIN:4;
    unsigned short      CWMAX:4;
    unsigned short      RETRY_LIMIT:8;
    unsigned short      AGC:8;
    unsigned short      ANTENNA:1;
    unsigned short      REVD:3;
    unsigned short      RATE_FALL_BACK_LIMIT:4;
} TX_DESC_8187, *PTX_DESC_8187;


typedef struct _RX_DESC_8187{
    ULONG  Length:12;
    ULONG  Reserved0:18;
    ULONG  EOR:1;
    ULONG  OWN:1;
    ULONG  Reserved1;
    ULONG  BufferAddress;
    ULONG  Reserved2;
} RX_DESC_8187, *PRX_DESC_8187;

typedef struct _RX_STATUS_DESC_8187{
    USHORT      Length:12;
    USHORT      ICV:1;
    USHORT      CRC32:1;
    USHORT      PWRMGT:1;
    USHORT      RSVD0:1;
    UCHAR      BAR:1;
    UCHAR      PAM:1;
    UCHAR      MAR:1;
    UCHAR      RSVD1:1;
    UCHAR      RXRATE:4;
    UCHAR      RSVD2:1;
    UCHAR      SPLCP:1;
    UCHAR      FOVF:1;
    UCHAR      RSVD3:5;
    UCHAR      SignalQuality;
    UCHAR      RSSI:7;
    UCHAR      ANTENNA:1;
    UCHAR      RXAGC;
    UCHAR      Decrypted:1;
    UCHAR      WAKEUP:1;
    UCHAR      RSVD4:6;

    // 2005.02.02, by rcnjko.
    //NDIS_PHYSICAL_ADDRESS BufferAddress;
    ULONG      TSFTL;
    ULONG      TSFTH;

} RX_STATUS_DESC_8187, *PRX_STATUS_DESC_8187;

#pragma pack()



#endif // #ifndef __INC_HAL8187REG_H


