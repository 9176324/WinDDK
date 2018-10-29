//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//
//==========================================================================;#
/**************************************************************************
    File    : encoder.cpp
    Abstract: Contains descriptors, dispatch routines, and property handlers 
              for the encoder filter
**************************************************************************/
#include "common.h"

// Pagable code
#ifdef ALLOC_PRAGMA
#pragma code_seg("PAGE")
#endif // ALLOC_PRAGMA

extern const KSFILTER_DISPATCH EncoderFilterDispatch;
extern const KSFILTER_DESCRIPTOR EncoderFilterDescriptor;
extern const KSPIN_DESCRIPTOR_EX EncoderFilterPinDescriptors [ENCODER_FILTER_PIN_COUNT];
extern const GUID EncoderFilterCategories [ENCODER_FILTER_CATEGORIES_COUNT];
extern const KSALLOCATOR_FRAMING_EX EncoderPinAllocatorFraming;
extern const KSPIN_DISPATCH EncoderPinDispatch;
extern const KSPIN_DISPATCH InputPinDispatch;
extern const PKSDATARANGE EncoderOutPinDataRanges [ENCODER_OUT_PIN_DATA_RANGE_COUNT];
extern const PKSDATARANGE EncoderInPinDataRanges [ENCODER_IN_PIN_DATA_RANGE_COUNT];

const KSPIN_MEDIUM PinMediumAudioIn =
{
    GUID_AnlgCapturePinMediumAudioOut, 0, 0
};

const KSPIN_MEDIUM PinMediumVideoIn =
{
    GUID_AnlgCapturePinMediumVideoOut, 0, 0
};

NTSTATUS
CEncoderFilter::
DispatchCreate (
    IN PKSFILTER Filter,
    IN PIRP Irp
    )

{
    PKSDEVICE   pKSDevice = NULL;
    CEncoderDevice *   pDevice = NULL;

    PAGED_CODE();
    NTSTATUS Status = STATUS_SUCCESS;

    if(!Filter)
    {
    return STATUS_UNSUCCESSFUL;
    }
    if(!Irp)
    {
    return STATUS_UNSUCCESSFUL;
    }
    

    PKSDEVICE pKsDevice = KsFilterGetDevice(Filter);
    if(!pKsDevice)
    {
    return Status;
    }
    BOOL pPinDirection[] = {FALSE, FALSE, TRUE};
    KSPIN_MEDIUM pMediumList[] = 
    {
    {
        GUID_AnlgCapturePinMediumVideoOut, 0, 0
    },
    {
        GUID_AnlgCapturePinMediumAudioOut, 0, 0
    },
    KSMEDIUM_TYPE_ANYINSTANCE
    
    };
    
    KsRegisterFilterWithNoKSPins(
        pKsDevice->PhysicalDeviceObject,
        &KSCATEGORY_ENCODER,
        3,
        pPinDirection,
        pMediumList,
        NULL
        );

    CEncoderFilter *EncFilter = new (NonPagedPool, MS_SAMPLE_ANALOG_POOL_TAG) CEncoderFilter (Filter);

    if (!EncFilter) {
        //
        // Return failure if we couldn't create the filter.
        //
        Status = STATUS_INSUFFICIENT_RESOURCES;

    } else {
        //
        // Add the item to the object bag if we we were successful. 
        // Whenever the filter closes, the bag is cleaned up and we will be
        // freed.
        //
        Status = KsAddItemToObjectBag (
            Filter -> Bag,
            reinterpret_cast <PVOID> (EncFilter),
            reinterpret_cast <PFNKSFREE> (CEncoderFilter::Cleanup)
            );

        if (!NT_SUCCESS (Status))
        {
            delete EncFilter;
            EncFilter = NULL;
        } else {
            Filter -> Context = reinterpret_cast <PVOID> (EncFilter);
        }

        pKSDevice = KsFilterGetDevice( Filter);
        ASSERT( pKSDevice);
        if (!pKSDevice)
        {
            Status = STATUS_DEVICE_NOT_CONNECTED;
            return Status;
        }

        //  Get the device object from the retrieved pointer to the KSDevice for this filter.
        //
        pDevice = reinterpret_cast<CEncoderDevice *>(pKSDevice->Context);
        ASSERT( pDevice);
        if (!pDevice)
        {
            Status = STATUS_DEVICE_NOT_CONNECTED;
            return Status;
        }

        //  Link the filter context to the device context.
        //  That is, set the filter's device pointer data member to the obtained device pointer.
        //
        if (EncFilter)
        {
            EncFilter->m_pDevice = pDevice;
        }
    }

	DbgPrint("EncoderFilter  create status %u", Status);
    return Status;

}


NTSTATUS
IntersectDataFormat(
    IN PVOID pContext,
    IN PIRP pIrp,
    IN PKSP_PIN Pin,
    IN PKSDATARANGE DataRange,
    IN PKSDATARANGE MatchingDataRange,
    IN ULONG DataBufferSize,
    OUT PVOID Data OPTIONAL,
    OUT PULONG DataSize
     )
{
    UNREFERENCED_PARAMETER(MatchingDataRange);
    UNREFERENCED_PARAMETER(Pin);
    UNREFERENCED_PARAMETER(pIrp);
    UNREFERENCED_PARAMETER(pContext);
    
    
    if ( DataBufferSize == 0)
    {
        *DataSize = sizeof( KS_DATARANGE_BDA_TRANSPORT );
		DbgPrint("EncoderFilter DataFormat OVERFLOW");
        return STATUS_BUFFER_OVERFLOW;
    }
    else if (DataRange -> FormatSize < sizeof (KS_DATARANGE_BDA_TRANSPORT)) 
    {
		DbgPrint("EncoderFilter DataFormat NO_MATCH");
    return STATUS_NO_MATCH;
    } else
    {
        ASSERT(DataBufferSize == sizeof(KS_DATARANGE_BDA_TRANSPORT));

        *DataSize = sizeof( KS_DATARANGE_BDA_TRANSPORT );
        RtlCopyMemory( Data, (PVOID)DataRange, sizeof(KS_DATARANGE_BDA_TRANSPORT));

		DbgPrint("EncoderFilter DataFormat scuccess");
        return STATUS_SUCCESS;
    }
}


static
NTSTATUS
VideoIntersectDataFormat(
    IN PVOID piKSFilter,                    //Pointer to KS filter structure.
    IN PIRP pIrp,                           //Pointer to data intersection
                                            //property request.
    IN PKSP_PIN pPinInstance,               //Pinter to structure indicating
                                            //pin in question.
    IN PKSDATARANGE pCallerDataRange,       //Pointer to the caller data
                                            //structure that should be
                                            //intersected.
    IN PKSDATARANGE pDescriptorDataRange,   //Pointer to the descriptor data

    IN DWORD dwBufferSize,                  //Size in bytes of the target
                                            //buffer structure. For size
                                            //queries, this value will be zero.
    OUT OPTIONAL PVOID pData,               //Pointer to the target data
                                            //structure representing the best
                                            //format in the intersection.
    OUT PDWORD pdwDataSize                  //Pointer to size of target data
                                            //format structure.
     )
{

    PKSFILTER pKSFilter = (PKSFILTER) piKSFilter;

    //invalid parameters?
    if( !pKSFilter || !pIrp || !pPinInstance ||
        !pCallerDataRange || !pDescriptorDataRange || !pdwDataSize )
    {
		DbgPrint("EncoderFilter VideoIn UNSUCCESSFUL 1");
        return STATUS_UNSUCCESSFUL;
    }

    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    //set output data size

    if( IsEqualGUID(pCallerDataRange->MajorFormat,
                              KSDATAFORMAT_TYPE_STREAM) &&
        IsEqualGUID(pCallerDataRange->SubFormat,
                              KSDATAFORMAT_TYPE_MPEG2_PROGRAM) )
    {        
        *pdwDataSize = sizeof(KSDATARANGE);
        
        if( pCallerDataRange->FormatSize != sizeof(KSDATARANGE) )
        {
			DbgPrint("EncoderFilter VideoIn NO_MATCH1 ");
            return STATUS_NO_MATCH;
        }
        //check for size only query
        if( 0 == dwBufferSize )
        {
			DbgPrint("EncoderFilter VideoIn OVERFLOW 1");
            return STATUS_BUFFER_OVERFLOW;
        }

        if( !pData )
        {
			DbgPrint("EncoderFilter VideoIn UNSUCCESSFUL 2");
            return STATUS_UNSUCCESSFUL;
        }

        //check if output buffer size is sufficient
        if( dwBufferSize < *pdwDataSize )
        {
			DbgPrint("EncoderFilter VideoIn TOO_SMALL 1");
            return STATUS_BUFFER_TOO_SMALL;
        }
                
        RtlCopyMemory(  pData,
                        pCallerDataRange,
                        sizeof(KSDATARANGE) );

		DbgPrint("EncoderFilter VideoIn SUCCESS 1");
        return STATUS_SUCCESS;
    }
    else if (IsEqualGUID(pDescriptorDataRange->Specifier,
            KSDATAFORMAT_SPECIFIER_VIDEOINFO)) {    

        //*** start intersection ***//

        //check if given datarange GUID matches to our datarange information
        if( pCallerDataRange->FormatSize != sizeof(KS_DATARANGE_VIDEO) )
        {
			DbgPrint("EncoderFilter VideoIn NO_MATCH 2");
            return STATUS_NO_MATCH;
        }

        //map given data ranges on video information structures
        //for some reason only a subpart of the KS_DATARANGE_VIDEO is used for intersection
        //this subpart is casted to KS_DATAFORMAT_VIDEOINFOHEADER and contains only
        //KSDATAFORMAT and KS_VIDEOINFOHEADER, see pTargetVideoDataRange
        PKS_DATARANGE_VIDEO pCallerVideoDataRange =
            reinterpret_cast <PKS_DATARANGE_VIDEO> (pCallerDataRange);
        PKS_DATARANGE_VIDEO pDescriptorVideoDataRange =
            reinterpret_cast <PKS_DATARANGE_VIDEO> (pDescriptorDataRange);
        PKS_DATAFORMAT_VIDEOINFOHEADER pTargetVideoDataRange =
            reinterpret_cast <PKS_DATAFORMAT_VIDEOINFOHEADER> (pData);

        //check if all other important fields match
        if( pCallerVideoDataRange->bFixedSizeSamples        !=
                pDescriptorVideoDataRange->bFixedSizeSamples        ||
            pCallerVideoDataRange->bTemporalCompression     !=
                pDescriptorVideoDataRange->bTemporalCompression     ||
            pCallerVideoDataRange->StreamDescriptionFlags   !=
                pDescriptorVideoDataRange->StreamDescriptionFlags   ||
            pCallerVideoDataRange->MemoryAllocationFlags    !=
                pDescriptorVideoDataRange->MemoryAllocationFlags    ||

           (RtlCompareMemory(&pCallerVideoDataRange->ConfigCaps,
                             &pDescriptorVideoDataRange->ConfigCaps,
                             sizeof(KS_VIDEO_STREAM_CONFIG_CAPS))) !=
                             sizeof(KS_VIDEO_STREAM_CONFIG_CAPS)    )
        {
			DbgPrint("EncoderFilter VideoIn NO_MATCH 3");
            return STATUS_NO_MATCH;
        }

    {
        ULONG VideoHeaderSize = KS_SIZE_VIDEOHEADER(
          &pCallerVideoDataRange->VideoInfoHeader
          );
        ULONG DataRangeSize = 
        FIELD_OFFSET(KS_DATARANGE_VIDEO, VideoInfoHeader) +
        VideoHeaderSize;
        if (
        VideoHeaderSize < pCallerVideoDataRange->
           VideoInfoHeader.bmiHeader.biSize ||
        DataRangeSize < VideoHeaderSize ||
        DataRangeSize > pCallerVideoDataRange->
           DataRange.FormatSize
        ) {
			DbgPrint("EncoderFilter VideoIn INVALID_PARAMETER 1");
        return STATUS_INVALID_PARAMETER;
        }
    }

    ULONG FormatSize;
    FormatSize = sizeof(KSDATAFORMAT) + 
        KS_SIZE_VIDEOHEADER (&pCallerVideoDataRange->VideoInfoHeader);

        *pdwDataSize = FormatSize;

        //check for size only query
        if( dwBufferSize == 0 )
        {
			DbgPrint("EncoderFilter VideoIn OVERFLOW 2");
            return STATUS_BUFFER_OVERFLOW;
        }


        //check if output buffer size is sufficient
        if( dwBufferSize < *pdwDataSize )
        {
			DbgPrint("EncoderFilter VideoIn TOO_SMALL 2");
            return STATUS_BUFFER_TOO_SMALL;
        }

        //copy the data range structure from our decriptor into the target format buffer,
        //for some reason two different names where given to the same structure
        //KSDATARANGE equals exactly KSDATAFORMAT
        pTargetVideoDataRange->DataFormat =
            static_cast <KSDATAFORMAT> (pDescriptorVideoDataRange->DataRange);

        //as mentioned above the target data range structure differs from the
        //caller and the descriptor structures, so the size is also different
        //and has to be set correctly
        pTargetVideoDataRange->DataFormat.FormatSize = *pdwDataSize;

        //copy the video info header structure from the caller into the target
        //buffer,we have to check at this time whether the requested caller
        //video format fits our capabilities (not implemented right now)
    RtlCopyMemory(
        &pTargetVideoDataRange->VideoInfoHeader,
        &pCallerVideoDataRange->VideoInfoHeader,
        KS_SIZE_VIDEOHEADER (&pCallerVideoDataRange->VideoInfoHeader));

        //If there is a format change (e.g. a new resolution) the size is not updated
        //automatically, so we have to calculate it here. There is a macro that multiplies
        //the width and height and that also aligns the size to DWORDs
        pTargetVideoDataRange->VideoInfoHeader.bmiHeader.biSizeImage =
            pTargetVideoDataRange->DataFormat.SampleSize =
            KS_DIBSIZE(pTargetVideoDataRange->VideoInfoHeader.bmiHeader);

		DbgPrint("EncoderFilter VideoIn SUCCESS 2");
        Status = STATUS_SUCCESS;
    } else if (IsEqualGUID(pDescriptorDataRange->Specifier,
         KSDATAFORMAT_SPECIFIER_VIDEOINFO2)) {

        //*** start intersection ***//

        //check if given datarange GUID matches to our datarange information
        if( pCallerDataRange->FormatSize != sizeof(KS_DATARANGE_VIDEO2) )
        {
			DbgPrint("EncoderFilter VideoIn NO_MATCH 4");
            return STATUS_NO_MATCH;
        }

        //map given data ranges on video information structures
        //for some reason only a subpart of the KS_DATARANGE_VIDEO is
        //used for intersection this subpart is casted to
        //KS_DATAFORMAT_VIDEOINFOHEADER and contains only
        //KSDATAFORMAT and KS_VIDEOINFOHEADER, see pTargetVideoDataRange
        PKS_DATARANGE_VIDEO2 pCallerVideoDataRange =
            reinterpret_cast <PKS_DATARANGE_VIDEO2> (pCallerDataRange);
        PKS_DATARANGE_VIDEO2 pDescriptorVideoDataRange =
            reinterpret_cast <PKS_DATARANGE_VIDEO2> (pDescriptorDataRange);
        PKS_DATAFORMAT_VIDEOINFOHEADER2 pTargetVideoDataRange =
            reinterpret_cast <PKS_DATAFORMAT_VIDEOINFOHEADER2> (pData);

        //check if all other important fields match
        if( pCallerVideoDataRange->bFixedSizeSamples        !=
                pDescriptorVideoDataRange->bFixedSizeSamples        ||
            pCallerVideoDataRange->bTemporalCompression     !=
                pDescriptorVideoDataRange->bTemporalCompression     ||
            pCallerVideoDataRange->StreamDescriptionFlags   !=
                pDescriptorVideoDataRange->StreamDescriptionFlags   ||
            pCallerVideoDataRange->MemoryAllocationFlags    !=
                pDescriptorVideoDataRange->MemoryAllocationFlags    ||

           (RtlCompareMemory(&pCallerVideoDataRange->ConfigCaps,
                             &pDescriptorVideoDataRange->ConfigCaps,
                             sizeof(KS_VIDEO_STREAM_CONFIG_CAPS))) !=
                             sizeof(KS_VIDEO_STREAM_CONFIG_CAPS)    )
        {
			DbgPrint("EncoderFilter VideoIn NO_MATCH 5");
            return STATUS_NO_MATCH;
        }

    {
        ULONG VideoHeaderSize = KS_SIZE_VIDEOHEADER(
          &pCallerVideoDataRange->VideoInfoHeader
          );
        ULONG DataRangeSize = 
        FIELD_OFFSET(KS_DATARANGE_VIDEO, VideoInfoHeader) +
        VideoHeaderSize;
        if (
        VideoHeaderSize < pCallerVideoDataRange->
           VideoInfoHeader.bmiHeader.biSize ||
        DataRangeSize < VideoHeaderSize ||
        DataRangeSize > pCallerVideoDataRange->
           DataRange.FormatSize
        ) {
			DbgPrint("EncoderFilter VideoIn INVALID_PARAMETER 2");
        return STATUS_INVALID_PARAMETER;
        }
    }

    ULONG FormatSize;
    FormatSize = sizeof(KSDATAFORMAT) + 
        KS_SIZE_VIDEOHEADER (&pCallerVideoDataRange->VideoInfoHeader);


        *pdwDataSize = FormatSize;

        //check for size only query
        if( dwBufferSize == 0 )
        {
			DbgPrint("EncoderFilter VideoIn OVERFLOW 3");
            return STATUS_BUFFER_OVERFLOW;
        }

        //check if output buffer size is sufficient
        if( dwBufferSize < *pdwDataSize )
        {
			DbgPrint("EncoderFilter VideoIn TOO_SMALL 3");
            return STATUS_BUFFER_TOO_SMALL;
        }

        //copy the data range structure from our decriptor into the target
        //format buffer, for some reason two different names where given
        //to the same structure KSDATARANGE equals exactly KSDATAFORMAT
        pTargetVideoDataRange->DataFormat =
            static_cast <KSDATAFORMAT> (pDescriptorVideoDataRange->DataRange);
        //as mentioned above the target data range structure differs from the
        //caller and the descriptor structures, so the size is also different
        //and has to be set correctly
        pTargetVideoDataRange->DataFormat.FormatSize = *pdwDataSize;

        //copy the video info header structure from the caller into the target
        //buffer, we have to check at this time whether the requested caller
        //video formatfits our capabilities (not implemented right now)
    RtlCopyMemory(
        &pTargetVideoDataRange->VideoInfoHeader2,
        &pCallerVideoDataRange->VideoInfoHeader,
        KS_SIZE_VIDEOHEADER (&pCallerVideoDataRange->VideoInfoHeader));

        //If there is a format change (e.g. a new resolution) the size is
        //not updated automatically, so we have to calculate it here.
        //There is a macro that multiplies the width and height and that
        //also aligns the size to DWORDs
        pTargetVideoDataRange->VideoInfoHeader2.bmiHeader.biSizeImage =
            pTargetVideoDataRange->DataFormat.SampleSize =
            KS_DIBSIZE(pTargetVideoDataRange->VideoInfoHeader2.bmiHeader);

		DbgPrint("EncoderFilter VideoIn SUCCESS 3");
        Status = STATUS_SUCCESS;
    } else {
    *pdwDataSize = 0;
	DbgPrint("EncoderFilter VideoIn NO_MATCH 6");
    Status = STATUS_NO_MATCH;
    }

    return Status;
}

#define D_X 320 
#define D_Y 240
#define ABS(x) ((x) < 0 ? (-(x)) : (x))

#ifndef mmioFOURCC    
#define mmioFOURCC( ch0, ch1, ch2, ch3 )                \
        ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
        ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#endif
#define FOURCC_YUV422       mmioFOURCC('U', 'Y', 'V', 'Y')

const 
KS_DATARANGE_VIDEO 
EncoderVideoInPinRange = {

    //
    // KSDATARANGE
    //
    {   
        sizeof (KS_DATARANGE_VIDEO),            // FormatSize
        0,                                      // Flags
        D_X * D_Y * 2,                          // SampleSize
        0,                                      // Reserved
        STATICGUIDOF (KSDATAFORMAT_TYPE_VIDEO), // aka. MEDIATYPE_Video
        0x59565955, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 
            0x00, 0x38, 0x9b, 0x71,             // aka. MEDIASUBTYPE_UYVY,
        STATICGUIDOF (KSDATAFORMAT_SPECIFIER_VIDEOINFO) // aka. FORMAT_VideoInfo
    },

    TRUE,               // BOOL,  bFixedSizeSamples (all samples same size?)
    TRUE,               // BOOL,  bTemporalCompression (all I frames?)
    0,                  // Reserved (was StreamDescriptionFlags)
    0,                  // Reserved (was MemoryAllocationFlags   
                        //           (KS_VIDEO_ALLOC_*))
    //
    // _KS_VIDEO_STREAM_CONFIG_CAPS  
    //
    {
        STATICGUIDOF( KSDATAFORMAT_SPECIFIER_VIDEOINFO ), // GUID
	KS_AnalogVideo_NTSC_M |KS_AnalogVideo_PAL_B | KS_AnalogVideo_PAL_D | KS_AnalogVideo_PAL_G | KS_AnalogVideo_PAL_H | KS_AnalogVideo_PAL_I | KS_AnalogVideo_PAL_M | KS_AnalogVideo_PAL_N ,                    // AnalogVideoStandard
        720,480,        // InputSize, (the inherent size of the incoming signal
                    //             with every digitized pixel unique)
        160,120,        // MinCroppingSize, smallest rcSrc cropping rect allowed
        720,480,        // MaxCroppingSize, largest  rcSrc cropping rect allowed
        8,              // CropGranularityX, granularity of cropping size
        1,              // CropGranularityY
        8,              // CropAlignX, alignment of cropping rect 
        1,              // CropAlignY;
        160, 120,       // MinOutputSize, smallest bitmap stream can produce
        720, 480,       // MaxOutputSize, largest  bitmap stream can produce
        8,              // OutputGranularityX, granularity of output bitmap size
        1,              // OutputGranularityY;
        0,              // StretchTapsX  (0 no stretch, 1 pix dup, 2 interp...)
        0,              // StretchTapsY
        0,              // ShrinkTapsX 
        0,              // ShrinkTapsY 
        333667,         // MinFrameInterval, 100 nS units
        640000000,      // MaxFrameInterval, 100 nS units
        8 * 2 * 30 * 160 * 120,  // MinBitsPerSecond;
        8 * 2 * 30 * 720 * 480   // MaxBitsPerSecond;
    }, 

    //
    // KS_VIDEOINFOHEADER (default format)
    //
    {
        0,0,0,0,                            // RECT  rcSource; 
        0,0,0,0,                            // RECT  rcTarget; 
        D_X * D_Y * 2 * 30,                 // DWORD dwBitRate;
        0L,                                 // DWORD dwBitErrorRate; 
        333667,                             // REFERENCE_TIME  AvgTimePerFrame;   
        sizeof (KS_BITMAPINFOHEADER),       // DWORD biSize;
        D_X,                                // LONG  biWidth;
        D_Y,                                // LONG  biHeight;
        1,                                  // WORD  biPlanes;
        16,                                 // WORD  biBitCount;
        FOURCC_YUV422,                      // DWORD biCompression;
        D_X * D_Y * 2,                      // DWORD biSizeImage;
        0,                                  // LONG  biXPelsPerMeter;
        0,                                  // LONG  biYPelsPerMeter;
        0,                                  // DWORD biClrUsed;
        0                                   // DWORD biClrImportant;
    }
}; 

static PKSDATAFORMAT EncoderVideoInPinRanges[] =
{
    (PKSDATAFORMAT) &EncoderVideoInPinRange,

    // Add more formats here if additional transport formats are supported.
    //
};


const KSDATARANGE_AUDIO EncoderAudioInPinRange = 
{    
    {
    sizeof(KSDATARANGE_AUDIO),
    0,
    0,
    0,
    STATICGUIDOF(KSDATAFORMAT_TYPE_AUDIO),
    STATICGUIDOF(KSDATAFORMAT_SUBTYPE_PCM),
    STATICGUIDOF(KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
    },
    2,      // maximum channel
    16,      // min bits per sample
    16,      // max bits per sample
    32000,   // min frequency
    32000    // max frequency
};


static PKSDATAFORMAT EncoderAudioInPinRanges[] =
{
    (PKSDATAFORMAT) &EncoderAudioInPinRange,

    // Add more formats here if additional transport formats are supported.
    //
};


static
NTSTATUS
AudioIntersectDataFormat (
    IN PVOID pContext,
    IN PIRP Irp,
    IN PKSP_PIN PinInstance,
    IN PKSDATARANGE CallerDataRange,
    IN PKSDATARANGE DescriptorDataRange,
    IN ULONG BufferSize,
    OUT PVOID Data OPTIONAL,
    OUT PULONG DataSize
    )

/*++

Routine Description:

    The intersect handler for the audio capture pin.  This is really quite
    simple because the audio pin only exposes the number of channels,
    sampling frequency, etc...  that the wave file it is synthesizing from
    contains.

Arguments:

    Filter -
        Contains a void pointer to the  filter structure.

    Irp -
        Contains a pointer to the data intersection property request.

    PinInstance -
        Contains a pointer to a structure indicating the pin in question.

    CallerDataRange -
        Contains a pointer to one of the data ranges supplied by the client
        in the data intersection request.  The format type, subtype and
        specifier are compatible with the DescriptorDataRange.

    DescriptorDataRange -
        Contains a pointer to one of the data ranges from the pin descriptor
        for the pin in question.  The format type, subtype and specifier are
        compatible with the CallerDataRange.

    BufferSize -
        Contains the size in bytes of the buffer pointed to by the Data
        argument.  For size queries, this value will be zero.

    Data -
        Optionally contains a pointer to the buffer to contain the data 
        format structure representing the best format in the intersection 
        of the two data ranges.  For size queries, this pointer will be 
        NULL.

    DataSize -
        Contains a pointer to the location at which to deposit the size 
        of the data format.  This information is supplied by the function 
        when the format is actually delivered and in response to size 
        queries.

Return Value:

    STATUS_SUCCESS if there is an intersection and it fits in the supplied
    buffer, STATUS_BUFFER_OVERFLOW for successful size queries, 
    STATUS_NO_MATCH if the intersection is empty, or 
    STATUS_BUFFER_TOO_SMALL if the supplied buffer is too small.

--*/


{
    
    PAGED_CODE();

    UNREFERENCED_PARAMETER(Irp);
    UNREFERENCED_PARAMETER(pContext);
    UNREFERENCED_PARAMETER(PinInstance);
    

    //
    // Because the only range we expose is such that it will match
    // KSDATARANGE_AUDIO, it is safe to interpret the data structures as
    // KSDATARANGE_AUDIO.  This is due to the fact that AVStream will have
    // prematched the GUIDs for us.
    //
    PKSDATARANGE_AUDIO CallerAudioRange =
        reinterpret_cast <PKSDATARANGE_AUDIO> (CallerDataRange);

    PKSDATARANGE_AUDIO DescriptorAudioRange =
        reinterpret_cast <PKSDATARANGE_AUDIO> (DescriptorDataRange);

    //
    // We are returning a KSDATAFORMAT_WAVEFORMATEX.  Specify such if a size
    // query happens.
    //
    if (BufferSize == 0) {
        *DataSize = sizeof (KSDATAFORMAT_WAVEFORMATEX);
		DbgPrint("EncoderFilter Audio OVERFLOW");
        return STATUS_BUFFER_OVERFLOW;
    }

    if (BufferSize < sizeof (KSDATAFORMAT_WAVEFORMATEX)) {
		DbgPrint("EncoderFilter Audio TOO_SMALL");
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Match the blocks.  We only support one format (not really a range), so
    // this intersection aught to be really simple.  It's more of a check
    // if the format we are going to use intersects somewhere in 
    // CallerAudioRange.
    //
    if (DescriptorAudioRange -> MaximumChannels > 
            CallerAudioRange -> MaximumChannels ||
        DescriptorAudioRange -> MinimumBitsPerSample <
            CallerAudioRange -> MinimumBitsPerSample ||
        DescriptorAudioRange -> MinimumBitsPerSample >
            CallerAudioRange -> MaximumBitsPerSample ||
        DescriptorAudioRange -> MinimumSampleFrequency <
            CallerAudioRange -> MinimumSampleFrequency ||
        DescriptorAudioRange -> MinimumSampleFrequency >
            CallerAudioRange -> MaximumSampleFrequency) {

        //
        // If the descriptor's "range" (more of a single format specified
        // in a range) doesn't intersect the caller's, no match the call.
        //
        *DataSize = sizeof (KSDATAFORMAT_WAVEFORMATEX);
		DbgPrint("EncoderFilter Audio NO_MATCH");
        return STATUS_NO_MATCH;

    }

    //
    // Build the format.
    //
    PKSDATAFORMAT_WAVEFORMATEX WaveFormat =
        reinterpret_cast <PKSDATAFORMAT_WAVEFORMATEX> (Data);

    RtlCopyMemory (
        &WaveFormat -> DataFormat,
        &DescriptorAudioRange -> DataRange,
        sizeof (KSDATAFORMAT)
        );

    WaveFormat -> WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
    WaveFormat -> WaveFormatEx.nChannels = 
        (WORD)DescriptorAudioRange -> MaximumChannels;
    WaveFormat -> WaveFormatEx.nSamplesPerSec =
        DescriptorAudioRange -> MaximumSampleFrequency;
    WaveFormat -> WaveFormatEx.wBitsPerSample =
        (WORD)DescriptorAudioRange -> MaximumBitsPerSample;
    WaveFormat -> WaveFormatEx.nBlockAlign =
        (WaveFormat -> WaveFormatEx.wBitsPerSample / 8) *
        WaveFormat -> WaveFormatEx.nChannels;
    WaveFormat -> WaveFormatEx.nAvgBytesPerSec =
        WaveFormat -> WaveFormatEx.nBlockAlign *
        WaveFormat -> WaveFormatEx.nSamplesPerSec;
    WaveFormat -> WaveFormatEx.cbSize = 0;
    WaveFormat -> DataFormat.SampleSize = 
        WaveFormat -> WaveFormatEx.nBlockAlign;

    *DataSize = sizeof (KSDATAFORMAT_WAVEFORMATEX);
	WaveFormat->DataFormat.FormatSize = *DataSize;

	DbgPrint("EncoderFilter Audio success");
	
    return STATUS_SUCCESS;

}

//
// EncoderFilterPinDescriptors:
//
// The list of pin descriptors on the capture filter.  
//
const 
KSPIN_DESCRIPTOR_EX
EncoderFilterPinDescriptors [3] = {
    //Video input pin
    {
        NULL,                                           // Dispatch
        NULL,                                           // Automation
        {
            NULL,                                       // Interfaces Count
            0,                                          // Interfaces
            1,                                          // Mediums Count
            &PinMediumVideoIn,                          // Mediums
            SIZEOF_ARRAY (EncoderVideoInPinRanges),     // Data Range Count
            EncoderVideoInPinRanges,                    // Data Ranges
            KSPIN_DATAFLOW_IN,                          // Data Flow
            KSPIN_COMMUNICATION_BOTH,                   // Communication
            NULL,                                       // Category
            (GUID *) &PINNAME_YUVIN,                    // Name
            0                                           // Constrained Data Range
        },
        
        KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT |
        KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING | 
        KSPIN_FLAG_FIXED_FORMAT,                        // Flags
        1,                                              // Instances possible
        1,                                              // Instances necessary
        NULL,                                           // Allocator Framing
        VideoIntersectDataFormat                        // Intersect Handler
    },
    //Audio input pin
    {
        NULL,                                           // Dispatch
        NULL,                                           // Automation
        {
            0,                                          // Interfaces Count
            NULL,                                       // Interfaces
            1,                                          // Mediums Count
            &PinMediumAudioIn,                          // Mediums
            SIZEOF_ARRAY (EncoderAudioInPinRanges),     // Data Range Count
            EncoderAudioInPinRanges,                    // Data Ranges
            KSPIN_DATAFLOW_IN,                          // Data Flow
            KSPIN_COMMUNICATION_BOTH,                   // Communication
            &PINNAME_VIDEO_CAPTURE,                     // Category
            (GUID *) &PINNAME_AUDIOPCMIN,               // Name
            0                                           // Constrained Data Range
        },
        
        KSPIN_FLAG_DO_NOT_USE_STANDARD_TRANSPORT | 
        KSPIN_FLAG_FRAMES_NOT_REQUIRED_FOR_PROCESSING | 
        KSPIN_FLAG_FIXED_FORMAT,                        // Flags
        1,                                              // Instances possible
        0,                                              // Instances necessary
        NULL,                                           // Allocator Framing
        AudioIntersectDataFormat                        // Intersect Handler
    },
    // Program stream output pin
    {
        &EncoderPinDispatch,                            // Dispatch
        NULL,                                           // Automation
        {                                               // Pin Descriptor
            0,                                          // Interfaces Count
            NULL,                                       // Interfaces
            0,                                          // Mediums Count
            NULL,                                       // Mediums
            SIZEOF_ARRAY (EncoderOutPinDataRanges),     // Data Range Count
            EncoderOutPinDataRanges,                    // Data Ranges
            KSPIN_DATAFLOW_OUT,                         // Data Flow
            KSPIN_COMMUNICATION_BOTH,                   // Communication
            &PINNAME_BDA_TRANSPORT,                     // Category
            &PINNAME_BDA_TRANSPORT,                     // Name
            0                                           // Constrained Data Range
        },
#if !defined(_BUILD_SW_TUNER_ON_X64)        
        KSPIN_FLAG_GENERATE_MAPPINGS |     
#endif
        KSPIN_FLAG_PROCESS_IN_RUN_STATE_ONLY,           // Flags
        1,                                              // Instances Possible
        1,                                              // Instances Necessary
        &EncoderPinAllocatorFraming,                    // Allocator Framing
        VideoIntersectDataFormat                        // Intersect Handler
    }
};

//
// EncoderFilterDispatch:
//
// This is the dispatch table for the encoder filter.  It provides notification
// of creation, closure, processing (for filter-centrics, not for the encoder
// filter), and resets (for filter-centrics, not for the encoder filter).
//
const 
KSFILTER_DISPATCH
EncoderFilterDispatch = {
    CEncoderFilter::DispatchCreate,         // Filter Create
    NULL,                                   // Filter Close
    NULL,                                   // Filter Process
    NULL                                    // Filter Reset
};


// Create a connection through the capture filter so that graph render will
// work.
//
const
KSTOPOLOGY_CONNECTION FilterConnections[] =
{   // KSFILTER_NODE  is defined as ((ULONG)-1) in ks.h 
    { KSFILTER_NODE, 0,                 KSFILTER_NODE, 2 }
};

NTSTATUS EncoderGenericHandler
(
    IN PIRP Irp,
    IN PKSPROPERTY_VIDEOENCODER_S pRequest,
    IN PKSPROPERTY_VIDEOENCODER_S pData
)
{ 
	UNREFERENCED_PARAMETER(Irp);
	UNREFERENCED_PARAMETER(pRequest);
	UNREFERENCED_PARAMETER(pData);

      return STATUS_SUCCESS;
}


DEFINE_KSPROPERTY_TABLE(EncoderPropertyTable)
{
    DEFINE_KSPROPERTY_ITEM                              
    (
        KSPROPERTY_VIDEOENCODER_CAPS,                   // Property
        EncoderGenericHandler,                          // Get Handler
        sizeof(KSPROPERTY_VIDEOENCODER_S),              // Min Property
        sizeof(KSPROPERTY_VIDEOENCODER_S),              // Min Data
        NULL,                                           // Set Handler
        NULL,                                           // Values
        0,                                              // Relations Count
        NULL,                                           // Relations
        NULL,                                           // Support Handler
        0                                               // Serialized Size
    ),
    DEFINE_KSPROPERTY_ITEM                              
    (
        KSPROPERTY_VIDEOENCODER_STANDARD,               // Property
        EncoderGenericHandler,                          // Get Handler
        sizeof(KSPROPERTY_VIDEOENCODER_S),              // Min Property
        sizeof(KSPROPERTY_VIDEOENCODER_S),              // Min Data
        EncoderGenericHandler,                          // Set Handler
        NULL,                                           // Values
        0,                                              // Relations Count
        NULL,                                           // Relations
        NULL,                                           // Support Handler
        0                                               // Serialized Size
    ),
    DEFINE_KSPROPERTY_ITEM                              
    (
        KSPROPERTY_VIDEOENCODER_COPYPROTECTION,         // Property
        EncoderGenericHandler,                          // Get Handler
        sizeof(KSPROPERTY_VIDEOENCODER_S),              // Min Property
        sizeof(KSPROPERTY_VIDEOENCODER_S),              // Min Data
        EncoderGenericHandler,                          // Set Handler
        NULL,                                           // Values
        0,                                              // Relations Count
        NULL,                                           // Relations
        NULL,                                           // Support Handler
        0                                               // Serialized Size
    ),
    DEFINE_KSPROPERTY_ITEM                              
    (
        KSPROPERTY_VIDEOENCODER_CC_ENABLE,              // Property
        EncoderGenericHandler,                          // Get Handler
        sizeof(KSPROPERTY_VIDEOENCODER_S),              // Min Property
        sizeof(KSPROPERTY_VIDEOENCODER_S),              // Min Data
        EncoderGenericHandler,                          // Set Handler
        NULL,                                           // Valuse
        0,                                              // Relations Count
        NULL,                                           // Relations
        NULL,                                           // Support Handler
        0                                               // Serialized Size
    )

};

DEFINE_KSPROPERTY_SET_TABLE(EncoderFilterPropertySetTable)
{
    DEFINE_KSPROPERTY_SET
    (
        &PROPSETID_VIDCAP_VIDEOENCODER,                   // Set
        SIZEOF_ARRAY(EncoderPropertyTable),               // PropertiesCount
        EncoderPropertyTable,                             // PropertyItems
        0,                                                // FastIoCount
        NULL                                              // FastIoTable
    )
};

DEFINE_KSAUTOMATION_TABLE(EncoderFilterAutomation) 
{
    DEFINE_KSAUTOMATION_PROPERTIES(EncoderFilterPropertySetTable),
    DEFINE_KSAUTOMATION_METHODS_NULL,
    DEFINE_KSAUTOMATION_EVENTS_NULL
};

const
GUID
EncoderFilterCategories [2] = {
    STATICGUIDOF( KSCATEGORY_ENCODER),
    STATICGUIDOF (KSCATEGORY_MULTIPLEXER)
};

extern const 
KSFILTER_DESCRIPTOR 
BdaEncoderFilterDescriptor = {
    &EncoderFilterDispatch,                             // Dispatch Table
    &EncoderFilterAutomation,                           // Automation Table
    KSFILTER_DESCRIPTOR_VERSION,                        // Version
    0,                                                  // Flags
    &KSNAME_BdaEncoderFilter,                           // Reference GUID
    DEFINE_KSFILTER_PIN_DESCRIPTORS (EncoderFilterPinDescriptors),// Pin Descriptors
    DEFINE_KSFILTER_CATEGORIES (EncoderFilterCategories),// Filter Categories
    0,                                                  // Node Descriptor Count (there is no macro)
    sizeof (KSNODE_DESCRIPTOR),                         // Node Descriptor Size
    NULL,                                               // Node Descriptor
    DEFINE_KSFILTER_CONNECTIONS(FilterConnections),     // Topology
    NULL                                                // Component Id
};



