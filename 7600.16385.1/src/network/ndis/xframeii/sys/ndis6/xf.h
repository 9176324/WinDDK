#ifndef XF_H
#define XF_H

#define XENAMP_PCI_VENDOR_ID            0x17D5
#define XENAMP_PCI_DEVICE_ID_WIN        0x5731
#define XENAMP_PCI_DEVICE_ID_UNIX       0x5831
#define XENAMP_PCI_DEVICE_ID_HERC       0x5832
#define XENAMP_MTAG                     'ecaf'

#ifndef MIN
# define MIN(_a, _b)           ((_a) > (_b) ? _b : _a)
#endif

#ifndef MAX
# define MAX(_a, _b)           ((_a) < (_b) ? _b : _a)
#endif

#define XF_ATOMIC_INC31(_val) NdisInterlockedIncrement(&(_val))
#define XF_ATOMIC_DEC31(_val) NdisInterlockedDecrement(&(_val))
/* Keep a 32-bit running counter using Interlocaked increment. If the counter
 * is ready to wrap around, then use a lock and add the 32-bit counter to
 * 64-bit counter.
 */
#define XF_ATOMIC_INC64(_cnt64, _cnt32, _pLatch)                              \
  do {                                                                        \
      if (NdisInterlockedIncrement(&(_cnt32)) == 0x7ffffffe)                  \
      {                                                                       \
        XMP_GET_XCLUSIVE_LATCH(_pLatch)                                       \
          (_cnt64) += 0x7ffffffe;                                             \
          do (_cnt32) = 0;  while(_cnt32);                                    \
        XMP_FREE_XCLUSIVE_LATCH(_pLatch)                                      \
      }                                                                       \
  } while(0)

#define XF_ATOMIC_INC32(_largeInt, _val32)                                    \
   NdisInterlockedAddLargeInteger(&(_largeInt), _val32)

/* Bit operations */
#define bit(_flg, _val) ((_flg) & (_val))
#define bitall(_flg, _val) (((_flg) & (_val)) == (_val))
#define bis(_flg, _val) ((_flg) |= (_val))
#define bic(_flg, _val) ((_flg) &= (~(_val)))

/* Queuing */
#define XF_Q_INIT(h)         ((h)->blink = (h)->flink = (h))
#define XF_Q_IS_NULL(h)     ((h)->flink == (h))
#define XF_Q_IS_QUEUED(l)   ((l)->flink && ((l)->flink != (l)))
#define XF_Q_NEXT(h, l)     (((l)->flink == (h)) ? NULL : (l)->flink)
#define XF_Q_ADD_TAIL(h,l)  do { (l)->flink = (h); (l)->blink = (h)->blink;   \
                                  (l)->blink->flink = (h)->blink = (l);       \
                             } while(0)
#define XF_Q_ADD_HEAD(h,l)  do { (l)->blink = (h); (l)->flink = (h)->flink;   \
                                  (l)->flink->blink = (h)->flink = (l);       \
                             } while(0)
#define XF_Q_DEL( l )       do { (l)->flink->blink = (l)->blink;              \
                                  (l)->blink->flink = (l)->flink;             \
                                  (l)->blink = (l)->flink = NULL;             \
                             } while(0)
#define XF_Q_DEL_HEAD(h,l)  do { if ( ((l) = (h)->flink) == (h))              \
                                    { (l) = NULL; break; }                    \
                                 if ( l ) { /* to keep prefast quiet */       \
                                  (h)->flink = (l)->flink;                    \
                                  (l)->flink->blink = (l)->blink;             \
                                  (l)->blink = (l)->flink = NULL;             \
                                 }                                            \
                             } while(0)
#define XF_Q_DEL_TAIL(h,l)  do { if (((l) = (h)->blink) == (h))               \
                                    { (l) = NULL; break; }                    \
                                 if ( l ) { /* to keep prefast quiet */       \
                                  (h)->blink = (l)->blink;                    \
                                  (l)->blink->flink = (l)->flink;             \
                                  (l)->flink = (l)->blink = NULL;             \
                                 }                                            \
                             } while(0)
#define XF_Q_JOIN(h1, h2)   do { if (XF_Q_IS_NULL(h2))                        \
                                   break;                                     \
                                 (h2)->flink->blink = (h1)->blink;            \
                                 (h1)->blink->flink = (h2)->flink;            \
                                 (h1)->blink = (h2)->blink;                   \
                                 (h1)->blink->flink = (h1);                   \
                                 XF_Q_INIT(h2);                               \
                            } while(0)
/*
#define XF_Q_JOIN(h1, h2)   do { (h1)->blink->flink = (h2)->flink;            \
                                  (h2)->flink->blink = (h1)->blink;           \
                                  (h1)->blink = (h2)->blink;                  \
                                  (h2)->blink->flink = h1;                    \
                                  XF_Q_INIT(h2);                              \
                             } while(0)
*/
/* Latching */
#define XF_INIT_LATCH(_platch)                                                \
  {NdisInitializeReadWriteLock((_platch));}

#define XF_GET_SHARED_LATCH(_platch)                                          \
  { LOCK_STATE _state;                                                        \
    NdisAcquireReadWriteLock((_platch), FALSE, &_state);

#define XF_FREE_SHARED_LATCH(_platch)                                         \
    NdisReleaseReadWriteLock((_platch), &_state);                             \
  }

#define XF_GET_XCLUSIVE_LATCH(_platch)                                        \
  { LOCK_STATE _state;                                                        \
    NdisAcquireReadWriteLock((_platch), TRUE, &_state);

#define XF_FREE_XCLUSIVE_LATCH(_platch)                                       \
    NdisReleaseReadWriteLock((_platch), &_state);                             \
  }

/* Memory management */
#define XF_MALLOC(_hMPAdapter, _size)                                         \
  NdisAllocateMemoryWithTagPriority(_hMPAdapter,                              \
                              _size,                                    \
                        XENAMP_MTAG,                              \
                        NormalPoolPriority)

#define XF_MCLEAR(_ptr, _size)      NdisZeroMemory(_ptr, _size);


#define XGE_TEMP_BUFFER_SIZE  512
#define XF_STRSAFE_MAX_CCH   2147483647 /*same as INT_MAX */                


int
xf_strlen(const char *s);

int 
xf_snprintf(__out_bcount(n) char *buf, int n, const char *fmt, ...);

void 
xf_vaprintf(const char *fmt, ...);

#define XF_STRLEN                   xf_strlen
#define XF_VAPRINTF                 xf_vaprintf
#define XF_SNPRINTF                 xf_snprintf

#ifdef XMP_SLOCK_AUDIT
typedef struct _xmpSpinLock_t
{
#define XF_SLOCK_MAGIC   0xfea5a5fe
  ULONG                  magic;
  ULONG                  getLine;
  ULONG                  freeLine;
  BOOLEAN                bDpr;
  DECLSPEC_ALIGN(8)
  NDIS_SPIN_LOCK         lock;
} xmpSpinLock_t;

#define XF_INIT_SLOCK(_pslock)                                                \
  do {                                                                        \
    (_pslock)->magic    = XF_SLOCK_MAGIC;                                     \
    (_pslock)->getLine  = 0;                                                  \
    (_pslock)->freeLine = 0;                                                  \
    (_pslock)->bDpr     = 0;                                                  \
     NdisAllocateSpinLock(&((_pslock)->lock));                                \
  } while(0)

#define XF_GET_SLOCK(_pslock)                                                 \
  do {                                                                        \
    ASSERT((_pslock)->magic == XF_SLOCK_MAGIC);                               \
    NdisAcquireSpinLock(&((_pslock)->lock));                                  \
    (_pslock)->getLine = __LINE__;                                            \
    (_pslock)->bDpr = FALSE;                                                  \
   } while(0)

#define XF_FREE_SLOCK(_pslock)                                                \
  do {                                                                        \
    ASSERT( NDIS_CURRENT_IRQL() == DISPATCH_LEVEL );                          \
    ASSERT((_pslock)->magic == XF_SLOCK_MAGIC);                               \
    NdisReleaseSpinLock(&((_pslock)->lock));                                  \
    (_pslock)->freeLine = __LINE__;                                           \
   } while(0)

#define XF_DPR_GET_SLOCK(_pslock)                                             \
  do {                                                                        \
    ASSERT( NDIS_CURRENT_IRQL() == DISPATCH_LEVEL );                          \
    ASSERT((_pslock)->magic == XF_SLOCK_MAGIC);                               \
    NdisDprAcquireSpinLock(&((_pslock)->lock));                               \
    (_pslock)->getLine = __LINE__;                                            \
    (_pslock)->bDpr    = TRUE;                                                \
   } while(0)

#define XF_DPR_FREE_SLOCK(_pslock)                                            \
  do {                                                                        \
    ASSERT( NDIS_CURRENT_IRQL() == DISPATCH_LEVEL );                          \
    ASSERT((_pslock)->magic == XF_SLOCK_MAGIC);                               \
    ASSERT((_pslock)->bDpr == TRUE);                                          \
    (_pslock)->bDpr = FALSE;                                                  \
    (_pslock)->freeLine = __LINE__;                                           \
    NdisDprReleaseSpinLock(&((_pslock)->lock));                               \
   } while(0)

#else                                                    /* !XMP_SLOCK_AUDIT */

# define xmpSpinLock_t NDIS_SPIN_LOCK
# define XF_INIT_SLOCK(_pslock)    do{NdisAllocateSpinLock(_pslock);} while(0)
# define XF_GET_SLOCK(_pslock)     do{NdisAcquireSpinLock(_pslock);} while(0)
# define XF_FREE_SLOCK(_pslock)    do{NdisReleaseSpinLock(_pslock);} while(0)
# define XF_DPR_GET_SLOCK(_pslock) do{NdisDprAcquireSpinLock(_pslock);} while(0)
# define XF_DPR_FREE_SLOCK(_pslock) do{NdisDprReleaseSpinLock(_pslock);} while(0)

#endif                                                    /* XMP_SLOCK_AUDIT */

#define XF_GET_SLOCK_LEVEL(_pslock, _isDpc)                                   \
  do {                                                                        \
    if (_isDpc)                                                               \
      XF_DPR_GET_SLOCK(_pslock);                                              \
    else                                                                      \
      XF_GET_SLOCK(_pslock);                                                  \
  } while(0)

#define XF_FREE_SLOCK_LEVEL(_pslock, _isDpc)                                  \
  do {                                                                        \
    if (_isDpc)                                                               \
      XF_DPR_FREE_SLOCK(_pslock);                                             \
    else                                                                      \
      XF_FREE_SLOCK(_pslock);                                                 \
  } while(0)

typedef struct _xmpQLink_t
{
  struct _xmpQLink_t *flink;
  struct _xmpQLink_t *blink;
} xmpQLink_t;

VOID
xenaMpSleep(
  IN  UINT              usec
  );

#endif /* XF_H */

