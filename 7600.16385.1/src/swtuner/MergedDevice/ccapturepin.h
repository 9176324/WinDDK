//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;

//
// STREAM_POINTER_CONTEXT:
//
// This is the context structure we associate with all clone stream pointers.
// It allows the mapping code to rip apart the buffer into chunks the same
// size as the scatter/gather mappings in order to fake scatter / gather
// bus-master DMA.
//
typedef struct _STREAM_POINTER_CONTEXT {
    
    PUCHAR BufferVirtual;

} STREAM_POINTER_CONTEXT, *PSTREAM_POINTER_CONTEXT;

//
// CCapturePin:
//
// The video capture pin class.
//
class CCapturePin :
    public ICaptureSink {

private:

    //
    // The AVStream pin we're associated with.
    //
    PKSPIN m_Pin;

    //
    // Pointer to the internal device object for our capture device.
    // We access the "fake" hardware through this object.
    //
    CDevice *m_Device;

    //
    // The state we've put the hardware into.  This allows us to keep track
    // of whether to do things like unpausing or restarting.
    //
    HARDWARE_STATE m_HardwareState;

    //
    // The clock we've been assigned.  As with other capture filters, we do
    // not expose a clock.  If one has been assigned, we will use it to
    // time stamp packets (plus a reasonable delta to work the capture stream
    // in a preview graph).
    //
    PIKSREFERENCECLOCK m_Clock;

    //
    // The transport information for this capture pin.  The settings for "fake" hardware will be
    // programmed for this transport info.
    //
    PBDA_TRANSPORT_INFO m_TransportInfo;

    //
    // If we are unable to insert all of the mappings in a stream pointer into
    // the "fake" hardware's scatter / gather table, we set this to the
    // stream pointer that's incomplete.  This is done both to make the 
    // relasing easier and to make it easier to fake the scatter / gather
    // hardware.
    //
    PKSSTREAM_POINTER m_PreviousStreamPointer;

    //
    // An indication of whether or not we pended I/O for some reason.  If this
    // is set, the DPC will resume I/O when any mappings are completed.
    //
    BOOLEAN m_PendIo;

    //
    // An indication of whether or not this pin has acquired the necessary
    // hardware resources to operate.  When the pin reaches KSSTATE_ACQUIRE,
    // we attempt to acquire the hardware.  This flag will be set based on
    // our success / failure.
    //
    BOOLEAN m_AcquiredResources;

    //
    // CleanupReferences():
    //
    // Clean up any references we hold on frames in the queue.  This is called
    // when we abruptly stop the fake hardware.
    //
    NTSTATUS
    CleanupReferences (
        );

    //
    // SetState():
    //
    // This is the state transition handler for the capture pin.  It attempts
    // to acquire resources for the capture pin (or releasing them if
    // necessary) and starts and stops the hardware as required.
    //
    NTSTATUS
    SetState (
        IN KSSTATE ToState,
        IN KSSTATE FromState
        );

    //
    // Process():
    //
    // This is the processing dispatch for the capture pin.  It handles
    // programming the scatter / gather tables for the hardware as buffers
    // become available.  This processing routine is designed for a direct
    // into the capture buffers kind of DMA as opposed to common-buffer
    // and copy strategies.
    //
    NTSTATUS
    Process (
        );

    //
    // CaptureBdaTransportInfo():
    //
    // This routine stashes the BDA Transport Info for the pin connection
    // in the CCapturePin object.  This is used to base hardware settings.
    //
    PBDA_TRANSPORT_INFO
    CaptureBdaTransportInfo (
        );

    //
    // Cleanup():
    //
    // This is the free callback from the bagged item (CCapturePin).  If we
    // do not provide a callback when we bag the CCapturePin, ExFreePool
    // would be called.  This is not desirable for C++ constructed objects.
    // We merely delete the object here.
    //
    static
    void
    Cleanup (
        IN CCapturePin *Pin
        )
    {
        delete Pin;
    }

public:

    //
    // CCapturePin():
    //
    // The capture pin's constructor.  Initialize any non-0, non-NULL fields
    // (since new will have zero'ed the memory anyway) and set up our
    // device level pointers for access during capture routines.
    //
    CCapturePin (
        IN PKSPIN Pin
        );

    //
    // ~CCapturePin():
    //
    // The capture pin's destructor.
    //
    ~CCapturePin (
        )
    {
    }

    CDevice* GetDevice()    {
        return m_Device;
    }

    //
    // ICaptureSink::CompleteMappings()
    //
    // This is the capture sink notification mechanism for mapping completion.
    // When the device DPC detects that a given number of mappings have been
    // completed by the fake hardware, it signals the capture sink of this
    // through this method.
    //
    virtual
    void
    CompleteMappings (
        IN ULONG NumMappings
        );

    //
    //  Network provider and AVStream use these functions 
    //  to set and get properties of nodes that are controlled 
    //  by the input pin. 
    //
    static
    STDMETHODIMP_(NTSTATUS)
    GetCenterFrequency(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PutCenterFrequency(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );

    /*************************************************

        Dispatch Routines

    *************************************************/

    //
    // DispatchCreate():
    //
    // This is the creation dispatch for the capture pin.  It creates
    // the CCapturePin object and associates it with the AVStream object
    // bagging it in the process.
    //
    static
    NTSTATUS
    DispatchCreate (
        IN PKSPIN Pin,
        IN PIRP Irp
        );

    //
    // DispatchSetState():
    //
    // This is the set device state dispatch for the pin.  The routine bridges
    // to SetState() in the context of the CCapturePin.
    //
    static
    NTSTATUS
    DispatchSetState (
        IN PKSPIN Pin,
        IN KSSTATE ToState,
        IN KSSTATE FromState
        )
    {
        return 
            (reinterpret_cast <CCapturePin *> (Pin -> Context)) ->
                SetState (ToState, FromState);
    }

    //
    // DispatchProcess():
    //
    // This is the processing dispatch for the capture pin.  The routine 
    // bridges to Process() in the context of the CCapturePin.
    //
    static 
    NTSTATUS
    DispatchProcess (
        IN PKSPIN Pin
        )
    {
        return 
            (reinterpret_cast <CCapturePin *> (Pin -> Context)) ->
                Process ();
    }


    //
    //  Define the AVStream Pin Dispatch Functions
    //  Network provider and AVStream use these functions 
    //  to create or remove a pin instance or to change the pin's 
    //  state after the minidriver receives a connection state 
    //  property 'set' IOCTL. 
    //
    static
    STDMETHODIMP_(NTSTATUS)
    PinCreate(
        IN OUT PKSPIN Pin,
        IN PIRP Irp
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PinClose(
        IN OUT PKSPIN Pin,
        IN PIRP Irp
        );

    //
    //  Define a data intersection handler function for the 
    //  pin (KSPIN_DESCRIPTOR_EX structure). 
    //  Network provider and AVStream use this function 
    //  to connect the output pin with a downstream filter.   
    //
    static
    STDMETHODIMP_(NTSTATUS)
    IntersectDataFormat(
        IN PVOID pContext,
        IN PIRP pIrp,
        IN PKSP_PIN Pin,
        IN PKSDATARANGE DataRange,
        IN PKSDATARANGE MatchingDataRange,
        IN ULONG DataBufferSize,
        OUT PVOID Data OPTIONAL,
        OUT PULONG DataSize
        );

    //
    //  BDA Signal Properties
    //
    static
    STDMETHODIMP_(NTSTATUS)
    GetSignalStatus(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PutAutoDemodProperty(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    GetDigitalDemodProperty(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PutDigitalDemodProperty(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PropSetTableSection(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PBDA_TABLE_SECTION       pulProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PropGetTableSection(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        OUT PBDA_TABLE_SECTION       pulProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PropGetECM_MapStatus(
        IN PIRP         pIrp, 
        IN PKSPROPERTY  pKSProperty,
        OUT PULONG      pulProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PropSetECM_MapStatus(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PropGetMapPIDS(
        IN PIRP          pIrp, 
        IN PKSPROPERTY   pKSProperty,
        OUT PBDA_PID_MAP pPidMapProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PropSetMapPIDS(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PBDA_PID_MAP pPidMapProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PropGetUnmapPIDS(
        IN PIRP            pIrp, 
        IN PKSPROPERTY     pKSProperty,
        OUT PBDA_PID_UNMAP pPidUnmapProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PropSetUnmapPIDS(
        IN PIRP           Irp,
        IN PKSPROPERTY    pKSProperty,
        IN PBDA_PID_UNMAP pPidUnmapProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    GetExtensionProperties(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );

    static
    STDMETHODIMP_(NTSTATUS)
    PutExtensionProperties(
        IN PIRP         Irp,
        IN PKSPROPERTY  pKSProperty,
        IN PULONG       pulProperty
        );

    STDMETHODIMP_(class CFilter *)
    GetFilter() { return m_pFilter;};

    STDMETHODIMP_(void)
    SetFilter(class CFilter * pFilter) { m_pFilter = pFilter;};
private:
    class CFilter*  m_pFilter;
    ULONG           ulReserved;
    KSSTATE         m_KsState;

    //  Node Properties
    //
    BOOLEAN         m_fResourceChanged;
    ULONG           m_ulCurrentProperty1;
    ULONG           m_ulPendingProperty1;

    ULONG           m_ulCurrentProperty2;

    ULONG           m_ulCurrentProperty3;
    ULONG           m_ulPendingProperty3;
    
};


