//
// SwapAPOLFX.cpp -- Copyright (c) Microsoft Corporation. All rights reserved.
//
// Description:
//
//  Implementation of SwapSamples
//
#include <atlbase.h>
#include <atlcom.h>
#include <atlcoll.h>
#include <atlsync.h>
#include <mmreg.h>

#include <audioenginebaseapo.h>
#include <baseaudioprocessingobject.h>
#include <resource.h>

#include <float.h>

#include "SwapAPO.h"

#pragma AVRT_CODE_BEGIN
void ProcessSwap(
    FLOAT32 *pf32OutputFrames,
    FLOAT32 *pf32InputFrames,
    UINT32   u32ValidFrameCount,
    UINT32   u32SamplesPerFrame )
{
    UINT32   u32SampleIndex;
    FLOAT32  fSwap32;

    ASSERT_REALTIME();
    ATLASSERT( AE_IS_VALID_TYPED_READ_POINTER(pf32InputFrames) );
    ATLASSERT( AE_IS_VALID_TYPED_WRITE_POINTER(pf32OutputFrames) );

    // loop through samples
    while (u32ValidFrameCount--)
    {
        for (u32SampleIndex=0; u32SampleIndex+1<u32SamplesPerFrame; u32SampleIndex += 2)
        {
            // apply swap
            fSwap32 = *pf32InputFrames;
            *pf32OutputFrames = *(pf32InputFrames + 1);
            pf32OutputFrames++;
            *pf32OutputFrames = fSwap32;
            pf32OutputFrames++;
            pf32InputFrames += 2;
        }
    }
}
#pragma AVRT_CODE_END


#pragma AVRT_CODE_BEGIN
void ProcessSwapScale(
    FLOAT32 *pf32OutputFrames,
    FLOAT32 *pf32InputFrames,
    UINT32   u32ValidFrameCount,
    UINT32   u32SamplesPerFrame,
    FLOAT32  *pf32Coefficients )
{
    UINT32   u32SampleIndex;
    FLOAT32  fSwap32;

    ASSERT_REALTIME();
    ATLASSERT( AE_IS_VALID_TYPED_READ_POINTER(pf32InputFrames) );
    ATLASSERT( AE_IS_VALID_TYPED_WRITE_POINTER(pf32OutputFrames) );

    // loop through samples
    while (u32ValidFrameCount--)
    {
        for (u32SampleIndex=0; u32SampleIndex+1<u32SamplesPerFrame; u32SampleIndex += 2)
        {
            // apply swap for each stereo pair and scale

            // save left channel
            fSwap32 = *pf32InputFrames;
            // left output equals right input times 1st coefficient
            *pf32OutputFrames = *(pf32InputFrames + 1) * pf32Coefficients[u32SampleIndex];
            pf32OutputFrames++;
            
            // right output equals left input times 2nd coefficient
            *pf32OutputFrames = fSwap32  * pf32Coefficients[u32SampleIndex+1];      
            pf32OutputFrames++;

            pf32InputFrames += 2;
        }
    }
}
#pragma AVRT_CODE_END



