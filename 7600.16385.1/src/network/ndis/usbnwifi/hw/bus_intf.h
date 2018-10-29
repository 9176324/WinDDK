
NDIS_STATUS
HwInitializeBus(
    __in PNIC Nic
    )  ;

VOID
HwDeInitBus(
    __in PNIC Nic
    );

NDIS_STATUS
HwReadAdapterInfo(
    __in PNIC Nic
    ) ;

NDIS_STATUS
HwInitializeAdapter(
    __in PNIC Nic
    );

NDIS_STATUS
ReadNicInfo8187(
    PNIC         Nic
    );

VOID
InitializeVariables8187(
    PNIC         Nic
    );
 
NDIS_STATUS
HwBusStartDevice(
                __in PNIC Nic
                 );


NDIS_STATUS
HwBusFreeRecvResources(
    __in PNIC Nic
    );


NDIS_STATUS
HwBusAllocateRecvResources(
    __in PNIC Nic
    );

NDIS_STATUS
HwBusAllocateXmitResources(
    __in PNIC Nic ,
    __in  ULONG NumTxd
    )  ;

VOID
HwBusFreeXmitResources(
    __in PNIC Nic
    ) ;

NDIS_STATUS
HwBusFindAdapterAndResources(
    __in PNIC Nic
    ) ;


NDIS_STATUS
HwBusSendPacketAsync(
    __in PNIC             Nic,
    __in UCHAR            QueueType,
    __in ULONG            BufferLength,
    __in PVOID            Buffer,
    __in_opt PNIC_TX_MSDU     NicTxd 
    ) ;

VOID
HwBusStopNotification(
    __in PNIC Nic
    );

VOID
HwBusFreeRecvFragment(
                __in  PNIC Nic,
                __in  PNIC_RX_FRAGMENT NicFragment
                ) ;


