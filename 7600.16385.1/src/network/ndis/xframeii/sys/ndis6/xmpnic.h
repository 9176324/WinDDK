#ifndef XMPNIC_H
#define XMPNIC_H

/******************************* Structure defn ******************************/

typedef struct _xmpTxFifoStats_t
{
  ULONGLONG             TxNBLCount;
  ULONGLONG             TxNBLComp;
  ULONGLONG             TxNBCount;
  ULONGLONG             txdPost;
  ULONGLONG             txdFree;
  ULONG                 txdUnder;
  LONG                  TxNBLUnder;
  LONG                  TxNBLJunk;
  LONG                  TxFailCnt;
  LONG                  MaxOsNBLs;
} xmpTxFifoStats_t;

/* Information per Tx Fifo.                                                  */
struct _xmpNicTxFifo_t
{
  xmpSpinLock_t          TxLock;                         /* Tx lock per FIFO */
  xmpSpinLock_t          HalTxLock;                       /* for HAL channel */
  xmpQLink_t             NBLCtxQ;
  xmpQLink_t             NBLWaitQ;
  xge_hal_channel_h      halChannel;
  UCHAR                 *NBLCtxBuf;
  VOID                  *pNicCtx;
  NET_BUFFER_LIST       *pNblTxC;                 /* List of Tx complete NBL */
  NET_BUFFER_LIST       *pNblTxCLast;         /* Most recent Tx Complete NBL */
  volatile LONG          OsSendCnt;            /* Count of NetBuf pending Tx */

#define XMP_TX_FIFO_STATE_OK_TO_TX 0x00000001
#define XMP_TX_FIFO_STATE_OPEN     0x00000002
  U32                    State;
  U32                    CfgTxDCnt;
  BOOLEAN                InService;
  BOOLEAN                TxWIQueued;
  BOOLEAN                TxUnderrun;
  NDIS_HANDLE            hTxFlushWI;
  ULONGLONG              txdFreeLast;
  xmpTxFifoStats_t       Stats;
#ifdef XENAMP_TXD_FREE_COALESCE
  xge_hal_dtr_h         *dtrFree;
#endif
};

typedef struct _xmpRxRingStats_t
{
  ULONGLONG             RxCnt;                             /* total Rx Count */
  LONG                  RxFailCnt;                      /* Rx Failures count */
  LONG                  MaxNBLUp;        /* Maximum NBLs indicated in upcall */
  LONG                  MaxOsNBLs;               /* Maximum Outstanding NBLs */
  ULONG                 RxDpcCnt;
  ULONG                 RxCpuErr;       /* packets received on incorrect CPU */
  ULONGLONG             RxItMiss;
  ULONGLONG             RxItHit;
  ULONG                 RxChkCnt;
  ULONG                 LroSesBegin;
  ULONG                 LroSesCont;
  ULONG                 LroSesEnd1;
  ULONG                 LroSesEnd2;
  ULONG                 LroSesEnd3;
} xmpRxRingStats_t;

typedef struct _xmpNicRxHdrBlk_t
{
  void                  *va;
  NDIS_PHYSICAL_ADDRESS  pa;
} xmpNicRxHdrBlk_t;

typedef struct _xmpNicRxHdrMDL
{
  MDL                   *pMDL;      /* MDL describing start of header buffer */
  UCHAR                 *pHdrBuf;            /* Header buffer virtal address */
  U64                    PhyAddr;          /* Header buffer physical address */
} xmpNicRxHdrMDL_t;

/* Information per Rx ring.                                                  */
typedef struct _xmpNicRxInfo_t
{
  xmpQLink_t             RxNBLQ;                      /* List of all Rx NBLs */
  xge_hal_channel_h      halChannel;
  NET_BUFFER_LIST       *pNBL;                    /* NBL List to indicate up */
  NET_BUFFER_LIST       *pPrvNBL;  /* Previous NBL: Rx completion processing */
  UINT                   NBLCount;  /* Count of NBLs in NBL list to indicate */
  volatile LONG          UlpNBLCnt; /* Count of NBLs with ULP pending return */
  xmpSpinLock_t          RxLock;                         /* Rx lock per ring */
#define XMP_RX_RING_STATE_OK_TO_RX 0x00000001
#define XMP_RX_RING_STATE_OPEN     0x00000002
  U32                    State;
  U32                    CfgRxDCnt;
  U32                    RxDCnt;
  ULONG                  hdrMDLPos;    /* current position into hdrMSLsArr[] */
  ULONG                  MsiId;        /* MSIId assigned to the ring, if any */
  xmpNicRxHdrMDL_t      *hdrMDLsArr;
  xmpNicRxHdrBlk_t      *hdrBlockArr;
  UCHAR                  RssCpu;
  xmpRxRingStats_t       Stats;
} xmpNicRxInfo_t;

typedef struct _xmpNicPciCfg_t
{
  UCHAR  RevisionID;
  USHORT SubVendorID;
  USHORT SubSystemID;
} xmpNicPciCfg_t;

typedef struct _xmpNicUParams_t
{
  ULONG tx_urange_a;
  ULONG tx_ufc_a;
  ULONG tx_urange_b;
  ULONG tx_ufc_b;
  ULONG tx_urange_c;
  ULONG tx_ufc_c;
  ULONG tx_ufc_d;
  ULONG tx_timer_val;
  ULONG tx_utilz_periodic;
  ULONG rx_urange_a;
  ULONG rx_ufc_a;
  ULONG rx_urange_b;
  ULONG rx_ufc_b;
  ULONG rx_urange_c;
  ULONG rx_ufc_c;
  ULONG rx_ufc_d;
  ULONG rx_timer_val;
  ULONG rx_utilz_periodic;
} xmpNicUParams_t;

typedef struct _xmpRegParams
{
#ifdef HAL_DEBUG_ON
  ULONG hal_debug;
#endif
  /* Tx side stuff */
  ULONG TxFIFONum;
  ULONG MaxTxDs;
  ULONG TxFIFOLen_0;
  ULONG TxFIFOLen_1;
  ULONG TxFIFOLen_2;
  ULONG TxFIFOLen_3;
  ULONG TxFIFOLen_4;
  ULONG TxFIFOLen_5;
  ULONG TxFIFOLen_6;
  ULONG TxFIFOLen_7;
  ULONG TxReqTimeOut;
  ULONG TxNoSnoop;

  /* Rx side stuff */
  ULONG RxRingNum;
  ULONG RxRingSz_0;
  ULONG RxRingSz_1;
  ULONG RxRingSz_2;
  ULONG RxRingSz_3;
  ULONG RxRingSz_4;
  ULONG RxRingSz_5;
  ULONG RxRingSz_6;
  ULONG RxRingSz_7;
  ULONG RxNoSnoop;
  ULONG RxThresh;
  ULONG RxD_Backoff_interval;

  /* MAC stuff */
  ULONG mtu;
  ULONG FlowCtrl;
  ULONG IpCkoV4;
  ULONG IpCkoV6;
  ULONG TcpCkoV4;
  ULONG TcpCkoV6;
  ULONG UdpCkoV4;
  ULONG UdpCkoV6;
  ULONG LsoV4;
  ULONG PriVlan;
  ULONG rss_enable;
  ULONG Lro;
  ULONG rx_split;
  ULONG intr_modr;
  ULONG offload_encapsulation;
  
  /* interrupt coalesing */
  ULONG tx_urange_a;
  ULONG tx_ufc_a;
  ULONG tx_urange_b;
  ULONG tx_ufc_b;
  ULONG tx_urange_c;
  ULONG tx_ufc_c;
  ULONG tx_ufc_d;
  ULONG tx_timer_val;
  ULONG tx_utilz_periodic;
  ULONG tx_ac_disable;
  ULONG tx_int_type_per_list;
  ULONG tx_int_type_utilz;

  ULONG pcc_enable;

  ULONG rx_urange_a;
  ULONG rx_ufc_a;
  ULONG rx_urange_b;
  ULONG rx_ufc_b;
  ULONG rx_urange_c;
  ULONG rx_ufc_c;
  ULONG rx_ufc_d;
  ULONG rx_timer_val;
  ULONG rx_utilz_periodic;
  ULONG rx_ac_disable;

  /* misc */
  ULONG stats_auto_enable;
  ULONG stats_refresh_time;
  ULONG max_split_trans;
  ULONG max_read_byte_cnt;
  ULONG sched_intr_timer_val; 
  ULONG indicate_max_pkts;
  ULONG rx_nbl_coalesce;
  ULONG tx_nbl_coalesce;
  ULONG interrupt_poll_cnt;
  ULONG shared_splits;
  ULONG rd2rd_dly;
  ULONG enable_tx_req_timeout;
  ULONG tx_req_timeout;
  ULONG latency_timer;
  ULONG intr_mode;
} xmpRegParams_t;

typedef UCHAR xmpMacAddr_t[ETH_LENGTH_OF_ADDRESS];

typedef struct _xmpAddrList
{
  xmpMacAddr_t MacAddr[XGE_HAL_MAX_MAC_ADDRESSES];
  ULONG MacCnt;
  ULONG McCnt;
} xmpAddrList_t;

# define XMP_NIC_RSS_MAX_KEY_SZ   40
# define XMP_NIC_RSS_MAX_TABLE_SZ 256

# define XMP_NIC_RSS_MAX_QUEUES	   8

typedef struct _xmpNicRssParams
{
  USHORT                 BaseCpuNum;
  USHORT                 Flags;
  ULONG                  HashType;
  ULONG                  HashFunction;
  USHORT                 HashBitsSz;
  USHORT                 SecretKeySz;
  USHORT                 TableSz;
  UCHAR                  SecretKey[XMP_NIC_RSS_MAX_KEY_SZ];
  UCHAR                  Table[XMP_NIC_RSS_MAX_TABLE_SZ];
} xmpNicRssParams_t;

/* To keep the setting from NDIS to return in query.                        */
typedef struct _xmpNicNdisRssSet
{
  NDIS_RECEIVE_SCALE_PARAMETERS params;       /* IMP: must be the 1st member */
  UCHAR                  key[XMP_NIC_RSS_MAX_KEY_SZ];
  UCHAR                  Table[XMP_NIC_RSS_MAX_TABLE_SZ];
  ULONG                  size;
} xmpNicNdisRssSet_t;

#define XENAMP_MAX_CPU MAXIMUM_PROCESSORS

#if MSI_X_ENABLE
typedef struct _xmpAddNicCtx_t
{
  NDIS_HANDLE	         NdisMiniportHandle;
  ULONG                  RssBaseCpu;
  ULONG                  RssCpuCount;
  ULONG                  MsiMsgCount;
  BOOLEAN                FilterResourcesDone;
  ULONG                  MsiCpuMap[XENAMP_MAX_CPU];
  UCHAR                  RssProcessors[XENAMP_MAX_CPU];
  UCHAR                  PhyCpuRssProcMap[XENAMP_MAX_CPU];

  UINT                   IntrMode;
} xmpAddNicCtx_t;
#endif

typedef struct _xmpMsiStats_t
{
  U64                    cpuIsrCnt[XENAMP_MAX_CPU];
  U64                    cpuDpcCnt[XENAMP_MAX_CPU];
} xmpMsiStats_t;

typedef struct _xmpOidStats_t
{
  ULONGLONG              rxErrFrames;
  ULONGLONG              rxDirFrames;
  ULONGLONG              rxMcastFrames;
  ULONGLONG              rxBcastFrames;
  ULONGLONG              rxDirBytes;
  ULONGLONG              rxBcastBytes;
  ULONGLONG              rxMcastBytes;

  ULONGLONG              txErrFrames;
  ULONGLONG              txDirFrames;
  ULONGLONG              txMcastFrames;
  ULONGLONG              txBcastFrames;
  ULONGLONG              txDirBytes;
  ULONGLONG              txBcastBytes;
  ULONGLONG              txMcastBytes;
} xmpOidStats_t;

typedef struct _xmpDiag_t
{
  VOID                  *pBuffAddr;
  NDIS_PHYSICAL_ADDRESS  BuffAddrPhy;
  UINT                   BuffSz;
  BOOLEAN                SendError; 
  BOOLEAN                RecvError;
  BOOLEAN                bSendInProgress;
  UINT                   rx_t_code; 
  UINT                   TxPktSz;
  ULONG                  TxPktCnt;
  ULONG                  TxPktCompCnt;
  ULONG                  RxPktCnt;
  ULONG                  RxPktLastCnt;
  NDIS_STATUS            status;
} xmpDiag_t;

/* Miniport specific information per NIC.                                    */
typedef struct _xmpNicCtx_t
{
#define XF_NIC_MAGIC     0xd00fdaed
  ULONG                  magic;
  DECLSPEC_ALIGN(8)
  volatile LONG          IntrDpcCnt;
  ULONG                  IntrProcessor;
  ULONG                  IntrMsiTxLo;
  ULONG                  IntrMsiTxHi;
  ULONG                  IntrMsiRxLo;
  ULONG                  IntrMsiRxHi;
  ULONGLONG              IntrReason;
  BOOLEAN                IntrLink;
  xge_hal_device_t      *pHalDev;
  xmpAddNicCtx_t        *devCtx;                       /* Add Device Context */
  NDIS_HANDLE            hMPAdapter;       /* NDIS's miniport adapter handle */
  NDIS_HANDLE            hMPDma;
  NDIS_HANDLE            hInterrupt;
  NDIS_HANDLE            hWorkItem;
  NDIS_HANDLE            hResetWorkItem;
  NDIS_HANDLE            hSetRssWI;
  xmpQLink_t             GblListLink;          /* Link for all adapters list */
  xmpQLink_t             RxBufBlkQ;              /* Allocated xmpRxBufBlk(s) */
  NDIS_HANDLE            hRxNBLPool;         /* Rx NetBufferList pool handle */
  LONG                   RefCnt;                 /* Ref count of this object */

#define XMP_NIC_FLAGS_SG_DMA_INIT       0x00000001
#define XMP_NIC_FLAGS_INTR_REG          0x00000002
#define XMP_NIC_FLAGS_HAL_NIC_INIT      0x00000004
#define XMP_NIC_FLAGS_USE_CURRENT_ADDR  0x00000008
#define XMP_NIC_FLAGS_CKO_IP4_TX        0x00000010
#define XMP_NIC_FLAGS_CKO_TCP4_TX       0x00000020
#define XMP_NIC_FLAGS_CKO_TCP6_TX       0x00000040
#define XMP_NIC_FLAGS_CKO_UDP4_TX       0x00000080
#define XMP_NIC_FLAGS_CKO_UDP6_TX       0x00000100
#define XMP_NIC_FLAGS_CKO_IP4_RX        0x00000200
#define XMP_NIC_FLAGS_CKO_TCP4_RX       0x00000400
#define XMP_NIC_FLAGS_CKO_TCP6_RX       0x00000800
#define XMP_NIC_FLAGS_CKO_UDP4_RX       0x00001000
#define XMP_NIC_FLAGS_CKO_UDP6_RX       0x00002000
  /* Header split flags:
   *  XMP_NIC_FLAGS_RX_SPLIT: should always be set meaning splitting is ON
   *  XMP_NIC_FLAGS_RX_MS_COMBINE: can be set by NDIS via OID
   *  XMP_NIC_FLAGS_RX_SPLIT_COMBINE: is set if header split is not enabled
   *  if neither MS_COMBINE or SPLIT_COMBINE is set, it means header split
   *  is on i.e. split and indicate according to MS spec
   */
#define XMP_NIC_FLAGS_RX_SPLIT          0x00010000      /* use 3-buffer mode */
#define XMP_NIC_FLAGS_RX_SPLIT_COMBINE  0x00020000   /* ULP unaware of split */
#define XMP_NIC_FLAGS_RX_MS_COMBINE     0x00040000    /* ULP wants combining */
#define XMP_NIC_FLAGS_LRO               0x00100000
#define XMP_NIC_FLAGS_LSO               0x00200000
#define XMP_NIC_FLAGS_GSO_IP4           0x00400000
#define XMP_NIC_FLAGS_GSO_IP6           0x00800000
#define XMP_NIC_FLAGS_JUMBO             0x01000000
#define XMP_NIC_FLAGS_8021P             0x02000000
#define XMP_NIC_FLAGS_8021Q             0x04000000
#define XMP_NIC_FLAGS_MSI		0x08000000
#define XMP_NIC_FLAGS_RSS		0x10000000
#define XMP_NIC_FLAGS_RX_NBL_CHAIN      0x20000000
#define XMP_NIC_FLAGS_TX_NBL_CHAIN      0x40000000
  ULONG                  Flags;

#define XMP_NIC_STATE_READY             0x00000001
#define XMP_NIC_STATE_OK_TO_TX          0x00000002
#define XMP_NIC_STATE_OK_TO_RX          0x00000004
#define XMP_NIC_STATE_NOLINK            0x00000008
#define XMP_NIC_STATE_ERROR             0x00000010
#define XMP_NIC_LED_BLINK_ENABLED       0x00000020
#define XMP_NIC_LED_BLINK_DISABLED      0x00000040
  ULONG                  State;

#define XMP_NIC_MPSTATE_HALT            0x00000001
#define XMP_NIC_MPSTATE_PAUSING         0x00000002
#define XMP_NIC_MPSTATE_PAUSED          0x00000003
#define XMP_NIC_MPSTATE_RUNNING         0x00000004
#define XMP_NIC_MPSTATE_RESET           0x00000008
  ULONG                  mpState;
  ULONG                  LinkFlickCnt;
  ULONG                  InterruptLevel;         /* Interrupt level assigned */
  ULONG                  SGListSize;
  ULONG                  CacheFillSize;
  ULONG                  MaxFrameSz;           /* Max frame size to tx or rx */
  ULONG                  rxBufSz;   /* rx buffer size not including backfill */
  ULONG                  RxHeaderSz;    /* Header buffer size for rx buffers */
  ULONG                  RxBackfillSz;       /* Backfill size for rx buffers */
  ULONG                  LookAheadSz;
  ULONG                  PacketFilter;
  ULONG                  deviceMem1Len;
  ULONG                  deviceMem2Len;
  ULONG                  deviceMem3Len;
  ULONGLONG              deviceMem1Phy;
  ULONGLONG              deviceMem2Phy;
  ULONGLONG              deviceMem3Phy;
  VOID                  *deviceMem1;
  VOID                  *deviceMem2;
  VOID                  *deviceMem3;
  U64                    txdCkoBits;
  NDIS_INTERRUPT_TYPE    InterruptType;
  IO_INTERRUPT_MESSAGE_INFO *IntrMsgInfo;
  NDIS_MEDIA_CONNECT_STATE ReportedState;
  UCHAR                  CurrentAddress[ETH_LENGTH_OF_ADDRESS];
  UCHAR                  PermanentAddress[ETH_LENGTH_OF_ADDRESS];
  xmpAddrList_t          AddrList;
  NDIS_RW_LOCK           latch;                     /* generic adapter latch */
  /* TODO: Align */
  xmpSpinLock_t          lock;                  /* generic adapter spin lock */
  xmpNicTxFifo_t         TxFifo[XGE_HAL_MAX_FIFO_NUM];
  xmpNicRxInfo_t         RxInfo[XGE_HAL_MAX_RING_NUM];
  NDIS_OFFLOAD           OffloadCapabilities;
  NDIS_OFFLOAD           HwOffloadCapabilities;
  NDIS_TCP_CONNECTION_OFFLOAD  TcpConnectionCapabilities;
  NDIS_TCP_CONNECTION_OFFLOAD  TcpConnectionHardwareCapabilities;
  NDIS_OFFLOAD_ENCAPSULATION   OffloadEncapsulation;
  xmpRegParams_t         RegParams;
  PCI_COMMON_CONFIG      PciCfg;              /* To restore PCI config space */
  NDIS_MINIPORT_TIMER    PollTimer;
  UCHAR                  PollTimerEnabled;
  DECLSPEC_ALIGN(8)
  volatile BOOLEAN       wiQueued;
  volatile BOOLEAN       wiRunning;
  xmpOidStats_t          OidStats;
  xmpNicRssParams_t      RssParamsReq;
  xmpNicRssParams_t      RssParamsWS;
  xmpNicNdisRssSet_t     ndisRssSet;
  UCHAR                  RssITable[XMP_NIC_RSS_MAX_TABLE_SZ];
  DECLSPEC_ALIGN(8)
  ULONG                  intrDpcRun;
  ULONG                  intrSpurious;
  ULONG                  intrErr;
  ULONGLONG              intrDpcQueued;
  ULONGLONG              txIntr;
  ULONGLONG              rxIntr;
 
  NDIS_HANDLE            xmpTimerHandle;
#define XMP_POLL_TIMER_EXPIRATION -1
#define XMP_POLL_TIMER_MILLIS      500
  ULONG                  NeedsNicReset;
#ifdef MSI_DBG
  ULONG                  msiCnt[XGE_HAL_MAX_MSIX_VECTORS];
  ULONG                  msiIgnore[XGE_HAL_MAX_MSIX_VECTORS];
  LONG                   MsiDpcCnt[XGE_HAL_MAX_MSIX_VECTORS];
  LONG                   MsiDpcQCnt[XGE_HAL_MAX_MSIX_VECTORS];
  LONG                   MsiDpcXCnt[XGE_HAL_MAX_MSIX_VECTORS];
  xmpMsiStats_t          msiStats;
#endif

#ifdef XENAMP_TXDBG
  LONG                   DbgTxNBL;
  LONG                   DbgTxNB;
  LONG                   DbgTxNBLComp;
  LONG                   DbgTxNBComp;
#endif
  BOOLEAN                bInDiagMode;
  BOOLEAN                bLoopback;
  xmpDiag_t              Diag;                      

#define XMP_NIC_MAX_SERIAL_NUMBER_SIZE          16
#define XMP_NIC_VPD_READ_ONLY_CONFIG_SPACE_SIZE	128
  UCHAR                  SerialNum[XMP_NIC_MAX_SERIAL_NUMBER_SIZE];
} xmpNicCtx_t;

typedef struct _xmpNicCtx_t xFrameNic_t;

/*********************************** Macros **********************************/
#define XMP_NIC_CTX_PTR(_pNic)    (_pNic)
#define XMP_NIC_GET_NIC(_pNicCtx) (_pNicCtx)
#define XMP_NIC_GET_DEV(_pNicCtx) (_pNicCtx)->pHalDev

#define XMP_NIC_PARAM_OFFSET(_fld) ((UINT) FIELD_OFFSET(xmpRegParams_t, _fld))
#define XMP_NIC_PARAM_SIZE(_fld)   sizeof(((xmpRegParams_t *)0)->_fld)

#define XMP_NIC_REFERENCE(_pNicCtx)                                           \
  NdisInterlockedIncrement( &((_pNicCtx)->RefCnt) )

#define XMP_NIC_DEREFERENCE(_pNicCtx)                                         \
  do { LONG _val;                                                             \
    if ( !(_val = NdisInterlockedDecrement(&((_pNicCtx)->RefCnt))) )          \
        xmpNicFree(_pNicCtx);                                                 \
  } while(0)

#define XMP_NIC_READY(_pNicCtx) bit((_pNicCtx)->State, XMP_NIC_STATE_READY)
#define XMP_NIC_ACCEPT_SEND(_pNicCtx)                                         \
  bit(_pNicCtx->State, XMP_NIC_STATE_OK_TO_TX)
#define XMP_NIC_TX_FIFO_OK_TO_TX(_pFifo)                                      \
  bit(_pFifo->State, XMP_TX_FIFO_STATE_OK_TO_TX)
#define XMP_NIC_RX_RING_OK_TO_RX(_pRing)                                      \
  bit(_pRing->State, XMP_RX_RING_STATE_OK_TO_RX)
#define XMP_NIC_TX_NBL_LINK(_pNBL) ((_pNBL)->MiniportReserved[0])
#define XMP_NIC_RX_NBL_SET_MR(_pNBL, _pRxCtx)                                 \
  *((xmpRxNBLCtx_t **) &(_pNBL)->MiniportReserved[0]) = _pRxCtx
#define XMP_NIC_RX_NBL_GET_MR(_pNBL)                                          \
*((xmpRxNBLCtx_t **) &(_pNBL)->MiniportReserved[0])
#define XMP_NIC_CURRENT_MTU(_pNicCtx) (_pNicCtx)->RegParams.mtu
#define XMP_NIC_MAX_SPEED(_pNicCtx) (ULONG64)10000000000         
#define XMP_NIC_MAX_MULTICAST_LIST_SIZE(_pNic) XMP_HAL_MAX_MC_ADDRESSES
#define XMP_NIC_PERMANENT_ADDRESS(_pNicCtx) (_pNicCtx)->PermanentAddress
#define XMP_NIC_CURRENT_ADDRESS(_pNicCtx) (_pNicCtx)->CurrentAddress
#define XMP_NIC_MAC_OPTIONS(_pNic)                                            \
 (NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA |                                       \
  NDIS_MAC_OPTION_TRANSFERS_NOT_PEND  |                                       \
  NDIS_MAC_OPTION_NO_LOOPBACK         |                                       \
  NDIS_MAC_OPTION_8021P_PRIORITY)

#define XMP_NIC_MSI_IN_CONFIG(_pNicCtx)                                       \
  ((_pNicCtx)->RegParams.intr_mode != 0)
#define XMP_NIC_MSI_ENABLE(_pNicCtx)                                          \
  bis((_pNicCtx)->Flags, XMP_NIC_FLAGS_MSI)
#define XMP_NIC_MSI_DISABLE(_pNicCtx)                                         \
  bic((_pNicCtx)->Flags, XMP_NIC_FLAGS_MSI)
#define XMP_NIC_MSI_IS_ENABLED(_pNicCtx)                                      \
  bit((_pNicCtx)->Flags, XMP_NIC_FLAGS_MSI)
#define XMP_NIC_MSI_GRANTED(_pNicCtx)                                         \
  ((_pNicCtx)->InterruptType == NDIS_CONNECT_MESSAGE_BASED)
#define XMP_NIC_RSS_IN_CONFIG(_pNicCtx)                                          \
  ((_pNicCtx)->RegParams.rss_enable != 0)
#define XMP_NIC_RSS_IS_ENABLED(_pNicCtx)                                      \
  bit((_pNicCtx)->Flags, XMP_NIC_FLAGS_RSS)
#define XMP_NIC_RSS_ENABLE(_pNicCtx)                                          \
  bis((_pNicCtx)->Flags, XMP_NIC_FLAGS_RSS)
#define XMP_NIC_RSS_DISABLE(_pNicCtx)                                         \
  bic((_pNicCtx)->Flags, XMP_NIC_FLAGS_RSS)
#define XMP_NIC_PRIORITY_IS_ENABLED(_pNicCtx)                                 \
  bit((_pNicCtx)->Flags, XMP_NIC_FLAGS_8021P)
#define XMP_NIC_VLAN_IS_ENABLED(_pNicCtx)                                     \
  bit((_pNicCtx)->Flags, XMP_NIC_FLAGS_8021Q)
#define XMP_NIC_8021PQ_IS_ENABLED(_pNicCtx)                                   \
  bit((_pNicCtx)->Flags, XMP_NIC_FLAGS_8021P | XMP_NIC_FLAGS_8021Q)
  /* Header split flags:
   *  XMP_NIC_FLAGS_RX_SPLIT: should always be set meaning splitting is ON
   *  XMP_NIC_FLAGS_RX_MS_COMBINE: can be set by NDIS via OID
   *  XMP_NIC_FLAGS_RX_SPLIT_COMBINE: is set if header split is not enabled
   *  if neither MS_COMBINE or SPLIT_COMBINE is set, it means header split
   *  is on i.e. split and indicate according to MS spec
   */
#define XMP_NIC_RX_SPLIT(_pNicCtx)                                            \
  bit((_pNicCtx)->Flags, XMP_NIC_FLAGS_RX_SPLIT)
#define XMP_NIC_RX_SPLIT_COMBINE(_pNicCtx)                                    \
  bit((_pNicCtx)->Flags, XMP_NIC_FLAGS_RX_SPLIT_COMBINE)
#define XMP_NIC_RX_MS_SPLIT(_pNicCtx)                                         \
  (XMP_NIC_RX_SPLIT(_pNicCtx) && !XMP_NIC_RX_SPLIT_COMBINE(_pNicCtx))
#define XMP_NIC_RX_MS_SPLIT_COMBINE(_pNicCtx)                                 \
  bit((_pNicCtx)->Flags, XMP_NIC_FLAGS_RX_MS_COMBINE)

#define XMP_NIC_PKT_FILTERS(_pNicCtx)                                         \
  ( NDIS_PACKET_TYPE_DIRECTED      |                                          \
    NDIS_PACKET_TYPE_MULTICAST     |                                          \
    NDIS_PACKET_TYPE_ALL_MULTICAST |                                          \
    NDIS_PACKET_TYPE_BROADCAST     |                                          \
    NDIS_PACKET_TYPE_PROMISCUOUS   )

#define XMP_NIC_STATS_SUPPORTED(_pNicCtx)                                     \
  ( NDIS_STATISTICS_XMIT_OK_SUPPORTED                |                        \
    NDIS_STATISTICS_RCV_OK_SUPPORTED                 |                        \
    NDIS_STATISTICS_XMIT_ERROR_SUPPORTED             |                        \
    NDIS_STATISTICS_RCV_ERROR_SUPPORTED              |                        \
    NDIS_STATISTICS_RCV_NO_BUFFER_SUPPORTED          |                        \
    NDIS_STATISTICS_DIRECTED_BYTES_XMIT_SUPPORTED    |                        \
    NDIS_STATISTICS_DIRECTED_FRAMES_XMIT_SUPPORTED   |                        \
    NDIS_STATISTICS_MULTICAST_BYTES_XMIT_SUPPORTED   |                        \
    NDIS_STATISTICS_MULTICAST_FRAMES_XMIT_SUPPORTED  |                        \
    NDIS_STATISTICS_BROADCAST_BYTES_XMIT_SUPPORTED   |                        \
    NDIS_STATISTICS_BROADCAST_FRAMES_XMIT_SUPPORTED  |                        \
    NDIS_STATISTICS_DIRECTED_BYTES_RCV_SUPPORTED     |                        \
    NDIS_STATISTICS_DIRECTED_FRAMES_RCV_SUPPORTED    |                        \
    NDIS_STATISTICS_MULTICAST_BYTES_RCV_SUPPORTED    |                        \
    NDIS_STATISTICS_MULTICAST_FRAMES_RCV_SUPPORTED   |                        \
    NDIS_STATISTICS_BROADCAST_BYTES_RCV_SUPPORTED    |                        \
    NDIS_STATISTICS_BROADCAST_FRAMES_RCV_SUPPORTED   |                        \
    NDIS_STATISTICS_RCV_CRC_ERROR_SUPPORTED          |                        \
    NDIS_STATISTICS_TRANSMIT_QUEUE_LENGTH_SUPPORTED  |                        \
    NDIS_STATISTICS_BYTES_RCV_SUPPORTED              |                        \
    NDIS_STATISTICS_BYTES_XMIT_SUPPORTED             |                        \
    NDIS_STATISTICS_RCV_DISCARDS_SUPPORTED           |                        \
    NDIS_STATISTICS_GEN_STATISTICS_SUPPORTED )

#define XMP_NIC_GEN_STAT_FLAGS(_pNic)                                         \
  ( NDIS_STATISTICS_FLAGS_VALID_DIRECTED_FRAMES_RCV       |                   \
    NDIS_STATISTICS_FLAGS_VALID_MULTICAST_FRAMES_RCV      |                   \
    NDIS_STATISTICS_FLAGS_VALID_BROADCAST_FRAMES_RCV      |                   \
    NDIS_STATISTICS_FLAGS_VALID_BYTES_RCV                 |                   \
    NDIS_STATISTICS_FLAGS_VALID_RCV_DISCARDS              |                   \
    NDIS_STATISTICS_FLAGS_VALID_RCV_ERROR                 |                   \
    NDIS_STATISTICS_FLAGS_VALID_DIRECTED_FRAMES_XMIT      |                   \
    NDIS_STATISTICS_FLAGS_VALID_MULTICAST_FRAMES_XMIT     |                   \
    NDIS_STATISTICS_FLAGS_VALID_BROADCAST_FRAMES_XMIT     |                   \
    NDIS_STATISTICS_FLAGS_VALID_BYTES_XMIT                |                   \
    NDIS_STATISTICS_FLAGS_VALID_XMIT_ERROR                |                   \
    NDIS_STATISTICS_FLAGS_VALID_XMIT_DISCARDS             |                   \
    NDIS_STATISTICS_FLAGS_VALID_DIRECTED_BYTES_RCV        |                   \
    NDIS_STATISTICS_FLAGS_VALID_MULTICAST_BYTES_RCV       |                   \
    NDIS_STATISTICS_FLAGS_VALID_BROADCAST_BYTES_RCV       |                   \
    NDIS_STATISTICS_FLAGS_VALID_DIRECTED_BYTES_XMIT       |                   \
    NDIS_STATISTICS_FLAGS_VALID_MULTICAST_BYTES_XMIT      |                   \
    NDIS_STATISTICS_FLAGS_VALID_BROADCAST_BYTES_XMIT )

#define XMP_NIC_MATCH_PKT_FILTERS(_pNicCtx, _filter)                          \
  (_filter & ~(XMP_NIC_PKT_FILTERS(_pNicCtx)) )

#define XMP_NIC_GET_TX_BUF_SIZE(_pNicCtx)                                     \
   (((_pNicCtx)->RegParams.TxFIFONum * (_pNicCtx)->RegParams.MaxTxDs) *       \
                   XMP_NIC_CURRENT_MTU(_pNicCtx))
 /* TODO: All Rings */
#define XMP_NIC_GET_RX_BUF_SIZE(_pNicCtx)                                     \
  ((_pNicCtx)->RegParams.RxRingSz_0 * _pNicCtx->MaxFrameSz);

#define XMP_NIC_CKO_IP4_CONFIG(_pNicCtx)  ((_pNicCtx)->RegParams.IpCkoV4)
#define XMP_NIC_CKO_IP6_CONFIG(_pNicCtx)  ((_pNicCtx)->RegParams.IpCkoV6)
#define XMP_NIC_CKO_TCP4_CONFIG(_pNicCtx) ((_pNicCtx)->RegParams.TcpCkoV4)
#define XMP_NIC_CKO_TCP6_CONFIG(_pNicCtx) ((_pNicCtx)->RegParams.TcpCkoV6)
#define XMP_NIC_CKO_UDP4_CONFIG(_pNicCtx) ((_pNicCtx)->RegParams.UdpCkoV4)
#define XMP_NIC_CKO_UDP6_CONFIG(_pNicCtx) ((_pNicCtx)->RegParams.UdpCkoV6)

#define XMP_NIC_LRO_CONFIG(_pNicCtx)	\
  bit((_pNicCtx)->Flags, XMP_NIC_FLAGS_LRO )

#define XMP_NIC_LSO_CONFIG(_pNicCtx)	\
  bit((_pNicCtx)->Flags, XMP_NIC_FLAGS_LSO ) 

#define XMP_NIC_MAX_LSO_SIZE(_pNicCtx) XENAMP_LSO_MAX_SIZE
#define XMP_NIC_MIN_LSO_COUNT(_pNicCtx) 2

#define XMP_NIC_LSO_OPTS(_pNicCtx) 1

#define XMP_NIC_IPV4_GSO_CONFIG(_pNicCtx)	\
  bit((_pNicCtx)->Flags, XMP_NIC_FLAGS_GSO_IP4 ) 
#define XMP_NIC_MAX_GSO_IPV4_SIZE(_pNicCtx) XGE_HAL_SEGEMENT_OFFLOAD_MAX_SIZE
#define XMP_NIC_MIN_GSO_IPV4_COUNT(_pNicCtx) 2

#define XMP_NIC_IPV6_GSO_CONFIG(_pNicCtx)	\
  bit((_pNicCtx)->Flags, XMP_NIC_FLAGS_GSO_IP6)

#define XMP_NIC_MAX_GSO_IPV6_SIZE(_pNicCtx) XGE_HAL_SEGEMENT_OFFLOAD_MAX_SIZE
#define XMP_NIC_MIN_GSO_IPV6_COUNT(_pNicCtx) 2

#define XMP_MAC_ADDR_MATCH(_one, _two)                                        \
  ((*( ULONG UNALIGNED *)&(_one)[2] == *(ULONG UNALIGNED *)&(_two)[2])        \
     && (*(USHORT UNALIGNED*)(_one) == *(USHORT UNALIGNED *)(_two)))

#define XMP_MAC_ADDR_ZER0(_one)                                               \
  ((*( ULONG UNALIGNED *)&(_one)[2] == 0 )                                    \
     && (*(USHORT UNALIGNED*)(_one) == 0 ))

#define XMP_NIC_CKO_OFLD_MAP_TO_NDIS(_ckoInReg)                               \
  ((_ckoInReg) ?                                                              \
    (((_ckoInReg) == XENAMP_CKO_TX) ? NDIS_OFFLOAD_PARAMETERS_TX_ENABLED_RX_DISABLED :         \
      ((_ckoInReg) == XENAMP_CKO_RX) ? NDIS_OFFLOAD_PARAMETERS_RX_ENABLED_TX_DISABLED :        \
     NDIS_OFFLOAD_PARAMETERS_TX_RX_ENABLED) :                                 \
     NDIS_OFFLOAD_PARAMETERS_TX_RX_DISABLED)

#define XMP_NIC_CKO_OFLD_MAP_FROM_NDIS(_ckoNdis)                              \
    ((_ckoNdis) == NDIS_OFFLOAD_PARAMETERS_TX_RX_DISABLED ? XENAMP_CKO_DISABLED :              \
     ((_ckoNdis) == NDIS_OFFLOAD_PARAMETERS_TX_ENABLED_RX_DISABLED) ? XENAMP_CKO_TX :          \
     ((_ckoNdis) == NDIS_OFFLOAD_PARAMETERS_RX_ENABLED_TX_DISABLED) ? XENAMP_CKO_RX :          \
     XENAMP_CKO_TXRX)   

#define XMP_NIC_GET_ADMIN_OFFLOADS(_pNicCtx, _pAdmin)                         \
  do {                                                                        \
    (_pAdmin)->IPv4Checksum  =                                                \
      XMP_NIC_CKO_OFLD_MAP_TO_NDIS(XMP_NIC_CKO_IP4_CONFIG(_pNicCtx));         \
    (_pAdmin)->TCPIPv4Checksum  =                                             \
      XMP_NIC_CKO_OFLD_MAP_TO_NDIS(XMP_NIC_CKO_TCP4_CONFIG(_pNicCtx));        \
    (_pAdmin)->TCPIPv6Checksum  =                                             \
      XMP_NIC_CKO_OFLD_MAP_TO_NDIS(XMP_NIC_CKO_TCP6_CONFIG(_pNicCtx));        \
    (_pAdmin)->UDPIPv4Checksum  =                                             \
      XMP_NIC_CKO_OFLD_MAP_TO_NDIS(XMP_NIC_CKO_UDP4_CONFIG(_pNicCtx));        \
    (_pAdmin)->UDPIPv6Checksum  =                                             \
      XMP_NIC_CKO_OFLD_MAP_TO_NDIS(XMP_NIC_CKO_UDP6_CONFIG(_pNicCtx));        \
    (_pAdmin)->LsoV1 = (XMP_NIC_LSO_CONFIG(_pNicCtx) ?                        \
                        NDIS_OFFLOAD_PARAMETERS_LSOV1_ENABLED :               \
                        NDIS_OFFLOAD_PARAMETERS_LSOV1_DISABLED);              \
    (_pAdmin)->LsoV2IPv4 = (XMP_NIC_IPV4_GSO_CONFIG(_pNicCtx) ?               \
                        NDIS_OFFLOAD_PARAMETERS_LSOV2_ENABLED :               \
                        NDIS_OFFLOAD_PARAMETERS_LSOV2_DISABLED );             \
    (_pAdmin)->LsoV2IPv6 = (XMP_NIC_IPV6_GSO_CONFIG(_pNicCtx) ?               \
                         NDIS_OFFLOAD_PARAMETERS_LSOV2_ENABLED:               \
                         NDIS_OFFLOAD_PARAMETERS_LSOV2_DISABLED);             \
    (_pAdmin)->IPsecV1 = NDIS_OFFLOAD_PARAMETERS_IPSECV1_DISABLED;  \
  } while( 0 )

#define XMP_NIC_SET_ADMIN_OFFLOADS(_pNicCtx, _pAdmin)                         \
  do {                                                                        \
    if ( (pAdmin)->IPv4Checksum != NDIS_OFFLOAD_PARAMETERS_NO_CHANGE ) {      \
      XMP_NIC_CKO_IP4_CONFIG(_pNicCtx) =                                      \
        XMP_NIC_CKO_OFLD_MAP_FROM_NDIS((pAdmin)->IPv4Checksum);               \
    }                                                                         \
    if ( (pAdmin)->TCPIPv4Checksum != NDIS_OFFLOAD_PARAMETERS_NO_CHANGE ) {        \
      XMP_NIC_CKO_TCP4_CONFIG(_pNicCtx) =                                     \
        XMP_NIC_CKO_OFLD_MAP_FROM_NDIS((pAdmin)->TCPIPv4Checksum);            \
    }                                                                         \
    if ( (pAdmin)->TCPIPv6Checksum != NDIS_OFFLOAD_PARAMETERS_NO_CHANGE ) {        \
      XMP_NIC_CKO_TCP6_CONFIG(_pNicCtx) =                                     \
        XMP_NIC_CKO_OFLD_MAP_FROM_NDIS((pAdmin)->TCPIPv6Checksum);            \
    }                                                                         \
    if ( (pAdmin)->UDPIPv4Checksum != NDIS_OFFLOAD_PARAMETERS_NO_CHANGE ) {   \
      XMP_NIC_CKO_UDP4_CONFIG(_pNicCtx) =                                     \
        XMP_NIC_CKO_OFLD_MAP_FROM_NDIS((pAdmin)->UDPIPv4Checksum);            \
    }                                                                         \
    if ( (pAdmin)->UDPIPv6Checksum != NDIS_OFFLOAD_PARAMETERS_NO_CHANGE ) {   \
      XMP_NIC_CKO_UDP6_CONFIG(_pNicCtx) =                                     \
        XMP_NIC_CKO_OFLD_MAP_FROM_NDIS((pAdmin)->UDPIPv6Checksum);            \
    }                                                                         \
    if ( (pAdmin)->LsoV1 != NDIS_OFFLOAD_PARAMETERS_NO_CHANGE ) {             \
      if ( (pAdmin)->LsoV1 == NDIS_OFFLOAD_PARAMETERS_LSOV1_ENABLED )         \
        bis((_pNicCtx)->Flags, XMP_NIC_FLAGS_LSO);                            \
      else                                                                    \
        bic((_pNicCtx)->Flags, XMP_NIC_FLAGS_LSO);                            \
    }                                                                         \
  } while( 0 )
/* TODO: how about GSO in above                                                */
VOID xmpNicDisableInt(
  IN xmpNicCtx_t *pNic
  );

/*__inline VOID xmpNicEnableInt(*/
VOID xmpNicEnableInt(
  IN xmpNicCtx_t *pNic
  );
 
#ifndef NET_BUFFER_LIST_COMPLETION_STATUS
#define NET_BUFFER_LIST_COMPLETION_STATUS NET_BUFFER_LIST_STATUS
#endif

/****************************** Public Functions******************************/
xmpNicCtx_t *
xmpNicAlloc(
  IN  NDIS_HANDLE             hAdapter
  );

VOID
xmpNicFree(
  IN  xmpNicCtx_t            *pNicCtx
  );

NDIS_STATUS
xmpNicReadConfig(
  IN  xmpNicCtx_t            *pNicCtx
  );

NDIS_STATUS 
xmpNicLookup(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  NDIS_RESOURCE_LIST     *Resources
  );
NDIS_STATUS
xmpNicTxRxAlloc(
  IN  xmpNicCtx_t            *pNicCtx
  );

VOID 
xmpNicShmAllocCompleteCB(
  IN  NDIS_HANDLE             MPNicCtx,
  IN  VOID                   *pVa,
  IN  NDIS_PHYSICAL_ADDRESS  *Pa,
  IN  ULONG                   Len,
  IN  VOID                   *pContext
  );

MINIPORT_PROCESS_SG_LIST xmpProcessSGListCB;

VOID
xmpNicTxNBL(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  NET_BUFFER_LIST        *pNBL,
  IN  ULONG                   SendFlags
  );

VOID
xmpNicRxIndicate(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  UINT                    RingNum
  );

VOID
xmpNicInitRssCapabilities(
  IN  xmpNicCtx_t *pNicCtx,
  OUT NDIS_RECEIVE_SCALE_CAPABILITIES *pCapabilities
  );

VOID
xmpNicInitOffloadCapabilities(
  IN  xmpNicCtx_t *pNicCtx,
  OUT PNDIS_OFFLOAD OffloadCapabilities
  );

VOID
xmpNicInitHwOffloadCapabilities(
  IN  xmpNicCtx_t *pNicCtx,
  OUT PNDIS_OFFLOAD OffloadCapabilities
  );

VOID
xmpNicSetMPState(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  ULONG                   State,
  IN  BOOLEAN                 DispatchLevel
  );

VOID
xmpNicIndicateStatus(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  BOOLEAN                 bLink
  );

VOID
xmpNicTxFifoDrainQ(
  IN  xmpNicCtx_t            *pNicCtx,
  IN  UINT                    FifoNum,
  IN  BOOLEAN                 DispatchLevel
  );

NDIS_IO_WORKITEM_FUNCTION xmpNicTxFlushCB;
VOID
xmpNicSetUtilParams(
  IN  xmpNicCtx_t            *pNicCtx
  );

NDIS_IO_WORKITEM_FUNCTION xmpNicSetRssParameters;

VOID
xmpNicSetCkoFlags(
  IN xmpNicCtx_t            *pNicCtx
  );

NDIS_STATUS
xmpNicReadIntrMode(
   IN NDIS_HANDLE            NdisHandle,
   OUT UINT                  *Value
   );

VOID
xmpNicStartTimer(
    IN xmpNicCtx_t          *pNicCtx
);

VOID
xmpNicFreeTimer(
    IN xmpNicCtx_t          *pNicCtx
);

NDIS_TIMER_FUNCTION xmpNicTimerCallback;

#endif

