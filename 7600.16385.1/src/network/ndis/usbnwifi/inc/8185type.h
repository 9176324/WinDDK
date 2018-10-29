/****************************************************************************
** COPYRIGHT (C) Realtek Semiconductor Corp.,                              **
** THIS FILE IS PART OF THE Realtek RTL8180/RTL8185 Adapter driver         **
****************************************************************************/
#ifndef _RTL8185_TYPE_H_
#define _RTL8185_TYPE_H_

#define MAX_CHANNEL_NUMBER					200

#define NUM_11B_CHANNEL						14
#define NUM_11A_CHANNEL						46

typedef enum _WIRELESS_MODE {
	WIRELESS_MODE_A = 0x01,
	WIRELESS_MODE_B = 0x02,
	WIRELESS_MODE_G = 0x04,
	WIRELESS_MODE_AUTO = 0x08,
} WIRELESS_MODE;

// Structs
typedef struct _WIRELESS_MODE_SETTING
{
	UCHAR	SIFS_Timer;
	UCHAR	DIFS_Timer;
	UCHAR	SlotTimeTimer;
	UCHAR	EIFS_Timer;

	UCHAR	CWminIndex;
	UCHAR	CWmaxIndex;
} WIRELESS_MODE_SETTING;

typedef struct _BRSR_AR_STRUCT
{
	USHORT		BRSR_Content;
	UCHAR		RR_Content;
} BRSR_AR_STRUCT;

typedef enum _CONTROL_RL {
	UNIVERSAL_CONTROL_RL = 0x00,
	PACKET_CONTROL_RL = 0x01,
} CONTROL_RL;

typedef enum _CONTROL_AGC {
	UNIVERSAL_CONTROL_AGC = 0x00,
	PACKET_CONTROL_AGC = 0x01,
} CONTROL_AGC;

typedef enum _CONTROL_ANTENNA {
	UNIVERSAL_CONTROL_ANTENNA = 0x00,
	PACKET_CONTROL_ANTENNA = 0x01,
} CONTROL_ANTENNA;

typedef enum _CONTROL_CW {
	UNIVERSAL_CONTROL_CW = 0x00,
	PACKET_CONTROL_CW = 0x01,
} CONTROL_CW;

typedef struct _TX_DESC_8185 {
	// DWORD 0
	unsigned short		FrameLength:12;
	unsigned short		Reserved0:3;
	unsigned short		NoEncrypt:1;
	unsigned short		Status;
	// DWORD 1
	unsigned short		RtsDur;
	unsigned short		Length:15;
	unsigned short		LengExt:1;
	// DWORD 2
	unsigned long		BufferAddress;
	// DWORD 3
	unsigned short		BufferLength:12;
	unsigned short		Reserved1:4;
	unsigned short		Reserved2;
	// DWORD 4
	unsigned long		NextDescAddress;
	// DWORD 5
	unsigned short		CWMin:4;
	unsigned short		CWMax:4;
	unsigned short		RetryLimit:8;
	unsigned short		AGC:8;
	unsigned short		Antenna:1;
	unsigned short		Reserved3:2;
	unsigned short		RateFallbackLimit:5;
	// DWORD 6
	unsigned long		Reserved4;
	// DWORD 7
	unsigned long		Reserved5;
} TX_DESC_8185, *PTX_DESC_8185;
//

#endif

