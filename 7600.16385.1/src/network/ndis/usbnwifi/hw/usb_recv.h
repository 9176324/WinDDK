#ifndef __INC_RECEIVE_H
#define __INC_RECEIVE_H

#define MAX_RECEIVE_BUFFER_SIZE 2500

NDIS_STATUS
HwUsbRecvStop(
    PNIC Nic
    );
       
NDIS_STATUS
HwUsbRecvStart(
    PNIC Nic
    );

NDIS_STATUS
HwUsbAllocateRecvResources(
    __in PNIC Nic
    );

NDIS_STATUS
HwUsbFreeRecvResources(
    __in PNIC Nic
    );

VOID
HwUsbFreeRecvFragment(
                __in  PNIC Nic,
                __in  PNIC_RX_FRAGMENT NicFragment
                ) ;

VOID
HwUsbProcessReceivedPacket(
    PNIC    Nic,
    PNIC_RX_FRAGMENT     Rfd
    );


#endif // #ifndef __INC_RECEIVE_H

