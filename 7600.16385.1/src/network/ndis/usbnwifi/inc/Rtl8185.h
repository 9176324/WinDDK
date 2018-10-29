/****************************************************************************
** COPYRIGHT (C) Realtek Semiconductor Corp.,                              **
** THIS FILE IS PART OF THE Realtek RTL8180/RTL8185 Adapter driver         **
****************************************************************************/
#define SIFS								0xB4
#define DIFS								0xB5
#define SLOT_TIME						0xB6
#define EIFS								0x35
#define CW_VALUE						0xBD


#define AnaParm2						0x60
//#define AR								0x34			// Ack rate
#define RR								0x34			// Ack rate

#define CW_CONFIG						0xBC

#define	GPO 							0x90
#define	GPE								0x91
#define	GPI								0x92

#define TXAGC_ANTSEL_CONFIG			0x9C
#define CCK_TXAGC						0x9D
#define OFDM_TXAGC						0x9E
#define ANTSEL							0x9F
#define AUTO_RATE_FALLBACK_CONTROL	0xBE

#define	TCR_SW_PLCP_LENGTH_LENGTH_EXT_OFFSET	24
#define TCR_NO_PROBE_RSP_TIMESTAMP_OFFSET		29

#define CAM_NONE				0x0
#define CAM_WEP40				0x01
#define CAM_TKIP				0x02
#define CAM_AES					0x04
#define CAM_WEP104				0x05

#define RWCAM		0xa0			// Software read/write CAM config
#define WCAMI		0xa4			// Software write CAM input content

#define TOTAL_CAM_ENTRY 	16
#define CAM_CONTENT_COUNT 6

#define EEPROM_11G_CHANNEL_OFDM_TX_POWER_LEVEL_OFFSET	0x40
#define EEPROM_11A_CHANNEL_TX_POWER_LEVEL_OFFSET			0x4e


VOID
Init8185(
	__in	PNIC		pNic
	);

void
SwWirelessMode(
	__in	PNIC		pNic,
	UCHAR		WirelessMode
	);

void
UpdateWirelessModeSetting(
	__in	PNIC		pNic,
	UCHAR		WirelessMode
	);

void
HwConfigure8185(
	__in	PNIC		pNic
	);

VOID
Set8185TxDesc(
	__in	PNIC		pNic,
	__in	PVOID		pDesc,
	__in	PUCHAR		FirstBuf,
	__in	USHORT		Rate
	);

VOID
ReadOFDMTxPowerFromEEPROM(
	__in	PNIC		pNic
	);

VOID
UpdateInitialGain(
	__in	PNIC		pNic
	);


