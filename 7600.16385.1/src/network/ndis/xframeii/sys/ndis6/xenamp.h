#ifndef XENAMP_H
#define XENAMP_H

/********************************** Constants ********************************/
#define XENAMP_NDIS_MAJOR_VERSION       0x06
#define XENAMP_NDIS_MINOR_VERSION       0x01 /* This will need to be changed if
						Vista is desired  */

#define XENAMP_DRIVER_VERSION          ((XENAMP_MAJOR_DRIVER_VERSION << 16) | \
                                         XENAMP_MINOR_DRIVER_VERSION)
#define XENAMP_VENDOR_ID                0x17D5
#define XENAMP_VENDOR_DESC              "MICROSOFT"
#define XENAMP_MTAG                     'ecaf'
#define XENAMP_NDIS_MEDIA_TYPE          NdisMedium802_3
#define XENAMP_NDIS_ATTRIBUTES                                                \
        (NDIS_MINIPORT_ATTRIBUTES_HARDWARE_DEVICE |                           \
         NDIS_MINIPORT_ATTRIBUTES_BUS_MASTER)
       /*NDIS_MINIPORT_ATTRIBUTES_NO_HALT_ON_SUSPEND)*/
#define XENAMP_CHECK_FOR_HANG_TO        1                      /* In seconds */
#define XENAMP_INTERFACE_TYPE           NdisInterfacePci

#ifdef XENAMP_RX_ALIGN_SZ
# define XENAMP_DATA_ALIGN              XENAMP_RX_ALIGN_SZ
#else
# define XENAMP_DATA_ALIGN              8             /* Eth Frame alignment */
#endif /* XENAMP_RX_ALIGN_SZ */

#define XENAMP_RXBUF_ETH_HDR_BACKFILL_SZ 32

#define XENAMP_RXBUF_BLOCK_SIZE(frmsz, align)                                 \
   ((frmsz + align + sizeof(xmpRxBufBlk_t))                                   \
    <= PAGE_SIZE ? PAGE_SIZE :                                                \
    (frmsz + align + sizeof(xmpRxBufBlk_t)))

#define XENAMP_ETH_HDR_LEN              14
#define XENAMP_MAX_ETH_HDR_LEN          22

#define XENAMP_L2_HDR_LEN               20
#define XENAMP_L3_HDR_LEN               20
#define XENAMP_L2_L3_HDR_LEN            (XENAMP_L2_HDR_LEN + XENAMP_L3_HDR_LEN)


#define XENAMP_MIN_RXDS                 127
#define XENAMP_DEFAULT_RING_QUEUE_BLOCKS 50

#ifdef XMP_RX_BUF_SPLIT
#define XENAMP_NUM_OF_RXDS_IN_BLOCK     85
#define XENAMP_DEFAULT_RXDS            (XENAMP_NUM_OF_RXDS_IN_BLOCK * XENAMP_DEFAULT_RING_QUEUE_BLOCKS)
#define XENAMP_MAX_RXDS                (XENAMP_NUM_OF_RXDS_IN_BLOCK * XGE_HAL_MAX_RING_QUEUE_BLOCKS)
#else
#define XENAMP_NUM_OF_RXDS_IN_BLOCK    127
#define XENAMP_DEFAULT_RXDS            (XENAMP_NUM_OF_RXDS_IN_BLOCK * XENAMP_DEFAULT_RING_QUEUE_BLOCKS)
#define XENAMP_MAX_RXDS                (XENAMP_NUM_OF_RXDS_IN_BLOCK * XGE_HAL_MAX_RING_QUEUE_BLOCKS)
#endif

#define XENAMP_LSO_MAX_SIZE             65535

#define XENAMP_CKO_DISABLED             0
#define XENAMP_CKO_TX                   1
#define XENAMP_CKO_RX                   2
#define XENAMP_CKO_TXRX                 3

#define XENAMP_PRI_VLAN_DISABLED        0
#define XENAMP_PRI_ENABLED              1
#define XENAMP_VLAN_ENABLED             2
#define XENAMP_PRI_VLAN_ENABLED         3

#define XENAMP_RX_SPLIT_NONE            0
#define XENAMP_RX_SPLIT                 1
#define XENAMP_RX_SPLIT_COMBINE         2

#define XENAMP_LRO_DISABLED             0
#define XENAMP_LRO_ENABLED              1

#define XENAMP_LINK_LED_BLINK_ENABLED   0
#define XENAMP_LINK_LED_BLINK_DISABLED	1

/* Error log codes */
#define ERRLOG_READ_PCI_SLOT_FAILED     0x00000101L
#define ERRLOG_WRITE_PCI_SLOT_FAILED    0x00000102L
#define ERRLOG_VENDOR_DEVICE_NOMATCH    0x00000103L

// NDIS_ERROR_CODE_ADAPTER_DISABLED
#define ERRLOG_BUS_MASTER_DISABLED      0x00000201L

// NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION
#define ERRLOG_INVALID_SPEED_DUPLEX     0x00000301L
#define ERRLOG_SET_SECONDARY_FAILED     0x00000302L

// NDIS_ERROR_CODE_OUT_OF_RESOURCES
#define ERRLOG_OUT_OF_MEMORY            0x00000401L
#define ERRLOG_OUT_OF_SHARED_MEMORY     0x00000402L
#define ERRLOG_OUT_OF_MAP_REGISTERS     0x00000403L
#define ERRLOG_OUT_OF_BUFFER_POOL       0x00000404L
#define ERRLOG_OUT_OF_NDIS_BUFFER       0x00000405L
#define ERRLOG_OUT_OF_PACKET_POOL       0x00000406L
#define ERRLOG_OUT_OF_NDIS_PACKET       0x00000407L
#define ERRLOG_OUT_OF_LOOKASIDE_MEMORY  0x00000408L
#define ERRLOG_OUT_OF_SG_RESOURCES      0x00000409L

// NDIS_ERROR_CODE_HARDWARE_FAILURE
#define ERRLOG_SELFTEST_FAILED          0x00000501L
#define ERRLOG_INITIALIZE_ADAPTER       0x00000502L
#define ERRLOG_REMOVE_MINIPORT          0x00000503L

// NDIS_ERROR_CODE_RESOURCE_CONFLICT
#define ERRLOG_MAP_IO_SPACE             0x00000601L
#define ERRLOG_QUERY_ADAPTER_RESOURCES  0x00000602L
#define ERRLOG_NO_IO_RESOURCE           0x00000603L
#define ERRLOG_NO_INTERRUPT_RESOURCE    0x00000604L
#define ERRLOG_NO_MEMORY_RESOURCE       0x00000605L

/****************************** Macro definitions ****************************/

#define XMP_LOG_ERROR(_pNicCtx, _Code, _Cnt, _Val)                            \
  NdisWriteErrorLogEntry(_pNicCtx, _Code, _Cnt, _Val)

/* Net buffer processing related */

#define XMP_RX_NBL_LINK(_pRing, _pNBLCtx)                                     \
  do {                                                                        \
      if ( !(_pRing)->pPrvNBL )                                               \
        (_pRing)->pNBL = (_pNBLCtx)->pNBL;                                    \
      else                                                                    \
        NET_BUFFER_LIST_NEXT_NBL((_pRing)->pPrvNBL) = (_pNBLCtx)->pNBL;       \
      (_pRing)->pPrvNBL = (_pNBLCtx)->pNBL;                                   \
      (_pRing)->NBLCount++;                                                   \
  } while( 0 )

#define XMP_TX_NBL_REFERENCE(_pNBLCtx)                                        \
    NdisInterlockedIncrement(&((pNBLCtx)->RefCnt))


# define XMP_TX_COMPLETE_FINAL(_pNicCtx, _pFifo, _bDpcLevel)                  \
  do {                                                                        \
    NET_BUFFER_LIST       *_pNBLx;                                            \
    XF_GET_SLOCK(&((_pFifo)->TxLock));                                        \
      _pNBLx = (_pFifo)->pNblTxC;                                             \
      (_pFifo)->pNblTxC = (_pFifo)->pNblTxCLast = NULL;                       \
    XF_FREE_SLOCK(&((_pFifo)->TxLock));                                       \
    if ( _pNBLx )                                                             \
    {                                                                         \
      XENAMP_TX_NBL_COMPLETE_UPCALL(_pNicCtx, _pNBLx,                         \
				      _bDpcLevel ?                            \
				      NDIS_SEND_COMPLETE_FLAGS_DISPATCH_LEVEL:\
				      0);                                     \
    }                                                                         \
  } while(0)

# define XMP_TX_NBL_DEREFERENCE_COAL(_pNicCtx, _pFifo, _pNBLCtx, _bDpcLevel, _Flsh)\
  do { LONG _val;                                                             \
   XMPTRACE(XMP_DBG_TX, ("XMP_TX_NBL_DEREFERENCE\n"));                        \
    if ( !(_val = NdisInterlockedDecrement(&((_pNBLCtx)->RefCnt))) )          \
    { NET_BUFFER_LIST *_pNBL = (_pNBLCtx)->pNBL;                              \
      xmpTxNBufCtx_t  *_pNBufCtx;                                             \
      xmpQLink_t      *_link;                                                 \
      (_pFifo)->Stats.TxNBLComp++;                                            \
      XF_GET_SLOCK_LEVEL(&((_pFifo)->TxLock), _bDpcLevel);                    \
        if ( !(_pFifo)->pNblTxC )                                             \
          (_pFifo)->pNblTxC = _pNBL;                                          \
        else                                                                  \
          NET_BUFFER_LIST_NEXT_NBL((_pFifo)->pNblTxCLast) = _pNBL;            \
        (_pFifo)->pNblTxCLast = _pNBL;                                        \
       XF_Q_ADD_TAIL(&_pFifo->NBLCtxQ, &_pNBLCtx->Link);                      \
      XF_FREE_SLOCK_LEVEL(&((_pFifo)->TxLock), _bDpcLevel);                   \
      XF_ATOMIC_DEC31((_pFifo)->OsSendCnt);                                   \
      if ( _Flsh )                                                            \
        XMP_TX_COMPLETE_FINAL(_pNicCtx, _pFifo, _bDpcLevel);                  \
      XMPTRACE(XMP_DBG_TX, ("XMP_TX_NBL_DEREFERENCE: Completed\n"));          \
   }                                                                          \
  } while(0)


#define XMP_TX_NBL_DEREFERENCE(_pNicCtx, _pFifo, _pNBLCtx, _bDpcLevel)        \
  do { LONG _val;                                                             \
   XMPTRACE(XMP_DBG_TX, ("XMP_TX_NBL_DEREFERENCE\n"));                        \
    if ( !(_val = NdisInterlockedDecrement(&((_pNBLCtx)->RefCnt))) )          \
    { NET_BUFFER_LIST *_pNBL = (_pNBLCtx)->pNBL;                              \
      xmpTxNBufCtx_t  *_pNBufCtx;                                             \
      xmpQLink_t      *_link;                                                 \
      (_pFifo)->Stats.TxNBLComp++;                                            \
      XENAMP_TX_NBL_COMPLETE_UPCALL(_pNicCtx, _pNBL,                          \
                                      _bDpcLevel ?                            \
				      NDIS_SEND_COMPLETE_FLAGS_DISPATCH_LEVEL:\
				      0);                                     \
      XF_GET_SLOCK_LEVEL(&((_pFifo)->TxLock), _bDpcLevel);                    \
        XF_Q_ADD_TAIL(&_pFifo->NBLCtxQ, &_pNBLCtx->Link);                     \
      XF_FREE_SLOCK_LEVEL(&((_pFifo)->TxLock), _bDpcLevel);                   \
      XF_ATOMIC_DEC31((_pFifo)->OsSendCnt);                                   \
      XMPTRACE(XMP_DBG_TX, ("XMP_TX_NBL_DEREFERENCE: Completed\n"));          \
   }                                                                          \
  } while(0)


/********************************** Shorthand ********************************/
#define XENAMP_NDIS_OBJECT_VALID(_phdr, _rev, _type, _size)                   \
  ( ((_phdr)->Revision == _rev)                                               \
   && ((_phdr)->Type == _type)                                                \
   && ((_phdr)->Size == _size) )

#define XENAMP_NDIS_OBJECT_INIT(_phdr, _type, _rev, _size)                    \
  (_phdr)->Revision = _rev, (_phdr)->Type = _type, (_phdr)->Size = _size

#define XMP_PARAM_OFFSET(_param)                                              \
  ((UINT) FIELD_OFFSET(xmpRegParams_t, (_param)))

#define XMP_PARAM_SIZE(_param)                                                \
  ((UINT) sizeof(((xmpRegParams_t *) 0)->_param))

#define XMP_PTR_DIFF(_PostVa, _PreVa) ((PUCHAR)(_PostVa) - (PUCHAR)(_PreVa))

    /* TODO: Correct ??? */
#define XMP_ALIGN_VA(_Va, _align)     ((PVOID)(((ULONG_PTR)(_Va) +            \
                                                (_align - 1)) &               \
                                               ~((ULONG_PTR)_align - 1)))

#define XMP_ALIGN(_val, _align)       (((_val) + ((_align) - 1)) &            \
                                       ~((_align) - 1))

#define XMP_RXBUF_ALIGN_SZ(_MinAlign) MAX(_MinAlign, XENAMP_DATA_ALIGN)

#define XMP_TX_NB_DATA_LEN_OK(_pNicCtx, _pNB)                                 \
  (NET_BUFFER_DATA_LENGTH(_pNB) > (_pNicCtx)->MaxFrameSz ?                    \
   NDIS_STATUS_INVALID_LENGTH : NDIS_STATUS_SUCCESS)

#define XMP_SET_VLAN_TAG(_val16, _pri, _vlan)                                 \
  do {                                                                        \
    _val16 = (USHORT)(((UCHAR)(_pri) << 5) | ((UCHAR) ((_vlan) >> 8) & 0x0f));\
    _val16 = (_val16 << 8) | (USHORT) ((UCHAR)(_vlan) & 0xff);                \
  } while(0)

#define XMP_GET_NBL_8021Q_INFO(_pNBLCtx)                                      \
  do { NDIS_PACKET_8021Q_INFO inf;                                            \
       inf.Value = NET_BUFFER_LIST_INFO((_pNBLCtx)->pNBL,                     \
                                        Ieee8021QNetBufferListInfo);          \
       XMP_SET_VLAN_TAG((_pNBLCtx)->vTagInfo, inf.TagHeader.UserPriority,     \
                        inf.TagHeader.VlanId);                                \
  } while(0)

#define XMP_SET_NBL_8021Q_INFO(_tag, _taghdr)                                 \
  do {                                                                        \
    UCHAR *tagbuf = (UCHAR *)_tag;                                            \
    (_taghdr)->TagHeader.VlanId = (USHORT)*tagbuf;                            \
    tagbuf++;                                                                 \
    (_taghdr)->TagHeader.VlanId |= (USHORT)(*tagbuf & 0xf) << 8;              \
    (_taghdr)->TagHeader.UserPriority = (*tagbuf & 0xe0) >> 5;                \
    (_taghdr)->TagHeader.CanonicalFormatId = (*tagbuf & 0x10) >> 4;           \
  } while(0)

#define XMP_GET_NBL_LSO_GSO_INFO(_pNBLCtx)                                    \
  do {                                                                        \
    NDIS_TCP_LARGE_SEND_OFFLOAD_NET_BUFFER_LIST_INFO LsoGsoInfo;              \
    LsoGsoInfo.Value =                                                        \
              (NET_BUFFER_LIST_INFO((_pNBLCtx)->pNBL,                         \
				      TcpLargeSendNetBufferListInfo));        \
    (_pNBLCtx)->Mss =                                                         \
      LsoGsoInfo.Transmit.Type == NDIS_TCP_LARGE_SEND_OFFLOAD_V2_TYPE ?       \
      LsoGsoInfo.LsoV2Transmit.MSS : LsoGsoInfo.LsoV1Transmit.MSS;            \
  } while(0)

/***********
#define XMP_GET_NBL_LSO_GSO_INFO(_pNBLCtx)                                    \
  do { NDIS_TCP_SEND_OFFLOAD_NET_BUFFER_LIST_INFO LsoGsoInfo;                 \
     LsoGsoInfo.Value = PtrToUlong(NET_BUFFER_LIST_INFO((_pNBLCtx)->pNBL,     \
                                   TcpLargeSendNetBufferListInfo));           \
     (_pNBLCtx)->Mss =                                                        \
       ( LsoGsoInfo.Transmit.Type == NDIS_TCP_GIANT_SEND_OFFLOAD_TYPE ) ?     \
         LsoGsoInfo.GSOTransmit.MSS : LsoGsoInfo.Transmit.Type ==  )        ((PVOID)(((ULONG_PTR)(_Va) +            \
                                                (_align - 1)) &               \
                                               ~((ULONG_PTR)_align - 1)))
********/

#define XMP_RXBUF_ALIGN_SZ(_MinAlign) MAX(_MinAlign, XENAMP_DATA_ALIGN)

#define XMP_TX_NB_DATA_LEN_OK(_pNicCtx, _pNB)                                 \
  (NET_BUFFER_DATA_LENGTH(_pNB) > (_pNicCtx)->MaxFrameSz ?                    \
   NDIS_STATUS_INVALID_LENGTH : NDIS_STATUS_SUCCESS)

#define XMP_SET_VLAN_TAG(_val16, _pri, _vlan)                                 \
  do {                                                                        \
    _val16 = (USHORT)(((UCHAR)(_pri) << 5) | ((UCHAR) ((_vlan) >> 8) & 0x0f));\
    _val16 = (_val16 << 8) | (USHORT) ((UCHAR)(_vlan) & 0xff);                \
  } while(0)

#define XMP_CLEAR_LSO_INFO(_pNBLCtx)                                          \
  NET_BUFFER_LIST_INFO((_pNBLCtx)->pNBL, TcpLargeSendNetBufferListInfo) = 0;

#define XMP_GET_NBL_CSUM_INFO(_pNBLCtx)                                       \
   do {                                                                       \
    NDIS_TCP_IP_CHECKSUM_PACKET_INFO _cinfo;                                  \
    _cinfo.Value = PtrToUlong(NET_BUFFER_LIST_INFO((_pNBLCtx)->pNBL,          \
                     TcpIpChecksumNetBufferListInfo));                        \
    (_pNBLCtx)->CkoInfo = 0;                                                  \
   if ( _cinfo.Transmit.NdisPacketChecksumV4 &&                               \
        _cinfo.Transmit.NdisPacketIpChecksum )                                \
      (_pNBLCtx)->CkoInfo |= XGE_HAL_TXD_TX_CKO_IPV4_EN;                      \
   if ( _cinfo.Transmit.NdisPacketTcpChecksum )                               \
      (_pNBLCtx)->CkoInfo |= XGE_HAL_TXD_TX_CKO_TCP_EN;                       \
   if (_cinfo.Transmit.NdisPacketUdpChecksum )                                \
      (_pNBLCtx)->CkoInfo |= XGE_HAL_TXD_TX_CKO_UDP_EN;                       \
   } while(0)

#ifdef XENAMP_TXDBG
# define XENAMP_TX_NBL_COMPLETE_UPCALL(_pNicCtx, _pNBL, _flags)               \
  xenampSendNetBufferListsComplete(_pNicCtx, _pNBL, _flags)
#else
# define XENAMP_TX_NBL_COMPLETE_UPCALL(_pNicCtx, _pNBL, _flags)               \
  NdisMSendNetBufferListsComplete((_pNicCtx)->hMPAdapter,                     \
                                   _pNBL,                                     \
                                   _flags)
#endif

/******************************* Structure Defn ******************************/
typedef struct _xmpNicCtx_t xmpNicCtx_t;
typedef struct _xmpNicTxFifo_t xmpNicTxFifo_t;

typedef struct _xmpRxBufBlk_t
{
  xmpQLink_t             Link;
  NDIS_PHYSICAL_ADDRESS  PhyAddr;
  DECLSPEC_ALIGN(MEMORY_ALLOCATION_ALIGNMENT)
  UCHAR                  Buf[];
} xmpRxBufBlk_t;

typedef struct _xmpRxNBLCtx_t
{
  xmpQLink_t             NBLQLink;              /* Link for all Rx NBL Queue */
  NET_BUFFER_LIST       *pNBL;                        /* Back Pointer to NBL */
  MDL                   *pBfillMDL;              /* MDL for entire rx buffer */
  MDL                   *pDataMDL;  /* rx buffer MDL at backfill size offset */
  UCHAR                 *pRxBuf;                 /* rx buffer virtal address */

  struct _xmpRxNBLCtx_t *lroNext;           /* next NBL context in LRO chain */
  MDL                   *pLastMDL;       /* last buffer MDL for LRO chaining */

  U64                    RxPa;   /* Receive buffer Phy addr @backfill offset */
  U32                    RxRing;                /* Posted on which Rx Ring # */
  ULONG                  idxHdrMDL;  /* index in hdrMDLsArr[] for header MDL */
  ULONG                  advance;             /* NB/MDL advanced(skipped) by */
#define XMP_RXNBL_STATE_FREE   0
#define XMP_RXNBL_STATE_POSTED 1
#define XMP_RXNBL_STATE_ULP    2
  ULONG                  state;

  /* Make sure it is a multiple of MEMORY_ALLOCATION_ALIGNMENT               */
  DECLSPEC_ALIGN(MEMORY_ALLOCATION_ALIGNMENT)
    UCHAR*                Pad;
} xmpRxNBLCtx_t;

typedef struct _xmpTxNBLCtx_t
{
  xmpQLink_t             Link;
  xmpNicCtx_t           *pNicCtx;
  NET_BUFFER_LIST       *pNBL;
  NET_BUFFER            *pNetBuf;
  LONG                   RefCnt;
  U64                    CkoInfo;
  ULONG                  Mss;
  U16                    TxQueue;
  U16                    vTagInfo;
} xmpTxNBLCtx_t;

typedef struct _xmpTxNBufCtx_t
{
  xmpNicCtx_t           *pNicCtx;                     /* NIC context pointer */
  xmpNicTxFifo_t        *pFifo;
  xmpTxNBLCtx_t         *pNBLCtx;                 /* NB List Context pointer */
  NET_BUFFER            *pNetBuf;                           /* NB associated */
  SCATTER_GATHER_LIST   *pSGList;         /* not necessarily same as pSglBuf */
  xge_hal_dtr_h          dtr;
  xmpQLink_t             Link;                   /* for NBufCtxQ list in TCB */
#define XMP_TX_NBUF_FLAG_NONE 0x0
#define XMP_TX_NBUF_FLAG_DIAG 0x1
  ULONG                  Flags;
  ULONG                  BytesSent;
#ifdef DBG
  ULONG                  SglCnt;
#endif
  DECLSPEC_ALIGN(MEMORY_ALLOCATION_ALIGNMENT)
  UCHAR*                  Pad;       /* to get all objects in array aligned */
} xmpTxNBufCtx_t;

typedef struct _xmpRegEntry_t
{
    NDIS_STRING RegName;
    BOOLEAN bRequired;
    UINT FieldOffset;
    UINT FieldSize;
    UINT Default;
    UINT Min;
    UINT Max;
} xmpRegEntry_t;

/****************************** Public Functions******************************/
VOID
xenaMpSleep(
  IN  UINT              usec
  );

VOID
xenampSendNetBufferListsComplete(
  IN xmpNicCtx_t             *pNicCtx,
  IN PNET_BUFFER_LIST         pNBL,
  IN  ULONG                   SendFlags
  );

MINIPORT_RETURN_NET_BUFFER_LISTS xenaMpReturnNetBufferLists;
#endif /* XENAMP_H */

