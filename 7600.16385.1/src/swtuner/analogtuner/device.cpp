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

#include "common.h"
#include <string.h>
/**************************************************************************

PAGEABLE CODE

**************************************************************************/
extern const KSFILTER_DESCRIPTOR BdaAnlgTunerFilterDescriptor;
extern const KSFILTER_DESCRIPTOR BdaXBarFilterDescriptor;
extern const KSFILTER_DESCRIPTOR BdaEncoderFilterDescriptor;
extern const KSFILTER_DESCRIPTOR BdaTVAudioFilterDescriptor;
extern const KSFILTER_DESCRIPTOR BdaAnlgCaptureFilterDescriptor;


UNICODE_STRING g_RegistryPath;

#ifdef ALLOC_PRAGMA
#pragma code_seg()
#endif // ALLOC_PRAGMA



NTSTATUS
CEncoderDevice::
DispatchCreate (
                IN PKSDEVICE Device
                )

                /*++

                Routine Description:

                Create the encoder device.  This is the creation dispatch for the
                encoder device.

                Arguments:

                Device -
                The AVStream device being created.

                Return Value:

                Success / Failure

                --*/

{

    PAGED_CODE();

    NTSTATUS Status = STATUS_SUCCESS;

    CEncoderDevice *EncDevice = new (NonPagedPool, MS_SAMPLE_ANALOG_POOL_TAG) CEncoderDevice (Device,Status);

    if (!EncDevice || !NT_SUCCESS(Status)) {
        //
        // Return failure if we couldn't create the pin.
        //
        Status = STATUS_INSUFFICIENT_RESOURCES;

    } else {

        //
        // Add the item to the object bag if we were successful.
        // Whenever the device goes away, the bag is cleaned up and
        // we will be freed.
        //
        // For backwards compatibility with DirectX 8.0, we must grab
        // the device mutex before doing this.  For Windows XP, this is
        // not required, but it is still safe.
        //
        KsAcquireDevice (Device);
        Status = KsAddItemToObjectBag (
            Device -> Bag,
            reinterpret_cast <PVOID> (EncDevice),
            reinterpret_cast <PFNKSFREE> (CEncoderDevice::Cleanup)
            );
        KsReleaseDevice (Device);

        if (!NT_SUCCESS (Status)) {
            delete EncDevice;
        } else {
            Device -> Context = reinterpret_cast <PVOID> (EncDevice);
                    
            EncDevice->m_czDeviceInstanceID[0] = '\0';
            EncDevice->m_bGetDeviceInstanceID = false;
        }


    }

    return Status;

}

/*************************************************/


NTSTATUS
CEncoderDevice::
PnpStart (
          IN PCM_RESOURCE_LIST TranslatedResourceList,
          IN PCM_RESOURCE_LIST UntranslatedResourceList
          )

          /*++

          Routine Description:

          Called at Pnp start.  We start up our virtual hardware simulation.

          Arguments:

          TranslatedResourceList -
          The translated resource list from Pnp

          UntranslatedResourceList -
          The untranslated resource list from Pnp

          Return Value:

          Success / Failure

          --*/

{

    PAGED_CODE();

    UNREFERENCED_PARAMETER(TranslatedResourceList);
    UNREFERENCED_PARAMETER(UntranslatedResourceList);

    //
    // Normally, we'd do things here like parsing the resource lists and
    // connecting our interrupt.  Since this is a simulation, there isn't
    // much to parse.  The parsing and connection should be the same as
    // any WDM driver.  The sections that will differ are illustrated below
    // in setting up a simulated DMA.
    //

    NTSTATUS Status = STATUS_SUCCESS;

    DbgPrint("MY-CEncoderDevice::PnpStart");

    //
    // By PnP, it's possible to receive multiple starts without an intervening
    // stop (to reevaluate resources, for example).  Thus, we only perform
    // creations of the simulation on the initial start and ignore any 
    // subsequent start.  Hardware drivers with resources should evaluate
    // resources and make changes on 2nd start.
    //
    if (!m_Device -> Started) {

        m_HardwareSimulation = new (NonPagedPool, MS_SAMPLE_ANALOG_POOL_TAG) CHardwareSimulation (this);

        if (!m_HardwareSimulation) {
            //
            // If we couldn't create the hardware simulation, fail.
            //
            Status = STATUS_INSUFFICIENT_RESOURCES;
            return Status;

        } else {
            //
            // Add the item to the object bag if we were successful. 
            //
            Status = KsAddItemToObjectBag (
                m_Device -> Bag,
                reinterpret_cast <PVOID> (m_HardwareSimulation),
                reinterpret_cast <PFNKSFREE> (CHardwareSimulation::CleanupHW)
                );	

            // add by Max
            if (!NT_SUCCESS(Status)) 
            {
                delete m_HardwareSimulation;
                return Status;
            }
            // end
        }

#if !defined(_BUILD_SW_TUNER_ON_X64)
    // DMA operation is to show typical call sequences for a real hardware driver.
    // Since this fake call below does not succeed on 64-bit, it is disabled. 
        INTERFACE_TYPE InterfaceBuffer;
        ULONG InterfaceLength;
        DEVICE_DESCRIPTION DeviceDescription;

        if (NT_SUCCESS (Status)) {
            //
            // Set up DMA...
            //
            IoGetDeviceProperty (
                m_Device -> PhysicalDeviceObject,
                DevicePropertyLegacyBusType,
                sizeof (INTERFACE_TYPE),
                &InterfaceBuffer,
                &InterfaceLength
                );

            //
            // Initialize our fake device description.  We claim to be a 
            // bus-mastering 32-bit scatter/gather capable piece of hardware.
            //
            // Ordinarilly, we'd be using InterfaceBuffer or 
            // InterfaceTypeUndefined if success as the 
            // InterfaceType below; however, for the purposes of this sample, 
            // we lie and say we're on the PCI Bus.  Otherwise, we're using map
            // registers on x86 32 bit physical to 32 bit logical and this isn't
            // what I want to show in this sample.
            //
            RtlZeroMemory(&DeviceDescription, sizeof(DeviceDescription));
            DeviceDescription.Version = DEVICE_DESCRIPTION_VERSION;
            DeviceDescription.DmaChannel = ((ULONG) ~0);
            DeviceDescription.InterfaceType = PCIBus;
            DeviceDescription.DmaWidth = Width32Bits;
            DeviceDescription.DmaSpeed = Compatible;
            DeviceDescription.ScatterGather = TRUE;
            DeviceDescription.Master = TRUE;
            DeviceDescription.Dma32BitAddresses = TRUE;
            DeviceDescription.AutoInitialize = FALSE;
            DeviceDescription.MaximumLength = (ULONG) -1;

            //
            // Get a DMA adapter object from the system.
            //
            m_DmaAdapterObject = IoGetDmaAdapter (
                m_Device -> PhysicalDeviceObject,
                &DeviceDescription,
                &m_NumberOfMapRegisters
                );

            if (!m_DmaAdapterObject) {
                Status = STATUS_UNSUCCESSFUL;
            }

        }

        if (NT_SUCCESS (Status)) {
            //
            // Initialize our DMA adapter object with AVStream.  This is 
            // **ONLY** necessary **IF** you are doing DMA directly into
            // capture buffers as this sample does.  For this,
            // KSPIN_FLAG_GENERATE_MAPPINGS must be specified on a queue.
            //

            //
            // The (1 << 20) below is the maximum size of a single s/g mapping
            // that this hardware can handle.  Note that I have pulled this
            // number out of thin air for the "fake" hardware.
            //
            KsDeviceRegisterAdapterObject (
                m_Device,
                m_DmaAdapterObject,
                (1 << 20),
                sizeof (KSMAPPING)
                );

        }

#endif
	
    }

    return Status;

}

/*************************************************/


void
CEncoderDevice::
PnpStop (
         )

         /*++

         Routine Description:

         This is the pnp stop dispatch for the encoder device.  It releases any
         adapter object previously allocated by IoGetDmaAdapter during Pnp Start.

         Arguments:

         None

         Return Value:

         None

         --*/

{

    if (m_DmaAdapterObject) {
        //
        // Return the DMA adapter back to the system.
        //
        m_DmaAdapterObject -> DmaOperations -> 
            PutDmaAdapter (m_DmaAdapterObject);

        m_DmaAdapterObject = NULL;
    }

}

/*************************************************/


NTSTATUS
CEncoderDevice::
AcquireHardwareResources (
                          IN IEncoderSink *EncoderSink,
                          IN PBDA_TRANSPORT_INFO TransportInfo
                          )

                          /*++

                          Routine Description:

                          Acquire hardware resources for the capture hardware.  If the 
                          resources are already acquired, this will return an error.
                          The hardware configuration must be passed as a VideoInfoHeader.

                          Arguments:

                          EncoderSink -
                          The capture sink attempting to acquire resources.  When scatter /
                          gather mappings are completed, the capture sink specified here is
                          what is notified of the completions.

                          VideoInfoHeader -
                          Information about the capture stream.  This **MUST** remain
                          stable until the caller releases hardware resources.  Note
                          that this could also be guaranteed by bagging it in the device
                          object bag as well.

                          Return Value:

                          Success / Failure

                          --*/

{

    PAGED_CODE();

    NTSTATUS Status = STATUS_SUCCESS;

    // We do not get device instance ID in Create or PnpStart Method
    // because the ID information has not been writen to the registry.
    GetDeviceInstanceID();

    //
    // If we're the first pin to go into acquire (remember we can have
    // a filter in another graph going simultaneously), grab the resources.
    //

    if (InterlockedCompareExchange (
        &m_PinsWithResources,
        1,
        0) == 0) {

            m_TransportInfo = TransportInfo;

            //
            // If there's an old hardware simulation sitting around for some
            // reason, blow it away.
            //
            if (m_TsSynth) {

                // edit by Max
                /*
                KsRemoveItemFromObjectBag (
                    m_Device -> Bag,
                    m_TsSynth,
                    TRUE
                    );
                */
                delete m_TsSynth;
                m_TsSynth = NULL;
            }

            //
            // Create the necessary type of transport stream synthesizer.
            //
            if (m_TransportInfo)
            {
                m_TsSynth = new (NonPagedPool, MS_SAMPLE_ANALOG_POOL_TAG) 
                    CTsSynthesizer (
                    m_Device,		
                    m_TransportInfo -> ulcbPhyiscalPacket,
                    m_TransportInfo -> ulcbPhyiscalFrame / m_TransportInfo -> ulcbPhyiscalPacket,
                    m_ulFrequency,
                    m_czDeviceInstanceID
                    );
            }
            else
                //
                // We don't synthesize anything but RGB 24 and UYVY.
                //
                Status = STATUS_INVALID_PARAMETER;

            if (NT_SUCCESS (Status) && !m_TsSynth) {

                Status = STATUS_INSUFFICIENT_RESOURCES;

            } 

            //
            // Bag the image synthesizer.
            //
            if (NT_SUCCESS (Status)) {
                //
                // If everything has succeeded thus far, set the capture sink.
                //
                m_EncoderSink = EncoderSink;

            } else {
                //
                // If anything failed in here, we release the resources we've
                // acquired.
                //
                ReleaseHardwareResources ();
            }

    } else {

        //
        // TODO: Better status code?
        //
        Status = STATUS_SHARING_VIOLATION;

    }

    return Status;

}

/*************************************************/


void
CEncoderDevice::
ReleaseHardwareResources (
                          )

                          /*++

                          Routine Description:

                          Release hardware resources.  This should only be called by
                          an object which has acquired them.

                          Arguments:

                          None

                          Return Value:

                          None

                          --*/

{

    PAGED_CODE();

    //
    // Blow away the image synth.
    //
    if (m_TsSynth) {
    // edit by Max
        /*
        KsRemoveItemFromObjectBag (
            m_Device -> Bag,
            m_TsSynth,
            TRUE
            );
        */
        delete m_TsSynth;
        m_TsSynth = NULL;

    }

    m_TransportInfo = NULL;
    m_EncoderSink = NULL;

    //
    // Release our "lock" on hardware resources.  This will allow another
    // pin (perhaps in another graph) to acquire them.
    //
    InterlockedExchange (
        &m_PinsWithResources,
        0
        );

}

void
CEncoderDevice::
SetFrequency(
             ULONG ulFrequency
             )
{
    DbgPrint("MY-CDevice::SetFrequency %d", ulFrequency);

    if (m_ulFrequency != ulFrequency)
    {
        m_ulFrequency = ulFrequency;
        if (m_TsSynth) 
            m_TsSynth->SetFrequency(ulFrequency);
    }
}

void
CEncoderDevice::
GetFrequency(
             ULONG *ulFrequency
             )
{
    *ulFrequency = m_ulFrequency;
}


WCHAR* 
CEncoderDevice::
GetDeviceInstanceID()
{
    if (!m_bGetDeviceInstanceID)
    {
        UNICODE_STRING DevInstanceID;
        RtlInitUnicodeString(&DevInstanceID, m_czDeviceInstanceID);
        DevInstanceID.MaximumLength = (MAX_PATH + 1) * sizeof(WCHAR);
        NTSTATUS    Status = STATUS_SUCCESS;

        Status	= PptRegGetDeviceParameterSZ(
            m_Device->PhysicalDeviceObject,
            L"DeviceInstanceID",
            &DevInstanceID);

        if (NT_SUCCESS (Status))
        {
            DbgPrint("Get Device Instance ID2 %S", m_czDeviceInstanceID);
			
	     if(m_czDeviceInstanceID[0])
	     	{
                   m_bGetDeviceInstanceID = true;
		 }
            else
		     DbgPrint("GetDeviceInstanceID is NULL,Registry isn't updated yet!\n");
        }
        else
        {
            m_czDeviceInstanceID[0] = '\0';
        }
    }

    return m_czDeviceInstanceID;
}

/*************************************************/


NTSTATUS
CEncoderDevice::
Start (
       )

       /*++

       Routine Description:

       Start the capture device based on the video info header we were told
       about when resources were acquired.

       Arguments:

       None

       Return Value:

       Success / Failure

       --*/

{
    PAGED_CODE();
    WCHAR *strDeviceInstanceId = GetDeviceInstanceID();
    if(!strDeviceInstanceId[0])
    {
         DbgPrint("CEncoderDevice::Start Failed! no device instance id\n");
	  return STATUS_ACCESS_DENIED;
    }
    m_LastMappingsCompleted = 0;
    m_InterruptTime = 0;

    m_TsSynth -> Start();

    return
        m_HardwareSimulation -> Start (
        m_TsSynth,
        m_TransportInfo -> AvgTimePerFrame,
        m_TransportInfo -> ulcbPhyiscalPacket,
        m_TransportInfo -> ulcbPhyiscalFrame / m_TransportInfo -> ulcbPhyiscalPacket
        );


}

/*************************************************/


NTSTATUS
CEncoderDevice::
Pause (
       IN BOOLEAN Pausing
       )

       /*++

       Routine Description:

       Pause or unpause the hardware simulation.  This is an effective start
       or stop without resetting counters and formats.  Note that this can
       only be called to transition from started -> paused -> started.  Calling
       this without starting the hardware with Start() does nothing.

       Arguments:

       Pausing -
       An indicatation of whether we are pausing or unpausing

       TRUE -
       Pause the hardware simulation

       FALSE -
       Unpause the hardware simulation

       Return Value:

       Success / Failure

       --*/

{

    PAGED_CODE();

    return
        m_HardwareSimulation -> Pause (
        Pausing
        );

}

/*************************************************/


NTSTATUS
CEncoderDevice::
Stop (
      )

      /*++

      Routine Description:

      Stop the capture device.

      Arguments:

      None

      Return Value:

      Success / Failure

      --*/

{

    PAGED_CODE();
    NTSTATUS Status;

    Status= m_HardwareSimulation -> Stop ();

    m_TsSynth -> Stop();

    return Status;
}

/*************************************************/


ULONG
CEncoderDevice::
ProgramScatterGatherMappings (
                              IN PUCHAR *Buffer,
                              IN PKSMAPPING Mappings,
                              IN ULONG MappingsCount
                              )

                              /*++

                              Routine Description:

                              Program the scatter / gather mappings for the "fake" hardware.

                              Arguments:

                              Buffer -
                              Points to a pointer to the virtual address of the topmost
                              scatter / gather chunk.  The pointer will be updated as the
                              device "programs" mappings.  Reason for this is that we get
                              the physical addresses and sizes, but must calculate the virtual
                              addresses...  This is used as scratch space for that.

                              Mappings -
                              An array of mappings to program

                              MappingsCount -
                              The count of mappings in the array

                              Return Value:

                              The number of mappings successfully programmed

                              --*/

{

    PAGED_CODE();

    return 
        m_HardwareSimulation -> ProgramScatterGatherMappings (
        Buffer,
        Mappings,
        MappingsCount,
        sizeof (KSMAPPING)
        );

}

/*************************************************************************

LOCKED CODE

**************************************************************************/

#ifdef ALLOC_PRAGMA
#pragma code_seg()
#endif // ALLOC_PRAGMA


ULONG
CEncoderDevice::
QueryInterruptTime (
                    )

                    /*++

                    Routine Description:

                    Return the number of frame intervals that have elapsed since the
                    start of the device.  This will be the frame number.

                    Arguments:

                    None

                    Return Value:

                    The interrupt time of the device (the number of frame intervals that
                    have elapsed since the start of the device).

                    --*/

{

    return m_InterruptTime;

}

/*************************************************/


void
CEncoderDevice::
Interrupt (
           )

           /*++

           Routine Description:

           This is the "faked" interrupt service routine for this device.  It
           is called at dispatch level by the hardware simulation.

           Arguments:

           None

           Return Value:

           None

           --*/

{

    m_InterruptTime++;

    //
    // Realistically, we'd do some hardware manipulation here and then queue
    // a DPC.  Since this is fake hardware, we do what's necessary here.  This
    // is pretty much what the DPC would look like short of the access
    // of hardware registers (ReadNumberOfMappingsCompleted) which would likely
    // be done in the ISR.
    //
    ULONG NumMappingsCompleted = 
        m_HardwareSimulation -> ReadNumberOfMappingsCompleted ();

    //
    // Inform the capture sink that a given number of scatter / gather
    // mappings have completed.
    //
    m_EncoderSink -> CompleteMappings (
        NumMappingsCompleted - m_LastMappingsCompleted
        );
    //  m_HardwareSimulation->m_SynthesisBuffer  buffer address


    m_LastMappingsCompleted = NumMappingsCompleted;



}

NTSTATUS
CEncoderDevice::GetTVStandard(
     const WCHAR* strHardwareID
     ,ULONG & ulTVStandard
    )
{
     if(!strHardwareID)
	   return STATUS_ABANDONED;

     NTSTATUS Status = STATUS_SUCCESS;
     DbgPrint("Hardware id is %S\n",strHardwareID);
     if(0 == wcscmp(strHardwareID , L"ms_swtNTSC"))//NTSC
      {
              DbgPrint("set analog NTSC device!\n");
              ulTVStandard =  KS_AnalogVideo_NTSC_M | KS_AnalogVideo_PAL_M | KS_AnalogVideo_PAL_N;                 // AnalogVideoStandard
      }
      else if(0 == wcscmp(strHardwareID, L"ms_swtPAL"))//PAL
      {
              DbgPrint("set analog PAL device!\n");
              ulTVStandard = KS_AnalogVideo_PAL_B | KS_AnalogVideo_PAL_D | KS_AnalogVideo_PAL_G | KS_AnalogVideo_PAL_H | KS_AnalogVideo_PAL_I;
      }
      else
      {
           DbgPrint("unkown analog Device!\n");
           Status = STATUS_ABANDONED;
      }
      return Status;
}


/**************************************************************************

DESCRIPTOR AND DISPATCH LAYOUT

**************************************************************************/

//
// CaptureFilterDescriptor:
//
// The filter descriptor for the capture device.
DEFINE_KSFILTER_DESCRIPTOR_TABLE (FilterDescriptors) { 
    &BdaAnlgTunerFilterDescriptor,
        &BdaTVAudioFilterDescriptor,
        &BdaXBarFilterDescriptor,
        &BdaAnlgCaptureFilterDescriptor,
        &BdaEncoderFilterDescriptor
};

//
// EncoderDeviceDispatch:
//
// This is the dispatch table for the capture device.  Plug and play
// notifications as well as power management notifications are dispatched
// through this table.
//
const
KSDEVICE_DISPATCH
EncoderDeviceDispatch = {
    CEncoderDevice::DispatchCreate,         // PnP Add Device
    CEncoderDevice::DispatchPnpStart,       // PnP Start
    NULL,                                   // Post-Start
    NULL,                                   // Pnp Query Stop
    NULL,                                   // Pnp Cancel Stop
    CEncoderDevice::DispatchPnpStop,        // Pnp Stop
    NULL,                                   // Pnp Query Remove
    NULL,                                   // Pnp Cancel Remove
    NULL,                                   // Pnp Remove
    NULL,                                   // Pnp Query Capabilities
    NULL,                                   // Pnp Surprise Remove
    NULL,                                   // Query Power
    NULL                                    // Set Power
};

//
// EncoderDeviceDescriptor:
//
// This is the device descriptor for the capture device.  It points to the
// dispatch table and contains a list of filter descriptors that describe
// filter-types that this device supports.  Note that the filter-descriptors
// can be created dynamically and the factories created via 
// KsCreateFilterFactory as well.  
//
const
KSDEVICE_DESCRIPTOR
EncoderDeviceDescriptor = {
    &EncoderDeviceDispatch,
    SIZEOF_ARRAY (FilterDescriptors),
    FilterDescriptors,
    KSDEVICE_DESCRIPTOR_VERSION
};

/**************************************************************************

INITIALIZATION CODE

**************************************************************************/


extern "C"
NTSTATUS
DriverEntry (
             IN PDRIVER_OBJECT DriverObject,
             IN PUNICODE_STRING RegistryPath
             )

             /*++

             Routine Description:

             Driver entry point.  Pass off control to the AVStream initialization
             function (KsInitializeDriver) and return the status code from it.

             Arguments:

             DriverObject -
             The WDM driver object for our driver

             RegistryPath -
             The registry path for our registry info

             Return Value:

             As from KsInitializeDriver

             --*/

{
    NTSTATUS    Status = STATUS_SUCCESS;

    //
    // Simply pass the device descriptor and parameters off to AVStream
    // to initialize us.  This will cause filter factories to be set up
    // at add & start.  Everything is done based on the descriptors passed
    // here.
    //
    Status = KsInitializeDriver (
        DriverObject,
        RegistryPath,
        &EncoderDeviceDescriptor
        );

    return Status;
}


