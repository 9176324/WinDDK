#ifndef XMPGBL_H
#define XMPGBL_H

extern NDIS_HANDLE     xmpDriverContext;
extern NDIS_HANDLE     xmpDriverHandle;
extern xmpSpinLock_t   xmpLock;
extern xmpQLink_t      xmpNicsList;
extern ULONG           xmpDebugLevel;

#endif

