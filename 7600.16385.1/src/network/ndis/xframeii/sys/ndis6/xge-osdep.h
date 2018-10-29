#include <ndis.h>
#include "xf.h"
#include "xmpgbl.h"
#include "xmpdbg.h"

#define inline __inline
/* ------------------------- includes and defines ------------------------- */

#undef XGE_OS_LLXFMT
#define XGE_OS_LLXFMT "%I64x"

#if BITS_PER_LONG == 64
#define XGE_OS_PLATFORM_64BIT                     1
#elif BITS_PER_LONG == 32
#define XGE_OS_PLATFORM_32BIT                     1
#endif

#ifdef XGE_DEBUG_ASSERT
# undef XGE_DEBUG_ASSERT
#endif


#ifndef XGE_HAL_PROCESS_LINK_INT_IN_ISR
# define XGE_HAL_PROCESS_LINK_INT_IN_ISR
#endif

#define XGE_OS_HOST_PAGE_SIZE	4096

/* ---------------------- fixed size primitive types -----------------------*/

#define u8			unsigned char
#define u16			unsigned short
#define u32		        unsigned long
#define u64			unsigned __int64
#define ulong_t			ULONG_PTR
#define dma_addr_t		ULONGLONG
#define spinlock_t		NDIS_SPIN_LOCK
#define ptrdiff_t               UINT_PTR

#define U8			u8
#define U16			u16
#define U32			u32
#define U64			u64

typedef NDIS_HANDLE pci_reg_h;
typedef NDIS_HANDLE pci_irq_h;
typedef NDIS_HANDLE pci_dev_h;
typedef NDIS_HANDLE pci_cfg_h;
typedef NDIS_PHYSICAL_ADDRESS  pci_dma_h;
typedef NDIS_PHYSICAL_ADDRESS  pci_dma_acc_h;

#define OS_NETSTACK_BUF  struct _xmpRxNBLCtx_t *

/* -------------------------- "libc" functionality -------------------------*/
#define xge_os_strlen        XF_STRLEN
#define xge_os_snprintf      XF_SNPRINTF
#define xge_os_timestamp     XF_QUERY_SYSTEM_TIME
#define xge_os_printf        XF_VAPRINTF


#define xge_os_println(buf) DbgPrint("%s\n", buf)  
#define xge_os_strlcpy(src1, src2, len)
#define xge_os_vaprintf(fmt, ...)
#define xge_os_memzero(addr, size)  XF_MCLEAR(addr, size);
#define xge_os_memcpy		  NdisMoveMemory
#define xge_os_memcmp(src1, src2, size)  RtlCompareMemory((CONST VOID *)src1,   \
                                       (CONST VOID *)src2, size)

#define xge_os_ntohs(s) RtlUshortByteSwap(s)
#define xge_os_htons(s) RtlUshortByteSwap(s)
#define xge_os_ntohl(s) RtlUlongByteSwap(s)
#define xge_os_htonl(s) RtlUlongByteSwap(s)
#define XGE_LL_IP_FAST_CSUM(hdr, len)             0
/* -------------------- synchronization primitives -------------------------*/

#define xge_os_spin_lock_init(lockp, ctx)	  NdisAllocateSpinLock(lockp)
#define xge_os_spin_lock_init_irq(lockp, irqh)    NdisAllocateSpinLock(lockp)
#define xge_os_spin_lock_destroy(lockp, ctx)      NdisFreeSpinLock(lockp)
#define xge_os_spin_lock(lockp)	                  NdisAcquireSpinLock(lockp)
#define xge_os_spin_unlock(lockp)		  NdisReleaseSpinLock(lockp)
#define xge_os_spin_lock_init_irq(lockp, ctx)     NdisAllocateSpinLock(lockp)
#define xge_os_spin_lock_destroy_irq(lockp, ctx)  NdisFreeSpinLock(lockp)
#define xge_os_spin_lock_irq(lockp, flags)	  NdisAcquireSpinLock(lockp)
#define xge_os_spin_unlock_irq(lockp, flags)      NdisReleaseSpinLock(lockp)
#define xge_os_wmb()                              KeMemoryBarrier()
#define xge_os_mdelay(millisec)                   xenaMpSleep(millisec * 1000)
#define xge_os_udelay(usec)                       xenaMpSleep(usec)
#define xge_os_cmpxchg(targetp, cmp, newval)      (0)

/* ------------------------- misc primitives -------------------------------*/
#define xge_os_prefetch
#define xge_os_prefetchw
#define xge_os_bug(fmt, void)                                                  \
   { DbgPrint("BugCheck :\n"); XMPTRACE(XMP_DBG_HAL, fmt); }
#define xge_os_unlikely(x)      (x)
/* -------------------------- compiler stuffs ------------------------------*/

#define __xge_os_attr_cacheline_aligned
#define __xge_os_cacheline_size			64 /* Opteron L1/L2 sizes */

/* ---------------------- memory primitives --------------------------------*/

static inline void *xge_os_malloc (IN  pci_dev_h pdev,
				   IN  unsigned long size)
{
  return ((void *)XF_MALLOC(pdev, size));
}

/* --------------------------- pci primitives ------------------------------*/

static __inline int xge_os_pci_read8(IN  pci_dev_h pdev,
				  IN  pci_cfg_h cfgh,	/* unused */
				  IN  int where,
				  IN  u8 *val)
{
   ULONG ulTmp;
  ulTmp = NdisMGetBusData(pdev,
                               PCI_WHICHSPACE_CONFIG,
                               (ULONG)where,
                                val, sizeof(UCHAR));
   
   if(ulTmp!= sizeof(UCHAR)) 
   {
      XMPTRACE(XMP_DBG_ERR, ("NdisMGetBusData (SERR) failed ulTmp=%d\n",
                             ulTmp));
      return 1;
    }
    return 0;
}

static __inline int xge_os_pci_write8(IN  pci_dev_h pdev,
				    IN  pci_cfg_h cfgh,	/* unused */
				    IN  int where,
				    IN  u8 val)
{
    ULONG ulTmp;
    if( (ulTmp = NdisMSetBusData(pdev,
                                 PCI_WHICHSPACE_CONFIG,
                                 (ULONG)where,
                                 &val, sizeof(UCHAR)))
         != sizeof(UCHAR) )
    {
      XMPTRACE(XMP_DBG_ERR, ("NdisMSetBusData (Command) failed ulTmp=%d\n",
                             ulTmp));
      return 1;
    }
    return 0;
}

static __inline int xge_os_pci_read16(IN  pci_dev_h pdev,
				  IN  pci_cfg_h cfgh,	/* unused */
				  IN  int where,
				  IN  u16 *val)
{
   ULONG ulTmp;
     ulTmp = NdisMGetBusData(pdev,
                               PCI_WHICHSPACE_CONFIG,
                               (ULONG) where,
                               val, sizeof(USHORT));
     if(  ulTmp != sizeof(USHORT) )
    {
      XMPTRACE(XMP_DBG_ERR, ("NdisMGetBusData (SERR) failed ulTmp=%d\n",
                             ulTmp));
      return 1;
    }
    return 0;
}

static __inline int xge_os_pci_write16(IN  pci_dev_h pdev,
				   IN  pci_cfg_h cfgh,	/* unused */
				   IN  int where,
				   IN  u16 val)
{
  ULONG ulTmp;
  if( (ulTmp = NdisMSetBusData(pdev,
                                PCI_WHICHSPACE_CONFIG,
                                (ULONG)where,
                                &val, sizeof(USHORT)))
        != sizeof(USHORT) )
   {
     XMPTRACE(XMP_DBG_ERR, ("NdisMSetBusData (Command) failed ulTmp=%d\n",
                            ulTmp));
     return 1;
   }
   return 0;
}

static __inline int xge_os_pci_read32(IN  pci_dev_h pdev,
				  IN  pci_cfg_h cfgh,	/* unused */
				  IN  int where,
				  IN  u32 *val)
{
  ULONG ulTmp;
  ulTmp = NdisMGetBusData(pdev,
                              PCI_WHICHSPACE_CONFIG,
                              (ULONG)where,
                              (PVOID)val, sizeof(ULONG));

   if(  ulTmp != sizeof(ULONG) )
   {
     XMPTRACE(XMP_DBG_ERR, ("NdisMGetBusData (SERR) failed ulTmp=%d\n",
                            ulTmp));
     return 1;
   }
   return 0;
}

static __inline int xge_os_pci_write32(IN  pci_dev_h pdev,
				   IN  pci_cfg_h cfgh,	/* unused */
				   IN  int where,
				   IN  u32 val)
{
  ULONG ulTmp;
  if( (ulTmp = NdisMSetBusData(pdev,
                                PCI_WHICHSPACE_CONFIG,
                                where,
                                &val, sizeof(ULONG)))
        != sizeof(ULONG) )
  {
    XMPTRACE(XMP_DBG_ERR, ("NdisMSetBusData (Command) failed ulTmp=%d\n",
                            ulTmp));
    return 1;
  }
  return 0;
}


/* --------------------------- io primitives -------------------------------*/

#if defined(_M_IX86) || defined(_M_AMD64)
VOID _mm_lfence ( VOID );
# ifndef StoreFence
#  define StoreFence() _mm_sfence()
# endif
#else
# define StoreFence()
#endif

static __inline u8 xge_os_pio_mem_read8(IN  pci_dev_h pdev,	/* unused */
				    IN  pci_reg_h regh,	/* unused */
				    IN  void *addr)
{
  UCHAR val;
  NdisReadRegisterUchar(addr, &val);
  return val;
}

static __inline void xge_os_pio_mem_write8(IN  pci_dev_h pdev,	/* unused */
				       IN  pci_reg_h regh,	/* unused */
				       IN  u8 val,
                                       IN  void *addr)
{
  NdisWriteRegisterUchar(addr, val);
}

static __inline u16 xge_os_pio_mem_read16(IN  pci_dev_h pdev,	/* unused */
				      IN  pci_reg_h regh,	/* unused */
				      IN  void *addr)
{
  USHORT val;
  NdisReadRegisterUshort(addr, &val);
  return val;
}

static __inline void xge_os_pio_mem_write16(IN  pci_dev_h pdev,	/* unused */
					IN  pci_reg_h regh,	/* unused */
					IN  u16 val,
                                        IN  void *addr)
{
  NdisWriteRegisterUshort(addr, val);
}

static __inline u32 xge_os_pio_mem_read32(IN  pci_dev_h pdev,	/* unused */
				      IN  pci_reg_h regh,	/* unused */
				      IN  void *addr)
{
  ULONG val;
  NdisReadRegisterUlong(addr, &val);
  return val;
}

static __inline void xge_os_pio_mem_write32(IN  pci_dev_h pdev,	/* unused */
					IN  pci_reg_h regh,	/* unused */
					IN  u32 val,
                                        IN  void *addr)
{
#if defined(_M_IA64)
    *((volatile U32 * const) (addr)) = val;
#else
    NdisWriteRegisterUlong(addr, val);
#endif
}

static __inline u64 xge_os_pio_mem_read64(IN  pci_dev_h pdev,	/* unused */
				      IN  pci_reg_h regh,	/* unused */
				      IN  void *addr)
{
  u64 _val;
  _val = *((volatile U64 * const) (addr));
  return _val;
}

static __inline void xge_os_pio_mem_write64(IN  pci_dev_h pdev,	/* unused */
					  IN  pci_reg_h regh,	/* unused */
					  IN  u64 val,
                                          IN  void *addr)
{
  ULONGLONG _val;
  *((volatile U64 * const) (addr)) = val;

}

static inline void xge_os_flush_bridge(pci_dev_h pdev, pci_reg_h regh,
				       void *addr)
{
  volatile U64 * const addr1 = (volatile U64 * const)addr;
#if defined(_M_IA64)
  *addr1;
#elif defined(_M_AMD64)
  KeMemoryBarrier();
  *addr1;
#else /* x86 */
  *addr1;
#endif
}


/* --------------------------- dma primitives ----------------------------- */

#define XGE_OS_DMA_DIR_TODEVICE		0
#define XGE_OS_DMA_DIR_FROMDEVICE	0
#define XGE_OS_DMA_DIR_BIDIRECTIONAL	0
#define XGE_OS_INVALID_DMA_ADDR         ((dma_addr_t)0)

static __inline void *xge_os_dma_malloc(IN  pci_dev_h pdev,	      /* unused */
				      IN  unsigned long size,
				      IN  int dma_flags,
                                      OUT pci_dma_h *p_dma_h,
				      OUT pci_dma_acc_h *p_dma_acch)  /* unused */
{
  void *vaddr;
  NdisMAllocateSharedMemory(pdev,
                            size,
                            FALSE,                                   /*not cached*/
                            &vaddr,
                            p_dma_h);
  if(vaddr != NULL )
  {
    *p_dma_acch = *p_dma_h; /* for dma_map(), dma_free() */
  }
  return vaddr;
}

static __inline void xge_os_dma_free (IN  pci_dev_h pdev,	      /* unused */
				    IN  const void *vaddr,
				    IN  unsigned long size,
                                    IN  pci_dma_acc_h *p_dma_acch,
                                    IN  pci_dma_h *p_dma_h)
{

    NdisMFreeSharedMemory(pdev,
			  size,
			  FALSE,                                   /*not cached*/
			  (PVOID)vaddr,
			  *p_dma_acch);
 
}

static __inline dma_addr_t xge_os_dma_map(IN  pci_dev_h pdev,
				      IN  pci_dma_h dmah,	/* unused */
				      IN  void *vaddr,
				      IN  size_t size,
				      IN  int dir,
				      IN  int dma_flags)
{
  return(dmah.QuadPart);
}

static __inline void xge_os_dma_unmap(IN  pci_dev_h pdev,
				  IN  pci_dma_h dmah,	/* unused */
				  IN  dma_addr_t dma_addr,
				  IN  size_t size,
				  IN  int dir)
{
}

static __inline void xge_os_dma_sync(IN  pci_dev_h pdev,
				 IN  pci_dma_h dmah,	  /* unused */
				 IN  dma_addr_t dma_addr,
				 IN  u64 dma_offset,	  /* unused */
				 IN  size_t length,
				 IN  int dir)
{
  return;
}

