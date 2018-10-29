#pragma warning(push)   // Save the current warning state
#pragma warning(disable:4100) //unreference parameter in functions
#pragma warning(disable:4101)  //unreference local variables
#pragma warning(disable:4189)  //local variable is initialized but not referenced
#pragma warning( disable : 4127 )
/*
   NAME
     xmpreq.c

   DESCRIPTION
     This module implements request path

   NOTES

   MODIFIED
     mgambhir   01/20/04 Creation.
*/

#include "precomp.h"
/* ============================================================================
 * Globals
 * ==========================================================================*/
NDIS_OID xmpNicSupportedOids[] =
{
    OID_GEN_SUPPORTED_LIST,
    OID_GEN_HARDWARE_STATUS,
    OID_GEN_MEDIA_SUPPORTED,
    OID_GEN_MEDIA_IN_USE,
    OID_GEN_MAXIMUM_LOOKAHEAD,
    OID_GEN_MAXIMUM_FRAME_SIZE,
    OID_GEN_LINK_SPEED,
    OID_GEN_TRANSMIT_BUFFER_SPACE,
    OID_GEN_RECEIVE_BUFFER_SPACE,
    OID_GEN_TRANSMIT_BLOCK_SIZE,
    OID_GEN_RECEIVE_BLOCK_SIZE,
    OID_GEN_VENDOR_ID,
    OID_GEN_VENDOR_DESCRIPTION,
    OID_GEN_VENDOR_DRIVER_VERSION,
    OID_GEN_CURRENT_PACKET_FILTER,
    OID_GEN_CURRENT_LOOKAHEAD,
    OID_GEN_DRIVER_VERSION,
    OID_GEN_MAXIMUM_TOTAL_SIZE,
    OID_GEN_MAC_OPTIONS,
    OID_GEN_MEDIA_CONNECT_STATUS,
    OID_GEN_MAXIMUM_SEND_PACKETS,
    /*    OID_GEN_SUPPORTED_GUIDS,*/
    OID_GEN_XMIT_OK,
    OID_GEN_RCV_OK,
    OID_GEN_XMIT_ERROR,
    OID_GEN_RCV_ERROR,
    OID_GEN_RCV_NO_BUFFER,
    OID_GEN_RCV_CRC_ERROR,
    OID_GEN_RCV_DISCARDS,
    OID_GEN_TRANSMIT_QUEUE_LENGTH,
    OID_802_3_XMIT_ONE_COLLISION,
    OID_802_3_XMIT_MORE_COLLISIONS,
    OID_802_3_XMIT_MAX_COLLISIONS,
    OID_802_3_RCV_ERROR_ALIGNMENT,
    /* OID_GEN_DIRECTED_FRAMES_RCV,
       OID_GEN_DIRECTED_FRAMES_XMIT,
    OID_GEN_MULTICAST_FRAMES_XMIT,
    OID_GEN_MULTICAST_FRAMES_RCV,
    OID_GEN_BROADCAST_FRAMES_XMIT,
    OID_GEN_BROADCAST_FRAMES_RCV,*/
    OID_GEN_STATISTICS,
    OID_GEN_PHYSICAL_MEDIUM,
    OID_802_3_PERMANENT_ADDRESS,
    OID_802_3_CURRENT_ADDRESS,
    OID_802_3_MULTICAST_LIST,
    OID_802_3_MAXIMUM_LIST_SIZE,
    OID_OFFLOAD_ENCAPSULATION,
    OID_GEN_RECEIVE_SCALE_CAPABILITIES,
    OID_GEN_RECEIVE_SCALE_PARAMETERS,
    OID_GEN_INTERRUPT_MODERATION,
    OID_TCP_OFFLOAD_PARAMETERS,
    OID_GEN_HD_SPLIT_PARAMETERS
#ifdef XMP_DIAG
   , XMP_DIAG_OID_LIST
#endif
};

/* ============================================================================
 * Private static functions protos
 * ==========================================================================*/

NDIS_STATUS
xmpQueryInformation(
  IN  NDIS_HANDLE             MiniportAdapterContext,
  IN  NDIS_OID_REQUEST        *Request
  );

NDIS_STATUS
xmpSetInformation(
  IN  NDIS_HANDLE             MiniportAdapterContext,
  IN  NDIS_OID_REQUEST        *Request
  );

NDIS_STATUS
xmpMethodRequest(
  IN  xFrameNic_t             *pAdapter,
  IN  PNDIS_OID_REQUEST        Request
  );

NDIS_STATUS
xmpGetStatsCounters( 
  IN  xFrameNic_t             *pNic,
  IN  NDIS_OID                Oid,
  OUT U64                    *pCounter 
  );

NDIS_STATUS
xmpSetFilter(
  IN  xFrameNic_t             *pNic,
  IN  ULONG                   PacketFilter
  );

/* ============================================================================
 * Public functions definition
 * =========================================================================

/* 
 * xenaMpRequest
 *
 * Description:
 *   Request Handler
 *
 * Arguments:
 *
 *   MiniportAdapterContext  Pointer to the adapter structure
 *   NdisRequest             Pointer to the query request
 *
 *  Return Value:
 *   
 *   NDIS_STATUS_SUCCESS
 *   NDIS_STATUS_NOT_SUPPORTED
 *   NDIS_STATUS_BUFFER_TOO_SHORT
 *
 *  IRQL: <=DISPATCH_LEVEL
 *
 *  Notes:
 */
NDIS_STATUS
xenaMpRequest(
  IN  NDIS_HANDLE             MiniportAdapterContext,
  IN  PNDIS_OID_REQUEST       NdisRequest
  )
{
  xmpNicCtx_t                 *pNic = (xmpNicCtx_t  *)MiniportAdapterContext;
  NDIS_STATUS                 Status = NDIS_STATUS_SUCCESS;
  NDIS_REQUEST_TYPE           RequestType;
  /* TBD : Need to check here if reset or adapter remove is in progress */
    
  RequestType = NdisRequest->RequestType;
    
  switch (RequestType)
  {
    case NdisRequestMethod:
       Status = xmpMethodRequest(pNic, NdisRequest);
       break;

    case NdisRequestSetInformation:            
       Status = xmpSetInformation(pNic, NdisRequest);
       break;
                
    case NdisRequestQueryInformation:
    case NdisRequestQueryStatistics:
       Status = xmpQueryInformation(pNic, NdisRequest);
        break;

    default:
       Status = NDIS_STATUS_NOT_SUPPORTED;
       break;
    }
    
    return Status;
}

VOID
xenaMpCancelRequest(
  IN  NDIS_HANDLE            MiniportAdapterContext,
  IN  PVOID                  RequestId
  )
{
  xFrameNic_t                *pNic = (xFrameNic_t *)MiniportAdapterContext;
  xmpNicCtx_t                *pNicCtx;
  pNicCtx = XMP_NIC_CTX_PTR(pNic);
  /*  NdisMRequestComplete(AdapterHandle, 
                       PendingRequest, 
                       NDIS_STATUS_REQUEST_ABORTED);
  */
}

/* ============================================================================
 * Private functions definition
 * ==========================================================================*/

/* 
 * xmpQueryInformation
 *
 * Description:
 *   QueryInformation Handler
 *
 * Arguments:
 *
 *   MiniportAdapterContext  Pointer to the adapter structure
 *   NdisRequest             Pointer to the query request
 *
 *  Return Value:
 *   
 *   NDIS_STATUS_SUCCESS
 *   NDIS_STATUS_NOT_SUPPORTED
 *   NDIS_STATUS_BUFFER_TOO_SHORT
 *
 *  IRQL: <=DISPATCH_LEVEL
 *
 *  Notes:
 */
NDIS_STATUS
xmpQueryInformation(
  IN  xmpNicCtx_t             *pNicCtx,
  IN  PNDIS_OID_REQUEST       NdisRequest
  )
{
  NDIS_STATUS                 Status = NDIS_STATUS_SUCCESS;
  NDIS_OID                    Oid;
  VOID                       *InformationBuffer;
  ULONG                       InformationBufferLength;
  ULONG                       BytesWritten;
  ULONG                       BytesNeeded;
  ULONG                       VendorDescSize;
  ULONG                       i;
  ULONG                       InfoVal = 0;
  USHORT                      InfoVal2;
  U64                         InfoVal8 = 0;
  VOID                       *pInfo = (VOID *) &InfoVal;
  ULONG                       InfoLen = sizeof(InfoVal);
  ULONG                       BytesAvailable = InfoLen;
  NDIS_HARDWARE_STATUS        HardwareStatus = NdisHardwareStatusReady;
  NDIS_MEDIUM                 Medium = XENAMP_NDIS_MEDIA_TYPE;
  NDIS_PHYSICAL_MEDIUM        PhysMedium = NdisPhysicalMediumUnspecified;
  UCHAR                       VendorDesc[] = XENAMP_VENDOR_DESC;
  ULONG                       VendorDriverVersion = XENAMP_DRIVER_VERSION;
  xge_hal_device_t           *pHalDev;
  xge_hal_pci_bar0_t         *bar0;
  NDIS_RECEIVE_SCALE_CAPABILITIES RssCapabilities = {0};
  NDIS_OFFLOAD OffloadCapabilities = {0};
  NDIS_INTERRUPT_MODERATION_PARAMETERS IntrModr = {0};


  pHalDev = XMP_NIC_GET_DEV(pNicCtx);
  bar0 = (xge_hal_pci_bar0_t *)(void *)pHalDev->bar0;

  Oid = NdisRequest->DATA.QUERY_INFORMATION.Oid;
  InformationBuffer = NdisRequest->DATA.QUERY_INFORMATION.InformationBuffer;
  InformationBufferLength = NdisRequest->DATA.QUERY_INFORMATION.InformationBufferLength;

  BytesWritten = 0;
  BytesNeeded = 0;
  XMPTRACE(XMP_DBG_REQ, ("==> xmpQueryInformation: OID=%x\n", Oid));

  switch(Oid)
  {
  case OID_GEN_SUPPORTED_LIST:
    pInfo = (PVOID) xmpNicSupportedOids;
    BytesAvailable = InfoLen = sizeof(xmpNicSupportedOids);
    break;
    
  case OID_GEN_HARDWARE_STATUS:
    pInfo = (PVOID) &HardwareStatus; /* TODO: Get from hal */
    BytesAvailable = InfoLen = sizeof(NDIS_HARDWARE_STATUS);
    break;

  case OID_GEN_MEDIA_SUPPORTED:
  case OID_GEN_MEDIA_IN_USE:
    pInfo = (PVOID) &Medium;
    BytesAvailable = InfoLen = sizeof(NDIS_MEDIUM);
    break;

  case OID_GEN_PHYSICAL_MEDIUM:
    pInfo = (PVOID) &PhysMedium;
    BytesAvailable = InfoLen = sizeof(NDIS_PHYSICAL_MEDIUM);
    break;

  case OID_GEN_CURRENT_LOOKAHEAD:
  case OID_GEN_MAXIMUM_LOOKAHEAD:
    InfoVal = (ULONG32) (pNicCtx->LookAheadSz ? 
                         pNicCtx->LookAheadSz : XMP_NIC_CURRENT_MTU(pNicCtx));
    XMPTRACE(XMP_DBG_REQ, ("xmpQueryInformation: Lookahead Sz=%d\n", InfoVal));
    break;   

  case OID_GEN_MAXIMUM_FRAME_SIZE:
    InfoVal = XMP_NIC_CURRENT_MTU(pNicCtx);
    XMPTRACE(XMP_DBG_REQ, ("xmpQueryInformation: Max Frame Sz=%d\n", InfoVal));
    break;

  case OID_GEN_MAXIMUM_TOTAL_SIZE:
  case OID_GEN_TRANSMIT_BLOCK_SIZE:
  case OID_GEN_RECEIVE_BLOCK_SIZE:
    InfoVal = pNicCtx->MaxFrameSz;
    XMPTRACE(XMP_DBG_REQ,
             ("xmpQueryInformation: Tx/Rx block/Max total Sz=%d\n", InfoVal));
    break;

  case OID_GEN_MAC_OPTIONS: 
    InfoVal = XMP_NIC_MAC_OPTIONS(pNicCtx);
    if ( pNicCtx->Flags & XMP_NIC_FLAGS_8021P )
    {
      InfoVal |= NDIS_MAC_OPTION_8021P_PRIORITY;
    }
    if ( pNicCtx->Flags & XMP_NIC_FLAGS_8021Q )
    {
      InfoVal |= NDIS_MAC_OPTION_8021Q_VLAN;
    }
    XMPTRACE(XMP_DBG_REQ,
             ("xmpQueryInformation: Mac Options=%lx: 802.1P=%s 802.1Q=%s\n",
              InfoVal,
              bit(InfoVal, NDIS_MAC_OPTION_8021P_PRIORITY) ? "YES" : "NO",
              bit(InfoVal, NDIS_MAC_OPTION_8021Q_VLAN) ? "YES" : "NO"));
    break;

  case OID_GEN_LINK_SPEED:
    InfoVal8 = XMP_NIC_MAX_SPEED(pNicCtx);
    XMPTRACE(XMP_DBG_REQ, ("xmpQueryInformation: Link speed=%ld\n", InfoVal8));
    pInfo = (PVOID) &InfoVal8;
    InfoLen = sizeof(InfoVal8);
    break;

  case OID_GEN_MEDIA_CONNECT_STATUS:
    XF_GET_SLOCK(&pNicCtx->lock);
     InfoVal =  bit(pNicCtx->State, XMP_NIC_STATE_NOLINK) ?
     NdisMediaStateDisconnected : NdisMediaStateConnected;
    pNicCtx->ReportedState = (InfoVal == NdisMediaStateConnected) ?
    MediaConnectStateConnected : MediaConnectStateDisconnected;
    XF_FREE_SLOCK(&pNicCtx->lock);
    XMPTRACE(XMP_DBG_REQ, ("xmpQueryInformation: Link Status=%s\n",
              InfoVal ==  NdisMediaStateDisconnected ? "DISCONN" : "Connect"));
    break;

  case OID_GEN_TRANSMIT_BUFFER_SPACE:
    InfoVal = XMP_NIC_GET_TX_BUF_SIZE(pNicCtx);
    XMPTRACE(XMP_DBG_REQ, ("xmpQueryInformation: Tx Buf Space=%d\n", InfoVal));
    break;

  case OID_GEN_RECEIVE_BUFFER_SPACE:
    InfoVal = XMP_NIC_GET_RX_BUF_SIZE(pNicCtx);
    XMPTRACE(XMP_DBG_REQ, ("xmpQueryInformation: Rx Buf Space=%d\n", InfoVal));
    break;

  case OID_GEN_VENDOR_DESCRIPTION:
    pInfo = VendorDesc;
    InfoLen = sizeof(VendorDesc);
    break;

  case OID_GEN_VENDOR_ID:
    InfoVal = XENAMP_VENDOR_ID;
    break;
    
  case OID_GEN_VENDOR_DRIVER_VERSION:
    InfoVal = (ULONG) VendorDriverVersion;
    break;

  case OID_GEN_DRIVER_VERSION:
    InfoVal2 = (USHORT) ((XENAMP_NDIS_MAJOR_VERSION << 8) |
                         XENAMP_NDIS_MINOR_VERSION);
    pInfo = (PVOID) &InfoVal2;
    InfoLen = sizeof(InfoVal2);
    break;

  case OID_OFFLOAD_ENCAPSULATION:

    if ( InformationBufferLength < sizeof(NDIS_OFFLOAD_ENCAPSULATION) )
    {
      BytesNeeded = sizeof(NDIS_OFFLOAD_ENCAPSULATION);
      Status = NDIS_STATUS_BUFFER_TOO_SHORT;
      break;
    }

    pInfo = (VOID *) &pNicCtx->OffloadEncapsulation;
    BytesAvailable = InfoLen = sizeof (NDIS_OFFLOAD_ENCAPSULATION);
    break;

  case OID_802_3_PERMANENT_ADDRESS:
   /* Should I store permanent address in the Nic Context too */
    pInfo =  XMP_NIC_PERMANENT_ADDRESS(pNicCtx);
    BytesAvailable = InfoLen = ETH_LENGTH_OF_ADDRESS;
    { UCHAR *pc = (UCHAR *) pInfo;
    XMPTRACE(XMP_DBG_REQ, ("xmpQueryInformation:PermAddr=%x%x%x%x%x%x\n",
                           pc[0], pc[1], pc[2], pc[3], pc[4], pc[5]));
    }
    break;

  case OID_802_3_CURRENT_ADDRESS:
    pInfo = XMP_NIC_CURRENT_ADDRESS(pNicCtx);
    BytesAvailable = InfoLen = ETH_LENGTH_OF_ADDRESS;
    { UCHAR *pc = (UCHAR *) pInfo;
    XMPTRACE(XMP_DBG_REQ, ("xmpQueryInformation:CurAddr=%x%x%x%x%x%x\n",
                           pc[0], pc[1], pc[2], pc[3], pc[4], pc[5]));
    }
    break;

  case OID_802_3_MAXIMUM_LIST_SIZE:
    InfoVal = XMP_NIC_MAX_MULTICAST_LIST_SIZE(pNicCtx);
    XMPTRACE(XMP_DBG_REQ, ("xmpQueryInformation: Max List Sz=%d\n", InfoVal));
    break;

  case OID_802_3_MULTICAST_LIST:
    pInfo  = pNicCtx->AddrList.MacAddr[XMP_HAL_MC_ADDRESS_START_OFFSET];
    BytesAvailable = InfoLen = pNicCtx->AddrList.McCnt * sizeof(xmpMacAddr_t);
    break;
    
  case OID_GEN_MAXIMUM_SEND_PACKETS:
   /* TBD: Need to correct it: I think it does not have to be fifo len, it's
    the no of packet descriptors in a single MiniportSendPackets Call i.e should
    */
    InfoVal = pNicCtx->TxFifo[0].CfgTxDCnt;
    XMPTRACE(XMP_DBG_REQ, ("xmpQueryInformation:Max Send Pkts=%d\n", InfoVal));
    break;

  case OID_GEN_XMIT_OK:
  case OID_GEN_RCV_OK:
  case OID_GEN_XMIT_ERROR:
  case OID_GEN_RCV_ERROR:
  case OID_GEN_RCV_NO_BUFFER:
  case OID_GEN_RCV_CRC_ERROR:
  case OID_GEN_TRANSMIT_QUEUE_LENGTH:
  case OID_802_3_XMIT_ONE_COLLISION:
  case OID_802_3_XMIT_MORE_COLLISIONS:
  case OID_802_3_XMIT_MAX_COLLISIONS:
  case OID_802_3_RCV_ERROR_ALIGNMENT:
  case OID_GEN_DIRECTED_FRAMES_XMIT:
  case OID_GEN_DIRECTED_FRAMES_RCV:
  case OID_GEN_MULTICAST_FRAMES_XMIT:
  case OID_GEN_MULTICAST_FRAMES_RCV:
  case OID_GEN_BROADCAST_FRAMES_XMIT:
  case OID_GEN_BROADCAST_FRAMES_RCV:
    if ( Oid == OID_GEN_RCV_OK )
      InfoVal8 = pNicCtx->RxInfo[0].Stats.RxCnt;
    else if ( Oid == OID_GEN_XMIT_OK )
      InfoVal8 = pNicCtx->TxFifo[0].Stats.TxNBLCount;
    else
      Status = xmpGetStatsCounters(pNicCtx, Oid, &InfoVal8);
    BytesNeeded = BytesAvailable = InfoLen = sizeof(InfoVal8);
    if ( Status == NDIS_STATUS_SUCCESS )
    {
      if ( InformationBufferLength < sizeof(ULONG) )
      {
        Status = NDIS_STATUS_BUFFER_TOO_SHORT;
        break;
      }
      InfoLen = MIN(InformationBufferLength, BytesAvailable);
      pInfo = &InfoVal8;
    }
    break;
  case OID_GEN_STATISTICS:
    if ( InformationBufferLength < sizeof(NDIS_STATISTICS_INFO) )
    {
      BytesNeeded = sizeof(NDIS_STATISTICS_INFO);
      Status = NDIS_STATUS_BUFFER_TOO_SHORT;
      break;
    }

    BytesWritten = sizeof(NDIS_STATISTICS_INFO);
    InfoLen = 0;                                           /* To avoid copy */
    Status = xmpGetStatsCounters(pNicCtx, Oid, (U64 *) InformationBuffer);
    break;

  case OID_GEN_RECEIVE_SCALE_CAPABILITIES:
    XMPTRACE(XMP_DBG_INFO, ("xmpQueryInformation: OID_GEN_RECEIVE_SCALE_CAPABILITIES\n"));
    if ( !(XMP_NIC_RSS_IN_CONFIG(pNicCtx)) )
    {
      Status = NDIS_STATUS_NOT_SUPPORTED;
      break;
    }

    if ( InformationBufferLength < sizeof(NDIS_RECEIVE_SCALE_CAPABILITIES) )
    {
      BytesNeeded = sizeof(NDIS_RECEIVE_SCALE_CAPABILITIES);
      Status = NDIS_STATUS_BUFFER_TOO_SHORT;
      break;
    }

    XENAMP_NDIS_OBJECT_INIT(&RssCapabilities.Header,
                            NDIS_OBJECT_TYPE_RSS_CAPABILITIES,
                            NDIS_RECEIVE_SCALE_CAPABILITIES_REVISION_1,
                            sizeof(NDIS_RECEIVE_SCALE_CAPABILITIES));
	  xmpNicInitRssCapabilities(pNicCtx,&RssCapabilities);

    pInfo = (VOID *) &RssCapabilities;
    InfoLen = sizeof (NDIS_RECEIVE_SCALE_CAPABILITIES);
    break;

  case OID_GEN_RECEIVE_SCALE_PARAMETERS:
    XMPTRACE(XMP_DBG_INFO, ("xmpQueryInformation: OID_GEN_RECEIVE_SCALE_PARAMETERS\n"));
    pInfo = (VOID *)&pNicCtx->ndisRssSet.params;
    InfoLen = pNicCtx->ndisRssSet.size;
	  break;

  case OID_GEN_INTERRUPT_MODERATION:
    XENAMP_NDIS_OBJECT_INIT(&IntrModr.Header,
                             NDIS_OBJECT_TYPE_DEFAULT,
                             NDIS_INTERRUPT_MODERATION_PARAMETERS_REVISION_1,
                             sizeof(NDIS_INTERRUPT_MODERATION_PARAMETERS));
    IntrModr.Flags = NDIS_INTERRUPT_MODERATION_CHANGE_NEEDS_RESET;
    IntrModr.InterruptModeration = NdisInterruptModerationEnabled; 
    pInfo = (PVOID) &IntrModr;
    InfoLen = sizeof(NDIS_INTERRUPT_MODERATION_PARAMETERS);
    break;

  default :
    Status = xmpDiagQueryInformation(pNicCtx, 
                                    Oid, 
                                    InformationBuffer, 
                                    InformationBufferLength,
                                    &InfoLen,
                                    &BytesNeeded);
    if(Status == NDIS_STATUS_SUCCESS)
      pInfo = (VOID *) InformationBuffer;
    break;
  }

  if ( Status == NDIS_STATUS_SUCCESS )
  {
    /*    BytesNeeded = BytesAvailable;*/
    if ( InfoLen <= InformationBufferLength )
    {
      if ( InfoLen )
      {
        BytesWritten = InfoLen;
        NdisMoveMemory(InformationBuffer, pInfo, InfoLen);
      }
    }
    else
    {
      BytesNeeded = InfoLen;
      Status = NDIS_STATUS_BUFFER_TOO_SHORT;
    }
  }

  NdisRequest->DATA.QUERY_INFORMATION.BytesWritten = BytesWritten;
  NdisRequest->DATA.QUERY_INFORMATION.BytesNeeded = BytesNeeded;
  XMPTRACE(XMP_DBG_REQ, ("<== xmpQueryInformation: Status=%x\n", Status));
  return Status;
}

/* 
 * xmpSetInformation
 *
 * Description:
 *   This is the handler for an OID set operation.
 *
 * Arguments:
 *
 *   MiniportAdapterContext  Pointer to the  xena_nic_t structure
 *   NdisRequest             Pointer to the  request to be processed
 *
 *  Return Value:
 *   
 *   NDIS_STATUS_SUCCESS
 *   NDIS_STATUS_NOT_SUPPORTED
 *   NDIS_STATUS_BUFFER_TOO_SHORT
 *
 *  IRQL: <=DISPATCH_LEVEL
 *
 *  Notes:
 */

NDIS_STATUS
xmpSetInformation(
  IN xmpNicCtx_t             *pNicCtx,
  IN PNDIS_OID_REQUEST       NdisRequest
  )
{
  NDIS_STATUS                 Status = NDIS_STATUS_SUCCESS;
  NDIS_OID                    Oid;
  PVOID                       InformationBuffer;
  ULONG                       InformationBufferLength;
  ULONG                       BytesRead;
  ULONG                       BytesNeeded;
  ULONG                       PacketFilter;
  ULONG                       MCastCnt;
  ULONG                       HashType;
  ULONG                       validBits;
  USHORT                      i;
  xge_hal_device_t           *pHalDev;
  NDIS_RECEIVE_SCALE_PARAMETERS *pRssParam;
  xmpSetRssContext			      SetRssCtx = {0};
  static ULONG setRssCount = 10;

  pHalDev = XMP_NIC_GET_DEV(pNicCtx);
  Oid = NdisRequest->DATA.SET_INFORMATION.Oid;
  InformationBuffer = NdisRequest->DATA.SET_INFORMATION.InformationBuffer;
  InformationBufferLength = 
    NdisRequest->DATA.SET_INFORMATION.InformationBufferLength;

  XMPTRACE(XMP_DBG_REQ, ("==> xmpSetInformation %x\n", Oid));
  BytesRead = 0;
  BytesNeeded = 0;

  switch(Oid)
  {
  case OID_802_3_MULTICAST_LIST:
    XMPTRACE(XMP_DBG_REQ, ("xmpSetInformation Multicast list\n"));
    if ( InformationBufferLength % ETH_LENGTH_OF_ADDRESS != 0 )
      return(NDIS_STATUS_INVALID_LENGTH);

    MCastCnt = InformationBufferLength / ETH_LENGTH_OF_ADDRESS;
    xmpHalMcastFilterClr(pNicCtx);
    Status = xmpHalSetMcastList(pNicCtx,
                                (UCHAR *) InformationBuffer,
                                &MCastCnt);
    BytesRead = MCastCnt * ETH_LENGTH_OF_ADDRESS;
    break;

  case OID_GEN_CURRENT_PACKET_FILTER:
    XMPTRACE(XMP_DBG_REQ, ("xmpSetInformation: packet filter\n"));
    if ( InformationBufferLength != sizeof (ULONG) )
      return ( NDIS_STATUS_INVALID_LENGTH );

    BytesRead = InformationBufferLength;
    PacketFilter = *((PULONG) InformationBuffer);
    Status = xmpSetFilter(pNicCtx, PacketFilter);
    break;

  case OID_GEN_CURRENT_LOOKAHEAD:
    XMPTRACE(XMP_DBG_REQ, ("xmpSetInformation: current lookahead\n"));
    if( InformationBufferLength < sizeof(ULONG) )
    {
      BytesNeeded = sizeof(ULONG);
      Status = NDIS_STATUS_INVALID_LENGTH;
      break;
    }
    pNicCtx->LookAheadSz = *((PULONG) InformationBuffer);
    BytesRead = sizeof(ULONG);
    Status = NDIS_STATUS_SUCCESS;
    break;

  case OID_GEN_RECEIVE_SCALE_PARAMETERS:

    XMPTRACE(XMP_DBG_INFO, ("xmpSetInformation: OID_GEN_RECEIVE_SCALE_PARAMETERS\n"));
    if ( !(XMP_NIC_RSS_IN_CONFIG(pNicCtx)) )
    {
      Status = NDIS_STATUS_NOT_SUPPORTED;
      break;
    }

    if ( InformationBufferLength < sizeof(NDIS_RECEIVE_SCALE_PARAMETERS) )
    {
      BytesNeeded = sizeof(NDIS_RECEIVE_SCALE_PARAMETERS);
      Status = NDIS_STATUS_INVALID_LENGTH;
      break;
    }
    pRssParam = (PNDIS_RECEIVE_SCALE_PARAMETERS)InformationBuffer;

#ifdef NDISTEST_BUG
    if ( !XENAMP_NDIS_OBJECT_VALID(&pRssParam->Header,
                                   NDIS_RECEIVE_SCALE_PARAMETERS_REVISION_1,
                                   NDIS_OBJECT_TYPE_RSS_PARAMETERS,
                                   sizeof(NDIS_RECEIVE_SCALE_PARAMETERS)) )
    {
      XMPTRACE(XMP_DBG_WRN, ("xmpSetInformation:RSS_PARAMETERS invalid!!"       
                             " Revision=%d Type=%d Size=%d\n",
                             pRssParam->Header.Revision,
                             pRssParam->Header.Type,
                             pRssParam->Header.Size));
      BytesRead = sizeof(NDIS_OBJECT_HEADER);
      Status = NDIS_STATUS_INVALID_PARAMETER;
      break;
    }
#endif

    BytesRead = sizeof(NDIS_RECEIVE_SCALE_PARAMETERS);

    if ( (InformationBufferLength < (pRssParam->IndirectionTableSize
                                     + pRssParam->IndirectionTableOffset)) ||
         (InformationBufferLength < (pRssParam->HashSecretKeySize
                                     + pRssParam->HashSecretKeyOffset)) )
    {
      BytesNeeded = sizeof(NDIS_RECEIVE_SCALE_PARAMETERS);
      Status = NDIS_STATUS_INVALID_LENGTH;
      break;
    }

    HashType = NDIS_RSS_HASH_TYPE_FROM_HASH_INFO(pRssParam->HashInformation);   
    /*if ( HashType != 0 || NDIS_RSS_HASH_FUNC_FROM_HASH_INFO(pRssParam->HashInformation) != 0) */
    validBits = (NDIS_HASH_IPV4 | NDIS_HASH_TCP_IPV4
#ifndef XMP_RSS_IPV4_ONLY
                 | NDIS_HASH_IPV6 | NDIS_HASH_TCP_IPV6
                 | NDIS_HASH_IPV6_EX | NDIS_HASH_TCP_IPV6_EX
#endif
                 );
    if ( ((NDIS_RSS_HASH_FUNC_FROM_HASH_INFO(pRssParam->HashInformation) !=
           NdisHashFunctionToeplitz) ||  /* the only hash function supported */
          !bit(HashType, validBits) ||    /* at least one type should be set */
          (HashType & ~validBits)) &&         /* no other type should be set */
         (!(bit(pRssParam->Flags, NDIS_RSS_PARAM_FLAG_DISABLE_RSS)) &&
          NDIS_RSS_HASH_FUNC_FROM_HASH_INFO(pRssParam->HashInformation) != 0) )
    {
      Status = NDIS_STATUS_INVALID_PARAMETER;
      break;
    }

    if ( /*(pRssParam->IndirectionTableSize < (1 << pRssParam->NumberOfLsbs)) || */
         (pRssParam->HashSecretKeySize & 0x7) ||    /* must be 8-bytes align */
         (pRssParam->HashSecretKeySize > 40)  ||      /* max secret key size */
         (pRssParam->BaseCpuNumber & (pRssParam->BaseCpuNumber - 1)) )/*pow2 */
    {
      XMPTRACE(XMP_DBG_WRN, ("xmpReqSetRss:Invalid settings: Key Size=%d"
                             " IDT size=%d BaseCpu=%d\n",
                             pRssParam->HashSecretKeySize,
                             pRssParam->IndirectionTableSize,
                             pRssParam->BaseCpuNumber));
      Status = NDIS_STATUS_INVALID_PARAMETER;
      break;
    }


    if ((pRssParam->IndirectionTableSize + pRssParam->IndirectionTableOffset) >
        (pRssParam->HashSecretKeySize + pRssParam->HashSecretKeyOffset))
      BytesRead = (pRssParam->IndirectionTableSize + 
                    pRssParam->IndirectionTableOffset);
    else
      BytesRead = (pRssParam->HashSecretKeySize +
                    pRssParam->HashSecretKeyOffset);

    SetRssCtx.pNicCtx = pNicCtx;
    SetRssCtx.pParams = pRssParam;

#if 0
    if(NdisMSynchronizeWithInterruptEx(pNicCtx->hInterrupt, 0, xmpReqSetRss, &SetRssCtx))
#endif
    if ( NDIS_RSS_HASH_FUNC_FROM_HASH_INFO(pRssParam->HashInformation) == 0 )
      bis(pRssParam->Flags, NDIS_RSS_PARAM_FLAG_DISABLE_RSS);

    if ( bit(pRssParam->Flags, NDIS_RSS_PARAM_FLAG_DISABLE_RSS) )
    {
      pNicCtx->ndisRssSet.size = sizeof(NDIS_RECEIVE_SCALE_PARAMETERS);
      pNicCtx->ndisRssSet.params.BaseCpuNumber          = 0;
      pNicCtx->ndisRssSet.params.Flags                  = 0;
      pNicCtx->ndisRssSet.params.HashInformation        = 0;
      pNicCtx->ndisRssSet.params.IndirectionTableSize   = 0;
      pNicCtx->ndisRssSet.params.IndirectionTableOffset = 0;
      pNicCtx->ndisRssSet.params.HashSecretKeySize      = 0;
      pNicCtx->ndisRssSet.params.HashSecretKeyOffset    = 0;
    }
    else
    {
      NdisMoveMemory(&pNicCtx->ndisRssSet.params, pRssParam, BytesRead);
      pNicCtx->ndisRssSet.size = BytesRead;
    }

      xmpReqSetRss(pNicCtx);
#if 0
    if ( !(setRssCount % 10) )
    {
      xmpReqSetRss(pNicCtx);
    }
    setRssCount++;
#endif
    Status = NDIS_STATUS_SUCCESS;
    break;

  case OID_OFFLOAD_ENCAPSULATION:
    XMPTRACE(XMP_DBG_OFLD, ("xmpSetInformation: offload encapsulation\n"));
    if ( InformationBufferLength < sizeof(NDIS_OFFLOAD_ENCAPSULATION) )
    {
      BytesNeeded = sizeof(NDIS_OFFLOAD_ENCAPSULATION);
      Status = NDIS_STATUS_INVALID_LENGTH;
      break;
    }

    NdisMoveMemory(&pNicCtx->OffloadEncapsulation,
                   InformationBuffer,
                   sizeof(NDIS_OFFLOAD_ENCAPSULATION));
    BytesRead = sizeof(NDIS_OFFLOAD_ENCAPSULATION);
    Status    = NDIS_STATUS_SUCCESS;
    break;

  case OID_GEN_INTERRUPT_MODERATION:
    {
      NDIS_INTERRUPT_MODERATION_PARAMETERS *pIntrModr;
      if ( InformationBufferLength < sizeof(NDIS_INTERRUPT_MODERATION_PARAMETERS) )
      {
        BytesNeeded = sizeof(NDIS_INTERRUPT_MODERATION_PARAMETERS);
        Status = NDIS_STATUS_INVALID_LENGTH;
        break;
      }
      pIntrModr  = (NDIS_INTERRUPT_MODERATION_PARAMETERS *) InformationBuffer;
      if ( !XENAMP_NDIS_OBJECT_VALID(&pIntrModr->Header,
                                 NDIS_INTERRUPT_MODERATION_PARAMETERS_REVISION_1,
                                 NDIS_OBJECT_TYPE_DEFAULT,
                                 sizeof(NDIS_INTERRUPT_MODERATION_PARAMETERS)) )
      {
        BytesRead = sizeof(NDIS_OBJECT_HEADER);
        Status = NDIS_STATUS_INVALID_DATA;
        break;
      }
     break;
    }

  case OID_TCP_OFFLOAD_PARAMETERS:
    {
      NDIS_OFFLOAD_PARAMETERS *pAdmin;
      NDIS_STATUS_INDICATION  StatusIndication;
      ULONG size;

      pAdmin  = (NDIS_OFFLOAD_PARAMETERS *) InformationBuffer;
  
      if ( InformationBufferLength < NDIS_SIZEOF_OFFLOAD_PARAMETERS_REVISION_1 )
      {
        BytesNeeded = NDIS_SIZEOF_OFFLOAD_PARAMETERS_REVISION_1;
        Status = NDIS_STATUS_INVALID_LENGTH;
        break;
      }

      if ( !XENAMP_NDIS_OBJECT_VALID(&pAdmin->Header,
                                     NDIS_OFFLOAD_PARAMETERS_REVISION_1,
                                     NDIS_OBJECT_TYPE_DEFAULT,
                                     NDIS_SIZEOF_OFFLOAD_PARAMETERS_REVISION_1 ))
      {
        BytesRead = sizeof(NDIS_OBJECT_HEADER);
        Status = NDIS_STATUS_INVALID_DATA;
        break;
      }

      XMP_NIC_SET_ADMIN_OFFLOADS(pNicCtx, pAdmin);
      xmpNicSetCkoFlags(pNicCtx);
      xmpNicInitOffloadCapabilities(pNicCtx, &pNicCtx->OffloadCapabilities);    
       /* TODO: Generate event */
      BytesRead = NDIS_SIZEOF_OFFLOAD_PARAMETERS_REVISION_1;
      Status    = NDIS_STATUS_SUCCESS;

      XENAMP_NDIS_OBJECT_INIT(&StatusIndication.Header,
         NDIS_OBJECT_TYPE_STATUS_INDICATION,
         NDIS_STATUS_INDICATION_REVISION_1,
         sizeof(NDIS_STATUS_INDICATION));

      StatusIndication.SourceHandle = pNicCtx->hMPAdapter;
      StatusIndication.PortNumber = 0;
      StatusIndication.StatusCode = NDIS_STATUS_TASK_OFFLOAD_CURRENT_CONFIG;
      StatusIndication.Flags = 0;
      StatusIndication.DestinationHandle = NULL;
      StatusIndication.RequestId = NdisRequest->RequestId;
      StatusIndication.StatusBuffer = &pNicCtx->OffloadCapabilities;
      StatusIndication.StatusBufferSize = sizeof(NDIS_OFFLOAD);
      NdisMIndicateStatusEx(pNicCtx->hMPAdapter, &StatusIndication);
    }
    break;
    
  case OID_GEN_HD_SPLIT_PARAMETERS:
     {
       NDIS_HD_SPLIT_PARAMETERS *pHdSplit;
    
       pHdSplit  = (NDIS_HD_SPLIT_PARAMETERS *) InformationBuffer;
       if ( InformationBufferLength < NDIS_SIZEOF_HD_SPLIT_PARAMETERS_REVISION_1 )
       {
         BytesNeeded = NDIS_SIZEOF_HD_SPLIT_PARAMETERS_REVISION_1;
         Status = NDIS_STATUS_INVALID_LENGTH;
         break;
       }

       if ( !XENAMP_NDIS_OBJECT_VALID(&pHdSplit->Header,
                                    NDIS_HD_SPLIT_PARAMETERS_REVISION_1,
                                    NDIS_OBJECT_TYPE_DEFAULT,                                     
                                    NDIS_SIZEOF_HD_SPLIT_PARAMETERS_REVISION_1 ))
       {
         BytesRead = sizeof(NDIS_OBJECT_HEADER);
         Status = NDIS_STATUS_INVALID_DATA;
         break;
       }
      
       if( bit(pHdSplit->HDSplitCombineFlags, NDIS_HD_SPLIT_COMBINE_ALL_HEADERS) ) 
         bis( pNicCtx->Flags, XMP_NIC_FLAGS_RX_SPLIT_COMBINE ); 
       else
         bic( pNicCtx->Flags, XMP_NIC_FLAGS_RX_SPLIT_COMBINE ); 
    }
    break;

  default:
    XMPTRACE(XMP_DBG_INFO, ("xmpSetInformation: unsupported OID=%lx\n", Oid));
    Status = xmpDiagSetInformation(
                                   pNicCtx, 
                                   Oid, 
                                   InformationBuffer, 
                                   InformationBufferLength,
                                   &BytesRead,
                                   &BytesNeeded);
    break;
  }

  NdisRequest->DATA.SET_INFORMATION.BytesRead = BytesRead;
  NdisRequest->DATA.SET_INFORMATION.BytesNeeded = BytesNeeded;

  XMPTRACE(XMP_DBG_REQ, ("<== xmpSetInformation: Status=%x\n", Status));
  return Status;
}
 

/* 
 * xmpMethodRequest
 *
 * Description:
 *   WMI Method Request Handler
 *
 * Arguments:
 *
 *   MiniportAdapterContext  Pointer to the adapter structure
 *   NdisRequest             Pointer to the query request

 *  Return Value:
 *   
 *   NDIS_STATUS_SUCCESS
 *   NDIS_STATUS_NOT_SUPPORTED
 *   NDIS_STATUS_BUFFER_TOO_SHORT
 *
 *  IRQL: <= DISPATCH_LEVEL
 *
 *  Notes:
 */
NDIS_STATUS
xmpMethodRequest(
  IN  xmpNicCtx_t             *pNicCtx,
  IN  PNDIS_OID_REQUEST       Request
  )
{
  NDIS_STATUS                 Status = NDIS_STATUS_NOT_SUPPORTED;

    /* Custom WMI Oids can be added later */
    
  return Status;
}


/* 
 * xmpGetStatsCounters
 *
 * Description:
 *   Get Statistics Counters
 *b
 * Arguments:
 *
 *   MiniportAdapterContext  Pointer to the adapter structure
 *   NdisRequest             Pointer to the query request
 *
 *  Return Value:
 *   
 *   NDIS_STATUS_SUCCESS
 *   NDIS_STATUS_NOT_SUPPORTED
 *   NDIS_STATUS_BUFFER_TOO_SHORT
 *
 *  IRQL: <= DISPATCH_LEVEL
 *
 *  Notes:
 */
NDIS_STATUS
xmpGetStatsCounters( 
  IN  xmpNicCtx_t            *pNicCtx,
  IN  NDIS_OID                Oid,
  OUT U64                    *pCounter )

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
  xge_hal_stats_hw_info_t     *stats;
  xge_hal_device_t            *pHalDev;
  xge_hal_status_e              halStatus;
  xmpOidStats_t               *swStats;
  NDIS_STATUS                  Status = NDIS_STATUS_SUCCESS;

  swStats = &pNicCtx->OidStats;
  pHalDev = XMP_NIC_GET_DEV(pNicCtx);
  *pCounter = 0;
  if ((halStatus = xge_hal_stats_hw(pHalDev, &stats))!=  XGE_HAL_OK)
  {
    XMPTRACE(XMP_DBG_REQ, ("xmpGetStatsCounters failed=%X\n", halStatus));
    return NDIS_STATUS_FAILURE;
  }

  ASSERT(stats);

  switch( Oid )
  {
  case OID_GEN_XMIT_OK:
    /*    *pCounter = stats->tmac_frms;*/
    *pCounter = (swStats->txDirFrames + swStats->txMcastFrames + 
                 swStats->txBcastFrames);
    break;

  case OID_GEN_RCV_OK:
    /* *pCounter = stats->rmac_vld_frms; */
    *pCounter = (swStats->rxDirFrames + swStats->rxMcastFrames + 
                 swStats->rxBcastFrames);
    break;

  case OID_GEN_XMIT_ERROR:
    /*    *pCounter = stats->tmac_drop_frms + stats->tmac_any_err_frms;*/
    *pCounter = swStats->txErrFrames;
    break;

  case OID_GEN_RCV_ERROR:
    /* *pCounter = stats->rmac_drop_frms;*/
    *pCounter = swStats->rxErrFrames;
    break;

  case OID_GEN_RCV_NO_BUFFER:
    *pCounter = stats->rmac_drop_frms;
    break;

  case OID_GEN_RCV_CRC_ERROR:
    *pCounter = stats->rmac_fcs_err_frms;
    break;

  case OID_GEN_RCV_DISCARDS:
    *pCounter = stats->rmac_discarded_frms;
    break;

  case OID_GEN_TRANSMIT_QUEUE_LENGTH:
    {
      UINT Queue;
      for ( Queue=0; Queue < pNicCtx->RegParams.TxFIFONum; Queue++ )
        *pCounter += pNicCtx->TxFifo[Queue].OsSendCnt;
    }
    break;

  case OID_802_3_XMIT_ONE_COLLISION:
  case OID_802_3_XMIT_MORE_COLLISIONS:
  case OID_802_3_XMIT_MAX_COLLISIONS:
    *pCounter = 0;
    break;

  case OID_802_3_RCV_ERROR_ALIGNMENT:
    *pCounter = stats->rmac_fcs_err_frms;
    break;

    case OID_GEN_DIRECTED_FRAMES_RCV:
    /* *pCounter =( stats->rmac_vld_frms - (stats->rmac_vld_mcst_frms + 
    stats->rmac_vld_bcst_frms) );*/
    *pCounter = swStats->rxDirFrames;
    break;

  case OID_GEN_DIRECTED_FRAMES_XMIT:
    /* *pCounter = stats->tmac_ucst_frms; */
    *pCounter = swStats->txDirFrames;
    break;

  case OID_GEN_MULTICAST_FRAMES_RCV:
    /* *pCounter = stats->rmac_vld_mcst_frms; */
    *pCounter = swStats->rxMcastFrames;
    break;

  case OID_GEN_MULTICAST_FRAMES_XMIT:
    /* *pCounter = stats->tmac_mcst_frms; */
    *pCounter = swStats->txMcastFrames;
    break;

 case OID_GEN_BROADCAST_FRAMES_RCV:
   /* *pCounter = stats->rmac_vld_bcst_frms; */
    *pCounter = swStats->rxBcastFrames;
    break;

  case OID_GEN_BROADCAST_FRAMES_XMIT:
    /* *pCounter = stats->tmac_bcst_frms;*/
    *pCounter = swStats->txBcastFrames;
    break;

  case OID_GEN_STATISTICS:
    {
      NDIS_STATISTICS_INFO *pStats = (NDIS_STATISTICS_INFO *)pCounter;
      XENAMP_NDIS_OBJECT_INIT(&pStats->Header,
                              NDIS_OBJECT_TYPE_DEFAULT,
                              NDIS_OBJECT_REVISION_1,
                              sizeof(NDIS_STATISTICS_INFO));
      pStats->SupportedStatistics = XMP_NIC_GEN_STAT_FLAGS(pNic);

      pStats->ifInDiscards = stats->rmac_drop_frms + 
                             stats->rmac_discarded_frms;

      pStats->ifInErrors = stats->rmac_drop_frms;

      //pStats->ifHCInOctets = stats->rmac_ttl_octets;
      pStats->ifHCInOctets = (swStats->rxDirBytes + swStats->rxMcastBytes +
                              swStats->rxBcastBytes);

      //pStats->ifHCInUcastPkts = stats->rmac_vld_frms;
      pStats->ifHCInUcastPkts = swStats->rxDirFrames;

      //pStats->ifHCInMulticastPkts =  stats->rmac_vld_mcst_frms;
      pStats->ifHCInMulticastPkts =  swStats->rxMcastFrames;

	    //pStats->ifHCInBroadcastPkts =  stats->rmac_vld_bcst_frms;
	    pStats->ifHCInBroadcastPkts =  swStats->rxBcastFrames;

      //pStats->ifHCOutOctets = stats->tmac_ttl_octets;
      pStats->ifHCOutOctets = (swStats->txDirBytes + swStats->txMcastBytes +
                              swStats->txBcastBytes);

      //pStats->ifHCOutUcastPkts = stats->tmac_ucst_frms;
      pStats->ifHCOutUcastPkts = swStats->txDirFrames;

      // pStats->ifHCOutMulticastPkts = stats->tmac_mcst_frms;
      pStats->ifHCOutMulticastPkts = swStats->txMcastFrames;

      //pStats->ifHCOutBroadcastPkts = stats->tmac_bcst_frms;
      pStats->ifHCOutBroadcastPkts = swStats->txBcastFrames;

      //pStats->ifOutErrors = stats->tmac_any_err_frms;
      pStats->ifOutErrors = swStats->txErrFrames;

      pStats->ifOutDiscards          = 0;
      pStats->ifHCInUcastOctets      = swStats->rxDirBytes;
      pStats->ifHCInMulticastOctets  = swStats->rxMcastBytes;
      pStats->ifHCInBroadcastOctets  = swStats->rxBcastBytes;
      pStats->ifHCOutUcastOctets     = swStats->txDirBytes;
      pStats->ifHCOutMulticastOctets = swStats->txMcastBytes;
      pStats->ifHCOutBroadcastOctets = swStats->txBcastBytes;
    }
    break;

  default:
    Status = NDIS_STATUS_NOT_SUPPORTED;
    break;
  }
  //DBGPRINT ( ( "<== MpGetStatsCounters\n" ) );
  return ( Status );
}

/* 
 * xmpSetFilter
 *
 * Description:
 *   Set Packet Filter
 *
 * Arguments:
 *
 *   MiniportAdapterContext  Pointer to the adapter structure
 *   PacketFilter            Filter Settings to be added to the NIC
 *
 *  Return Value:
 *   
 *   NDIS_STATUS_SUCCESS
 *   NDIS_STATUS_NOT_SUPPORTED
 *   NDIS_STATUS_BUFFER_TOO_SHORT
 *
 *  IRQL: <= DISPATCH_LEVEL
 *
 *  Notes:
 */
NDIS_STATUS
xmpSetFilter(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  ULONG                   PacketFilter
  )
{
  NDIS_STATUS                 Status = NDIS_STATUS_SUCCESS;
  xge_hal_device_t           *pHalDev;
  ULONG                       delta;
  UCHAR                       MacAddr[6]={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  pHalDev = XMP_NIC_GET_DEV(pNicCtx);
  XMPTRACE(XMP_DBG_REQ, ("xmpSetfilter:Given filter=%X\n", PacketFilter));
  do 
  { 
    if ( XMP_NIC_MATCH_PKT_FILTERS(pNicCtx, PacketFilter) )
    {
      Status = NDIS_STATUS_NOT_SUPPORTED;
      break;
    } 

    delta = PacketFilter ^ pNicCtx->PacketFilter;
    pNicCtx->PacketFilter = PacketFilter;

    if ( delta & NDIS_PACKET_TYPE_PROMISCUOUS )
    {
      if ( PacketFilter & NDIS_PACKET_TYPE_PROMISCUOUS )
        xge_hal_device_promisc_enable(pHalDev);
      else
        xge_hal_device_promisc_disable(pHalDev);
    }

    if ( delta & NDIS_PACKET_TYPE_DIRECTED )
    {
      if ( PacketFilter & NDIS_PACKET_TYPE_DIRECTED )
        xmpHalSetAddress(pNicCtx, pNicCtx->CurrentAddress);
      else
        xmpHalSetAddress(pNicCtx, MacAddr);
    }

    if ( delta & NDIS_PACKET_TYPE_BROADCAST )
    {
      if ( PacketFilter & NDIS_PACKET_TYPE_BROADCAST )
        xge_hal_device_bcast_enable(pHalDev);
      else
        xge_hal_device_bcast_disable(pHalDev);
    }

    if ( delta & NDIS_PACKET_TYPE_ALL_MULTICAST )
    {
      if ( PacketFilter & NDIS_PACKET_TYPE_ALL_MULTICAST )
        xge_hal_device_mcast_enable(pHalDev);
      else
        xge_hal_device_mcast_disable(pHalDev);
    }

    if ( delta & NDIS_PACKET_TYPE_MULTICAST )
    {
      if ( PacketFilter & NDIS_PACKET_TYPE_MULTICAST )
        xmpHalMcastFilterSet(pNicCtx);
      else
        xmpHalMcastFilterClr(pNicCtx);
    }
  } while(0);

  XMPTRACE(XMP_DBG_REQ, ("<== xmpSetfilter: Status=%x\n", Status));
  return Status;
}

BOOLEAN xmpReqSetRss(
  __in  NDIS_HANDLE         SynchronizeContext
  )
{
  USHORT                      i;
  UCHAR                      *pSecretKey;
  UCHAR                      *pTable;
  BOOLEAN                     bTable, bKey, bCpu, bHashInfo;
  BOOLEAN                     bQueue = FALSE;
  KIRQL                       OldIrql;
  xmpNicCtx_t                 *pNicCtx = SynchronizeContext;
  NDIS_RECEIVE_SCALE_PARAMETERS *pParams = &pNicCtx->ndisRssSet.params;
  static  int                 count = 0;

  XMPTRACE(XMP_DBG_WRN, ("==> xmpReqSetRss Count = %d Flags = 0x%8.8lx\n",
                         ++count, pParams->Flags));
  XF_GET_SLOCK(&pNicCtx->lock);

    pSecretKey = ((UCHAR *) pParams) + pParams->HashSecretKeyOffset;
    pTable = ((UCHAR *) pParams) + pParams->IndirectionTableOffset;
    bTable = bKey = bCpu = bHashInfo = FALSE;

    if ( XMP_NIC_RSS_IS_ENABLED(pNicCtx) &&
         !(bit(pParams->Flags, NDIS_RSS_PARAM_FLAG_DISABLE_RSS)) )
    {
      if ( !(bit(pParams->Flags, NDIS_RSS_PARAM_FLAG_HASH_KEY_UNCHANGED)) )
        bKey = TRUE;

      if ( !(bit(pParams->Flags, NDIS_RSS_PARAM_FLAG_ITABLE_UNCHANGED)) )
        bTable = TRUE;

       if ( !(bit(pParams->Flags, NDIS_RSS_PARAM_FLAG_BASE_CPU_UNCHANGED )) )
        bCpu = TRUE;

    }
    else
    {
      bTable = bKey = bCpu = bHashInfo = TRUE;
      pNicCtx->RssParamsReq.HashFunction = 0;
    }

    /* Enable RTH_CFG, RTS_ENHANCED and RMAC_STRIP_FCS to 0 */
    if ( bHashInfo )
    {
      pNicCtx->RssParamsReq.HashType               = 
        NDIS_RSS_HASH_TYPE_FROM_HASH_INFO(pParams->HashInformation);
      pNicCtx->RssParamsReq.HashFunction               = 
        NDIS_RSS_HASH_FUNC_FROM_HASH_INFO(pParams->HashInformation);
    }

    if ( NDIS_RSS_HASH_FUNC_FROM_HASH_INFO(pParams->HashInformation) != 0 )
    {
      if( bCpu )
        pNicCtx->RssParamsReq.BaseCpuNum = pParams->BaseCpuNumber;

      if ( bTable )
      {
        pNicCtx->RssParamsReq.TableSz      =
          pParams->IndirectionTableSize > XMP_NIC_RSS_MAX_TABLE_SZ ?
          XMP_NIC_RSS_MAX_TABLE_SZ : pParams->IndirectionTableSize;
        {
          ULONG TableSz =  pNicCtx->RssParamsReq.TableSz;
          USHORT NumLsbs = 0;
          while (TableSz != 1)
          {  
            TableSz = TableSz>>1;                                         
            NumLsbs++;
          } 
          pNicCtx->RssParamsReq.HashBitsSz = NumLsbs;
        }

        NdisMoveMemory(pNicCtx->RssParamsReq.Table, pTable,
                       pNicCtx->RssParamsReq.TableSz);
      }

      if ( bKey )
      {
        pNicCtx->RssParamsReq.SecretKeySz  = pParams->HashSecretKeySize;
        NdisMoveMemory(pNicCtx->RssParamsReq.SecretKey, pSecretKey,
                       pNicCtx->RssParamsReq.SecretKeySz);
      }
    }
    pNicCtx->RssParamsReq.Flags = pParams->Flags;

    KeMemoryBarrier();
    if ( !pNicCtx->wiQueued )
    {
      pNicCtx->wiQueued = TRUE;
      if ( !pNicCtx->wiRunning )
        bQueue = TRUE;
    }
  XF_FREE_SLOCK(&pNicCtx->lock);

#ifdef XMP_SET_RSS_IN_WORKER
  if ( bQueue )
    NdisQueueIoWorkItem(pNicCtx->hSetRssWI, 
                        (NDIS_IO_WORKITEM_ROUTINE)xmpNicSetRssParameters,
                        pNicCtx);
#else
    xmpNicSetRssParameters(pNicCtx, NULL);
#endif

  XMPTRACE(XMP_DBG_WRN, ("<== xmpReqSetRss\n"));
  return TRUE;
}


VOID
xmpTmpRssRegDump(
  xmpNicCtx_t                *pNicCtx
  )
{
  xge_hal_device_t           *hldev;
  xge_hal_pci_bar0_t         *bar0;
  U64 val64;
  UINT i;

  hldev = XMP_NIC_GET_DEV(pNicCtx);
  bar0 = (xge_hal_pci_bar0_t *)(void *)hldev->bar0;
	val64 = xge_os_pio_mem_read64(hldev->pdev, hldev->regh0, &bar0->rts_ctrl);
  DBGPRINT(("RTS_CTRL=0x%I64x\n", val64));
	val64 = xge_os_pio_mem_read64(hldev->pdev, hldev->regh0, &bar0->rts_rth_cfg);
  DBGPRINT(("RTS_RTH_CFG=0x%I64x\n", val64));
	val64 = xge_os_pio_mem_read64(hldev->pdev, hldev->regh0, &bar0->rts_default_q);
  DBGPRINT(("RTS_DEFAULT_Q=0x%I64x\n", val64));

  for (i=0; i < 5; i++)
  {
    val64 = xge_os_pio_mem_read64(hldev->pdev, hldev->regh0, &bar0->rts_rth_hash_mask[i]);
     DBGPRINT(("RTS_HASH_MASK[%d]=0x%I64x\n", i, val64));
  }

  for (i=0; i < 4; i++)
  {
    val64 = xge_os_pio_mem_read64(hldev->pdev, hldev->regh0, &bar0->prc_ctrl_n[i]);
    DBGPRINT(("PRC_CTRL[%d]=0x%I64x\n", i, val64));
  }
  
  for (i = 0; i < pNicCtx->RssParamsReq.TableSz; i++)
  {
   
		/* execute */
		val64 = ( XGE_HAL_RTS_RTH_MAP_MEM_CTRL_STROBE |
             XGE_HAL_RTS_RTH_MAP_MEM_CTRL_OFFSET(i));
		xge_os_pio_mem_write64(hldev->pdev, hldev->regh0, val64,
                           &bar0->rts_rth_map_mem_ctrl);

  
    
		/* poll until done */
		if (__hal_device_register_poll(hldev,
                                   &bar0->rts_rth_map_mem_ctrl, 0,
                                   XGE_HAL_RTS_RTH_MAP_MEM_CTRL_STROBE,
                                   XGE_HAL_DEVICE_CMDMEM_WAIT_MAX_MILLIS)
           != XGE_HAL_OK)
      /* FIXME: Return Error or what */;
    val64 = xge_os_pio_mem_read64(hldev->pdev, hldev->regh0, 
                                  &bar0->rts_rth_map_mem_data);
    DBGPRINT(("iTable[%d] = 0x%I64x\n", i, val64));
  }

}

NDIS_OID *
xmpReqGetSupportedOIDList()
{
	return xmpNicSupportedOids;
}

ULONG
xmpReqGetSupportedOIDListLength()
{
	return sizeof(xmpNicSupportedOids);
}

#pragma warning(pop)   // Save the current warning state

