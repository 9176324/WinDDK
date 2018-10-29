#ifndef __USB_TRANSMIT_H
#define __USB_TRANSMIT_H



typedef struct _USB_WRITE_REQ_CONTEXT {
    WDFMEMORY         UrbMemory;
    PURB              Urb;
    PMDL              Mdl;
    USBD_PIPE_HANDLE  UsbdPipeHandle ;
    WDFUSBPIPE        UsbPipe;
    PADAPTER          NdisContext;
    UCHAR             QueueType;  
    PNIC              Nic;
    PNIC_TX_MSDU      NicTxd ;
    WDFIOTARGET       IoTarget;
} USB_WRITE_REQ_CONTEXT, *PUSB_WRITE_REQ_CONTEXT  ;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(USB_WRITE_REQ_CONTEXT, GetWriteRequestContext)


NTSTATUS
HwUsbSendPacketAsync (
    PNIC            Nic,
    UCHAR           QueueType,
    ULONG           BufferLength,   // Number of bytes in pData buffer
    PVOID           Buffer,
    PNIC_TX_MSDU    NicTxd
    );

BOOL
HwUsbAllocateWriteRequests(
    PNIC    Nic,    
    BYTE    MaxOutstanding      
    );

VOID
HwUsbFreeXmitMemory(
    PNIC            Nic
    );


NDIS_STATUS
HwUsbAllocateXmitResources(
    __in PNIC            Nic,
    __in  ULONG NumTxd
    );


VOID
HwUsbFreeWriteRequests(
    PNIC    Nic
    );


NDIS_STATUS
HwUsbXmitStart( 
    PNIC    Nic
    );

#endif // #ifndef __USB_TRANSMIT_H

