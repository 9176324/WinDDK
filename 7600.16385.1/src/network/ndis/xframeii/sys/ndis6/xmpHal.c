#pragma warning(push)   // Save the current warning state
#pragma warning(disable:4100) //unreference parameter in functions
#pragma warning(disable:4101)  //unreference local variables
#pragma warning(disable:4189)  //local variable is initialized but not referenced
#pragma warning( disable : 4127 )
#pragma warning( disable : 4706 )
#include "precomp.h"

xge_hal_device_config_t      devCfg;
xge_hal_device_attr_t        devAttr;

INT
xmpHalInit()
{
  xge_hal_driver_config_t       drvCfg;
  xge_hal_uld_cbs_t             cbk = {0};
  xge_hal_status_e              halStatus;

  drvCfg.queue_size_initial = XMP_HAL_EVENT_QUEUE_SZ_MIN;
  drvCfg.queue_size_max     = XMP_HAL_EVENT_QUEUE_SZ_MAX;

  cbk.link_up   = xmpHalLinkUpCB;
  cbk.link_down = xmpHalLinkDownCB;
  cbk.crit_err  = xmpHalCritErrCB;
  cbk.event     = xmpHalEventCB;
  cbk.event_queued = NULL;
  cbk.before_device_poll = NULL;
  cbk.after_device_poll = NULL;
  cbk.sched_timer = NULL;

  if ( (halStatus = xge_hal_driver_initialize(&drvCfg, &cbk))
       != XGE_HAL_OK )
  {
    XMPTRACE(XMP_DBG_ERR, ("DriverEntry: xge_hal_driver_initialize Failed=%x\n",
                           halStatus));
    return -1;
  }
  return 0;
}

VOID
xmpHalDeInit()
{
  /* TODO: Stop the poll timer */
  xge_hal_driver_terminate();
}

NDIS_STATUS
xmpHalNicInit(
  IN  xmpNicCtx_t            *pNicCtx
  )
{
  xge_hal_status_e             halStatus;
  UINT                         i,j;
  UINT                         rxdInBlk;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpHalNicInit:nic=%p\n", pNicCtx));

  XF_MCLEAR((VOID *)&devCfg, sizeof(devCfg));
  XF_MCLEAR((VOID *)&devAttr, sizeof(devAttr));

  devCfg.mtu                     = XMP_NIC_CURRENT_MTU(pNicCtx);
  devCfg.isr_polling_cnt         = XMP_HAL_CFG_ISR_POLLING_CNT;
  devCfg.no_isr_events           = 1;
  devCfg.latency_timer           = (pNicCtx->RegParams.latency_timer == 1)?
                                    XGE_HAL_USE_BIOS_DEFAULT_LATENCY : 
                                    pNicCtx->RegParams.latency_timer;

  devCfg.napi_weight             = XGE_HAL_DEF_NAPI_WEIGHT;
  devCfg.max_splits_trans        = (pNicCtx->RegParams.max_split_trans > 
                                    XGE_HAL_THIRTYTWO_SPLIT_TRANSACTION ) ?
                                    XGE_HAL_USE_BIOS_DEFAULT_SPLITS :
                                    pNicCtx->RegParams.max_split_trans;

  devCfg.mmrb_count              = (pNicCtx->RegParams.max_read_byte_cnt >  XGE_HAL_MAX_MMRB_COUNT) ?
                                    XGE_HAL_DEFAULT_BIOS_MMRB_COUNT :
                                    pNicCtx->RegParams.max_read_byte_cnt;
  devCfg.shared_splits           = pNicCtx->RegParams.shared_splits;
  devCfg.stats_refresh_time_sec  = pNicCtx->RegParams.stats_refresh_time;
  devCfg.pci_freq_mherz          = XGE_HAL_PCI_FREQ_MHERZ_133;

  devCfg.intr_mode               = pNicCtx->RegParams.intr_mode;
  devCfg.sched_timer_us          = pNicCtx->RegParams.sched_intr_timer_val;

  devCfg.sched_timer_one_shot    = XGE_HAL_SCHED_TIMER_ON_SHOT_DISABLE;

  /* Configure ring properties                                               */
  devCfg.ring.memblock_size      = XMP_HAL_CFG_MEM_BLOCK_SZ;
  devCfg.ring.strip_vlan_tag     = (XMP_NIC_8021PQ_IS_ENABLED(pNicCtx) ?
                                    XGE_HAL_RING_STRIP_VLAN_TAG :
                                    XGE_HAL_RING_DONOT_STRIP_VLAN_TAG);

  devCfg.lro_sg_size = XMP_HAL_LRO_SG_SIZE;
  devCfg.lro_frm_len = XMP_HAL_LRO_FRM_LEN;

  if ( XMP_NIC_RX_SPLIT_COMBINE(pNicCtx) )
  { /* 2 buffer mode.                                                        */
    devCfg.ring.scatter_mode       = XGE_HAL_RING_QUEUE_SCATTER_MODE_B;
    rxdInBlk = XGE_HAL_RING_RXDS_PER_BLOCK(XGE_HAL_RING_QUEUE_BUFFER_MODE_3);
  }
  else if ( XMP_NIC_RX_SPLIT(pNicCtx) )
  {
    /* in SCATTER_MODE_A header buffer is split into two parts: 1st part
     * receives ethernet header, 2nd part receives IP+TCP header. Then
     * headers are combined together by copying ETH header just ahead of
     * IP header to get all headers contiguous.
     */
    devCfg.ring.scatter_mode       = XGE_HAL_RING_QUEUE_SCATTER_MODE_A;
    rxdInBlk = XGE_HAL_RING_RXDS_PER_BLOCK(XGE_HAL_RING_QUEUE_BUFFER_MODE_3);
  }
  else
  { /* 1 buffer mode.                                                        */
    devCfg.ring.scatter_mode       = XGE_HAL_RING_QUEUE_SCATTER_MODE_A;
    rxdInBlk = XGE_HAL_RING_RXDS_PER_BLOCK(XGE_HAL_RING_QUEUE_BUFFER_MODE_1);
  }

  for (i=0; i < pNicCtx->RegParams.RxRingNum; i++)
  {
    devCfg.ring.queue[i].max           = pNicCtx->RxInfo[i].CfgRxDCnt/rxdInBlk;
    devCfg.ring.queue[i].initial       = pNicCtx->RxInfo[i].CfgRxDCnt/rxdInBlk;

    if ( XMP_NIC_RX_SPLIT(pNicCtx) )
      devCfg.ring.queue[i].buffer_mode   = XGE_HAL_RING_QUEUE_BUFFER_MODE_3;
    else
      devCfg.ring.queue[i].buffer_mode   = XGE_HAL_RING_QUEUE_BUFFER_MODE_1;

    devCfg.ring.queue[i].dram_size_mb  = XGE_HAL_DEFAULT_USE_HARDCODE;
    devCfg.ring.queue[i].max_frm_len   = XGE_HAL_RING_USE_MTU;
    devCfg.ring.queue[i].priority      = 0;
    devCfg.ring.queue[i].rth_en        = pNicCtx->RegParams.rss_enable;
    devCfg.ring.queue[i].no_snoop_bits = 0;
    devCfg.ring.queue[i].configured    = 1;
    devCfg.ring.queue[i].rti.urange_a  = (UINT) pNicCtx->RegParams.rx_urange_a;
    devCfg.ring.queue[i].rti.ufc_a     = (UINT) pNicCtx->RegParams.rx_ufc_a;
    devCfg.ring.queue[i].rti.urange_b  = (UINT) pNicCtx->RegParams.rx_urange_b;
    devCfg.ring.queue[i].rti.ufc_b     = (UINT) pNicCtx->RegParams.rx_ufc_b;
    devCfg.ring.queue[i].rti.urange_c  = (UINT) pNicCtx->RegParams.rx_urange_c;
    devCfg.ring.queue[i].rti.ufc_c     = (UINT) pNicCtx->RegParams.rx_ufc_c;
    devCfg.ring.queue[i].rti.ufc_d     = (UINT) pNicCtx->RegParams.rx_ufc_d;
    devCfg.ring.queue[i].rti.timer_ac_en = 
                        (int) !pNicCtx->RegParams.rx_ac_disable;
    devCfg.ring.queue[i].rti.timer_val_us = 
                        (int) pNicCtx->RegParams.rx_timer_val;
    devCfg.ring.queue[i].indicate_max_pkts =
                        (int) pNicCtx->RegParams.indicate_max_pkts;
    devCfg.ring.queue[i].backoff_interval_us = 
                        pNicCtx->RegParams.RxD_Backoff_interval;
  }
#if 0
 for (j=0; j < pNicCtx->RegParams.RxRingNum; j++)
 {
   if (pNicCtx->PciCfg.VendorID == XENAMP_PCI_DEVICE_ID_HERC) 
   {
     devCfg.ring.queue[i].dram_size_mb  =     
      (int)(XGE_HAL_MAX_RING_QUEUE_SIZE_HERC / i); 
   }
   else
     devCfg.ring.queue[i].dram_size_mb  =     
       (int)(XGE_HAL_MAX_RING_QUEUE_SIZE_XENA / i); 

 }
#endif
  devCfg.mac.tmac_util_period        = XMP_HAL_CFG_TMAC_UTIL_PERIOD;
  devCfg.mac.rmac_util_period        = XMP_HAL_CFG_RMAC_UTIL_PERIOD;
  devCfg.mac.rmac_bcast_en           = XMP_HAL_CFG_RMAC_BCAST_ENABLE;
  devCfg.mac.rmac_pause_gen_en       = 0;
  devCfg.mac.rmac_pause_rcv_en       = 0;
  devCfg.mac.rmac_pause_time         = XMP_HAL_CFG_MC_PAUSE_TIME;
  devCfg.mac.mc_pause_threshold_q0q3 = XMP_HAL_CFG_MC_PAUSE_THRESHOLD;
  devCfg.mac.mc_pause_threshold_q4q7 = XMP_HAL_CFG_MC_PAUSE_THRESHOLD;

   /* Configure Tx Fifo properties                                           */
  devCfg.fifo.max_frags              = pNicCtx->RegParams.MaxTxDs;
  devCfg.fifo.reserve_threshold      = XGE_HAL_MIN_FIFO_RESERVE_THRESHOLD;
  devCfg.fifo.memblock_size          = XMP_HAL_CFG_MEM_BLOCK_SZ;
  devCfg.fifo.alignment_size         = XGE_HAL_MIN_ALIGNMENT_SIZE;

  for (i=0; i < pNicCtx->RegParams.TxFIFONum; i++)
  {
    devCfg.fifo.queue[i].max         = pNicCtx->TxFifo[i].CfgTxDCnt;
    devCfg.fifo.queue[i].initial     = pNicCtx->TxFifo[i].CfgTxDCnt;
    devCfg.fifo.queue[i].intr        = XGE_HAL_MIN_FIFO_QUEUE_INTR;    
  /* Type = utilization S2HAL_MIN_FIFO_QUEUE_INTR;                         */
    devCfg.fifo.queue[i].no_snoop_bits =
      XGE_HAL_MIN_FIFO_QUEUE_NO_SNOOP_DISABLED;
    devCfg.fifo.queue[i].configured  = XGE_HAL_MAX_FIFO_CONFIGURED;

  /* TODO: Change the inf and reg                                          */
    devCfg.fifo.queue[i].tti[0].enabled = XGE_HAL_TTI_ENABLE;
    devCfg.fifo.queue[i].tti[0].urange_a = pNicCtx->RegParams.tx_urange_a;
    devCfg.fifo.queue[i].tti[0].ufc_a = pNicCtx->RegParams.tx_ufc_a;
    devCfg.fifo.queue[i].tti[0].urange_b = pNicCtx->RegParams.tx_urange_b;
    devCfg.fifo.queue[i].tti[0].ufc_b = pNicCtx->RegParams.tx_ufc_b;
    devCfg.fifo.queue[i].tti[0].urange_c = pNicCtx->RegParams.tx_urange_c;
    devCfg.fifo.queue[i].tti[0].ufc_c = pNicCtx->RegParams.tx_ufc_c;
    devCfg.fifo.queue[i].tti[0].ufc_d = pNicCtx->RegParams.tx_ufc_d;
  /* TODO: check val for rss */ 
    devCfg.fifo.queue[i].tti[0].timer_val_us = pNicCtx->RegParams.tx_timer_val;       
    devCfg.fifo.queue[i].tti[0].timer_ac_en = !pNicCtx->RegParams.tx_ac_disable;
    devCfg.fifo.queue[i].tti[0].timer_ci_en = pNicCtx->RegParams.tx_utilz_periodic;
  }
  devCfg.dump_on_serr                = XGE_HAL_DUMP_ON_SERR_DISABLE;
  devCfg.dump_on_eccerr              = XGE_HAL_DUMP_ON_ECCERR_DISABLE;
  devCfg.dump_on_parityerr           = XGE_HAL_DUMP_ON_PARITYERR_DISABLE;

  devCfg.rth_en                      = XGE_HAL_RTH_DISABLE;
  devCfg.rth_bucket_size             = XGE_HAL_MAX_RTH_BUCKET_SIZE;
  devCfg.rth_spdm_en                 = XGE_HAL_RTH_SPDM_DISABLE;
  devCfg.rth_spdm_use_l4             = XGE_HAL_RTH_SPDM_USE_L4;

#ifndef XGE_HAL_IRQ_POLLING			// NK - remove these ifdef's since sources file never defines this flags
  devCfg.rxufca_intr_thres           = 35;
  devCfg.rxufca_lo_lim               = 1;
  devCfg.rxufca_hi_lim               = 16;
  devCfg.rxufca_lbolt_period         = 1;
#endif

  devCfg.link_valid_cnt              = XGE_HAL_LINK_VALID_CNT_MIN;
  devCfg.link_retry_cnt              = XGE_HAL_LINK_RETRY_CNT_MIN;
  devCfg.link_stability_period       = XGE_HAL_DEFAULT_LINK_STABILITY_PERIOD;
  devCfg.device_poll_millis          = XGE_HAL_DEFAULT_DEVICE_POLL_MILLIS;

  devAttr.bar0                       = (char *) pNicCtx->deviceMem1;
  devAttr.bar1                       = (char *) pNicCtx->deviceMem2;
  devAttr.bar2                       = (char *) pNicCtx->deviceMem3;
  devAttr.regh0                      = pNicCtx->hMPAdapter;
  devAttr.regh1                      = pNicCtx->hMPAdapter;
  devAttr.regh2                      = pNicCtx->hMPAdapter;
  devAttr.irqh                       = pNicCtx->hMPAdapter;
  devAttr.cfgh                       = pNicCtx->hMPAdapter;
  devAttr.pdev                       = pNicCtx->hMPAdapter;

  /* TODO: hal poll timer for slow path */
  if ( (halStatus = xge_hal_device_initialize(XMP_NIC_GET_DEV(pNicCtx),
                                              &devAttr, &devCfg))
       != XGE_HAL_OK )
  {
    XMPTRACE(XMP_DBG_ERR, ("xge_hal_device_initialize failed: Err=%x\n",
                           halStatus));
    return NDIS_STATUS_FAILURE;
  }

  xge_hal_device_private_set(XMP_NIC_GET_DEV(pNicCtx), pNicCtx);

  xmpHalGetAdapterSerialnum( pNicCtx );
 
  if ( (halStatus = xge_hal_pma_loopback(XMP_NIC_GET_DEV(pNicCtx),
                                         pNicCtx->bLoopback)) != XGE_HAL_OK )
  {
    XMPTRACE(XMP_DBG_ERR, ("xge_hal_pma_loopback failed: Err=%x\n",
                           halStatus));
    return NDIS_STATUS_FAILURE;
  }


  XMPTRACE(XMP_DBG_INFO, ("<== xmpHalNicInit:nic=%p\n", pNicCtx));
  return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
xmpHalNicStart(
  IN  xmpNicCtx_t            *pNicCtx
  )
{
  xge_hal_device_t           *pHalDev;
  xmpQLink_t                 *link;
  xmpRxNBLCtx_t              *pNBLCtx;
	xge_hal_channel_attr_t      attr = {0};
  UINT                        i;
  UINT                        ui;
  NDIS_STATUS                 Status = NDIS_STATUS_FAILURE;
  xge_hal_status_e            halStatus;

  pHalDev = XMP_NIC_GET_DEV(pNicCtx);

  XMPTRACE(XMP_DBG_INFO, ("==> xmpHalNicStart:nic=%p\n", pNicCtx));

  do
  {
    /*if ( (halStatus = s2hal_device_mtu_set(pHalDev,
                                           XMP_NIC_CURRENT_MTU(pNicCtx)))
         != S2HAL_OK )
    {
      XMPTRACE(XMP_DBG_ERR, ("s2hal_device_mtu_set failed: Err=%x\n",
                             halStatus));
      break;
      }*/

    if ( (halStatus = xge_hal_device_enable(pHalDev)) != XGE_HAL_OK )
    {
      XMPTRACE(XMP_DBG_ERR, ("xge_hal_device_enable failed: Err=%x\n",
                             halStatus));
      break;
    }
 
    for (i=0; i < pNicCtx->RegParams.RxRingNum; i++)
    {
      UINT posted;
      UINT ulp;
      UINT free;

      attr.post_qid	     = i;
      attr.compl_qid	   = 0;
      attr.callback	     = xmpHalRxFrameCB;
      attr.per_dtr_space = sizeof(xmpHalRxDtrCtx_t);
      attr.flags         = 0;
      attr.type          = XGE_HAL_CHANNEL_TYPE_RING;
      attr.userdata	     = (void *) pNicCtx;
      attr.dtr_term		   = xmpHalRxAbortCB;
      attr.dtr_init      = NULL;
      if ( (halStatus = xge_hal_channel_open(pHalDev, &attr,
                                             &pNicCtx->RxInfo[i].halChannel,
                                             XGE_HAL_CHANNEL_OC_NORMAL))
           != XGE_HAL_OK )
      {
        XMPTRACE(XMP_DBG_ERR, ("xge_hal_channel_open Rx[%d] failed: Err=%x\n",
                               i, halStatus));
        break;
      }

      /* recover the posted RxDs.                                            */
      posted = ulp = free = 0;
      XF_GET_SLOCK(&(pNicCtx->RxInfo[i].RxLock));
        link = &pNicCtx->RxInfo[i].RxNBLQ;
        for ( ui=0; ui < pNicCtx->RxInfo[i].RxDCnt; ui++ )
        {
          link = XF_Q_NEXT(&pNicCtx->RxInfo[i].RxNBLQ, link);
          ASSERT(link != NULL);
          pNBLCtx = CONTAINING_RECORD(link, xmpRxNBLCtx_t, NBLQLink);
          if ( pNBLCtx->state == XMP_RXNBL_STATE_POSTED )
          {
            pNBLCtx->state = XMP_RXNBL_STATE_FREE;
            posted++;
          }
          else if ( pNBLCtx->state == XMP_RXNBL_STATE_ULP )
            ulp++;
          else
            free++;
        }
      bis(pNicCtx->RxInfo[i].State, XMP_RX_RING_STATE_OPEN);
      XF_FREE_SLOCK(&(pNicCtx->RxInfo[i].RxLock));
      ASSERT( pNicCtx->RxInfo[i].RxDCnt == (posted + ulp + free) );
    }

    if ( halStatus != XGE_HAL_OK )
      break;

    for (i=0; i < pNicCtx->RegParams.TxFIFONum; i++)
    {
      attr.post_qid	     = i;
      attr.compl_qid	   = 0;
      attr.callback	     = xmpHalTxCompleteCB;
      attr.per_dtr_space = sizeof(xmpTxNBufCtx_t) + pNicCtx->SGListSize;
      attr.flags         = 0;
      attr.type          = XGE_HAL_CHANNEL_TYPE_FIFO;
      attr.userdata	     = (void *)pNicCtx;
      attr.dtr_term		   = xmpHalTxAbortCB;
      attr.dtr_init		   = NULL;
      if ( (halStatus = xge_hal_channel_open(pHalDev, &attr,
                          &pNicCtx->TxFifo[i].halChannel, XGE_HAL_CHANNEL_OC_NORMAL)) !=
           XGE_HAL_OK )
      {
        XMPTRACE(XMP_DBG_ERR, ("xge_hal_channel_open Tx[%d] failed: Err=%x\n",
                               i, halStatus));
        break;
      }
      bis(pNicCtx->TxFifo[i].State, XMP_TX_FIFO_STATE_OPEN);
    }
    if ( halStatus != XGE_HAL_OK )
      break;

    if ( (Status = xmpHalPostRxDs(pNicCtx)) != NDIS_STATUS_SUCCESS )
      break;
    
     Status = NDIS_STATUS_SUCCESS;
  } while(0);

  XMPTRACE(XMP_DBG_INFO, ("<== xmpHalNicStart:nic=%p status=%x\n",
                          pNicCtx, Status));
  return Status;
}

VOID
xmpHalNicStop(
  IN  xmpNicCtx_t            *pNicCtx
  )
{
}

NDIS_STATUS
xmpHalNicReset(
  IN  xmpNicCtx_t            *pNicCtx
  )
{
  xmpTxNBufCtx_t             *pNBufCtx;
  xmpNicTxFifo_t             *pFifo;
  xmpNicRxInfo_t             *pRxRing;
  UINT                        i;
  BOOLEAN                     bclose;
  NDIS_STATUS                 Status;
  xge_hal_status_e            halStatus;
  xge_hal_dtr_h               dtr;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpHalNicReset:nic=%p\n", pNicCtx));

  halStatus = xge_hal_device_disable(XMP_NIC_GET_DEV(pNicCtx));
  if ( halStatus != XGE_HAL_OK )
  {
    XMPTRACE(XMP_DBG_ERR, ("xmpHalNicReset: xgehal_device_disable Failed=%x\n",
                           halStatus));
  }

  xge_hal_device_intr_disable(XMP_NIC_GET_DEV(pNicCtx));

  for (i=0; i < pNicCtx->RegParams.TxFIFONum; i++)
  {
    if ( bit(pNicCtx->TxFifo[i].State, XMP_TX_FIFO_STATE_OPEN) )
    {
      pFifo = &pNicCtx->TxFifo[i];
      xge_hal_channel_close(pFifo->halChannel, XGE_HAL_CHANNEL_OC_NORMAL);
      bic(pFifo->State, XMP_TX_FIFO_STATE_OPEN);
      ASSERT(pFifo->OsSendCnt == 0);
    }
  }
  for (i=0; i < pNicCtx->RegParams.RxRingNum; i++)
  {
    pRxRing = &pNicCtx->RxInfo[i];
    bclose = FALSE;
    XF_GET_SLOCK(&pRxRing->RxLock);
      if ( bit(pRxRing->State, XMP_RX_RING_STATE_OPEN) )
      {
        /* ASSERT(pRxRing->UlpNBLCnt == 0); */
        bic(pRxRing->State, XMP_RX_RING_STATE_OPEN);
        bclose = TRUE;
      }
    XF_FREE_SLOCK(&pRxRing->RxLock);
    if ( bclose )
      xge_hal_channel_close(pRxRing->halChannel, XGE_HAL_CHANNEL_OC_NORMAL);
  }

  xge_hal_device_reset(XMP_NIC_GET_DEV(pNicCtx));

  XMPTRACE(XMP_DBG_INFO, ("<== xmpHalNicReset:nic=%p\n", pNicCtx));
  return NDIS_STATUS_SUCCESS;
}

VOID
xmpHalEnableInterrupts(
  IN  xmpNicCtx_t            *pNicCtx
  )
{
  xge_hal_device_t             *pHalDev;

  pHalDev = XMP_NIC_GET_DEV(pNicCtx);
  xge_hal_device_intr_enable(pHalDev);  
}

VOID
xmpHalDisableInterrupts(
  IN  xmpNicCtx_t            *pNicCtx
  )
{
  xge_hal_device_t             *pHalDev;

  pHalDev = XMP_NIC_GET_DEV(pNicCtx);
  xge_hal_device_intr_disable(pHalDev);  
}

VOID
xmpHalMcastFilterSet(
  IN  xmpNicCtx_t            *pNicCtx
  )
{
  xge_hal_device_t           *pHalDev;
  ULONG                       slot;

  pHalDev = XMP_NIC_GET_DEV(pNicCtx);

  for ( slot = XMP_HAL_MC_ADDRESS_START_OFFSET;
        slot < (XMP_HAL_MC_ADDRESS_START_OFFSET + pNicCtx->AddrList.McCnt);
        slot++ )
  {
    xge_hal_device_macaddr_set(pHalDev, (int)slot,
                               (UCHAR*)&pNicCtx->AddrList.MacAddr[slot]);
  }
}

VOID
xmpHalMcastFilterClr(
  IN  xmpNicCtx_t            *pNicCtx
  )
{
  xge_hal_device_t           *pHalDev;
  ULONG                       slot;
  UCHAR                       delAddr[6]={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  pHalDev = XMP_NIC_GET_DEV(pNicCtx);

  for ( slot = XMP_HAL_MC_ADDRESS_START_OFFSET;
        slot < (XMP_HAL_MC_ADDRESS_START_OFFSET + pNicCtx->AddrList.McCnt);
        slot++ )
  {
    xge_hal_device_macaddr_set(pHalDev, (int)slot, delAddr);
  }
}

NDIS_STATUS
xmpHalSetMcastList(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  UCHAR                  *mlist,
  IN  ULONG                  *count
  )
{

  xge_hal_device_t           *pHalDev;
  ULONG                       slot;
  ULONG                       orig;
  ULONG                       toadd;
  UCHAR                       delAddr[6]={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  pHalDev = XMP_NIC_GET_DEV(pNicCtx);

  toadd = (*count > XMP_HAL_MAX_MC_ADDRESSES ? XMP_HAL_MAX_MC_ADDRESSES : 
           *count);

  for ( slot = XMP_HAL_MC_ADDRESS_START_OFFSET;
        slot < (XMP_HAL_MC_ADDRESS_START_OFFSET + toadd);
        slot++ )
  { /* overwrite the current list with the new list.                         */
    NdisMoveMemory((UCHAR *)&pNicCtx->AddrList.MacAddr[slot], mlist,
                   ETH_LENGTH_OF_ADDRESS);
    if ( pNicCtx->PacketFilter & NDIS_PACKET_TYPE_MULTICAST )
      xge_hal_device_macaddr_set(pHalDev, (int)slot, mlist);
    mlist += ETH_LENGTH_OF_ADDRESS;
  }

  for ( ; slot < (XMP_HAL_MC_ADDRESS_START_OFFSET + pNicCtx->AddrList.McCnt);
        slot++ )
  { /* clear the rest.                                                       */
    xge_hal_device_macaddr_set(pHalDev, (int)slot, delAddr);
  }

  orig   = *count;
  *count = toadd;
  pNicCtx->AddrList.McCnt = toadd;
  return (toadd < orig ? NDIS_STATUS_MULTICAST_FULL : NDIS_STATUS_SUCCESS);
}

NDIS_STATUS
xmpHalSetAddress(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  UCHAR                  *pAddr
  )
{
  if ( xge_hal_device_macaddr_set(XMP_NIC_GET_DEV(pNicCtx), 0, &pAddr[0]) 
       != XGE_HAL_OK )
  {
   return NDIS_STATUS_FAILURE;
  }
  return NDIS_STATUS_SUCCESS;
}

VOID
xmpHalReadPermanentAddress(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  UCHAR                  *pAddr
  )
{
 macaddr_t pMac;
 xge_hal_device_macaddr_get(XMP_NIC_GET_DEV(pNicCtx), 0, &pMac);
 NdisMoveMemory(pAddr, pMac, ETH_LENGTH_OF_ADDRESS);
}

NDIS_STATUS
xmpHalGetLinkStatus(
  IN  xmpNicCtx_t            *pNicCtx
  )
{
  xge_hal_device_link_state_e   linkStatus;
  xge_hal_device_link_state (XMP_NIC_GET_DEV(pNicCtx), &linkStatus);
  return( linkStatus == XGE_HAL_LINK_UP ? NDIS_STATUS_SUCCESS : NDIS_STATUS_FAILURE );
}

VOID
xmpHalLinkUpCB(
  IN  void	                 *userdata
  )
{
  xmpNicCtx_t                *pNicCtx = (xFrameNic_t *)userdata;
  XMPTRACE(XMP_DBG_INFO, ("xmpHalLinkUpCB: Link is up \n"));

  if ( NDIS_CURRENT_IRQL() > DISPATCH_LEVEL )
  {
    pNicCtx->IntrLink = TRUE;
    return;
  }
  xmpNicIndicateStatus(pNicCtx,  TRUE);
}

VOID
xmpHalLinkDownCB(
  IN  void	                 *userdata
  )
{
  xmpNicCtx_t                *pNicCtx = (xFrameNic_t *)userdata;
  XMPTRACE(XMP_DBG_INFO, ("xmpHalLinkDownCB: Link is down \n"));

  if ( NDIS_CURRENT_IRQL() > DISPATCH_LEVEL )
  {
    pNicCtx->IntrLink = TRUE;
    return;
  }
  xmpNicIndicateStatus(pNicCtx,  FALSE);
}

VOID
xmpHalCritErrCB(
  IN  void                   *userdata,
	IN  xge_hal_event_e	       type,
	IN  u64			               serr_data
  )
{
  xmpNicCtx_t                *pNicCtx = (xFrameNic_t *)userdata;
  XMPTRACE(XMP_DBG_WRN, ("xmpHalCritErrCB: !!!Critical Errors!!! \n"));
  pNicCtx->NeedsNicReset = TRUE;
}

VOID
xmpHalEventCB(
  IN  xge_queue_item_t	     *item
  )
{
  XMPTRACE(XMP_DBG_WRN, ("xmpHalEventCB: event id=%d critical=%s\n",
                         item->event_type, item->is_critical ? "YES" : "No"));
}

NDIS_STATUS
xmpHalPostRxD(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  xmpNicRxInfo_t         *pRxRing,
  IN  xmpRxNBLCtx_t          *pNBLCtx
  )
{
  xmpHalRxDtrCtx_t           *pHalRxCtx;
  xmpNicRxHdrMDL_t           *mdlInfo;
	xge_hal_dtr_h               dtr;
  dma_addr_t                  bufs[3];
  int                         bufsz[3];
  NDIS_STATUS                 Status;
  xge_hal_status_e            halerr;

  do
  {
    if ( (halerr = xge_hal_ring_dtr_reserve(pRxRing->halChannel, &dtr)) !=
          XGE_HAL_OK)
    {
      XMPTRACE(XMP_DBG_ERR, ("xmpHalPostRxs:xge_hal_ring_dtr_reserve "
                             "failed err=0x%x", halerr));
      ASSERT( 0 );
      Status = NDIS_STATUS_FAILURE;
      break;
    }

    pNBLCtx->state = XMP_RXNBL_STATE_POSTED;
    pHalRxCtx = xge_hal_ring_dtr_private(pRxRing->halChannel, dtr);
    pHalRxCtx->pRxCtx = pNBLCtx;

    if ( XMP_NIC_RX_SPLIT(pNicCtx) )
    {
      /* One of the following cases:
       * - SCATTER_MODE_A, i.e., 3-buffer mode. 1st buffer receives ETH Header,
       *   2nd one for IP+TCP Header, and 3rd one for payload.
       * - SCATTER_MODE_B, i.e., 2-buffer mode. 1st buffer receives ETH Header,
       *   2nd one left empty, and 3rd one for reset of the frame.
       * In either case 1st buffer comes from header buffers array, 2nd
       * buffer is set at the offset of XENAMP_RXBUF_ETH_HDR_BACKFILL_SZ, and
       * third buffer is the receive buffer at backfill offset. Backfill space
       * in the receive buffer is good enough depending upon the buffer mode
       * used.
       */
      mdlInfo = &pRxRing->hdrMDLsArr[pRxRing->hdrMDLPos];
      pNBLCtx->idxHdrMDL = pRxRing->hdrMDLPos;
      pRxRing->hdrMDLPos = (pRxRing->hdrMDLPos + 1) % pRxRing->RxDCnt;

      bufs[0]  = mdlInfo->PhyAddr;
      bufsz[0] = XENAMP_RXBUF_ETH_HDR_BACKFILL_SZ;
      bufs[1]  = mdlInfo->PhyAddr + XENAMP_RXBUF_ETH_HDR_BACKFILL_SZ;
      bufsz[1] = pNicCtx->RxHeaderSz - XENAMP_RXBUF_ETH_HDR_BACKFILL_SZ;

      /* Buffer-2 has a backfill provision.                                  */
      bufs[2]  = pNBLCtx->RxPa;                /* already at backfill offset */
      bufsz[2] = pNicCtx->rxBufSz;         /* does not include backfill size */
      xge_hal_ring_dtr_3b_set(dtr, bufs, bufsz);
    }
    else
    {
      xge_hal_ring_dtr_1b_set(dtr, pNBLCtx->RxPa, pNicCtx->rxBufSz);
    }
    xge_hal_ring_dtr_post(pRxRing->halChannel, dtr);
    Status = NDIS_STATUS_SUCCESS;
  } while(0);

  return Status;
}

NDIS_STATUS
xmpHalPostRxDs(
  IN  xmpNicCtx_t            *pNicCtx
  )
{
  xmpQLink_t                 *pRxNBLQ;
  xmpQLink_t                 *link;
  xmpRxNBLCtx_t              *pNBLCtx;
  NDIS_STATUS                 Status = NDIS_STATUS_SUCCESS;
  UINT                        ui;
  UINT                        ring;

  XMPTRACE(XMP_DBG_INI, (">> xmpHalPostRxDs: pNicCtx=%p\n", pNicCtx));

  for ( ring = 0; ring < pNicCtx->RegParams.RxRingNum; ring++ )
  {
    pRxNBLQ = link = &pNicCtx->RxInfo[ring].RxNBLQ;
    for ( ui=0; ui < pNicCtx->RxInfo[ring].RxDCnt; ui++ )
    {
      link = XF_Q_NEXT(pRxNBLQ, link);
      ASSERT(link != NULL);
      pNBLCtx = CONTAINING_RECORD(link, xmpRxNBLCtx_t, NBLQLink);
      if ( (pNBLCtx->state == XMP_RXNBL_STATE_FREE) &&
           (Status = xmpHalPostRxD(pNicCtx, &pNicCtx->RxInfo[ring], pNBLCtx))
           != NDIS_STATUS_SUCCESS )
        break;
    }

    XMPTRACE(XMP_DBG_INI, ("xmpHalPostRxDs: Posted %d RxDs on %d ring\n",
                           ui, ring));
    if ( Status != NDIS_STATUS_SUCCESS )
      break;
  }

  XMPTRACE(XMP_DBG_INI, ("<< xmpHalPostRxDs: Status=%lx\n", Status));
  return Status;
}

BOOLEAN
xmpHalRxLRO(
  xmpNicCtx_t                *pNicCtx,
  xmpRxNBLCtx_t              *pNBLCtx,
  xge_hal_dtr_info_t         *rxInfo,
  u32                         rxLen,                         /* total rx len */
  u32                         rxLen2,          /* buf2 length (for split rx) */
  u32                         rxLen3,          /* buf3 length (for split rx) */
  u32                         ringNum
  )
{
  BOOLEAN                     queue_it = TRUE;

#ifdef XGE_HAL_CONFIG_LRO

  tcplro_t                   *tcp;
  lro_t                      *lro;
  lro_t                      *lro_end3;
  NET_BUFFER                 *pNetBuf;
  MDL                        *pMDL = (MDL *)0;
  u8                         *ip = (u8 *)0;
  u32                         len;
  xge_hal_status_e            status;
  xmpNicRxInfo_t             *pRing = &pNicCtx->RxInfo[ringNum];

#if 0

  do
  {
    pNetBuf           = NET_BUFFER_LIST_FIRST_NB(pNBLCtx->pNBL);
    pMDL              = NET_BUFFER_FIRST_MDL(pNetBuf);
    pNBLCtx->lroNext  = NULL;
    pNBLCtx->pLastMDL = NULL;
    len               = rxLen;

    if ( !rxLen2 )
      break;
    ip = pNBLCtx->pBuf2;
    ASSERT(ip);
    
    status = xge_hal_lro_process_rx(ringNum,
                                    pNBLCtx->pBuf,
                                    ip,
                                    &tcp,
                                    &len,
                                    &lro,
                                    rxInfo,
                                    pNicCtx->pHalDev,
                                    &lro_end3);
    switch( status )
    {
    case XGE_HAL_INF_LRO_BEGIN:
      /* begining of a new LRO session                                       */
        lro->os_buf = lro->os_buf_end = pNBLCtx;
        pNBLCtx->pLastMDL = ip ? pNBLCtx->pMDL3 : pMDL;
        pRing->Stats.LroSesBegin++;
        break;

    case XGE_HAL_INF_LRO_CONT:
       pRing->Stats.LroSesCont++;
      /* continuation of LRO session. Put this one at end of the LRO list.   */
    case XGE_HAL_INF_LRO_END_1:
      /* End of LRO session with the current segment part of it              */
      lro->os_buf_end->lroNext = pNBLCtx;
      lro->os_buf_end          = pNBLCtx;

      /* exclude headers */
      if ( ip )
      {
        pMDL = pNBLCtx->pMDL3;
      }
      else
      { /* Adjust MDL for this segment i.e., advance it to exclude headers.  */
        NdisAdjustMdlLength(pMDL, rxLen);
        NET_BUFFER_DATA_LENGTH(pNetBuf) = rxLen;
        if ( pNBLCtx->advance = (rxLen - len) )
          NdisAdvanceNetBufferDataStart(pNetBuf, pNBLCtx->advance, FALSE,
                                        NULL);
      }

      /* chain this segment MDL to the LRO list of MDLs.                     */
      NDIS_MDL_LINKAGE(lro->os_buf->pLastMDL) = pMDL;
      lro->os_buf->pLastMDL = pMDL;
      /* Account for this segment in the 1st NET_BUFFER.                     */
      pNetBuf = NET_BUFFER_LIST_FIRST_NB(lro->os_buf->pNBL);
      NET_BUFFER_DATA_LENGTH(pNetBuf) += len;
      if ( status == XGE_HAL_INF_LRO_END_1 )
      {
        pRing->Stats.LroSesEnd1++;
        xge_hal_lro_close_session(lro);
      }
      queue_it = FALSE;
      break;

    case XGE_HAL_INF_LRO_END_2:
      /* End of LRO session; current segment is not part of it               */
      pRing->Stats.LroSesEnd2++;
      xge_hal_lro_close_session(lro);
      break;

    case XGE_HAL_INF_LRO_END_3:
      /* End of LRO session. current segment begins a new LRO session        */
      pRing->Stats.LroSesEnd3++;
      lro_end3->os_buf = lro_end3->os_buf_end = pNBLCtx;      /* new session */
      pNBLCtx->pLastMDL = ip ? pNBLCtx->pMDL3 : pMDL;
      xge_hal_lro_close_session(lro);
      break;

    case XGE_HAL_INF_LRO_UNCAPABLE:
    case XGE_HAL_INF_LRO_SESSIONS_XCDED:
      break;

    default:
      ASSERT(0);                                            /* not possible! */
      break;
    }
  } while( 0 );

#endif /* #if 0 */

#endif /* XGE_HAL_CONFIG_LRO */

  return queue_it;
}

xge_hal_status_e
xmpHalRxFrameCB(
  IN  xge_hal_channel_h       channelh,
  IN  xge_hal_dtr_h           dtr,
  IN  u8                      rxStatus,
  IN  void                   *cbCtx
  )
{
  xmpNicCtx_t                *pNicCtx = (xmpNicCtx_t *) cbCtx;
  xmpRxNBLCtx_t              *pNBLCtx;
  xmpNicRxInfo_t             *pRxRing;
  xge_hal_ring_rxd_1_t       *rxdp;
  xmpNicRxHdrMDL_t           *pHdrMdlInfo;
  NET_BUFFER                 *pNetbuf;
  MDL                        *pMDL = (MDL *)0;
  UCHAR                      *ethHdr = (UCHAR *)0;
  UCHAR                      *origHdr;
  NDIS_TCP_IP_CHECKSUM_PACKET_INFO *pNdisCkoInfo;
  NDIS_NET_BUFFER_LIST_8021Q_INFO  *pNdis8021qInfo;
  ULONG                       unusedData;
  ULONG                       pktLen;
  ULONG                       hdrSz;
  int                         blens[3];
  INT                         ring;
  INT                         proc = NDIS_CURRENT_PROCESSOR_NUMBER;
  u8                          proto;
  U64                         phyAddr;
  dma_addr_t                  addrs[3];
  xge_hal_dtr_info_t          rxInfo;
  int                         kt_length=0;

  ring = xge_hal_channel_id(channelh);
  pRxRing = &pNicCtx->RxInfo[ring];
  hdrSz=0; //initialization
  XMPTRACE(XMP_DBG_RX, (">> xmpHalRxFrameCB: pNicCtx=%p Ring=%d\n",
                        pNicCtx, ring));
  do
  {
    pNBLCtx =
      ((xmpHalRxDtrCtx_t *)xge_hal_ring_dtr_private(channelh,dtr))->pRxCtx;

    if ( !pNBLCtx->pNBL )                      /* just to keep prefast quiet */
      break;
    /* Get the rx frame info and setup NBL and NBF accordingly.              */
    if ( XMP_NIC_RX_SPLIT(pNicCtx) )
    {
      xge_hal_ring_dtr_3b_get(channelh, dtr, addrs, blens);
      xge_hal_ring_dtr_info_nb_get(channelh, dtr, &rxInfo);
      hdrSz  = blens[0] + blens[1];                 /* total size of headers */
      pktLen = hdrSz + blens[2];                        /* total packet size */
    }
    else
    {
      xge_hal_ring_dtr_1b_get(channelh, dtr, &phyAddr, &kt_length);
      pktLen = (ULONG)kt_length;
      xge_hal_ring_dtr_info_get(channelh, dtr, &rxInfo);
      blens[0] = pktLen;
      blens[1] = blens[2] = 0;
    }

    rxdp = (xge_hal_ring_rxd_1_t *)dtr;

    XMPTRACE(XMP_DBG_RX, ("xmpHalRxFrameCB: pNicCtx=%p Ring=%d RxNBL Len=%d\n",
                          pNicCtx, ring, pktLen));

#ifdef NEVER
    {
      DbgPrint("rxdp:%I64x", rxdp);
		  DbgPrint("\n control_1:%I64x, control_2:%I64x, b0_ptr:%I64x, host_ctrl:%I64x",rxdp->control_1,rxdp->control_2, rxdp->buffer0_ptr, rxdp->host_control);
      DbgPrint("\n t_code:%llx", (u8)XGE_HAL_RXD_GET_T_CODE(rxdp->control_1));
      DbgPrint("\n it_hit:%x  spdm_hit:%x  rth_hash_type:%x rth_value:%x",
         rxInfo.rth_it_hit, rxInfo.rth_spdm_hit, rxInfo.rth_hash_type,
         rxInfo.rth_value);
      DbgPrint("\n l3_cksum:%x, l4_cksum:%x, vlantag:%x, frametype:%x ", 
               rxInfo.l3_cksum, rxInfo.l4_cksum, rxInfo.vlan, rxInfo.frame);
      DbgPrint("\n proto_ipv4:%x, proto_ipv6:%x, proto_ip_frag:%x,proto_tcp:%x, proto_udp:%x",(u8) (rxdp->control_1 & XGE_HAL_RXD_FRAME_PROTO_IPV4),
         (u8) (rxdp->control_1 & XGE_HAL_RXD_FRAME_PROTO_IPV6), 
         (u8) (rxdp->control_1 & XGE_HAL_RXD_FRAME_PROTO_IP_FRAGMENTED),
         (u8) (rxdp->control_1 & XGE_HAL_RXD_FRAME_PROTO_TCP),
         (u8) (rxdp->control_1 & XGE_HAL_RXD_FRAME_PROTO_UDP));
    }
    DbgPrint("\n it_hit:%x  spdm_hit:%x  rth_hash_type:%x rth_value:%x",
         rxInfo.rth_it_hit, rxInfo.rth_spdm_hit, rxInfo.rth_hash_type,
         rxInfo.rth_value);
#endif
    ASSERT( pNBLCtx->state == XMP_RXNBL_STATE_POSTED );
    pNetbuf = NET_BUFFER_LIST_FIRST_NB(pNBLCtx->pNBL);
    pNBLCtx->pNBL->NblFlags = 0;

    if ( XMP_NIC_RX_SPLIT(pNicCtx) )
    {
      pHdrMdlInfo = &pRxRing->hdrMDLsArr[pNBLCtx->idxHdrMDL];
      origHdr     = pHdrMdlInfo->pHdrBuf;/* virtual address of header buffer */
      pMDL        = pNBLCtx->pBfillMDL;

      if ( !blens[1] || XMP_NIC_RX_MS_SPLIT_COMBINE(pNicCtx) || !blens[2] )
      { /* single MDL case.                                                  */
        /* IMP: in case of 3-buffer mode i.e., MS combine, 3rd buffer could
         * be zero (when there is no TCP/UDP payload). In that case we could
         * use the header MDL itself. However, we won't be able to reclaim
         * the header buffer right away after indicate upcall returns. That is
         * why it is better to copy Eth+IP+TCP header even in that case from
         * the header buffer to the receive buffer.
         */
        unusedData = pNicCtx->RxBackfillSz - hdrSz;
        if ( blens[1] )
        { /* Must be MS combine.                                             */
          NdisMoveMemory(pNBLCtx->pRxBuf + (pNicCtx->RxBackfillSz - blens[1]),
                         origHdr + XENAMP_RXBUF_ETH_HDR_BACKFILL_SZ,
                         blens[1]);
        }
        ethHdr = pNBLCtx->pRxBuf + unusedData;
        NdisMoveMemory(ethHdr, origHdr, blens[0]);
        NdisAdjustMdlLength(pMDL, pNicCtx->RxBackfillSz + blens[2]);
      }
      else
      {
        /*
         * Ist buffer pointer receives ethernet header, then there is a gap
         * followed by 2nd buffer pointer that starts at an offset of
         * XENAMP_RXBUF_ETH_HDR_BACKFILL_SZ of the header buffer that receives
         * IP+TCP header. We need to move Ethernet header just ahead of IP
         * header as follows-
         *
         *       ------------------              ------------------
         *      |  Ethernet header |            |      Unused      |
         *       ------------------              ------------------
         *      |      Unused      |            |  Ethernet Header |
         *       -----------------               ------------------
         *      |                  |            |                  |
         *      |   IP+TCP Header  |            |   IP+TCP Header  |
         *      |                  |            |                  |
         *       -----------------               ------------------
         */
        unusedData = XENAMP_RXBUF_ETH_HDR_BACKFILL_SZ - blens[0];
        ethHdr  = origHdr + unusedData;
        NdisMoveMemory(ethHdr, origHdr, blens[0]);

        pMDL   = pHdrMdlInfo->pMDL;            /* 1st MDL containing headers */
        NdisAdjustMdlLength(pMDL, unusedData + hdrSz);
        NdisAdjustMdlLength(pNBLCtx->pDataMDL, blens[2]);/* 2nd MDL length */
        NDIS_MDL_LINKAGE(pNBLCtx->pDataMDL) = NULL;
        NDIS_MDL_LINKAGE(pMDL) = pNBLCtx->pDataMDL;
        bis(pNBLCtx->pNBL->NblFlags, NDIS_NBL_FLAGS_HD_SPLIT);
        bis(pNBLCtx->pNBL->NblFlags,
            NDIS_NBL_FLAGS_SPLIT_AT_UPPER_LAYER_PROTOCOL_PAYLOAD);
        pNetbuf->DataPhysicalAddress.QuadPart = pNBLCtx->RxPa;
      }
      /* Setup MDL chain in NET_BUFFER, and adjust NET_BUFFER descriptor.    */
      NET_BUFFER_FIRST_MDL(pNetbuf)          = pMDL;
      NET_BUFFER_CURRENT_MDL(pNetbuf)        = pMDL;
      NET_BUFFER_DATA_LENGTH(pNetbuf)        = pktLen;
      NET_BUFFER_DATA_OFFSET(pNetbuf)        = unusedData;
      NET_BUFFER_CURRENT_MDL_OFFSET(pNetbuf) = unusedData;
    }                                           /* XMP_NIC_RX_SPLIT(pNicCtx) */
    else
    { /* !XMP_NIC_RX_SPLIT(pNicCtx).                                         */
      ethHdr = pNBLCtx->pRxBuf;
      pMDL = pNBLCtx->pDataMDL;
      NdisAdjustMdlLength(pMDL, pktLen);
      NET_BUFFER_DATA_LENGTH(pNetbuf) = pktLen;
    }
    /*NdisFlushBuffer(pNBLCtx->pMDL, FALSE);*/

    pNdisCkoInfo =
      ((NDIS_TCP_IP_CHECKSUM_PACKET_INFO *)
       &NET_BUFFER_LIST_INFO(pNBLCtx->pNBL, TcpIpChecksumNetBufferListInfo));
    pNdisCkoInfo->Value = 0;

    proto = XGE_HAL_RXD_GET_FRAME_PROTO(rxdp->control_1);
    if ( bit(proto, XGE_HAL_FRAME_PROTO_IPV4) )
    {
      bis(pNBLCtx->pNBL->NblFlags, NDIS_NBL_FLAGS_IS_IPV4);
      if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_IP4_RX) )
      {
        if ( rxInfo.l3_cksum == XGE_HAL_L3_CKSUM_OK )
          pNdisCkoInfo->Receive.NdisPacketIpChecksumSucceeded = 1;
        else
          pNdisCkoInfo->Receive.NdisPacketIpChecksumFailed = 1;
      }
    
      if ( (rxdp->control_1 & XGE_HAL_RXD_FRAME_PROTO_IP_FRAGMENTED) != 
          XGE_HAL_RXD_FRAME_PROTO_IP_FRAGMENTED)  
      {
       if ( bit(proto, XGE_HAL_FRAME_PROTO_TCP) )
       {
         bis(pNBLCtx->pNBL->NblFlags, NDIS_NBL_FLAGS_IS_TCP);
         if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP4_RX) )
         {
           if ( rxInfo.l4_cksum == XGE_HAL_L4_CKSUM_OK )
             pNdisCkoInfo->Receive.NdisPacketTcpChecksumSucceeded = 1;
           else
             pNdisCkoInfo->Receive.NdisPacketTcpChecksumFailed = 1;
         }
       }
       else if ( bit(proto, XGE_HAL_FRAME_PROTO_UDP) )
       {
         bis(pNBLCtx->pNBL->NblFlags, NDIS_NBL_FLAGS_IS_UDP);
        if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP4_RX) )
        {
          if ( rxInfo.l4_cksum == XGE_HAL_L4_CKSUM_OK )
            pNdisCkoInfo->Receive.NdisPacketUdpChecksumSucceeded = 1;
          else
            pNdisCkoInfo->Receive.NdisPacketUdpChecksumFailed = 1;
        }
       }
      }  
    }
    else if ( bit(proto, XGE_HAL_FRAME_PROTO_IPV6) )
    {
      bis(pNBLCtx->pNBL->NblFlags, NDIS_NBL_FLAGS_IS_IPV6);
      if ( bit(proto, XGE_HAL_FRAME_PROTO_TCP) )
      {
        bis(pNBLCtx->pNBL->NblFlags, NDIS_NBL_FLAGS_IS_TCP);
        if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_TCP6_RX) )
        {
          if ( rxInfo.l4_cksum == XGE_HAL_L4_CKSUM_OK )
            pNdisCkoInfo->Receive.NdisPacketTcpChecksumSucceeded = 1;
          else
            pNdisCkoInfo->Receive.NdisPacketTcpChecksumFailed = 1;
        }
      }
      else if ( bit(proto, XGE_HAL_FRAME_PROTO_UDP) )
      {
        bis(pNBLCtx->pNBL->NblFlags, NDIS_NBL_FLAGS_IS_UDP);
        if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_CKO_UDP6_RX) )
        {
          if ( rxInfo.l4_cksum == XGE_HAL_L4_CKSUM_OK )
            pNdisCkoInfo->Receive.NdisPacketUdpChecksumSucceeded = 1;
          else
            pNdisCkoInfo->Receive.NdisPacketUdpChecksumFailed = 1;
        }
      }
    }

    if( XMP_NIC_RSS_IS_ENABLED(pNicCtx) )
    {
      ULONG hashtype = 0;
      if ( rxInfo.rth_it_hit )
      {
        XMP_HAL_MAP_HASH_TYPE(rxInfo.rth_hash_type, hashtype);
        if (hashtype)
        {
          NET_BUFFER_LIST_SET_HASH_FUNCTION(pNBLCtx->pNBL,
                                            NdisHashFunctionToeplitz);
        }
        pRxRing->Stats.RxItHit++;
      }
      else
      {
        rxInfo.rth_value = 0;
        hashtype = 0;
        NET_BUFFER_LIST_SET_HASH_FUNCTION(pNBLCtx->pNBL, 0);
        pRxRing->Stats.RxItMiss++;
      }

      NET_BUFFER_LIST_SET_HASH_VALUE(pNBLCtx->pNBL, rxInfo.rth_value);
      NET_BUFFER_LIST_SET_HASH_TYPE(pNBLCtx->pNBL, hashtype);
    }

    if ( bit(proto, XGE_HAL_FRAME_PROTO_VLAN_TAGGED) )
      /* && XMP_NIC_8021PQ_IS_ENABLED(pNicCtx) )*/
    {
      pNdis8021qInfo = ((NDIS_NET_BUFFER_LIST_8021Q_INFO *)
                        &NET_BUFFER_LIST_INFO(pNBLCtx->pNBL,
                                              Ieee8021QNetBufferListInfo));
      XMP_SET_NBL_8021Q_INFO(&rxInfo.vlan, pNdis8021qInfo);
      XMPTRACE(XMP_DBG_RX, ("RX 8021Q info: vlan=%x Priority=%x CanonId=%x\n",
                            pNdis8021qInfo->TagHeader.VlanId,
                            pNdis8021qInfo->TagHeader.UserPriority,
                            pNdis8021qInfo->TagHeader.CanonicalFormatId));
    }

    XF_DPR_GET_SLOCK(&pRxRing->RxLock);
      pNBLCtx->state = XMP_RXNBL_STATE_ULP;
      xge_hal_ring_dtr_free(channelh, dtr);
    XF_DPR_FREE_SLOCK(&pRxRing->RxLock);

#ifdef XMP_STATS_OID
    /* In case of split-combining, what if the copy may overwrites the
     * begining???                                                           
     * I'm not going to mutex protect these stats.                           */
    if ( !ETH_IS_MULTICAST(ethHdr) )
    { /* directed */
      pNicCtx->OidStats.rxDirFrames++;
      pNicCtx->OidStats.rxDirBytes += pktLen;
    }
    else if ( ETH_IS_BROADCAST(ethHdr) )
    {
      pNicCtx->OidStats.rxBcastFrames++;
      pNicCtx->OidStats.rxBcastBytes += pktLen;
    }
    else
    {
      pNicCtx->OidStats.rxMcastFrames++;
      pNicCtx->OidStats.rxMcastBytes += pktLen;
    }

    if ( rxStatus != XGE_HAL_RXD_T_CODE_OK )
    {
      pNicCtx->OidStats.rxErrFrames++;
      xge_hal_device_handle_tcode(channelh, dtr, rxStatus);
    }
#endif

    NET_BUFFER_LIST_STATUS(pNBLCtx->pNBL) =
      (rxStatus == XGE_HAL_RXD_T_CODE_OK) ?
      NDIS_STATUS_SUCCESS : NDIS_STATUS_FAILURE;

    if ( pNicCtx->bInDiagMode )
    {
      xmpDiagRecvComplete(pNicCtx, pNBLCtx, pktLen, rxStatus, ring);
    }
    else if ( !XMP_NIC_LRO_CONFIG(pNicCtx)                                   ||
              (rxInfo.proto & XGE_HAL_FRAME_PROTO_IP_FRAGMENTED)             ||
              !(rxInfo.proto & XGE_HAL_FRAME_PROTO_TCP_OR_UDP)               ||
              (rxInfo.l3_cksum != XGE_HAL_L3_CKSUM_OK)                       ||
              (rxInfo.l4_cksum != XGE_HAL_L4_CKSUM_OK)                       ||
              (NET_BUFFER_LIST_STATUS(pNBLCtx->pNBL) != NDIS_STATUS_SUCCESS) ||
              xmpHalRxLRO(pNicCtx, pNBLCtx, &rxInfo, pktLen, blens[1],
                          blens[2], ring) )
    { /* NBL is ready for upcall. If we have pending NBLs waiting for
       * indication then queue this one at the tail.
       * TODO: need to mutex the pPrvNBL,pNBL ???
       */
      XMP_RX_NBL_LINK(pRxRing, pNBLCtx);
      if ( !bit(pNicCtx->Flags, XMP_NIC_FLAGS_RX_NBL_CHAIN) )
        xmpNicRxIndicate(pNicCtx, ring);
    }

#ifdef NEVER
  DbgPrint("\nRxCnt = %d, RingNum = %d\n", pRxRing->Stats.RxCnt, ring);
#endif
  } while (xge_hal_ring_dtr_next_completed(channelh, &dtr, &rxStatus)
           == XGE_HAL_OK);

#ifdef XGE_HAL_CONFIG_LRO
  if ( XMP_NIC_LRO_CONFIG(pNicCtx) )
  {
		lro_t *lro= xge_hal_lro_next_session(pNicCtx->pHalDev, ring);
		while( lro != 0 )
    {
			xge_hal_lro_close_session(lro);
            lro = xge_hal_lro_next_session(pNicCtx->pHalDev, ring);
    }
  }
#endif /* XGE_HAL_CONFIG_LRO */

  xmpNicRxIndicate(pNicCtx, ring);
  XMPTRACE(XMP_DBG_RX, ("<< xmpHalRxFrameCB: pNicCtx=%p: Ring=%d\n",
                        pNicCtx, ring));
  return XGE_HAL_OK;
}

xge_hal_status_e
xmpHalContinueRSSIrq(
  IN  NDIS_HANDLE             MiniportInterruptContext,
  IN  PVOID                   MiniportDpcContext,
	IN  ULONG                   proc
	)
{
  xmpNicCtx_t                *pNicCtx;
  xge_hal_device_t           *pHalDev;
  xmpNicRxInfo_t             *pRing;
  xmpNicTxFifo_t             *pFifo;
  xge_hal_dtr_h               dtr = NULL;
  u8                          tcode;
  ULONG						  tproc;
  ULONG						  ringno;
  ULONG						  TargetProcessors=0;
  xge_list_t				 *item;
  xge_hal_channel_t		     *channel;

  pNicCtx = (xmpNicCtx_t *) MiniportInterruptContext;
  pHalDev = XMP_NIC_GET_DEV(pNicCtx);

  do
  {
    ringno = proc - pNicCtx->devCtx->RssBaseCpu; 
    if ( ringno < pNicCtx->RegParams.RxRingNum )
    {
      pRing = &pNicCtx->RxInfo[ringno];
      pRing->Stats.RxDpcCnt++;
      ((xge_hal_ring_t*)pRing->halChannel)->cmpl_cnt = 0;
      if ( xge_hal_ring_dtr_next_completed(pRing->halChannel, &dtr, &tcode)     
           == XGE_HAL_OK )
        xmpHalRxFrameCB(pRing->halChannel, dtr, tcode, pNicCtx);
    }
    else
    {
      ASSERT(FALSE);
    }
 
    if( pNicCtx->devCtx->RssProcessors[0] != proc )
      break;

		xge_list_for_each(item, &pHalDev->fifo_channels) 
    {
			channel = xge_container_of(item, xge_hal_channel_t, item);

			if (xge_hal_fifo_dtr_next_completed (channel, &dtr, &tcode) == XGE_HAL_OK) 
        channel->callback(channel, dtr, tcode, channel->userdata);
		}
  } while(0);

  return XGE_HAL_OK;
}

xge_hal_status_e
xmphalIsRxQueueReady(
  IN  xge_hal_channel_h channelh
  )
{
 xge_hal_dtr_h dtrh;
 xge_hal_ring_t *ring = (xge_hal_ring_t *)channelh;
 xge_hal_ring_rxd_1_t *rxdp; /* doesn't matter 1, 3 or 5... */
 __hal_channel_dtr_try_complete(ring, &dtrh);
 if (dtrh == NULL) {
  return XGE_HAL_INF_NO_MORE_COMPLETED_DESCRIPTORS;
 }
 rxdp = (xge_hal_ring_rxd_1_t *)dtrh;
 /* check whether it is not the end */
 if (!(rxdp->control_2 & XGE_HAL_RXD_NOT_COMPLETED) &&
  !(rxdp->control_1 & XGE_HAL_RXD_POSTED_4_XFRAME)) {
  return XGE_HAL_OK;
 }
 return XGE_HAL_INF_NO_MORE_COMPLETED_DESCRIPTORS;
}

/* 
 * xmpHalTxComplete
 *
 * Description:
 *   Completion handler for Transmit Net Buffer requests. It is called by
 * hal interrupt DPC. *
 * Arguments:
 *
 * Return Value:
 *   none
 *   
 * Callers:
 *   Normally by HAL in interrupt DPC when it processes Tx completion
 *   interrupts.
 *
 * IRQL:
 *   DISPATCH Level
 *
 * Notes:
 *
 */ 
xge_hal_status_e
xmpHalTxCompleteCB(
  IN  xge_hal_channel_h      channelh,
  IN  xge_hal_dtr_h          dtr,
  IN  u8                     txStatus,
  IN  void                   *cbCtx

  )
{
  xFrameNic_t                *pNic = (xFrameNic_t *) cbCtx;
  xmpNicCtx_t                *pNicCtx;
  xmpNicTxFifo_t             *pFifo;
  xmpTxNBufCtx_t             *pNBufCtx;
  NDIS_STATUS                 Status;
  ULONG                       LsoBytes=0;
  ULONG                       count = 0;
  xge_hal_device_t           *pHalDev;
  xge_hal_dtr_h              *dtrFree;

  pNicCtx = XMP_NIC_CTX_PTR(pNic); 
  pHalDev = XMP_NIC_GET_DEV(pNicCtx);

  ASSERT( NDIS_CURRENT_IRQL() == DISPATCH_LEVEL );

  pNBufCtx = (xmpTxNBufCtx_t *)xge_hal_fifo_dtr_private(dtr);
  pFifo = pNBufCtx->pFifo;
#ifdef XENAMP_TXD_FREE_COALESCE
  dtrFree = pFifo->dtrFree;
#endif /*  XENAMP_TXD_FREE_COALESCE */

  do
  {
    pNBufCtx = (xmpTxNBufCtx_t *)xge_hal_fifo_dtr_private(dtr);    
    if ( pNBufCtx->pNBLCtx && pNBufCtx->pNBLCtx->Mss ) 
    { 
      xge_hal_fifo_txd_t *txdp = (xge_hal_fifo_txd_t *)dtr;
      /* TODO Encapsulation header checking needs to be incorporated */
     if (xge_hal_device_check_id(pHalDev) == XGE_HAL_CARD_XENA)
     {
       LsoBytes = NET_BUFFER_DATA_LENGTH(pNBufCtx->pNetBuf) - 54;
     }
     else
     {
       LsoBytes = (ULONG)XGE_HAL_TXD_GET_LSO_BYTES_SENT(txdp->control_1);
     }
    }

#ifdef  DBG
    if ( txStatus != XGE_HAL_OK )
    {
      XMPTRACE(XMP_DBG_WRN, ("xmpHalTxCompleteCB: TxFailure=%d\n", txStatus));
    }
#endif
    Status = (txStatus == XGE_HAL_OK ? NDIS_STATUS_SUCCESS :
                                        NDIS_STATUS_FAILURE);

    ASSERT( NDIS_CURRENT_IRQL() == DISPATCH_LEVEL );
    if ( !bit(pNBufCtx->Flags, XMP_TX_NBUF_FLAG_DIAG) )
      xmpHalTxCompleteNetBuf(pNicCtx, pNBufCtx, LsoBytes, Status, TRUE, FALSE);
    else
      xmpDiagSendComplete(pNicCtx, pNBufCtx, txStatus);

#ifdef XENAMP_TXD_FREE_COALESCE
    *dtrFree = dtr;
    dtrFree++;
#else /* !XENAMP_TXD_FREE_COALESCE */
    XF_DPR_GET_SLOCK(&pFifo->HalTxLock);
      xge_hal_fifo_dtr_free(channelh, dtr);
    XF_DPR_FREE_SLOCK(&pFifo->HalTxLock);
#endif /* XENAMP_TXD_FREE_COALESCE */
    count++;
    pFifo->Stats.txdFree++;
  } while (count < pNicCtx->RegParams.indicate_max_pkts &&
           xge_hal_fifo_dtr_next_completed(channelh, &dtr, &txStatus)
           == XGE_HAL_OK);

#ifdef XENAMP_TXD_FREE_COALESCE
  XF_DPR_GET_SLOCK(&pFifo->HalTxLock);
    dtrFree = pFifo->dtrFree;
    while(count--)
    {
      xge_hal_fifo_dtr_free(channelh, *dtrFree);
      dtrFree++;
    }
  XF_DPR_FREE_SLOCK(&pFifo->HalTxLock);
#endif /* XENAMP_TXD_FREE_COALESCE */

  if ( pFifo->OsSendCnt && !pFifo->InService )
  { /* There are pending sends in the wait queue. Queue a WI to flush the Q. */
    XF_DPR_GET_SLOCK(&pFifo->TxLock);
      if ( !(pFifo->InService || XF_Q_IS_NULL(&pFifo->NBLWaitQ)) &&
           !pFifo->TxWIQueued )
      {
        pFifo->TxWIQueued = TRUE;
        NdisQueueIoWorkItem(pFifo->hTxFlushWI,
                            (NDIS_IO_WORKITEM_ROUTINE)xmpNicTxFlushCB,
                            pFifo);
      }
    XF_DPR_FREE_SLOCK(&pFifo->TxLock);
  }

  if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_TX_NBL_CHAIN) )
    XMP_TX_COMPLETE_FINAL(pNicCtx, pFifo, TRUE);

  return XGE_HAL_OK;
}

VOID
xmpHalTxCompleteNetBuf(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  xmpTxNBufCtx_t         *pNBufCtx,
  IN  ULONG                   LsoBytes,
  IN  NDIS_STATUS             Status,
  IN  BOOLEAN                 isDpcLevel,
  IN  BOOLEAN                 FlushIt
  )
{
  xmpTxNBLCtx_t              *pNBLCtx = pNBufCtx->pNBLCtx;
  xmpNicTxFifo_t             *pFifo   = pNBufCtx->pFifo;
  KIRQL                       OldIrql;
  OldIrql = (KIRQL)0;   //initialization

  XMPTRACE(XMP_DBG_TX, ("==> xmpHalTxCompleteNB\n"));

  if ( !isDpcLevel )
    NDIS_RAISE_IRQL_TO_DISPATCH(&OldIrql);

  ASSERT( NDIS_CURRENT_IRQL() == DISPATCH_LEVEL );

  if ( pNBufCtx->pSGList )
  {
    NdisMFreeNetBufferSGList(pNicCtx->hMPDma,
                             pNBufCtx->pSGList,
                             pNBufCtx->pNetBuf);
    pNBufCtx->pSGList = NULL;
  }

  if ( Status == NDIS_STATUS_SUCCESS )
  {
    NDIS_TCP_LARGE_SEND_OFFLOAD_NET_BUFFER_LIST_INFO LsoGsoInfo;

    LsoGsoInfo.Value =
      (NET_BUFFER_LIST_INFO(pNBLCtx->pNBL,
                            TcpLargeSendNetBufferListInfo));
    if ( (LsoGsoInfo.Transmit.Type == NDIS_TCP_LARGE_SEND_OFFLOAD_V1_TYPE) &&
         (LsoGsoInfo.LsoV1Transmit.MSS) )
    {
      LsoGsoInfo.LsoV1TransmitComplete.TcpPayload = LsoBytes;
      LsoGsoInfo.LsoV1TransmitComplete.Type =
        NDIS_TCP_LARGE_SEND_OFFLOAD_V1_TYPE;
    }
    else if( LsoGsoInfo.LsoV2Transmit.MSS )
    {
      /*      LsoGsoInfo.LsoV2TransmitComplete.Reserved = 0;               */
      LsoGsoInfo.LsoV2TransmitComplete.Type =
        LsoGsoInfo.Transmit.Type;
   }
    NET_BUFFER_LIST_INFO(pNBLCtx->pNBL,
                         TcpLargeSendNetBufferListInfo) = LsoGsoInfo.Value;
#ifdef XMP_STATS_OID
    {
      MDL    *mdl;
      UCHAR  *va;
      ULONG   mdlo;
      ULONG   pktLen = NET_BUFFER_DATA_LENGTH(pNBufCtx->pNetBuf);

      mdl     = NET_BUFFER_CURRENT_MDL(pNBufCtx->pNetBuf);
      mdlo    = NET_BUFFER_CURRENT_MDL_OFFSET(pNBufCtx->pNetBuf);
      va      = (UCHAR *)MmGetMdlVirtualAddress(mdl);
      ASSERT(MmIsAddressValid(va));
      ASSERT(MmIsAddressValid(va + mdlo));

      va += mdlo;
      if ( !ETH_IS_MULTICAST(va) )
      { /* directed */
        pNicCtx->OidStats.txDirFrames++;
        pNicCtx->OidStats.txDirBytes += (ULONGLONG)pktLen;
      }
      else if ( ETH_IS_BROADCAST(va) )
      {
        pNicCtx->OidStats.txBcastFrames++;
        pNicCtx->OidStats.txBcastBytes += (ULONGLONG)pktLen;
      }
      else
      {
        pNicCtx->OidStats.txMcastFrames++;
        pNicCtx->OidStats.txMcastBytes += (ULONGLONG)pktLen;
      }
    }
#endif
  }
  else
  {
    NET_BUFFER_LIST_COMPLETION_STATUS(pNBLCtx->pNBL) = Status;
#ifdef XMP_STATS_OID
    pNicCtx->OidStats.txErrFrames++;
#endif
  }

  if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_TX_NBL_CHAIN) ){
     XMP_TX_NBL_DEREFERENCE_COAL(pNicCtx, pFifo, pNBLCtx, TRUE, FlushIt);
  }
  else{
      XMP_TX_NBL_DEREFERENCE(pNicCtx, pFifo, pNBLCtx, TRUE);
  }

 
  if ( !isDpcLevel )
    NDIS_LOWER_IRQL(OldIrql, DISPATCH_LEVEL);

  XMPTRACE(XMP_DBG_TX, ("<== xmpHalTxCompleteNB\n"));
}

VOID
xmpHalTxAbortCB(
   IN  xge_hal_channel_h	      channelh,
	 IN  xge_hal_dtr_h		        dtr,
	 IN  xge_hal_dtr_state_e      state,
	 IN  void			               *userdata,
   IN  xge_hal_channel_reopen_e reopen
  )
{
  xmpTxNBufCtx_t             *pNBufCtx;
  xmpNicCtx_t                *pNicCtx = (xFrameNic_t *)userdata;
  xmpNicTxFifo_t             *pFifo;
  NDIS_STATUS                 Status;
  BOOLEAN                     isDpcLevel;

  if (state != XGE_HAL_DTR_STATE_POSTED) 
    return;
  pNBufCtx = (xmpTxNBufCtx_t *)xge_hal_fifo_dtr_private(dtr);
  pFifo = pNBufCtx->pFifo; 

  XF_GET_SLOCK(&pFifo->HalTxLock);
    xge_hal_fifo_dtr_free(channelh, dtr);
  XF_FREE_SLOCK(&pFifo->HalTxLock);
  pFifo->Stats.txdFree++;

  XMPTRACE(XMP_DBG_WRN, ("xmpHalTxAbortCB: completing NET_BUFFER for reclaimed"
                         " TxD: nic=%p outstanding sends=%d\n",
                         pNicCtx, pFifo->OsSendCnt));

  Status = NDIS_STATUS_FAILURE;
  isDpcLevel = NDIS_CURRENT_IRQL() == DISPATCH_LEVEL;
  /* TODO: check what if ISR/DPC is currently executing? Synchronization???  */

  if ( !bit(pNBufCtx->Flags, XMP_TX_NBUF_FLAG_DIAG) )
    xmpHalTxCompleteNetBuf(pNicCtx, pNBufCtx, 0, Status, isDpcLevel, TRUE);
  else
    xmpDiagSendComplete(pNicCtx, pNBufCtx, XGE_HAL_FAIL);
}

VOID
xmpHalRxAbortCB(
   IN  xge_hal_channel_h	      channelh,
	 IN  xge_hal_dtr_h		        dtr,
	 IN  xge_hal_dtr_state_e	    state,
	 IN  void			               *userdata,
   IN  xge_hal_channel_reopen_e reopen
  )
{
  xmpRxNBLCtx_t              *pNBLCtx;
  xmpNicRxInfo_t             *pRxRing;
  xmpNicCtx_t                *pNicCtx = (xFrameNic_t *)userdata;
  INT                         ring;

  pNBLCtx = ((xmpHalRxDtrCtx_t *)xge_hal_ring_dtr_private(channelh,dtr))->pRxCtx;

  ring = xge_hal_channel_id(channelh);
  pRxRing = &pNicCtx->RxInfo[ring];

  if (state == XGE_HAL_DTR_STATE_POSTED) 
  {
    XF_GET_SLOCK(&pRxRing->RxLock);
      xge_hal_ring_dtr_free(channelh, dtr);
    XF_FREE_SLOCK(&pRxRing->RxLock);
  }
}

NDIS_STATUS
xmpHalGetHwStats( 
  IN  xmpNicCtx_t             *pNicCtx,
  OUT xge_hal_stats_hw_info_t **pStats )

/*++
Routine Description:

    Get the value for a statistics OID

Arguments:

    pNic        Pointer to our adapter 
    Oid         Oid  
    pCounter    Pointer to receive the value
    
Return Value:

    NDIS_STATUS_SUCCESS
    NDIS_STATUS_NOT_SUPPORTED
    
--*/
{
  xge_hal_device_t            *pHalDev;
  xge_hal_status_e             halStatus;
  NDIS_STATUS                  Status = NDIS_STATUS_SUCCESS;

  pHalDev = XMP_NIC_GET_DEV(pNicCtx);
  if ((halStatus =  xge_hal_stats_hw(pHalDev, pStats))!=  XGE_HAL_OK)
  {
    XMPTRACE(XMP_DBG_REQ, ("xmpGetStatsCounters failed=%X\n", halStatus));
    return NDIS_STATUS_FAILURE;
  }
  return Status;
}

VOID
xmpHalGetAdapterSerialnum(
    IN  xmpNicCtx_t        *pNicCtx
    )
{
    xge_hal_device_t           *pHalDev;
    U8 *vpdData, data;
    U8 i = 0;
    int cnt, fail = 0;
    int vpdAddr = 0x80;
    
    pHalDev = XMP_NIC_GET_DEV(pNicCtx);
    if (pHalDev == NULL)
    {
      return;
    }

    if (xge_hal_device_check_id(pHalDev) == XGE_HAL_CARD_HERC)
    {
      vpdAddr = 0x80;
    }
    else if (xge_hal_device_check_id(pHalDev) == XGE_HAL_CARD_XENA)
    {
      vpdAddr = 0x50;
    }
    else
    {
      return;
    }

    RtlCopyMemory((PUCHAR)&pNicCtx->SerialNum, "UNKNOWN", sizeof("UNKNOWN"));


    if ((vpdData = xge_os_malloc(pHalDev->pdev, 
                         XMP_NIC_VPD_READ_ONLY_CONFIG_SPACE_SIZE)) == NULL)
    {
      return;
    }

    xge_os_memzero(vpdData, XMP_NIC_VPD_READ_ONLY_CONFIG_SPACE_SIZE);

    for (i = 0; i < XMP_NIC_VPD_READ_ONLY_CONFIG_SPACE_SIZE; i +=4 )
    {
       xge_os_pci_write8(pHalDev->pdev, pHalDev->cfgh, (vpdAddr + 2), i);
       xge_os_pci_read8(pHalDev->pdev,  pHalDev->cfgh, (vpdAddr + 2), &data);
       xge_os_pci_write8(pHalDev->pdev, pHalDev->cfgh, (vpdAddr + 3), 0);
       for (cnt = 0; cnt <5; cnt++)
       {
         xge_os_mdelay(2);
         xge_os_pci_read8(pHalDev->pdev, pHalDev->cfgh, (vpdAddr + 3), &data);
         if (data == 0x80)
           break;
       }
        if (cnt >= 5)
        {
            xge_debug_device(XGE_ERR, "%s", "Read of VPD data failed");
            fail = 1;
            break;
        }
        xge_os_pci_read32(pHalDev->pdev, pHalDev->cfgh, (vpdAddr + 4),
                          (u32 *)&vpdData[i]);
    }
    if (!fail)
    {
      /* read serial number of adapter */
      for (cnt = 0; cnt < XMP_NIC_VPD_READ_ONLY_CONFIG_SPACE_SIZE; cnt++)
      {
        if ((vpdData[cnt] == 'S') &&
            (vpdData[cnt+1] == 'N') &&
            (vpdData[cnt+2] < XMP_NIC_MAX_SERIAL_NUMBER_SIZE))
        {
           xge_os_memcpy(&pNicCtx->SerialNum, &vpdData[cnt + 3],
                          vpdData[cnt+2]);
             break;
        }
      }
    }

    NdisFreeMemory(vpdData, 0, 0);
}

#pragma warning(pop)   // Save the current warning state

