#ifndef XMPHAL_H
#define XMPHAL_H

#define XMP_HAL_EVENT_QUEUE_SZ_MIN      1
#define XMP_HAL_EVENT_QUEUE_SZ_MAX      4
#define XMP_HAL_CFG_ISR_POLLING_CNT     2
#define XMP_HAL_CFG_MC_PAUSE_THRESHOLD  185          /* Queue =185/255% full */

/* Value inserted in pause frames, 0xFFFF is the reset value                 */
#define XMP_HAL_CFG_MC_PAUSE_TIME       65535
#define XMP_HAL_CFG_TMAC_UTIL_PERIOD    5         /* Sampling period = 1.6ms */
#define XMP_HAL_CFG_RMAC_UTIL_PERIOD    5
#define XMP_HAL_CFG_RMAC_BCAST_ENABLE   1
#define XMP_HAL_CFG_MEM_BLOCK_SZ        PAGE_SIZE  /* TODO: CHECK            */

#define XMP_HAL_MAX_ADDRESSES           64
#define XMP_HAL_MC_ADDRESS_START_OFFSET 8          /* TODO: Change to S2HAL  */
#define XMP_HAL_MAX_MC_ADDRESSES        54            /* Multicast addresses */
#define XMP_HAL_MAC_ADDR_START_OFFSET   0
#define XMP_HAL_MAX_ADDRESS_OFFSET      63

#define XMP_HAL_MAX_MSI_MESSAGES        32
#define XMP_HAL_MAX_MSIX_MESSAGES       64

#define XMP_HAL_LRO_SG_SIZE             16
#define XMP_HAL_LRO_FRM_LEN             65535

typedef struct _xmpHalRxDtrCtx_t
{
  xmpRxNBLCtx_t         *pRxCtx;
} xmpHalRxDtrCtx_t;

#define XMP_HAL_MAP_HASH_TYPE(_htype, _ndishashtype)                          \
  switch(_htype)                                                              \
  {                                                                           \
  case XGE_HAL_RXD_HASH_TYPE_TCP_IPV4:                                        \
    _ndishashtype = NDIS_HASH_TCP_IPV4;                                       \
    break;                                                                    \
  case XGE_HAL_RXD_HASH_TYPE_IPV4:                                            \
    _ndishashtype = NDIS_HASH_IPV4;                                           \
    break;                                                                    \
  case XGE_HAL_RXD_HASH_TYPE_TCP_IPV6:                                        \
    _ndishashtype = NDIS_HASH_TCP_IPV6;                                       \
    break;                                                                    \
  case XGE_HAL_RXD_HASH_TYPE_IPV6:                                            \
    _ndishashtype = NDIS_HASH_IPV6;                                           \
    break;                                                                    \
  case XGE_HAL_RXD_HASH_TYPE_TCP_IPV6_EX:                                     \
    _ndishashtype = NDIS_HASH_TCP_IPV6_EX;                                    \
    break;                                                                    \
  case XGE_HAL_RXD_HASH_TYPE_IPV6_EX:                                         \
    _ndishashtype = NDIS_HASH_IPV6_EX;                                        \
    break;                                                                    \
  default:                                                                    \
    _ndishashtype = 0;                                                        \
    break;                                                                    \
  }

INT
xmpHalInit();

VOID
xmpHalDeInit();

NDIS_STATUS
xmpHalNicInit(
  IN  xmpNicCtx_t            *pNicCtx
  );

NDIS_STATUS
xmpHalNicStart(
  IN  xmpNicCtx_t            *pNicCtx
  );

VOID
xmpHalNicStop(
  IN  xmpNicCtx_t            *pNicCtx
  );

NDIS_STATUS
xmpHalNicReset(
  IN  xmpNicCtx_t            *pNicCtx
  );

VOID
xmpHalEnableInterrupts(
  IN  xmpNicCtx_t            *pNicCtx
  );

VOID
xmpHalDisableInterrupts(
  IN  xmpNicCtx_t            *pNicCtx
  );

VOID
xmpHalMcastFilterSet(
  IN  xmpNicCtx_t            *pNicCtx
  );

VOID
xmpHalMcastFilterClr(
  IN  xmpNicCtx_t            *pNicCtx
  );

NDIS_STATUS
xmpHalSetMcastList(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  UCHAR                  *mlist,
  IN  ULONG                  *count
  );

VOID
xmpHalReadPermanentAddress(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  UCHAR                  *pAddr
  );

NDIS_STATUS
xmpHalSetAddress(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  UCHAR                  *pAddr
  );

NDIS_STATUS
xmpHalGetLinkStatus(
  IN  xmpNicCtx_t            *pNicCtx
  );

VOID
xmpHalLinkUpCB(
  IN  void                   *userdata
  );

VOID
xmpHalLinkDownCB(
  IN  void                   *userdata
  );

VOID
xmpHalCritErrCB(
  IN  void                    *userdata,
  IN  xge_hal_event_e       type,
  IN  u64                      serr_data
  );

VOID
xmpHalEventCB(
  IN  xge_queue_item_t       *item
  );

VOID
xmpHalCallBack(
  IN  PVOID                   SystemSpecific1,
  IN  PVOID                   FunctionContext,
  IN  PVOID                   SystemSpecific2,
  IN  PVOID                   SystemSpecific3
  );

NDIS_STATUS
xmpHalPostRxD(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  xmpNicRxInfo_t         *pRxRing,
  IN  xmpRxNBLCtx_t          *pNBLCtx
  );

NDIS_STATUS
xmpHalPostRxDs(
  IN  xmpNicCtx_t            *pNicCtx
  );

xge_hal_status_e
xmpHalRssQueueFrameCB(
  IN  ULONG                   proc,
  IN  xmpNicCtx_t            *pNicCtx
  );

xge_hal_status_e
xmpHalRxFrameCB(
  IN  xge_hal_channel_h       channelh,
  IN  xge_hal_dtr_h           dtr,
  IN  u8                      rxStatus,
  IN  void                   *cbCtx
  );

xge_hal_status_e
xmpHalContinueRSSIrq(
  IN  NDIS_HANDLE             MiniportInterruptContext,
  IN  PVOID                   MiniportDpcContext,
  IN  ULONG                   proc
  );

xge_hal_status_e
xmphalIsRxQueueReady(
  IN  xge_hal_channel_h channelh
  );

xge_hal_status_e
xmpHalTxCompleteCB(
  IN  xge_hal_channel_h       channelh,
  IN  xge_hal_dtr_h           dtr,
  IN  u8                      txStatus,
  IN  void                   *cbCtx
  );

VOID
xmpHalTxCompleteNetBuf(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  xmpTxNBufCtx_t         *pNBufCtx,
  IN  ULONG                   LsoBytes,
  IN  NDIS_STATUS             Status,
  IN  BOOLEAN                 isDpcLevel,
  IN  BOOLEAN                 FlushIt
  );

VOID
xmpHalTxAbortCB(
  IN  xge_hal_channel_h         channelh,
  IN  xge_hal_dtr_h             dtr,
  IN  xge_hal_dtr_state_e       state,
  IN  void                     *userdata,
  IN  xge_hal_channel_reopen_e reopen
  );

VOID
xmpHalRxAbortCB(
  IN  xge_hal_channel_h         channelh,
  IN  xge_hal_dtr_h             dtr,
  IN  xge_hal_dtr_state_e       state,
  IN  void                     *userdata,
  IN  xge_hal_channel_reopen_e reopen
  );

VOID
xmpHalPollCB(
  IN  PVOID                   SystemSpecific1,
  IN  PVOID                   FunctionContext,
  IN  PVOID                   SystemSpecific2,
  IN  PVOID                   SystemSpecific3
  );

NDIS_STATUS
xmpHalGetHwStats( 
  IN  xmpNicCtx_t             *pNicCtx,
  OUT xge_hal_stats_hw_info_t **pStats 
  );

VOID
xmpHalEnableMSI(
  IN  xmpNicCtx_t            *pNicCtx);

VOID
xmpHalGetAdapterSerialnum(
  IN  xmpNicCtx_t            *pNic);

#endif

