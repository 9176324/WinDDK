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
/**************************************************************************
    File    : encoderpin.cpp
    Abstract: Contains the descriptors, property handlers and intersect
              handlers for the encoder pin.
**************************************************************************/

#include "common.h"


/**************************************************************************

    PAGEABLE CODE

**************************************************************************/

#ifdef ALLOC_PRAGMA
#pragma code_seg("PAGE")
#endif // ALLOC_PRAGMA


CEncoderPin::
CEncoderPin (
    IN PKSPIN Pin
    ) :
    m_Pin (Pin)

/*++

Routine Description:

    Construct a new capture pin.

Arguments:

    Pin -
        The AVStream pin object corresponding to the capture pin

Return Value:

    None

--*/

{

    PAGED_CODE();

    PKSDEVICE Device = KsPinGetDevice (Pin);

    //
    // Set up our device pointer.  This gives us access to "hardware I/O"
    // during the capture routines.
    //
    m_Device = reinterpret_cast <CEncoderDevice *> (Device -> Context);

}

/*************************************************/


NTSTATUS
CEncoderPin::
DispatchCreate (
    IN PKSPIN Pin,
    IN PIRP Irp
    )

/*++

Routine Description:

    Create a new capture pin.  This is the creation dispatch for
    the video capture pin.

Arguments:

    Pin -
        The pin being created

    Irp -
        The creation Irp

Return Value:

    Success / Failure

--*/

{

    PAGED_CODE();

    NTSTATUS Status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(Irp);

    CEncoderPin *EncPin = new (NonPagedPool, MS_SAMPLE_ANALOG_POOL_TAG) CEncoderPin (Pin);
    if (!EncPin) {
        //
        // Return failure if we couldn't create the pin.
        //
        Status = STATUS_INSUFFICIENT_RESOURCES;

    } else {
        //
        // Add the item to the object bag if we we were successful. 
        // Whenever the pin closes, the bag is cleaned up and we will be
        // freed.
        //
        Status = KsAddItemToObjectBag (
            Pin -> Bag,
            reinterpret_cast <PVOID> (EncPin),
            reinterpret_cast <PFNKSFREE> (CEncoderPin::Cleanup)
            );

        if (!NT_SUCCESS (Status)) {
            delete EncPin;
        } else {
            Pin -> Context = reinterpret_cast <PVOID> (EncPin);
        }

    }

    //
    // If we succeeded so far, stash the video info header away and change
    // our allocator framing to reflect the fact that only now do we know
    // the framing requirements based on the connection format.
    //
    PBDA_TRANSPORT_INFO TransportInfo = NULL;

    if (NT_SUCCESS (Status)) {

        TransportInfo = EncPin -> EncoderBdaTransportInfo ();
        if (!TransportInfo) {
            Status = STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    if (NT_SUCCESS (Status)) {
        
        //
        // We need to edit the descriptor to ensure we don't mess up any other
        // pins using the descriptor or touch read-only memory.
        //
        Status = KsEdit (Pin, &Pin -> Descriptor, MS_SAMPLE_ANALOG_POOL_TAG);

        if (NT_SUCCESS (Status)) {
            Status = KsEdit (
                Pin, 
                &(Pin -> Descriptor -> AllocatorFraming),
                MS_SAMPLE_ANALOG_POOL_TAG
                );
        }

        //
        // If the edits proceeded without running out of memory, adjust 
        // the framing based on the video info header.
        //
        if (NT_SUCCESS (Status)) {

            //
            // We've KsEdit'ed this...  I'm safe to cast away constness as
            // long as the edit succeeded.
            //
            PKSALLOCATOR_FRAMING_EX Framing =
                const_cast <PKSALLOCATOR_FRAMING_EX> (
                    Pin -> Descriptor -> AllocatorFraming
                    );

            Framing -> FramingItem [0].Frames = 8;

            //
            // The physical and optimal ranges must be biSizeImage.  We only
            // support one frame size, precisely the size of each capture
            // image.
            //
            Framing -> FramingItem [0].PhysicalRange.MinFrameSize =
                Framing -> FramingItem [0].PhysicalRange.MaxFrameSize =
                Framing -> FramingItem [0].FramingRange.Range.MinFrameSize =
                Framing -> FramingItem [0].FramingRange.Range.MaxFrameSize =
                TransportInfo -> ulcbPhyiscalFrame;

            Framing -> FramingItem [0].PhysicalRange.Stepping = 
                Framing -> FramingItem [0].FramingRange.Range.Stepping =
                0;

        }

    }

    return Status;

}

/*************************************************/


PBDA_TRANSPORT_INFO 
CEncoderPin::
EncoderBdaTransportInfo (
    )

/*++

Routine Description:

    Encoder the video info header out of the connection format.  This
    is what we use to base synthesized images off.

Arguments:

    None

Return Value:

    The captured video info header or NULL if there is insufficient
    memory.

--*/

{

    PAGED_CODE();

    m_TransportInfo = reinterpret_cast <PBDA_TRANSPORT_INFO> (
        ExAllocatePoolWithTag (
            NonPagedPool,
            sizeof(BDA_TRANSPORT_INFO),
			MS_SAMPLE_ANALOG_POOL_TAG
            )
        );

    if (!m_TransportInfo)
        return NULL;

    //
    // Bag the newly allocated header space.  This will get cleaned up
    // automatically when the pin closes.
    //
    NTSTATUS Status =
        KsAddItemToObjectBag (
            m_Pin -> Bag,
            reinterpret_cast <PVOID> (m_TransportInfo),
            NULL
            );

    if (!NT_SUCCESS (Status)) {

        ExFreePool (m_TransportInfo);
        return NULL;

    } else {

        m_TransportInfo->ulcbPhyiscalPacket = PS_PACKET_SIZE;
        m_TransportInfo->ulcbPhyiscalFrame = PS_SAMPLE_SIZE;
        m_TransportInfo->ulcbPhyiscalFrameAlignment = 1;
        m_TransportInfo->AvgTimePerFrame = PS_SAMPLE_SIZE * 8 * 10 / 15;  //in 100ns units, <15Mbit/s

    }

    return m_TransportInfo;

}
/*************************************************/


NTSTATUS
CEncoderPin::
Process (
    )

/*++

Routine Description:

    The process dispatch for the pin bridges to this location.
    We handle setting up scatter gather mappings, etc...

Arguments:

    None

Return Value:

    Success / Failure

--*/

{

    PAGED_CODE();

    NTSTATUS Status = STATUS_SUCCESS;
    PKSSTREAM_POINTER Leading;

    Leading = KsPinGetLeadingEdgeStreamPointer (
        m_Pin,
        KSSTREAM_POINTER_STATE_LOCKED
        );

    while (NT_SUCCESS (Status) && Leading) {

        PKSSTREAM_POINTER ClonePointer;
        PSTREAM_POINTER_CONTEXT SPContext;

        //
        // For optimization sake in this particular sample, I will only keep
        // one clone stream pointer per frame.  This complicates the logic
        // here but simplifies the completions.
        //
        // I'm also choosing to do this since I need to keep track of the
        // virtual addresses corresponding to each mapping since I'm faking
        // DMA.  It simplifies that too.
        //
        if (!m_PreviousStreamPointer) {
            //
            // First thing we need to do is clone the leading edge.  This allows
            // us to keep reference on the frames while they're in DMA.
            //
            Status = KsStreamPointerClone (
                Leading,
                NULL,
                sizeof (STREAM_POINTER_CONTEXT),
                &ClonePointer
                );

            //
            // I use this for easy chunking of the buffer.  We're not really
            // dealing with physical addresses.  This keeps track of what 
            // virtual address in the buffer the current scatter / gather 
            // mapping corresponds to for the fake hardware.
            //
            if (NT_SUCCESS (Status)) {

                //
                // Set the stream header data used to 0.  We update this 
                // in the DMA completions.  For queues with DMA, we must
                // update this field ourselves.
                //
                ClonePointer -> StreamHeader -> DataUsed = 0;

                SPContext = reinterpret_cast <PSTREAM_POINTER_CONTEXT> 
                    (ClonePointer -> Context);

                SPContext -> BufferVirtual = 
                    reinterpret_cast <PUCHAR> (
                        ClonePointer -> StreamHeader -> Data
                        );
            } else {
                SPContext = NULL;
            }

        } else {

            ClonePointer = m_PreviousStreamPointer;
            SPContext = reinterpret_cast <PSTREAM_POINTER_CONTEXT> 
                (ClonePointer -> Context);
            Status = STATUS_SUCCESS;
        }

        //
        // If the clone failed, likely we're out of resources.  Break out
        // of the loop for now.  We may end up starving DMA.
        //
        if (!NT_SUCCESS (Status) || SPContext == NULL) {
            KsStreamPointerUnlock (Leading, FALSE);
            break;
        }

        //
        // Program the fake hardware.  I would use Clone -> OffsetOut.*, but
        // because of the optimization of one stream pointer per frame, it
        // doesn't make complete sense.
        //
        ULONG MappingsUsed =
            m_Device -> ProgramScatterGatherMappings (
                &(SPContext -> BufferVirtual),
                Leading -> OffsetOut.Mappings,
                Leading -> OffsetOut.Remaining
                );

        //
        // In order to keep one clone per frame and simplify the fake DMA
        // logic, make a check to see if we completely used the mappings in
        // the leading edge.  Set a flag.
        //
        if (MappingsUsed == Leading -> OffsetOut.Remaining) {
            m_PreviousStreamPointer = NULL;
        } else {
            m_PreviousStreamPointer = ClonePointer;
        }

        if (MappingsUsed) {
            //
            // If any mappings were added to scatter / gather queues, 
            // advance the leading edge by that number of mappings.  If 
            // we run off the end of the queue, Status will be 
            // STATUS_DEVICE_NOT_READY.  Otherwise, the leading edge will
            // point to a new frame.  The previous one will not have been
            // dismissed (unless "DMA" completed) since there's a clone
            // pointer referencing the frames.
            //
            Status =
                KsStreamPointerAdvanceOffsets (
                    Leading,
                    0,
                    MappingsUsed,
                    FALSE
                    );
        } else {

            //
            // The hardware was incapable of adding more entries.  The S/G
            // table is full.
            //
            Status = STATUS_PENDING;
            break;

        }

    }

    //
    // If the leading edge failed to lock (this is always possible, remember
    // that locking CAN occassionally fail), don't blow up passing NULL
    // into KsStreamPointerUnlock.  Also, set m_PendIo to kick us later...
    //
    if (!Leading) {

        m_PendIo = TRUE;

        //
        // If the lock failed, there's no point in getting called back 
        // immediately.  The lock could fail due to insufficient memory,
        // etc...  In this case, we don't want to get called back immediately.
        // Return pending.  The m_PendIo flag will cause us to get kicked
        // later.
        //
        Status = STATUS_PENDING;
    }

    //
    // If we didn't run the leading edge off the end of the queue, unlock it.
    //
    if (NT_SUCCESS (Status) && Leading) {
        KsStreamPointerUnlock (Leading, FALSE);
    } else {
        //
        // DEVICE_NOT_READY indicates that the advancement ran off the end
        // of the queue.  We couldn't lock the leading edge.
        //
        if (Status == STATUS_DEVICE_NOT_READY) Status = STATUS_SUCCESS;
    }

    //
    // If we failed with something that requires pending, set the pending I/O
    // flag so we know we need to start it again in a completion DPC.
    //
    if (!NT_SUCCESS (Status) || Status == STATUS_PENDING) {
        m_PendIo = TRUE;
    }

    return Status;

}

/*************************************************/


NTSTATUS
CEncoderPin::
CleanupReferences (
    )

/*++

Routine Description:

    Clean up any references we're holding on frames after we abruptly
    stop the hardware.

Arguments:

    None

Return Value:

    Success / Failure

--*/

{

    PAGED_CODE();

    PKSSTREAM_POINTER Clone = KsPinGetFirstCloneStreamPointer (m_Pin);
    PKSSTREAM_POINTER NextClone = NULL;

    //
    // Walk through the clones, deleting them, and setting DataUsed to
    // zero since we didn't use any data!
    //
    while (Clone) {

        NextClone = KsStreamPointerGetNextClone (Clone);

        Clone -> StreamHeader -> DataUsed = 0;
        KsStreamPointerDelete (Clone);

        Clone = NextClone;

    }

    return STATUS_SUCCESS;

}

/*************************************************/


NTSTATUS
CEncoderPin::
SetState (
    IN KSSTATE ToState,
    IN KSSTATE FromState
    )

/*++

Routine Description:

    This is called when the caputre pin transitions state.  The routine
    attempts to acquire / release any hardware resources and start up
    or shut down capture based on the states we are transitioning to
    and away from.

Arguments:

    ToState -
        The state we're transitioning to

    FromState -
        The state we're transitioning away from

Return Value:

    Success / Failure

--*/

{

    PAGED_CODE();

    NTSTATUS Status = STATUS_SUCCESS;
	DbgPrint("MY-CEncoderPin::SetState F%d T%d", FromState, ToState);

    switch (ToState) {

        case KSSTATE_STOP:

            //
            // First, stop the hardware if we actually did anything to it.
            //
            if (m_HardwareState != HardwareStopped) {
                m_Device -> Stop ();
                ASSERT (NT_SUCCESS (Status));

                m_HardwareState = HardwareStopped;
            }

            //
            // We've stopped the "fake hardware".  It has cleared out
            // it's scatter / gather tables and will no longer be 
            // completing clones.  We had locks on some frames that were,
            // however, in hardware.  This will clean them up.  An
            // alternative location would be in the reset dispatch.
            // Note, however, that the reset dispatch can occur in any
            // state and this should be understood.
            //
            // Some hardware may fill all S/G mappings before stopping...
            // in this case, you may not have to do this.  The 
            // "fake hardware" here simply stops filling mappings and 
            // cleans its scatter / gather tables out on the Stop call.
            //
            Status = CleanupReferences ();

            //
            // Release any hardware resources related to this pin.
            //
            if (m_AcquiredResources) {
                //
                // If we got an interface to the clock, we must release it.
                //
                if (m_Clock) {
                    m_Clock -> Release ();
                    m_Clock = NULL;
                }

                m_Device -> ReleaseHardwareResources (
                    );

                m_AcquiredResources = FALSE;

				DbgPrint("MY-Pin Stopped");
            }

            break;

        case KSSTATE_ACQUIRE:
            //
            // Acquire any hardware resources related to this pin.  We should
            // only acquire them here -- **NOT** at filter create time. 
            // This means we do not fail creation of a filter because of
            // limited hardware resources.
            //
            if (FromState == KSSTATE_STOP) {
                Status = m_Device -> AcquireHardwareResources (
                    this,
                    m_TransportInfo
                    );

                if (NT_SUCCESS (Status)) {
                    m_AcquiredResources = TRUE;

                    //
                    // Attempt to get an interface to the master clock.
                    // This will fail if one has not been assigned.  Since
                    // one must be assigned while the pin is still in 
                    // KSSTATE_STOP, this is a guranteed method of getting
                    // the clock should one be assigned.
                    //
                    if (!NT_SUCCESS (
                        KsPinGetReferenceClockInterface (
                            m_Pin,
                            &m_Clock
                            )
                        )) {

                        //
                        // If we could not get an interface to the clock,
                        // don't use one.  
                        //
                        m_Clock = NULL;

                    }

                } else {
                    m_AcquiredResources = FALSE;
                }

            } else {
                //
                // Standard transport pins will always receive transitions in
                // +/- 1 manner.  This means we'll always see a PAUSE->ACQUIRE
                // transition before stopping the pin.  
                //
                // The below is done because on DirectX 8.0, when the pin gets
                // a message to stop, the queue is inaccessible.  The reset 
                // which comes on every stop happens after this (at which time
                // the queue is inaccessible also).  So, for compatibility with
                // DirectX 8.0, I am stopping the "fake" hardware at this
                // point and cleaning up all references we have on frames.  See
                // the comments above regarding the CleanupReferences call.
                //
                // If this sample were targeting XP only, the below code would
                // not be here.  Again, I only do this so the sample does not
                // hang when it is stopped running on a configuration such as
                // Win2K + DX8. 
                //
                if (m_HardwareState != HardwareStopped) {
                    Status = m_Device -> Stop ();
                    if (NT_SUCCESS (Status)) {
                        m_HardwareState = HardwareStopped;
                    }
                }

                Status = CleanupReferences ();
            }

            break;

        case KSSTATE_PAUSE:
            //
            // Stop the hardware simulation if we're coming down from run.
            //
            if (FromState == KSSTATE_RUN) {

                Status = m_Device -> Pause (TRUE);

                if (NT_SUCCESS (Status)) {
                    m_HardwareState = HardwarePaused;
                }

            }
            break;

        case KSSTATE_RUN:
            //
            // Start the hardware simulation or unpause it depending on
            // whether we're initially running or we've paused and restarted.
            //
            if (m_HardwareState == HardwarePaused) {
                Status = m_Device -> Pause (FALSE);
            } else {
                m_PreviousStreamPointer = NULL;
                Status = m_Device -> Start ();
            }

            if (NT_SUCCESS (Status)) {
                m_HardwareState = HardwareRunning;
            }

            break;

    }

    return Status;

}


/**************************************************************************

    LOCKED CODE

**************************************************************************/

#ifdef ALLOC_PRAGMA
#pragma code_seg()
#endif // ALLOC_PRAGMA


void
CEncoderPin::
CompleteMappings (
    IN ULONG NumMappings
    )

/*++

Routine Description:

    Called to notify the pin that a given number of scatter / gather
    mappings have completed.  Let the buffers go if possible.
    We're called at DPC.

Arguments:

    NumMappings -
        The number of mappings that have completed.

Return Value:

    None

--*/

{

    ULONG MappingsRemaining = NumMappings;

    //
    // Walk through the clones list and delete clones whose time has come.
    // The list is guaranteed to be kept in the order they were cloned.
    //
    PKSSTREAM_POINTER Clone = KsPinGetFirstCloneStreamPointer (m_Pin);

    while (MappingsRemaining && Clone) {

        PKSSTREAM_POINTER NextClone = KsStreamPointerGetNextClone (Clone);

#if defined(_BUILD_SW_TUNER_ON_X64)

#else
        //
        // Count up the number of bytes we've completed and mark this
        // in the Stream Header.  In mapped queues 
        // (KSPIN_FLAG_GENERATE_MAPPINGS), this is the responsibility of
        // the minidriver.  In non-mapped queues, AVStream performs this.
        //
        ULONG MappingsToCount = 
            (MappingsRemaining > Clone -> OffsetOut.Remaining) ?
                 Clone -> OffsetOut.Remaining :
                 MappingsRemaining;

        //
        // Update DataUsed according to the mappings.
        //
        for (ULONG CurMapping = 0; CurMapping < MappingsToCount; CurMapping++) {
            Clone -> StreamHeader -> DataUsed +=
                Clone -> OffsetOut.Mappings [CurMapping].ByteCount;
        }
#endif

        // 
        // If we have completed all remaining mappings in this clone, it
        // is an indication that the clone is ready to be deleted and the
        // buffer released.  Set anything required in the stream header which
        // has not yet been set.  If we have a clock, we can timestamp the
        // sample.
        //
#if defined(_BUILD_SW_TUNER_ON_X64)
        if (Clone -> StreamHeader -> DataUsed >= Clone -> OffsetOut.Remaining) {
#else
        if (MappingsRemaining >= Clone -> OffsetOut.Remaining) {
#endif
            Clone -> StreamHeader -> Duration =
                m_TransportInfo -> AvgTimePerFrame;

            Clone -> StreamHeader -> PresentationTime.Numerator =
                Clone -> StreamHeader -> PresentationTime.Denominator = 1;

            //
            // If a clock has been assigned, timestamp the packets with the
            // time shown on the clock. 
            //
            if (m_Clock) {

                LONGLONG ClockTime = m_Clock -> GetTime ();

                Clone -> StreamHeader -> PresentationTime.Time = ClockTime;

                Clone -> StreamHeader -> OptionsFlags =
                    KSSTREAM_HEADER_OPTIONSF_TIMEVALID |
                    KSSTREAM_HEADER_OPTIONSF_DURATIONVALID;

            } else {
	      //
	      // If there is no clock, don't time stamp the packets.
	      //
	      Clone -> StreamHeader -> PresentationTime.Time = 0;
	      
            }

            //
            // If all of the mappings in this clone have been completed,
            // delete the clone.  We've already updated DataUsed above.
            //

#if defined(_BUILD_SW_TUNER_ON_X64)
            MappingsRemaining--;
#else
            MappingsRemaining -= Clone -> OffsetOut.Remaining;
#endif
            KsStreamPointerDelete (Clone);

        } else {
            //
            // If only part of the mappings in this clone have been completed,
            // update the pointers.  Since we're guaranteed this won't advance
            // to a new frame by the check above, it won't fail.
            //
#if defined(_BUILD_SW_TUNER_ON_X64)
            KsStreamPointerAdvanceOffsets (
                Clone,
                0,
                Clone -> StreamHeader -> DataUsed,
                FALSE
                );

#else
            KsStreamPointerAdvanceOffsets (
                Clone,
                0,
                MappingsRemaining,
                FALSE
                );

#endif
            MappingsRemaining = 0;

        }

        //
        // Go to the next clone.
        //
        Clone = NextClone;

    }

    //
    // If we've used all the mappings in hardware and pended, we can kick
    // processing to happen again if we've completed mappings.
    //
    if (m_PendIo) {
        m_PendIo = TRUE;
        KsPinAttemptProcessing (m_Pin, TRUE);
    }

}

/**************************************************************************

    DISPATCH AND DESCRIPTOR LAYOUT

**************************************************************************/

//
// This is the data range description of the capture output pin.
//

// MEDIASUBTYPE_None
#define MEDIASUBTYPE_MPEG2_PROGRAM\
  0xe06d8022, 0xdb46, 0x11cf, 0xb4, 0xd1, 0x00, 0x80, 0x05f, 0x6c, 0xbb, 0xea

const
KSDATARANGE FormatEncoderOut =
{
   // insert the KSDATARANGE and KSDATAFORMAT here
    {
        sizeof( KSDATARANGE),                               // FormatSize
        0,                                                  // Flags - (N/A)
        PS_SAMPLE_SIZE,                                     // SampleSize
        0,                                                  // Reserved
        { STATIC_KSDATAFORMAT_TYPE_STREAM },                // MajorFormat
        { MEDIASUBTYPE_MPEG2_PROGRAM},						// SubFormat
        { STATIC_KSDATAFORMAT_SPECIFIER_NONE }              // Specifier
    }
};

//
// This is the data range description of the capture input pin.
//
const
KS_DATARANGE_BDA_TRANSPORT FormatEncoderIn =
{
   // insert the KSDATARANGE and KSDATAFORMAT here
    {
        sizeof( KS_DATARANGE_BDA_TRANSPORT),                // FormatSize
        0,                                                  // Flags - (N/A)
        0,                                                  // SampleSize - (N/A)
        0,                                                  // Reserved
        { STATIC_KSDATAFORMAT_TYPE_STREAM },                // MajorFormat
        { STATIC_KSDATAFORMAT_TYPE_MPEG2_TRANSPORT },       // SubFormat
        { STATIC_KSDATAFORMAT_SPECIFIER_BDA_TRANSPORT }     // Specifier
    },
    // insert the BDA_TRANSPORT_INFO here
    {
        PS_PACKET_SIZE,                     //  ulcbPhyiscalPacket
        PS_SAMPLE_SIZE,                     //  ulcbPhyiscalFrame
        0,          //  ulcbPhyiscalFrameAlignment (no requirement)
        0           //  AvgTimePerFrame (not known)
    }
};

//
// EncoderPinDispatch:
//
// This is the dispatch table for the capture pin.  It provides notifications
// about creation, closure, processing, data formats, etc...
//
extern const
KSPIN_DISPATCH
EncoderPinDispatch = {
    CEncoderPin::DispatchCreate,            // Pin Create
    NULL,                                   // Pin Close
    CEncoderPin::DispatchProcess,           // Pin Process
    NULL,                                   // Pin Reset
    NULL,                                   // Pin Set Data Format
    CEncoderPin::DispatchSetState,          // Pin Set Device State
    NULL,                                   // Pin Connect
    NULL,                                   // Pin Disconnect
    NULL,                                   // Clock Dispatch
    NULL                                    // Allocator Dispatch
};

//
// InputPinDispatch:
//
// This is the dispatch table for the capture pin.  It provides notifications
// about creation, closure, processing, data formats, etc...
//
extern const
KSPIN_DISPATCH
InputPinDispatch = {
    CEncoderPin::DispatchCreate,            // Pin Create
    NULL,                                   // Pin Close
    NULL,                                   // Pin Process
    NULL,                                   // Pin Reset
    NULL,                                   // Pin Set Data Format
    NULL,                                   // Pin Set Device State
    NULL,                                   // Pin Connect
    NULL,                                   // Pin Disconnect
    NULL,                                   // Clock Dispatch
    NULL                                    // Allocator Dispatch
};

//
// EncoderPinAllocatorFraming:
//
// This is the simple framing structure for the capture pin.  Note that this
// will be modified via KsEdit when the actual capture format is determined.
//
extern
DECLARE_SIMPLE_FRAMING_EX (
    EncoderPinAllocatorFraming,                     //  FramingExName
    STATICGUIDOF (KSMEMORY_TYPE_KERNEL_NONPAGED),   //  MemoryType
    KSALLOCATOR_REQUIREMENTF_SYSTEM_MEMORY |
        KSALLOCATOR_REQUIREMENTF_PREFERENCES_ONLY,  //  Flags
    8,                                              //  Frames
    0,                                              //  Alignment
    PS_SAMPLE_SIZE,                                 //  MinFrameSize
    PS_SAMPLE_SIZE                                  //  MaxFrameSize
    );

//
// EncoderOutPinDataRanges:
//
// This is the list of data ranges supported on the capture output pin.
//
extern const 
PKSDATARANGE 
EncoderOutPinDataRanges [ENCODER_OUT_PIN_DATA_RANGE_COUNT] = {
    (PKSDATARANGE) &FormatEncoderOut
    };

//
// EncoderInPinDataRanges:
//
// This is the list of data ranges supported on the capture input pin.
//
extern const 
PKSDATARANGE 
EncoderInPinDataRanges [ENCODER_IN_PIN_DATA_RANGE_COUNT] = {
    (PKSDATARANGE) &FormatEncoderIn
    };



