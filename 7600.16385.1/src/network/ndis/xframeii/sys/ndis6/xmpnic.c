#pragma warning(push)   // Save the current warning state
#pragma warning(disable:4100) //unreference parameter in functions
#pragma warning(disable:4101)  //unreference local variables
#pragma warning(disable:4189)  //local variable is initialized but not referenced
#pragma warning( disable : 4127 )
#pragma warning( disable : 4706 )
/*
   NAME
     xmpnic.c

   DESCRIPTION
     This module implements...

   NOTES
     - No HARD TABS, indent (tab) width=2, Max columns=80

   MODIFIED
     mamta.gambhir@s2io.com        01/24/04 Creation.
*/

#include "precomp.h"
#include "xmpgbl.h"

/*
#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, xmpNicAlloc)
#pragma alloc_text(INIT, xmpNicLookup)
#endif
*/

/* ============================================================================
 * Globals definitions
 * ==========================================================================*/

/* Registry parameters table. Note: The sequence of field definitions should
 * match exactly with the structure definition of xmpRegEntry_t.
 */
xmpRegEntry_t XMPRegTable[] =
{
/*
{ reg value name,
  Offset in MP_ADAPTER,
  Field size,
  Default Value, Min, Max
} 
*/
#if HAL_DEBUG_ON
    {NDIS_STRING_CONST ( "hal_debug" ),
     0,
     XMP_NIC_PARAM_OFFSET ( hal_debug ),
     XMP_NIC_PARAM_SIZE ( hal_debug ),
     0xffffffff, 0, 0xffffffff},
#endif
 {NDIS_STRING_CONST ( "TxFIFONum" ),
     0,
     XMP_NIC_PARAM_OFFSET ( TxFIFONum ),
     XMP_NIC_PARAM_SIZE ( TxFIFONum ),
     1 /* default */ , 1 /*min */ , XGE_HAL_MAX_FIFO_NUM},

    {NDIS_STRING_CONST ( "MaxTxDs" ), /* max. TxDs per TxDL for all Tx FIFOs */
     0,
     XMP_NIC_PARAM_OFFSET ( MaxTxDs ),
     XMP_NIC_PARAM_SIZE ( MaxTxDs ),
     32 /*default */ , 0 /*min */ , 127 /*max */ }, /* TODO: 127: use define */

    {NDIS_STRING_CONST ( "*TransmitBuffers" ),
     0,
     XMP_NIC_PARAM_OFFSET ( TxFIFOLen_0 ),
     XMP_NIC_PARAM_SIZE ( TxFIFOLen_0 ),
     8192 /* def */ , XGE_HAL_MIN_FIFO_QUEUE_LENGTH /*min */, HAL_MAX_FIFO_THRESHOLD  /*max */ },

    {NDIS_STRING_CONST ( "TransmitBuffers_1" ),
     0,
     XMP_NIC_PARAM_OFFSET ( TxFIFOLen_1 ),
     XMP_NIC_PARAM_SIZE ( TxFIFOLen_1 ),
     0 /* def */ , XGE_HAL_MIN_FIFO_QUEUE_LENGTH/*min */ , HAL_MAX_FIFO_THRESHOLD  /*max */ },

    {NDIS_STRING_CONST ( "TransmitBuffers_2" ),
     0,
     XMP_NIC_PARAM_OFFSET ( TxFIFOLen_2 ),
     XMP_NIC_PARAM_SIZE ( TxFIFOLen_2 ),
     0 /* def */ ,  XGE_HAL_MIN_FIFO_QUEUE_LENGTH /*min */ , HAL_MAX_FIFO_THRESHOLD  /*max */ },

    {NDIS_STRING_CONST ( "TransmitBuffers_3" ),
     0,
     XMP_NIC_PARAM_OFFSET ( TxFIFOLen_3 ),
     XMP_NIC_PARAM_SIZE ( TxFIFOLen_3 ),
     0 /* def */ ,  XGE_HAL_MIN_FIFO_QUEUE_LENGTH /*min */ , HAL_MAX_FIFO_THRESHOLD  /*max */ },

    {NDIS_STRING_CONST ( "TransmitBuffers_4" ),
     0,
     XMP_NIC_PARAM_OFFSET ( TxFIFOLen_4 ),
     XMP_NIC_PARAM_SIZE ( TxFIFOLen_4 ),
     0 /* def */ , XGE_HAL_MIN_FIFO_QUEUE_LENGTH /*min */ , HAL_MAX_FIFO_THRESHOLD  /*max */ },

    {NDIS_STRING_CONST ( "TransmitBuffers_5" ),
     0,
     XMP_NIC_PARAM_OFFSET ( TxFIFOLen_5 ),
     XMP_NIC_PARAM_SIZE ( TxFIFOLen_5 ),
     0 /* def */ , XGE_HAL_MIN_FIFO_QUEUE_LENGTH /*min */ , HAL_MAX_FIFO_THRESHOLD  /*max */ },

    {NDIS_STRING_CONST ( "TransmitBuffers_6" ),
     0,
     XMP_NIC_PARAM_OFFSET ( TxFIFOLen_6 ),
     XMP_NIC_PARAM_SIZE ( TxFIFOLen_6 ),
     0 /* def */ , XGE_HAL_MIN_FIFO_QUEUE_LENGTH /*min */ , HAL_MAX_FIFO_THRESHOLD  /*max */ },

    {NDIS_STRING_CONST ( "TransmitBuffers_7" ),
     0,
     XMP_NIC_PARAM_OFFSET ( TxFIFOLen_7 ),
     XMP_NIC_PARAM_SIZE ( TxFIFOLen_7 ),
     0 /* def */ , XGE_HAL_MIN_FIFO_QUEUE_LENGTH /*min */ , HAL_MAX_FIFO_THRESHOLD  /*max */ },

    {NDIS_STRING_CONST ( "TxReqTimeOut" ),
     0,
     XMP_NIC_PARAM_OFFSET ( TxReqTimeOut ),
     XMP_NIC_PARAM_SIZE ( TxReqTimeOut ),
     HAL_TX_REQ_TIMEOUT_DEFAULT, HAL_TX_REQ_TIMEOUT_MIN,
     HAL_TX_REQ_TIMEOUT_MAX},

    {NDIS_STRING_CONST ( "TxNoSnoop" ),
     0,
     XMP_NIC_PARAM_OFFSET ( TxNoSnoop ),
     XMP_NIC_PARAM_SIZE ( TxNoSnoop ),
     0 /*def */ , 0, 1},

    /* Rx Stuff */
    {NDIS_STRING_CONST ( "RxRingNum" ),
     0,
     XMP_NIC_PARAM_OFFSET ( RxRingNum ),
     XMP_NIC_PARAM_SIZE ( RxRingNum ),
     1 /*def */ , 1 /*min */ , XGE_HAL_MAX_RING_NUM},

    {NDIS_STRING_CONST ( "*ReceiveBuffers" ),
     0,
     XMP_NIC_PARAM_OFFSET ( RxRingSz_0 ),
     XMP_NIC_PARAM_SIZE ( RxRingSz_0 ),
     XENAMP_DEFAULT_RXDS /*def */, XENAMP_MIN_RXDS/*min */,
     XENAMP_MAX_RXDS /*max */ },

    {NDIS_STRING_CONST ( "ReceiveBuffers_1" ),
     0,
     XMP_NIC_PARAM_OFFSET ( RxRingSz_1 ),
     XMP_NIC_PARAM_SIZE ( RxRingSz_1 ),
     XENAMP_DEFAULT_RXDS /*def */, XENAMP_MIN_RXDS /*min */ ,
     XENAMP_MAX_RXDS /*max */ },

    {NDIS_STRING_CONST ( "ReceiveBuffers_2" ),
     0,
     XMP_NIC_PARAM_OFFSET ( RxRingSz_2 ),
     XMP_NIC_PARAM_SIZE ( RxRingSz_2 ),
     XENAMP_DEFAULT_RXDS /*def */, XENAMP_MIN_RXDS /*min */ ,
     XENAMP_MAX_RXDS /*max */ },

    {NDIS_STRING_CONST ( "ReceiveBuffers_3" ),
     0,
     XMP_NIC_PARAM_OFFSET ( RxRingSz_3 ),
     XMP_NIC_PARAM_SIZE ( RxRingSz_3 ),
     XENAMP_DEFAULT_RXDS /*def */, XENAMP_MIN_RXDS /*min */ ,
     XENAMP_MAX_RXDS /*max */ },

    {NDIS_STRING_CONST ( "ReceiveBuffers_4" ),
     0,
     XMP_NIC_PARAM_OFFSET ( RxRingSz_4 ),
     XMP_NIC_PARAM_SIZE ( RxRingSz_4 ),
     XENAMP_DEFAULT_RXDS /*def */, XENAMP_MIN_RXDS /*min */ ,
     XENAMP_MAX_RXDS /*max */ },

    {NDIS_STRING_CONST ( "ReceiveBuffers_5" ),
     0,
     XMP_NIC_PARAM_OFFSET ( RxRingSz_5 ),
     XMP_NIC_PARAM_SIZE ( RxRingSz_5 ),
     XENAMP_DEFAULT_RXDS /*def */, XENAMP_MIN_RXDS /*min */ ,
     XENAMP_MAX_RXDS /*max */ },

    {NDIS_STRING_CONST ( "ReceiveBuffers_6" ),
     0,
     XMP_NIC_PARAM_OFFSET ( RxRingSz_6 ),
     XMP_NIC_PARAM_SIZE ( RxRingSz_6 ),
     XENAMP_DEFAULT_RXDS /*def */, XENAMP_MIN_RXDS /*min */ ,
     XENAMP_MAX_RXDS /*max */ },

    {NDIS_STRING_CONST ( "ReceiveBuffers_7" ),
     0,
     XMP_NIC_PARAM_OFFSET ( RxRingSz_7 ),
     XMP_NIC_PARAM_SIZE ( RxRingSz_7 ),
     XENAMP_DEFAULT_RXDS /*def */, XENAMP_MIN_RXDS /*min */ ,
     XENAMP_MAX_RXDS /*max */ },

    {NDIS_STRING_CONST ( "RxNoSnoop" ),
     0,
     XMP_NIC_PARAM_OFFSET ( RxNoSnoop ),
     XMP_NIC_PARAM_SIZE ( RxNoSnoop ),
     1 /*def */ , 0, 1},

    {NDIS_STRING_CONST ( "RxThresh" ),         /* not sure will this be used */
     0,
     XMP_NIC_PARAM_OFFSET ( RxThresh ),
     XMP_NIC_PARAM_SIZE ( RxThresh ),
     0, 0, 0},

    {NDIS_STRING_CONST ( "RxD_Backoff_interval" ),
     0,
     XMP_NIC_PARAM_OFFSET ( RxD_Backoff_interval ),
     XMP_NIC_PARAM_SIZE (  RxD_Backoff_interval ),
     XGE_HAL_MIN_BACKOFF_INTERVAL_US/*def */ , XGE_HAL_MIN_BACKOFF_INTERVAL_US,
     XGE_HAL_MAX_BACKOFF_INTERVAL_US},

    /* mac stuff */
    {NDIS_STRING_CONST ( "*JumboPacket" ),                /* essentially MTU */
     0,
     XMP_NIC_PARAM_OFFSET ( mtu ),
     XMP_NIC_PARAM_SIZE ( mtu ),
     HAL_MAX_PYLD /*def */ , HAL_MAX_PYLD /*min */ , HAL_MAX_PYLD_JUMBO},

    {NDIS_STRING_CONST ( "*FlowControl" ),
     0,
     XMP_NIC_PARAM_OFFSET ( FlowCtrl ),
     XMP_NIC_PARAM_SIZE ( FlowCtrl ),
     0 /*def */ , 0, 3},

    {NDIS_STRING_CONST ( "*IPChecksumOffloadIPv4" ),
     0,
     XMP_NIC_PARAM_OFFSET ( IpCkoV4 ),
     XMP_NIC_PARAM_SIZE ( IpCkoV4 ),
     XENAMP_CKO_DISABLED, XENAMP_CKO_DISABLED, XENAMP_CKO_TXRX},

    {NDIS_STRING_CONST ( "*IPChecksumOffloadIPv6" ),
     0,
     XMP_NIC_PARAM_OFFSET ( IpCkoV6 ),
     XMP_NIC_PARAM_SIZE ( IpCkoV6 ),
     XENAMP_CKO_DISABLED, XENAMP_CKO_DISABLED, XENAMP_CKO_TXRX},

    {NDIS_STRING_CONST ( "*TCPChecksumOffloadIPv4" ),
     0,
     XMP_NIC_PARAM_OFFSET ( TcpCkoV4 ),
     XMP_NIC_PARAM_SIZE ( TcpCkoV4 ),
     XENAMP_CKO_DISABLED, XENAMP_CKO_DISABLED, XENAMP_CKO_TXRX},

    {NDIS_STRING_CONST ( "*TCPChecksumOffloadIPv6" ),
     0,
     XMP_NIC_PARAM_OFFSET ( TcpCkoV6 ),
     XMP_NIC_PARAM_SIZE ( TcpCkoV6 ),
     XENAMP_CKO_DISABLED, XENAMP_CKO_DISABLED, XENAMP_CKO_TXRX},
 
    {NDIS_STRING_CONST ( "*UDPChecksumOffloadIPv4" ),
     0,
     XMP_NIC_PARAM_OFFSET ( UdpCkoV4 ),
     XMP_NIC_PARAM_SIZE ( UdpCkoV4 ),
     XENAMP_CKO_DISABLED, XENAMP_CKO_DISABLED, XENAMP_CKO_TXRX},

    {NDIS_STRING_CONST ( "*UDPChecksumOffloadIPv6" ),
     0,
     XMP_NIC_PARAM_OFFSET ( UdpCkoV6 ),
     XMP_NIC_PARAM_SIZE ( UdpCkoV6 ),
     XENAMP_CKO_DISABLED, XENAMP_CKO_DISABLED, XENAMP_CKO_TXRX},

    {NDIS_STRING_CONST ( "*LSOv1IPv4" ),
     0,
     XMP_NIC_PARAM_OFFSET ( LsoV4 ),
     XMP_NIC_PARAM_SIZE ( LsoV4 ),
     0, 0, 1},

    {NDIS_STRING_CONST ( "*PriorityVLANTag" ),
     0,
     XMP_NIC_PARAM_OFFSET ( PriVlan ),
     XMP_NIC_PARAM_SIZE ( PriVlan ),
     XENAMP_PRI_VLAN_DISABLED, XENAMP_PRI_VLAN_DISABLED,
     XENAMP_PRI_VLAN_ENABLED},

    {NDIS_STRING_CONST ( "*RSS" ),
     0,
     XMP_NIC_PARAM_OFFSET ( rss_enable ),
     XMP_NIC_PARAM_SIZE ( rss_enable ),
     0 /*def */ , 0 /*min */ , 1},

    {NDIS_STRING_CONST ( "LRO" ),
     0,
     XMP_NIC_PARAM_OFFSET ( Lro ),
     XMP_NIC_PARAM_SIZE ( Lro ),
     XENAMP_LRO_DISABLED, XENAMP_LRO_DISABLED, XENAMP_LRO_ENABLED},
 
    {NDIS_STRING_CONST ( "*HeaderDataSplit" ),
     0,
     XMP_NIC_PARAM_OFFSET ( rx_split ),
     XMP_NIC_PARAM_SIZE ( rx_split ),
     XENAMP_RX_SPLIT_NONE, XENAMP_RX_SPLIT_NONE,  XENAMP_RX_SPLIT},

    {NDIS_STRING_CONST ( "*InterruptModeration" ),
     0,
     XMP_NIC_PARAM_OFFSET (intr_modr ),
     XMP_NIC_PARAM_SIZE ( intr_modr ),
     0 /*def */ , 0 /*min */ , 1},

    {NDIS_STRING_CONST ( "offload_encapsulation" ),
     0,
     XMP_NIC_PARAM_OFFSET ( offload_encapsulation ),
     XMP_NIC_PARAM_SIZE ( offload_encapsulation ),
     0 /*def */ , 0 /*min */ , 2},

    /* interrupt coalesing */
    {NDIS_STRING_CONST ( "tx_urange_a" ),
     0,
     XMP_NIC_PARAM_OFFSET ( tx_urange_a ),
     XMP_NIC_PARAM_SIZE ( tx_urange_a ),
     1 /*def */ , 1 /*min */ , 100},

    {NDIS_STRING_CONST ( "tx_ufc_a" ),
     0,
     XMP_NIC_PARAM_OFFSET ( tx_ufc_a ),
     XMP_NIC_PARAM_SIZE ( tx_ufc_a ),
     1 /*def */ , 1 /*min */ , 100000},

    {NDIS_STRING_CONST ( "tx_urange_b" ),
     0,
     XMP_NIC_PARAM_OFFSET ( tx_urange_b ),
     XMP_NIC_PARAM_SIZE ( tx_urange_b ),
     1 /*def */ , 1 /*min */ , 100},

    {NDIS_STRING_CONST ( "tx_ufc_b" ),
     0,
     XMP_NIC_PARAM_OFFSET ( tx_ufc_b ),
     XMP_NIC_PARAM_SIZE ( tx_ufc_b ),
     1 /*def */ , 1 /*min */ , 100000},

    {NDIS_STRING_CONST ( "tx_urange_c" ),
     0,
     XMP_NIC_PARAM_OFFSET ( tx_urange_c ),
     XMP_NIC_PARAM_SIZE ( tx_urange_c ),
     1 /*def */ , 1 /*min */ , 100},

    {NDIS_STRING_CONST ( "tx_ufc_c" ),
     0,
     XMP_NIC_PARAM_OFFSET ( tx_ufc_c ),
     XMP_NIC_PARAM_SIZE ( tx_ufc_c ),
     1 /*def */ , 1 /*min */ , 100000},

    {NDIS_STRING_CONST ( "tx_ufc_d" ),
     0,
     XMP_NIC_PARAM_OFFSET ( tx_ufc_d ),
     XMP_NIC_PARAM_SIZE ( tx_ufc_d ),
     1 /*def */ , 1 /*min */ , 100000},

    {NDIS_STRING_CONST ( "tx_timer_val" ),
     0,
     XMP_NIC_PARAM_OFFSET ( tx_timer_val ),
     XMP_NIC_PARAM_SIZE ( tx_timer_val ),
     1 /*def */ , 0 /*min */ , 0xFFFFFFFF},
 
    {NDIS_STRING_CONST ( "tx_utilz_periodic" ),
     0,
     XMP_NIC_PARAM_OFFSET ( tx_utilz_periodic ),
     XMP_NIC_PARAM_SIZE ( tx_utilz_periodic ),
     0 /*def */ , 0 /*min */ , 1},
    
    {NDIS_STRING_CONST ( "tx_ac_disable" ),
     0,
     XMP_NIC_PARAM_OFFSET ( tx_ac_disable ),
     XMP_NIC_PARAM_SIZE ( tx_ac_disable ),
     0 /*def */ , 0 /*min */ , 1},

    {NDIS_STRING_CONST ( "tx_int_type_per_list" ),
     0,
     XMP_NIC_PARAM_OFFSET ( tx_int_type_per_list ),
     XMP_NIC_PARAM_SIZE ( tx_int_type_per_list ),
     0 /*def */ , 0 /*min */ , 1},
    
    {NDIS_STRING_CONST ( "tx_int_type_per_utilz" ),
     0,
     XMP_NIC_PARAM_OFFSET ( tx_int_type_utilz ),
     XMP_NIC_PARAM_SIZE ( tx_int_type_utilz ),
     1 /*def */ , 0 /*min */ , 1},
    
    {NDIS_STRING_CONST ( "pcc_enable" ),
     0,
     XMP_NIC_PARAM_OFFSET ( pcc_enable ),
     XMP_NIC_PARAM_SIZE ( pcc_enable ),
     0xE1 /*def */ , 0x1 /*min */ , 0xFF},

    {NDIS_STRING_CONST ( "rx_urange_a" ),
     0,
     XMP_NIC_PARAM_OFFSET ( rx_urange_a ),
     XMP_NIC_PARAM_SIZE ( rx_urange_a ),
     1 /*def */ , 1 /*min */ , 100},

    {NDIS_STRING_CONST ( "rx_ufc_a" ),
     0,
     XMP_NIC_PARAM_OFFSET ( rx_ufc_a ),
     XMP_NIC_PARAM_SIZE ( rx_ufc_a ),
     1 /*def */ , 1 /*min */ , 100000},

    {NDIS_STRING_CONST ( "rx_urange_b" ),
     0,
     XMP_NIC_PARAM_OFFSET ( rx_urange_b ),
     XMP_NIC_PARAM_SIZE ( rx_urange_b ),
     1 /*def */ , 1 /*min */ , 100},

    {NDIS_STRING_CONST ( "rx_ufc_b" ),
     0,
     XMP_NIC_PARAM_OFFSET ( rx_ufc_b ),
     XMP_NIC_PARAM_SIZE ( rx_ufc_b ),
     1 /*def */ , 1 /*min */ , 100000},

    {NDIS_STRING_CONST ( "rx_urange_c" ),
     0,
     XMP_NIC_PARAM_OFFSET ( rx_urange_c ),
     XMP_NIC_PARAM_SIZE ( rx_urange_c ),
     1 /*def */ , 1 /*min */ , 100},

    {NDIS_STRING_CONST ( "rx_ufc_c" ),
     0,
     XMP_NIC_PARAM_OFFSET ( rx_ufc_c ),
     XMP_NIC_PARAM_SIZE ( rx_ufc_c ),
     1 /*def */ , 1 /*min */ , 100000},

    {NDIS_STRING_CONST ( "rx_ufc_d" ),
     0,
     XMP_NIC_PARAM_OFFSET ( rx_ufc_d ),
     XMP_NIC_PARAM_SIZE ( rx_ufc_d ),
     1 /*def */ , 1 /*min */ , 100000},

    {NDIS_STRING_CONST ( "rx_timer_val" ),
     0,
     XMP_NIC_PARAM_OFFSET ( rx_timer_val ),
     XMP_NIC_PARAM_SIZE ( rx_timer_val ),
     1 /*def */ , 0 /*min */ , 0xFFFFFFFF},

    {NDIS_STRING_CONST ( "rx_utilz_periodic" ),
     0,
     XMP_NIC_PARAM_OFFSET ( rx_utilz_periodic ),
     XMP_NIC_PARAM_SIZE ( rx_utilz_periodic ),
     0 /*def */ , 0 /*min */ , 1},
    
     {NDIS_STRING_CONST ( "rx_ac_disable" ),
     0,
     XMP_NIC_PARAM_OFFSET ( rx_ac_disable ),
     XMP_NIC_PARAM_SIZE ( rx_ac_disable ),
     0 /*def */ , 0 /*min */ , 1},
    
    /* misc stuff */
    {NDIS_STRING_CONST ( "stats_auto_enable" ),
     0,
     XMP_NIC_PARAM_OFFSET ( stats_auto_enable ),
     XMP_NIC_PARAM_SIZE ( stats_auto_enable ),
     1 /*def */ , 0 /*min */ , 1},

    {NDIS_STRING_CONST ( "stats_refresh_time" ),
     0,
     XMP_NIC_PARAM_OFFSET ( stats_refresh_time ),
     XMP_NIC_PARAM_SIZE ( stats_refresh_time ),
     2 /*def */ , 1 /*min */ , 100},

    {NDIS_STRING_CONST ( "max_split_trans" ),
     0,
     XMP_NIC_PARAM_OFFSET ( max_split_trans ),
     XMP_NIC_PARAM_SIZE ( max_split_trans ),
     XGE_HAL_TWO_SPLIT_TRANSACTION, XGE_HAL_ONE_SPLIT_TRANSACTION, 
     8},
      
    {NDIS_STRING_CONST ( "max_read_byte_cnt" ),
     0,
     XMP_NIC_PARAM_OFFSET ( max_read_byte_cnt ),
     XMP_NIC_PARAM_SIZE ( max_read_byte_cnt ),
     2/*def */ , 0 /*min */ , 4},

    {NDIS_STRING_CONST ( "sched_intr_timer_val" ),
     0,
     XMP_NIC_PARAM_OFFSET ( sched_intr_timer_val ),
     XMP_NIC_PARAM_SIZE ( sched_intr_timer_val ),
     1 /*def */ , 0 /*min */ , 0xFFFFFFF},

    {NDIS_STRING_CONST ( "indicate_max_pkts" ),
     0,
     XMP_NIC_PARAM_OFFSET (indicate_max_pkts ),
     XMP_NIC_PARAM_SIZE ( indicate_max_pkts ),
     0 /*def - zero means infinity */ , 8 /*min */ , 0xFFFFFFFF},

    {NDIS_STRING_CONST ( "rx_nbl_coalesce" ),
     0,
     XMP_NIC_PARAM_OFFSET ( rx_nbl_coalesce ),
     XMP_NIC_PARAM_SIZE ( rx_nbl_coalesce ),
     0 /*def - zero means infinity */ , 0 /*min */ , 1},

    {NDIS_STRING_CONST ( "tx_nbl_coalesce" ),
     0,
     XMP_NIC_PARAM_OFFSET ( tx_nbl_coalesce ),
     XMP_NIC_PARAM_SIZE ( tx_nbl_coalesce ),
     0 /*def - zero means infinity */ , 0 /*min */ , 1},

    {NDIS_STRING_CONST ( "interrupt_poll_cnt" ),
     0,
     XMP_NIC_PARAM_OFFSET (interrupt_poll_cnt ),
     XMP_NIC_PARAM_SIZE ( interrupt_poll_cnt ),
     2 /*def - zero means infinity */ , 2 /*min */ , 256},

    {NDIS_STRING_CONST ( "shared_splits" ),
     0,
     XMP_NIC_PARAM_OFFSET ( shared_splits ),
     XMP_NIC_PARAM_SIZE ( shared_splits ),
     0 /*def - zero means infinity */ , 0 /*min */ , 32},

    {NDIS_STRING_CONST ( "rd2rd_dly" ),
     0,
     XMP_NIC_PARAM_OFFSET (rd2rd_dly),
     XMP_NIC_PARAM_SIZE ( rd2rd_dly),
     0 /*def - zero means infinity */ , 0 /*min */ , 0x7},

    {NDIS_STRING_CONST ( "enable_tx_req_timeout" ),
     0,
     XMP_NIC_PARAM_OFFSET ( enable_tx_req_timeout ),
     XMP_NIC_PARAM_SIZE ( enable_tx_req_timeout ),
     0 /*def - zero means infinity */ , 0 /*min */ , 1},

    {NDIS_STRING_CONST ( "tx_req_timeout" ),
     0,
     XMP_NIC_PARAM_OFFSET ( tx_req_timeout ),
     XMP_NIC_PARAM_SIZE ( tx_req_timeout ),
     0 /*def - zero means infinity */ , 0 /*min */ , 0xFFFFFFFF},

    {NDIS_STRING_CONST ( "latency_timer" ),
     0,
     XMP_NIC_PARAM_OFFSET ( latency_timer ),
     XMP_NIC_PARAM_SIZE ( latency_timer),
     0x1 /*def */ , 0x1 /*min */ , 0xFF},

    {NDIS_STRING_CONST ( "intr_mode" ),
     0,
     XMP_NIC_PARAM_OFFSET ( intr_mode ),
     XMP_NIC_PARAM_SIZE ( intr_mode ),
     XGE_HAL_INTR_MODE_IRQLINE  /*def */, 0,  XGE_HAL_INTR_MODE_MSIX}
};

#define XMP_REG_PARAM_COUNT (sizeof (XMPRegTable) / sizeof(xmpRegEntry_t))


/* ============================================================================
 * Private static function protos
 * ==========================================================================*/
VOID
xmpNicTxServe(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  xmpNicTxFifo_t         *pFifo
  );

NDIS_STATUS
xmpNicTxNBLValid(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  NET_BUFFER_LIST        *pNBL
  );

NET_BUFFER_LIST *
xmpNicRxNBLAlloc(
  IN xmpNicCtx_t             *pNicCtx,
  IN UCHAR                   *pBuf,
  IN U32                      BufSz
  );

NDIS_STATUS
xmpNicHdrMDLsAlloc(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  ULONG                   HeaderSz,
  IN  ULONG                   MdlCount,
  OUT xmpNicRxHdrMDL_t        MdlArr[],
  OUT xmpNicRxHdrBlk_t        BlkArr[]
  );

NDIS_STATUS
xmpNicHdrMDLBlockAlloc(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  ULONG                   HeaderSz,
  IN  ULONG                   MdlCount,
  OUT void                  **pBlockVa,
  OUT NDIS_PHYSICAL_ADDRESS  *pBlockPa,
  OUT xmpNicRxHdrMDL_t       *pMdlArr
  );

VOID
xmpNicRxNBLFree(
  IN NET_BUFFER_LIST         *pNBL
  );

NDIS_STATUS
xmpNicRxNBLBlkAlloc(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  UINT                    FrameBufSz,
  IN  UINT                    RingNum,
  IN  UINT                    RxBufCnt,
  IN  xmpQLink_t             *pRxNBLQ,
  IN  xmpQLink_t             *pRxBLKQ
  );

VOID
xmpDbgVerifyNBL(
  IN xmpNicCtx_t             *pNicCtx,
  IN NET_BUFFER_LIST         *nbl
  );

/* ============================================================================
 * Public functions definition
 * ==========================================================================*/

/* 
 * Description:
 *   Allocates memory for the logical adapter representing nic. It also grabs
 *   initial adapter reference count. This adapter is deallocated by
 *   XMP_NIC_DEREFERENCE when this reference count becomes 0.
 *
 * Arguments:
 *   hAdapter       - NDIS Miniport adapter handle
 *
 * Return Value:
 *   A pointer to the newly allocated adapter.
 *
 * Callers:
 *   called by xenaMpInitialize
 *
 * IRQL:
 *   PASSIVE_LEVEL
 *
 * See Also:
 *   xenaMpInitialize, xmpNicFree, XMP_NIC_REFERENCE, XMP_NIC_DEREFERENCE
 *
 */ 
xmpNicCtx_t *
xmpNicAlloc(
  IN  NDIS_HANDLE             hAdapter
  )
{
  xmpNicCtx_t                *pNicCtx = (xmpNicCtx_t *) 0;
  UINT                        ui;
  NDIS_STATUS                 Status = NDIS_STATUS_FAILURE;

  XMPTRACE(XMP_DBG_INI, ("==> xmpNicAlloc\n"));

  do
  {
    if ( !(pNicCtx = (xmpNicCtx_t *)
           XF_MALLOC(hAdapter, sizeof(xmpNicCtx_t) + sizeof(xge_hal_device_t))) )
    {
      XMPTRACE(XMP_DBG_ERR, ("xmpNicAlloc: Allocate NIC failed\n"));
      break;
    }
    XF_MCLEAR(pNicCtx, sizeof(xFrameNic_t) + sizeof(xge_hal_device_t));
    pNicCtx->magic = XF_NIC_MAGIC;
    pNicCtx->pHalDev = (xge_hal_device_t *) (pNicCtx + 1);
    XMP_NIC_REFERENCE(pNicCtx);

    pNicCtx->hMPAdapter = hAdapter;
    if ( !(pNicCtx->hWorkItem = NdisAllocateIoWorkItem(pNicCtx->hMPAdapter)) )
    {
      XMPTRACE(XMP_DBG_ERR, ("xmpNicAlloc: NdisAllocateIoWorkItem failed\n"));
      break;
    }

    if ( !(pNicCtx->hResetWorkItem =
           NdisAllocateIoWorkItem(pNicCtx->hMPAdapter)) )
    {
      XMPTRACE(XMP_DBG_ERR, ("xmpNicAlloc: NdisAllocateIoWorkItem failed\n"));
      break;
    }

    if ( !(pNicCtx->hSetRssWI = NdisAllocateIoWorkItem(pNicCtx->hMPAdapter)) )
    {
      XMPTRACE(XMP_DBG_ERR, ("xmpNicAlloc: NdisAllocateIoWorkItem failed\n"));
      break;
    }

    XF_INIT_LATCH(&pNicCtx->latch);                      /* Generic RW latch */
    XF_INIT_SLOCK(&pNicCtx->lock);                      /* Generic Spin lock */
    XF_Q_INIT(&pNicCtx->GblListLink);      /* Link for global adapters queue */
    XF_Q_INIT(&pNicCtx->RxBufBlkQ);   /* List to track allocated RxBufBlk(s) */

    for( ui=0; ui < XGE_HAL_MAX_FIFO_NUM; ui++ )
    {
      pNicCtx->TxFifo[ui].pNicCtx = (VOID *)pNicCtx;
      XF_INIT_SLOCK(&pNicCtx->TxFifo[ui].TxLock);
      XF_INIT_SLOCK(&pNicCtx->TxFifo[ui].HalTxLock);
      XF_Q_INIT(&pNicCtx->TxFifo[ui].NBLCtxQ);
      XF_Q_INIT(&pNicCtx->TxFifo[ui].NBLWaitQ);
      if ( !(pNicCtx->TxFifo[ui].hTxFlushWI =
             NdisAllocateIoWorkItem(pNicCtx->hMPAdapter)) )
      {
        XMPTRACE(XMP_DBG_ERR, ("xmpNicAlloc: NdisAllocateIoWorkItem failed\n"));
        break;
      }
    }
    if ( ui < XGE_HAL_MAX_FIFO_NUM )
      break;
    
    for( ui=0; ui < XGE_HAL_MAX_RING_NUM; ui++ )
    {
      XF_INIT_SLOCK(&pNicCtx->RxInfo[ui].RxLock);           /* Rx Queue lock */
      XF_Q_INIT(&pNicCtx->RxInfo[ui].RxNBLQ);     /* track allocated Rx NBLs */
    }

    XENAMP_NDIS_OBJECT_INIT(&pNicCtx->ndisRssSet.params.Header,
                            NDIS_OBJECT_TYPE_RSS_PARAMETERS,
                            NDIS_RECEIVE_SCALE_PARAMETERS_REVISION_1,
                            sizeof(NDIS_RECEIVE_SCALE_PARAMETERS));

    /* Put the nic on global link list here.                                 */
    XF_GET_SLOCK(&xmpLock);
      XF_Q_ADD_TAIL(&xmpNicsList, &pNicCtx->GblListLink);
    XF_FREE_SLOCK(&xmpLock);
    Status = NDIS_STATUS_SUCCESS;
    XMPTRACE(XMP_DBG_INI, ("xmpNicAlloc: Success\n"));
  } while(0);

  if ( (Status != NDIS_STATUS_SUCCESS) && pNicCtx )
  {
    XMP_NIC_DEREFERENCE(pNicCtx);
    pNicCtx = (xmpNicCtx_t *) 0;
  }

  XMPTRACE(XMP_DBG_INI, ("<== xmpNicAlloc: pNicCtx=%p pHalDev=%p\n",
                         pNicCtx, pNicCtx->pHalDev));
  return pNicCtx;
}

/* 
 * Description:
 *   Frees up the nic and its all associated resources. This function is
 * should be called when the nic's reference count becomes 0 (xenaMpHalt)
 * There should be no pending Tx or Rx NBLs with HAL, or with ULPs.
 *
 * Arguments:
 *   pNic      - Pointer to the logical adapter to free up.
 *
 * Return Value:
 *   none
 *
 * Callers:
 *   called by xenaMpHalt, xmpNicAlloc
 *
 * IRQL:
 *   PASSIVE_LEVEL
 *
 * See Also:
 *   XMP_NIC_DEREFERENCE
 *
 */ 
VOID
xmpNicFree(
  IN  xmpNicCtx_t            *pNicCtx
  )
{
  xmpRxBufBlk_t              *pRxBlk;
  xmpRxNBLCtx_t              *pRxCtx;
  xmpQLink_t                 *link;
  xmpNicRxInfo_t             *pRing;
  UINT                        ui;
  UINT                        ui2;
  UCHAR                       bCancelled;

  XMPTRACE(XMP_DBG_TRM, ("==> xmpNicFree: pNic=%p\n", pNicCtx));
  /* First check to make sure there is no pending work on the adapter.       */

  /* Make sure the ref count is 0.                                           */
  ASSERT(pNicCtx->RefCnt == 0);

  if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_INTR_REG) )
  {
    NdisMDeregisterInterruptEx(pNicCtx->hInterrupt);
    bic(pNicCtx->Flags, XMP_NIC_FLAGS_INTR_REG);
  }

  if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_SG_DMA_INIT) )
    NdisMDeregisterScatterGatherDma(pNicCtx->hMPDma);

  if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_HAL_NIC_INIT) )
    xge_hal_device_terminate(XMP_NIC_GET_DEV(pNicCtx));

  if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_INTR_REG) )
  {
    XMPTRACE(XMP_DBG_TRM, ("xmpNicFree: NdisMDeregisterInterruptEx\n"));
    NdisMDeregisterInterruptEx(pNicCtx->hInterrupt);
    bic(pNicCtx->Flags, XMP_NIC_FLAGS_INTR_REG);
  }

  /* Remove the nic from the global nics list.                               */
  XF_GET_SLOCK(&xmpLock);
    XF_Q_DEL(&pNicCtx->GblListLink);
  XF_FREE_SLOCK(&xmpLock);

  {
    XMPTRACE(XMP_DBG_TRM, ("xmpNicFree: ==> halFree\n"));
    XMPTRACE(XMP_DBG_TRM, ("xmpNicFree: <== halFree\n"));
  }

  if ( pNicCtx->deviceMem1 )
  {
    XMPTRACE(XMP_DBG_TRM, ("xmpNicFree: NdisMUnmapIoSpace\n"));
    NdisMUnmapIoSpace(pNicCtx->hMPAdapter, pNicCtx->deviceMem1,
                      pNicCtx->deviceMem1Len);
  }

  if ( pNicCtx->deviceMem2 )
  {
    XMPTRACE(XMP_DBG_TRM, ("xmpNicFree: NdisMUnmapIoSpace 2\n"));
    NdisMUnmapIoSpace(pNicCtx->hMPAdapter, pNicCtx->deviceMem2,
                      pNicCtx->deviceMem2Len);
  }

  if ( pNicCtx->deviceMem3 )
  {
    XMPTRACE(XMP_DBG_TRM, ("xmpNicFree: NdisMUnmapIoSpace 3\n"));
    NdisMUnmapIoSpace(pNicCtx->hMPAdapter, pNicCtx->deviceMem3,
                      pNicCtx->deviceMem3Len);
  }

  /* Free up Tx resources.                                                   */
  for (ui = 0; ui < XGE_HAL_MAX_FIFO_NUM; ui++)
  {
    NdisFreeIoWorkItem(pNicCtx->TxFifo[ui].hTxFlushWI);
    if ( pNicCtx->TxFifo[ui].NBLCtxBuf )
      NdisFreeMemory(pNicCtx->TxFifo[ui].NBLCtxBuf,0,0);
#ifdef XENAMP_TXD_FREE_COALESCE
    if ( pNicCtx->TxFifo[ui].dtrFree )
      NdisFreeMemory(pNicCtx->TxFifo[ui].dtrFree,0,0);
#endif /*  XENAMP_TXD_FREE_COALESCE */
  }
  XMPTRACE(XMP_DBG_TRM, ("xmpNicFree: Freed TX resources\n"));

  if ( pNicCtx->Diag.pBuffAddr )
  {
    NdisMFreeSharedMemory(pNicCtx->hMPAdapter,
                          pNicCtx->Diag.BuffSz,
                          TRUE,
                          (VOID *)pNicCtx->Diag.pBuffAddr,
                          pNicCtx->Diag.BuffAddrPhy);
    pNicCtx->Diag.pBuffAddr = NULL;
  }

  /* Free up Rx resources.                                                   */
  for (ui = 0; ui < pNicCtx->RegParams.RxRingNum; ui++)
  {
    pRing = &pNicCtx->RxInfo[ui];
    XF_Q_DEL_TAIL(&pRing->RxNBLQ, link);
    while(link)
    {
      pRxCtx = CONTAINING_RECORD(link, xmpRxNBLCtx_t, NBLQLink);
      xmpNicRxNBLFree(pRxCtx->pNBL);
      XF_Q_DEL_TAIL(&pRing->RxNBLQ, link);
    }

    if ( pRing->hdrMDLsArr )
    {
      for ( ui2 = 0; ui2 < pRing->RxDCnt; ui2++)
        NdisFreeMdl(pRing->hdrMDLsArr[ui2].pMDL);

      NdisFreeMemory(pRing->hdrMDLsArr,0,0);
    }

    if ( pRing->hdrBlockArr )
    {
      xmpNicRxHdrBlk_t *blk = pRing->hdrBlockArr;
      ui2 = 0;
      while( blk->va )
      {
        NdisMFreeSharedMemory(pNicCtx->hMPAdapter, PAGE_SIZE, TRUE,
                              (VOID *)blk->va, blk->pa);
        blk++;
      }
      NdisFreeMemory(pRing->hdrBlockArr,0,0);
    }
  }
  XMPTRACE(XMP_DBG_TRM, ("xmpNicFree: Freed RX resources\n"));

  XF_Q_DEL_TAIL(&pNicCtx->RxBufBlkQ, link);
  while(link)
  {
    ULONG RxAlignSz, blkSz;
    pRxBlk = CONTAINING_RECORD(link, xmpRxBufBlk_t, Link);
    RxAlignSz = XMP_RXBUF_ALIGN_SZ(pNicCtx->CacheFillSize);
    blkSz = XENAMP_RXBUF_BLOCK_SIZE(pNicCtx->MaxFrameSz, RxAlignSz);
    NdisMFreeSharedMemory(pNicCtx->hMPAdapter, blkSz, 
                          TRUE, (VOID *)pRxBlk, pRxBlk->PhyAddr);
    XF_Q_DEL_TAIL(&pNicCtx->RxBufBlkQ, link);
  }

  XMPTRACE(XMP_DBG_TRM, ("xmpNicFree: Rx Buffer Block\n"));

  if ( pNicCtx->hRxNBLPool )
  {
    XMPTRACE(XMP_DBG_TRM, ("xmpNicFree: Rx buffer pool\n"));
    NdisFreeNetBufferListPool(pNicCtx->hRxNBLPool);
  }

  if ( pNicCtx->hSetRssWI )
  {
    XMPTRACE(XMP_DBG_TRM, ("xmpNicFree: Pause Work Item\n"));
    NdisFreeIoWorkItem(pNicCtx->hSetRssWI);
  }

  if ( pNicCtx->hResetWorkItem )
  {
    XMPTRACE(XMP_DBG_TRM, ("xmpNicFree: Reset Work Item\n"));
    NdisFreeIoWorkItem(pNicCtx->hResetWorkItem);
  }

  if ( pNicCtx->hWorkItem )
  {
    XMPTRACE(XMP_DBG_TRM, ("xmpNicFree: Pause Work Item\n"));
    NdisFreeIoWorkItem(pNicCtx->hWorkItem);
  }

  /* Finally free up the nic memory.                                         */
  XMPTRACE(XMP_DBG_TRM, ("xmpNicFree: Free NIC\n"));
  NdisFreeMemory(pNicCtx,0,0);
  XMPTRACE(XMP_DBG_TRM, ("<== xmpNicFree: pNic=%p\n", pNicCtx));
}

/* 
 * xmpNicReadConfig
 *
 * Description:
 *   Read registry parameters, and set the hal/nic configuration information
 *
 * Arguments:
 *   pNic                - pointer to the nic
 *
 * Return Value:
 *   NDIS_STATUS_SUCCESS if successful, NDIS_STATUS_XXX error code if not.
 *
 * Callers:
 *   called by xmpInitialize
 *
 * IRQL:
 *   PASSIVE_LEVEL
 *
 * Notes:
 *   1. hMPAdapter must be set before calling this function.
 *   2. The sequence of field definitions should match exactly with
 *      the structure definition of xmpRegEntry_t.
 *
 */ 
NDIS_STATUS
xmpNicReadConfig(
  IN  xmpNicCtx_t            *pNicCtx
  )
{
  CCHAR                       pc;
  UINT                        i;
  UINT                        uiTmp;
  ULONG                       rxdPerBlk;
  NDIS_STATUS                 Status;
  NDIS_HANDLE                 hConfig = NULL;
  xmpRegEntry_t              *pRegEntry;
  UCHAR                      *pucTmp;
  NDIS_CONFIGURATION_PARAMETER *pValue;
  NDIS_CONFIGURATION_OBJECT   ConfigObject;

  XMPTRACE(XMP_DBG_INI, ("==> xmpNicReadConfig: pNicCtx=%p\n", pNicCtx));

  do
  {
    ConfigObject.Header.Type = NDIS_OBJECT_TYPE_CONFIGURATION_OBJECT;
    ConfigObject.Header.Revision = NDIS_CONFIGURATION_OBJECT_REVISION_1;
    ConfigObject.Header.Size = sizeof(NDIS_CONFIGURATION_OBJECT);
    ConfigObject.NdisHandle = pNicCtx->hMPAdapter;
    ConfigObject.Flags = 0;
    if ( (Status = NdisOpenConfigurationEx(&ConfigObject, &hConfig)
           != NDIS_STATUS_SUCCESS) )
    {
      XMPTRACE(XMP_DBG_ERR, ("NdisOpenConfigurationEx failed = %x\n", Status));
      break;
    }

    for (i=0, pRegEntry=XMPRegTable; i < XMP_REG_PARAM_COUNT; i++, pRegEntry++)
    {
      pucTmp = ((UCHAR *) &pNicCtx->RegParams) + pRegEntry->FieldOffset;

      NdisReadConfiguration(&Status, &pValue, hConfig, &pRegEntry->RegName,
                            NdisParameterInteger);
      if ( Status == NDIS_STATUS_SUCCESS )
      {
        if ( (pValue->ParameterData.IntegerData < pRegEntry->Min) ||
             (pValue->ParameterData.IntegerData > pRegEntry->Max) )
          uiTmp = pRegEntry->Default;
        else
          uiTmp = pValue->ParameterData.IntegerData;
      }
      else if ( pRegEntry->bRequired )
      {
        ASSERT(FALSE);
        Status = NDIS_STATUS_FAILURE;
        break;
      }
      else
      {
        uiTmp = pRegEntry->Default;
        Status = NDIS_STATUS_SUCCESS;
      }

      switch ( pRegEntry->FieldSize )
      {
      case 1:
        *((UCHAR *) pucTmp) = (UCHAR) uiTmp;
        break;

      case 2:
        *((USHORT *) pucTmp) = (USHORT) uiTmp;
        break;

      case 4:
        *((ULONG *) pucTmp) = (ULONG) uiTmp;
        break;
        
      default:
        ASSERT(FALSE);
        break;
      }
    }

    if ( Status != NDIS_STATUS_SUCCESS )
      break;

    NdisReadNetworkAddress(&Status, &pucTmp, &uiTmp, hConfig);
    if ( ( Status == NDIS_STATUS_SUCCESS )
         && ( uiTmp == ETH_LENGTH_OF_ADDRESS ) )
    {
      if ( ETH_IS_MULTICAST(pucTmp) ||
           ETH_IS_BROADCAST(pucTmp) )
      {
        XMPTRACE(XMP_DBG_WRN, ("Invalid NetworkAddress in registry"
                               "- %02x-%02x-%02x-%02x-%02x-%02x\n",
                               pucTmp[0], pucTmp[1],
                               pucTmp[2], pucTmp[3],
                               pucTmp[4], pucTmp[5]));
      }
      else
      {
        ETH_COPY_NETWORK_ADDRESS(pNicCtx->CurrentAddress,
                                 pucTmp);
        bis(pNicCtx->Flags, XMP_NIC_FLAGS_USE_CURRENT_ADDR);
        XMPTRACE(XMP_DBG_INFO, ("Using NetworkAddress in registry"
                                "- %02x-%02x-%02x-%02x-%02x-%02x\n",
                                pucTmp[0], pucTmp[1],
                                pucTmp[2], pucTmp[3],
                                pucTmp[4], pucTmp[5]));
      }
    }

    pNicCtx->TxFifo[0].CfgTxDCnt = pNicCtx->RegParams.TxFIFOLen_0;
    pNicCtx->TxFifo[1].CfgTxDCnt = pNicCtx->RegParams.TxFIFOLen_1;
    pNicCtx->TxFifo[2].CfgTxDCnt = pNicCtx->RegParams.TxFIFOLen_2;
    pNicCtx->TxFifo[3].CfgTxDCnt = pNicCtx->RegParams.TxFIFOLen_3;
    pNicCtx->TxFifo[4].CfgTxDCnt = pNicCtx->RegParams.TxFIFOLen_4;
    pNicCtx->TxFifo[5].CfgTxDCnt = pNicCtx->RegParams.TxFIFOLen_5;
    pNicCtx->TxFifo[6].CfgTxDCnt = pNicCtx->RegParams.TxFIFOLen_6;
    pNicCtx->TxFifo[7].CfgTxDCnt = pNicCtx->RegParams.TxFIFOLen_7;

    XMPTRACE(XMP_DBG_INFO, ("Registry: TxFIFONum=%d\n",
                            pNicCtx->RegParams.TxFIFONum));
    for ( i=0; i < pNicCtx->RegParams.TxFIFONum; i++ )
    {
      XMPTRACE(XMP_DBG_INFO, ("Registry: Fifo[i] len=%d\n",
                              pNicCtx->TxFifo[i].CfgTxDCnt));
    }

    /* Offloads configuration.                                               */
    xmpNicSetCkoFlags(pNicCtx);

#ifndef XENAMP_LRO
    pNicCtx->RegParams.Lro = 0;
#endif

    /* Set LRO flags according to the configuration.                         */
    if ( pNicCtx->RegParams.Lro )
    { /* TODO: fix LRO with new rx split */
      bis(pNicCtx->Flags, XMP_NIC_FLAGS_LRO);
      XMPTRACE(XMP_DBG_INFO, ("Registry: Large Receive offload enabled\n"));
    }

    if ( pNicCtx->RegParams.rx_split )
    {
      bis(pNicCtx->Flags, XMP_NIC_FLAGS_RX_SPLIT);
      XMPTRACE(XMP_DBG_INFO, ("Registry: RX header split enabled\n"));
    }
#ifdef NEVER
    else
    {
      bis(pNicCtx->Flags, XMP_NIC_FLAGS_RX_SPLIT);
      bis(pNicCtx->Flags, XMP_NIC_FLAGS_RX_SPLIT_COMBINE);
      XMPTRACE(XMP_DBG_INFO, ("Registry: RX buffer split with combining"
                              " enabled\n"));
    }
#endif

    /* Set LSO flags according to the configuration.                         */
    if ( pNicCtx->RegParams.LsoV4 )
    {
      bis(pNicCtx->Flags, XMP_NIC_FLAGS_LSO);
      XMPTRACE(XMP_DBG_INFO, ("Registry: Large Send offload enabled\n"));
    }

    if ( XMP_NIC_RX_SPLIT(pNicCtx) )
      rxdPerBlk=XGE_HAL_RING_RXDS_PER_BLOCK(XGE_HAL_RING_QUEUE_BUFFER_MODE_3);
    else
      rxdPerBlk=XGE_HAL_RING_RXDS_PER_BLOCK(XGE_HAL_RING_QUEUE_BUFFER_MODE_1);

    if ( pNicCtx->RegParams.rx_nbl_coalesce )
      bis(pNicCtx->Flags, XMP_NIC_FLAGS_RX_NBL_CHAIN);

   if ( pNicCtx->RegParams.tx_nbl_coalesce )
      bis(pNicCtx->Flags, XMP_NIC_FLAGS_TX_NBL_CHAIN);

    pNicCtx->RegParams.RxRingSz_0 = 
      (pNicCtx->RegParams.RxRingSz_0 / rxdPerBlk) * rxdPerBlk;
    pNicCtx->RegParams.RxRingSz_1 = 
      (pNicCtx->RegParams.RxRingSz_1 / rxdPerBlk) * rxdPerBlk;
    pNicCtx->RegParams.RxRingSz_2 = 
      (pNicCtx->RegParams.RxRingSz_2 / rxdPerBlk) * rxdPerBlk;
    pNicCtx->RegParams.RxRingSz_3 = 
      (pNicCtx->RegParams.RxRingSz_3 / rxdPerBlk) * rxdPerBlk;
    pNicCtx->RegParams.RxRingSz_4 = 
      (pNicCtx->RegParams.RxRingSz_4 / rxdPerBlk) * rxdPerBlk;
    pNicCtx->RegParams.RxRingSz_5 = 
      (pNicCtx->RegParams.RxRingSz_5 / rxdPerBlk) * rxdPerBlk;
    pNicCtx->RegParams.RxRingSz_6 = 
      (pNicCtx->RegParams.RxRingSz_6 / rxdPerBlk) * rxdPerBlk;
    pNicCtx->RegParams.RxRingSz_7 = 
      (pNicCtx->RegParams.RxRingSz_7 / rxdPerBlk) * rxdPerBlk;

    pNicCtx->RxInfo[0].CfgRxDCnt = pNicCtx->RegParams.RxRingSz_0;
    pNicCtx->RxInfo[1].CfgRxDCnt = pNicCtx->RegParams.RxRingSz_1;
    pNicCtx->RxInfo[2].CfgRxDCnt = pNicCtx->RegParams.RxRingSz_2;
    pNicCtx->RxInfo[3].CfgRxDCnt = pNicCtx->RegParams.RxRingSz_3;
    pNicCtx->RxInfo[4].CfgRxDCnt = pNicCtx->RegParams.RxRingSz_4;
    pNicCtx->RxInfo[5].CfgRxDCnt = pNicCtx->RegParams.RxRingSz_5;
    pNicCtx->RxInfo[6].CfgRxDCnt = pNicCtx->RegParams.RxRingSz_6;
    pNicCtx->RxInfo[7].CfgRxDCnt = pNicCtx->RegParams.RxRingSz_7;

    XMPTRACE(XMP_DBG_INFO, ("Registry: RxRingNum=%d\n",
                            pNicCtx->RegParams.RxRingNum));

    for ( i=0; i < pNicCtx->RegParams.RxRingNum; i++ )
    {
      XMPTRACE(XMP_DBG_INFO, ("Registry: Ring[i] len=%d\n",
                              pNicCtx->RxInfo[i].CfgRxDCnt));
    }

    switch( pNicCtx->RegParams.PriVlan )
    {
    case XENAMP_PRI_ENABLED:
      bis(pNicCtx->Flags, XMP_NIC_FLAGS_8021P);
      XMPTRACE(XMP_DBG_INFO, ("Registry: 8021 Priority is enabled\n"));
      break;

    case XENAMP_VLAN_ENABLED:
      bis(pNicCtx->Flags, XMP_NIC_FLAGS_8021Q);
      XMPTRACE(XMP_DBG_INFO, ("Registry: VLAN is enabled\n"));
      break;

    case XENAMP_PRI_VLAN_ENABLED:
      bis(pNicCtx->Flags, XMP_NIC_FLAGS_8021P);
      bis(pNicCtx->Flags, XMP_NIC_FLAGS_8021Q);
      XMPTRACE(XMP_DBG_INFO, ("Registry: 8021 Priority is enabled\n"));
      XMPTRACE(XMP_DBG_INFO, ("Registry: VLAN is enabled\n"));
      break;
    }

    /* TODO : StatsAutorefresh should be FALSE if StatsRefresh Time = 0.     */
    XMPTRACE(XMP_DBG_INFO, ("Registry: Stats Auto Refresh=%s\n",
                            pNicCtx->RegParams.stats_auto_enable ?
                            "YES" : "NO"));

    XENAMP_NDIS_OBJECT_INIT
      (&pNicCtx->OffloadEncapsulation.Header,
       NDIS_OBJECT_TYPE_OFFLOAD_ENCAPSULATION,
       NDIS_OFFLOAD_ENCAPSULATION_REVISION_1,
       sizeof(NDIS_OFFLOAD_ENCAPSULATION));
    
    pNicCtx->OffloadEncapsulation.IPv4.HeaderSize = 0;
    pNicCtx->OffloadEncapsulation.IPv6.HeaderSize = 0;
    pNicCtx->OffloadEncapsulation.IPv4.Enabled    = 1;
    pNicCtx->OffloadEncapsulation.IPv6.Enabled    = 1;

    switch( pNicCtx->RegParams.offload_encapsulation )
    {
    case 0:
      pNicCtx->OffloadEncapsulation.IPv4.EncapsulationType = 
        NDIS_ENCAPSULATION_IEEE_802_3;
      pNicCtx->OffloadEncapsulation.IPv6.EncapsulationType =
        NDIS_ENCAPSULATION_IEEE_802_3;
      break;

    case 1:
      pNicCtx->OffloadEncapsulation.IPv4.EncapsulationType =
        NDIS_ENCAPSULATION_IEEE_LLC_SNAP_ROUTED;
      pNicCtx->OffloadEncapsulation.IPv6.EncapsulationType =
        NDIS_ENCAPSULATION_IEEE_LLC_SNAP_ROUTED;
      break;

    case 2:
      pNicCtx->OffloadEncapsulation.IPv4.EncapsulationType =
        NDIS_ENCAPSULATION_IEEE_802_3 | 
        NDIS_ENCAPSULATION_IEEE_LLC_SNAP_ROUTED;
      pNicCtx->OffloadEncapsulation.IPv6.EncapsulationType =
        NDIS_ENCAPSULATION_IEEE_802_3 | 
        NDIS_ENCAPSULATION_IEEE_LLC_SNAP_ROUTED;
      break;
    }
    xmpNicInitOffloadCapabilities(pNicCtx, &pNicCtx->OffloadCapabilities);

    /* TODO :       Check for the frame size                               */
    XMPTRACE(XMP_DBG_INFO, ("Registry: MTU=%d\n", pNicCtx->RegParams.mtu));
    if ( pNicCtx->RegParams.mtu > 1514 )
      bis(pNicCtx->Flags, XMP_NIC_FLAGS_JUMBO);

    pNicCtx->MaxFrameSz = pNicCtx->RegParams.mtu + 
      ( (bit(pNicCtx->Flags, XMP_NIC_FLAGS_8021Q)) ? 22 : 14 );

    if ( XMP_NIC_RX_SPLIT(pNicCtx) )
    {
      /* in SCATTER_MODE_A header buffer is split into two parts: 1st part
       * receives ethernet header, 2nd part receives IP+TCP header. Then
       * headers are combined together by copying ETH header just ahead of
       * IP header to get all headers contiguous.
       */
      /* pNicCtx->RxHeaderSz = XMP_ALIGN(pNicCtx->RxHeaderSz, 4); */
    }

    if ( pNicCtx->MaxFrameSz > 1518 )
      bis(pNicCtx->Flags, XMP_NIC_FLAGS_JUMBO);

    if (pNicCtx->RegParams.intr_mode)
    {
      XMPTRACE(XMP_DBG_INFO, ("Registry: MSI enabled\n"));
    }

    if ( pNicCtx->devCtx->RssCpuCount < pNicCtx->RegParams.RxRingNum )
    {
      /* if MSIs enabled, shouldn't we consider MsiMsgCount in scaling back      
       * RSS queues to avoid having Rx rings > MSIs available ???
       */
      XMPTRACE(XMP_DBG_WRN, ("Rx Rings scaled back to RSS cpu count=%d\n",
                             pNicCtx->devCtx->RssCpuCount));
      pNicCtx->RegParams.RxRingNum = pNicCtx->devCtx->RssCpuCount;
    }

    if ( pNicCtx->RegParams.rss_enable )
    {
      if ( pNicCtx->RegParams.RxRingNum > 1 )
      {
         XMPTRACE(XMP_DBG_INFO, ("Registry: RSS enabled; Rx Queues=%d\n",
                                pNicCtx->RegParams.RxRingNum));
      }
      else
      {
        pNicCtx->RegParams.rss_enable = 0;
        XMPTRACE(XMP_DBG_WRN, ("RSS can not be enabled with single RxRing\n"));
      }
    }
    else if ( pNicCtx->RegParams.RxRingNum > 1 )
    {
       XMPTRACE(XMP_DBG_WRN,
               ("RSS is disabled, Single RxRing would be used\n"));
      pNicCtx->RegParams.RxRingNum = 1;
    }

    if ( pNicCtx->RegParams.TxFIFONum > pNicCtx->RegParams.RxRingNum )
    {
      pNicCtx->RegParams.TxFIFONum = pNicCtx->RegParams.RxRingNum;
      XMPTRACE(XMP_DBG_WRN, ("Tx Rings scaled back to Rx Rings count=%d\n",
                             pNicCtx->RegParams.TxFIFONum));
    }
    XMPTRACE(XMP_DBG_INFO, ("nic flags = %08X\n", pNicCtx->Flags));
    Status = NDIS_STATUS_SUCCESS;
  } while(0);

  if (hConfig)
    NdisCloseConfiguration(hConfig);
  XMPTRACE(XMP_DBG_INI, ("<== xmpNicReadConfig: pNic=%p Status=%x\n",
                         pNicCtx, Status));
  return Status;
}

/* 
 * xmpNicLookup
 *
 * Description:
 *   Read the pci config space, enable bus master, ...
 *
 * Arguments:
 *   pNic                - pointer to the nic
 *
 * Return Value:
 *   NDIS_STATUS_SUCCESS, if successful
 *   NDIS_STATUS_ADAPTER_NOT_FOUND
 *   NDIS_STATUS_RESOURCE_CONFLICT
 *   
 *
 * Callers:
 *   called by xmpInitialize
 *
 * IRQL:
 *   PASSIVE_LEVEL
 *
 * Notes:
 *   hMPAdapter must be set in pNic before calling this function.
 *
 */ 
NDIS_STATUS 
xmpNicLookup(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  NDIS_RESOURCE_LIST     *Resources
  )
{
  USHORT                      usTmp;
  ULONG                       ErrorCode=0;
  ULONG                       ErrorValue=0;
  ULONG                       ulTmp;
  NDIS_STATUS                 Status = NDIS_STATUS_ADAPTER_NOT_FOUND;
  CM_PARTIAL_RESOURCE_DESCRIPTOR *pResDesc;
  UCHAR                       Buffer[PCI_COMMON_HDR_LENGTH];
  UCHAR                       ucTmp;

  XMPTRACE(XMP_DBG_INI, ("==> xmpNicLookup: pNicCtx=%p\n", pNicCtx));
  do
  {
    if ( !Resources )
    {
      Status = NDIS_STATUS_RESOURCE_CONFLICT;
      ErrorCode = NDIS_ERROR_CODE_RESOURCE_CONFLICT;
      ErrorValue = ERRLOG_QUERY_ADAPTER_RESOURCES;
      XMPTRACE(XMP_DBG_ERR, ("xmpNicLookup: No Resources!\n"));
      break;
    }

    if ( (ulTmp = NdisMGetBusData(pNicCtx->hMPAdapter,
                                  PCI_WHICHSPACE_CONFIG,
                                  FIELD_OFFSET(PCI_COMMON_CONFIG, VendorID),
                                  &pNicCtx->PciCfg,
                                  PCI_COMMON_HDR_LENGTH))
         != PCI_COMMON_HDR_LENGTH )
    {
      XMPTRACE(XMP_DBG_ERR, ("xmpNicLookup:PCI Config read=%d\n", ulTmp));
      ErrorCode = NDIS_ERROR_CODE_ADAPTER_NOT_FOUND;
      ErrorValue = ERRLOG_READ_PCI_SLOT_FAILED;
      break;
    }

    if ( (pNicCtx->PciCfg.VendorID != XENAMP_PCI_VENDOR_ID) ||
         ((pNicCtx->PciCfg.DeviceID != XENAMP_PCI_DEVICE_ID_WIN) &&
          (pNicCtx->PciCfg.DeviceID != XENAMP_PCI_DEVICE_ID_UNIX) &&
          (pNicCtx->PciCfg.DeviceID != XENAMP_PCI_DEVICE_ID_HERC)) )
    {
      XMPTRACE(XMP_DBG_ERR, ("xmpNicLookup:Vendor=%x Device=%x\n",
                             pNicCtx->PciCfg.VendorID,
                             pNicCtx->PciCfg.DeviceID));
      ErrorCode = NDIS_ERROR_CODE_ADAPTER_NOT_FOUND;
      ErrorValue = ERRLOG_VENDOR_DEVICE_NOMATCH;
      break;
    }

     usTmp = pNicCtx->PciCfg.Command;

    /* TODO: Do we care???   if (usTmp & PCI_ENABLE_WRITE_AND_INVALIDATE)    */
    if ( !bit(usTmp, PCI_ENABLE_BUS_MASTER) )
    {
      /* Bus master is not currently enabled in BIOS. Must enable it now!    */
      XMPTRACE(XMP_DBG_WRN, ("Bus master not enabled in BIOS! PciCommand=%x\n",
                             usTmp));
      bis(usTmp, PCI_ENABLE_BUS_MASTER);

      if ( (ulTmp = NdisMSetBusData(pNicCtx->hMPAdapter,
                                    PCI_WHICHSPACE_CONFIG,
                                    FIELD_OFFSET(PCI_COMMON_CONFIG, Command),
                                    &usTmp, sizeof(USHORT)))
           != sizeof(USHORT) )
      {            
        XMPTRACE(XMP_DBG_ERR, ("NdisMSetBusData (Command) failed ulTmp=%d\n",
                               ulTmp));
        ErrorCode = NDIS_ERROR_CODE_ADAPTER_NOT_FOUND;
        ErrorValue = ERRLOG_WRITE_PCI_SLOT_FAILED;
        break;
      }

      /* Verify that Bus master got enabled.                                 */
      if ( ulTmp = NdisMGetBusData(pNicCtx->hMPAdapter,
                                   PCI_WHICHSPACE_CONFIG,
                                   FIELD_OFFSET(PCI_COMMON_CONFIG, Command),
                                   &usTmp,
                                   sizeof(USHORT) )
           != sizeof(USHORT) )
      {
        XMPTRACE(XMP_DBG_ERR, ("NdisMGetBusData (Command) failed ulTmp=%d\n",
                               ulTmp));
        ErrorCode = NDIS_ERROR_CODE_ADAPTER_NOT_FOUND;
        ErrorValue = ERRLOG_READ_PCI_SLOT_FAILED;
        break;
      }

      if ( !bit(usTmp, PCI_ENABLE_BUS_MASTER))
      {
        XMPTRACE(XMP_DBG_ERR, ("Failed to enable bus master! usTmp=%x\n", 
                               usTmp));
        ErrorCode = NDIS_ERROR_CODE_ADAPTER_DISABLED;
        ErrorValue = ERRLOG_BUS_MASTER_DISABLED;
        break;
      }
    } /* !bit(usTmp, PCI_ENABLE_BUS_MASTER) */

    XMPTRACE(XMP_DBG_INFO, ("Bus master is enabled!\n"));

    /* Look for alloted PCI resources.                                       */
    usTmp = 0;
    for (ulTmp=0; ulTmp < Resources->Count; ulTmp++)
    {
      pResDesc = &Resources->PartialDescriptors[ulTmp];
      switch(pResDesc->Type)
      {
      case CmResourceTypePort:
        break;

      case CmResourceTypeInterrupt:
        usTmp = 1;
        pNicCtx->InterruptLevel = pResDesc->u.Interrupt.Level;
        XMPTRACE(XMP_DBG_INFO, ("InterruptLevel is 0x%x\n",
                                pNicCtx->InterruptLevel));
        break;

      case CmResourceTypeMemory:
        if ( pResDesc->u.Memory.Length )
        {
          if ( !pNicCtx->deviceMem1Phy )
          {
            pNicCtx->deviceMem1Phy = pResDesc->u.Memory.Start.QuadPart;
            pNicCtx->deviceMem1Len = pResDesc->u.Memory.Length;
            XMPTRACE( XMP_DBG_INFO, ("BAR1 = 0x%I64X, len = %d\n",
                                     pNicCtx->deviceMem1Phy,
                                     pNicCtx->deviceMem1Len));
          }
          else if ( !pNicCtx->deviceMem2Phy )
          {
            pNicCtx->deviceMem2Phy = pResDesc->u.Memory.Start.QuadPart;
            pNicCtx->deviceMem2Len = pResDesc->u.Memory.Length;
            XMPTRACE( XMP_DBG_INFO, ("BAR2 = 0x%I64X, len = %d\n",
                                     pNicCtx->deviceMem2Phy,
                                     pNicCtx->deviceMem2Len));
          }
          else if ( !pNicCtx->deviceMem3Phy )
          {
            pNicCtx->deviceMem3Phy = pResDesc->u.Memory.Start.QuadPart;
            pNicCtx->deviceMem3Len = pResDesc->u.Memory.Length;
            XMPTRACE( XMP_DBG_INFO, ("BAR3 = 0x%I64X, len = %d\n",
                                     pNicCtx->deviceMem3Phy,
                                     pNicCtx->deviceMem3Len));
          }
        }
        break;
      }
    } /* for */

    /* Make sure we got all the resources we want.                           */
    if ( !usTmp || !pNicCtx->deviceMem1Len || !pNicCtx->deviceMem2Len)
    {
      Status = NDIS_STATUS_RESOURCE_CONFLICT;
      ErrorCode = NDIS_ERROR_CODE_RESOURCE_CONFLICT;
      if ( !usTmp )
        ErrorValue = ERRLOG_NO_INTERRUPT_RESOURCE;
      else
        ErrorValue = ERRLOG_NO_MEMORY_RESOURCE;
      break;
    }

    Status = NDIS_STATUS_SUCCESS;
  } while(0);

  if ( Status != NDIS_STATUS_SUCCESS )
  {
    XMP_LOG_ERROR(pNicCtx->hMPAdapter,
                  ErrorCode,
                  1,
                  ErrorValue);
  }

  xmpNicInitHwOffloadCapabilities(pNicCtx, &pNicCtx->HwOffloadCapabilities);

  XMPTRACE(XMP_DBG_INI, ("<== xmpNicLookup: pNicCtx=%p Status=%x\n",
                         pNicCtx, Status));
  return (Status);
}

/* 
 * xmpNicTxRxAlloc
 *
 * Description:
 *   This function is to allocate Rx frame buffers for all the configured
 *   Rx rings. It sets up pNicCtx->RxNBLQ[] for each ring.
 *
 * Arguments:
 *   pNic                - pointer to the nic
 *
 * Return Value:
 *   NDIS_STATUS_SUCCESS if successful, NDIS_STATUS_XXX error code if not.
 *
 * IRQL:
 *   PASSIVE_LEVEL
 *
 * Notes:
 */
NDIS_STATUS
xmpNicTxRxAlloc(
  IN  xmpNicCtx_t            *pNicCtx
  )
{
  NDIS_STATUS                 Status;
  xmpTxNBLCtx_t              *pNBLCtx;
  xmpNicRxInfo_t             *pRing;
  xmpQLink_t                  RxNBLQ;
  xmpQLink_t                  RxBLKQ;
  ULONG                       ulval;
  ULONG                       ulval2;
  ULONG                       FrameBufSz;
  ULONG                       HdrsPerPage=0;
  ULONG                       nBlocks=0;
  UINT                        ui;
  UINT                        j;
  NDIS_SG_DMA_DESCRIPTION     DmaDesc = {0};
  NET_BUFFER_LIST_POOL_PARAMETERS ListPoolObj = {0};
  ULONG                       MaxSize = 0;
  U32                         nblCtxCnt;

  XMPTRACE(XMP_DBG_INI, ("==> xmpNicTxRxAlloc: pNicCtx=%p\n", pNicCtx));
  do
  {
    XENAMP_NDIS_OBJECT_INIT(&DmaDesc.Header,
                            NDIS_OBJECT_TYPE_SG_DMA_DESCRIPTION,
                            NDIS_SG_DMA_DESCRIPTION_REVISION_1,
                            sizeof(NDIS_SG_DMA_DESCRIPTION));
    DmaDesc.Flags = NDIS_SG_DMA_64_BIT_ADDRESS;
    MaxSize = pNicCtx->MaxFrameSz;

    if( XMP_NIC_LSO_CONFIG(pNicCtx) && 
        (MaxSize < XMP_NIC_MAX_LSO_SIZE(pNicCtx)) )
    {
      MaxSize = XMP_NIC_MAX_LSO_SIZE(pNicCtx);
    }

    DmaDesc.MaximumPhysicalMapping           = MaxSize;
    DmaDesc.ProcessSGListHandler             = xmpProcessSGListCB;
    DmaDesc.SharedMemAllocateCompleteHandler = NULL;
    if ( (Status =
          NdisMRegisterScatterGatherDma(pNicCtx->hMPAdapter,
                                        &DmaDesc,
                                        &pNicCtx->hMPDma))
         != NDIS_STATUS_SUCCESS )
    {
      ulval = ERRLOG_OUT_OF_SG_RESOURCES;
      XMPTRACE(XMP_DBG_ERR, ("xmpNicRxAlloc: SG DMA init failed = %x\n",
                             Status));
      break;
    }
    pNicCtx->SGListSize = DmaDesc.ScatterGatherListSize;
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_SG_DMA_INIT);

    /* Query and remember cache boundary requirements.                       */
    pNicCtx->CacheFillSize = NdisMGetDmaAlignment(pNicCtx->hMPAdapter);
    XMPTRACE(XMP_DBG_INI, ("xmpNicRxAlloc: SGL Size=%d CacheFillSize=%d\n",
                            pNicCtx->SGListSize, pNicCtx->CacheFillSize));

    ListPoolObj.Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
    ListPoolObj.Header.Revision = NET_BUFFER_LIST_POOL_PARAMETERS_REVISION_1;
    ListPoolObj.Header.Size = sizeof(ListPoolObj);
    ListPoolObj.ProtocolId = 0;
    ListPoolObj.ContextSize = sizeof(xmpRxNBLCtx_t);
    ListPoolObj.fAllocateNetBuffer = TRUE;
    ListPoolObj.PoolTag = XENAMP_MTAG;

    if ( !(pNicCtx->hRxNBLPool = 
           NdisAllocateNetBufferListPool(pNicCtx->hMPAdapter, &ListPoolObj)) )
    {
      Status = NDIS_STATUS_RESOURCES;
      XMPTRACE(XMP_DBG_ERR,
               ("xmpNicRxAlloc: NdisAllocateNetBufferListPool failed\n"));
      break;
    }

    /* Tx related allocations.                                               */
    for (ui = 0; ui < pNicCtx->RegParams.TxFIFONum; ui++)
    { /* Per xmit queue allocations and initialization.                      */
      j = sizeof(xge_hal_dtr_h) * pNicCtx->TxFifo[ui].CfgTxDCnt;

#ifdef XENAMP_TXD_FREE_COALESCE
      if ( !(pNicCtx->TxFifo[ui].dtrFree = 
             (xge_hal_dtr_h *)XF_MALLOC(pNicCtx->hMPAdapter, j)) )
      {
        Status = NDIS_STATUS_RESOURCES;
        XMPTRACE(XMP_DBG_ERR, ("xmpNicTxRxAlloc: dtrFree alloc failed\n"));
        break;
      }
      XF_MCLEAR(pNicCtx->TxFifo[ui].dtrFree, j);
#endif /* XENAMP_TXD_FREE_COALESCE */

      /* Pre-allocate NBL context memory.                                    */
      nblCtxCnt = pNicCtx->TxFifo[ui].CfgTxDCnt * 2;
      j = sizeof(xmpTxNBLCtx_t) * nblCtxCnt;
      if ( !(pNicCtx->TxFifo[ui].NBLCtxBuf =
             XF_MALLOC(pNicCtx->hMPAdapter, j)) )
      {
        Status = NDIS_STATUS_RESOURCES;
        XMPTRACE(XMP_DBG_ERR, ("xmpNicTxRxAlloc: NBLCtxBuf alloc failed\n"));
        break;
      }
      XF_MCLEAR(pNicCtx->TxFifo[ui].NBLCtxBuf, j);

      /* Initialize NBL contexts for this Tx queue.                         */
      for (j = 0, pNBLCtx =(xmpTxNBLCtx_t *) pNicCtx->TxFifo[ui].NBLCtxBuf;
           j < nblCtxCnt; j++, pNBLCtx++)
      {
        pNBLCtx->pNicCtx = pNicCtx;
        pNBLCtx->TxQueue = (U16) ui;
        XF_Q_ADD_TAIL(&pNicCtx->TxFifo[ui].NBLCtxQ, &pNBLCtx->Link);
      }
    }

    if (Status != NDIS_STATUS_SUCCESS)
      break;

    pNicCtx->Diag.BuffSz = (sizeof(UCHAR) * XG_DIAG_PKT_BUFF);
    NdisMAllocateSharedMemory(pNicCtx->hMPAdapter,
                              pNicCtx->Diag.BuffSz,
                              TRUE,
                              (VOID **)&pNicCtx->Diag.pBuffAddr,
                              &pNicCtx->Diag.BuffAddrPhy);
    if ( !pNicCtx->Diag.pBuffAddr )
    {
      Status = NDIS_STATUS_RESOURCES;
      XMPTRACE(XMP_DBG_ERR,
               ("xmpNicTxRxAlloc:SharedMemory for diag buffer failed\n"));
      break;
    }

    /* Rx related allocations.                                               */
    FrameBufSz = pNicCtx->MaxFrameSz;
    pNicCtx->rxBufSz = (U32)FrameBufSz;     /* Does not account for backfill */
    if ( XMP_NIC_RX_SPLIT(pNicCtx) )
    {
      /* Adjust frame buffer size for backfill provision.                    */
      FrameBufSz += pNicCtx->RxBackfillSz;
      /* Make header size pow2 so that even number of headers fit in a page. */
      if ( pNicCtx->RxHeaderSz & (pNicCtx->RxHeaderSz - 1) )
      {
        ulval = 0;
        while(pNicCtx->RxHeaderSz)
        {
          pNicCtx->RxHeaderSz >>= 1;
          ulval++;
        }
        pNicCtx->RxHeaderSz = 1 << ulval;
      }

      HdrsPerPage = PAGE_SIZE / pNicCtx->RxHeaderSz;
      XMPTRACE(XMP_DBG_INFO, ("xmpNicTxRxAlloc:Header Sz=%d Backfill Sz=%d\n",
                              pNicCtx->RxHeaderSz, pNicCtx->RxBackfillSz));
      XMPTRACE(XMP_DBG_INFO, ("xmpNicTxRxAlloc:Header buffers per block=%d\n",
                              HdrsPerPage));
    }

    /* First allocate minimum buffers per ring.                              */
    for (ui = 0; ui < pNicCtx->RegParams.RxRingNum; ui++)
    {
      pRing = &pNicCtx->RxInfo[ui];
      XF_Q_INIT(&RxNBLQ);
      XF_Q_INIT(&RxBLKQ);
      XMPTRACE(XMP_DBG_INFO, ("xmpNicTxRxAlloc:Allocating rx buffers for ring"
                              " %d\n", ui));

      if ( XMP_NIC_RX_SPLIT(pNicCtx) )
      { /* Allocate MDL resources for separate header buffers.               */
        /* Memory for header MDLs array.                                     */
        ulval = sizeof(xmpNicRxHdrMDL_t) * pRing->CfgRxDCnt;
        if ( !(pRing->hdrMDLsArr = 
               (xmpNicRxHdrMDL_t *)XF_MALLOC(pNicCtx->hMPAdapter, ulval)) )
        {
          Status = NDIS_STATUS_RESOURCES;
          XMPTRACE(XMP_DBG_ERR, ("xmpNicTxRxAlloc:hdrMDLsArr[] array alloc"
                                 " failed\n"));
          break;
        }
        XF_MCLEAR(pRing->hdrMDLsArr, ulval);

        /* Memory for header MDLs blocks array.                              */
        nBlocks = ((pRing->CfgRxDCnt + (HdrsPerPage-1)) /
                   HdrsPerPage);
        XMPTRACE(XMP_DBG_INFO, ("xmpNicTxRxAlloc:Ring[%d] Header blocks =%d\n",
                                ui, nBlocks));
        ulval = sizeof(xmpNicRxHdrBlk_t) * (nBlocks + 1);/* extra for NULL 
                                                          * termination.     */
        if ( !(pRing->hdrBlockArr =
               (xmpNicRxHdrBlk_t *)XF_MALLOC(pNicCtx->hMPAdapter, ulval)) )
        {
          Status = NDIS_STATUS_RESOURCES;
          XMPTRACE(XMP_DBG_ERR, ("xmpNicTxRxAlloc:Header Block array alloc"
                                 " failed\n"));
          break;
        }
        XF_MCLEAR(pRing->hdrBlockArr, ulval);

        /* Allocate the header MDLs in blocks all (not min) at once.         */
        if ( (Status = xmpNicHdrMDLsAlloc(pNicCtx,
                                          pNicCtx->RxHeaderSz,
                                          pRing->CfgRxDCnt,
                                          pRing->hdrMDLsArr,
                                          pRing->hdrBlockArr))
             != NDIS_STATUS_SUCCESS )
        {
          XMPTRACE(XMP_DBG_ERR, ("xmpNicTxRxAlloc: xmpNicHdrMDLAlloc failed "
                                 "Status%lx\n", Status));
          break;
        }
      }
      XMPTRACE(XMP_DBG_INFO, ("xmpNicTxRxAlloc:ring[%d] %d Rx Header MDL "
                              "blocks allocated\n", ui, nBlocks));

      if ( (Status = xmpNicRxNBLBlkAlloc(pNicCtx, FrameBufSz, ui,
                                         XENAMP_MIN_RXDS,
                                         &RxNBLQ, &RxBLKQ) )
           != NDIS_STATUS_SUCCESS )
      {
        XMPTRACE(XMP_DBG_ERR, ("xmpNicTxRxAlloc:xmpNicRxNBLBlkAlloc"
                               "failed, Status=%lx\n", Status));
        break;
      }

      XF_Q_JOIN(&pRing->RxNBLQ, &RxNBLQ);
      XF_Q_JOIN(&pNicCtx->RxBufBlkQ, &RxBLKQ);
      pRing->RxDCnt = XENAMP_MIN_RXDS;
    }
    if ( Status != NDIS_STATUS_SUCCESS )
      break;

    /* Allocate more to equal the configured number of buffers per ring.     */
    for (ui = 0; ui < pNicCtx->RegParams.RxRingNum; ui++)
    {
      pRing = &pNicCtx->RxInfo[ui];

      if ( (pRing->CfgRxDCnt) <= XENAMP_MIN_RXDS )
        continue;

      XF_Q_INIT(&RxNBLQ);
      XF_Q_INIT(&RxBLKQ);

      if ( (Status == NDIS_STATUS_SUCCESS) &&
           ((Status = xmpNicRxNBLBlkAlloc(pNicCtx, FrameBufSz, ui,
                                          pRing->CfgRxDCnt - XENAMP_MIN_RXDS,
                                          &RxNBLQ, &RxBLKQ))
            == NDIS_STATUS_SUCCESS) )
      {
        XF_Q_JOIN(&pRing->RxNBLQ, &RxNBLQ);
        XF_Q_JOIN(&pNicCtx->RxBufBlkQ, &RxBLKQ);
        pRing->RxDCnt = pNicCtx->RxInfo[ui].CfgRxDCnt;
        XMPTRACE(XMP_DBG_INFO, ("xmpNicTxRxAlloc: %d more Rx Header MDL blocks"
                                " allocated\n", nBlocks));
        continue;
      }

      XMPTRACE(XMP_DBG_WRN, ("xmpNicTxRxAlloc: xmpNicRxNBLBlkAlloc failed "
                             "Status%lx\n", Status));
      break;
    }

    Status = NDIS_STATUS_SUCCESS;
  } while(0);

  XMPTRACE(XMP_DBG_INI, ("<== xmpNicTxRxAlloc: pNicCtx=%p Status=%lx\n",
                         pNicCtx, Status));
  return Status;
}

/* 
 * xmpNicShmAllocCompleteCB
 *
 * Description:
 *   Completion handler for NdisMAllocateSharedMemoryAsync
 *
 * Arguments:
 *   MPNicCtx                - pointer to our nic context
 *   pVa                     - virtual addr of memory alloc'd
 *   Pa                      - physical addr of memory alloc'd
 *   Len                     - length (bytes) of memory alloc'd
 *   pContext                - async call context
 *
 * Return Value:
 *   none
 *
 * Callers:
 *   Called by NDIS
 *
 * IRQL:
 *   DISPATCH Level
 *
 * Notes:
 *  Currently not used
 *
 */ 
VOID 
xmpNicShmAllocCompleteCB(
  IN  NDIS_HANDLE             MPNicCtx,
  IN  VOID                   *pVa,
  IN  NDIS_PHYSICAL_ADDRESS  *Pa,
  IN  ULONG                   Len,
  IN  VOID                   *pContext
  )
{
  ASSERT(FALSE);
}

/* 
 * xmpProcessSGListCB
 *
 * Description:
 *   Completion handler for NdisMAllocateNetBufferSGList. Here we put the
 *   frame in the Tx queue of the nic to be transmitted by calling
 *   halQueueSendPkt. Eventually, the Tx completion is indicated by HAL
 *   through OSM_SEND_COMPLETE, which in turn calls xmpNicTxComplete().
 *
 * Arguments:
 *   pDO                     - should be ignored
 *   Reserved                - should be ignored
 *   pSGList                 - SG List describing NetBuffer
 *   pContext                - NetBuf context pointer (async call context)
 *
 * Return Value:
 *   none
 *   
 * Callers:
 *   Called by NDIS in response to a call to NdisMAllocateNetBufferSGList in
 *   xmpNicTxNBL()
 *
 * IRQL:
 *   DISPATCH Level
 *
 * Notes:
 *  SGL for the NetBuf may or may NOT reside in the pre-alloc'd buffer passed
 *  in NdisMAllocateNetBufferSGList.
 *  The SGL list is freed up in xmpHalTxCompleteNetBuf().
 *
 */
VOID
xmpProcessSGListCB(
  IN  DEVICE_OBJECT          *pDO,
  IN  VOID                   *Reserved,
  IN  SCATTER_GATHER_LIST    *pSGList,
  IN  VOID                   *pContext
  )
{
  SCATTER_GATHER_ELEMENT     *pSGE;
  xmpTxNBufCtx_t             *pNBufCtx = (xmpTxNBufCtx_t *) pContext;
  xmpTxNBLCtx_t              *pNBLCtx;
  xmpNicTxFifo_t             *pFifo;
  xge_hal_dtr_h               dtr;
  NET_BUFFER                 *pNetBuf;
  HAL_STATUS                  halStatus;
  ULONG                       Offset;
  UINT                        ui;
  UINT                        frag;

  XMPTRACE(XMP_DBG_TX, ("==> xmpProcessSGListCB\n"));
  pNBLCtx = pNBufCtx->pNBLCtx;
  dtr     = pNBufCtx->dtr;
  pNetBuf = pNBufCtx->pNetBuf;
  pNBufCtx->pSGList = pSGList;

  pFifo = pNBufCtx->pFifo;

#ifdef NEVER
  if ( pNBLCtx->CkoInfo )
    xge_hal_fifo_dtr_cksum_set_bits(dtr, pNBLCtx->CkoInfo);
#else
  if ( pNBLCtx->CkoInfo )
    xge_hal_fifo_dtr_cksum_set_bits(dtr, pNBufCtx->pNicCtx->txdCkoBits);
#endif

  if ( pNBLCtx->vTagInfo )
  {
#if DBG
    NDIS_PACKET_8021Q_INFO inf;
    inf.Value = NET_BUFFER_LIST_INFO(pNBLCtx->pNBL,
                                     Ieee8021QNetBufferListInfo);
    XMPTRACE(XMP_DBG_RX, ("TX: vlan=%x Priority=%x CanonId=%x TxD Tag=%x\n",
                          inf.TagHeader.VlanId,
                          inf.TagHeader.UserPriority,
                          inf.TagHeader.CanonicalFormatId,
                          pNBLCtx->vTagInfo));
#endif
    xge_hal_fifo_dtr_vlan_set(dtr, pNBLCtx->vTagInfo);
  }

  if ( pNBLCtx->Mss )
    xge_hal_fifo_dtr_mss_set(dtr, (int)pNBLCtx->Mss);

  /* Offset is the effective offset where in SG list we have the frame to send.
   * Therefore, we need to skip the elements that fall within this offset.
   */
  Offset = NET_BUFFER_CURRENT_MDL_OFFSET(pNetBuf);
  frag = 0;
  for (ui = 0; ui < pSGList->NumberOfElements; ui++)
  {
    pSGE = &pNBufCtx->pSGList->Elements[ui];
    if (pSGE->Length <= Offset)
      Offset -= pSGE->Length;                 /* Skip this element entirely */
    else
    {
      xge_hal_fifo_dtr_buffer_set(pFifo->halChannel, dtr, frag++,
                                  pSGE->Address.QuadPart + Offset,
                                  pSGE->Length - Offset);
      ui++;
      break;
    }
  }

  for (; ui < pSGList->NumberOfElements; ui++)
  {
    pSGE = &pNBufCtx->pSGList->Elements[ui];
    xge_hal_fifo_dtr_buffer_set(pFifo->halChannel, dtr, frag++,
                                pSGE->Address.QuadPart, pSGE->Length);
  }

  /* TODO: 8021P/Q (VlanID), Interrupt type??? */
#if NEVER
  if ( pNBufCtx->pNBLCtx->Mss )
  {
    ULONG nblen = NET_BUFFER_DATA_LENGTH(pNetBuf);
    for (ui = 0; ui < pSGList->NumberOfElements; ui++)
    {
      pSGE = &pNBufCtx->pSGList->Elements[ui];
      XMPTRACE(XMP_DBG_INFO, ("xmpProcessSGListCB: SGE(Len=%d ptr=%I64x)\n",
                              pSGE->Length, pSGE->Address.QuadPart));
    }
  }
  pNBufCtx->SglCnt++;
  if ( pNBufCtx->SglCnt > 1 )
  {
    XMPTRACE(XMP_DBG_INFO, ("xmpProcessSGListCB: NBuf SGList count=%d\n",
                            pNBufCtx->SglCnt));
  }
#endif

  pFifo->Stats.txdPost++;
  xge_hal_fifo_dtr_post(pFifo->halChannel, dtr);

#if NEVER
  if ( pNBufCtx->pNBLCtx->Mss )
  {
    int i;
    xge_hal_fifo_txd_t *txd = (xge_hal_fifo_txd_t *)pNBufCtx->dtr;
    for ( i=0; i < frag; i++, txd++ )
    {
      XMPTRACE(XMP_DBG_INFO, ("xmpProcessSGListCB: DTR control_1=%I64x\n",
                              txd->control_1));
    }
  }
#endif

  XMPTRACE(XMP_DBG_TX, ("<== xmpProcessSGListCB\n"));
}

/* 
 * xmpNicTxNBL
 *
 * Description:
 *   This function processes NBL- allocates NetBuf contexts for each NetBuf of
 *   of the NBL, and calls NDIS to allocate/prepare SGL for each NetBuf. This
 *   function manages the queue of outstanding NBLs waiting to be sent out.
 *
 * Arguments:
 *   pNic                    - NIC pointer
 *   pNBL                    - pointer to NBL to xmit
 *   DispatchLevel           - TRUE, if called at DISPATCH_LEVEL
 *
 * Return Value:
 *   none
 *   
 * Callers:
 *   Called by xenaMpSendNetBufferLists to xmit NBL passed down by NDIS.
 *   In this case, pNBL is not null.
 *   Also called by Tx completion handler, watchdog to flush NBL PendingQ,
 *   in which case pNBL is null.
 *
 * IRQL:
 *   <= DISPATCH Level
 *
 * Notes:
 *
 */
VOID
xmpNicTxNBL(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  NET_BUFFER_LIST        *pNBL,
  IN  ULONG                   SendFlags
  )
{
  xmpTxNBLCtx_t              *pNBLCtx = NULL;
  xmpTxNBufCtx_t             *pNBufCtx = NULL;
  xmpNicTxFifo_t             *pFifo;
  xmpQLink_t                 *link;
  xmpNicRssParams_t          *pParams;
  xmpQLink_t                  SendQ;
  UINT                        Queue = 0;
  xge_hal_dtr_h               dtr;
  KIRQL                       OldIrql=(KIRQL)0;
  BOOLEAN                     DispatchLevel;
  BOOLEAN                     Serve = 0;
  NDIS_STATUS                 Status = NDIS_STATUS_SUCCESS;
  ULONG                       hash = 0;

  XMPTRACE(XMP_DBG_TX, ("==> xmpNicTxNBL:pNicCtx=%p pNBL=%p\n",
                        pNicCtx, pNBL));

  
  if ( XMP_NIC_RSS_IS_ENABLED(pNicCtx) &&
       (hash = NET_BUFFER_LIST_GET_HASH_VALUE(pNBL)) )
  {
    pParams = &pNicCtx->RssParamsWS;
    hash &= pParams->HashBitsSz;
    ASSERT( hash < pParams->TableSz );
    Queue = pNicCtx->RssITable[hash];
    if ( Queue >= pNicCtx->RegParams.TxFIFONum )
      Queue = 0;
  }
  pFifo = &pNicCtx->TxFifo[Queue];

  /* Raise to DISPATCH IRQ, if not already                                  */
  DispatchLevel = NDIS_TEST_SEND_AT_DISPATCH_LEVEL(SendFlags); 
  if (!DispatchLevel)
    NDIS_RAISE_IRQL_TO_DISPATCH(&OldIrql);
  ASSERT( NDIS_CURRENT_IRQL() == DISPATCH_LEVEL );

#ifdef XENAMP_TXDBG
  {
    NET_BUFFER *nb;
    MDL                        *mdl;
    ULONG                       mdl_len;
    NDIS_TCP_IP_CHECKSUM_NET_BUFFER_LIST_INFO nblCkoInfo; 
   
    nb = NET_BUFFER_LIST_FIRST_NB(pNBL);

    nblCkoInfo.Value =
      PtrToUlong(NET_BUFFER_LIST_INFO(pNBL,
                                    TcpIpChecksumNetBufferListInfo));

    }
#endif

  do
  {
    if ( (Status = xmpNicTxNBLValid(pNicCtx, pNBL))
         != NDIS_STATUS_SUCCESS )
    { /* NBL failed validation. has been tx-completed already.               */
      XMPTRACE(XMP_DBG_ERR, ("xmpNicTxNBLValidate:NBL validation failed\n"));
      pFifo->Stats.TxNBLJunk++;
      break;
    }

    XF_DPR_GET_SLOCK(&pFifo->TxLock);
      if ( !XMP_NIC_TX_FIFO_OK_TO_TX(pFifo) )
      {
        XF_DPR_FREE_SLOCK(&pFifo->TxLock);
        Status = NDIS_STATUS_NOT_ACCEPTED;
        break;
      }

#ifdef NDISTEST65_STATS_TEST_FAILS
      if ( pFifo->TxUnderrun )
      {
        XF_DPR_FREE_SLOCK(&pFifo->TxLock);
        Status = NDIS_STATUS_RESOURCES;
        pFifo->Stats.TxNBLUnder++;
        XMPTRACE(XMP_DBG_WRN, ("xmpNicTxNBL:TxUnderrun: reject NBL\n"));
        break;
      }
#endif

      XF_Q_DEL_HEAD(&pFifo->NBLCtxQ, link);
      if ( !link )
      { /* out of NBL contexts, can't even queue this NBL.                   */
        XF_DPR_FREE_SLOCK(&pFifo->TxLock);
        Status = NDIS_STATUS_RESOURCES;
        pFifo->Stats.TxNBLUnder++;
        /* XMPTRACE(XMP_DBG_WRN, ("xmpNicTxNBL:Out of NBL Contexts\n")); */
        /* ASSERT(FALSE); */
        break;
      }
      pNBLCtx = CONTAINING_RECORD(link, xmpTxNBLCtx_t, Link);
      pFifo->Stats.TxNBLCount++;
      /* Initialize NBL context.                                             */
      pNBLCtx->pNicCtx = pNicCtx;
      pNBLCtx->pNBL = pNBL;
      pNBLCtx->pNetBuf = NET_BUFFER_LIST_FIRST_NB(pNBL);
      pNBLCtx->RefCnt = 1;
      pNBLCtx->TxQueue = (USHORT)Queue;
      XMP_GET_NBL_CSUM_INFO(pNBLCtx);
      XMP_GET_NBL_8021Q_INFO(pNBLCtx);
      XMP_GET_NBL_LSO_GSO_INFO(pNBLCtx);
      NET_BUFFER_LIST_COMPLETION_STATUS(pNBL) = NDIS_STATUS_SUCCESS;
      XF_ATOMIC_INC31(pFifo->OsSendCnt);

      XF_Q_ADD_TAIL(&pFifo->NBLWaitQ, &pNBLCtx->Link);

      if ( !pFifo->InService )
      {
        pFifo->InService = 1;
        Serve = 1;
      }
    XF_DPR_FREE_SLOCK(&pFifo->TxLock);
  } while(0);

  if ( Status != NDIS_STATUS_SUCCESS )
  {
    NET_BUFFER_LIST_COMPLETION_STATUS(pNBL) = Status;
    XENAMP_TX_NBL_COMPLETE_UPCALL(pNicCtx, pNBL,
                                  NDIS_SEND_COMPLETE_FLAGS_DISPATCH_LEVEL);
  }

  if ( Serve )
    xmpNicTxServe(pNicCtx, pFifo);

  if ( !DispatchLevel )
    NDIS_LOWER_IRQL(OldIrql, DISPATCH_LEVEL);

  XMPTRACE(XMP_DBG_TX, ("<== xmpNicTxNBL\n"));
}

// NK- Fix comments below. comments are for transmit
/* 
 * xmpNicRxIndicate
 *
 * Description:
 *   This function processes NBL- allocates NetBuf contexts for each NetBuf of
 *   of the NBL, and calls NDIS to allocate/prepare SGL for each NetBuf. This
 *   function manages the queue of outstanding NBLs waiting to be sent out.
 *
 * Arguments:
 *   pNic                    - NIC pointer
 *   pNBL                    - pointer to NBL to xmit
 *   DispatchLevel           - TRUE, if called at DISPATCH_LEVEL
 *
 * Return Value:
 *   none
 *   
 * Callers:
 *   Called by xenaMpSendNetBufferLists to xmit NBL passed down by NDIS.
 *   In this case, pNBL is not null.
 *   Also called by Tx completion handler, watchdog to flush NBL PendingQ,
 *   in which case pNBL is null.
 *
 * IRQL:
 *   <= DISPATCH Level
 *
 * Notes:
 *
 */ 
VOID
xmpNicRxIndicate(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  UINT                    RingNum
  )
{
  xmpNicRxInfo_t             *pRxInfo;
  NET_BUFFER_LIST            *pNBL = (NET_BUFFER_LIST *) 0;
  UINT                        NBLCount;

  pRxInfo = &(pNicCtx->RxInfo[RingNum]);

  XMPTRACE(XMP_DBG_RX, ("==> xmpNicRxIndicate\n"));
  ASSERT( NDIS_CURRENT_IRQL() == DISPATCH_LEVEL );

  do
  {
    XF_DPR_GET_SLOCK(&pRxInfo->RxLock);
      if ( !XMP_NIC_RX_RING_OK_TO_RX(pRxInfo) )
      {
        XF_DPR_FREE_SLOCK(&pRxInfo->RxLock);
        break;
      }

      pNBL = pRxInfo->pNBL;
      pRxInfo->pNBL = pRxInfo->pPrvNBL = NULL;
      NBLCount = pRxInfo->NBLCount;
      pRxInfo->NBLCount = 0;
      pRxInfo->UlpNBLCnt += NBLCount;
      pRxInfo->Stats.RxCnt += NBLCount;
      if ( NBLCount > (UINT) pRxInfo->Stats.MaxNBLUp )
        pRxInfo->Stats.MaxNBLUp = (LONG) NBLCount;
    XF_DPR_FREE_SLOCK(&pRxInfo->RxLock);

    if (pNBL)
    {
      if ( bit(pNicCtx->State, XMP_NIC_STATE_NOLINK) )
        xmpNicIndicateStatus(pNicCtx, TRUE);

#ifdef XMP_RX_NBL_AUDIT
      {
        NET_BUFFER_LIST *a_nbl = pNBL;
        while( a_nbl )
        {
          xmpDbgVerifyNBL(pNicCtx, a_nbl );
          a_nbl = NET_BUFFER_LIST_NEXT_NBL(a_nbl);
        }
      }
#endif
      NdisMIndicateReceiveNetBufferLists(pNicCtx->hMPAdapter,
                                         pNBL,
                                         NDIS_DEFAULT_PORT_NUMBER,
                                         NBLCount,
                                         NDIS_RECEIVE_FLAGS_DISPATCH_LEVEL 
                                         );
    }
  } while(0);
  XMPTRACE(XMP_DBG_RX, ("<== xmpNicRxIndicate\n"));
}

VOID
xmpNicSetMPState(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  ULONG                   State,
  IN  BOOLEAN                 DispatchLevel
  )
{
  xmpNicTxFifo_t             *pTxFifo;
  xmpNicRxInfo_t             *pRxRing;
  KIRQL                       OldIrql;
  UINT                        ui;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpNicSetMPState: Old=%d New=%d\n",
                        pNicCtx->mpState, State));
  //  if ( !DispatchLevel )
  //NDIS_RAISE_IRQL_TO_DISPATCH(&OldIrql);

  XF_GET_SLOCK(&pNicCtx->lock);
    do
    {
      pNicCtx->mpState = State;
      if ( (pNicCtx->mpState == XMP_NIC_MPSTATE_PAUSING) ||
           (pNicCtx->mpState == XMP_NIC_MPSTATE_RESET) )
      {
        /* Stop accepting new send requests.                                 */
        bic(pNicCtx->State, XMP_NIC_STATE_OK_TO_TX | XMP_NIC_STATE_OK_TO_RX);
        for (ui = 0; ui < pNicCtx->RegParams.TxFIFONum; ui++)
        {
          pTxFifo = &pNicCtx->TxFifo[ui];
          XF_GET_SLOCK(&pTxFifo->TxLock);
            bic(pTxFifo->State, XMP_TX_FIFO_STATE_OK_TO_TX);
          XF_FREE_SLOCK(&pTxFifo->TxLock);
        }

        /* Suspend Rx Indications.                                           */
        for (ui = 0; ui < pNicCtx->RegParams.RxRingNum; ui++)
        {
          pRxRing = &pNicCtx->RxInfo[ui];
          XF_GET_SLOCK(&pRxRing->RxLock);
            bic(pRxRing->State, XMP_RX_RING_STATE_OK_TO_RX);
          XF_FREE_SLOCK(&pRxRing->RxLock);
        }
      }
      else if ( pNicCtx->mpState == XMP_NIC_MPSTATE_RUNNING )
      {
        bis(pNicCtx->State, XMP_NIC_STATE_OK_TO_TX | XMP_NIC_STATE_OK_TO_RX);
        for (ui = 0; ui < pNicCtx->RegParams.TxFIFONum; ui++)
        {
          pTxFifo = &pNicCtx->TxFifo[ui];
          XF_GET_SLOCK(&pTxFifo->TxLock);
            bis(pTxFifo->State, XMP_TX_FIFO_STATE_OK_TO_TX);
          XF_FREE_SLOCK(&pTxFifo->TxLock);
        }
        for (ui = 0; ui < pNicCtx->RegParams.RxRingNum; ui++)
        {
          pRxRing = &pNicCtx->RxInfo[ui];
          XF_GET_SLOCK(&pRxRing->RxLock);
            bis(pRxRing->State, XMP_RX_RING_STATE_OK_TO_RX);
          XF_FREE_SLOCK(&pRxRing->RxLock);
        }
      }
    } while(0);
    XF_FREE_SLOCK(&pNicCtx->lock);

    //  if ( !DispatchLevel )
    //    NDIS_LOWER_IRQL(OldIrql, DISPATCH_LEVEL);
  XMPTRACE(XMP_DBG_INFO, ("<== xmpNicSetMPState:New=%d\n", pNicCtx->mpState));
}

VOID
xmpNicIndicateStatus(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  BOOLEAN                 bLink
  )
{
  NDIS_MEDIA_CONNECT_STATE    MediaState;
  BOOLEAN                     bIndicate = FALSE;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpNicIndicateStatus: nic %p: Link=%s\n",
                          pNicCtx, bLink ? "UP" : "DOWN"));

  XF_GET_SLOCK(&pNicCtx->lock);
    if ( bLink )
    {
      bic(pNicCtx->State, XMP_NIC_STATE_NOLINK);
      MediaState = MediaConnectStateConnected;
    }
    else
    {
      bis(pNicCtx->State, XMP_NIC_STATE_NOLINK);
      MediaState = MediaConnectStateDisconnected;
    }
    if ( pNicCtx->ReportedState != MediaState )
    {
      pNicCtx->ReportedState = MediaState;
      bIndicate = TRUE;
    }
  XF_FREE_SLOCK(&pNicCtx->lock);
  if ( bIndicate )
  {
    NDIS_LINK_STATE  LinkState = {0};
    NDIS_STATUS_INDICATION StatusObj = {0};
    LinkState.MediaConnectState = MediaState;
    LinkState.MediaDuplexState = MediaDuplexStateFull;
    LinkState.XmitLinkSpeed = LinkState.RcvLinkSpeed = XMP_NIC_MAX_SPEED(pNic);
    XENAMP_NDIS_OBJECT_INIT
      (&StatusObj.Header,
       NDIS_OBJECT_TYPE_STATUS_INDICATION,
       NDIS_STATUS_INDICATION_REVISION_1,
       sizeof(NDIS_STATUS_INDICATION));
    StatusObj.SourceHandle = pNicCtx->hMPAdapter;
    StatusObj.StatusCode = NDIS_STATUS_LINK_STATE;
    StatusObj.StatusBuffer = &LinkState;
    StatusObj.StatusBufferSize = sizeof(LinkState);

    XMP_NIC_REFERENCE(pNicCtx);
    XMPTRACE(XMP_DBG_INFO, ("Link state changed: Notify NDIS\n"));
    NdisMIndicateStatusEx(pNicCtx->hMPAdapter, &StatusObj);
    XMP_NIC_DEREFERENCE(pNicCtx);
  }

  XMPTRACE(XMP_DBG_INFO, ("<== xmpNicIndicateStatus:nic %p\n", pNicCtx));
}

VOID
xmpNicTxFifoDrainQ(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  UINT                    FifoNum,
  IN  BOOLEAN                 DispatchLevel
  )
{
  xmpNicTxFifo_t             *pFifo;
  xmpTxNBLCtx_t              *pNBLCtx = NULL;
  xmpTxNBufCtx_t             *pNBufCtx = NULL;
  NET_BUFFER_LIST            *pNBL;
  xmpQLink_t                 *link;
  KIRQL                       OldIrql=(KIRQL)0;

  pFifo = &(pNicCtx->TxFifo[FifoNum]);

  XMPTRACE(XMP_DBG_INFO, ("==> xmpNicTxFifoDrainQ: nic %p\n", pNicCtx));

  if ( !DispatchLevel )
    NDIS_RAISE_IRQL_TO_DISPATCH(&OldIrql);

  XF_DPR_GET_SLOCK(&pFifo->TxLock);
    do
    {
      XF_Q_DEL_HEAD(&pFifo->NBLWaitQ, link);
      if ( !link )
        break;
      pNBLCtx = CONTAINING_RECORD(link, xmpTxNBLCtx_t, Link);
      pNBL = pNBLCtx->pNBL;
      NET_BUFFER_LIST_COMPLETION_STATUS(pNBL) = NDIS_STATUS_NOT_ACCEPTED;
      /* Release spin lock before dereferencing NBL because NBL may be
       * returned (send completed).
       */
      XF_DPR_FREE_SLOCK(&pFifo->TxLock);
      if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_TX_NBL_CHAIN) )
        XMP_TX_NBL_DEREFERENCE_COAL(pNicCtx, pFifo, pNBLCtx, TRUE, FALSE);
      else
        XMP_TX_NBL_DEREFERENCE(pNicCtx, pFifo, pNBLCtx, TRUE);
         
      XF_DPR_GET_SLOCK(&pFifo->TxLock);
    } while(0);
    XF_DPR_FREE_SLOCK(&pFifo->TxLock);

   if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_TX_NBL_CHAIN) )     
     XMP_TX_COMPLETE_FINAL(pNicCtx, pFifo, TRUE);

  if ( !DispatchLevel )
    NDIS_LOWER_IRQL(OldIrql, DISPATCH_LEVEL);

  XMPTRACE(XMP_DBG_INFO, ("<== xmpNicTxFifoDrainQ: nic %p\n", pNicCtx));
}

VOID xmpNicTxFlushCB(
  IN VOID                    *pContext,
  IN NDIS_HANDLE              Handle
  )
{
  xmpNicTxFifo_t             *pFifo;
  BOOLEAN                     serve = FALSE;
  NDIS_STATUS                 Status;

  /*  XMPTRACE(XMP_DBG_INFO, ("==> xmpNicTxFlush:pFifo=%p\n", pContext));*/
  pFifo = (xmpNicTxFifo_t *)pContext;
  ASSERT(pFifo->TxWIQueued);

  XF_GET_SLOCK(&pFifo->TxLock);
    pFifo->TxWIQueued = FALSE;
    if ( !pFifo->InService)
    {
      pFifo->InService = 1;
      serve = TRUE;
    }
  XF_FREE_SLOCK(&pFifo->TxLock);

  if ( serve )
    xmpNicTxServe((xmpNicCtx_t *)pFifo->pNicCtx, pFifo);

  XF_GET_SLOCK(&pFifo->TxLock);
    if ( XF_Q_IS_NULL(&pFifo->NBLWaitQ) )
      pFifo->TxUnderrun = FALSE;
  XF_FREE_SLOCK(&pFifo->TxLock);
  /*  XMPTRACE(XMP_DBG_INFO, ("<== xmpNicTxFlush:pFifo=%p\n", pContext));*/
}

/* 
 * xmpNicSetUtilParams
 *
 * Description:
 *   This function is used to set Tx and Rx utilization Params.          
 *
 * Arguments:
 *   pNic                - pointer to the nic
 *
 * Return Value:
 *   NDIS_STATUS_SUCCESS if successful, NDIS_STATUS_XXX error code if not.
 *
 * IRQL:
 *   PASSIVE_LEVEL
 *
 * Notes:
 */
#ifdef oHAL
VOID
xmpNicSetUtilParams(
  IN  xmpNicCtx_t            *pNicCtx
  )
{
  NDIS_STATUS                 Status;
  xena_utilz_t                utilzParams;
  UINT                        i;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpNicSetUtilParams\n"));

  for ( i = 0; i < pNicCtx->RegParams.TxFIFONum; i++ )
  {
   utilzParams.urange_a =     (UCHAR) pNicCtx->RegParams.tx_urange_a;
   utilzParams.ufc_a =        (USHORT)pNicCtx->RegParams.tx_ufc_a;
   utilzParams.urange_b =     (UCHAR) pNicCtx->RegParams.tx_urange_b;
   utilzParams.ufc_b =        (USHORT)pNicCtx->RegParams.tx_ufc_b;
   utilzParams.urange_c =     (UCHAR) pNicCtx->RegParams.tx_urange_c;
   utilzParams.ufc_c =        (USHORT)pNicCtx->RegParams.tx_ufc_c;
   utilzParams.urange_d =     (UCHAR) 100;
   utilzParams.ufc_d =        (USHORT)pNicCtx->RegParams.tx_ufc_d;
   utilzParams.periodic =     (BOOL) pNicCtx->RegParams.tx_utilz_periodic;
   utilzParams.timer_val =    (BOOL) pNicCtx->RegParams.tx_timer_val;
#ifdef HAL_LATEST
   utilzParams.ac_disable =    (BOOL) pNicCtx->RegParams.tx_ac_disable;
#endif
   halSetDMATxUtilz ( pNic, 0, &utilzParams );
  }
  for ( i = 0; i < pNicCtx->RegParams.TxFIFONum; i++ )
  {
   utilzParams.urange_a =     (UCHAR) pNicCtx->RegParams.rx_urange_a;
   utilzParams.ufc_a =        (USHORT)pNicCtx->RegParams.rx_ufc_a;
   utilzParams.urange_b =     (UCHAR) pNicCtx->RegParams.rx_urange_b;
   utilzParams.ufc_b =        (USHORT)pNicCtx->RegParams.rx_ufc_b;
   utilzParams.urange_c =     (UCHAR) pNicCtx->RegParams.rx_urange_c;
   utilzParams.ufc_c =        (USHORT)pNicCtx->RegParams.rx_ufc_c;
   utilzParams.urange_d =     (UCHAR) 100;
   utilzParams.ufc_d =        (USHORT)pNicCtx->RegParams.rx_ufc_d;
   utilzParams.periodic =     (BOOL) pNicCtx->RegParams.rx_utilz_periodic;
   utilzParams.timer_val =    (BOOL) pNicCtx->RegParams.rx_timer_val;
#ifdef HAL_LATEST
   utilzParams.ac_disable =    (BOOL) pNicCtx->RegParams.rx_ac_disable;
#endif
   halSetDMARxUtilz ( pNic, 0, &utilzParams ); 
  }
  XMPTRACE(XMP_DBG_INFO, ("<== xmpNicSetUtilParams\n"));
}
#endif

/* ============================================================================
 * Private static functions
 * ==========================================================================*/

VOID
xmpNicTxServe(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  xmpNicTxFifo_t         *pFifo
  )
{
  xmpTxNBLCtx_t              *pNBLCtx = NULL;
  xmpTxNBufCtx_t             *pNBufCtx = NULL;
  xmpQLink_t                 *link;
  NET_BUFFER_LIST            *pNBL;
  xmpQLink_t                  SendQ;
  xge_hal_dtr_h               dtr;
  xge_hal_status_e            halStatus = XGE_HAL_OK;
  BOOLEAN                     keepGoing = 1;
  NDIS_STATUS                 Status = NDIS_STATUS_SUCCESS;
  KIRQL                       OldIrql = DISPATCH_LEVEL;

  ASSERT(pFifo->InService == 1);
  ASSERT( NDIS_CURRENT_IRQL() <= DISPATCH_LEVEL );

  if (NDIS_CURRENT_IRQL() != DISPATCH_LEVEL)
    NDIS_RAISE_IRQL_TO_DISPATCH(&OldIrql);

  while( keepGoing )
  {
    XF_DPR_GET_SLOCK(&pFifo->TxLock);
      XF_Q_DEL_HEAD(&pFifo->NBLWaitQ, link);
      if ( !link )
      {
        pFifo->InService = 0;
        XF_DPR_FREE_SLOCK(&pFifo->TxLock);
        break;
      }
    XF_DPR_FREE_SLOCK(&pFifo->TxLock);

    pNBLCtx = CONTAINING_RECORD(link, xmpTxNBLCtx_t, Link);
    pNBL = pNBLCtx->pNBL;

    /* Allocate a TxD for each NET_BUFFER of the NBL.                        */
    XF_Q_INIT(&SendQ);
    XF_DPR_GET_SLOCK(&pFifo->HalTxLock);
      /* Allocate a NetBuf context for every NetBuf of this NBL.             */
      while ( pNBLCtx->pNetBuf )
      {
        if ( (halStatus = xge_hal_fifo_dtr_reserve(pFifo->halChannel, &dtr))
             != XGE_HAL_OK )
        { /* Queue up the NBL to be sent later.                              */
          XMP_TX_NBL_REFERENCE(pNBLCtx);
          XF_Q_ADD_HEAD(&pFifo->NBLWaitQ, &pNBLCtx->Link);
          /* XMPTRACE(XMP_DBG_WRN, ("xmpNicTxNBL: Out of TxDs: err=%x\n",
             halStatus));*/
          keepGoing = 0;           /* Serve the NetBufs in SendQ and get out */
          pFifo->Stats.txdUnder++;
          /* ASSERT(FALSE); */
          XF_DPR_GET_SLOCK(&pFifo->TxLock);
            pFifo->InService  = 0;
            pFifo->TxUnderrun = TRUE;
          XF_DPR_FREE_SLOCK(&pFifo->TxLock);
          break;
        }

        XMP_TX_NBL_REFERENCE(pNBLCtx);          /* reference for each NetBuf */
        pNBufCtx = (xmpTxNBufCtx_t *)xge_hal_fifo_dtr_private(dtr);
        pNBufCtx->pNicCtx = pNicCtx;
        pNBufCtx->pFifo   = pFifo;
        pNBufCtx->pNBLCtx = pNBLCtx;
        pNBufCtx->pNetBuf = pNBLCtx->pNetBuf;
        pNBufCtx->dtr     = dtr;
        pNBufCtx->Flags   = XMP_TX_NBUF_FLAG_NONE;
        XF_Q_ADD_TAIL(&SendQ, &pNBufCtx->Link);
        pFifo->Stats.TxNBCount++;
        pNBLCtx->pNetBuf = NET_BUFFER_NEXT_NB(pNBLCtx->pNetBuf);
      } /* for each netbuf of NBL */
      XF_DPR_FREE_SLOCK(&pFifo->HalTxLock);

    XF_Q_DEL_HEAD(&SendQ, link);
    while ( link )
    {
      pNBufCtx = CONTAINING_RECORD(link, xmpTxNBufCtx_t, Link);

      /* Ask NDIS to allocate/prepare a SG list for the NetBuf and call our
       * xmpProcessSGListCB, which will xmit the SG list. It has to be
       * called at DISPATCH_LEVEL only.
       */
      /* if ( ((Status = XMP_TX_NB_DATA_LEN_OK(pNicCtx, pNBufCtx->pNetBuf))
         != NDIS_STATUS_SUCCESS) ||*/
      if ((Status =
           NdisMAllocateNetBufferSGList(pNicCtx->hMPDma,
                                        pNBufCtx->pNetBuf,
                                        pNBufCtx,
                                        NDIS_SG_LIST_WRITE_TO_DEVICE, 
                                        (PVOID)(pNBufCtx+1),
                                        pNicCtx->SGListSize))
          != NDIS_STATUS_SUCCESS )
      { /* TODO: If NDIS_STATUS_RESOURCES, queue it back for retry.      */
        XMPTRACE(XMP_DBG_WRN,
                 ("xmpNicTxNBL:SG List alloc failed=%lx\n", Status));
        xmpHalTxCompleteNetBuf(pNicCtx, pNBufCtx, 0, Status, TRUE, TRUE);
      }
      XF_Q_DEL_HEAD(&SendQ, link);
    } /* for all NetBufs of NBL */

    /* We are done with this NBL. Once references of all the NetBufs are
     * are released, NBL would be completed.
     */
    if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_TX_NBL_CHAIN) )     
      XMP_TX_NBL_DEREFERENCE_COAL(pNicCtx, pFifo, pNBLCtx, TRUE, TRUE);
    else
      XMP_TX_NBL_DEREFERENCE(pNicCtx, pFifo, pNBLCtx, TRUE);
 
  } /* while(1) */

  if (OldIrql != DISPATCH_LEVEL)
    NDIS_LOWER_IRQL(OldIrql, DISPATCH_LEVEL);
}

/* Perform NBL validation.                                             */
NDIS_STATUS
xmpNicTxNBLValid(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  NET_BUFFER_LIST        *pNBL
  )
{
  NDIS_STATUS                 Status = NDIS_STATUS_SUCCESS;

  NDIS_TCP_IP_CHECKSUM_NET_BUFFER_LIST_INFO nblCkoInfo; 

  nblCkoInfo.Value = NET_BUFFER_LIST_INFO(pNBL, TcpIpChecksumNetBufferListInfo);
  if ( nblCkoInfo.Transmit.IsIPv4 )
  {
    if ( (nblCkoInfo.Transmit.IpHeaderChecksum && 
          !bit(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_IP4_TX)) ||
         (nblCkoInfo.Transmit.TcpChecksum &&
          !bit(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP4_TX)) ||
         (nblCkoInfo.Transmit.UdpChecksum &&
          !bit(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP4_TX)) )
      Status = NDIS_STATUS_NOT_ACCEPTED;
  }
  else if ( nblCkoInfo.Transmit.IsIPv6)
  {
    if ( nblCkoInfo.Transmit.IpHeaderChecksum ||
         (nblCkoInfo.Transmit.TcpChecksum &&
          !bit(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP6_TX)) ||
         (nblCkoInfo.Transmit.UdpChecksum &&
          !bit(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP6_TX)) )
      Status = NDIS_STATUS_NOT_ACCEPTED;
  }

  return Status;
}

/* 
 * xmpNicRxNBLBlkAlloc
 *
 * Description:
 *   This function is to allocate 'RxBufCnt' number of Rx frame buffers that
 *   are packaged into NBLs. Shared-memory for Rx buffer is allocated in
 *   chunks of 'XENAMP_RXBUF_BLOCK_SIZE' before it is divided into individual
 *   Rx frame buffers that are then described/packaged using a NBL.
 *
 * Arguments:
 *   pNicCtx             - pointer to the nic context
 *   FrameBufSz          - size (in bytes) of each frame (including backfill)
 *   RingNum             - Ring number (0 based)
 *   RxBufCnt            - Number of descriptors to allocate
 *   pRxNBLQ             - List for appending newly allocated NBLs
 *   pRxBLKQ             - List for appending newly allocated xmpRxBufBlk_t
 *
 * Return Value:
 *   NDIS_STATUS_SUCCESS if successful, NDIS_STATUS_XXX error code if not.
 *   This function returns the chunks of shared-memory allocated in the
 *   pRxBLKQ arg and pRxNBLQ is used to return the list of allocated NBLs.
 *
 * IRQL:
 *   PASSIVE_LEVEL
 *
 * Notes:
 *   hMPAdapter must be set before calling this function.
 *
 */ 
NDIS_STATUS
xmpNicRxNBLBlkAlloc(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  UINT                    FrameBufSz,
  IN  UINT                    RingNum,
  IN  UINT                    RxBufCnt,
  IN  xmpQLink_t             *pRxNBLQ,
  IN  xmpQLink_t             *pRxBLKQ
  )
{
  NET_BUFFER_LIST            *pNBL;
  xmpRxBufBlk_t              *pRxBlk;
  xmpRxNBLCtx_t              *pRxCtx;
  xmpQLink_t                 *link;
  UCHAR                      *pVa;
  UCHAR                      *ptr;
  UCHAR                      *pVax;
  UINT                        Cnt;
  UINT                        sz;
  UINT                        RxAlignSz;
  UINT                        blkSz;
  NDIS_STATUS                 Status;
  U64                         Pa;
  NDIS_PHYSICAL_ADDRESS       RxBlkPhy;

  XMPTRACE(XMP_DBG_INI, ("==> xmpNicRxNBLBlkAlloc: pNicCtx=%p Sz=%d Ring=%d"
                         "Cnt=%d\n", pNicCtx, FrameBufSz, RingNum, RxBufCnt));
  XF_Q_INIT(pRxNBLQ);
  XF_Q_INIT(pRxBLKQ);
  do
  {
    /* Get alignment overhead required for each buffer */
    RxAlignSz = XMP_RXBUF_ALIGN_SZ(pNicCtx->CacheFillSize);
    blkSz = XENAMP_RXBUF_BLOCK_SIZE(FrameBufSz, RxAlignSz);
    Cnt = 0;
    Status = NDIS_STATUS_SUCCESS;
    while( (Cnt < RxBufCnt) && (Status == NDIS_STATUS_SUCCESS) )
    {
      /* Allocate Rx buffer in chunks of XENAMP_RXBUF_BLOCK_SIZE.            */
      NdisMAllocateSharedMemory(pNicCtx->hMPAdapter, blkSz,
                                 TRUE, (PVOID *) &pRxBlk, &RxBlkPhy);
      if ( !pRxBlk )
      {
        Status = NDIS_STATUS_RESOURCES;
        XMPTRACE(XMP_DBG_ERR,
                 ("xmpNicRxNBLBlkAlloc:SharedMemory for RxBufBlk failed\n"));
        break;
      }
      XF_Q_ADD_TAIL(pRxBLKQ, &pRxBlk->Link);

      pVa = &pRxBlk->Buf[0];
      pRxBlk->PhyAddr = RxBlkPhy;
      Pa = RxBlkPhy.QuadPart;
      Pa += XMP_PTR_DIFF(pVa, (UCHAR *) pRxBlk);
      /* Given the chunk of newly allocated shared-memory, divide it into
       * individual Rx frame buffers, and package each Rx frame buf into a
       * NBL.
       */
      while( (pVa <= (((UCHAR *)pRxBlk) + (blkSz - FrameBufSz - RxAlignSz))
              && (Cnt++ < RxBufCnt)) )
      {
        /* Align begining of Rx buffer.                                      */
        pVax = XMP_ALIGN_VA(pVa, RxAlignSz);    /* Align VA for the Rx frame */
        Pa += XMP_PTR_DIFF(pVax, pVa);            /* Adjust Physical Address */

        /**************** TRIAL *****************/
        if ( !XMP_NIC_RX_SPLIT(pNicCtx) )
        {
          pVax += 2; 
          Pa   += 2;
        }
        pVa = pVax;
        /* TODO: Do we want to align for TCP Payload instead of frame start?
         * Then, how do we ensure cache line alignment requirement?
         */

        /* Now package the Rx frame buffer in a NBL that involves allocating
         * a NBL and a NET_BUFFER.
         */
        ptr = pVa;                /* points at the start (could be backfill) */
        sz = FrameBufSz;                        /* includes backfill, if any */

        if ( !(pNBL = xmpNicRxNBLAlloc(pNicCtx, ptr, sz)) )
        {
          Status = NDIS_STATUS_RESOURCES;
          break;
        }

        pRxCtx = (xmpRxNBLCtx_t *)NET_BUFFER_LIST_CONTEXT_DATA_START(pNBL);
        ASSERT(pRxCtx != NULL);
        pRxCtx->state    = XMP_RXNBL_STATE_FREE;
        pRxCtx->pRxBuf   = ptr;
        pRxCtx->RxPa     = Pa;
        pRxCtx->RxRing   = RingNum; 
        pRxCtx->lroNext  = NULL;
        pRxCtx->advance  = 0;
        pRxCtx->pLastMDL = NULL;
        /* Adjust physical address by backfill offset, if needed.            */
        if ( XMP_NIC_RX_SPLIT(pNicCtx) )
          pRxCtx->RxPa += pNicCtx->RxBackfillSz;

        XMP_NIC_RX_NBL_SET_MR(pNBL, pRxCtx);
        XF_Q_ADD_TAIL(pRxNBLQ, &pRxCtx->NBLQLink);
        pVa += FrameBufSz;
        Pa  += FrameBufSz;
      }
    }
  } while(0);

  if ( Status != NDIS_STATUS_SUCCESS )
  {
    NdisWriteErrorLogEntry(pNicCtx->hMPAdapter,
                           (NDIS_ERROR_CODE) NDIS_ERROR_CODE_OUT_OF_RESOURCES, 0);
    XF_Q_DEL_TAIL(pRxNBLQ, link);
    while(link)
    {
      pRxCtx = CONTAINING_RECORD(link, xmpRxNBLCtx_t, NBLQLink);
      xmpNicRxNBLFree(pRxCtx->pNBL);
      XF_Q_DEL_TAIL(pRxNBLQ, link);
    }

    XF_Q_DEL_TAIL(pRxBLKQ, link);
    while(link)
    {
      pRxBlk = CONTAINING_RECORD(link, xmpRxBufBlk_t, Link);
      NdisMFreeSharedMemory(pNicCtx->hMPAdapter, blkSz, TRUE,
                            (VOID *)pRxBlk, pRxBlk->PhyAddr);
      XF_Q_DEL_TAIL(pRxNBLQ, link);
    }
  }

  XMPTRACE(XMP_DBG_INI, ("<== xmpNicRxNBLBlkAlloc: Status = %lx\n", Status) );
  return Status;
}

NET_BUFFER_LIST *
xmpNicRxNBLAlloc(
  IN xmpNicCtx_t             *pNicCtx,
  IN UCHAR                   *pBuf,
  IN U32                      BufSz
  )
{
  xmpRxNBLCtx_t              *pRxCtx;
  NET_BUFFER_LIST            *pNBL = (NET_BUFFER_LIST *)0;
  UCHAR                      *pDataBuf;
  MDL                        *pBfillMDL  = (MDL *)0;
  MDL                        *pDataMDL   = (MDL *)0;
  UINT                        DataSz;

  DataSz   = BufSz - pNicCtx->RxBackfillSz;
  pDataBuf = pBuf + pNicCtx->RxBackfillSz;

  do
  {
    if ( pNicCtx->RxBackfillSz )
    {
      if ( !(pBfillMDL = NdisAllocateMdl(pNicCtx->hMPAdapter, pBuf, BufSz)) )
      {
        XMPTRACE(XMP_DBG_ERR, ("xmpNicRxNBLAlloc:Backfill MDL failed\n"));
        break;
      }
      NDIS_MDL_LINKAGE(pBfillMDL) = NULL;
    }

    if ( !(pDataMDL = NdisAllocateMdl(pNicCtx->hMPAdapter, pDataBuf, DataSz)) )
    {
      XMPTRACE(XMP_DBG_ERR, ("xmpNicRxNBLAlloc:Data MDL failed\n"));
      break;
    }
    NDIS_MDL_LINKAGE(pDataMDL) = NULL;

    if ( !(pNBL =
           NdisAllocateNetBufferAndNetBufferList(pNicCtx->hRxNBLPool,
                                                 sizeof(xmpRxNBLCtx_t),
                                                 0,      /* context backfill */
                                                 pDataMDL,
                                                 0,
                                                 DataSz)) )
    {
      XMPTRACE(XMP_DBG_ERR, ("xmpNicRxNBLAlloc:Rx NetBufferList failed\n"));
      break;
    }

    ASSERT(pDataMDL == NET_BUFFER_FIRST_MDL(NET_BUFFER_LIST_FIRST_NB(pNBL)));

    pRxCtx = (xmpRxNBLCtx_t *)NET_BUFFER_LIST_CONTEXT_DATA_START(pNBL);
    ASSERT(pRxCtx != NULL);
    pRxCtx->pNBL      = pNBL;
    pRxCtx->pBfillMDL = pBfillMDL;
    pRxCtx->pDataMDL  = pDataMDL;
  } while(0);

  if ( !pNBL)
  {
    if ( pDataMDL )
      NdisFreeMdl(pDataMDL);

    if ( pBfillMDL )
      NdisFreeMdl(pBfillMDL);
  }

  return pNBL;
}

NDIS_STATUS
xmpNicHdrMDLsAlloc(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  ULONG                   HeaderSz,
  IN  ULONG                   MdlCount,
  OUT xmpNicRxHdrMDL_t        MdlArr[],
  OUT xmpNicRxHdrBlk_t        BlkArr[]
  )
{
  ULONG                       ul;
  ULONG                       perBlock;
  ULONG                       nBlocks;
  ULONG                       left=0;
  ULONG                       count=0;
  NDIS_STATUS                 Status = NDIS_STATUS_SUCCESS;

  perBlock = PAGE_SIZE / HeaderSz;
  nBlocks  = ((MdlCount + (perBlock-1)) / perBlock);

  left = MdlCount;                             /* remaining MDLs to allocate */
  for ( ul = 0; ul < nBlocks; ul++ )
  {
    count = left < perBlock ? left : perBlock;
    if ( (Status = xmpNicHdrMDLBlockAlloc(pNicCtx,
                                          HeaderSz,
                                          count,
                                          &BlkArr[ul].va,
                                          &BlkArr[ul].pa,
                                          &MdlArr[MdlCount - left]))
         != NDIS_STATUS_SUCCESS )
    {
      break;
    }
    left -= count;
  }

  if ( Status != NDIS_STATUS_SUCCESS )
  {
    count = MdlCount - left;
    while( count )
    {
      --count;
      NdisFreeMdl(MdlArr[count].pMDL);
      MdlArr[count].pMDL    = NULL;
      MdlArr[count].pHdrBuf = NULL;
      MdlArr[count].PhyAddr = (U64)0;
    }

    for ( count = 0; count < ul; count++ )
    {
      NdisMFreeSharedMemory(pNicCtx->hMPAdapter, PAGE_SIZE, TRUE,
                            (VOID *)BlkArr[count].va, BlkArr[count].pa);
      BlkArr[count].va = NULL;
    }
  }
  return Status;
}

NDIS_STATUS
xmpNicHdrMDLBlockAlloc(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  ULONG                   HeaderSz,
  IN  ULONG                   MdlCount,
  OUT void                  **pBlockVa,
  OUT NDIS_PHYSICAL_ADDRESS  *pBlockPa,
  OUT xmpNicRxHdrMDL_t       *pMdlArr
  )
{
  VOID                       *block;
  UCHAR                      *va;
  MDL                        *pMDL;
  xmpNicRxHdrMDL_t           *pMdlInfo;
  NDIS_PHYSICAL_ADDRESS       pa;
  ULONG                       ulval;
  UINT                        ui;
  NDIS_STATUS                 Status = NDIS_STATUS_SUCCESS;

  ASSERT( MdlCount <= (PAGE_SIZE / HeaderSz) );

  do
  {
    /* Allocate rx headers buffer in chunks of PAGE_SIZE.                   */
    NdisMAllocateSharedMemory(pNicCtx->hMPAdapter,
                              PAGE_SIZE,
                              TRUE, 
                              &block,
                              &pa);
    if ( !block )
    {
      Status = NDIS_STATUS_RESOURCES;
      XMPTRACE(XMP_DBG_ERR, ("xmpNicHdrMDLAlloc:SharedMemory alloc failed\n"));
      break;
    }
    va = (UCHAR *)block;
    *pBlockVa = block;
    *pBlockPa = pa;

    pMdlInfo = pMdlArr;
    for (ui = 0; ui < MdlCount; ui++, va += HeaderSz, pMdlInfo++)
    {
      if ( !(pMDL = NdisAllocateMdl(pNicCtx->hMPAdapter, va, HeaderSz)) )
      {
        Status = NDIS_STATUS_RESOURCES;
        XMPTRACE(XMP_DBG_ERR, ("xmpNicHdrMDLAlloc: NdisAllocateMdl failed\n"));
        break;
      }
      NDIS_MDL_LINKAGE(pMDL) = NULL;
      pMdlInfo->pMDL    = pMDL;
      pMdlInfo->pHdrBuf = va;
      pMdlInfo->PhyAddr = pa.QuadPart + (ui * HeaderSz);
    }
  } while(0);

  if ( Status != NDIS_STATUS_SUCCESS )
  {
    pMdlInfo = pMdlArr;
    while( pMdlInfo->pMDL )
    {
      NdisFreeMdl(pMdlInfo->pMDL);
      pMdlInfo->pMDL = NULL;
      pMdlInfo->pHdrBuf = NULL;
      pMdlInfo->PhyAddr = (U64)0;
      pMdlInfo++;
    }

    if ( block )
    {
      NdisMFreeSharedMemory(pNicCtx->hMPAdapter, PAGE_SIZE, TRUE,
                            (VOID *)block, pa);
    }

    *pBlockVa = NULL;
  }
  return Status;
}

VOID
xmpNicRxNBLFree(
  IN NET_BUFFER_LIST         *pNBL
  )
{
  MDL                        *pMDL;
  xmpRxNBLCtx_t              *pNBLCtx;
  MDL                        *pNext;

  pNBLCtx = (xmpRxNBLCtx_t *)NET_BUFFER_LIST_CONTEXT_DATA_START(pNBL);
  pMDL = pNBLCtx->pBfillMDL;
  pNBLCtx->pBfillMDL     = (MDL *)0;
  if ( pMDL )
  {
    NDIS_MDL_LINKAGE(pMDL) = (MDL *)0;
    NdisFreeMdl(pMDL);
  }
  pMDL = pNBLCtx->pDataMDL;
  pNBLCtx->pDataMDL      = (MDL *)0;
  NDIS_MDL_LINKAGE(pMDL) = (MDL *)0;
  if ( pMDL )
  {
    NDIS_MDL_LINKAGE(pMDL) = (MDL *)0;
    NdisFreeMdl(pMDL);
  }
  NdisFreeNetBufferList(pNBL);
}

VOID 
xmpNicInitRssCapabilities(
  IN  xmpNicCtx_t            *pNicCtx,
  OUT NDIS_RECEIVE_SCALE_CAPABILITIES *pCapabilities
  )
{ 
  pCapabilities->CapabilitiesFlags = 
    (NDIS_RSS_CAPS_CLASSIFICATION_AT_ISR | 
#ifdef XENAMP_MSI_SUPPORT
     NDIS_RSS_CAPS_MESSAGE_SIGNALED_INTERRUPTS |
#endif
     NdisHashFunctionToeplitz
     | NDIS_RSS_CAPS_HASH_TYPE_TCP_IPV4
#ifndef XMP_RSS_IPV4_ONLY
     | NDIS_RSS_CAPS_HASH_TYPE_TCP_IPV6 
     | NDIS_RSS_CAPS_HASH_TYPE_TCP_IPV6_EX
#endif /* XMP_RSS_IPV4_ONLY */
     );

#ifdef XENAMP_MSI_SUPPORT
  pCapabilities->NumberOfInterruptMessages = XMP_HAL_MAX_MSIX_MESSAGES;
#else
  pCapabilities->NumberOfInterruptMessages = 1;
#endif
  pCapabilities->NumberOfReceiveQueues = pNicCtx->RegParams.RxRingNum; 
}

VOID
xmpNicInitHwOffloadCapabilities(
  IN  xmpNicCtx_t            *pNicCtx,
  OUT PNDIS_OFFLOAD           OffloadCapabilities
  )
{
  NDIS_STATUS Status = NDIS_STATUS_SUCCESS;

  XMPTRACE(XMP_DBG_OFLD, ("==> xmpNicInitOffloadCapabilities\n"));

  do
  {
    XF_MCLEAR(OffloadCapabilities,sizeof(NDIS_OFFLOAD));

    XENAMP_NDIS_OBJECT_INIT
      (&OffloadCapabilities->Header,
       NDIS_OBJECT_TYPE_OFFLOAD,
       NDIS_OFFLOAD_REVISION_1,
      sizeof(NDIS_OFFLOAD));

    /* Checksum Offload information.                                         */
    OffloadCapabilities->Checksum.IPv4Transmit.Encapsulation =
      NDIS_ENCAPSULATION_IEEE_802_3 | NDIS_ENCAPSULATION_IEEE_LLC_SNAP_ROUTED;
    OffloadCapabilities->Checksum.IPv4Transmit.IpOptionsSupported  = 1;
    OffloadCapabilities->Checksum.IPv4Transmit.TcpOptionsSupported = 1;
    OffloadCapabilities->Checksum.IPv4Transmit.TcpChecksum         = 1;
    OffloadCapabilities->Checksum.IPv4Transmit.UdpChecksum         = 1;
    OffloadCapabilities->Checksum.IPv4Transmit.IpChecksum          = 1;

    OffloadCapabilities->Checksum.IPv4Receive.Encapsulation = 
      NDIS_ENCAPSULATION_IEEE_802_3 | NDIS_ENCAPSULATION_IEEE_LLC_SNAP_ROUTED;
    OffloadCapabilities->Checksum.IPv4Receive.IpOptionsSupported  = 1;
    OffloadCapabilities->Checksum.IPv4Receive.TcpOptionsSupported = 1;
    OffloadCapabilities->Checksum.IPv4Receive.TcpChecksum         = 1;
    OffloadCapabilities->Checksum.IPv4Receive.UdpChecksum         = 1;
    OffloadCapabilities->Checksum.IPv4Receive.IpChecksum          = 1;

    OffloadCapabilities->Checksum.IPv6Transmit.Encapsulation =
      NDIS_ENCAPSULATION_IEEE_802_3 | NDIS_ENCAPSULATION_IEEE_LLC_SNAP_ROUTED;
    OffloadCapabilities->Checksum.IPv6Transmit.IpExtensionHeadersSupported = 0;
    OffloadCapabilities->Checksum.IPv6Transmit.TcpOptionsSupported         = 1;
    OffloadCapabilities->Checksum.IPv6Transmit.TcpChecksum                 = 1;
    OffloadCapabilities->Checksum.IPv6Transmit.UdpChecksum                 = 1;

    OffloadCapabilities->Checksum.IPv6Receive.Encapsulation =
      NDIS_ENCAPSULATION_IEEE_802_3 | NDIS_ENCAPSULATION_IEEE_LLC_SNAP_ROUTED;
    OffloadCapabilities->Checksum.IPv6Receive.IpExtensionHeadersSupported = 0;
    OffloadCapabilities->Checksum.IPv6Receive.TcpOptionsSupported         = 1;
    OffloadCapabilities->Checksum.IPv6Receive.TcpChecksum                 = 1;
    OffloadCapabilities->Checksum.IPv6Receive.UdpChecksum                 = 1;

    /* Large Send Offload information.                                       */
    OffloadCapabilities->LsoV1.IPv4.Encapsulation   =
      NDIS_ENCAPSULATION_IEEE_802_3 | NDIS_ENCAPSULATION_IEEE_LLC_SNAP_ROUTED;
    OffloadCapabilities->LsoV1.IPv4.MaxOffLoadSize  = XENAMP_LSO_MAX_SIZE;
    OffloadCapabilities->LsoV1.IPv4.MinSegmentCount = 2;
    OffloadCapabilities->LsoV1.IPv4.TcpOptions      = 1;
    OffloadCapabilities->LsoV1.IPv4.IpOptions       = 1;

    /* IPSec Offload Information.                                            */
    OffloadCapabilities->IPsecV1.Supported.Encapsulation             =
      NDIS_ENCAPSULATION_NOT_SUPPORTED;
    OffloadCapabilities->IPsecV1.Supported.AhEspCombined             = 0;
    OffloadCapabilities->IPsecV1.Supported.TransportTunnelCombined   = 0;
    OffloadCapabilities->IPsecV1.Supported.IPv4Options               = 0;
    OffloadCapabilities->IPsecV1.Supported.Flags                     = 0;

    OffloadCapabilities->IPsecV1.IPv4AH.Md5         = 0;
    OffloadCapabilities->IPsecV1.IPv4AH.Sha_1       = 0;
    OffloadCapabilities->IPsecV1.IPv4AH.Transport   = 0;
    OffloadCapabilities->IPsecV1.IPv4AH.Tunnel      = 0;
    OffloadCapabilities->IPsecV1.IPv4AH.Send        = 0;
    OffloadCapabilities->IPsecV1.IPv4AH.Receive     = 0;

    OffloadCapabilities->IPsecV1.IPv4ESP.Des        = 0;
    OffloadCapabilities->IPsecV1.IPv4ESP.Reserved   = 0;
    OffloadCapabilities->IPsecV1.IPv4ESP.TripleDes  = 0;
    OffloadCapabilities->IPsecV1.IPv4ESP.NullEsp    = 0;
    OffloadCapabilities->IPsecV1.IPv4ESP.Transport  = 0;
    OffloadCapabilities->IPsecV1.IPv4ESP.Tunnel     = 0;
    OffloadCapabilities->IPsecV1.IPv4ESP.Send       = 0;
    OffloadCapabilities->IPsecV1.IPv4ESP.Receive    = 0;

  } while(0);

  XMPTRACE(XMP_DBG_OFLD, ("<== xmpNicInitOffloadCapabilities: Status=%x\n",
                          Status));
}

VOID
xmpNicInitOffloadCapabilities(
  IN  xmpNicCtx_t            *pNicCtx,
  OUT PNDIS_OFFLOAD           OffloadCapabilities
  )
{
  NDIS_STATUS                 Status = NDIS_STATUS_SUCCESS;

  XMPTRACE(XMP_DBG_OFLD, ("==> xmpNicInitOffloadCapabilities\n"));

  do
  {
    XF_MCLEAR(OffloadCapabilities, sizeof(NDIS_OFFLOAD));

    XENAMP_NDIS_OBJECT_INIT
      (&OffloadCapabilities->Header,
       NDIS_OBJECT_TYPE_OFFLOAD,
       NDIS_OFFLOAD_REVISION_1,
       sizeof(NDIS_OFFLOAD));

    /* Checksum Offload information.                                         */
    if ( bit(pNicCtx->Flags, (XMP_NIC_FLAGS_CKO_IP4_TX |
                              XMP_NIC_FLAGS_CKO_IP4_RX|
                              XMP_NIC_FLAGS_CKO_TCP4_TX | 
                              XMP_NIC_FLAGS_CKO_TCP4_RX |
                              XMP_NIC_FLAGS_CKO_UDP4_TX | 
                              XMP_NIC_FLAGS_CKO_UDP4_RX)) )
    {
      OffloadCapabilities->Checksum.IPv4Transmit.Encapsulation       =
        pNicCtx->OffloadEncapsulation.IPv4.EncapsulationType;
      OffloadCapabilities->Checksum.IPv4Receive.Encapsulation        =
        pNicCtx->OffloadEncapsulation.IPv4.EncapsulationType;
    }

    if ( bit(pNicCtx->Flags, (XMP_NIC_FLAGS_CKO_IP4_TX |
                              XMP_NIC_FLAGS_CKO_IP4_RX)) )
    {
      OffloadCapabilities->Checksum.IPv4Transmit.IpChecksum         = 1;
      OffloadCapabilities->Checksum.IPv4Receive.IpChecksum          = 1;
      OffloadCapabilities->Checksum.IPv4Transmit.IpOptionsSupported = 1;
      OffloadCapabilities->Checksum.IPv4Receive.IpOptionsSupported  = 1;
    }

    if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP4_TX) )
    {
      OffloadCapabilities->Checksum.IPv4Transmit.TcpChecksum         = 1;
      OffloadCapabilities->Checksum.IPv4Transmit.TcpOptionsSupported = 1;
    }

    if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP4_TX) )
    {
      OffloadCapabilities->Checksum.IPv4Receive.TcpChecksum          = 1;
      OffloadCapabilities->Checksum.IPv4Receive.TcpOptionsSupported  = 1;
    }

    if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP4_TX) )
      OffloadCapabilities->Checksum.IPv4Transmit.UdpChecksum         = 1;

    if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP4_RX) )
      OffloadCapabilities->Checksum.IPv4Receive.UdpChecksum          = 1;

    if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP6_TX) )
      OffloadCapabilities->Checksum.IPv6Transmit.UdpChecksum         = 1;

    if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP6_RX) )
      OffloadCapabilities->Checksum.IPv6Receive.UdpChecksum          = 1;

    if ( bit(pNicCtx->Flags, (XMP_NIC_FLAGS_CKO_TCP6_TX |
                              XMP_NIC_FLAGS_CKO_TCP6_RX |
                              XMP_NIC_FLAGS_CKO_UDP6_TX |
                              XMP_NIC_FLAGS_CKO_UDP6_RX
                              )) )
    {
      OffloadCapabilities->Checksum.IPv6Receive.Encapsulation =
        pNicCtx->OffloadEncapsulation.IPv6.EncapsulationType;
      OffloadCapabilities->Checksum.IPv6Transmit.Encapsulation =
        pNicCtx->OffloadEncapsulation.IPv6.EncapsulationType;
      OffloadCapabilities->Checksum.IPv6Transmit.IpExtensionHeadersSupported = 0;
      OffloadCapabilities->Checksum.IPv6Receive.IpExtensionHeadersSupported = 0;
    }

    if ( bit(pNicCtx->Flags, (XMP_NIC_FLAGS_CKO_TCP6_TX |
                              XMP_NIC_FLAGS_CKO_TCP6_RX)) )
    {
      OffloadCapabilities->Checksum.IPv6Transmit.TcpOptionsSupported = 1;
      OffloadCapabilities->Checksum.IPv6Transmit.TcpChecksum         = 1;
      OffloadCapabilities->Checksum.IPv6Receive.TcpOptionsSupported  = 1;
      OffloadCapabilities->Checksum.IPv6Receive.TcpChecksum          = 1;
    }

    /* Large Send Offload.                                                   */
    if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_LSO) )
    {
      OffloadCapabilities->LsoV1.IPv4.MaxOffLoadSize  = XENAMP_LSO_MAX_SIZE;
      OffloadCapabilities->LsoV1.IPv4.MinSegmentCount = 2;
      OffloadCapabilities->LsoV1.IPv4.TcpOptions      = 1;
      OffloadCapabilities->LsoV1.IPv4.IpOptions       = 1;
      OffloadCapabilities->LsoV1.IPv4.Encapsulation   =
        pNicCtx->OffloadEncapsulation.IPv4.EncapsulationType;      
    }



    /* IPSec Offload Information.                                            */
    XF_MCLEAR(&OffloadCapabilities->IPsecV1, sizeof(OffloadCapabilities->IPsecV1));

  } while(0);

  XMPTRACE(XMP_DBG_OFLD, ("<== xmpNicInitOffloadCapabilities: Status=%x\n",
                         Status));
}

VOID
xmpNicSetRssParameters(
  IN VOID                    *pContext,
  IN NDIS_HANDLE              Handle
  )
{
  USHORT                      Flags;
  xge_hal_device_t           *pHalDev;
  xmpNicCtx_t                *pNicCtx = (xmpNicCtx_t *)pContext;
  xmpNicRssParams_t          *pParams;
  BOOLEAN                     bTable, bKey, bCpu, bHashInfo;
  BOOLEAN                     again = FALSE;
  U64                         HashType;
  UCHAR                       DefTable[256]={0};

  XMPTRACE(XMP_DBG_WRN, ("==> xmpNicSetRssParameters\n"));

  pHalDev = XMP_NIC_GET_DEV(pNicCtx);

  do
  {
    KeMemoryBarrier();
    XF_GET_SLOCK(&pNicCtx->lock);
      ASSERT(pNicCtx->wiQueued);
      ASSERT(!pNicCtx->wiRunning);
      pNicCtx->wiRunning = TRUE;
      pNicCtx->wiQueued  = FALSE;
      NdisMoveMemory(&pNicCtx->RssParamsWS,
                     &pNicCtx->RssParamsReq,
                     sizeof(xmpNicRssParams_t));
    XF_FREE_SLOCK(&pNicCtx->lock);
    KeMemoryBarrier();

    pParams = &pNicCtx->RssParamsWS;
    bTable = bKey = bCpu = bHashInfo = FALSE;
    Flags = pParams->Flags;

    do
    {
      if(pParams->HashFunction == 0)
      {
        xge_hal_rts_rth_clr(pHalDev);
        XMP_NIC_RSS_DISABLE(pNicCtx);
        pParams->BaseCpuNum  = 0;
        pParams->Flags       = 0;
        pParams->HashType    = 0;
        pParams->HashBitsSz  = 0;
        pParams->SecretKeySz = 0;
        pParams->TableSz     = 0;
        XMPTRACE(XMP_DBG_WRN, ("==> xmpNicSetRssParameters: Rss Disabled\n"));
        break;
      }

      if ( XMP_NIC_RSS_IS_ENABLED(pNicCtx) )
      {
        if ( !(bit(Flags, NDIS_RSS_PARAM_FLAG_HASH_KEY_UNCHANGED)) )
          bKey = TRUE;

        if ( !(bit(Flags, NDIS_RSS_PARAM_FLAG_ITABLE_UNCHANGED)) )
          bTable = TRUE;

        if ( !(bit(Flags, NDIS_RSS_PARAM_FLAG_BASE_CPU_UNCHANGED )) )
          bCpu = TRUE;

        if ( !(bit(Flags, NDIS_RSS_PARAM_FLAG_HASH_INFO_UNCHANGED )) )
          bHashInfo = TRUE;
      }
      else
      {
        bTable = bKey = bCpu = bHashInfo = TRUE;
        xge_hal_rts_rth_init(pHalDev);
      }

      if (bKey)
      {
        xge_hal_device_rts_rth_key_set(pHalDev,
                                       (UCHAR)pParams->SecretKeySz >> 3,
                                       pParams->SecretKey);
      }

      if (bTable)
      {
        if (pNicCtx->RegParams.RxRingNum == 1)
        {
          xge_hal_rts_rth_itable_set(pHalDev, DefTable, pParams->TableSz);
        }
        else
        {
          USHORT usTmp;
          ULONG  ulTmp;
          ULONG  ring = 0;
          UCHAR  cpu  = 0;
          NDIS_STATUS Status;
          NDIS_MSIX_CONFIG_PARAMETERS msiCfgParam;

          __writableTo(byteCount(XMP_NIC_RSS_MAX_TABLE_SZ)) UCHAR  tmpTable[XMP_NIC_RSS_MAX_TABLE_SZ];
          __writableTo(byteCount(XMP_NIC_RSS_MAX_TABLE_SZ)) UCHAR  setTable[XMP_NIC_RSS_MAX_TABLE_SZ];

          if ( pNicCtx->RegParams.intr_mode != XGE_HAL_INTR_MODE_IRQLINE )
          {
            __analysis_assume(sizeof(tmpTable) >= pParams->TableSz);
            NdisMoveMemory(tmpTable, pParams->Table, pParams->TableSz);

            __analysis_assume(sizeof(setTable) >= pParams->TableSz);
            NdisMoveMemory(setTable, pParams->Table, pParams->TableSz);

            while( 1 )
            {
              if ( pNicCtx->RegParams.RxRingNum >= pNicCtx->devCtx->RssCpuCount )
                break;

              for ( usTmp=0, cpu=0xff; usTmp < pParams->TableSz; usTmp++ )
              {
                if ( tmpTable[usTmp] != 0xff )
                {
                  cpu = tmpTable[usTmp];
                  break;
                }
              }

              if ( cpu == 0xff )
                break;

              ASSERT( ring < pNicCtx->RegParams.RxRingNum );
              for ( usTmp=0; usTmp < pParams->TableSz; usTmp++ )
              {
                if ( tmpTable[usTmp] == cpu )
                {
                  tmpTable[usTmp] = 0xff;
                  setTable[usTmp] = (UCHAR)(ring & 0xFF);
                }
              }
              if ( pNicCtx->RxInfo[ring].RssCpu != cpu )
              {
                /* if the CPU number of the ring has changed, find MSI message    
                 * that corresponds to the desired CPU, and change the affinity
                 * of the ring to the CPU.
                 */
                UCHAR ocpu =  pNicCtx->RxInfo[ring].RssCpu;
                pNicCtx->RxInfo[ring].RssCpu = cpu;
                /* TODO: protect pNicCtx->devCtx->MsiMsgCount if filter
                 * resources can change it dynamically.
                 */
                for (ulTmp=1; ulTmp < pNicCtx->devCtx->MsiMsgCount; ulTmp++)
                {
                  if ( pNicCtx->devCtx->MsiCpuMap[ulTmp] == cpu )
                    break;
                }
                ASSERT( ulTmp < pNicCtx->devCtx->MsiMsgCount );
                XMPTRACE(XMP_DBG_INFO, ("xmpNicSetRssParameters: "
                                      "Ring[%d] CPU map change: CPU %d -> %d",
                                      ring, ocpu, cpu));
                XENAMP_NDIS_OBJECT_INIT
                  (&msiCfgParam.Header,
                   NDIS_OBJECT_TYPE_DEFAULT,
                   NDIS_MSIX_CONFIG_PARAMETERS_REVISION_1,
                   NDIS_SIZEOF_MSIX_CONFIG_PARAMETERS_REVISION_1);
                msiCfgParam.ConfigOperation = NdisMSIXTableConfigSetTableEntry;
                msiCfgParam.TableEntry      = pNicCtx->RxInfo[ring].MsiId;
                msiCfgParam.MessageNumber   = ulTmp; /* TODO: is this correct? */
                Status = NdisMConfigMSIXTableEntry(pNicCtx->hMPAdapter,
                                                 &msiCfgParam);
                ASSERT(Status == NDIS_STATUS_SUCCESS);
              }
              ring++;
            }

          }
          else
          {
            /* Replace the entries in the table with the RSSProcessor Set */
            __analysis_assume(sizeof(setTable) >= pParams->TableSz);
            NdisMoveMemory(setTable, pParams->Table, pParams->TableSz);        
          }

          for ( usTmp=0; usTmp < pParams->TableSz; usTmp++ )
            setTable[usTmp] = pNicCtx->devCtx->PhyCpuRssProcMap[setTable[usTmp]];

          __analysis_assume(sizeof(pNicCtx->RssITable) >= pParams->TableSz);
          NdisMoveMemory(pNicCtx->RssITable, setTable, pParams->TableSz);
          xge_hal_rts_rth_itable_set(pHalDev, setTable, pParams->TableSz);
        }
      }

      HashType = 0;
      if ( bit(pParams->HashType, NDIS_HASH_IPV4) )
        HashType |= XGE_HAL_RTS_RTH_IPV4_EN;
      if ( bit(pParams->HashType, NDIS_HASH_TCP_IPV4) )
        HashType |= XGE_HAL_RTS_RTH_TCP_IPV4_EN;
      if ( bit(pParams->HashType, NDIS_HASH_IPV6) )
        HashType |= XGE_HAL_RTS_RTH_IPV6_EN;
      if ( bit(pParams->HashType, NDIS_HASH_TCP_IPV6) )
        HashType |= XGE_HAL_RTS_RTH_TCP_IPV6_EN;
      if ( bit(pParams->HashType, NDIS_HASH_IPV6_EX) )
        HashType |= XGE_HAL_RTS_RTH_IPV6_EX_EN;
      if ( bit(pParams->HashType, NDIS_HASH_TCP_IPV6_EX) )
        HashType |= XGE_HAL_RTS_RTH_TCP_IPV6_EX_EN;

      xge_hal_rts_rth_set(pHalDev, 0, HashType, pParams->HashBitsSz);
      XMP_NIC_RSS_ENABLE(pNicCtx);
    } while(0);

    KeMemoryBarrier();
    XF_GET_SLOCK(&pNicCtx->lock);
      pNicCtx->wiRunning = FALSE;
      again = pNicCtx->wiQueued;
    XF_FREE_SLOCK(&pNicCtx->lock);
  } while(again);

  XMPTRACE(XMP_DBG_WRN, ("<== xmpNicSetRssParameters\n"));
}

VOID
xmpNicSetCkoFlags(
  IN xmpNicCtx_t             *pNicCtx
  )
{
  U64                         txdCkoBits = (U64) 0;

  bic(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_IP4_TX);
  bic(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_IP4_RX);
  bic(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP4_TX);
  bic(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP4_RX);
  bic(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP6_TX);
  bic(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP6_RX);
  bic(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP4_TX);
  bic(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP4_RX);
  bic(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP6_TX);
  bic(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP6_RX);

  /* IPv4 checksum offloads.                                                 */
  if ( XMP_NIC_CKO_IP4_CONFIG(pNicCtx) == XENAMP_CKO_TXRX )
  {
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_IP4_TX);
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_IP4_RX);
    bis(txdCkoBits, XGE_HAL_TXD_TX_CKO_IPV4_EN);
    XMPTRACE(XMP_DBG_INFO, ("Registry: IP4 TX,RX Checksum offload enabled\n"));
  }
  else if ( XMP_NIC_CKO_IP4_CONFIG(pNicCtx) == XENAMP_CKO_RX )
  {
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_IP4_RX);
    XMPTRACE(XMP_DBG_INFO, ("Registry: IP4 RX Checksum offload enabled\n"));
  }
  else if ( XMP_NIC_CKO_IP4_CONFIG(pNicCtx) == XENAMP_CKO_TX )
  {
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_IP4_TX);
    bis(txdCkoBits, XGE_HAL_TXD_TX_CKO_IPV4_EN);
    XMPTRACE(XMP_DBG_INFO, ("Registry: IP4 TX Checksum offload enabled\n"));
  }
  else
  {
    XMPTRACE(XMP_DBG_WRN, ("Registry: IP4 Checksum offloads disabled!\n"));
  }

  /* TCP over IPv4 checksum offloads.                                        */
  if ( XMP_NIC_CKO_TCP4_CONFIG(pNicCtx) == XENAMP_CKO_TXRX )
  {
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP4_TX);
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP4_RX);
    bis(txdCkoBits, XGE_HAL_TXD_TX_CKO_TCP_EN);
    XMPTRACE(XMP_DBG_INFO, ("Registry: TCP TX,RX Checksum offload enabled\n"));
  }
  else if ( XMP_NIC_CKO_TCP4_CONFIG(pNicCtx) == XENAMP_CKO_RX )
  {
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP4_RX);
    XMPTRACE(XMP_DBG_INFO, ("Registry: TCP RX Checksum offload enabled\n"));
  }
  else if ( XMP_NIC_CKO_TCP4_CONFIG(pNicCtx) == XENAMP_CKO_TX )
  {
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP4_TX);
    bis(txdCkoBits, XGE_HAL_TXD_TX_CKO_TCP_EN);
    XMPTRACE(XMP_DBG_INFO, ("Registry: TCP TX Checksum offload enabled\n"));
  }
  else
  {
    XMPTRACE(XMP_DBG_WRN, ("Registry: TCP4 Checksum offloads disabled!\n"));
  }

  /* TCP over IPv6 checksum offloads.                                        */
  if ( XMP_NIC_CKO_TCP6_CONFIG(pNicCtx) == XENAMP_CKO_TXRX )
  {
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP6_TX);
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP6_RX);
    bis(txdCkoBits, XGE_HAL_TXD_TX_CKO_TCP_EN);
    XMPTRACE(XMP_DBG_INFO, ("Registry: TCP6 TX,RX Checksum offload"
                            " enabled\n"));
  }
  else if ( XMP_NIC_CKO_TCP6_CONFIG(pNicCtx) == XENAMP_CKO_RX )
  {
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP6_RX);
    XMPTRACE(XMP_DBG_INFO, ("Registry: TCP6 RX Checksum offload enabled\n"));
  }
  else if ( XMP_NIC_CKO_TCP6_CONFIG(pNicCtx) == XENAMP_CKO_TX )
  {
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP6_TX);
    bis(txdCkoBits, XGE_HAL_TXD_TX_CKO_TCP_EN);
    XMPTRACE(XMP_DBG_INFO, ("Registry: TCP6 TX Checksum offload enabled\n"));
  }
  else
  {
    XMPTRACE(XMP_DBG_WRN, ("Registry: TCP6 Checksum offloads disabled!\n"));
  }

  /* UDP over IPv4 checksum offloads.                                        */
  if ( XMP_NIC_CKO_UDP4_CONFIG(pNicCtx) == XENAMP_CKO_TXRX )
  {
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP4_TX);
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP4_RX);
    bis(txdCkoBits, XGE_HAL_TXD_TX_CKO_UDP_EN);
    XMPTRACE(XMP_DBG_INFO, ("Registry: UDP4 TX,RX Checksum offload"
                            " enabled\n"));
  }
  else if ( XMP_NIC_CKO_UDP4_CONFIG(pNicCtx) == XENAMP_CKO_RX )
  {
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP4_RX);
    XMPTRACE(XMP_DBG_INFO, ("Registry: UDP4 RX Checksum offload enabled\n"));
  }
  else if ( XMP_NIC_CKO_UDP4_CONFIG(pNicCtx) == XENAMP_CKO_TX )
  {
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP4_TX);
    bis(txdCkoBits, XGE_HAL_TXD_TX_CKO_UDP_EN);
    XMPTRACE(XMP_DBG_INFO, ("Registry: UDP4 TX Checksum offload enabled\n"));
  }
  else
  {
    XMPTRACE(XMP_DBG_WRN, ("Registry: UDP4 Checksum offload disabled!\n"));
  }

  /* UDP over IPv6 checksum offloads.                                        */
  if ( XMP_NIC_CKO_UDP6_CONFIG(pNicCtx) == XENAMP_CKO_TXRX )
  {
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP6_TX);
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP6_RX);
    XMPTRACE(XMP_DBG_INFO, ("Registry: UDP6 TX,RX Checksum offload"
                            " enabled\n"));
  }
  else if ( XMP_NIC_CKO_UDP6_CONFIG(pNicCtx) == XENAMP_CKO_RX )
  {
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP6_RX);
    XMPTRACE(XMP_DBG_INFO, ("Registry: UDP6 RX Checksum offload enabled\n"));
  }
  else if ( XMP_NIC_CKO_UDP6_CONFIG(pNicCtx) == XENAMP_CKO_TX )
  {
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP6_TX);
    bis(txdCkoBits, XGE_HAL_TXD_TX_CKO_UDP_EN);
    XMPTRACE(XMP_DBG_INFO, ("Registry: UDP6 TX Checksum offload enabled\n"));
  }
  else
  {
    XMPTRACE(XMP_DBG_WRN, ("Registry: UDP6 Checksum offload disabled!\n"));
  }
  pNicCtx->txdCkoBits = txdCkoBits;
}

ULONG xmpNicGetMacOptions(
  IN xmpNicCtx_t             *pNicCtx)
{

  ULONG InfoVal = XMP_NIC_MAC_OPTIONS(pNicCtx);
  if ( pNicCtx->Flags & XMP_NIC_FLAGS_8021P )
  {
     InfoVal |= NDIS_MAC_OPTION_8021P_PRIORITY;
  }
  if ( pNicCtx->Flags & XMP_NIC_FLAGS_8021Q )
  {
    InfoVal |= NDIS_MAC_OPTION_8021Q_VLAN;
  }
  return InfoVal;
}

VOID
xmpDbgVerifyNBL(
  IN xmpNicCtx_t             *pNicCtx,
  IN NET_BUFFER_LIST         *nbl
)
{
  NET_BUFFER                 *nb;
  MDL                        *mdl;
  VOID                       *va;
  ULONG                       nb_len;
  ULONG                       mdl_len;
  ULONG                       mdlo;
  ULONG                       len = 0;

  nb   = NET_BUFFER_LIST_FIRST_NB(nbl);
  ASSERT(nb);

  while(nb)
  {
    nb_len  = NET_BUFFER_DATA_LENGTH(nb);
    mdl     = NET_BUFFER_CURRENT_MDL(nb);
    ASSERT(mdl);
    len = 0;

    while(mdl)
    {
      mdlo    = NET_BUFFER_CURRENT_MDL_OFFSET(nb);
      va      = MmGetMdlVirtualAddress(mdl);
      ASSERT(MmIsAddressValid(va));
      mdl_len = MmGetMdlByteCount(mdl);
      len    += mdl_len;
      mdl = NDIS_MDL_LINKAGE(mdl);
    }
    ASSERT(len >= nb_len);
    nb = NET_BUFFER_NEXT_NB(nb);
  }
}

VOID
xmpNicFreeTimer(
  IN  xmpNicCtx_t            *pNicCtx)
{
  XMPTRACE(XMP_DBG_TIMER, ("==> xmpNicFreeTimer:nic=%p\n", pNicCtx));

  NdisCancelTimerObject(pNicCtx->xmpTimerHandle);
  NdisFreeTimerObject(pNicCtx->xmpTimerHandle);

  XMPTRACE(XMP_DBG_TIMER, ("<== xmpNicFreeTimer:nic=%p\n", pNicCtx));
}

VOID
xmpNicTimerCallback(
   IN VOID  *SystemSpecific1,
   IN VOID  *pContext,
   IN VOID  *SystemSpecific2,
   IN VOID  *SystemSpecific3
   )
{
  xmpNicCtx_t                *pNicCtx = (xmpNicCtx_t *)pContext;
  xge_hal_device_t           *pHalDev;

  XMPTRACE(XMP_DBG_TIMER, ("==> xmpNicTimerCallback:nic=%p\n", pNicCtx));

  ASSERT(pNicCtx);
  pHalDev = XMP_NIC_GET_DEV(pNicCtx);
  if ( pNicCtx->mpState == XMP_NIC_MPSTATE_RUNNING )
    xge_hal_device_poll(XMP_NIC_GET_DEV(pNicCtx));

  if ( bit(pNicCtx->State, XMP_NIC_LED_BLINK_ENABLED) )
  {            
    xge_hal_flick_link_led(pHalDev);
    pNicCtx->LinkFlickCnt++;
  }

  if ( bit(pNicCtx->State, XMP_NIC_LED_BLINK_DISABLED) &&
       pNicCtx->LinkFlickCnt )
  {
    if ( pNicCtx->LinkFlickCnt % 2 )
      xge_hal_flick_link_led(pHalDev);

    xge_hal_restore_link_led(pHalDev);
    pNicCtx->LinkFlickCnt = 0;
  }
     
  XMPTRACE(XMP_DBG_TIMER, ("<== xmpNicTimerCallback:nic=%p\n", pNicCtx));
  return;
}

VOID
xmpNicStartTimer(
  IN  xmpNicCtx_t            *pNicCtx)
{
  NDIS_TIMER_CHARACTERISTICS xmpTimerChars;
  LARGE_INTEGER TimerExpVal;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpNicStartTimer:nic=%p\n", pNicCtx));
  
  XENAMP_NDIS_OBJECT_INIT
    (&xmpTimerChars.Header,
     NDIS_OBJECT_TYPE_TIMER_CHARACTERISTICS,
     NDIS_TIMER_CHARACTERISTICS_REVISION_1,
     sizeof(NDIS_TIMER_CHARACTERISTICS)); 

  xmpTimerChars.AllocationTag = XENAMP_MTAG;
  xmpTimerChars.TimerFunction = xmpNicTimerCallback;
  xmpTimerChars.FunctionContext = (VOID*) pNicCtx;

  NdisAllocateTimerObject(pNicCtx->hMPAdapter,
                          &xmpTimerChars,
                          &pNicCtx->xmpTimerHandle);
  /* Initialize Poll Time to -1 to sync with the current system Time */
  TimerExpVal.QuadPart = -1;

  NdisSetTimerObject(pNicCtx->xmpTimerHandle,
                     TimerExpVal,
                     XMP_POLL_TIMER_MILLIS,
                     (VOID *)pNicCtx);
 XMPTRACE(XMP_DBG_INFO, ("<== xmpNicStartTimer:nic=%p\n", pNicCtx));
}

NDIS_STATUS
xmpNicReadIntrMode(
  IN  NDIS_HANDLE             NdisHandle,
  OUT UINT                   *Value
  )
{
  UINT                        uiTmp;
  NDIS_STATUS                 Status;
  NDIS_HANDLE                 hConfig = NULL;
  NDIS_CONFIGURATION_PARAMETER *pValue;
  NDIS_CONFIGURATION_OBJECT   ConfigObject;
  NDIS_STRING  IntrModeKey  = NDIS_STRING_CONST("intr_mode");

  XMPTRACE(XMP_DBG_INI, ("==> xmpNicReadIntrMode \n"));

  ConfigObject.Header.Type = NDIS_OBJECT_TYPE_CONFIGURATION_OBJECT;
  ConfigObject.Header.Revision = NDIS_CONFIGURATION_OBJECT_REVISION_1;
  ConfigObject.Header.Size = sizeof(NDIS_CONFIGURATION_OBJECT);
  ConfigObject.NdisHandle = NdisHandle;
  ConfigObject.Flags = 0;
  if ( (Status = NdisOpenConfigurationEx(&ConfigObject, &hConfig)
          != NDIS_STATUS_SUCCESS) )
  {
    XMPTRACE(XMP_DBG_ERR, ("NdisOpenConfigurationEx failed = %x\n", Status));
    return NDIS_STATUS_FAILURE;
  }
  NdisReadConfiguration(&Status, &pValue, hConfig, &IntrModeKey,
                            NdisParameterInteger);
  if ( Status == NDIS_STATUS_SUCCESS )
   *Value = pValue->ParameterData.IntegerData;
  
  if (hConfig)
    NdisCloseConfiguration(hConfig);

  return Status;

 
}

#pragma warning(pop)   // Save the current warning state

