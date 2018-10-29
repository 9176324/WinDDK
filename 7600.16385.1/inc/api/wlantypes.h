/*++

    Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    wlantypes.h

Abstract:

    Structures used to hold wlan information.

--*/

#ifndef __WLANTYPES_H__
#define __WLANTYPES_H__

typedef enum _DOT11_BSS_TYPE {
    dot11_BSS_type_infrastructure = 1,
    dot11_BSS_type_independent = 2,
    dot11_BSS_type_any = 3
} DOT11_BSS_TYPE, * PDOT11_BSS_TYPE;

#define DOT11_SSID_MAX_LENGTH   32      // 32 bytes
typedef struct _DOT11_SSID {
#ifndef __midl
    __range(0,32)
#endif
    ULONG uSSIDLength;
    UCHAR ucSSID[DOT11_SSID_MAX_LENGTH];
} DOT11_SSID, * PDOT11_SSID;


// DOT11_AUTH_ALGO_LIST
#ifdef __midl
// use the 4-byte enum
typedef [v1_enum] enum _DOT11_AUTH_ALGORITHM {
#else
typedef enum _DOT11_AUTH_ALGORITHM {
#endif
    DOT11_AUTH_ALGO_80211_OPEN = 1,
    DOT11_AUTH_ALGO_80211_SHARED_KEY = 2,
    DOT11_AUTH_ALGO_WPA = 3,
    DOT11_AUTH_ALGO_WPA_PSK = 4,
    DOT11_AUTH_ALGO_WPA_NONE = 5,               // used in NatSTA only
    DOT11_AUTH_ALGO_RSNA = 6,
    DOT11_AUTH_ALGO_RSNA_PSK = 7,
    DOT11_AUTH_ALGO_IHV_START = 0x80000000,
    DOT11_AUTH_ALGO_IHV_END = 0xffffffff
} DOT11_AUTH_ALGORITHM, * PDOT11_AUTH_ALGORITHM;

#define DOT11_AUTH_ALGORITHM_OPEN_SYSTEM        DOT11_AUTH_ALGO_80211_OPEN
#define DOT11_AUTH_ALGORITHM_SHARED_KEY         DOT11_AUTH_ALGO_80211_SHARED_KEY
#define DOT11_AUTH_ALGORITHM_WPA                DOT11_AUTH_ALGO_WPA
#define DOT11_AUTH_ALGORITHM_WPA_PSK            DOT11_AUTH_ALGO_WPA_PSK
#define DOT11_AUTH_ALGORITHM_WPA_NONE           DOT11_AUTH_ALGO_WPA_NONE
#define DOT11_AUTH_ALGORITHM_RSNA               DOT11_AUTH_ALGO_RSNA
#define DOT11_AUTH_ALGORITHM_RSNA_PSK           DOT11_AUTH_ALGO_RSNA_PSK

// Cipher algorithm Ids (for little endian platform)
#ifdef __midl
// use the 4-byte enum
typedef [v1_enum] enum _DOT11_CIPHER_ALGORITHM {
#else
typedef enum _DOT11_CIPHER_ALGORITHM {
#endif
    DOT11_CIPHER_ALGO_NONE = 0x00,
    DOT11_CIPHER_ALGO_WEP40 = 0x01,
    DOT11_CIPHER_ALGO_TKIP = 0x02,
    DOT11_CIPHER_ALGO_CCMP = 0x04,
    DOT11_CIPHER_ALGO_WEP104 = 0x05,
    DOT11_CIPHER_ALGO_WPA_USE_GROUP = 0x100,
    DOT11_CIPHER_ALGO_RSN_USE_GROUP = 0x100,
    DOT11_CIPHER_ALGO_WEP = 0x101,
    DOT11_CIPHER_ALGO_IHV_START = 0x80000000,
    DOT11_CIPHER_ALGO_IHV_END = 0xffffffff
} DOT11_CIPHER_ALGORITHM, * PDOT11_CIPHER_ALGORITHM;

typedef struct DOT11_AUTH_CIPHER_PAIR {
    DOT11_AUTH_ALGORITHM AuthAlgoId;
    DOT11_CIPHER_ALGORITHM CipherAlgoId;
} DOT11_AUTH_CIPHER_PAIR, * PDOT11_AUTH_CIPHER_PAIR;


#endif // __WLANTYPES_H__

