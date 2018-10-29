/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Hw_Decrypt.h

Abstract:
    Hw layer software-based CCMP encryption and decryption 
    function prototypes
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/

NDIS_STATUS
HwEncryptCCMP(
    PNICKEY pNicKey,
    PNET_BUFFER NetBuffer
    );
    
NDIS_STATUS
HwDecryptCCMP(
    PNICKEY pNicKey,
    PUCHAR  data,
    ULONG   size
    );
