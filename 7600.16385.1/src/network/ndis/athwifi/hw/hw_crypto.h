/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    hw_crypto.h

Abstract:
    Contains defines used for receive functionality 
    in the HW layer
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    09-04-2007    Created

Notes:

--*/
#pragma once

// Implementation in hw_ccmp.c
NDIS_STATUS
HwDecryptCCMP(
    __in  PNICKEY                 Key,
    __inout_bcount(BufferLength)  PUCHAR                  DataBuffer,
    __in  ULONG                   BufferLength
    );

// Implementation in hw_ccmp.c
NDIS_STATUS
HwEncryptCCMP(
    PNICKEY                     Key,
    PNET_BUFFER                 NetBuffer
    );

NDIS_STATUS
HwCalculateTxMIC(
    __in  PNET_BUFFER             NetBuffer,
    __in  UCHAR                   Priority,
    __in_bcount(8)  PUCHAR                  MICKey,
    __out_bcount(HW_MIC_LENGTH) PUCHAR                  MICData
    );

NDIS_STATUS
HwCalculateRxMIC(
    __in  PHW_RX_MSDU             Msdu,
    __in  UCHAR                   Priority,
    __in_bcount(8)  PUCHAR                  MICKey,
    __out_bcount(HW_MIC_LENGTH) PUCHAR                  MICData
    );

