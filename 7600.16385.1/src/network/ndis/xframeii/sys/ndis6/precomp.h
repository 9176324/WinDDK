#ifndef PRECOMP_H
#define PRECOMP_H

#define XENAMP_MAJOR_DRIVER_VERSION     0x02
#define XENAMP_MINOR_DRIVER_VERSION     0x06

#include <ndis.h>
#include "xf.h"
#include "../../include/xgehal.h"
#include "xmphalmap.h"
#include "xenamp.h"
#include "xmpnic.h"
#include "xmpreq.h"
#include "xmpdbg.h"
#include "xmphal.h"
#include "xmpdiag.h"

#define WPP_CONTROL_GUIDS \
WPP_DEFINE_CONTROL_GUID(XenaEth,(7823ECB1,B0C6,4d51,A141,1B70F0908DF4),\
  WPP_DEFINE_BIT(XMP_DBG_ERR )                                          \
  WPP_DEFINE_BIT(XMP_DBG_WRN )                                          \
  WPP_DEFINE_BIT(XMP_DBG_RESERVED)                                          \
  WPP_DEFINE_BIT(XMP_DBG_EVT )                                          \
  WPP_DEFINE_BIT(XMP_DBG_PNP )                                          \
  WPP_DEFINE_BIT(XMP_DBG_INFO)                                          \
  WPP_DEFINE_BIT(XMP_DBG_INI )                                          \
  WPP_DEFINE_BIT(XMP_DBG_TRM )                                          \
  WPP_DEFINE_BIT(XMP_DBG_REQ )                                          \
  WPP_DEFINE_BIT(XMP_DBG_OFLD )                                         \
  WPP_DEFINE_BIT(XMP_DBG_LACP )                                         \
  WPP_DEFINE_BIT(XMP_DBG_LACPALL )                                      \
  WPP_DEFINE_BIT(XMP_DBG_TX )                                           \
  WPP_DEFINE_BIT(XMP_DBG_RX )                                           \
  WPP_DEFINE_BIT(XMP_DBG_TX_DUMP )                                      \
  WPP_DEFINE_BIT(XMP_DBG_RX_DUMP )                                      \
  WPP_DEFINE_BIT(XMP_DBG_INT )                                          \
  WPP_DEFINE_BIT(XMP_DBG_HAL )                                          \
  WPP_DEFINE_BIT(XMP_DBG_DBG )                                          \
  WPP_DEFINE_BIT(XMP_DBG_NOP ))

#endif

