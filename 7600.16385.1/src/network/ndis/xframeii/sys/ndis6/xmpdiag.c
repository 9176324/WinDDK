#pragma warning(push)   // Save the current warning state
#pragma warning(disable:4100) //unreference parameter in functions
#pragma warning(disable:4101)  //unreference local variables
#pragma warning(disable:4189)  //local variable is initialized but not referenced
#pragma warning( disable : 4127 )
#include "precomp.h"

NDIS_STATUS
xmpDiagSetInformation(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  NDIS_OID                Oid,
  IN  VOID                   *pInfoBuf,
  IN  ULONG                   InfoBufLen,
  OUT ULONG                  *pWritten,
  OUT ULONG                  *pNeeded
  )
{
  NDIS_STATUS           Status = NDIS_STATUS_SUCCESS;
  ULONG                 InfoVal = 0;
  USHORT                InfoVal2;
  U64                   InfoVal8 = 0;
  ULONG				          Offset;
  ULONG                 index;
  xge_hal_device_t     *pHalDev;
  xge_hal_pci_bar0_t   *bar0;
 

  pHalDev = XMP_NIC_GET_DEV(pNicCtx);
  bar0 = (xge_hal_pci_bar0_t *)(void *)pHalDev->bar0;

  if ( Oid != OID_CUSTOM_OID_ADD_MAC_ADDR && 
       Oid != OID_CUSTOM_OID_RMV_MAC_ADDR ) 
    return NDIS_STATUS_NOT_SUPPORTED;

  switch (Oid)
  {
    case OID_CUSTOM_OID_ADD_MAC_ADDR:

      if ( InfoBufLen != ETH_LENGTH_OF_ADDRESS )
        Status = NDIS_STATUS_INVALID_LENGTH;
      /* TODO will need to store the index of address locally  */
   
      Status = xge_hal_device_macaddr_set( pHalDev, 1, pInfoBuf);
  
      if (Status != NDIS_STATUS_SUCCESS)
      {
        ASSERT(0);
        Status = NDIS_STATUS_FAILURE;
      }
      break;

    case OID_CUSTOM_OID_RMV_MAC_ADDR:

      if( InfoBufLen != ETH_LENGTH_OF_ADDRESS )
        Status = NDIS_STATUS_INVALID_LENGTH;

      index = xge_hal_device_macaddr_find( pHalDev, pInfoBuf );
      if ( index )
        Status = xge_hal_device_macaddr_clear( pHalDev, index );
      else
        Status = NDIS_STATUS_FAILURE;

      if (Status != NDIS_STATUS_SUCCESS)
      {
        ASSERT(0);
        Status = NDIS_STATUS_FAILURE;
      }
      break;
  }

  return Status;
}

NDIS_STATUS
xmpDiagQueryInformation(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  NDIS_OID                Oid,
  IN  VOID                   *pInfoBuf,
  IN  ULONG                   InfoBufLen,
  OUT ULONG                  *pWritten,
  OUT ULONG                  *pNeeded
  )
{
  NDIS_STATUS				  Status = NDIS_STATUS_SUCCESS;
  ULONG                       InfoVal = 0;
  USHORT                      InfoVal2;
  U64                         InfoVal8 = 0;
  ULONG						  offset;
  ULONG                       i;
  xge_hal_device_t           *pHalDev;
  xge_hal_pci_bar0_t         *bar0;
  USHORT                      usTmp;
  ULONG                       ulTmp;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpDiagQueryInformation %x\n", Oid));

  if ( (Oid > OID_CUSTOM_OID_START) && (Oid < OID_CUSTOM_OID_END) )
    Status = NDIS_STATUS_SUCCESS;
  else
    Status = NDIS_STATUS_NOT_SUPPORTED;

  pHalDev = XMP_NIC_GET_DEV(pNicCtx);
  bar0 = (xge_hal_pci_bar0_t *)(void *)pHalDev->bar0;

  switch (Oid)
  {

   case OID_CUSTOM_OID_LED_BLINK_START:

     bis( pNicCtx->State, XMP_NIC_LED_BLINK_ENABLED );
     bic( pNicCtx->State, XMP_NIC_LED_BLINK_DISABLED );
     break;

   case OID_CUSTOM_OID_LED_BLINK_STOP:

     bis( pNicCtx->State, XMP_NIC_LED_BLINK_DISABLED );
     bic( pNicCtx->State, XMP_NIC_LED_BLINK_ENABLED );
     break;

   case OID_CUSTOM_OID_READ_REG:

		if( InfoBufLen < sizeof(u64) )
		{
		Status = NDIS_STATUS_INVALID_LENGTH;
		*pNeeded = sizeof(u64);
		break;
		}

		InfoVal = *((u32*)pInfoBuf);                          /* Offset */
		if( InfoVal > 0x4000 )
		{
		Status = NDIS_STATUS_INVALID_PARAMETER;
		*pWritten = 0;
		break;
		}

		InfoVal8 = xge_os_pio_mem_read64( pHalDev->pdev, pHalDev->regh0,
										(u64 *)(((u8 *)bar0) + InfoVal) );
		NdisMoveMemory(pInfoBuf, &InfoVal8, sizeof(InfoVal8));
		*pWritten = sizeof(InfoVal8);
		break;

  case OID_CUSTOM_OID_WRITE_REG:

		if( InfoBufLen < (sizeof(u64) * 2) )
		{
			Status = NDIS_STATUS_INVALID_LENGTH;
			*pNeeded = 2 * sizeof(u64);
			break;
		} 

		InfoVal = *((u32 *)pInfoBuf);                       /* Offset */
		if( InfoVal > 0x4000 )
		{
			*pWritten = 0;
			Status = NDIS_STATUS_INVALID_PARAMETER;
			break;
		}

		InfoVal8 = *((u64*)pInfoBuf+1);
		xge_os_pio_mem_write64(pHalDev->pdev, pHalDev->regh0, InfoVal8,
								(u64 *)(((u8 *)bar0)+InfoVal) );
		*pWritten = sizeof(InfoVal8);
		break;

  case OID_CUSTOM_OID_RX_BYTES:
 
    for (i = 0; i < pNicCtx->RegParams.RxRingNum; i++)
       InfoVal8 += pNicCtx->RxInfo[i].Stats.RxCnt;
    break; 

  case OID_CUSTOM_OID_TX_BYTES:
 
   for (i = 0; i < pNicCtx->RegParams.TxFIFONum; i++)
      InfoVal8 += pNicCtx->TxFifo[i].Stats.TxNBLCount;
    break;

  case OID_CUSTOM_OID_GET_STATS:
	  break;

  case OID_CUSTOM_OID_GET_STATS_DUMP:

	  {
	  xge_hal_stats_hw_info_t     *HwStats;       
      if( InfoBufLen < sizeof (xge_hal_stats_hw_info_t) )
      {
        Status = NDIS_STATUS_INVALID_LENGTH;
        *pNeeded = sizeof( xge_hal_stats_hw_info_t );
        *pWritten = 0;
        break;
      }
      Status = xmpHalGetHwStats(pNicCtx, &HwStats);
	  NdisMoveMemory(pInfoBuf, HwStats, sizeof(xge_hal_stats_hw_info_t));
	  *pWritten = sizeof(xge_hal_stats_hw_info_t);
	  }
	  break;

  case OID_CUSTOM_OID_DUMP_TRACE:
	  break;

  case OID_CUSTOM_OID_DUMP_ADAPTER_STATS:
	  break;

  case OID_CUSTOM_OID_GET_REG_DUMP:
  case OID_CUSTOM_OID_DUMP_BAR0:

		if( InfoBufLen < sizeof(xge_hal_pci_bar0_t)  )
		{
		Status = NDIS_STATUS_INVALID_LENGTH;
		*pNeeded = sizeof(xge_hal_pci_bar0_t);
		*pWritten = 0;
		break;
		}

		for( offset = 0 ; offset < (sizeof(xge_hal_pci_bar0_t)/sizeof(U64)); 
			offset++ )
		{
			InfoVal8 = xge_os_pio_mem_read64(pHalDev->pdev, pHalDev->regh0, 
							((u64 *)((u8 *)bar0)) + offset);
			*(( U64* )pInfoBuf + offset) = InfoVal8;
		}
		*pWritten = sizeof(xge_hal_pci_bar0_t);  
		break;

#if 0
  case OID_CUSTOM_OID_DUMP_BAR2:

		if( InfoBufLen < sizeof(xge_hal_pci_bar2_t)  )
		{
		Status = NDIS_STATUS_INVALID_LENGTH;
		*pNeeded = sizeof(xge_hal_pci_bar2_t);
		*pWritten = 0;
		break;
		}

		for( offset = 0 ; offset < (sizeof(xge_hal_pci_bar2_t)/sizeof(U64)); 
			offset++ )
		{
			InfoVal8 = xge_os_pio_mem_read64(pHalDev->pdev, pHalDev->regh2, 
							((u64 *)((u8 *)bar2)) + offset);
			*(( U64* )pInfoBuf + offset) = InfoVal8;
		}
		*pWritten = sizeof(xge_hal_pci_bar2_t);  
		break;

#endif
  case OID_CUSTOM_OID_PCI_DUMP:
  case OID_CUSTOM_OID_DUMP_PCI_CONFIG_SPACE:
      if( InfoBufLen < sizeof(PCI_COMMON_CONFIG)  )
      {
				Status = NDIS_STATUS_INVALID_LENGTH;
				*pNeeded = sizeof(PCI_COMMON_CONFIG);
				*pWritten = 0;
				break;
      }

      for( i = 0; i < (sizeof(PCI_COMMON_CONFIG)/sizeof(U16)); i++ )
      { 
					 ulTmp = NdisMGetBusData(pNicCtx->hMPAdapter,
																			PCI_WHICHSPACE_CONFIG,
																			i*2,
																			&usTmp,
																			sizeof(USHORT) );
					if( ulTmp!= sizeof(USHORT) )
					{
						XMPTRACE(XMP_DBG_ERR, ("NdisMGetBusData (Command) failed ulTmp=%d\n",
																	ulTmp));
						break;
					}

          *( (( U16* )pInfoBuf) + i ) = usTmp;
      }

#if DBG
			{
				ULONG ulTmp1;
				NdisMGetBusData(pNicCtx->hMPAdapter,
																			PCI_WHICHSPACE_CONFIG,
																			0x90,
																			&ulTmp1,
																			sizeof(ULONG));

				DbgPrint( "MSIX_Cap = %8x \n", ulTmp1 );

				NdisMGetBusData(pNicCtx->hMPAdapter,
																			PCI_WHICHSPACE_CONFIG,
																			0x92,
																			&ulTmp1,
																			sizeof(USHORT));

				DbgPrint( "MSIX_Cap = %8x \n", ulTmp1 & 0xffff );

			}
#endif
	  break;

  case OID_CUSTOM_OID_DUMP_DRIVER_STATS:
	  break;

  case OID_CUSTOM_OID_GET_RX_STATS:
    {
      xmpRxStats_t *rxs = (xmpRxStats_t *) pInfoBuf;

      if ( InfoBufLen < sizeof(xmpRxStats_t) )
      {
        *pNeeded = sizeof(xmpRxStats_t);
        Status = NDIS_STATUS_BUFFER_TOO_SHORT;
        break;
      }

      XF_MCLEAR(pInfoBuf, sizeof(xmpRxStats_t));
      rxs->rxRingCnt = pNicCtx->RegParams.RxRingNum;
      for (i=0; i < pNicCtx->RegParams.RxRingNum; i++)
      {
        NdisMoveMemory(&rxs->rxStats[i], &pNicCtx->RxInfo[i].Stats,
                       sizeof(xmpRxRingStats_t));
      }
      *pWritten = sizeof(xmpRxStats_t);
	  }
	  break;

  case OID_CUSTOM_OID_GET_RSS_REG:
	  *pWritten = 0;
	  xmpTmpRssRegDump(pNicCtx);
	  break;

  case OID_CUSTOM_OID_GET_PCI_MODE:
    {
      xge_hal_status_e hal_status;
      pci_mode_t pci_mode = {0};
      if( InfoBufLen < sizeof(pci_mode_t)  )
      {
        Status = NDIS_STATUS_INVALID_LENGTH;
        *pNeeded = sizeof(pci_mode_t);
        *pWritten = 0;
        break;
      }
      hal_status = xge_hal_device_pci_info_get(pHalDev, 
                (xge_hal_pci_mode_e *)&pci_mode.pci_mode_e,
                (xge_hal_pci_bus_frequency_e *)&pci_mode.pci_bus_frequency_e, 
                (xge_hal_pci_bus_width_e *)&pci_mode.pci_bus_width_e);
      if (hal_status != XGE_HAL_OK)
      {
         if (hal_status == XGE_HAL_ERR_INVALID_PCI_INFO)
         {
           XMPTRACE(XMP_DBG_ERR, ("UnSuppported PCI_MODE !!\n"));
         }
         else
         {
           XMPTRACE(XMP_DBG_ERR, ("Invalid pci device id !!\n"));
         }
         ASSERT(0);
         *pNeeded = sizeof(pci_mode_t);
         *pWritten = 0;
         Status = NDIS_STATUS_NOT_SUPPORTED;
         break;
      }

     if ( (pci_mode.pci_mode_e == XGE_HAL_PCI_33MHZ_MODE) ||
         (pci_mode.pci_mode_e == XGE_HAL_PCI_66MHZ_MODE) ||
         (pci_mode.pci_mode_e == XGE_HAL_PCI_BASIC_MODE) )
         pci_mode.pci_bus_type_e = 0; //PCI bus
     else
         pci_mode.pci_bus_type_e = 1; //PCIX bus

     NdisMoveMemory(pInfoBuf,  &pci_mode, sizeof(pci_mode_t) );
     *pWritten = sizeof(pci_mode_t);
    }
    break;
    
    case OID_CUSTOM_OID_GET_SERIAL_NUMBER:
      if ( InfoBufLen < (sizeof(UCHAR) * XMP_NIC_MAX_SERIAL_NUMBER_SIZE) )
      {     
       Status = NDIS_STATUS_INVALID_LENGTH;
       *pNeeded = sizeof(UCHAR) * XMP_NIC_MAX_SERIAL_NUMBER_SIZE;
      }
      else
      {
       xge_os_memzero(pInfoBuf, XMP_NIC_MAX_SERIAL_NUMBER_SIZE);
       NdisMoveMemory(pInfoBuf, &pNicCtx->SerialNum, 
                      XMP_NIC_MAX_SERIAL_NUMBER_SIZE);
       *pWritten = sizeof(UCHAR) * XMP_NIC_MAX_SERIAL_NUMBER_SIZE;
       *pNeeded = 0;
      }
       
    case OID_CUSTOM_OID_XENA_REV:
      if ( InfoBufLen < sizeof(UCHAR) )
      {
        *pWritten = 0;
        Status = NDIS_STATUS_INVALID_LENGTH;
        *pNeeded = sizeof(UCHAR);
      }
      else
      {
        *(UCHAR *)pInfoBuf = pNicCtx->PciCfg.RevisionID;
        *pWritten = sizeof(UCHAR);
        *pNeeded = 0;
      }
      break;

    case OID_CUSTOM_OID_DIAG_INIT:
      XMPTRACE(XMP_DBG_INFO, ("OID_CUSTOM_DIAG_INIT\n"));
      Status = xmpDiagInit(pNicCtx, pInfoBuf, InfoBufLen, pWritten, pNeeded);
      break;

    case OID_CUSTOM_OID_DIAG_RESET_TEST_INIT:
      XMPTRACE(XMP_DBG_INFO, ("OID_CUSTOM_DIAG_RESET_TEST_INIT\n"));
      Status = xmpDiagResetTestInit(pNicCtx, pInfoBuf, InfoBufLen, pWritten,
                                    pNeeded);
      break;

    case OID_CUSTOM_OID_DIAG_RESET_TEST:
      XMPTRACE(XMP_DBG_INFO, ("OID_CUSTOM_DIAG_RESET_TEST\n"));
      Status = xmpDiagResetTest(pNicCtx, pInfoBuf, InfoBufLen, pWritten,
                                pNeeded);
      break;

    case OID_CUSTOM_OID_DIAG_INIT_TEST:
      XMPTRACE(XMP_DBG_INFO, ("OID_CUSTOM_DIAG_INIT_TEST\n"));
      Status = xmpDiagInitTest(pNicCtx, pInfoBuf, InfoBufLen, pWritten,
                               pNeeded);
      break;

    case OID_CUSTOM_OID_DIAG_SEND_TEST:
      XMPTRACE(XMP_DBG_INFO, ("OID_CUSTOM_DIAG_SEND_TEST\n"));
      Status = xmpDiagSend(pNicCtx, pInfoBuf, InfoBufLen, pWritten, pNeeded);
      break;

    case OID_CUSTOM_OID_DIAG_RCV_TEST:
      XMPTRACE(XMP_DBG_INFO, ("OID_CUSTOM_DIAG_RCV_TEST\n"));
      Status = xmpDiagRecv(pNicCtx, pInfoBuf, InfoBufLen, pWritten, pNeeded);
      break;

    case OID_CUSTOM_OID_DIAG_FINISH_PH1:
      XMPTRACE(XMP_DBG_INFO, ("OID_CUSTOM_DIAG_FINISH_PH1\n"));
      Status = xmpDiagFinish1(pNicCtx, pInfoBuf, InfoBufLen, pWritten,
                              pNeeded);
      break;

    case OID_CUSTOM_OID_DIAG_FINISH_PH2:
      XMPTRACE(XMP_DBG_INFO, ("OID_CUSTOM_DIAG_FINISH_PH2\n"));
      Status = xmpDiagFinish2(pNicCtx, pInfoBuf, InfoBufLen, pWritten,
                              pNeeded);
      break;

    case OID_CUSTOM_OID_DIAG_FINISH:
      XMPTRACE(XMP_DBG_INFO, ("OID_CUSTOM_DIAG_FINISH\n"));
      Status = xmpDiagFinish(pNicCtx, pInfoBuf, InfoBufLen, pWritten,
                              pNeeded);
      break;

  }
  XMPTRACE(XMP_DBG_INFO, ("<== xmpDiagQueryInformation\n" ));
  return Status;
}

NDIS_STATUS
xmpDiagInit(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  )
{
  NDIS_STATUS                 status = NDIS_STATUS_SUCCESS;
  BOOLEAN                     in_dpc = NDIS_CURRENT_IRQL() > PASSIVE_LEVEL;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpDiagInit\n"));

  do
  {
    if ( InformationBufferLength != sizeof(diag_init_t) )
    {
      XMPTRACE(XMP_DBG_WRN, ("xmpDiagInit: invalid length = %d\n",
                             InformationBufferLength));
      *BytesWritten = 0;
      *BytesNeeded  = sizeof(diag_init_t);
      status = NDIS_STATUS_BUFFER_TOO_SHORT;
      break;
    }
    *BytesNeeded = sizeof(diag_init_t);
    *BytesWritten = sizeof(diag_init_t);
#ifdef ENABLE_IT_AFTER_TEST
    if (  pNicCtx->bInDiagMode || 
          pNicCtx->Diag.bSendInProgress ||
          pNicCtx->NeedsNicReset )
    {
      XMPTRACE(XMP_DBG_WRN, ("xmpDiagInit:Failure: DiagMode=%s "
                             "SendInProgress=%s NicNeedsReset=%s\n",
                             (pNicCtx->bInDiagMode ? "Yes" : "No"),
                             (pNicCtx->Diag.bSendInProgress ? "Yes" : "No"),
                             (pNicCtx->NeedsNicReset ? "Yes" : "No")));
      status = NDIS_STATUS_FAILURE;
      break;
    }
#endif
    pNicCtx->Diag.status       = XGE_HAL_OK;
    pNicCtx->Diag.SendError    = FALSE;
    pNicCtx->Diag.RecvError    = FALSE;
    pNicCtx->Diag.status       = XGE_HAL_FAIL;
    pNicCtx->Diag.RxPktLastCnt = pNicCtx->Diag.RxPktCnt;
    pNicCtx->Diag.rx_t_code    = 0;
    pNicCtx->bInDiagMode       = TRUE;
    pNicCtx->bLoopback         = TRUE;
    pNicCtx->NeedsNicReset     = TRUE;
    XMPTRACE(XMP_DBG_INFO, ("xmpDiagInit: Enter diagnostics mode\n"));
  } while(FALSE);

  XMPTRACE(XMP_DBG_INFO, ("<== xmpDiagInit\n"));
  return status;
}

NDIS_STATUS
xmpDiagResetTestInit(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  )
{
  NDIS_STATUS                 status = NDIS_STATUS_SUCCESS;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpDiagResetTestInit\n"));

  do
  {
    if ( InformationBufferLength != sizeof(diag_reset_init_t) )
    {
      XMPTRACE(XMP_DBG_WRN, ("xmpDiagResetTestInit: invalid length = %d\n",
                             InformationBufferLength));
      *BytesWritten = 0;
      *BytesNeeded = sizeof(diag_reset_init_t);
      status = NDIS_STATUS_FAILURE;
      break;
    }

    *BytesNeeded = sizeof(diag_reset_init_t);
    *BytesWritten = sizeof(diag_reset_init_t);

    XMPTRACE(XMP_DBG_WRN, ("xmpDiagResetTestInit: Done\n"));
  } while(FALSE);

  XMPTRACE(XMP_DBG_INFO, ("<== xmpDiagResetTestInit\n"));
  return status;
}

/*
 * Performs BIST test, Register init test, Link indications test
 * If XFRAME is set into fiber loopback mode then it is assumed that before 
 * invoking this function loopback fiber is already connected.
 * Loopback mode could be fiber or MAC loopback mode
 *
 * If this function returns other than NDIS_STATUS_SUCCESS then caller 
 * has to see reason code for additional information.
 *
 * Assumption: Caller has already called xg_diag_reset_test_init() and after 
 * delay of two seconds caller calls xg_diag_reset_test().
 *
 * OID_CUSTOM_RESET_TEST
 *
 * IN/OUT parameter
 * struct {
 *  U32 reason_code;
 *  U32 loopback_mode;
 * }
 */
NDIS_STATUS
xmpDiagResetTest(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  )
{
  diag_reset_t               *info;
  NDIS_STATUS                 status = NDIS_STATUS_SUCCESS;
  BOOLEAN                     in_dpc = NDIS_CURRENT_IRQL() > PASSIVE_LEVEL;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpDiagResetTest\n"));

  do
  {
    if ( InformationBufferLength != sizeof(diag_reset_t) )
    {
      XMPTRACE(XMP_DBG_WRN, ("xmpDiagResetTest: invalid length = %d\n",
                             InformationBufferLength));
      *BytesWritten = 0;
      *BytesNeeded  = sizeof(diag_reset_t);
      status = NDIS_STATUS_BUFFER_TOO_SHORT;
      break;
    }

    info = (diag_reset_t *)InformationBuffer;
    *BytesNeeded = sizeof(diag_reset_t);
    *BytesWritten = sizeof(diag_reset_t);
    info->reason_code = 0;

    XMPTRACE(XMP_DBG_INFO, ("xmpDiagResetTest: Done.\n"));
  } while( FALSE );

  XMPTRACE(XMP_DBG_INFO, ("<== xmpDiagResetTest\n"));
  return status;;
}

NDIS_STATUS
xmpDiagInitTest(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  )
{
  diag_init_test_t           *info;
  NDIS_STATUS                 status = NDIS_STATUS_SUCCESS;
  BOOLEAN                     in_dpc = NDIS_CURRENT_IRQL() > PASSIVE_LEVEL;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpDiagInitTest\n"));

  do
  {
    if ( InformationBufferLength != sizeof(diag_init_test_t) )
    {
      XMPTRACE(XMP_DBG_WRN, ("xmpDiagInitTest: invalid length = %d\n",
                             InformationBufferLength));
      *BytesWritten = 0;
      *BytesNeeded  = sizeof(diag_init_test_t);
      status = NDIS_STATUS_BUFFER_TOO_SHORT;
      break;
    }

    info = (diag_init_test_t *)InformationBuffer;
    *BytesNeeded = sizeof(diag_init_test_t);
    *BytesWritten = sizeof(diag_init_test_t);
    info->reason_code = 0;

    if ( pNicCtx->mpState != XMP_NIC_MPSTATE_RUNNING )
    {
      status = NDIS_STATUS_FAILURE;
      XMPTRACE(XMP_DBG_WRN, ("xmpDiagInitTest: miniport adapter state = %d\n",
                             pNicCtx->mpState));
      break;
    }

    /* TODO: clear OK_TO_TK and OK_TO_RX flags */
    info->reason_code = ((pNicCtx->Diag.status == XGE_HAL_OK) ? 
                         XG_DIAG_SUCCESS : XG_DIAG_ERR_RESET_ERROR);
    XMPTRACE(XMP_DBG_INFO, ("xmpDiagInitTest: Successful.\n"));
  } while( FALSE );

  XMPTRACE(XMP_DBG_INFO, ("<== xmpDiagInitTest\n"));
  return status;
}

/*
 *  Send a single packet
 *  OID_CUSTOM_SEND_RCV_TEST
 */
NDIS_STATUS
xmpDiagSend( 
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  )
{
  diag_send_t                *info;
  xmpTxNBufCtx_t             *pNBufCtx;
  xmpNicTxFifo_t             *pFifo;
  UCHAR                      *pBuffAddr;
  UCHAR                      *ptr;
  xge_hal_dtr_h               dtr;
  xge_hal_status_e            halStatus = XGE_HAL_OK;
  NDIS_STATUS                 status = NDIS_STATUS_SUCCESS;
  KIRQL                       OldIrql = DISPATCH_LEVEL;
  UINT                        i;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpDiagSend\n"));

  if (NDIS_CURRENT_IRQL() != DISPATCH_LEVEL)
    NDIS_RAISE_IRQL_TO_DISPATCH(&OldIrql);

  do
  {
    if ( InformationBufferLength != sizeof(diag_init_test_t) )
    {
      XMPTRACE(XMP_DBG_WRN, ("xmpDiagSend: invalid length = %d\n",
                             InformationBufferLength));
      *BytesWritten = 0;
      *BytesNeeded  = sizeof(diag_send_t);
      status = NDIS_STATUS_BUFFER_TOO_SHORT;
      break;
    }

    info = (diag_send_t *)InformationBuffer;
    *BytesNeeded = sizeof(diag_send_t);
    *BytesWritten = sizeof(diag_send_t);
    info->reason_code = XG_DIAG_SUCCESS;

    if ( !pNicCtx->bInDiagMode || pNicCtx->Diag.bSendInProgress )
    {
      XMPTRACE(XMP_DBG_WRN, ("xmpDiagSend:Failure: DiagMode=%d "
                             "SendInProgress=%s\n",
                             (pNicCtx->bInDiagMode ? "Yes" : "No"),
                             (pNicCtx->Diag.bSendInProgress ? "Yes" : "No")));
      status = NDIS_STATUS_FAILURE;
      break;
    }

    pNicCtx->Diag.SendError = TRUE;
    pNicCtx->Diag.RecvError = TRUE;
    pNicCtx->Diag.TxPktSz   = 512;

    ptr = pBuffAddr = (UCHAR*) pNicCtx->Diag.pBuffAddr;
    /* Data */
    /* dest addr */
    NdisMoveMemory(ptr, pNicCtx->CurrentAddress, ETH_LENGTH_OF_ADDRESS);
    ptr += ETH_LENGTH_OF_ADDRESS;
    /* src addr */
    NdisMoveMemory(ptr, pNicCtx->CurrentAddress, ETH_LENGTH_OF_ADDRESS);
    ptr += ETH_LENGTH_OF_ADDRESS;

    /* type */
    *ptr++ = 0x08; 
    *ptr++ = 0x01; 

    /* data */
    for( i=0; i < (pNicCtx->Diag.TxPktSz - XENAMP_ETH_HDR_LEN); i++ )
    {
      *ptr++ = 0x50; 
    }
    pFifo = &pNicCtx->TxFifo[0];
    XF_DPR_GET_SLOCK(&pFifo->HalTxLock);
      halStatus = xge_hal_fifo_dtr_reserve(pFifo->halChannel, &dtr);
    XF_DPR_FREE_SLOCK(&pFifo->HalTxLock);

    if ( halStatus != XGE_HAL_OK )
    {
      XMPTRACE(XMP_DBG_WRN, ("xmpDiagSend: Out of TxDs: err=%x\n", halStatus));
      break;
    }

    pNBufCtx = (xmpTxNBufCtx_t *)xge_hal_fifo_dtr_private(dtr);
    pNBufCtx->pNicCtx = pNicCtx;
    pNBufCtx->pFifo   = pFifo;
    pNBufCtx->pNBLCtx = NULL;
    pNBufCtx->pNetBuf = NULL;
    pNBufCtx->dtr     = dtr;
    bis(pNBufCtx->Flags, XMP_TX_NBUF_FLAG_DIAG);
    pNicCtx->Diag.bSendInProgress = TRUE;

    xge_hal_fifo_dtr_buffer_set(pFifo->halChannel,
                                dtr,
                                0,
                                pNicCtx->Diag.BuffAddrPhy.QuadPart,
                                pNicCtx->Diag.TxPktSz);
    XMPTRACE(XMP_DBG_INFO, ("xmpDiagSend: Posting dtr=%p, pNBufCtx=%p\n",
                            dtr, pNBufCtx));
    pFifo->Stats.txdPost++;
    pNicCtx->Diag.TxPktCnt++;
    XF_DPR_GET_SLOCK(&pFifo->HalTxLock);
      xge_hal_fifo_dtr_post(pFifo->halChannel, dtr);
    XF_DPR_FREE_SLOCK(&pFifo->HalTxLock);
    XMPTRACE(XMP_DBG_INFO, ("xmpDiagSend: Posted dtr=%p\n", dtr));
  } while(0);

  if (OldIrql != DISPATCH_LEVEL)
    NDIS_LOWER_IRQL(OldIrql, DISPATCH_LEVEL);

  XMPTRACE(XMP_DBG_INFO, ("<== xmpDiagSend\n"));
  return status;
}

void
xmpDiagSendComplete(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  xmpTxNBufCtx_t         *pNBufCtx,
  IN  U8                      txStatus
  )
{
  XMPTRACE(XMP_DBG_INFO, ("xmpDiagSendComplete: pNBufCtx=%p\n", pNBufCtx));
  ASSERT(pNicCtx->Diag.bSendInProgress);
  ASSERT(pNicCtx->Diag.TxPktCompCnt < pNicCtx->Diag.TxPktCnt);
  pNicCtx->Diag.TxPktCompCnt++;
  pNicCtx->Diag.SendError = (txStatus == XGE_HAL_OK);
  pNicCtx->Diag.bSendInProgress = FALSE;
  XMPTRACE(XMP_DBG_INFO, ("<== xmpDiagSendComplete\n"));
}

void
xmpDiagRecvComplete(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  xmpRxNBLCtx_t          *pNBLCtx,
  IN  ULONG                   pktLen,
  IN  U8                      rxStatus,
  IN  U32                     ringNum
  )
{
  ULONG                       Flags = 0;
  xmpNicRxInfo_t             *pRxInfo;


  XMPTRACE(XMP_DBG_INFO, ("==> xmpDiagRecvComplete: pktLen=%d rxStatus=%d\n",
                          pktLen, rxStatus));
#if ENABLE_ASSERT
  ASSERT(pNicCtx->Diag.RxPktCnt < pNicCtx->Diag.TxPktCnt);
#endif

  pRxInfo = &(pNicCtx->RxInfo[ringNum]);
  pNicCtx->Diag.RxPktCnt++;
  if (pktLen != pNicCtx->Diag.TxPktSz)
  {
    pNicCtx->Diag.RecvError = FALSE;
    pNicCtx->Diag.rx_t_code = XG_DIAG_RX_LEN_INCORRECT;
  }
  else
  {
    pNicCtx->Diag.RecvError = TRUE;
  }

  if (rxStatus)
  {
    /* rx error */
    pNicCtx->Diag.RecvError = TRUE;
    switch (rxStatus)
    {
    case XGE_HAL_RXD_T_CODE_PARITY:
      pNicCtx->Diag.rx_t_code = XG_DIAG_RX_PARITY_ERROR;
      break;
    case XGE_HAL_RXD_T_CODE_ABORT:
      pNicCtx->Diag.rx_t_code = XG_DIAG_RX_ABORT_ERROR;
      break;
    case XGE_HAL_RXD_T_CODE_PARITY_ABORT:
      pNicCtx->Diag.rx_t_code = XG_DIAG_RX_PARITY_ABORT_ERROR;
      break;
    case XGE_HAL_RXD_T_CODE_RDA_FAILURE:
      pNicCtx->Diag.rx_t_code = XG_DIAG_RX_RDA_ERROR;
      break;
    case XGE_HAL_RXD_T_CODE_UNKNOWN_PROTO:
      /* not an error */
      pNicCtx->Diag.rx_t_code = 0;
      pNicCtx->Diag.RecvError = FALSE;
      break;
    case XGE_HAL_RXD_T_CODE_BAD_FCS:
      pNicCtx->Diag.rx_t_code = XG_DIAG_RX_FCS_ERROR;
      break;
    case XGE_HAL_RXD_T_CODE_BUFF_SIZE:
      pNicCtx->Diag.rx_t_code = XG_DIAG_RX_BUFF_SZ_ERROR;
      break;
    case XGE_HAL_RXD_T_CODE_BAD_ECC:
      pNicCtx->Diag.rx_t_code = XG_DIAG_RX_BUFF_ECC_ERROR;
      break;
    case XGE_HAL_RXD_T_CODE_UNKNOWN:
      pNicCtx->Diag.rx_t_code = XG_DIAG_RX_BUFF_UNKNOWN_ERROR;
      break;
    };
  }

  if (NDIS_CURRENT_IRQL() == DISPATCH_LEVEL)
    bis(Flags, NDIS_RETURN_FLAGS_DISPATCH_LEVEL);
  XF_DPR_GET_SLOCK(&pRxInfo->RxLock);
   pRxInfo->UlpNBLCnt++; 
  XF_DPR_FREE_SLOCK(&pRxInfo->RxLock);
  xenaMpReturnNetBufferLists((NDIS_HANDLE)pNicCtx, pNBLCtx->pNBL, Flags);
  XMPTRACE(XMP_DBG_INFO, ("<== xmpDiagRecvComplete\n"));
}

NDIS_STATUS
xmpDiagRecv(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  )
{
  diag_recv_test_t           *info;
  NDIS_STATUS                 status = NDIS_STATUS_SUCCESS;
  BOOLEAN                     in_dpc = NDIS_CURRENT_IRQL() > PASSIVE_LEVEL;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpDiagRecv\n"));

  do
  {
    if ( InformationBufferLength != sizeof(diag_recv_test_t) )
    {
      XMPTRACE(XMP_DBG_WRN, ("xmpDiagRecvTest: invalid length = %d\n",
                             InformationBufferLength));
      *BytesWritten = 0;
      *BytesNeeded  = sizeof(diag_recv_test_t);
      status = NDIS_STATUS_BUFFER_TOO_SHORT;
      break;
    }

    info = (diag_recv_test_t *)InformationBuffer;
    *BytesNeeded = sizeof(diag_recv_test_t);
    *BytesWritten = sizeof(diag_recv_test_t);

    if ( pNicCtx->Diag.RxPktCnt == pNicCtx->Diag.RxPktLastCnt )
      info->reason_code = XG_DIAG_RX_FAILED;
    else
      info->reason_code = XG_DIAG_SUCCESS;
  } while(0);

  XMPTRACE(XMP_DBG_INFO, ("<== xmpDiagRecv\n"));
  return status;
}

NDIS_STATUS
xmpDiagFinish1(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  )
{
  diag_fin_test_t            *info;
  NDIS_STATUS                 status = NDIS_STATUS_SUCCESS;
  BOOLEAN                     in_dpc = NDIS_CURRENT_IRQL() > PASSIVE_LEVEL;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpDiagFinish1\n"));

  do
  {
    if ( InformationBufferLength != sizeof(diag_fin_test_t) )
    {
      XMPTRACE(XMP_DBG_WRN, ("xmpDiagFinish1: invalid length = %d\n",
                             InformationBufferLength));
      *BytesWritten = 0;
      *BytesNeeded  = sizeof(diag_fin_test_t);
      status = NDIS_STATUS_BUFFER_TOO_SHORT;
      break;
    }

    info = (diag_fin_test_t *)InformationBuffer;

    pNicCtx->bLoopback     = FALSE;
    pNicCtx->NeedsNicReset = TRUE;
    info->reason_code = XG_DIAG_SUCCESS;
    *BytesNeeded = sizeof(diag_fin_test_t);
    *BytesWritten = sizeof(diag_fin_test_t);
  } while(0);

  XMPTRACE(XMP_DBG_INFO, ("<== xmpDiagFinish1\n"));
  return status;
}

NDIS_STATUS
xmpDiagFinish2(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  )
{
  diag_fin_test_t            *info;
  NDIS_STATUS                 status = NDIS_STATUS_SUCCESS;
  BOOLEAN                     in_dpc = NDIS_CURRENT_IRQL() > PASSIVE_LEVEL;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpDiagFinish2\n"));

  do
  {
    if ( InformationBufferLength != sizeof(diag_fin_test_t) )
    {
      XMPTRACE(XMP_DBG_WRN, ("xmpDiagFinish2: invalid length = %d\n",
                             InformationBufferLength));
      *BytesWritten = 0;
      *BytesNeeded  = sizeof(diag_fin_test_t);
      status = NDIS_STATUS_BUFFER_TOO_SHORT;
      break;
    }

    info = (diag_fin_test_t *)InformationBuffer;
    info->reason_code = XG_DIAG_SUCCESS;
    *BytesNeeded = sizeof(diag_fin_test_t);
    *BytesWritten = sizeof(diag_fin_test_t);
  } while(0);

  XMPTRACE(XMP_DBG_INFO, ("<== xmpDiagFinish2\n"));
  return status;
}

NDIS_STATUS
xmpDiagFinish(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  VOID                   *InformationBuffer,
  IN  U32                     InformationBufferLength,
  OUT U32                    *BytesWritten, 
  OUT U32                    *BytesNeeded
  )
{
  diag_fin_test_t            *info;
  NDIS_STATUS                 status = NDIS_STATUS_SUCCESS;
  BOOLEAN                     in_dpc = NDIS_CURRENT_IRQL() > PASSIVE_LEVEL;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpDiagFinish\n"));

  do
  {
    if ( InformationBufferLength != sizeof(diag_fin_test_t) )
    {
      XMPTRACE(XMP_DBG_WRN, ("xmpDiagFinish2: invalid length = %d\n",
                             InformationBufferLength));
      *BytesWritten = 0;
      *BytesNeeded  = sizeof(diag_fin_test_t);
      status = NDIS_STATUS_BUFFER_TOO_SHORT;
      break;
    }

    info = (diag_fin_test_t *)InformationBuffer;
    info->reason_code = XG_DIAG_SUCCESS;
    *BytesNeeded = sizeof(diag_fin_test_t);
    *BytesWritten = sizeof(diag_fin_test_t);

    pNicCtx->bInDiagMode       = FALSE;
    pNicCtx->bLoopback         = FALSE;

    if ( pNicCtx->mpState != XMP_NIC_MPSTATE_RUNNING )
    {
      status = NDIS_STATUS_FAILURE;
      XMPTRACE(XMP_DBG_WRN, ("xmpDiagFinish: miniport adapter state = %d\n",
                             pNicCtx->mpState));
      break;
    }
  } while(0);

  XMPTRACE(XMP_DBG_INFO, ("<== xmpDiagFinish\n"));
  return status;
}
#pragma warning(pop)   // Save the current warning state

