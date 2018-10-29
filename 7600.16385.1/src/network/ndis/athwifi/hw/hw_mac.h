/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    hw_mac.h

Abstract:
    Contains defines used for MAC functionality 
    in the HW layer
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/
#pragma once


NDIS_STATUS
HwSetATIMWindow(
    __in  PHW                     Hw,
    __in  ULONG                   Value
    );

VOID
HwUpdateUnicastCipher(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         HwMac
    );

NDIS_STATUS
HwSetPowerMgmtMode(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PDOT11_POWER_MGMT_MODE  PMMode
    );

NDIS_STATUS
HwUpdatePacketFilter(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         HwMac
    );

NDIS_STATUS
HwUpdateMulticastAddressList(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         HwMac
    );

/** Add the key to the NIC */
NDIS_STATUS
HwAddKeyEntry(
    __in  PHW                     Hw,
    __in  PHW_KEY_ENTRY           KeyEntry
    );

VOID
HwUpdateKeyEntry(
    __in  PHW                     Hw,
    __in  PHW_KEY_ENTRY           KeyEntry
    );

VOID
HwRemoveKeyEntry(
    __in  PHW                     Hw,
    __in  PHW_KEY_ENTRY           KeyEntry
    );

UCHAR
HwQueryDefaultKeyMask(
    __in  PHW_MAC_CONTEXT         HwMac
    );

VOID
HwSetDefaultKeyId(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  ULONG                   KeyId
    );

VOID
HwDeleteKeyFromContext(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  UCHAR                   KeyIndex,
    __in  BOOLEAN                 fProgramHardware
    );

VOID
HwDeleteAllKeysFromContext(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 fProgramHardware
    );

VOID
HwDeleteAllKeysFromHw(
    __in  PHW_MAC_CONTEXT         HwMac
    );

NDIS_STATUS
HwSetKeyInContext(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PHW_KEY_ENTRY           KeyEntry,
    __in  UCHAR                   KeyID,
    __in  BOOLEAN                 Persistent,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId,
    __in  ULONG                   KeyLength,
    __in_bcount(KeyLength)  PUCHAR                  KeyValue,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
HwSetDefaultKeyInContext(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr,
    __in  UCHAR                   KeyID,
    __in  BOOLEAN                 Persistent,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId,
    __in  ULONG                   KeyLength,
    __in_bcount(KeyLength)  PUCHAR                  KeyValue,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS 
HwSetSoftwareDefaultKeyInContext(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr,
    __in  UCHAR                   KeyID,
    __in  BOOLEAN                 Persistent,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId,
    __in  ULONG                   KeyLength,
    __in_bcount(KeyLength)  PUCHAR                  KeyValue
    );

NDIS_STATUS 
HwSetKeyMappingKeyInContext(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in_bcount(DOT11_ADDRESS_SIZE)  DOT11_MAC_ADDRESS       MacAddr,
    __in  DOT11_DIRECTION         Direction,
    __in  BOOLEAN                 Persistent,
    __in  DOT11_CIPHER_ALGORITHM  AlgoId,
    __in  ULONG                   KeyLength,
    __in_bcount(KeyLength)  PUCHAR                  KeyValue,
    __in  BOOLEAN                 fProgramHardware
    );

NDIS_STATUS
HwUpdateOperationMode(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         HwMac
    );


NDIS_STATUS
HwUpdateBSSType(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         HwMac
    );

NDIS_STATUS
HwUpdateBSSID(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         HwMac
    );


NDIS_STATUS
HwSetBeaconPeriod(
    __in  PHW                     Hw,
    __in  ULONG                   BeaconPeriod
    );

NDIS_STATUS
HwUpdateLinkState(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         HwMac
    );

NDIS_STATUS
HwUpdateAssociateId(
    __in  PHW                     Hw,
    __in  USHORT                  AssociateId
    );

VOID
HwSetBeaconIE(
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PVOID                   pBeaconIEBlob,
    __in  ULONG                   uBeaconIEBlobSize
    );

VOID
HwPopulateBeacon(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         BeaconMac,
    __in  PHW_TX_MSDU             BeaconMsdu
    );

NDIS_STATUS
HwSetupBeacon(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         BeaconMac
    );

VOID
HwEnableBSSBeacon(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         HwMac
    );

VOID
HwDisableBSSBeacon(
    __in  PHW                     Hw
    );

NDIS_STATUS
HwStartBSS(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 BeaconEnabled
    );


VOID
HwStopBSS(
    __in  PHW                     Hw
    );

VOID
HwPauseBSS(
    __in  PHW                     Hw
    );

VOID
HwResumeBSS(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  BOOLEAN                 BeaconEnabled
    );


VOID
HwPrepareToJoinBSS(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         HwMac
    );

NDIS_STATUS
HwProcessProbeRequest(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         MacContext,
    __in  PHW_RX_MPDU             Mpdu
    );

NDIS_STATUS
HwSendProbeRequest(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         MacContext,
    __in  PDOT11_MAC_ADDRESS      BSSID,
    __in_opt  PDOT11_SSID             SSID,
    __in_bcount(ScanAppendIEByteArrayLength)  PUCHAR                  ScanAppendIEByteArray,
    __in  USHORT                  ScanAppendIEByteArrayLength
    );

NDIS_STATUS
HwStartScan(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         HwMac,
    __in  PMP_SCAN_REQUEST        ScanRequest
    );

VOID
HwCancelScan(
    __in  PHW                     Hw
    );

NDIS_STATUS 
HwScanChannelSwitchComplete(
    __in  PHW                     Hw,
    __in_opt  PHW_MAC_CONTEXT         MacContext,
    __in  PVOID                   Data
    );

NDIS_STATUS 
HwScanChannelRestoreComplete(
    __in  PHW                     Hw,
    __in  PHW_MAC_CONTEXT         MacContext,
    __in  PVOID                   Data
    );

NDIS_TIMER_FUNCTION HwScanTimer;


