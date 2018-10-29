#ifndef XMPDBG_H
#define XMPDBG_H

# define XMP_DBG_NOP            0x00000000
#if DBG
# define XMP_DBG_ERR            0x00000001                   /* Fatal Errors */
# define XMP_DBG_WRN            0x00000002                       /* Warnings */
# define XMP_DBG_RESERVED           0x00000004               /* Reserved for future use */
# define XMP_DBG_EVT            0x00000008                   /* Async events */
# define XMP_DBG_PNP            0x00000008     /* PNP events; same as events */
# define XMP_DBG_INFO           0x00000010                  /* Informational */
# define XMP_DBG_INI            0x00000100                 /* Initialization */
# define XMP_DBG_TRM            0x00000200                    /* Termination */
# define XMP_DBG_REQ            0x00000400                  /* NDIS Requests */
# define XMP_DBG_OFLD           0x00000800                       /* Offloads */
# define XMP_DBG_LACP           0x00001000                           /* LACP */
# define XMP_DBG_LACPALL        0x00002000           /* LACP extensive debug */
# define XMP_DBG_TX             0x00004000                        /* TX path */
# define XMP_DBG_RX             0x00008000                        /* RX Path */
# define XMP_DBG_TX_DUMP        0x00010000    /* Excessive TX - Packet dumps */
# define XMP_DBG_RX_DUMP        0x00020000    /* Excessive RX - Packet dumps */
# define XMP_DBG_INT            0x00040000                            /* ISR */
# define XMP_DBG_HAL            0x00080000                            /* HAL */
# define XMP_DBG_TIMER          0x00100000                           /* Timer*/
# define XMP_DBG_DBG            0x10000000                           /* Misc */

# define XMP_DBGALL (XMP_DBG_ERR   |                                          \
        XMP_DBG_WRN    |                                          \
        XMP_DBG_RESERVED   |                                          \
        XMP_DBG_EVT    |                                          \
        XMP_DBG_PNP    |                                          \
        XMP_DBG_INFO   |                                          \
        XMP_DBG_INI    |                                          \
        XMP_DBG_TRM    |                                          \
        XMP_DBG_REQ    |                                          \
        XMP_DBG_OFLD   |                                          \
        XMP_DBG_LACP   |                                          \
        XMP_DBG_LACPALL|                                          \
        XMP_DBG_TX     |                                          \
        XMP_DBG_RX     |                                          \
        XMP_DBG_TX_DUMP|                                          \
        XMP_DBG_RX_DUMP|                                          \
        XMP_DBG_INT    |                                          \
        XMP_DBG_HAL    |                                          \
        XMP_DBG_DBG    |                                          \
        XMP_DBG_NOP)

# define XMP_DBGDEF (XMP_DBG_ERR   |                                          \
        XMP_DBG_WRN    |                                          \
        XMP_DBG_RESERVED   |                                          \
        XMP_DBG_EVT    |                                          \
        XMP_DBG_PNP    |                                          \
        XMP_DBG_INFO   |                                          \
        XMP_DBG_INI    |                                          \
        XMP_DBG_TRM    |                                          \
        XMP_DBG_OFLD   |                                          \
        XMP_DBG_LACP   |                                          \
        XMP_DBG_LACPALL|                                          \
        XMP_DBG_HAL    |                                          \
        XMP_DBG_NOP)

# define XMPTRACE(lev, Fmt)         \
  {                               \
    if (bit(xmpDebugLevel, lev))  \
    {                             \
      DbgPrint("XENA-MP: ");      \
      DbgPrint Fmt;               \
    }                             \
  }
#else /* DBG */
# define XMPTRACE(lev, Fmt)
# define XMP_DBGDEF  XMP_DBG_NOP
# define XMP_DBGALL  XMP_DBG_NOP
#endif /* DBG */

#define DBGPRINT(fmt) //XMPTRACE(XMP_DBG_HAL, fmt)

#endif

