/*
 *  FileName :    xgehal-mgmtaux.h
 *
 *  Description:  management auxiliary API
 */

#ifndef XGE_HAL_MGMTAUX_H
#define XGE_HAL_MGMTAUX_H

#include "xgehal-mgmt.h"

__EXTERN_BEGIN_DECLS

#define XGE_HAL_AUX_SEPA        ' '

xge_hal_status_e xge_hal_aux_about_read(xge_hal_device_h devh,
	    int bufsize, __out_bcount(bufsize) char *retbuf, int *retsize);

xge_hal_status_e xge_hal_aux_stats_tmac_read(xge_hal_device_h devh,
	    int bufsize, __out_bcount(bufsize) char *retbuf, int *retsize);

xge_hal_status_e xge_hal_aux_stats_rmac_read(xge_hal_device_h devh,
	    int bufsize, __out_bcount(bufsize) char *retbuf, int *retsize);

xge_hal_status_e xge_hal_aux_stats_sw_dev_read(xge_hal_device_h devh,
	    int bufsize, __out_bcount(bufsize) char *retbuf, int *retsize);

xge_hal_status_e xge_hal_aux_stats_pci_read(xge_hal_device_h devh,
	    int bufsize, __out_bcount(bufsize) char *retbuf, int *retsize);

xge_hal_status_e xge_hal_aux_stats_hal_read(xge_hal_device_h devh,
	    int bufsize, __out_bcount(bufsize) char *retbuf, int *retsize);

xge_hal_status_e xge_hal_aux_bar0_read(xge_hal_device_h devh,
	    unsigned int offset, int bufsize, __out_bcount(bufsize) char *retbuf, int *retsize);

xge_hal_status_e xge_hal_aux_bar0_write(xge_hal_device_h devh,
	    unsigned int offset, u64 value);

xge_hal_status_e xge_hal_aux_bar1_read(xge_hal_device_h devh,
	    unsigned int offset, int bufsize, __out_bcount(bufsize) char *retbuf, int *retsize);

xge_hal_status_e xge_hal_aux_pci_config_read(xge_hal_device_h devh,
	    int bufsize, __out_bcount(bufsize) char *retbuf, int *retsize);

xge_hal_status_e xge_hal_aux_stats_herc_enchanced(xge_hal_device_h devh,
	    int bufsize, __out_bcount(bufsize) char *retbuf, int *retsize);

xge_hal_status_e xge_hal_aux_channel_read(xge_hal_device_h devh,
	    int bufsize, __out_bcount(bufsize) char *retbuf, int *retsize);

xge_hal_status_e xge_hal_aux_device_dump(xge_hal_device_h devh);

xge_hal_status_e xge_hal_aux_driver_config_read(
	    int bufsize, __out_bcount(bufsize) char *retbuf, int *retsize);

xge_hal_status_e xge_hal_aux_device_config_read(xge_hal_device_h devh,
	    int bufsize, __out_bcount(bufsize) char *retbuf, int *retsize);

__EXTERN_END_DECLS

#endif /* XGE_HAL_MGMTAUX_H */

