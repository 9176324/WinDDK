/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Hw_Defines.h

Abstract:
    Hardware specific defines common to the complete driver
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/

#ifndef _MP_802_11_HW_DEFINES_

#define _MP_802_11_HW_DEFINES_


/** The minimum channel number supported by the hardware for DSSS PHY */
#define HW_MIN_CHANNEL_NUMBER     1

/** The maximum channel number supported by the hardware for DSSS PHY */
#define HW_MAX_CHANNEL_NUMBER     14

/** The default channel that the NIC will initialize itself with in b/g mode*/
#define HW_DEFAULT_CHANNEL        11

/** The default channel that the NIC will initialize itself with in "a" mode*/
#define HW_DEFAULT_CHANNEL_A      52

/** The minimum frequency supported by the hardware for OFDM PHY */
#define HW_MIN_OFDM_FREQ          1

/** The maximum frequency supported by the hardware for OFDM PHY */
#define HW_MAX_OFDM_FREQ          200

/** 
 * Specifies the type of interrupt that the NIC generates as one of the following values: 
 * NdisInterruptLatched          Interrupts are triggered by a transition from low to high on the interrupt line.
 * NdisInterruptLevelSensitive   Interrupts are active as long as the interrupt line is asserted. 
 */
#define HW_INTERRUPT_MODE         NdisInterruptLevelSensitive

/** The interrupt for this miniport is sharable with other devices */
#define HW_INTERRUPT_SHARABLE     TRUE

/** The time interval in which NDIS should call MiniportCheckForHang handler. See NdisMSetAttributesEx for details */
#define HW_CHECK_FOR_HANG_TIME_IN_SECONDS   6       // Increased CheckForHang time since our DOT11_RESET can
                                                    // take more than 2 seconds

/** Specifies the I/O bus interface type of the NIC, which usually is the type of I/O bus on which the NIC is connected */
#define HW_BUS_INTERFACE_TYPE                                     NdisInterfacePci

/** 
 * This macro defines the size of buffer that will be passed to NdisReadPciSlotInformation
 * By default this size is assumed to be PCI_COMMON_HDR_LENGTH which implies that there is
 * no DeviceSpecific information to be read. If the vendor has some device specific info
 * that is needed, please increase the size of this macro by that length. The function
 * Hw11ParsePciConfiguration function will always be called with the buffer returned by
 * NdisReadPciSlotInformation.
 */
#define HW_PCI_CONFIG_BUFFER_LENGTH       PCI_COMMON_HDR_LENGTH

/** the manufacturer of the device. This must be a value allocated by the PCI SIG */
#define HW_PCI_VENDOR_ID                  0x10EC

/** Identifies the particular device. This value is assigned by the manufacturer */
#define HW_PCI_8180_DEVICE_ID             0x8180
#define HW_PCI_8185_DEVICE_ID             0x8185

#define HW_RESOURCE_BUFFER_SIZE          (sizeof(NDIS_RESOURCE_LIST) \
                                                    + (10 * sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)))

#define HW_REQUIRES_ADDRESS_RESET           FALSE

/** The max link speed supported by this hardware (in bps) */
#define HW_LINK_SPEED                     11000000

/** Minimum number of Rx MSDU that we can allocate or shrink to */
#define HW_MIN_RX_MSDUS           64
/** Maximum number of Rx MSDUs that we will ever allocate or grow to */
#define HW_MAX_RX_MSDUS           128
/** Default number of Rx MSDUs to start with */
#define HW_DEF_RX_MSDUS           64


/** Minimum number of Tx MSDUs that need to be initialized with */
#define HW_MIN_TX_MSDUS           1
/** Maximum number of Rx MSDUs that we can allocate */
#define HW_MAX_TX_MSDUS           64
/** Default number of Tx MSDUs to allocate and initialize miniport with */
#define HW_DEF_TX_MSDUS           64

/** Maximum number of on-NIC multicast address entries */
#define HW_MAX_MCAST_LIST_SIZE    32

/** The backfill size this miniport driver needs. This is the maximum size of encryption IV */
#define HW_REQUIRED_BACKFILL_SIZE         8

/** Defines the maximum number of MAC Address States this miniport can maintain */
#define HW_MAX_MAC_ADDRESS_STATES         0

/*
 * The PS Lifetime that should be provided to each packet.
 * This value has to be derived from the PSLifetime that
 * has been passed in from the OS
 */
#define HW_PS_PACKET_LIFETIME(_uPSLifetime)     (_uPSLifetime * 2)

#define HW_MAJOR_DRIVER_VERSION         1
#define HW_MINOR_DRIVER_VERSION         9

#define HW_RECEIVE_NBL_CONTEXT_SIZE     0
#define HW_MINIPORT_NDIS_ATTRIBUTES     NDIS_MINIPORT_ATTRIBUTES_BUS_MASTER

         
#endif  // _MP_802_11_HW_DEFINES_

