#ifndef XMPHALMAP_H
#define XMPHALMAP_H
#define HAL_TX_MAX_FIFOS                XGE_HAL_MAX_FIFO_NUM 
#define HAL_RX_MAX_RINGS                XGE_HAL_MAX_RING_NUM

#define HAL_TX_REQ_TIMEOUT_DEFAULT      0x0
#define HAL_TX_REQ_TIMEOUT_MIN          0x0
#define HAL_TX_REQ_TIMEOUT_MAX          0x0
#define HAL_MAX_PYLD                    1500
#define HAL_MAX_MTU                     (HAL_MAX_PYLD+14)
#define HAL_MAX_MTU_VLAN                (HAL_MAX_PYLD+18)
#define HAL_MAX_PYLD_JUMBO              9600
#define HAL_MAX_MTU_JUMBO               (HAL_MAX_PYLD_JUMBO+14)
#define HAL_MAX_MTU_JUMBO_VLAN          (HAL_MAX_PYLD_JUMBO+18)
#define HAL_RXD_COUNT_PER_BLOCK         XGE_HAL_RING_RXDS_PER_BLOCK(1) /*TODO*/
#define HAL_STATUS                      xge_hal_status_e          
#define XENA_HAL_PCIX_CMD_OFFSET        0x62
#define HAL_MAX_FIFO_THRESHOLD          XGE_HAL_MAX_FIFO_RESERVE_THRESHOLD
#define XENA_HAL_SET_SPLIT_DMA_CMD(_maxSplitTxn, _cmd)                        \
  (_cmd) = (_cmd & 0xff8f) | (0x0080 | ((U16) (_maxSplitTxn) << 4))
#define HAL_TX_CKO_IPV6_EN              0x0008

#endif

