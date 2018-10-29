#ifndef XMPREQ_H
#define XMPREQ_H

typedef struct _xmpRxStats
{
  LONG                  rxRingCnt;
  xmpRxRingStats_t      rxStats[XGE_HAL_MAX_RING_NUM];
} xmpRxStats_t;

typedef struct _xmpSetRssContext
{
  xmpNicCtx_t            *pNicCtx;
  NDIS_RECEIVE_SCALE_PARAMETERS *pParams;
} xmpSetRssContext;

MINIPORT_OID_REQUEST xenaMpRequest;

MINIPORT_CANCEL_OID_REQUEST xenaMpCancelRequest;

VOID
xmpTmpRssRegDump(
  xmpNicCtx_t                *pNicCtx
  );

NDIS_OID *
xmpReqGetSupportedOIDList();

ULONG
xmpReqGetSupportedOIDListLength();

MINIPORT_SYNCHRONIZE_INTERRUPT xmpReqSetRss;
#endif

