#pragma warning(push)   // Save the current warning state
#pragma warning(disable:4100) //unreference parameter in functions
#pragma warning(disable:4101)  //unreference local variables
#pragma warning(disable:4189)  //local variable is initialized but not referenced
#pragma warning( disable : 4127 )
#pragma warning( disable : 4706 )
/*
   NAME
     xenamp.c

   DESCRIPTION
     This module implements Ndis miniport handlers.
*/

#include "precomp.h"

/* ============================================================================
 * Globals definitions
 * ==========================================================================*/
NDIS_HANDLE     xmpDriverContext = NULL;
NDIS_HANDLE     xmpDriverHandle = NULL;
xmpSpinLock_t   xmpLock;                                      /* Global Lock */
xmpQLink_t      xmpNicsList;                  /* Global list of all the nics */
U32             xmpDebugLevel = XMP_DBGDEF;

#ifdef XENAMP_TRACING
#include "xenamp.tmh"
PDRIVER_OBJECT  xmpDriverObject;
#endif

/* ============================================================================
 * Function prototypes
 * ==========================================================================*/
NDIS_STATUS
DriverEntry(
  IN  PDRIVER_OBJECT          pDriverObject,
  IN  PUNICODE_STRING         pRegistryPath
  );

MINIPORT_UNLOAD xenaMpUnload;

MINIPORT_SET_OPTIONS xenaMpSetOptions;

MINIPORT_DEVICE_PNP_EVENT_NOTIFY xenaMpPnPEvent;

MINIPORT_INITIALIZE xenaMpInitialize;

MINIPORT_HALT xenaMpHalt;

MINIPORT_RESET xenaMpReset;

MINIPORT_PAUSE xenaMpPause;

MINIPORT_RESTART xenaMpRestart;

MINIPORT_SHUTDOWN xenaMpShutdown;

MINIPORT_CHECK_FOR_HANG xenaMpCheckForHang;

MINIPORT_SEND_NET_BUFFER_LISTS xenaMpSendNetBufferLists;

MINIPORT_CANCEL_SEND xenaMpCancelSendNetBufferLists;

MINIPORT_ISR xenaMpIsr;

MINIPORT_INTERRUPT_DPC xenaMpIntrDpc;

MINIPORT_DISABLE_INTERRUPT xenaMpIntrDisable;

MINIPORT_ENABLE_INTERRUPT xenaMpIntrEnable;

MINIPORT_MESSAGE_INTERRUPT xenaMpMSIsr;

MINIPORT_MESSAGE_INTERRUPT_DPC xenaMpMSIDpc;

MINIPORT_DISABLE_MESSAGE_INTERRUPT xenaMpMSIDisable;

MINIPORT_ENABLE_MESSAGE_INTERRUPT xenaMpMSIEnable;

NDIS_IO_WORKITEM_FUNCTION xmpPauseHandler;

NDIS_IO_WORKITEM_FUNCTION xmpResetHandler;

#if MSI_X_ENABLE

MINIPORT_ADD_DEVICE xenaMpAddDevice;

MINIPORT_REMOVE_DEVICE xenaMpRemoveDevice;

MINIPORT_FILTER_RESOURCE_REQUIREMENTS xenaMpFilterResources;

#endif                                                       /* MSI_X_ENABLE */

NDIS_STATUS
xenaMpSetHdrSplitAttribs(
  IN xmpNicCtx_t             *pNicCtx
  );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#endif

__inline
VOID xg_flush_bus(
  IN xmpNicCtx_t *pNicCtx
  )
{
  xge_hal_device_t   *pHalDev = XMP_NIC_GET_DEV(pNicCtx);
  xge_hal_pci_bar0_t *isrbar0 = (xge_hal_pci_bar0_t *)pHalDev->isrbar0;
  volatile U64 * const addr = &isrbar0->general_int_status;
    
#if defined(_M_IA64)
  *addr;
#elif defined(_M_AMD64)
  KeMemoryBarrier();
  *addr;
#else //x86
  *addr;
#endif
}



/* ============================================================================
 * Functions definition
 * ==========================================================================*/

/* 
 * DriverEntry
 *
 * Description:
 *   Driver entry point which is the first routine to be called at driver
 *   initialization.
 *
 * Arguments:
 *   pDriverObject    -   pointer to the driver object
 *   pRegistryPath    -   pointer to the driver registry path
 *
 * Return Value:
 *   NDIS_STATUS_SUCCESS is returned in 'Status' if all initialization is
 *   successful, NDIS_STATUS_XXX error code if not.
 *
 * Callers:
 *   called by OS
 *
 * IRQL:
 *   PASSIVE_LEVEL
 *
 * See Also:
 *   NDIS WDK help for more information.
 *
 */ 
NDIS_STATUS
DriverEntry(
  IN  PDRIVER_OBJECT          pDriverObject,
  IN  PUNICODE_STRING         pRegistryPath
  )
{
  NDIS_STATUS                 Status;
  NDIS_MINIPORT_DRIVER_CHARACTERISTICS MPChar = {0};
  XMPTRACE(XMP_DBG_INI, ("==> DriverEntry\n"));

  /* Initialize driver globals.                                              */
  XF_Q_INIT(&xmpNicsList);                          /* global adapters queue */
  XF_INIT_SLOCK(&xmpLock);                               /* global spin lock */


  if ( xmpHalInit() )
  {
    XMPTRACE(XMP_DBG_ERR, ("DriverEntry: xmpHalInit Failed\n"));
    return NDIS_STATUS_FAILURE;
  }
  
  /* Object header.                                                          */
  MPChar.Header.Type = NDIS_OBJECT_TYPE_MINIPORT_DRIVER_CHARACTERISTICS;
  MPChar.Header.Revision = NDIS_MINIPORT_DRIVER_CHARACTERISTICS_REVISION_2;
  MPChar.Header.Size = NDIS_SIZEOF_MINIPORT_DRIVER_CHARACTERISTICS_REVISION_2;

  /* NDIS and driver version information.                                    */
  MPChar.MajorNdisVersion  = NDIS_MINIPORT_MAJOR_VERSION;
  MPChar.MinorNdisVersion  = NDIS_MINIPORT_MINOR_VERSION;
  MPChar.MajorDriverVersion = XENAMP_MAJOR_DRIVER_VERSION;
  MPChar.MinorDriverVersion = XENAMP_MINOR_DRIVER_VERSION;

  
  MPChar.SetOptionsHandler = xenaMpSetOptions; /* register optional services */
  MPChar.InitializeHandlerEx = xenaMpInitialize;      /* Miniport Initialize */
  MPChar.HaltHandlerEx = xenaMpHalt;                        /* Miniport halt */
  MPChar.UnloadHandler = xenaMpUnload;                      /* Driver unload */
  MPChar.PauseHandler  = xenaMpPause;                   /* Pause nic for I/O */
  MPChar.RestartHandler = xenaMpRestart;               /* Restart paused nic */
  MPChar.OidRequestHandler = xenaMpRequest;          /* NDIS_REQUEST handler */
  MPChar.SendNetBufferListsHandler = xenaMpSendNetBufferLists;   /* send NBL */
  MPChar.ReturnNetBufferListsHandler = xenaMpReturnNetBufferLists;/* return NBL */
  MPChar.CancelSendHandler = xenaMpCancelSendNetBufferLists;  /* Cancel send */
  MPChar.DevicePnPEventNotifyHandler = xenaMpPnPEvent;         /* PnP Events */
  MPChar.ShutdownHandlerEx = xenaMpShutdown;              /* System shutdown */
  MPChar.CheckForHangHandlerEx = xenaMpCheckForHang;       /* Check for hang */
  MPChar.ResetHandlerEx = xenaMpReset;                          /* nic reset */
  MPChar.CancelOidRequestHandler = xenaMpCancelRequest;
  Status = NdisMRegisterMiniportDriver(pDriverObject,
                                       pRegistryPath,
                                       xmpDriverContext,
                                       &MPChar,
                                       &xmpDriverHandle);
  if ( Status != NDIS_STATUS_SUCCESS )
  {
    XMPTRACE(XMP_DBG_ERR, ("DriverEntry: Failed=%x\n", Status));
     xmpHalDeInit();
  }

#ifdef XENAMP_TRACING
  WPP_INIT_TRACING(pDriverObject, pRegistryPath);
  xmpDriverObject = pDriverObject;
  DoTraceMessage(XMP_DBG_INI , "==> DriverEntry, Status=0x%x\n", Status); 
#endif  

  XMPTRACE(XMP_DBG_INI, ("<== DriverEntry: Status=%x\n", Status));
  return Status;
}

/* 
 * xenaMpUnload
 *
 * Description:
 *   Driver unload handler. By this time all the adapters must have been
 * halted.
 *
 * Arguments:
 *   pDriverObject    -   pointer to the driver object
 *
 * Return Value:
 *   void
 *
 * Callers:
 *   called by OS
 *
 * IRQL:
 *   PASSIVE_LEVEL
 *
 * See Also:
 *   NDIS WDK help for more information...
 *
 */ 
VOID 
xenaMpUnload(
  IN  DRIVER_OBJECT          *pDriverObject
  )
{
  UNREFERENCED_PARAMETER(pDriverObject);
  XMPTRACE(XMP_DBG_INI, ("==> xenaMpUnload\n"));

  NdisMDeregisterMiniportDriver(xmpDriverHandle);
  xmpHalDeInit();
#ifdef XENAMP_TRACING
  WPP_CLEANUP(xmpDriverObject);
#endif
  XMPTRACE(XMP_DBG_INI, ("<== xenaMpUnload\n"));
}

/* 
 * xenaMpSetOptions
 *
 * Description:
 *   This function is for registration of optional services with NDIS.
 *
 * Arguments:
 *   hDriver          - same driver handle that was returned by
 *                      NdisMResgisterMiniport
 *   hDriverConext    - same Driver context handle that was passed to 
 *                      NdisMResgisterMiniport
 *
 * Return Value:
 *   NDIS_STATUS_SUCCESS is returned in 'Status' if all initialization is
 *   successful, NDIS_STATUS_XXX error code if not.
 *
 * Callers:
 *   called by OS
 *
 * IRQL:
 *   PASSIVE_LEVEL
 *
 * See Also:
 *   NDIS WDK help for more information...
 */ 
NDIS_STATUS
xenaMpSetOptions(
  IN NDIS_HANDLE              hDriver,
  IN NDIS_HANDLE              hDriverContext
  )
{
#if MSI_X_ENABLE 
    NDIS_MINIPORT_PNP_CHARACTERISTICS          MsiChars;
#endif
  NDIS_STATUS               Status = NDIS_STATUS_SUCCESS;
  UNREFERENCED_PARAMETER(hDriver);
  UNREFERENCED_PARAMETER(hDriverContext);

  XMPTRACE(XMP_DBG_INI, ("==> xenaMpSetOptions\n"));

#if MSI_X_ENABLE    
    NdisZeroMemory(&MsiChars, sizeof(NDIS_MINIPORT_PNP_CHARACTERISTICS));
    /*
     * Here we don't set StartDevice handler because we don't add any resources
     * in xenaMpFilterResourceRequirement.
     */
    MsiChars.Header.Type = NDIS_OBJECT_TYPE_MINIPORT_PNP_CHARACTERISTICS;
    MsiChars.Header.Revision = NDIS_MINIPORT_PNP_CHARACTERISTICS_REVISION_1;
    MsiChars.Header.Size = sizeof(NDIS_MINIPORT_PNP_CHARACTERISTICS);
    MsiChars.MiniportAddDeviceHandler = xenaMpAddDevice;
    MsiChars.MiniportRemoveDeviceHandler = xenaMpRemoveDevice;
    MsiChars.MiniportFilterResourceRequirementsHandler = xenaMpFilterResources;

    Status =
      NdisSetOptionalHandlers(hDriver,
                              ((PNDIS_DRIVER_OPTIONAL_HANDLERS)&MsiChars));
    if (Status != NDIS_STATUS_SUCCESS)
    {
        ASSERT(FALSE);
    }

#endif

  /*  NdisSetOptionalHandlers(hDriver, &MPOptionalHandlers);*/
  XMPTRACE(XMP_DBG_INI, ("<== xenaMPSetOptions\n"));

  return (NDIS_STATUS_SUCCESS);
}

#if MSI_X_ENABLE
NDIS_STATUS
xenaMpAddDevice(
  IN NDIS_HANDLE           NdisMiniportHandle,
  IN NDIS_HANDLE           MiniportDriverContext
  )
{
  xmpAddNicCtx_t            *devCtx;
  NDIS_MINIPORT_BLOCK       *NdisHandle;
  NDIS_STATUS                Status = NDIS_STATUS_SUCCESS;
  NDIS_MINIPORT_ADD_DEVICE_REGISTRATION_ATTRIBUTES attribs = {0};

  XMPTRACE(XMP_DBG_INI, ("==> xenaMpAddDevice\n"));

  NdisHandle =  (PNDIS_MINIPORT_BLOCK)NdisMiniportHandle;
  do
  {
    devCtx = (xmpAddNicCtx_t *)XF_MALLOC(NdisHandle, sizeof(xmpAddNicCtx_t));
    if (devCtx == NULL)
    {
      Status = NDIS_STATUS_RESOURCES;
      break;
    }

    devCtx->NdisMiniportHandle = NdisHandle;
    attribs.Header.Type =
      NDIS_OBJECT_TYPE_MINIPORT_ADD_DEVICE_REGISTRATION_ATTRIBUTES;
    attribs.Header.Revision = 1;
    attribs.Header.Size =
      sizeof(NDIS_MINIPORT_ADD_DEVICE_REGISTRATION_ATTRIBUTES);
    attribs.MiniportAddDeviceContext = devCtx;
    
    Status =
      NdisMSetMiniportAttributes(NdisHandle,
                                 (NDIS_MINIPORT_ADAPTER_ATTRIBUTES *)&attribs);
    if (Status != NDIS_STATUS_SUCCESS)
      break;

    if (xmpNicReadIntrMode(NdisMiniportHandle, &devCtx->IntrMode) 
        == NDIS_STATUS_SUCCESS)
    {
       XMPTRACE(XMP_DBG_INFO, ("AddDevice-Registry: intr_mode%d\n", 
                               devCtx->IntrMode));
    }
    else
    {
      /*TODO Log in Event viewer                                           */
      break;
    }
  } while(0);

  if ( (Status != NDIS_STATUS_SUCCESS) && devCtx )
  {
    NdisFreeMemory(devCtx, 0, 0);
  }

  XMPTRACE(XMP_DBG_INI, ("<== xenaMpAddDevice: Status=%x\n", Status));
  return Status;
}

VOID
xenaMpRemoveDevice(
  IN NDIS_HANDLE          AddDeviceContext
  )
{
  NdisFreeMemory(AddDeviceContext,0,0);
}


NDIS_STATUS
xenaMpFilterResources(
  IN NDIS_HANDLE              AddDeviceContext,
  IN PIRP                     pIrp
  )
{
  IO_RESOURCE_REQUIREMENTS_LIST  *pOldList;
  IO_RESOURCE_REQUIREMENTS_LIST  *pNewList;
  xmpAddNicCtx_t             *pDevCtx = (xmpAddNicCtx_t *)AddDeviceContext;
  IO_STACK_LOCATION          *pIrpStack;
  IO_RESOURCE_LIST           *pIoResList;
  IO_RESOURCE_DESCRIPTOR     *pIoResDesc;
  ULONG                       i;
  ULONG                       j;
  ULONG                       cpunum  = 0;
  NDIS_HANDLE                 hAdapter;
  NTSTATUS                    Status = NDIS_STATUS_SUCCESS;
  KAFFINITY                   TargetCpu;
  NDIS_SYSTEM_PROCESSOR_INFO  *cpuInfo = NULL;
  ULONG                       IntrMode;
 
  XMPTRACE(XMP_DBG_INI, ("==> xenaMpFilterResources\n"));

  do 
  {
    ASSERT(pDevCtx);
    ASSERT(pDevCtx->NdisMiniportHandle);
    /* This routine can be called multiple times.                              */
    hAdapter = pDevCtx->NdisMiniportHandle;
    IntrMode = pDevCtx->IntrMode;
    NdisZeroMemory(pDevCtx, sizeof(xmpAddNicCtx_t)); 

    pDevCtx->NdisMiniportHandle = hAdapter;
    cpuInfo = (NDIS_SYSTEM_PROCESSOR_INFO *)
     XF_MALLOC(hAdapter,
               sizeof(NDIS_SYSTEM_PROCESSOR_INFO) + 
               MAXIMUM_PROCESSORS * sizeof(UCHAR));
    if ( !cpuInfo )
    {
      XMPTRACE(XMP_DBG_ERR, ("xenaMPFilterResources: XF_MALLOC failed\n"));
      return NDIS_STATUS_RESOURCES;
    }
    XF_MCLEAR(cpuInfo, sizeof(NDIS_SYSTEM_PROCESSOR_INFO));
    cpuInfo->RssProcessors = (UCHAR *)(cpuInfo + 1);
    cpuInfo->Flags = 0;
    XENAMP_NDIS_OBJECT_INIT
                (&cpuInfo->Header,
                 NDIS_OBJECT_TYPE_DEFAULT,
                 NDIS_SYSTEM_PROCESSOR_INFO_REVISION_1,
                 NDIS_SIZEOF_SYSTEM_PROCESSOR_INFO_REVISION_1 ); 
    
    if ( (Status = NdisGetProcessorInformation(cpuInfo))
       == NDIS_STATUS_SUCCESS )
    {
      pDevCtx->RssCpuCount = cpuInfo->RssCpuCount;
      pDevCtx->RssBaseCpu  = cpuInfo->RssBaseCpu;
      NdisMoveMemory(pDevCtx->RssProcessors,
                     cpuInfo->RssProcessors,
                     cpuInfo->RssCpuCount); 
    
     for ( i=0; i < cpuInfo->RssCpuCount; i++ )
     {
       pDevCtx->PhyCpuRssProcMap[cpuInfo->RssProcessors[i]] = (UCHAR) i;
       XMPTRACE(XMP_DBG_INI, ("\tRSSCPU %d\n", cpuInfo->RssProcessors[i]));
       XMPTRACE(XMP_DBG_INI, ("\tCPUNumber %d\n", 
                                cpuInfo->CpuInfo[i].CpuNumber));
     }

    }
    else
    {
      XMPTRACE(XMP_DBG_ERR, ("xenaMPFilterResources: "
                           "NdisGetProcessorInformation failed=%x\n",
                           Status));
      Status = NDIS_STATUS_FAILURE;
      break;
    }
    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

  /* DDK says to use List in IoStatus block but if for some reason it
   * isn't there then try the list in the Irpstack.
   */
    if ( pIrp->IoStatus.Information == 0 )
    {
      pOldList = (pIrpStack->Parameters.FilterResourceRequirements.
                  IoResourceRequirementList);
    
      if ( pOldList == NULL )
      {
        XMPTRACE(XMP_DBG_INI, ("<== xenaMpFilterResources: NULL List\n"));
        break;                        /* NULL List, nothing to be done */
      }
    }
    else
    {
      pOldList = (IO_RESOURCE_REQUIREMENTS_LIST *)pIrp->IoStatus.Information;
    }

    pIoResList = pOldList->List;                      /* ptr to our old list */

#if DBG
      for ( i = 0, j = 0; i < pIoResList->Count; i++ )
      {
        pIoResDesc = &pIoResList->Descriptors[i];
        XMPTRACE(XMP_DBG_INI, ("xenaMpFilterResources: type=%d opt=%d Flag=%d \n",
                               pIoResDesc->Type,
                               pIoResDesc->Option, pIoResDesc->Flags) );
      }       
#endif

    if ( IntrMode == XGE_HAL_INTR_MODE_IRQLINE )
    { /* need to remove MSI-X resources.                                     */
      for ( i = 0, j = 0; i < pIoResList->Count; i++ )
      {
        pIoResDesc = &pIoResList->Descriptors[i];
        if ( (pIoResDesc->Type == CmResourceTypeInterrupt) &&
             (pIoResDesc->Flags & CM_RESOURCE_INTERRUPT_MESSAGE) )
          continue;
        
        if ( pIoResDesc->Type == CmResourceTypeInterrupt )
        {
          pIoResDesc->Option = 0;
        }

        if ( i != j )
        {
          NdisMoveMemory(&pIoResList->Descriptors[j],
                         pIoResDesc, 
                         sizeof(IO_RESOURCE_DESCRIPTOR));
        }
        j++;
      }
      pIoResList->Count = j;

#if DBG
      XMPTRACE(XMP_DBG_INI, ("xenaMpFilterReources: Modified list is:\n"));
      for ( i = 0, j = 0; i < pIoResList->Count; i++ )
      {
        pIoResDesc = &pIoResList->Descriptors[i];
        XMPTRACE(XMP_DBG_INI, ("xenaMpFilterReources: type=%d opt=%d Flag=%d \n",
                               pIoResDesc->Type,
                               pIoResDesc->Option, pIoResDesc->Flags) );
      }       
#endif
      break;
    }

    /* ActiveProcessors = KeQueryActiveProcessors(); */
    j = 0;          /* start from the 0th index into the RssProcessors array */
    TargetCpu = 1;   /* start from the 1st processor i.e., bit0 in KAFFINITY */
    for ( i = 0; i < pIoResList->Count; i++ )
    {
      pIoResDesc = &pIoResList->Descriptors[i];
      if ( pIoResDesc->Type == CmResourceTypeInterrupt )
      {
        if ( (pIoResDesc->Option == 0) && /*(pIoResDesc->Option == 1) &&*/
             bit(pIoResDesc->Flags, CM_RESOURCE_INTERRUPT_LATCHED) &&
             bit(pIoResDesc->Flags, CM_RESOURCE_INTERRUPT_MESSAGE) )
        {
          ASSERT( pIoResDesc->ShareDisposition == 
                  CmResourceShareDeviceExclusive );
          /* Set interrupt policy.                                           */
          pIoResDesc->Flags |= CM_RESOURCE_INTERRUPT_POLICY_INCLUDED;
          pIoResDesc->u.Interrupt.AffinityPolicy =
            IrqPolicySpecifiedProcessors;
          /* Assign messages to different processors accordingly.
           *  - 1st message is reserved for alarm interrupts
           *  - subsequent messages are set to have affinity to RSS processors
           *  - tx fifo(s) processing is also done on RSS processors
           */
          if ( pDevCtx->MsiMsgCount )             /* skip the 1st for alaram */
          {
            TargetCpu = AFFINITY_MASK(pDevCtx->RssProcessors[j]);
            cpunum = j;
            j = (j+1) % pDevCtx->RssCpuCount;
          }
          pIoResDesc->u.Interrupt.TargetedProcessors = TargetCpu;
          pDevCtx->MsiCpuMap[pDevCtx->MsiMsgCount++] = cpunum;
        }
      }
    }
  } while(0);

   pDevCtx->FilterResourcesDone = TRUE;
   XMPTRACE(XMP_DBG_INI, ("xenaMpFilterResources: %d MSI messages found\n",
                         pDevCtx->MsiMsgCount));
   if ( cpuInfo )
     NdisFreeMemory(cpuInfo,0,0);

   XMPTRACE(XMP_DBG_INI, ("<== xenaMpFilterResources: status=%d\n", Status));
   return Status;
}
#endif                                                       /* MSI_X_ENABLE */ 


/* 
 * xenaMpPnPEvent
 *
 * Description:
 *   PnP event handler
 *
 * Arguments:
 *   pAdapterContext  - pointer to xena adapter for which the event has been
 *                      received. Some events are system events for which the
 *                      adapter context is not meaningful such as
 *                      'NdisDevicePnPEventPowerProfileChanged'
 *   PnPEvent         - PnP event to handle
 *   pInfoBuf         - Event specific information buffer pointer
 *   InfoBufLen       - Information buffer length
 *
 * Return Value:
 *   void
 *
 * Callers:
 *   NDIS
 *
 * IRQL:
 *   PASSIVE_LEVEL
 *
 * See Also:
 *   NDIS 6.0 DDK help for more information...
 */ 
VOID 
xenaMpPnPEvent(
  IN  NDIS_HANDLE             pAdapterContext,
  IN  PNET_DEVICE_PNP_EVENT   pNetDevicePnPEvent
  )
{
  xmpNicCtx_t                *pNicCtx;

  pNicCtx = (xmpNicCtx_t *) pAdapterContext;
  XMPTRACE(XMP_DBG_INI, ("==> xenaMpPnPEvent\n"));
  switch (pNetDevicePnPEvent->DevicePnPEvent)
  {
  case NdisDevicePnPEventQueryRemoved:
    XMPTRACE(XMP_DBG_INFO, ("NdisDevicePnPEventQueryRemoved\n"));
    break;

  case NdisDevicePnPEventRemoved:
    XMPTRACE(XMP_DBG_INFO, ("NdisDevicePnPEventRemoved\n"));
    break;       

  case NdisDevicePnPEventSurpriseRemoved:
    /* Set the state of the adapter to be removed to reject all requests (send
     * and OIDs) with status NDIS_STATUS_NOT_ACCEPTED until the adapter is
     * removed by xenaMpHalt. Also, cancel any pending sends, etc.
     * BTW, is surprise removal permissible for XENA???
     */
    XMPTRACE(XMP_DBG_INFO, ("NdisDevicePnPEventSurpriseRemoved\n"));
    break;

  case NdisDevicePnPEventQueryStopped:
    XMPTRACE(XMP_DBG_INFO, ("NdisDevicePnPEventQueryStopped\n"));
    break;

  case NdisDevicePnPEventStopped:
    XMPTRACE(XMP_DBG_INFO, ("NdisDevicePnPEventStopped\n"));
    break;      
            
  case NdisDevicePnPEventPowerProfileChanged:
    XMPTRACE(XMP_DBG_INFO, ("NdisDevicePnPEventPowerProfileChanged\n"));
    break;      
            
  default:
    XMPTRACE(XMP_DBG_INFO, ("Unknown\n"));
    break;         
  }
  XMPTRACE(XMP_DBG_INI, ("<== xenaMpPnPEvent\n"));
}

/* 
 * xenaMpInitialize
 *
 * Description:
 *   Miniport Initialize handler: Create a new miniport adapter
 *
 * Arguments:
 *   hAdapter         - Miniport adapter handle used by ndis
 *   hDriverContext   - Driver Context Handle
 *   InitParameters   - Miniport initialization parameters
 *
 * Return Value:
 *   NDIS_STATUS_SUCCESS or NDIS_STATUS_XXXX
 *
 * Callers:
 *   NDIS
 *
 * IRQL:
 *   PASSIVE_LEVEL
 *
 * See Also:
 *   NDIS 6.0 DDK help for more information...
 */ 
NDIS_STATUS
xenaMpInitialize(
  IN  NDIS_HANDLE             hAdapter,
  IN  NDIS_HANDLE             hDriverContext,
  IN  PNDIS_MINIPORT_INIT_PARAMETERS InitParam
  )
{
  UINT                        i;
  NDIS_STATUS                 Status;
  xmpNicCtx_t                *pNicCtx = (xmpNicCtx_t *)0;
  NDIS_PHYSICAL_ADDRESS       phyAddr;
  NDIS_MINIPORT_ADAPTER_ATTRIBUTES MPAttribs = {0};
  NDIS_MINIPORT_INTERRUPT_CHARACTERISTICS IntrInfo = {0};
  NDIS_RECEIVE_SCALE_CAPABILITIES RssCapabilities = {0};
  NDIS_ERROR_CODE ErrorStatus;
  xmpAddNicCtx_t  *pDevCtx = 
                (xmpAddNicCtx_t *)(InitParam->MiniportAddDeviceContext);

  XMPTRACE(XMP_DBG_INI, ("==>xenaMpInitialize: hAdapter %p\n", hAdapter));

  ASSERT( pDevCtx ); 
  do
  {
    /* Check the object header to make sure ndis is sane.                    */
    if ( !XENAMP_NDIS_OBJECT_VALID(&InitParam->Header,
                                   NDIS_MINIPORT_INIT_PARAMETERS_REVISION_1,
                                   NDIS_OBJECT_TYPE_MINIPORT_INIT_PARAMETERS,
                                   sizeof(NDIS_MINIPORT_INIT_PARAMETERS)) ) 
    {
      Status = NDIS_STATUS_FAILURE;
      break;
    }

    /* Allocate a logical adapter representing the nic.                      */
    if ( !(pNicCtx = xmpNicAlloc(hAdapter)) )
    {
      Status = NDIS_STATUS_RESOURCES;
      ErrorStatus =  NDIS_ERROR_CODE_OUT_OF_RESOURCES;
      NdisWriteErrorLogEntry(hAdapter,(NDIS_ERROR_CODE) ErrorStatus, 0);
      break;
    }

    pNicCtx->devCtx = pDevCtx;
    /* Read the registry paramaters.                                         */
    if ( (Status = xmpNicReadConfig(pNicCtx)) != NDIS_STATUS_SUCCESS )
    {
      Status = NDIS_STATUS_NOT_ACCEPTED;
      ErrorStatus = NDIS_ERROR_CODE_MISSING_CONFIGURATION_PARAMETER;
      NdisWriteErrorLogEntry(hAdapter,(NDIS_ERROR_CODE) ErrorStatus, 0);
      break;
    }

    XENAMP_NDIS_OBJECT_INIT
      (&MPAttribs.RegistrationAttributes.Header,
       NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES,
       NDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES_REVISION_1,
       sizeof(NDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES));
      MPAttribs.RegistrationAttributes.MiniportAdapterContext =
      (NDIS_HANDLE) pNicCtx;
    MPAttribs.RegistrationAttributes.AttributeFlags = XENAMP_NDIS_ATTRIBUTES;
    MPAttribs.RegistrationAttributes.CheckForHangTimeInSeconds =
      XENAMP_CHECK_FOR_HANG_TO;
    MPAttribs.RegistrationAttributes.InterfaceType = XENAMP_INTERFACE_TYPE;  

    /* Need to tell our characterstics to ndis before requesting any resources.
     * Also, this is where association of ndis MP adapter and our nic context
     * is done.
     */
    if ( (Status =
          NdisMSetMiniportAttributes(hAdapter,
                                     &MPAttribs)) != NDIS_STATUS_SUCCESS )
      break;

    /* Look for our PCI device, read PCI config, enable bus master, and request
     * PCI resources.
     */
    if ( (Status = xmpNicLookup(pNicCtx, InitParam->AllocatedResources))
         != NDIS_STATUS_SUCCESS )
    {
       ErrorStatus = NDIS_ERROR_CODE_ADAPTER_NOT_FOUND;
       NdisWriteErrorLogEntry(hAdapter, (NDIS_ERROR_CODE) ErrorStatus, 0);
       break;
    } 

    if ( (Status = xenaMpSetHdrSplitAttribs(pNicCtx)) != NDIS_STATUS_SUCCESS )
      break;

    if ( (Status = xmpNicTxRxAlloc(pNicCtx)) != NDIS_STATUS_SUCCESS )
    {
      Status = NDIS_STATUS_RESOURCES;
      ErrorStatus =  NDIS_ERROR_CODE_OUT_OF_RESOURCES;
      NdisWriteErrorLogEntry(hAdapter,(NDIS_ERROR_CODE) ErrorStatus, 0);
      break;
    }
  
    /* Map bus-relative registers to virtual system-space */
    phyAddr.QuadPart = pNicCtx->deviceMem1Phy;
    Status = NdisMMapIoSpace((VOID **) &pNicCtx->deviceMem1,
                             pNicCtx->hMPAdapter,
                             phyAddr, pNicCtx->deviceMem1Len);

    if ( Status != NDIS_STATUS_SUCCESS )
    {
      XMPTRACE(XMP_DBG_ERR, ("NdisMMapIoSpace failed for BAR1\n"));
      ErrorStatus = NDIS_ERROR_CODE_BAD_IO_BASE_ADDRESS;
      NdisWriteErrorLogEntry(hAdapter, (NDIS_ERROR_CODE) ErrorStatus, 0);
       break;
    }
    XMPTRACE(XMP_DBG_INFO, ("BAR1 mapped @ %p\n", pNicCtx->deviceMem1));

    phyAddr.QuadPart = pNicCtx->deviceMem2Phy;
    Status = NdisMMapIoSpace((VOID **) &pNicCtx->deviceMem2,
                             pNicCtx->hMPAdapter,
                             phyAddr, pNicCtx->deviceMem2Len);

    if ( Status != NDIS_STATUS_SUCCESS )
    {
      XMPTRACE(XMP_DBG_ERR, ("NdisMMapIoSpace failed for BAR2\n"));
      ErrorStatus = NDIS_ERROR_CODE_BAD_IO_BASE_ADDRESS;
      NdisWriteErrorLogEntry(hAdapter,(NDIS_ERROR_CODE) ErrorStatus,  0);
 
      break;
    }
    XMPTRACE(XMP_DBG_INFO, ("BAR2 mapped @ %p\n", pNicCtx->deviceMem2));

    if ( pNicCtx->PciCfg.DeviceID == XENAMP_PCI_DEVICE_ID_HERC )
    {
      phyAddr.QuadPart = pNicCtx->deviceMem3Phy;
      Status = NdisMMapIoSpace((VOID **) &pNicCtx->deviceMem3,
                               pNicCtx->hMPAdapter,
                               phyAddr, pNicCtx->deviceMem3Len);

      if ( Status != NDIS_STATUS_SUCCESS )
      {
        XMPTRACE(XMP_DBG_ERR, ("NdisMMapIoSpace failed for BAR3\n"));
        ErrorStatus = NDIS_ERROR_CODE_BAD_IO_BASE_ADDRESS;
        NdisWriteErrorLogEntry(hAdapter,(NDIS_ERROR_CODE) ErrorStatus,  0);
        break;
      }
      XMPTRACE(XMP_DBG_INFO, ("BAR3 mapped @ %p\n", pNicCtx->deviceMem3));
    }
    /* If NO message interrupt Resources allocated, the intr_mode should be 
       line based */

    if ( !pDevCtx->MsiMsgCount )
    {
      pNicCtx->RegParams.intr_mode = 0;
    }

    /* Initialize the device.                                                */
    if ( (Status = xmpHalNicInit(pNicCtx)) != NDIS_STATUS_SUCCESS )
    {
      XMPTRACE(XMP_DBG_ERR, ("xmpNicDeviceInit failed\n"));
      ErrorStatus = NDIS_ERROR_CODE_HARDWARE_FAILURE;
      NdisWriteErrorLogEntry(hAdapter, (NDIS_ERROR_CODE)ErrorStatus, 0);
      break;
    }
    bis(pNicCtx->Flags, XMP_NIC_FLAGS_HAL_NIC_INIT);
    ASSERT( KeGetCurrentIrql() == PASSIVE_LEVEL );
    xmpHalReadPermanentAddress(pNicCtx, pNicCtx->PermanentAddress);

   /* Set Current Address */
    if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_USE_CURRENT_ADDR) )
    {
      if ( xmpHalSetAddress(pNicCtx, &pNicCtx->CurrentAddress[0])
          != NDIS_STATUS_SUCCESS)
      {
        XMPTRACE(XMP_DBG_ERR, ("xmpHalSetMacAddress failed\n"));
        Status = NDIS_STATUS_FAILURE;
        break;
      }
    }
    else
    { 
      ETH_COPY_NETWORK_ADDRESS(pNicCtx->CurrentAddress,
                               pNicCtx->PermanentAddress);
    }
    XF_MCLEAR(&MPAttribs, sizeof(NDIS_MINIPORT_ADAPTER_ATTRIBUTES));

    XENAMP_NDIS_OBJECT_INIT
      (&MPAttribs.GeneralAttributes.Header,
       NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES,
       NDIS_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES_REVISION_1,
       sizeof(NDIS_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES));

    MPAttribs.GeneralAttributes.MediaType = XENAMP_NDIS_MEDIA_TYPE;
    MPAttribs.GeneralAttributes.PhysicalMediumType = NdisPhysicalMedium802_3;
    MPAttribs.GeneralAttributes.MtuSize = XMP_NIC_CURRENT_MTU(pNicCtx);
    MPAttribs.GeneralAttributes.MaxXmitLinkSpeed = XMP_NIC_MAX_SPEED(pNicCtx);
    MPAttribs.GeneralAttributes.MaxRcvLinkSpeed = XMP_NIC_MAX_SPEED(pNicCtx);
    MPAttribs.GeneralAttributes.XmitLinkSpeed = XMP_NIC_MAX_SPEED(pNicCtx);
    MPAttribs.GeneralAttributes.RcvLinkSpeed = XMP_NIC_MAX_SPEED(pNicCtx);
    MPAttribs.GeneralAttributes.MediaConnectState = 
      ((xmpHalGetLinkStatus(pNicCtx) == NDIS_STATUS_SUCCESS) ?
       MediaConnectStateConnected : MediaConnectStateDisconnected);
    MPAttribs.GeneralAttributes.MediaDuplexState = MediaDuplexStateFull;
    MPAttribs.GeneralAttributes.LookaheadSize = XMP_NIC_CURRENT_MTU(pNicCtx);
    MPAttribs.GeneralAttributes.PowerManagementCapabilities = NULL;
    MPAttribs.GeneralAttributes.MacOptions =  XMP_NIC_MAC_OPTIONS(pNicCtx);
    MPAttribs.GeneralAttributes.SupportedPacketFilters = 
      XMP_NIC_PKT_FILTERS(pNicCtx);
    MPAttribs.GeneralAttributes.MaxMulticastListSize =
      XMP_NIC_MAX_MULTICAST_LIST_SIZE(pNicCtx);
    MPAttribs.GeneralAttributes.MacAddressLength = ETH_LENGTH_OF_ADDRESS;
    NdisMoveMemory(MPAttribs.GeneralAttributes.PermanentMacAddress,
                   XMP_NIC_PERMANENT_ADDRESS(pNicCtx),
                   ETH_LENGTH_OF_ADDRESS);
    NdisMoveMemory(MPAttribs.GeneralAttributes.CurrentMacAddress,
                   XMP_NIC_CURRENT_ADDRESS(pNicCtx),
                   ETH_LENGTH_OF_ADDRESS);

    /* Set RSS Capabilities.                                                 */
    if ( XMP_NIC_RSS_IN_CONFIG(pNicCtx) )
    {
      XENAMP_NDIS_OBJECT_INIT
        (&RssCapabilities.Header,
         NDIS_OBJECT_TYPE_RSS_CAPABILITIES,
         NDIS_RECEIVE_SCALE_CAPABILITIES_REVISION_1,
         sizeof(NDIS_RECEIVE_SCALE_CAPABILITIES));
      xmpNicInitRssCapabilities(pNicCtx,&RssCapabilities);
      MPAttribs.GeneralAttributes.RecvScaleCapabilities = &RssCapabilities;
    }

    MPAttribs.GeneralAttributes.AccessType  = NET_IF_ACCESS_BROADCAST;
    MPAttribs.GeneralAttributes.DirectionType = NET_IF_DIRECTION_SENDRECEIVE;
    MPAttribs.GeneralAttributes.ConnectionType = NET_IF_CONNECTION_DEDICATED;
    MPAttribs.GeneralAttributes.IfType = IF_TYPE_ETHERNET_CSMACD;
    MPAttribs.GeneralAttributes.IfConnectorPresent = TRUE;
    MPAttribs.GeneralAttributes.SupportedStatistics = 
      XMP_NIC_STATS_SUPPORTED(pNicCtx);
    MPAttribs.GeneralAttributes.SupportedOidList = xmpReqGetSupportedOIDList();
    MPAttribs.GeneralAttributes.SupportedOidListLength =
      xmpReqGetSupportedOIDListLength();


    /* Tell our characterstics to ndis before requesting any resources.      */
    if ( (Status =
          NdisMSetMiniportAttributes(hAdapter,
                                     &MPAttribs)) != NDIS_STATUS_SUCCESS )
      break;

    XF_MCLEAR(&MPAttribs, sizeof(NDIS_MINIPORT_ADAPTER_ATTRIBUTES));

    XENAMP_NDIS_OBJECT_INIT
      (&MPAttribs.OffloadAttributes.Header,
       NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_OFFLOAD_ATTRIBUTES,
       NDIS_MINIPORT_ADAPTER_OFFLOAD_ATTRIBUTES_REVISION_1,
       sizeof(NDIS_MINIPORT_ADAPTER_OFFLOAD_ATTRIBUTES));

    MPAttribs.OffloadAttributes.DefaultOffloadConfiguration = &pNicCtx->OffloadCapabilities;
    MPAttribs.OffloadAttributes.HardwareOffloadCapabilities = &pNicCtx->HwOffloadCapabilities;

    XENAMP_NDIS_OBJECT_INIT
      (&pNicCtx->TcpConnectionCapabilities.Header,
       NDIS_OBJECT_TYPE_DEFAULT,
       NDIS_TCP_CONNECTION_OFFLOAD_REVISION_1,
       sizeof(NDIS_TCP_CONNECTION_OFFLOAD));
     pNicCtx->TcpConnectionCapabilities.Encapsulation = NDIS_ENCAPSULATION_NOT_SUPPORTED ;
     MPAttribs.OffloadAttributes.DefaultTcpConnectionOffloadConfiguration = &pNicCtx->TcpConnectionCapabilities;

    XENAMP_NDIS_OBJECT_INIT
      (&pNicCtx->TcpConnectionHardwareCapabilities.Header,
       NDIS_OBJECT_TYPE_DEFAULT,
       NDIS_TCP_CONNECTION_OFFLOAD_REVISION_1,
       sizeof(NDIS_TCP_CONNECTION_OFFLOAD));
    pNicCtx->TcpConnectionHardwareCapabilities.Encapsulation = NDIS_ENCAPSULATION_NOT_SUPPORTED ;
    MPAttribs.OffloadAttributes.TcpConnectionOffloadHardwareCapabilities = &pNicCtx->TcpConnectionHardwareCapabilities;

    if ( (Status =
          NdisMSetMiniportAttributes(hAdapter,
                                     &MPAttribs)) != NDIS_STATUS_SUCCESS )
      break;

    XENAMP_NDIS_OBJECT_INIT
      (&IntrInfo.Header,
       NDIS_OBJECT_TYPE_MINIPORT_INTERRUPT,
       NDIS_MINIPORT_INTERRUPT_REVISION_1,
       sizeof(NDIS_MINIPORT_INTERRUPT_CHARACTERISTICS));
    IntrInfo.InterruptHandler               = xenaMpIsr;
    IntrInfo.InterruptDpcHandler            = xenaMpIntrDpc;
    IntrInfo.DisableInterruptHandler        = xenaMpIntrDisable;
    IntrInfo.EnableInterruptHandler         = xenaMpIntrEnable;

    if ( pNicCtx->devCtx->MsiMsgCount )
    {
      IntrInfo.MsiSupported                   = TRUE;
      IntrInfo.MsiSyncWithAllMessages         = TRUE;
      IntrInfo.MessageInterruptHandler        = xenaMpMSIsr;
      IntrInfo.MessageInterruptDpcHandler     = xenaMpMSIDpc;
      IntrInfo.DisableMessageInterruptHandler = xenaMpMSIDisable;
      IntrInfo.EnableMessageInterruptHandler  = xenaMpMSIEnable;
    }
    else
    {
      IntrInfo.MsiSupported                   = FALSE;
      IntrInfo.MsiSyncWithAllMessages         = FALSE;
      IntrInfo.MessageInterruptHandler        = NULL;
      IntrInfo.MessageInterruptDpcHandler     = NULL;
      IntrInfo.DisableMessageInterruptHandler = NULL;
      IntrInfo.EnableMessageInterruptHandler  = NULL;
    }
    if ( (Status = NdisMRegisterInterruptEx(pNicCtx->hMPAdapter,
                                            pNicCtx,
                                            &IntrInfo,
                                            &pNicCtx->hInterrupt))
         != NDIS_STATUS_SUCCESS )
    {
      XMPTRACE(XMP_DBG_ERR, ("NdisMRegisterInterruptEx failed\n"));
      ErrorStatus = NDIS_ERROR_CODE_INTERRUPT_CONNECT;
      NdisWriteErrorLogEntry(hAdapter,(NDIS_ERROR_CODE) ErrorStatus, 0);
      break;
    }

    bis(pNicCtx->Flags, XMP_NIC_FLAGS_INTR_REG);
    pNicCtx->InterruptType = IntrInfo.InterruptType;
    pNicCtx->IntrMsgInfo   = IntrInfo.MessageInfoTable;

    xmpNicSetMPState(pNicCtx, XMP_NIC_MPSTATE_PAUSING, FALSE);
    xmpNicSetMPState(pNicCtx, XMP_NIC_MPSTATE_PAUSED, FALSE);

    /* Post Initialization                                                   */
    /* xmpNicSetUtilParams(pNic);                                            */

    if ( (Status = xmpHalNicStart(pNicCtx)) != NDIS_STATUS_SUCCESS )
    {
      ErrorStatus = NDIS_ERROR_CODE_ADAPTER_DISABLED;
      NdisWriteErrorLogEntry(hAdapter,(NDIS_ERROR_CODE) ErrorStatus, 0);
      Status = NDIS_STATUS_FAILURE;
      break;
    }

    /* Check what MSI intr resources we have been granted                    */
    if ( pNicCtx->InterruptType == NDIS_CONNECT_MESSAGE_BASED )
    {
#ifdef DBG
      {
        IO_INTERRUPT_MESSAGE_INFO *pTable = pNicCtx->IntrMsgInfo;
        IO_INTERRUPT_MESSAGE_INFO_ENTRY *entry;
        ULONG mi;
        XMPTRACE(XMP_DBG_INFO, ("MSIs granted: Total = %d\n",
                                pTable->MessageCount));
        for ( mi=0, entry = pTable->MessageInfo;
              mi < pTable->MessageCount;
              mi++, entry++ )
        {
          XMPTRACE(XMP_DBG_INFO, ("[%d] MessageData=%x ProcessorSet=%x\n",
                                  mi,
                                  entry->MessageData,
                                  entry->TargetProcessorSet));
        }
      }
#endif
      XMP_NIC_MSI_ENABLE(pNicCtx);
      if ( pNicCtx->IntrMsgInfo->MessageCount < pNicCtx->RegParams.RxRingNum )  
      {
        XMPTRACE(XMP_DBG_WRN, ("MessageCnt is less than RxRingNum!!!\n"));
        Status = NDIS_STATUS_RESOURCES;
        break;
       /* TODO: reduce number of rx rings if did not get one msi per ring!!! */
       /* Fail Initialization for now. */
      }
      else
      {
        int msiId;
        /* MSI 0 is reserved for Alarms.                                     */

        /* set MSI-X for Rings.                                              */
        for (i=0, msiId=1; i < pNicCtx->RegParams.RxRingNum; i++, msiId++)
        {
          pNicCtx->RxInfo[i].MsiId = msiId;
          xge_hal_channel_msix_set(pNicCtx->RxInfo[i].halChannel, msiId);
        }
        pNicCtx->IntrMsiRxLo = 1;
        pNicCtx->IntrMsiRxHi = (pNicCtx->IntrMsiRxLo + 
                                pNicCtx->RegParams.RxRingNum - 1);

        /* set MSI-X for Fifos.                                              */
        for (i=0, msiId=1; i < pNicCtx->RegParams.TxFIFONum; i++, msiId++)     
        {
          xge_hal_channel_msix_set(pNicCtx->TxFifo[i].halChannel, msiId);
        }
        pNicCtx->IntrMsiTxLo = 1;
        pNicCtx->IntrMsiTxHi = (pNicCtx->IntrMsiTxLo + 
                                pNicCtx->RegParams.TxFIFONum - 1);
      }
    }
    else if ( XMP_NIC_MSI_IN_CONFIG(pNicCtx) )
    {
      XMPTRACE(XMP_DBG_WRN, ("MSIs NOT granted!!!\n"));
    }
    else
    {
      XMPTRACE(XMP_DBG_INFO, ("MSIs not enabled!\n"));
    }

    for (i=0; i < pNicCtx->RegParams.RxRingNum; i++)
    {
      pNicCtx->RxInfo[i].RssCpu = pDevCtx->RssProcessors[i];
    }

    xmpHalEnableInterrupts(pNicCtx);

    if ( xmpHalGetLinkStatus(pNicCtx) == NDIS_STATUS_SUCCESS )
      xmpNicIndicateStatus(pNicCtx, TRUE);
    else
      bis(pNicCtx->State, XMP_NIC_STATE_NOLINK);
  
    xmpNicStartTimer(pNicCtx);
 
    Status = NDIS_STATUS_SUCCESS;
    XMPTRACE(XMP_DBG_INFO, ("xenaMpInitialize: SUCCESS\n"));
  } while (0);

  if ( (Status != NDIS_STATUS_SUCCESS) && pNicCtx ){
      XMP_NIC_DEREFERENCE(pNicCtx); }

  XMPTRACE(XMP_DBG_INI, ("<== xenaMpInitialize: hMPAdapter=%p Status=%x\n",
                          hAdapter, Status));

  return Status;
}

VOID
xenaMpHalt(
  IN  NDIS_HANDLE             MiniportAdapterContext,
  IN  NDIS_HALT_ACTION        HaltAction
  )
{
  xmpNicCtx_t                *pNicCtx;
  xmpNicTxFifo_t             *pTxFifo;
  xmpNicRxInfo_t             *pRxRing;
  UINT                        ui;
  NDIS_STATUS                 Status = NDIS_STATUS_PENDING;

  XMPTRACE(XMP_DBG_TRM, ("==> xenaMpHalt\n"));

  pNicCtx = (xmpNicCtx_t *) MiniportAdapterContext;
  ASSERT(pNicCtx);

  XMPTRACE(XMP_DBG_INFO, ("xenaMpHalt: RefCount=%d\n", pNicCtx->RefCnt));

  xmpNicSetMPState(pNicCtx, XMP_NIC_MPSTATE_HALT, FALSE);
  xmpHalDisableInterrupts(pNicCtx);
 
  xmpNicFreeTimer(pNicCtx);

  KeMemoryBarrier();
  while ( pNicCtx->wiQueued || pNicCtx->wiRunning )
  {
    XMPTRACE(XMP_DBG_INFO, ("xmpResetHandler: Wait for set RSS worker\n"));
    xenaMpSleep(100 * 1000);
    KeMemoryBarrier();
  }

  if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_INTR_REG) )
  {
    NdisMDeregisterInterruptEx(pNicCtx->hInterrupt);
    bic(pNicCtx->Flags, XMP_NIC_FLAGS_INTR_REG);
  }

  for (ui = 0; ui < pNicCtx->RegParams.TxFIFONum; ui++)
  {
    pTxFifo = &(pNicCtx->TxFifo[ui]);
    ASSERT(XF_Q_IS_NULL(&pTxFifo->NBLWaitQ));
  }

  for (ui = 0; ui < pNicCtx->RegParams.RxRingNum; ui++)
  {
    pRxRing = &pNicCtx->RxInfo[ui];
    ASSERT(pRxRing->UlpNBLCnt == 0);
  }

  xmpHalNicReset(pNicCtx);

  XMP_NIC_DEREFERENCE(pNicCtx);

  XMPTRACE(XMP_DBG_TRM, ("<== xenaMpHalt\n"));
}

NDIS_STATUS 
xenaMpReset(
  IN  NDIS_HANDLE             MiniportAdapterContext,
  OUT PBOOLEAN                AddressingReset
  )
{
  xmpNicCtx_t                *pNicCtx;
  NDIS_STATUS                 Status = NDIS_STATUS_PENDING;

  XMPTRACE(XMP_DBG_WRN, ("==> xenaMpReset: nic %p\n",
                         MiniportAdapterContext));
  pNicCtx = (xmpNicCtx_t *) MiniportAdapterContext;
  ASSERT(pNicCtx);

  do
  {
    XF_GET_SLOCK(&pNicCtx->lock);
      if ( pNicCtx->mpState == XMP_NIC_MPSTATE_RESET )
      {
        XMPTRACE(XMP_DBG_WRN, ("xenaMpReset:nic %p:Reset already in prog.\n",
                               pNicCtx));
        Status = NDIS_STATUS_RESET_IN_PROGRESS;
        XF_FREE_SLOCK(&pNicCtx->lock);
        break;
      }
    XF_FREE_SLOCK(&pNicCtx->lock);
    xmpNicSetMPState(pNicCtx, XMP_NIC_MPSTATE_RESET, FALSE);
    pNicCtx->NeedsNicReset = FALSE;
   
    *AddressingReset = TRUE;
    xmpNicIndicateStatus(pNicCtx, FALSE);
    NdisQueueIoWorkItem(pNicCtx->hResetWorkItem,
                        (NDIS_IO_WORKITEM_ROUTINE)xmpResetHandler,
                        pNicCtx);
  } while(0);

  XMPTRACE(XMP_DBG_WRN, ("<== xenaMpReset: nic %p\n", pNicCtx));
  return Status;
}

NDIS_STATUS 
xenaMpPause(
  IN  NDIS_HANDLE             MiniportAdapterContext,    
  IN  PNDIS_MINIPORT_PAUSE_PARAMETERS MiniportPauseParameters
  )
{
  xmpNicCtx_t                *pNicCtx;

  XMPTRACE(XMP_DBG_WRN, ("==> xenaMpPause: nic %p\n",
                         MiniportAdapterContext));

  pNicCtx = (xmpNicCtx_t *) MiniportAdapterContext;
  ASSERT(pNicCtx);

  /* ASSERT(pNicCtx->mpState == XMP_NIC_MPSTATE_RUNNING); */

  xmpNicSetMPState(pNicCtx, XMP_NIC_MPSTATE_PAUSING, FALSE);
  NdisQueueIoWorkItem(pNicCtx->hWorkItem, (NDIS_IO_WORKITEM_ROUTINE) xmpPauseHandler, pNicCtx);

  XMPTRACE(XMP_DBG_WRN, ("<== xenaMpPause: nic %p\n", pNicCtx));
  return NDIS_STATUS_PENDING;
}

NDIS_STATUS 
xenaMpRestart(
  IN  NDIS_HANDLE             MiniportAdapterContext,    
  IN  PNDIS_MINIPORT_RESTART_PARAMETERS MiniportRestartParameters
  )
{
  xmpNicCtx_t                *pNicCtx;
  xmpNicTxFifo_t             *pTxFifo;
  xmpNicRxInfo_t             *pRxRing;
  UINT                        ui;
  NDIS_STATUS                 Status = NDIS_STATUS_PENDING;

  XMPTRACE(XMP_DBG_WRN, ("==> xenaMpRestart: nic %p\n",
                         MiniportAdapterContext));

  pNicCtx = (xmpNicCtx_t *) MiniportAdapterContext;
  ASSERT(pNicCtx);
  
  xge_hal_device_poll(XMP_NIC_GET_DEV(pNicCtx));

  xmpNicSetMPState(pNicCtx, XMP_NIC_MPSTATE_RUNNING, FALSE);
    /* TODO: Do we need to inititate rx indications of any pending rx frames 
     * in our queue???
     */
  XMPTRACE(XMP_DBG_INFO, ("<== xenaMpRestart\n"));
  return NDIS_STATUS_SUCCESS;
}

VOID
xenaMpShutdown(
  IN  NDIS_HANDLE             MiniportAdapterContext,
  IN  NDIS_SHUTDOWN_ACTION    ShutdownAction
  )
{
  XMPTRACE(XMP_DBG_WRN, ("==> xenaMpShutdown: nic %p\n",
                         MiniportAdapterContext));
#if oHAL
  halDisableInterrupts((xena_nic_t *) MiniportAdapterContext, HAL_INTR_ALL);
#endif
  XMPTRACE(XMP_DBG_INFO, ("<== xenaMpShutdown\n"));
}

BOOLEAN 
xenaMpCheckForHang(
  IN  NDIS_HANDLE             MiniportAdapterContext
  )
{
  xmpNicCtx_t                *pNicCtx;
  xmpNicTxFifo_t             *pFifo;
  UINT                        ui;
  BOOLEAN                     bStatus = FALSE;
  ULONG i;
  static                      timerCount = 0;
  
  XMPTRACE(XMP_DBG_DBG, ("==> xenaMpCheckForHang\n"));
   pNicCtx = (xmpNicCtx_t *) MiniportAdapterContext;
  ASSERT(pNicCtx);

#ifdef POLL_FROM_CFH
  if ( pNicCtx->mpState == XMP_NIC_MPSTATE_RUNNING )
    xge_hal_device_poll(XMP_NIC_GET_DEV(pNicCtx));
#endif

  if ( pNicCtx->NeedsNicReset )
    return TRUE;
  
  if ((++timerCount) % 2)
    return FALSE;

  for (ui=0; ui < pNicCtx->RegParams.TxFIFONum; ui++)
  {
    pFifo = &pNicCtx->TxFifo[ui];
    XF_GET_SLOCK(&pFifo->TxLock);
    if ( pFifo->OsSendCnt )
    {
      if ( pFifo->txdFreeLast == pFifo->Stats.txdFree )
      {
        XMPTRACE(XMP_DBG_WRN, ("xenaMpCheckForHang: Hang detected!!!"
                               " requesting reset... nic=%p Fifo =%d\n",
                               pNicCtx, ui));
        XF_FREE_SLOCK(&pFifo->TxLock);
        bStatus = TRUE;
        break;
      }
      else
      {
        pFifo->txdFreeLast = pFifo->Stats.txdFree;
      }
    }
    XF_FREE_SLOCK(&pFifo->TxLock);
  }

  if ( bStatus )
  {
    XMPTRACE(XMP_DBG_DBG, ("<== xenaMpCheckForHang\n"));
    return TRUE;
  }

  for (ui=0; ui < pNicCtx->RegParams.TxFIFONum; ui++)
  {
    pFifo = &pNicCtx->TxFifo[ui];
    XF_GET_SLOCK(&pFifo->TxLock);
      if ( !XF_Q_IS_NULL(&pFifo->NBLWaitQ) )
      {
        if ( !pFifo->InService && !pFifo->TxWIQueued )
        {
          pFifo->TxWIQueued = TRUE;
          NdisQueueIoWorkItem(pFifo->hTxFlushWI,
                              (NDIS_IO_WORKITEM_ROUTINE)xmpNicTxFlushCB,
                              pFifo);
          XMPTRACE(XMP_DBG_WRN, ("xenaMpCheckForHang: Flushing Fifo=%d\n",
                                 ui));
        }
      }
    XF_FREE_SLOCK(&pFifo->TxLock);
  }
#if MSI_DBG
  {
    static cfhCnt = 0;


    if ( ++cfhCnt == 10 )
    {
      cfhCnt = 0;

      XMPTRACE(XMP_DBG_INFO, ("MSI counters:\n"));
      for ( i=0; i < XGE_HAL_MAX_MSIX_VECTORS; i++ )
      {
        if ( pNicCtx->msiCnt[i] )
        {
          XMPTRACE(XMP_DBG_INFO, ("\t[MsiId=%d] Count=%d Ignore=%d DpcQ=%d DpcX=%d\n",
                                  i, pNicCtx->msiCnt[i],
                                  pNicCtx->msiIgnore[i],
                                  pNicCtx->MsiDpcQCnt[i],
                                  pNicCtx->MsiDpcXCnt[i]));
          if ( !pNicCtx->MsiDpcXCnt[i] )
            pNicCtx->MsiDpcCnt[i] = 0; /* HACK */
        }
      }

      for ( i=0; i < XENAMP_MAX_CPU; i++ )
      {
        if ( pNicCtx->msiStats.cpuIsrCnt[i] ||
             pNicCtx->msiStats.cpuDpcCnt[i] )
        {
          XMPTRACE(XMP_DBG_INFO, ("\t[Cpu=%d] ISR=%I64d Dpc=%I64d\n", i,
                                  pNicCtx->msiStats.cpuIsrCnt[i],
                                  pNicCtx->msiStats.cpuDpcCnt[i]));
        }
      }

      for (i=0; i < pNicCtx->RegParams.RxRingNum; i++)
      {
        xmpNicRxInfo_t *pRing = &pNicCtx->RxInfo[i];
        XMPTRACE(XMP_DBG_INFO, ("Ring[%d] Stats: \n\tRxCount=%I64d"
                                "\n\tMaxNBLup=%d"
                                "\n\tRxItMiss=%I64d \n\tRxItHit=%I64d"
                                " \n\tRxCpuErr=%d \n\tRxDpcCnt=%d\n"
                                " \n\tRxChkCnt=%d \n",
                                i,
                                pRing->Stats.RxCnt,
                                pRing->Stats.MaxNBLUp,
                                pRing->Stats.RxItMiss,
                                pRing->Stats.RxItHit,
                                pRing->Stats.RxCpuErr,
                                pRing->Stats.RxDpcCnt,
                                pRing->Stats.RxChkCnt));
      }
    }
  }
#endif
#ifdef LRO_DBG
  {
    static disCnt = 0;
    ULONG j;
    if ( ++disCnt == 30 )
    {
      disCnt = 0;
      for (j=0; j<pNicCtx->RegParams.RxRingNum; j++)
      {
        xmpNicRxInfo_t *pRing = &pNicCtx->RxInfo[j];
        XMPTRACE(XMP_DBG_INFO, ("Ring[%d] Stats: \n\tLroBeginCount=%d"
                                "\n\tLroContCount=%d"
                                "\n\tLroEnd1Count=%d \n\tLroEnd2Count=%d"
                                "\n\tLroEnd3Count=%d\n",
                                j,
                                pRing->Stats.LroSesBegin,
                                pRing->Stats.LroSesCont,
                                pRing->Stats.LroSesEnd1,
                                pRing->Stats.LroSesEnd2,
                                pRing->Stats.LroSesEnd3));
      }   
    }
  }
#endif
  XMPTRACE(XMP_DBG_DBG, ("<== xenaMpCheckForHang\n"));
  return bStatus;
}
VOID 
xenaMpSendNetBufferLists(
  IN  NDIS_HANDLE             MiniportAdapterContext,
  IN  PNET_BUFFER_LIST        pNBL,
  IN  NDIS_PORT_NUMBER        PortNumber,
  IN  ULONG                   SendFlags
  )
{
  xmpNicCtx_t                *pNicCtx;
  NET_BUFFER_LIST            *pCurNBL;
  NDIS_STATUS                 Status = NDIS_STATUS_SUCCESS;

  XMPTRACE(XMP_DBG_TX, ("==> xenaMpSendNetBufferLists\n"));

  pNicCtx = (xmpNicCtx_t *) MiniportAdapterContext;
  ASSERT(pNicCtx);

  XMP_NIC_REFERENCE(pNicCtx);

  do
  {
    pCurNBL = pNBL;
    if ( XMP_NIC_ACCEPT_SEND(pNicCtx) )
    {
      while(pCurNBL)
      {
        pNBL = NET_BUFFER_LIST_NEXT_NBL(pCurNBL);           /* Save next NBL */
        NET_BUFFER_LIST_NEXT_NBL(pCurNBL) = NULL;/* clear next NBL before Tx */
        xmpNicTxNBL(pNicCtx, pCurNBL, SendFlags);
        pCurNBL = pNBL;
      }
    }
    else if ( (pNicCtx->mpState == XMP_NIC_MPSTATE_PAUSING) ||
              (pNicCtx->mpState == XMP_NIC_MPSTATE_PAUSED) )
      Status = NDIS_STATUS_PAUSED;
    else if ( pNicCtx->mpState == XMP_NIC_MPSTATE_RESET )
      Status = NDIS_STATUS_RESET_IN_PROGRESS;
    else if ( bit(pNicCtx->State, XMP_NIC_STATE_NOLINK) )
      Status = NDIS_STATUS_NO_CABLE;
    else
      Status = NDIS_STATUS_DEVICE_FAILED;

    if ( pNBL == pCurNBL )
    { /* Complete NBLs that we did not send/queue.                           */
      while(pCurNBL)
      {
        NET_BUFFER_LIST_COMPLETION_STATUS(pCurNBL) = Status;
        pCurNBL = NET_BUFFER_LIST_NEXT_NBL(pCurNBL);
      }
      if( pNBL == 0 )
      {
          //please check why pNBL is null
          ASSERT(pNBL != 0);
      }
      else
      {
        XENAMP_TX_NBL_COMPLETE_UPCALL(pNicCtx, pNBL, SendFlags);
      }
    }
  } while(0);

  XMP_NIC_DEREFERENCE(pNicCtx);

  XMPTRACE(XMP_DBG_TX, ("<== xenaMpSendNetBufferLists\n"));
}

VOID 
xenaMpCancelSendNetBufferLists(
  IN  NDIS_HANDLE       MiniportAdapterContext,
  IN  VOID             *pCancelId
  )
{
  xmpNicCtx_t                *pNicCtx;
  xmpNicTxFifo_t             *pFifo;
  xmpTxNBLCtx_t              *pNBLCtx = NULL;
  NET_BUFFER_LIST            *pNBL;
  xmpQLink_t                 *link;
  KIRQL                       OldIrql;
  UINT                        ui;
  BOOLEAN                     bDispatch = TRUE;

  XMPTRACE(XMP_DBG_INFO, ("==> xenaMpCancelSendNetBufferLists:\n"));
  pNicCtx = (xmpNicCtx_t *) MiniportAdapterContext;
  ASSERT(pNicCtx);

  OldIrql =  NDIS_CURRENT_IRQL();
  if ( OldIrql != DISPATCH_LEVEL )
  {
    NDIS_RAISE_IRQL_TO_DISPATCH(&OldIrql);
    bDispatch = FALSE;
  }

  for (ui = 0; ui < pNicCtx->RegParams.TxFIFONum; ui++)
  {
    pFifo = &(pNicCtx->TxFifo[ui]);
    link = &pFifo->NBLWaitQ;                              /* Start from head */
    XF_DPR_GET_SLOCK(&pFifo->TxLock);
      do
      {
        link = XF_Q_NEXT(&pFifo->NBLWaitQ, link);
        if ( !link )
          break;
        pNBLCtx = CONTAINING_RECORD(link, xmpTxNBLCtx_t, Link);
        pNBL = pNBLCtx->pNBL;
        if ( (NDIS_GET_NET_BUFFER_LIST_CANCEL_ID(pNBL) == pCancelId) &&
             (pNBLCtx->RefCnt == 1) )
        {
          XF_Q_DEL(&pNBLCtx->Link);
          NET_BUFFER_LIST_COMPLETION_STATUS(pNBL) =
            NDIS_STATUS_REQUEST_ABORTED;
          pNBLCtx->RefCnt = 0;
          XF_DPR_FREE_SLOCK(&pFifo->TxLock);
          XENAMP_TX_NBL_COMPLETE_UPCALL(pNicCtx, pNBL, bDispatch);
          XF_ATOMIC_DEC31(pFifo->OsSendCnt);
          XF_DPR_GET_SLOCK(&pFifo->TxLock);
        }
      } while(1);
    XF_DPR_FREE_SLOCK(&pFifo->TxLock);
  }

  if ( OldIrql != DISPATCH_LEVEL )
    NDIS_LOWER_IRQL(OldIrql, DISPATCH_LEVEL);

  XMPTRACE(XMP_DBG_INFO, ("<== xenaMpCancelSendNetBufferLists:\n"));
}

VOID 
xenaMpReturnNetBufferLists(
  IN  NDIS_HANDLE       MiniportAdapterContext,
  IN  NET_BUFFER_LIST  *pNetBufferLists,
  IN  ULONG             ReturnFlags
  )

{
  xmpNicCtx_t                *pNicCtx;
  xmpRxNBLCtx_t              *pRxCtx;
  xmpRxNBLCtx_t              *pNextRxCtx;
  xmpNicRxInfo_t             *pRxRing;
  NET_BUFFER_LIST            *pNBL;
  KIRQL                       OldIrql = KeGetCurrentIrql();/* keep prefast quite */

  XMPTRACE(XMP_DBG_RX, ("==> xenaMpReturnNetBufferLists"));
  pNicCtx = (xmpNicCtx_t *) MiniportAdapterContext;
  ASSERT(pNicCtx);

  if ( !(bit(ReturnFlags, NDIS_RETURN_FLAGS_DISPATCH_LEVEL)) )
    NDIS_RAISE_IRQL_TO_DISPATCH(&OldIrql);
  XMP_NIC_REFERENCE(pNicCtx);
  do
  {
    while(pNBL = pNetBufferLists)
    {
      pNetBufferLists = NET_BUFFER_LIST_NEXT_NBL(pNetBufferLists);
      NET_BUFFER_LIST_NEXT_NBL(pNBL) = NULL;               /* clear next NBL */
      pRxCtx = (xmpRxNBLCtx_t *)NET_BUFFER_LIST_CONTEXT_DATA_START(pNBL);
      ASSERT(pRxCtx != NULL);
      ASSERT(pRxCtx == XMP_NIC_RX_NBL_GET_MR(pNBL));
      ASSERT(pRxCtx->pNBL == pNBL);
      pRxRing = &pNicCtx->RxInfo[pRxCtx->RxRing];

      XF_DPR_GET_SLOCK(&pRxRing->RxLock);
      do
      {
        pNextRxCtx      = pRxCtx->lroNext;
        pRxCtx->lroNext = NULL;

        if ( pRxCtx->advance )
        {
          NdisRetreatNetBufferDataStart(NET_BUFFER_LIST_FIRST_NB(pRxCtx->pNBL),
                                        pRxCtx->advance, FALSE, NULL);
          pRxCtx->advance = 0;
        }

        ASSERT( pRxCtx->state == XMP_RXNBL_STATE_ULP );
        if ( !bit(pRxRing->State, XMP_RX_RING_STATE_OPEN) ||
             (xmpHalPostRxD(pNicCtx, pRxRing, pRxCtx) != NDIS_STATUS_SUCCESS) )
        { /* must be reset in progress.                                      */
          XMPTRACE(XMP_DBG_WRN, ("xenaMpReturnNetBufferLists: Can't post\n"));
          pRxCtx->state = XMP_RXNBL_STATE_FREE;
        }

        pRxCtx = pNextRxCtx;
      } while( pRxCtx );

      --pRxRing->UlpNBLCnt;
      ASSERT(pRxRing->UlpNBLCnt >= 0);
      XF_DPR_FREE_SLOCK(&pRxRing->RxLock);
    }
  } while(0);
  XMP_NIC_DEREFERENCE(pNicCtx);

  NDIS_LOWER_IRQL(OldIrql, DISPATCH_LEVEL);

  XMPTRACE(XMP_DBG_RX, ("<== xenaMpReturnNetBufferLists"));
}

BOOLEAN 
xenaMpIsr(
  IN  NDIS_HANDLE             MiniportInterruptContext,
  OUT PBOOLEAN                QueueDefaultInterruptDpc,
  OUT PULONG                  TargetProcessors
  )
{
  xmpNicCtx_t                  *pNicCtx;
  xge_hal_device_t             *hal_dev;
//  xge_hal_dtr_h                dtr = NULL;
//  xmpNicRxInfo_t               *pRing;
//  u8                           tcode;
  U64                          reason;
  xge_hal_status_e             halStatus;
  ULONG                        i;
//  ULONG                        targetcpu;
//  ULONG                        rthvalue;
//  ULONG                        tproc;

  XMPTRACE(XMP_DBG_INT, ("==> xenaMpIsr\n"));
  pNicCtx = (xmpNicCtx_t *) MiniportInterruptContext;

  ASSERT(pNicCtx);
  ASSERT(pNicCtx->magic == XF_NIC_MAGIC);
  hal_dev = XMP_NIC_GET_DEV(pNicCtx);

  if (pNicCtx->IntrDpcCnt)    // NK- Active DPC count. there is a DPC running now. We decrement IntrDpcCnt in InterruptDpc routine
  {
    U64 val64;
    U64 val64s;
    xge_hal_pci_bar0_t *isrbar0 = (xge_hal_pci_bar0_t *)hal_dev->isrbar0;

#if DBG
    val64 = xge_os_pio_mem_read64(hal_dev->pdev,
                                  hal_dev->regh0,
                                  &isrbar0->general_int_mask);

    val64s = xge_os_pio_mem_read64(hal_dev->pdev,
                                    hal_dev->regh0,
                                    &isrbar0->general_int_status);

    XMPTRACE(XMP_DBG_ERR, ("xenaMpIsr: Spurious interrupt"
                           " nic=%p dpc=%lu gim=%I64X gis=%I64X\n",
                           pNicCtx, pNicCtx->IntrDpcCnt, val64, val64s));
#endif


// NK - For debug purposes, we never check. Spurious because we have an active DPC right now. This looks to be a workaround to hw bug of generating spurious interrupts
    pNicCtx->intrSpurious++;
// NK -  see if we should implement and use a Disable/Enable Interrupt handler here.
    xge_hal_device_mask_all(hal_dev);
    KeMemoryBarrier();
    if ( !pNicCtx->IntrDpcCnt )
      xge_hal_device_unmask_all(hal_dev);

    xg_flush_bus(pNicCtx);

    // NK - it may be a good idea to set QueueDefaultInterruptDpc to false.
    return TRUE;
  }

  if ( (halStatus = xge_hal_device_begin_irq(hal_dev, &reason)) == XGE_HAL_OK )
  {
    xge_hal_device_mask_all(hal_dev);
    pNicCtx->IntrDpcCnt       = 1;
    pNicCtx->IntrReason       = reason;
    pNicCtx->IntrProcessor    = NDIS_CURRENT_PROCESSOR_NUMBER;
    *TargetProcessors         = 0;
      
    if ( XMP_NIC_RSS_IS_ENABLED(pNicCtx) ) 
    {
      *QueueDefaultInterruptDpc = FALSE;
      (*TargetProcessors) |= AFFINITY_MASK(pNicCtx->RxInfo[0].RssCpu);
       pNicCtx->intrDpcQueued++;
    }
    else
    {
      *QueueDefaultInterruptDpc = TRUE;
    }
  
    if (reason & XGE_HAL_GEN_INTR_TXTRAFFIC)
    {
      xge_hal_device_clear_tx(hal_dev);
      pNicCtx->txIntr++;
    }

    if (reason & XGE_HAL_GEN_INTR_RXTRAFFIC)
    {
      xge_hal_device_clear_rx(hal_dev);
      pNicCtx->rxIntr++;
      if (XMP_NIC_RSS_IS_ENABLED(pNicCtx) &&
          (pNicCtx->RegParams.RxRingNum > 1))
      {
        for (i=1; i < pNicCtx->RegParams.RxRingNum; i++)
        {
          if (xmphalIsRxQueueReady(pNicCtx->RxInfo[i].halChannel)
              ==  XGE_HAL_OK)
          {
            (*TargetProcessors) |= AFFINITY_MASK(pNicCtx->RxInfo[i].RssCpu);
             NdisInterlockedIncrement(&pNicCtx->IntrDpcCnt);
          }
        }
        *QueueDefaultInterruptDpc = FALSE;
      }
    }

    xg_flush_bus(pNicCtx);
    return TRUE;
  }
  else
  {
    XMPTRACE(XMP_DBG_WRN, ("xenaMpIsr: hal_begin_irq=%l nic=%p dpc=%lu\n",
                           halStatus, pNicCtx, pNicCtx->IntrDpcCnt));
    pNicCtx->intrErr++;
  }

  /* not Xframe interrupt.                                                   */
  *QueueDefaultInterruptDpc = FALSE; 
  xge_hal_device_unmask_all(hal_dev);

  return FALSE;
}

VOID 
xenaMpIntrDpc(
  IN  NDIS_HANDLE             MiniportInterruptContext,
  IN  PVOID                   MiniportDpcContext,
  IN  PULONG                  NdisReserved1,
  IN  PULONG                  NdisReserved2
  )
{
  xmpNicCtx_t                *pNicCtx;
  xge_hal_device_t           *pHalDev;
  LONG                        dpcCnt;
  ULONG                       proc = NDIS_CURRENT_PROCESSOR_NUMBER;
  
  XMPTRACE(XMP_DBG_INT, ("==> xenaMpIntrDpc\n"));
  pNicCtx = (xmpNicCtx_t *) MiniportInterruptContext;
  pHalDev = XMP_NIC_GET_DEV(pNicCtx);

  NdisInterlockedIncrement((PLONG)&pNicCtx->intrDpcRun);

  if ( pNicCtx->IntrLink )
  {
    BOOLEAN blink = xmpHalGetLinkStatus(pNicCtx) == NDIS_STATUS_SUCCESS;
    pNicCtx->IntrLink = FALSE;
    xmpNicIndicateStatus(pNicCtx, blink);
  }

  if (XMP_NIC_RSS_IS_ENABLED(pNicCtx))
  {
    proc = pNicCtx->devCtx->PhyCpuRssProcMap[proc];   
    xmpHalContinueRSSIrq(MiniportInterruptContext, MiniportDpcContext, proc);
  }
  else
  {
    xge_hal_device_continue_irq(pHalDev);
  }
  
  if ( !(dpcCnt = NdisInterlockedDecrement(&pNicCtx->IntrDpcCnt)) )
  {
    xge_hal_device_unmask_all(pHalDev);
  }

  xg_flush_bus(pNicCtx);
  XMPTRACE(XMP_DBG_INT, ("<== xenaMpIntrDpc\n"));
}

VOID 
xenaMpIntrDisable(
  IN  NDIS_HANDLE             MiniportInterruptContext
  )
{
  //  OSM_ISR_SYNC(xmpNicDisableInt, (s2io_nic_t *)MiniportInterruptContext);
}

VOID 
xenaMpIntrEnable(
  IN  NDIS_HANDLE             MiniportInterruptContext
  )
{
  //  OSM_ISR_SYNC(xmpNicEnableInt, (s2io_nic_t *)MiniportInterruptContext);
}

/**
 * xge_hal_handle_alaram_interrupt - Begin IRQ processing.
 * @hldev: HAL device handle.
 * @reason: "Reason" for the interrupt, the value of Xframe's
 *          general_int_status register.
 *
 * The function performs two actions, It first checks whether (shared IRQ) the
 * interrupt was raised by the device. Next, it masks the device interrupts.
 *
 * Note:
 * xge_hal_device_begin_irq() does not flush MMIO writes through the
 * bridge. Therefore, two back-to-back interrupts are potentially possible.
 * It is the responsibility of the ULD to make sure that only one
 * xge_hal_device_continue_irq() runs at a time.
 *
 * Returns: 0, if the interrupt is not "ours" (note that in this case the
 * device remain enabled).
 * Otherwise, xge_hal_device_begin_irq() returns 64bit general adapter
 * status.
 * See also: xge_hal_device_handle_irq()
 */
__HAL_STATIC_DEVICE __HAL_INLINE_DEVICE xge_hal_status_e
xge_hal_handle_alaram_interrupt(xge_hal_device_t *hldev, u64 *reason)
{
  u64 val64;
  xge_hal_pci_bar0_t *isrbar0 = (xge_hal_pci_bar0_t *)(void *)hldev->isrbar0;

  //hldev->stats.sw_dev_info_stats.total_intr_cnt++;

  val64 = xge_os_pio_mem_read64(hldev->pdev,
            hldev->regh0, &isrbar0->general_int_status);
  if (xge_os_unlikely(val64 == XGE_HAL_ALL_FOXES)) {
                u64 adapter_status =
                        xge_os_pio_mem_read64(hldev->pdev, hldev->regh0,
                &isrbar0->adapter_status);
                if (adapter_status == XGE_HAL_ALL_FOXES)  {
      if (!hldev->config.no_isr_events) {
        (void) xge_queue_produce(hldev->queueh,
             XGE_HAL_EVENT_SLOT_FREEZE,
             hldev,
             1,  /* critical: slot freeze */
             sizeof(u64),
             (void*)&adapter_status);
      }
      *reason = 0;
      return XGE_HAL_ERR_CRITICAL;
    }
  }

  *reason = val64;

  /* separate fast path, i.e. no errors */
  if ((val64 & ~(XGE_HAL_GEN_INTR_TXTRAFFIC |
          XGE_HAL_GEN_INTR_RXTRAFFIC)) == 0)
  {
    ASSERT(FALSE);
    return XGE_HAL_OK;
  }

  if (xge_os_unlikely(val64 & XGE_HAL_GEN_INTR_TXPIC)) {
    xge_hal_status_e status;

    XMPTRACE(XMP_DBG_INFO, (" Alarm Intr: mask in ISR\n"));
    status = __hal_device_handle_txpic(hldev, val64);

    if (status != XGE_HAL_OK) {
      ASSERT(FALSE);
      return status;
    }
  }

  if (xge_os_unlikely(val64 & XGE_HAL_GEN_INTR_TXDMA)) {
    xge_hal_status_e status;
    status = __hal_device_handle_txdma(hldev, val64);
    if (status != XGE_HAL_OK) {
      return status;
    }
  }

  if (xge_os_unlikely(val64 & XGE_HAL_GEN_INTR_TXMAC)) {
    xge_hal_status_e status;
    status = __hal_device_handle_txmac(hldev, val64);
    if (status != XGE_HAL_OK) {
      return status;
    }
  }

  if (xge_os_unlikely(val64 & XGE_HAL_GEN_INTR_TXXGXS)) {
    xge_hal_status_e status;
    status = __hal_device_handle_txxgxs(hldev, val64);
    if (status != XGE_HAL_OK) {
      return status;
    }
  }

  if (xge_os_unlikely(val64 & XGE_HAL_GEN_INTR_RXPIC)) {
    xge_hal_status_e status;
    status = __hal_device_handle_rxpic(hldev, val64);
    if (status != XGE_HAL_OK) {
      return status;
    }
  }

  if (xge_os_unlikely(val64 & XGE_HAL_GEN_INTR_RXDMA)) {
    xge_hal_status_e status;
    status = __hal_device_handle_rxdma(hldev, val64);
    if (status != XGE_HAL_OK) {
      return status;
    }
  }

  if (xge_os_unlikely(val64 & XGE_HAL_GEN_INTR_RXMAC)) {
    xge_hal_status_e status;
    status = __hal_device_handle_rxmac(hldev, val64);
    if (status != XGE_HAL_OK) {
      return status;
    }
  }

  if (xge_os_unlikely(val64 & XGE_HAL_GEN_INTR_RXXGXS)) {
    xge_hal_status_e status;
    status = __hal_device_handle_rxxgxs(hldev, val64);
    if (status != XGE_HAL_OK) {
      return status;
    }
  }

  if (xge_os_unlikely(val64 & XGE_HAL_GEN_INTR_MC)) {
    xge_hal_status_e status;
    status = __hal_device_handle_mc(hldev, val64);
    if (status != XGE_HAL_OK) {
      return status;
    }
  }

  return XGE_HAL_OK;
}

VOID
HandleFifoInterrupt(
  xge_hal_device_t           *pHalDev, 
  xge_hal_channel_h            channelh
  )
{
  xge_hal_dtr_h      dtr = NULL;
  u8                tcode;
  xge_hal_channel_t *channel = (xge_hal_channel_t *)channelh;

  if ( xge_hal_fifo_dtr_next_completed(channelh, &dtr, &tcode) == XGE_HAL_OK )
  {
      channel->callback(channel, dtr, tcode, channel->userdata);
  }
}

VOID
HandleRingInterrupt(
  xmpNicCtx_t                *pNicCtx, 
  xge_hal_channel_h            channelh
  )
{
  xge_hal_dtr_h      dtr = NULL;
  u8                tcode;
  xge_hal_channel_t *channel = (xge_hal_channel_t *)channelh;

  ((xge_hal_ring_t*)channel)->cmpl_cnt = 0;
  if ( xge_hal_ring_dtr_next_completed(channelh, &dtr, &tcode) == XGE_HAL_OK )
  {
    xmpHalRxFrameCB(channelh, dtr, tcode, pNicCtx);
  }
}

#define USE_NDIS_MSIX_MASKING 1

BOOLEAN 
xenaMpMSIsr(
  IN  VOID                   *MiniportInterruptContext,
  IN  ULONG                   MessageId,
  OUT BOOLEAN                *pQueueMiniportInterruptDpcHandler,
  OUT ULONG                  *TargetProcessors
  )
{
  xmpNicCtx_t                *pNicCtx;
  NDIS_STATUS                 Status;
  NDIS_MSIX_CONFIG_PARAMETERS msiCfgParam;
  ULONG                       index = 0;

  XMPTRACE(XMP_DBG_INT, ("==> xenaMpMSIsr\n"));

  pNicCtx = (xmpNicCtx_t *) MiniportInterruptContext;

  ASSERT(pNicCtx);
  ASSERT(MessageId < pNicCtx->devCtx->MsiMsgCount);
  index = pNicCtx->devCtx->MsiCpuMap[MessageId];
#ifdef MSI_DBG
  if ( MessageId <= (pNicCtx->RegParams.RxRingNum) )
  {
    ASSERT(pNicCtx->devCtx->RssProcessors[index] == NDIS_CURRENT_PROCESSOR_NUMBER);
  }
  else
  {
   
    ASSERT(FALSE);
  }

#ifdef ASSERT_ON_ERROR
  ASSERT(pNicCtx->MsiDpcCnt[MessageId] == 0); 
#endif

  if (pNicCtx->MsiDpcCnt[MessageId])
  {
    pNicCtx->msiIgnore[MessageId]++;
    *pQueueMiniportInterruptDpcHandler = FALSE;
    return TRUE;
  }

  NdisInterlockedIncrement(&pNicCtx->MsiDpcCnt[MessageId]);            
  NdisInterlockedIncrement(&pNicCtx->MsiDpcQCnt[MessageId]);
  pNicCtx->msiCnt[MessageId]++;
  pNicCtx->msiStats.cpuIsrCnt[NDIS_CURRENT_PROCESSOR_NUMBER]++;
#endif

#ifdef USE_NDIS_MSIX_MASKING
  XENAMP_NDIS_OBJECT_INIT(&msiCfgParam.Header,
                          NDIS_OBJECT_TYPE_DEFAULT,
                          NDIS_MSIX_CONFIG_PARAMETERS_REVISION_1,
                          NDIS_SIZEOF_MSIX_CONFIG_PARAMETERS_REVISION_1);
  msiCfgParam.ConfigOperation = NdisMSIXTableConfigMaskTableEntry;
  msiCfgParam.TableEntry      = MessageId;
  msiCfgParam.MessageNumber   = 0;         /* Ignored for MASK operation */
  Status = NdisMConfigMSIXTableEntry(pNicCtx->hMPAdapter,
                                     &msiCfgParam);
  ASSERT(Status == NDIS_STATUS_SUCCESS);
#else
  {
    xge_hal_device_t           *pHalDev;
    pHalDev = XMP_NIC_GET_DEV(pNicCtx);
    xge_hal_mask_msix(pHalDev, MessageId);
    xg_flush_bus(pNicCtx);  
  }
#endif

  *pQueueMiniportInterruptDpcHandler = TRUE;
  XMPTRACE(XMP_DBG_INT, ("<== xenaMpIsr YES \n"));
  return TRUE;  /* no sharing in case of MSI, always recognize the interrupt */
}

VOID 
xenaMpMSIDpc(
  IN  NDIS_HANDLE             MiniportInterruptContext,
  IN  ULONG                   MessageId,
  IN  PVOID                   MiniportDpcContext,
  IN  PULONG                  NdisReserved1,
  IN  PULONG                  NdisReserved2
  )
{
  xmpNicCtx_t                *pNicCtx;
  xge_hal_device_t           *pHalDev;
  xmpNicRxInfo_t             *pRing;
  xmpNicTxFifo_t             *pFifo;
  xge_hal_dtr_h               dtr = NULL;
  U64                         reason;
  LONG                        dpcCnt;
  xge_hal_channel_h           channelh;
  ULONG                       proc = NDIS_CURRENT_PROCESSOR_NUMBER;  
  xge_hal_status_e            halStatus;
  NDIS_STATUS                 Status;
  NDIS_MSIX_CONFIG_PARAMETERS msiCfgParam;
  ULONG                       i; 
  ULONG                       index;

  XMPTRACE(XMP_DBG_INT, ("==> xenaMpMSIDpc\n"));
  pNicCtx = (xmpNicCtx_t *) MiniportInterruptContext;
  pHalDev = XMP_NIC_GET_DEV(pNicCtx); 

  index  = pNicCtx->devCtx->MsiCpuMap[MessageId]; 
  ASSERT (pNicCtx->devCtx->RssProcessors[index] == proc);

  if ( MessageId )
  {
    if ( (MessageId >= pNicCtx->IntrMsiTxLo) &&
         (MessageId <= pNicCtx->IntrMsiTxHi) )
    {
      channelh = pNicCtx->TxFifo[MessageId - pNicCtx->IntrMsiTxLo].halChannel;
      /* Process Tx trafic interrupt for fifo.                               */
      HandleFifoInterrupt(pHalDev, channelh);
    }

    if ( (MessageId >= pNicCtx->IntrMsiRxLo) &&
         (MessageId <= pNicCtx->IntrMsiRxHi) )
    {
      channelh = pNicCtx->RxInfo[MessageId - pNicCtx->IntrMsiRxLo].halChannel ;
      /* Process Rx trafic interrupt for ring */
      HandleRingInterrupt(pNicCtx, channelh);
    }
  }
  else
  {                  
    /*TODO* Should return status from here                                */
    xge_hal_handle_alaram_interrupt(pHalDev, &reason);
    XMPTRACE(XMP_DBG_INFO, ("xenaMpMSIsr: MessageId=0 \n"));
  }

#ifdef MSI_DBG
    NdisInterlockedIncrement(&pNicCtx->MsiDpcXCnt[MessageId]);

    {
      LONG ndpc;
      ndpc = NdisInterlockedDecrement(&pNicCtx->MsiDpcCnt[MessageId]);
      ASSERT(ndpc == 0);
      if ( ndpc )
      {
        XMPTRACE(XMP_DBG_WRN, ("xenaMpMSIDpc: MSI[%d] DPC_Count=%d\n",
                               MessageId, ndpc));
      }
    }
    pNicCtx->msiStats.cpuDpcCnt[proc]++;
#endif

#ifdef USE_NDIS_MSIX_MASKING
     XENAMP_NDIS_OBJECT_INIT(&msiCfgParam.Header,
                              NDIS_OBJECT_TYPE_DEFAULT,
                              NDIS_MSIX_CONFIG_PARAMETERS_REVISION_1,
                              NDIS_SIZEOF_MSIX_CONFIG_PARAMETERS_REVISION_1);
      msiCfgParam.ConfigOperation = NdisMSIXTableConfigUnmaskTableEntry;
      msiCfgParam.TableEntry      = MessageId;
      msiCfgParam.MessageNumber   = 0;         /* Ignored for MASK operation */
      Status = NdisMConfigMSIXTableEntry(pNicCtx->hMPAdapter,
                                         &msiCfgParam);
#else
    xge_hal_unmask_msix(pHalDev, MessageId);
    xg_flush_bus(pNicCtx);
#endif
      
  /* NdisInterlockedDecrement(&pNicCtx->IntrDpcCnt); */
  XMPTRACE(XMP_DBG_INT, ("<== xenaMpMSIDpc\n"));
  /* xge_hal_unmask_msix(pHalDev, channelh); */
  /* xge_hal_device_mask_all(pHalDev); */
}

VOID 
xenaMpMSIDisable(
  IN  NDIS_HANDLE             MiniportInterruptContext,
  IN  ULONG                   MessageId
  )
{
  //  OSM_ISR_SYNC(xmpNicDisableInt, (s2io_nic_t *)MiniportInterruptContext);
}

VOID 
xenaMpMSIEnable(
  IN  NDIS_HANDLE             MiniportInterruptContext,
  IN  ULONG                   MessageId
  )
{
  //  OSM_ISR_SYNC(xmpNicEnableInt, (s2io_nic_t *)MiniportInterruptContext);
}

VOID
xenaMpSleep(
  IN  UINT              usec
  )
{
  if ( KeGetCurrentIrql( ) == PASSIVE_LEVEL  )
    NdisMSleep (usec);
  else
    NdisStallExecution(usec);
}

VOID xmpPauseHandler(
  IN VOID           *pContext,
  IN NDIS_HANDLE     Handle
  )
{
  xmpNicCtx_t                *pNicCtx;
  xmpNicTxFifo_t             *pTxFifo;
  xmpNicRxInfo_t             *pRxRing;
  UINT                        ui;
  NDIS_STATUS                 Status = NDIS_STATUS_PENDING;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpPauseHandler\n"));

  pNicCtx = (xmpNicCtx_t *) pContext;
  ASSERT(pNicCtx);
  /* Drain the pending NBL send queue for every Tx FIFO.                     */
  for (ui = 0; ui < pNicCtx->RegParams.TxFIFONum; ui++)
  {
    pTxFifo = &pNicCtx->TxFifo[ui];
    xmpNicTxFifoDrainQ(pNicCtx, ui, FALSE);
  }

  /* Wait for in-flight sends.                                               */
  for (ui = 0; ui < pNicCtx->RegParams.TxFIFONum; ui++)
  {
    pTxFifo = &pNicCtx->TxFifo[ui];
    while(pTxFifo->OsSendCnt)
    {
      XMPTRACE(XMP_DBG_INFO, ("xmpPauseHandler: Fifo=%d o/s sends=%d\n",
                              ui, pTxFifo->OsSendCnt));
      xenaMpSleep(100 * 1000);
    }
  }

  /* Wait for Rx NBLs to be returned from ULPs.                              */
  for (ui = 0; ui < pNicCtx->RegParams.RxRingNum; ui++)
  {
    pRxRing = &pNicCtx->RxInfo[ui];
    while(pRxRing->UlpNBLCnt)
    {
      XMPTRACE(XMP_DBG_INFO, ("xmpPauseHandler: Ring=%d o/s recv=%x\n",
                              ui, pRxRing->UlpNBLCnt));
      xenaMpSleep(100 * 1000);
    }
  }

  xmpNicSetMPState(pNicCtx, XMP_NIC_MPSTATE_PAUSED, FALSE);
  XMP_NIC_REFERENCE(pNicCtx);
    NdisMPauseComplete(pNicCtx->hMPAdapter);
  XMP_NIC_DEREFERENCE(pNicCtx);
  XMPTRACE(XMP_DBG_INFO, ("<== xmpPauseHandler\n"));
}

VOID xmpResetHandler(
     IN VOID                    *pContext,
     IN NDIS_HANDLE              Handle
  )
{
  xmpNicCtx_t                *pNicCtx;
  xmpNicTxFifo_t             *pTxFifo;
  xmpNicRxInfo_t             *pRxRing;
  UINT                        ui;
  UINT                        linkCnt = 0;
  NDIS_STATUS                 Status;

  XMPTRACE(XMP_DBG_INFO, ("==> xmpResetHandler:nic=%p\n", pContext));

  pNicCtx = (xmpNicCtx_t *) pContext;
  ASSERT(pNicCtx);
  /* Drain the pending NBL send queue for every Tx FIFO.                     */
  for (ui = 0; ui < pNicCtx->RegParams.TxFIFONum; ui++)
  {
    pTxFifo = &pNicCtx->TxFifo[ui];
    xmpNicTxFifoDrainQ(pNicCtx, ui, FALSE);
  }

  do
  {
    while ( pNicCtx->wiQueued || pNicCtx->wiRunning )
    {
      XMPTRACE(XMP_DBG_INFO, ("xmpResetHandler: Wait for set RSS worker\n"));
      KeMemoryBarrier();
      xenaMpSleep(100 * 1000);
    }

    xge_hal_device_intr_disable(XMP_NIC_GET_DEV(pNicCtx));
    /* Reset the nic, free in-flight Tx NBLs, reset Tx FIFOs and Rx Rings.   */
    if ( (Status = xmpHalNicReset(pNicCtx)) != NDIS_STATUS_SUCCESS )
    {
      XMPTRACE(XMP_DBG_ERR, ("xmpHalNicReset failed: nic=%p\n", pNicCtx));
      break;
    }

    for (ui = 0; ui < pNicCtx->RegParams.TxFIFONum; ui++)
    {
      pTxFifo = &pNicCtx->TxFifo[ui];
      ASSERT(pTxFifo->OsSendCnt == 0);
    }

    /* Set Current Address */
    if ( bit(pNicCtx->Flags, XMP_NIC_FLAGS_USE_CURRENT_ADDR) )
    {
      if ( xmpHalSetAddress(pNicCtx, &pNicCtx->CurrentAddress[0])
           != NDIS_STATUS_SUCCESS)
      {
        XMPTRACE(XMP_DBG_ERR, ("xmpHalSetMacAddress failed: nic=%p\n",
                               pNicCtx));
        Status = NDIS_STATUS_FAILURE;
        break;
      }
    }

    if ( pNicCtx->bInDiagMode )
    {
      u64 NotPassed = TRUE;
      xge_hal_rldram_test(XMP_NIC_GET_DEV(pNicCtx), &NotPassed);
      pNicCtx->Diag.status = NotPassed ? XGE_HAL_FAIL : XGE_HAL_OK;
    }

    if ( (Status = xmpHalNicStart(pNicCtx)) != NDIS_STATUS_SUCCESS )
      break;

    if ( XMP_NIC_RSS_IS_ENABLED(pNicCtx) )
    { /* restore RSS settings.                                               */
      xmpReqSetRss(pNicCtx);
      /* XMP_NIC_RSS_DISABLE(pNicCtx);
         xmpNicSetRssParameters(pNicCtx, NULL); */
    }

    xmpHalEnableInterrupts(pNicCtx);

    while ( (xmpHalGetLinkStatus(pNicCtx) != NDIS_STATUS_SUCCESS) &&
            linkCnt++ < 10 )
    {
      XMPTRACE(XMP_DBG_INFO, ("xmpResetHandler: [nic:%p] Wait for link\n",
                              pNicCtx));
      xge_hal_device_poll(XMP_NIC_GET_DEV(pNicCtx));
      xenaMpSleep(100 * 1000);
    }

    if ( xmpHalGetLinkStatus(pNicCtx) == NDIS_STATUS_SUCCESS )
      xmpNicIndicateStatus(pNicCtx, TRUE);
    else
      bis(pNicCtx->State, XMP_NIC_STATE_NOLINK);

    xmpNicSetMPState(pNicCtx, XMP_NIC_MPSTATE_RUNNING, FALSE);
    Status = NDIS_STATUS_SUCCESS;
  } while(0);

  if ( Status != NDIS_STATUS_SUCCESS )
  {
    XMPTRACE(XMP_DBG_ERR, ("xmpResetHandler: Failed to reset!\n"));
  }
  NdisMResetComplete(pNicCtx->hMPAdapter, Status, TRUE);
  XMPTRACE(XMP_DBG_INFO, ("<== xmpResetHandler\n"));
}

VOID
xenampSendNetBufferListsComplete(
  IN xmpNicCtx_t             *pNicCtx,
  IN PNET_BUFFER_LIST         pNBL,
  IN  ULONG                   SendFlags
  )
{
  NET_BUFFER_LIST            *pCurNBL;
  NET_BUFFER                 *nb;

#ifdef XENAMP_TXDBG
  pCurNBL = pNBL;
  while(pCurNBL)
  {
    XF_ATOMIC_INC31(pNicCtx->DbgTxNBLComp);
    nb = NET_BUFFER_LIST_FIRST_NB(pCurNBL);
    while(nb)
    {
      XF_ATOMIC_INC31(pNicCtx->DbgTxNBComp);
      nb = NET_BUFFER_NEXT_NB(nb);
    }
    pCurNBL = NET_BUFFER_LIST_NEXT_NBL(pCurNBL);
  }
#endif
  if ( pNBL)
    NdisMSendNetBufferListsComplete(pNicCtx->hMPAdapter,
                                    pNBL,
                                    NDIS_SEND_COMPLETE_FLAGS_DISPATCH_LEVEL);
}

NDIS_STATUS
xenaMpSetHdrSplitAttribs(
  IN xmpNicCtx_t             *pNicCtx
  )
{
  NDIS_STATUS                 Status;
  NDIS_HD_SPLIT_ATTRIBUTES    hdAttribs = {0};
  NDIS_MINIPORT_ADAPTER_HARDWARE_ASSIST_ATTRIBUTES *hwAssist;
  NDIS_MINIPORT_ADAPTER_ATTRIBUTES MPAttribs = {0};

  if ( !XMP_NIC_RX_SPLIT(pNicCtx) )
    return NDIS_STATUS_SUCCESS;

  if ( XMP_NIC_RX_SPLIT_COMBINE(pNicCtx) )
  { /* We'll use scatter mode-B i.e., 2-buffer mode.                         */
    pNicCtx->RxHeaderSz   = XENAMP_RXBUF_ETH_HDR_BACKFILL_SZ;
    pNicCtx->RxBackfillSz = XENAMP_RXBUF_ETH_HDR_BACKFILL_SZ;
    return NDIS_STATUS_SUCCESS;
  }

  hwAssist  = &MPAttribs.HardwareAssistAttributes;
  hwAssist->HDSplitAttributes = &hdAttribs;

  XENAMP_NDIS_OBJECT_INIT
      (&hwAssist->Header,
       NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_HARDWARE_ASSIST_ATTRIBUTES,
       NDIS_MINIPORT_ADAPTER_HARDWARE_ASSIST_ATTRIBUTES_REVISION_1,
       NDIS_SIZEOF_MINIPORT_ADAPTER_HARDWARE_ASSIST_ATTRIBUTES_REVISION_1);

  XENAMP_NDIS_OBJECT_INIT(&hdAttribs.Header,
                          NDIS_OBJECT_TYPE_HD_SPLIT_ATTRIBUTES,
                          NDIS_HD_SPLIT_ATTRIBUTES_REVISION_1,
                          NDIS_SIZEOF_HD_SPLIT_ATTRIBUTES_REVISION_1);

  hdAttribs.HardwareCapabilities = 
    (NDIS_HD_SPLIT_CAPS_SUPPORTS_HEADER_DATA_SPLIT       |
     NDIS_HD_SPLIT_CAPS_SUPPORTS_IPV4_OPTIONS            |
     /* NDIS_HD_SPLIT_CAPS_SUPPORTS_IPV6_EXTENSION_HEADERS  |*/
     NDIS_HD_SPLIT_CAPS_SUPPORTS_TCP_OPTIONS);

  hdAttribs.CurrentCapabilities = hdAttribs.HardwareCapabilities;
  hdAttribs.HDSplitFlags  = 0;
  hdAttribs.BackfillSize  = 0;
  hdAttribs.MaxHeaderSize = 0;

  if ((Status = NdisMSetMiniportAttributes(pNicCtx->hMPAdapter,
                                           &MPAttribs)) == NDIS_STATUS_SUCCESS)
  {
    ASSERT(hdAttribs.BackfillSize);
    ASSERT(hdAttribs.BackfillSize >= hdAttribs.MaxHeaderSize);
    pNicCtx->RxHeaderSz   = hdAttribs.MaxHeaderSize;
    pNicCtx->RxBackfillSz = hdAttribs.BackfillSize;
  }
  else
  {
    XMPTRACE(XMP_DBG_ERR, ("NdisMSetMiniportAttributes failed: HD split:%x\n",
                           Status));
  }
  return Status;
}
#pragma warning(pop)   // Save the current warning state

