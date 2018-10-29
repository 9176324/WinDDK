/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Sta_Interface.h

Abstract:
    Interface functions into Sta layer
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#ifndef _STATION_INTERFACE_H
#define _STATION_INTERFACE_H

//
// Forward declaration
//
typedef struct _ADAPTER         ADAPTER, *PADAPTER;
typedef struct _MP_TX_MSDU      MP_TX_MSDU, *PMP_TX_MSDU;
typedef struct _MP_RX_MSDU      MP_RX_MSDU, *PMP_RX_MSDU;
typedef struct _MP_RX_FRAGMENT  MP_RX_FRAGMENT, *PMP_RX_FRAGMENT;


typedef struct _NIC_TX_MSDU     NIC_TX_MSDU, *PNIC_TX_MSDU;
typedef struct _NIC_RX_MSDU     NIC_RX_MSDU, *PNIC_RX_MSDU;
typedef struct _NIC_RX_FRAGMENT NIC_RX_FRAGMENT, *PNIC_RX_FRAGMENT;
typedef struct _NIC             NIC, *PNIC;


typedef struct _STATION         STATION, *PSTATION;

/**
 * Called during initialization. Allocates and initializes the station 
 * structure. 
 * 
 * \param MiniportAdapterHandle MiniportHandle registered with Ndis
 * \param ppStation             Returns the allocated STATION structure
 * \param pAdapter              ADAPTER structure to use when calling
 *                              Mp11 functions
 * \param pNic                  NIC structure to use when calling
 *                              Hw11 functions
 * \return                      Appropriate NDIS_STATUS 
 * \sa Sta11FreeStation
 */
NDIS_STATUS
Sta11AllocateStation(
    __in  NDIS_HANDLE     MiniportAdapterHandle,
    __out PSTATION*       ppStation
    );

/**
 * Initializes the internal variables in the station. When this is called,
 * the NIC is not ready, station cannot yet call any Hw11 interfaces
 * 
 * \param pStation              The STATION structure that was previously
 *                              allocated
 * \param pAdapter              ADAPTER structure to use when calling
 *                              Mp11 functions
 * \param pNic                  NIC structure to use when calling
 *                              Hw11 functions
 * \return                      Appropriate NDIS_STATUS 
 * \sa Sta11StartStation, Sta11FreeStation
 */
NDIS_STATUS
Sta11InitializeStation(
    __in  PSTATION        pStation,
    __in  PADAPTER        pAdapter,
    __in  PNIC            pNic
    );

/**
 * Frees the STATION structure when miniport is being cleaned up. This
 * function is only called if Sta11AllocateStation succeeds. This function
 * cannot invoke any of the Hw11 functions. This function is invoked at
 * IRQL_PASSIVE
 * 
 * \param pStation      Previously allocated STATION structure
 * \sa Sta11AllocateStation
 */
VOID
Sta11FreeStation(
    __in  PSTATION        pStation
    );

/**
 * Informs the station that the NIC is ready and it can initialize
 * itself and initiate station functionality. This is called while
 * the miniport is still in the paused state and hence it must not
 * indicate any packets
 * 
 * \param pStation      STATION structure
 */
NDIS_STATUS
Sta11StartStation(
    __in  PSTATION        pStation
    );

/**
 * Informs the station that the NIC has already stopped and it should
 * stop and cleanup. When this is called, the NIC is already stopped
 * and interrupts disabled. This is called on halt or if initialize
 * has failed.
 * 
 * \param pStation      STATION structure
 * \param HaltAction    Reason for the stop. This corresponds to HaltAction
 *                      from MiniportHaltEx. NdisHaltDeviceInitializationFailed is
 *                      overloaded and used for Stop called due to miniport
 *                      initialize failure
 */
VOID
Sta11StopStation(
    __in  PSTATION        pStation,
    __in  NDIS_HALT_ACTION    HaltAction
    );

/**
 * If the STATION needs to read any configuration values from the registry and store
 * them, it can be done in this function. This function is called during MiniportInitialize.
 * The configuration handle passed in will NOT remain alive after this function ends so
 * do not try to use it later.
 *
 * \warning This function must NEVER fail. Station should always be able to use default
 * values. Initialization of the Station failing due to registry values being incorrect
 * is a security problem.
 * 
 * \param pStation              STATION structure
 * \param hConfigurationHandle  The handle to Configuration object returned by
 * NdisOpenConfiguration
 * \sa MpReadRegistryConfiguration
 */
VOID
Sta11ReadRegistryConfiguration(
    __in PSTATION         pStation,
    __in NDIS_HANDLE      hConfigurationHandle
    );

/**
 * Informs the station that the miniport is about to be reset (either 
 * NdisReset or OID_DOT11_RESET). This is called before the reset 
 * has actually begun. This function is invoked at IRQL_PASSIVE.
 * 
 * \param pStation      STATION structure
 * \param ResetType     The type/cause of the reset
 *
 */
VOID
Sta11ResetStep1(
    __in  PSTATION         pStation,
    __in  MP_RESET_TYPE    ResetType
    );

/**
 * Reset the station state. This is called during NdisReset and OID_DOT11_RESET. 
 * This function is invoked at IRQL_DISPATCH
 * 
 * \param pStation      STATION structure
 * \param ResetType     The type/cause of the reset
 *
 */
VOID
Sta11ResetStep2(
    __in  PSTATION         pStation,
    __in  MP_RESET_TYPE    ResetType
    );

/**
 * Informs the station that the miniport has completed the reset (either 
 * NdisReset or OID_DOT11_RESET). This is called after the reset 
 * has completed. This function is invoked at IRQL_PASSIVE
 * 
 * \param pStation      STATION structure
 * \param ResetType     The type/cause of the reset
 *
 */
VOID
Sta11ResetStep4(
    __in  PSTATION         pStation,
    __in  MP_RESET_TYPE    ResetType
    );

/**
 * Informs the station of a packet receive. This is invoked in the 
 * context of a receive handler DPC and invoked for every valid receive
 * packet.
 * 
 * \param pStation      STATION structure      
 * \param pMpFragment   MP_RX_FRAGMENT structure
 * \param pNicFragment  NIC_RX_FRAGMENT structure to use when invoking HW functions
 * \param pFragmentHdr  pointer to DOT11_MAC_HEADER
 * \param FragmentSize  Length of the PACKET
 * \return NDIS_STATUS whether the packet is acceptable or not
 */
NDIS_STATUS
Sta11PacketReceived(
    __in  PSTATION                        pStation,
    __in  PMP_RX_FRAGMENT                 pMpFragment,
    __in  PNIC_RX_FRAGMENT                pNicFragment,
    __in  PDOT11_MAC_HEADER               pFragmentHdr,
    __in  USHORT                          uFragmentSize
    );
    
/**
 * Invoked on OID_DOT11_ENUM_BSS_LIST to populate the structure with
 * discovered access points information
 * 
 * \param pStation              STATION structure
 * \param pDot11ByteArray       DOT11_BYTE_ARRAY to populate
 * \param TotalLength           Total length of the buffer
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11EnumerateBSSList(
    __in  PSTATION         pStation,
    __inout PDOT11_BYTE_ARRAY        pDot11ByteArray,
    __in  ULONG            TotalLength
    );

/**
 * Invoked on OID_DOT11_FLUSH_BSS_LIST to clear discovered access
 * points
 *
 * \param pStation              STATION structure
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11FlushBSSList(
    __in  PSTATION        pStation
    );

/**
 * Invoked on OID_DOT11_DESIRED_BSS_TYPE query 
 * 
 * \param pStation              STATION structure
 * \param bssType               Pointer to the BSS type to be queried
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryDesiredBSSType(
    __in  PSTATION        pStation,
    __in  PDOT11_BSS_TYPE bssType
    );

/**
 * Invoked on OID_DOT11_DESIRED_SSID_LIST query
 * 
 * \param pStation              STATION structure
 * \param pDot11SSIDList        Structure to populate
 * \param TotalLength           Total length of the structure
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryDesiredSSIDList(
    __in  PSTATION        pStation,
    __out PDOT11_SSID_LIST    pDot11SSIDList,
    __in  ULONG           TotalLength
    );

/**
 * Invoked on OID_DOT11_EXCLUDED_MAC_ADDRESS_LIST query.
 * 
 * \param pStation              STATION structure
 * \param pDot11MacAddrList     Structure to populate
 * \param TotalLength           Total length of the structure
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryExcludedMACAddressList(
    __in  PSTATION        pStation,
    __out_bcount(TotalLength) PDOT11_MAC_ADDRESS_LIST pDot11MacAddrList,
    __in __range(sizeof(DOT11_MAC_ADDRESS_LIST) - sizeof(DOT11_MAC_ADDRESS), 0x7FFFFFFF) ULONG           TotalLength
    );

/**
 * Invoked on OID_DOT11_QUERY_DESIRED_BSSID_LIST query
 * 
 * \param pStation              STATION structure
 * \param pDot11MacAddrList     Structure to populate
 * \param TotalLength           Total length of the structure
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryDesiredBSSIDList(
    __in  PSTATION        pStation,
    __out_bcount(TotalLength) PDOT11_BSSID_LIST   pDot11BSSIDList,
    __in __range(sizeof(DOT11_BSSID_LIST) - sizeof(DOT11_MAC_ADDRESS), 0x7FFFFFFF)  ULONG           TotalLength
    );

/**
 * Invoked on OID_DOT11_ENABLED_AUTHENTICATION_ALGORITHM query
 * 
 * \param pStation              STATION structure
 * \param pAuthAlgoList         Structure to populate
 * \param TotalLength           Total length of the structure
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryEnabledAuthenticationAlgorithm(
    __in  PSTATION        pStation,
    __out PDOT11_AUTH_ALGORITHM_LIST  pAuthAlgoList,
    __in  ULONG           TotalLength
    );

/**
 * Invoked to OID_DOT11_SUPPORTED_UNICAST_ALGORITHM_PAIR query
 * 
 * \param pStation              STATION structure
 * \param pAuthCipherList       Structure to populate
 * \param TotalLength           Total length of the structure
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QuerySupportedUnicastAlgorithmPair(
    __in  PSTATION        pStation,
    __out_bcount(TotalLength) PDOT11_AUTH_CIPHER_PAIR_LIST    pAuthCipherList,
    __in __range(sizeof(DOT11_AUTH_CIPHER_PAIR_LIST) - sizeof(DOT11_AUTH_CIPHER_PAIR), 0x7FFFFFFF) ULONG           TotalLength
    );

/**
 * Invoked on OID_DOT11_SUPPORTED_MULTICAST_ALGORITHM_PAIR query
 * 
 * \param pStation              STATION structure
 * \param pAuthCipherList       Structure to populate
 * \param TotalLength           Total length of the structure
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QuerySupportedMulticastAlgorithmPair(
    __in  PSTATION        pStation,
    __out_bcount(TotalLength) PDOT11_AUTH_CIPHER_PAIR_LIST    pAuthCipherList,
    __in __range(sizeof(DOT11_AUTH_CIPHER_PAIR_LIST) - sizeof(DOT11_AUTH_CIPHER_PAIR), 0x7FFFFFFF) ULONG           TotalLength
    );

/**
 * Invoked on OID_DOT11_ENABLED_UNICAST_ALGORITHM_PAIR query
 * 
 * \param pStation              STATION structure
 * \param pAuthCipherList       Structure to populate
 * \param TotalLength           Total length of the structure
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryEnabledUnicastCipherAlgorithm(
    __in  PSTATION        pStation,
    __out PDOT11_CIPHER_ALGORITHM_LIST  pAuthCipherList,
    __in  ULONG           TotalLength
    );
    
/**
 * Invoked on OID_DOT11_ENABLED_MULTICAST_ALGORITHM_PAIR query
 * 
 * \param pStation              STATION structure
 * \param pAuthCipherList       Structure to populate
 * \param TotalLength           Total length of the structure
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryEnabledMulticastCipherAlgorithm(
    __in  PSTATION        pStation,
    __out PDOT11_CIPHER_ALGORITHM_LIST  pAuthCipherList,
    __in  ULONG           TotalLength
    );

/**
 * Invoked on OID_DOT11_CIPHER_DEFAULT_KEY_ID query
 * 
 * \param pStation              STATION structure
 * \param keyID                 caller allocated ULONG pointer to store the default keyID.
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryCipherDefaultKeyID(
    __in  PSTATION pStation,
    __in  PULONG keyID
    );

/**
 * Invoked on OID_DOT11_EXTSTA_CAPABILITY query
 * 
 * \param pStation              STATION structure
 * \param pDot11ExtStaCap       Structure to populate
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryExtStaCapability(
    __in  PSTATION        pStation,
    __out PDOT11_EXTSTA_CAPABILITY   pDot11ExtStaCap
    );

/**
 * Invoked on OID_DOT11_EXCLUDE_UNENCRYPTED query. 
 * 
 * \param pStation              STATION structure
 * \param param                 caller allocated BOOLEAN pointer to store the parameter value.
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryExcludeUnencrypted(
    __in  PSTATION pStation,
    __in  PBOOLEAN param
    );

/**
 * Invoked on OID_DOT11_PRIVACY_EXEMPTION_LIST query. The MP functions
 * have already verified that buffer is big enough to
 * contain NumEntries privacy exemption list
 * 
 * 
 * \param pStation              STATION structure
 * \param list                  Privacy exemption list to populate
 * \param TotalLength           Total length of the structure
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryPrivacyExemptionList(
    __in  PSTATION                      pStation,
    __out_bcount(TotalLength) PDOT11_PRIVACY_EXEMPTION_LIST list,
    __in __range(sizeof(DOT11_PRIVACY_EXEMPTION_LIST) - sizeof(DOT11_PRIVACY_EXEMPTION), 0x7FFFFFFF) ULONG                         TotalLength
    );

/**
 * Invoked on OID_DOT11_DATA_RATE_MAPPING_TABLE query
 * 
 * \param pStation              STATION structure
 * \param pDataRateMappingTable Structure to populate
 * \param TotalLength           Total length of structure
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryDataRateMappingTable(
    __in  PSTATION        pStation,
    __out PDOT11_DATA_RATE_MAPPING_TABLE  pDataRateMappingTable,
    __in  ULONG           TotalLength
    );

/**
 * Invoked on OID_DOT11_POWER_MGMT_REQUEST query. 
 * 
 * \param pStation              STATION structure
 * \param PowerSavingLevel      Caller allocated ULONG pointer to store the power saving level
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryPowerMgmtRequest(
    __in  PSTATION pStation,
    __in  PULONG PowerSavingLevel
    );


/**
 * Invoked on OID_DOT11_IBSS_PARAMS query. 
 * 
 * \param pStation              STATION structure
 * \param InformationBuffer     Information buffer to fill the DOT11_IBSS_PARAMS structure into. This
 *                              structure is atleast DOT11_IBSS_PARAMS bytes long
 * \param InformationBufferLength   Length of information buffer
 * \param BytesWritten          Number of into Information Buffer
 * \param BytesNeeded           Number of bytes needed for filling DOT11_IBSS_PARAMS
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS 
Sta11QueryIBSSParameters(
    __in  PSTATION pStation,
    __in  PVOID InformationBuffer,
    __in  ULONG InformationBufferLength,
    __inout PULONG BytesWritten,
    __inout PULONG BytesNeeded
    );

/**
 * Invoked on OID_DOT11_HARDWARE_PHY_STATE query. 
 * 
 * \param pStation              STATION structure
 * \param pState                caller allocated BOOLEAN pointer to store the hardware PHY state 
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryHardwarePhyState(
    __in  PSTATION        pStation,
    __out PBOOLEAN        pState
    );

/**
 * Invoked on OID_DOT11_DESIRED_PHY_LIST query. 
 * 
 * \param pStation              STATION structure
 * \param PhyIdList             PHY ID list structure to populate
 * \param TotalLength           Total length of structure
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryDesiredPhyList(
    __in  PSTATION        pStation,
    __out PDOT11_PHY_ID_LIST  PhyIdList,
    __in  ULONG           TotalLength
    );

/**
 * Invoked on OID_DOT11_ACTIVE_PHY_LIST query. 
 * 
 * \param pStation              STATION structure
 * \param PhyIdList             PHY ID list structure to populate
 * \param TotalLength           Total length of structure
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryActivePhyList(
    __in  PSTATION        pStation,
    __out PDOT11_PHY_ID_LIST  PhyIdList,
    __in  ULONG           TotalLength
    );

/**
 * Invoked on OID_DOT11_CURRENT_PHY_ID query. 
 * 
 * \param pStation              STATION structure
 * \param PhyId                 caller allocated ULONG pointer to store the current Phy ID
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryCurrentPhyId(
    __in  PSTATION        pStation,
    __in  PULONG          PhyId
    );

/**
 * Invoked on OID_DOT11_PMKID_LIST query. 
 * 
 * \param pStation              STATION structure
 * \param PMKIDList             PMKID list structure to populate
 * \param TotalLength           Total length of structure
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryPMKIDList(
    __in  PSTATION        pStation,
    __out_bcount(TotalLength) PDOT11_PMKID_LIST  PMKIDList,
    __in __range(sizeof(DOT11_PMKID_LIST) - sizeof(DOT11_PMKID_ENTRY), 0x7FFFFFFF) ULONG           TotalLength
    );

/**
 * Invoked on OID_DOT11_MEDIA_STREAMING_ENABLED query. 
 * 
 * \param pStation              STATION structure
 * \param mediaStreamingEnabled caller allocated BOOLEAN pointer to store the current setting
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryMediaStreamingOption(
    __in  PSTATION        pStation,
    __out PBOOLEAN        mediaStreamingEnabled
    );

/**
 * Invoked on OID_DOT11_UNREACHABLE_DETECTION_THRESHOLD query. 
 * 
 * \param pStation              STATION structure
 * \param threshold             caller allocated ULONG pointer to store the current threshold
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryUnreachableDetectionThreshold(
    __in  PSTATION        pStation,
    __in  PULONG          threshold
    );

/**
 * Invoked on OID_DOT11_UNICAST_USE_GROUP_ENABLED query. 
 * 
 * \param pStation              STATION structure
 * \param unicastUseGroupEnabled  caller allocated BOOLEAN pointer to store the current settting
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryUnicastUseGroupOption(
    __in  PSTATION        pStation,
    __out  PBOOLEAN        unicastUseGroupEnabled
    );
    
/**
 * Invoked on OID_DOT11_SAFE_MODE_ENABLED query. 
 * 
 * \param pStation              STATION structure
 * \param safeModeEnabled      caller allocated BOOLEAN pointer to store the current settting
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QuerySafeModeOption(
    __in  PSTATION        pStation,
    __out  PBOOLEAN        safeModeEnabled
    );

/**
 * Invoked on OID_DOT11_HIDDEN_NETWORK_ENABLED query. 
 * 
 * \param pStation              STATION structure
 * \param hiddenNetworkEnabled      caller allocated BOOLEAN pointer to store the current settting
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryHiddenNetworkOption(
    __in  PSTATION        pStation,
    __out  PBOOLEAN        hiddenNetworkEnabled
    );

/**
 * Invoked on OID_DOT11_STATISTICS query
 * 
 * \param pStation              STATION structure
 * \param pDot11Stats
 * \param InformationBufferLength
 * \param BytesWritten
 * \param BytesNeeded
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11QueryDot11Statistics(
    __in  PSTATION        pStation,
    __in PDOT11_STATISTICS pDot11Stats,
    __in ULONG InformationBufferLength,
    __out PULONG BytesWritten,
    __out PULONG BytesNeeded
    );

/**
 * Invoked on OID_DOT11_DESIRED_BSS_TYPE set 
 * 
 * \param pStation              STATION structure
 * \param bssType               The BSS type as requested by the OS
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetDesiredBSSType(
    __in  PSTATION        pStation,
    __in  DOT11_BSS_TYPE  bssType
    );

/**
 * Invoked on OID_DOT11_CURRENT_OPERATION_MODE set 
 * 
 * \param pStation              STATION structure
 * \param currentOperationMode  The operation mode as requested by the OS
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetOperationMode(
    __in  PSTATION        pStation,
    __in  PDOT11_CURRENT_OPERATION_MODE currentOperationMode
    );


/**
 * Invoked on OID_DOT11_DESIRED_SSID_LIST set. The MP functions
 * have already verified that buffer is big enough to
 * contain NumEntries SSIDs
 * 
 * \param pStation              STATION structure
 * \param pDot11SSIDList        List of SSIDs to set
 * \param pBytesRead            Returns number of BytesRead
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetDesiredSSIDList(
    __in  PSTATION        pStation,
    __in  PDOT11_SSID_LIST    pDot11SSIDList,
    __out PULONG          pBytesRead
    );

/**
 * Invoked on OID_DOT11_EXCLUDED_MAC_ADDRESS_LIST set. The MP functions
 * have already verified that buffer is big enough to
 * contain NumEntries MAC addresses
 * 
 * 
 * \param pStation              STATION structure
 * \param pDot11MacAddrList     List of MAC  addresses to populate
 * \param pBytesRead            Returns number of BytesRead
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetExcludedMACAddressList(
    __in  PSTATION        pStation,
    __in  PDOT11_MAC_ADDRESS_LIST pDot11MacAddrList,
    __out PULONG          pBytesRead
    );

/**
 * Invoked on OID_DOT11_DESIRED_BSSID_LIST set. The MP functions
 * has already verified that buffer is big enough to
 * contain NumEntries BSSIDs
 * 
 * \param pStation              STATION structure
 * \param pDot11BSSIDList       List of BSSIDs
 * \param pBytesRead            Returns number of BytesRead
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetDesiredBSSIDList(
    __in  PSTATION        pStation,
    __in  PDOT11_BSSID_LIST   pDot11BSSIDList,
    __out PULONG          pBytesRead
    );

/**
 * Invoked on OID_DOT11_ENABLED_AUTHENTICATION_ALGORITHMS set. 
 * The MP functions has already verified that buffer is big enough to
 * contain NumEntries auth algorithm pairs
 * 
 * \param pStation              STATION structure
 * \param pAuthAlgoList         List of Auth algorithms
 * \param pBytesRead            Returns number of BytesRead
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetEnabledAuthenticationAlgorithm(
    __in  PSTATION        pStation,
    __in  PDOT11_AUTH_ALGORITHM_LIST  pAuthAlgoList,
    __out PULONG          pBytesRead
    );

/**
 * Invoked on OID_DOT11_ENABLED_UNICAST_CIPHER_ALGORITHM set. 
 * The MP functions has already verified that buffer is big enough to
 * contain NumEntries auth algorithm pairs
 * 
 * \param pStation              STATION structure
 * \param pCipherAlgoList       List of cipher algorithms
 * \param pBytesRead            Returns number of BytesRead
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetEnabledUnicastCipherAlgorithm(
    __in  PSTATION        pStation,
    __in  PDOT11_CIPHER_ALGORITHM_LIST  pCipherAlgoList,
    __out PULONG          pBytesRead
    );

/**
 * Invoked on OID_DOT11_ENABLED_MULTICAST_CIPHER_ALGORITHM set. 
 * The MP functions has already verified that buffer is big enough to
 * contain NumEntries auth algorithm pairs
 * 
 * \param pStation              STATION structure
 * \param pCipherAlgoList       List of cipher algorithms
 * \param pBytesRead            Returns number of BytesRead
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetEnabledMulticastCipherAlgorithm(
    __in  PSTATION        pStation,
    __in  PDOT11_CIPHER_ALGORITHM_LIST  pCipherAlgoList,
    __out PULONG          pBytesRead
    );

/**
 * Invoked on OID_DOT11_CIPHER_DEFAULT_KEY_ID set. 
 * 
 * \param pStation              STATION structure
 * \param keyID                 new default key ID
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetCipherDefaultKeyID(
    __in  PSTATION pStation,
    __in  ULONG keyID
    );

/**
 * Invoked on OID_DOT11_CIPHER_DEFAULT_KEY set. 
 * 
 * \param pStation              STATION structure
 * \param defaultKey            default key to be set
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetCipherDefaultKey(
    __in  PSTATION pStation,
    __in  PDOT11_CIPHER_DEFAULT_KEY_VALUE defaultKey
    );

/**
 * Invoked on OID_DOT11_CIPHER_KEY_MAPPING_KEY set. 
 * 
 * \param pStation              STATION structure
 * \param pKeyMappingKeys       a list of key mapping keys to be set
 * \param TotalLength           total length of all the keys in the list.
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetCipherKeyMappingKey(
    __in  PSTATION pStation,
    __in  PDOT11_CIPHER_KEY_MAPPING_KEY_VALUE pKeyMappingKeys,
    __in  ULONG TotalLength
    );

/**
 * Invoked on OID_DOT11_EXCLUDE_UNENCRYPTED set. 
 * 
 * \param pStation              STATION structure
 * \param param                 BOOLEAN param value
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetExcludeUnencrypted(
    __in  PSTATION pStation,
    __in  BOOLEAN param
    );

/**
 * Invoked on OID_DOT11_PRIVACY_EXEMPTION_LIST set. The MP functions
 * have already verified that buffer is big enough to
 * contain NumEntries privacy exemption list
 * 
 * 
 * \param pStation              STATION structure
 * \param list                  Privacy exemption list to populate
 * \param pBytesRead            Returns number of BytesRead
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetPrivacyExemptionList(
    __in  PSTATION                      pStation,
    __in  PDOT11_PRIVACY_EXEMPTION_LIST list,
    __out PULONG              pBytesRead
    );

/**
 * Invoked on OID_DOT11_POWER_MGMT_REQUEST set. 
 * 
 * \param pStation              STATION structure
 * \param PowerSavingLevel      desired power saving level
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetPowerMgmtRequest(
    __in  PSTATION pStation,
    __in  ULONG PowerSavingLevel
    );

/**
 * Invoked on OID_DOT11_IBSS_PARAMS set. 
 * 
 * \param pStation              STATION structure
 * \param pDot11IBSSParams      The IBSS configuration parameters to set
 * \param BytesRead             Number of bytes read
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetIBSSParameters(
    __in  PSTATION pStation,
    __in  PDOT11_IBSS_PARAMS pDot11IBSSParams,
    __out PULONG BytesRead
    );

/**
 * Invoked on OID_DOT11_DESIRED_PHY_LIST set. 
 * 
 * \param pStation              STATION structure
 * \param PhyIdList             PHY ID list structure to set
 * \param BytesRead             Number of bytes read
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetDesiredPhyList(
    __in  PSTATION            pStation,
    __in  PDOT11_PHY_ID_LIST  PhyIdList,
    __out PULONG              BytesRead
    );

/**
 * Invoked on OID_DOT11_CURRENT_PHY_ID set. 
 * 
 * \param pStation              STATION structure
 * \param PhyId                 PHY ID to set
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetCurrentPhyId(
    __in  PSTATION        pStation,
    __in  ULONG           PhyId
    );

/**
 * Invoked on OID_DOT11_MEDIA_STREAMING_ENABLED set. 
 * 
 * \param pStation              STATION structure
 * \param mediaStreamingEnabled new media streaming setting
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetMediaStreamingOption(
    __in  PSTATION        pStation,
    __in  BOOLEAN         mediaStreamingEnabled
    );

/**
 * Invoked on OID_DOT11_PMKID_LIST set. 
 * 
 * \param pStation              STATION structure
 * \param PMKIDList             PMKID list structure to set
 * \param BytesRead             Number of bytes read
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetPMKIDList(
    __in  PSTATION            pStation,
    __in  PDOT11_PMKID_LIST  PMKIDList,
    __out PULONG              BytesRead
    );

/**
 * Invoked on OID_DOT11_UNREACHABLE_DETECTION_THRESHOLD set. 
 * 
 * \param pStation              STATION structure
 * \param threshold             New threshold to set to
 *
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetUnreachableDetectionThreshold(
    __in  PSTATION        pStation,
    __in  ULONG           threshold
    );

/**
 * Invoked on OID_DOT11_UNICAST_USE_GROUP_ENABLED set. 
 * 
 * \param pStation              STATION structure
 * \param unicastUseGroupEnabled      The new value for unicast use group enabled
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetUnicastUseGroupOption(
    __in  PSTATION pStation,
    __in  BOOLEAN  unicastUseGroupEnabled
    );

/**
 * Invoked on OID_DOT11_SAFE_MODE_ENABLED set. 
 * 
 * \param pStation              STATION structure
 * \param safeModeEnabled      The new safe mode setting
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetSafeModeOption(
    __in  PSTATION pStation,
    __in  BOOLEAN  safeModeEnabled
    );

/**
 * Invoked on OID_DOT11_HIDDEN_NETWORK_ENABLED set. 
 * 
 * \param pStation              STATION structure
 * \param hiddenNetworkEnabled      The new safe mode setting
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11SetHiddenNetworkOption(
    __in  PSTATION pStation,
    __in  BOOLEAN  hiddenNetworkEnabled
    );

/**
 * Invoked on OID_DOT11_SCAN_REQUEST.
 * 
 * \param pStation              STATION structure
 * \param ScanRequestID         The ID to use when completing the scan request
 * \param pDot11ScanRequest     The V2 scan request structure
 * \param ScanRequestBufferLength  Length of request buffer
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11StartScan(
    __in  PSTATION            pStation,
    __in  PVOID               ScanRequestID,
    __in  PDOT11_SCAN_REQUEST_V2 pDot11ScanRequest,
    __out ULONG               ScanRequestBufferLength
    );


/**
 * This function should be called by Mp when an outstanding scan operation completes.
 * Scan operations always pends and have to be completed asynchronously.
 * 
 * \param pStation          The STATION structure
 * \param ndisStatus        The success status of the scan request
 * \sa Sta11StartScan
 */
VOID
Sta11ScanComplete(
    __in  PSTATION        pStation,
    __in  NDIS_STATUS     ndisStatus
    );

/**
 * Initiates connection to an 802.11 infrastructure network. 
 * This function must return before the connection process has 
 * completed. Completion will be indicated by status indication
 * 
 * \param pStation              STATION structure
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11ConnectInfra(
    __in  PSTATION        pStation
    );


/**
 * Initiate disconnection from an 802.11 infrastructure network
 * 
 * \param pStation              STATION structure
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11DisconnectInfra(
    __in  PSTATION        pStation
    );

/**
 * Invoked once the hardware has synchronized with an access point or
 * has timed out waiting
 * 
 * \param pStation              STATION structure
 * \param ndisStatus            NDIS_STATUS from the Join attempt
 */
VOID
Sta11JoinCompleteInfra(
    __in  PSTATION        pStation,
    __in  NDIS_STATUS     ndisStatus
    );

/**
 * Initiates connection to an 802.11 independent (adhoc) network. 
 * This function must return before the connection process has 
 * completed. Completion will be indicated by status indication
 * 
 * \param pStation              STATION structure
 * \return NDIS_STATUS
 */
NTSTATUS
Sta11ConnectAdHoc(
    __in  PSTATION pStation
    );

/**
 * Initiate disconnection from an 802.11 independent (adhoc) network
 * 
 * \param pStation              STATION structure
 * \return NDIS_STATUS
 */
NTSTATUS
Sta11DisconnectAdHoc(
    __in  PSTATION pStation
    );

/**
 * Invoked once the hardware has synchronized with an Ad Hoc network or
 * has timed out waiting
 * 
 * \param pStation              STATION structure
 * \param ndisStatus            NDIS_STATUS from the Join attempt
 */

VOID
Sta11JoinCompleteAdHoc(
    __in  PSTATION pStation,
    __in  NDIS_STATUS ndisStatus
    );

/**
 * Invoked on an OID_DOT11_ENUM_ASSOCIATION_INFO query request
 * when the BSS type is set to infrastructure
 * 
 * \param pStation              STATION structure
 * \param pAssocInfoList        Buffer to fill association information into
 * \param TotalLength           Length of association information buffer
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11EnumerateAssociationInfoInfra(
    __in  PSTATION        pStation,
    __out PDOT11_ASSOCIATION_INFO_LIST   pAssocInfoList,
    __in  ULONG           TotalLength
    );

/**
 * Invoked on an OID_DOT11_ENUM_ASSOCIATION_INFO query request
 * when the BSS type is set to adhoc
 * 
 * \param pStation              STATION structure
 * \param pAssocInfoList        Buffer to fill association information into
 * \param TotalLength           Length of association information buffer
 * \return NDIS_STATUS
 */
NDIS_STATUS
Sta11EnumerateAssociationInfoAdHoc(
    __in  PSTATION        pStation,
    __out PDOT11_ASSOCIATION_INFO_LIST   pAssocInfoList,
    __in  ULONG           TotalLength
    );

/**
 * 
 * This function is called when a packet to be sent is submitted to
 * the miniport driver. This function can perform a number of operations
 * on the packet being sent. Please be careful about major operations
 * like subsuming packets etc. as they may affect the operation of the
 * rest of the miniport driver.
 * 
 * \param pStation              STATION structure
 * \param pNetBufferList        The NetBufferList submitted to the driver
 * \param SendFlags             Send flags such a dispatch level, etc
 * \param pDot11SendContext     The send context for this packet
 *
 * \return NDIS_STATUS_SUCCESS if this packet has been subsumed by this function.
 * Responsibility for sending and send completing this packet is on this function.
 * NDIS_STATUS_NOT_RECOGNIZED be returned if this packet is to process normally.
 * Any other status will be treated as failure and packet failed back to the OS.
 */
NDIS_STATUS
Sta11SendNetBufferList(
    __in  PSTATION        pStation,
    __in  PNET_BUFFER_LIST    pNetBufferList,
    __in  ULONG           SendFlags,
    __in  PDOT11_EXTSTA_SEND_CONTEXT  pSendContext
    );


/**
 * Determine if the station can send packets at this time or not. This is generally used
 * to block sending during scanning, etc. If this returns FALSE, the MP portion
 * will queue the packets up for later sending. If true, the Sta11ProcessSend routine 
 * will be called to start sending
 *
 * \param pStation              STATION structure
 * \return TRUE if the station can send packets at this time. FALSE otherwise
 */
BOOLEAN
Sta11CanTransmit(
    __in  PSTATION        pStation
    );


/**
 * This function is called just before a NET_BUFFER_LIST is going to
 * be submitted to hardware for sending. This function can update the MP_TX_MSDU 
 * structure with information that it wants the hardware portion to look
 * at when sending the packet out.
 * 
 * \param pStation              STATION structure
 * \param pMpTxd                The MSDU descriptor the packet to be transmitted
 *
 */
VOID
Sta11ProcessSend(
    __in  PSTATION        pStation,
    __in  PMP_TX_MSDU     pMpTxd
    );

/*
 * This function is called to notify when a send complete has occured and
 * is about to relayed to NDIS. Note that this function is only called
 * for TX MSDU that were actually handed to the hardware. Packets failed because
 * of issues that prevented us from ever handing the packet to the hardware
 * may not be provided to this API.
 *
 * \param   pStation            STATION structure
 * \param   pTxd                The TX MSDU completing
 * \param   ndisStatus          Completion status for this packet
 */
VOID 
Sta11ProcessSendComplete(
    __in  PSTATION        pStation,
    __in  PMP_TX_MSDU     pMpTxd,
    __in  NDIS_STATUS     ndisStatus
    ); 

/*
 * This function is called fill the station portion of the 
 * NDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES structure.
 *
 * \param   pStation            STATION structure
 * \param   attr                NDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES structure
 */
NDIS_STATUS
Sta11Fill80211Attributes(
    __in  PSTATION        pStation,
    __out PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr
    );

/*
 * This function is called cleanup the station portion of the 
 * NDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES structure.
 *
 * \param   pStation            STATION structure
 * \param   attr                NDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES structure
 */
VOID
Sta11Cleanup80211Attributes(
    __in PSTATION        pStation,
    __in PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES attr
    );

/*
 * This function is called by hardware layer to indicate MIC failure
 *
 * \param   pStation            STATION structure
 * \param   Header              802.11 header of the data frame in which MIC failure was detected
 * \param   KeyId               KeyId of the default key on which MIC failure was detected
 * \param   IsDefaultKey        Indicating if the key on which MIC failure was detected is a default key
 */
VOID
Sta11IndicateMICFailure(
    __in PSTATION                 pStation,
    __in PDOT11_DATA_SHORT_HEADER Header,
    __in ULONG                    KeyId,
    __in BOOLEAN                  IsDefaultKey
    );

// Invoked when the OID_PNP_SET_POWER is received to put device to sleep. This is
// called before hardware is called
VOID
Sta11PowerSleep(
    __in PSTATION                 pStation,
    __in NDIS_DEVICE_POWER_STATE  NewDeviceState
    );

// Invoked when the OID_PNP_SET_POWER is received to wake up device (being set to 
// NdisDeviceStateD0) . This is called after hardware is called and ready
VOID
Sta11PowerWakeup(
    __in PSTATION                 pStation
    );

VOID
Sta11NotifyEventRoamForSendFailures(
    __in  PSTATION        pStation
    );

VOID
Sta11NotifyEventUpdateLinkQuality(
    __in  PSTATION        pStation,
    __in  BOOLEAN         bState
    );

#endif //_STATION_INTERFACE_H

