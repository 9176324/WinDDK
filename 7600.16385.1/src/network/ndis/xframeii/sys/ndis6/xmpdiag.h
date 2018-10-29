#ifndef XMPDIAG_H
#define XMPDIAG_H

#define OID_CUSTOM_OID_START                    0xFFDD0000
#define OID_CUSTOM_OID_LED_BLINK_START          0xFFDD0001
#define OID_CUSTOM_OID_LED_BLINK_STOP           0xFFDD0002
#define OID_CUSTOM_OID_PERF_BIST_TEST           0xFFDD0003
#define OID_CUSTOM_OID_PERF_LOOP_TXRX_TEST      0xFFDD0004
#define OID_CUSTOM_OID_PERF_REG_TEST            0xFFDD0005
#define OID_CUSTOM_OID_PERF_ADAPT_EN_TEST       0xFFDD0006
#define OID_CUSTOM_OID_ADD_MAC_ADDR             0xFFDD0007
#define OID_CUSTOM_OID_RMV_MAC_ADDR             0xFFDD0008
#define OID_CUSTOM_OID_READ_REG                 0xFFDD0009
#define OID_CUSTOM_OID_WRITE_REG                0xFFDD000a
#define OID_CUSTOM_OID_GET_STATS                0xFFDD000b
#define OID_CUSTOM_OID_TX_BYTES                 0xFFDD000C
#define OID_CUSTOM_OID_RX_BYTES                 0xFFDD000D
#define OID_CUSTOM_OID_SET_TX_DMA_UTILZ         0xFFDD000E
#define OID_CUSTOM_OID_GET_TX_DMA_UTILZ         0xFFDD000F
#define OID_CUSTOM_OID_SET_RX_DMA_UTILZ         0xFFDD0010
#define OID_CUSTOM_OID_GET_RX_DMA_UTILZ         0xFFDD0011
#define OID_CUSTOM_OID_GET_TX_LNK_UTILZ         0xFFDD0012
#define OID_CUSTOM_OID_GET_RX_LNK_UTILZ         0xFFDD0013
 
#define OID_CUSTOM_OID_DIAG_INIT                0xFFDD0014
#define OID_CUSTOM_OID_DIAG_RESET_TEST_INIT     0xFFDD0015
#define OID_CUSTOM_OID_DIAG_RESET_TEST          0xFFDD0016
#define OID_CUSTOM_OID_DIAG_INIT_TEST           0xFFDD0017
#define OID_CUSTOM_OID_DIAG_SEND_TEST           0xFFDD0018
#define OID_CUSTOM_OID_DIAG_RCV_TEST            0xFFDD0019
#define OID_CUSTOM_OID_DIAG_FINISH_PH1          0xFFDD0020
#define OID_CUSTOM_OID_DIAG_FINISH_PH2          0xFFDD0021
#define OID_CUSTOM_OID_DIAG_FINISH              0xFFDD0022
#define OID_CUSTOM_OID_GET_STATS_DUMP           0xFFDD0023
#define OID_CUSTOM_OID_GET_REG_DUMP             0xFFDD0024
#define OID_CUSTOM_OID_XENA_REV                 0xFFDD0025
#define OID_CUSTOM_OID_PCI_DUMP                 0xFFDD0026
#define OID_CUSTOM_OID_DUMP_TRACE               0xFFDD0027

#define OID_CUSTOM_OID_GET_MAC_ADDR             0xFFDD0033
#define OID_CUSTOM_OID_DUMP_DRIVER_STATS        0xFFDD0034
#define OID_CUSTOM_OID_GET_PCI_MODE             0xFFDD0035
#define OID_CUSTOM_OID_SET_MAC_STEER            0xFFDD0036
#define OID_CUSTOM_OID_GET_SERIAL_NUMBER        0xFFDD0037

#define OID_CUSTOM_OID_GET_RSS_REG              0xFFDD00FF
#define OID_CUSTOM_OID_GET_RX_STATS             0xFFDD0100
#define OID_CUSTOM_OID_DUMP_ADAPTER_STATS       0xFFDD0101
#define OID_CUSTOM_OID_DUMP_BAR0                0xFFDD0102
#define OID_CUSTOM_OID_DUMP_PCI_CONFIG_SPACE    0xFFDD0103
#define OID_CUSTOM_DUMP_DRIVER_STATS_FORMATED   0xFFDD0104
#define OID_CUSTOM_DUMP_HAL_CONFIGURATION       0xFFDD0105
#define OID_CUSTOM_DUMP_ADAPTER_INFORMATION     0xFFDD0106
#define OID_CUSTOM_OID_DUMP_BAR2                0xFFDD0107

#define OID_CUSTOM_OID_END                      0xFFDD0107

#define XMP_DIAG_OID_LIST                                                     \
        OID_CUSTOM_OID_START,                                                 \
	OID_CUSTOM_OID_LED_BLINK_START,          			      \
        OID_CUSTOM_OID_LED_BLINK_STOP,                                        \
        OID_CUSTOM_OID_PERF_BIST_TEST,                                        \
        OID_CUSTOM_OID_PERF_LOOP_TXRX_TEST,                                   \
        OID_CUSTOM_OID_PERF_REG_TEST,                                         \
        OID_CUSTOM_OID_PERF_ADAPT_EN_TEST,                                    \
        OID_CUSTOM_OID_ADD_MAC_ADDR,                                          \
        OID_CUSTOM_OID_RMV_MAC_ADDR,                                          \
        OID_CUSTOM_OID_READ_REG,                                              \
        OID_CUSTOM_OID_WRITE_REG,                                             \
        OID_CUSTOM_OID_GET_STATS,                                             \
        OID_CUSTOM_OID_TX_BYTES,                                              \
        OID_CUSTOM_OID_RX_BYTES,                                              \
        OID_CUSTOM_OID_SET_TX_DMA_UTILZ,                                      \
        OID_CUSTOM_OID_GET_TX_DMA_UTILZ,                                      \
        OID_CUSTOM_OID_SET_RX_DMA_UTILZ,                                      \
        OID_CUSTOM_OID_GET_RX_DMA_UTILZ,                                      \
        OID_CUSTOM_OID_GET_TX_LNK_UTILZ,                                      \
        OID_CUSTOM_OID_GET_RX_LNK_UTILZ,                                      \
        OID_CUSTOM_OID_DIAG_INIT,                                             \
        OID_CUSTOM_OID_DIAG_RESET_TEST_INIT,                                  \
        OID_CUSTOM_OID_DIAG_RESET_TEST,                                       \
        OID_CUSTOM_OID_DIAG_INIT_TEST,                                        \
        OID_CUSTOM_OID_DIAG_SEND_TEST,                                        \
        OID_CUSTOM_OID_DIAG_RCV_TEST,                                         \
        OID_CUSTOM_OID_DIAG_FINISH_PH1,                                       \
        OID_CUSTOM_OID_DIAG_FINISH_PH2,                                       \
        OID_CUSTOM_OID_DIAG_FINISH,                                           \
        OID_CUSTOM_OID_GET_STATS_DUMP,                                        \
        OID_CUSTOM_OID_GET_REG_DUMP,                                          \
        OID_CUSTOM_OID_XENA_REV,                                              \
        OID_CUSTOM_OID_PCI_DUMP,                                              \
        OID_CUSTOM_OID_DUMP_TRACE,                                            \
	OID_CUSTOM_OID_GET_PCI_MODE,                                          \
        OID_CUSTOM_OID_SET_MAC_STEER,                                         \
        OID_CUSTOM_OID_GET_SERIAL_NUMBER,                                     \
        OID_CUSTOM_OID_DUMP_DRIVER_STATS,	                              \
        OID_CUSTOM_OID_DUMP_ADAPTER_STATS,                                    \
        OID_CUSTOM_OID_DUMP_BAR0,                                             \
        OID_CUSTOM_OID_DUMP_PCI_CONFIG_SPACE,                                 \
        OID_CUSTOM_OID_DUMP_DRIVER_STATS,                                     \
        OID_CUSTOM_OID_GET_RX_STATS,                                          \
        OID_CUSTOM_OID_GET_RSS_REG

#define XG_DIAG_SUCCESS                     0
#define XG_DIAG_ERR_HAL_STOP_ERROR          1
#define XG_DIAG_ERR_RESET_ERROR             2
#define XG_DIAG_ERR_PCI_WR_FAILED           3
#define XG_DIAG_ERR_PCI_RD_FAILED           4
#define XG_DIAG_ERR_RESET_VALUES            5
#define XG_DIAG_ERR_LINK                    6
#define XG_DIAG_ERR_BIST                    7
#define XG_DIAG_ERR_HAL_INIT_FAILED         8
#define XG_DIAG_ERR_HAL_START_FAILED        9 
#define XG_DIAG_TX_FAILED                   10
#define XG_DIAG_TX_COMPLETE_FAILED          11
#define XG_DIAG_RX_FAILED                   12
#define XG_DIAG_RX_LEN_INCORRECT            13
#define XG_DIAG_RX_CORRUPT_DATA             14
#define XG_DIAG_RX_ABORT_ERROR              15
#define XG_DIAG_RX_PARITY_ERROR             16
#define XG_DIAG_RX_RDA_ERROR                17
#define XG_DIAG_RX_FCS_ERROR                18
#define XG_DIAG_RX_BUFF_SZ_ERROR            19
#define XG_DIAG_RX_BUFF_ECC_ERROR           20
#define XG_DIAG_RX_PARITY_ABORT_ERROR       21
#define XG_DIAG_RX_BUFF_UNKNOWN_ERROR       22

#define XG_DIAG_PKT_MARKER                  1
#define XG_DIAG_PKT_BUFF                    0x1000

typedef struct _XMP_DIAG_MAC_ADDR_INFO
{
  UCHAR macaddr[6];
  U32 offset;
  BOOLEAN remove;                          /* if not to remove a MAC address */
} XMP_DIAG_MAC_ADDR_INFO, *PXMP_DIAG_MAC_ADDR_INFO;

/* This structure is used to provide info regarding the PCI slot to GUI      */
typedef struct _pci_mode_t
{
  int pci_mode_e;
  int pci_bus_type_e;
  int pci_bus_width_e;
  int pci_bus_frequency_e; 
} pci_mode_t;

typedef struct _diag_dummy_t
{
  U32 dummy;
  U32 reason_code;
} diag_dummy_t;

typedef struct _diag_reset_t
{
  U32 dummy;
  U32 reason_code;
  U32 loopback_mode;
} diag_reset_t;

typedef struct _diag_dummy_t diag_init_t;
typedef struct _diag_dummy_t diag_reset_init_t;
typedef struct _diag_dummy_t diag_init_test_t;
typedef struct _diag_dummy_t diag_send_t;
typedef struct _diag_dummy_t diag_recv_test_t;
typedef struct _diag_dummy_t diag_fin_test_t;

NDIS_STATUS
xmpDiagQueryInformation(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  NDIS_OID                Oid,
  IN  VOID                   *pInfoBuf,
  IN  ULONG                   InfoBufLen,
  OUT ULONG                  *pWritten,
  OUT ULONG                  *pNeeded
  );

NDIS_STATUS
xmpDiagSetInformation(
  IN  xmpNicCtx_t             *pNicCtx,
  IN  NDIS_OID                Oid,
  IN  VOID                   *pInfoBuf,
  IN  ULONG                   InfoBufLen,
  OUT ULONG                  *pRead,
  OUT ULONG                  *pNeeded
  );

NDIS_STATUS
xmpDiagInit(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  );

NDIS_STATUS
xmpDiagResetTestInit(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  );

NDIS_STATUS
xmpDiagResetTest(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  );

NDIS_STATUS
xmpDiagInitTest(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  );

NDIS_STATUS
xmpDiagSend( 
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  );

void
xmpDiagSendComplete(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  xmpTxNBufCtx_t         *pNBufCtx,
  IN  U8                      txStatus
  );

void
xmpDiagRecvComplete(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  xmpRxNBLCtx_t          *pNBLCtx,
  IN  ULONG                   pktLen,
  IN  U8                      rxStatus,
  IN  U32                     RingNum
  );

NDIS_STATUS
xmpDiagRecv(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  );

NDIS_STATUS
xmpDiagFinish1(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  );

NDIS_STATUS
xmpDiagFinish2(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  );

NDIS_STATUS
xmpDiagFinish(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  );
#endif

