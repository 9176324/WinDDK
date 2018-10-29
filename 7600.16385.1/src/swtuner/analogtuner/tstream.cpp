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

/***************************************************************************
File    : tstream.cpp
Abstract: This class synthesizes a transport stream for output from the
encoder filter.
****************************************************************************/
#include "common.h"
extern UNICODE_STRING g_RegistryPath;
extern ULONG g_FileCompleteStatus;


#ifdef ALLOC_PRAGMA
#pragma code_seg()
#endif // ALLOC_PRAGMA

CTsSynthesizer::
CTsSynthesizer (
                PKSDEVICE Device,
                ULONG PacketSize,
                ULONG PacketsPerBuffer,
                ULONG Frequency,
                WCHAR* pczDeviceInstanceID
                ) :
m_PacketSize (PacketSize),
m_PacketsPerBuffer (PacketsPerBuffer),
m_SynthesisBuffer (NULL),
m_fFreqChanged(FALSE),
m_fFileReaderFreqChanged(FALSE),
m_llLastPts(0),
m_llInitialPts(0),
m_lNewFrequencyChanging(0),
m_fileReader(Device, pczDeviceInstanceID, &g_RegistryPath)
{
    m_liInitialPerformanceCounter = KeQueryPerformanceCounter(&m_liPerformanceFrequency);
    m_ulFrequency = Frequency;
    m_pczDeviceInstanceID = pczDeviceInstanceID;
    KeInitializeSpinLock (&m_FreqSpinLock);
}


void
CTsSynthesizer::
SynthesizeTS (
              )
{
    // Copy the synthesized transport stream to the synthesis buffer
}


ULONG
CTsSynthesizer::
GetFrequency()
{
    return m_ulFrequency;
}

void
CTsSynthesizer::
SetFrequency(ULONG ulFrequency)
{
    DbgPrint("CTsSynthesizer::SetFrequency %u", ulFrequency);

    KIRQL oldIrql;
    KeAcquireSpinLock (&m_FreqSpinLock, &oldIrql);
    m_ulFrequency = ulFrequency;
    KeReleaseSpinLock (&m_FreqSpinLock, oldIrql);
    if (m_fileReader.GetFrequency() != ulFrequency)
    {
        m_fileReader.SetFrequency(ulFrequency);
        //InterlockedExchange(&m_lNewFrequencyChanging,1);
        if (m_fileReader.IsReadStarted())
        {
            m_lNewFrequencyChanging = 1;
            BOOL fFileReaderFreqChanged;
            KeAcquireSpinLock (&m_FreqSpinLock, &oldIrql);
            m_fFileReaderFreqChanged = FALSE;
            fFileReaderFreqChanged = FALSE;
            KeReleaseSpinLock (&m_FreqSpinLock, oldIrql);

            WORD wCount = 0;

            // Wait until the file reader is done with the frequency change
            while (fFileReaderFreqChanged == FALSE && wCount < 4000)//timeout to 1.2s (300us*4000)
            {
                LARGE_INTEGER llWaitTime;
                llWaitTime.QuadPart = -3000;
                KeWaitForSingleObject(m_fileReader.GetFreqChangedEvent(),
                    Executive,
                    KernelMode,
                    FALSE,
                    &llWaitTime);
                KeAcquireSpinLock (&m_FreqSpinLock, &oldIrql);
                fFileReaderFreqChanged = m_fFileReaderFreqChanged;
                KeReleaseSpinLock (&m_FreqSpinLock, oldIrql);
                if (fFileReaderFreqChanged == FALSE)
                {
                    wCount ++;
                }
                else
                {
                    break; //we timeout, because the file reader thread is not reacting or a frequency change is not really happen
                }
                //DbgPrint("CTsSynthesizer::SetFrequency fFileReaderFreqChanged%d,count is %d\n",(int)fFileReaderFreqChanged,(int)wCount);
            }

            m_lNewFrequencyChanging = 0;
            DbgPrint("CTsSynthesizer::SetFrequency end!%d\n",(int)fFileReaderFreqChanged);
        }
        //InterlockedExchange(&m_lNewFrequencyChanging,0);
    }
}

bool 
CTsSynthesizer
::NextMpegStartCode(
                    BYTE &bCode,
                    const char * &buffer,
                    ULONG& iLen
                    )
{
    if(!iLen || !buffer)
        return false;
    
    if (iLen == 0)
    {
        return false;
    }
    
    DWORD dwCode = 0xFFFFFFFF;
    ULONG iScaned = 0;
    do{
       dwCode |= *buffer;
       dwCode <<= 8;
       buffer++;
       iScaned ++;
    }
    while(((dwCode & 0xFFFFFF00) != 0x00000100) && (iScaned < iLen));

    iLen -= iScaned;
    if(((dwCode & 0xFFFFFF00) == 0x00000100) && (iLen > 0))
    {
        dwCode |= *buffer;
        buffer++;
        iScaned ++;
        iLen --;

        bCode = (BYTE)(dwCode & 0x000000FF);
        return true;
    }
    return false;
}

bool 
CTsSynthesizer
::GetCurTimestamp(
                  const char * buffer,
                  ULONG iLen,
                  LONGLONG &llCurPts
                  )
{
   BYTE bCode = 0;

   while(NextMpegStartCode(bCode,buffer,iLen))
   {
       if(bCode == 0xba)//mpeg2 system header
       {
           if(iLen < 9)
           {
               DbgPrint("reach buffer end when finding the scr\n");
            return false;
           }
           LONGLONG llScr = 0;
           short iScrExtension = 0;
           INT iBitrate = 0;
           BYTE b = *buffer++;

           llScr |= (b &0x38);
           llScr = llScr << 3;         

           llScr |= (b & 0x03);
           llScr = llScr << 2;

           b = *buffer++;
                 llScr |= b;
           llScr = llScr << 8;

           b= *buffer++;
           llScr |= (b & 0xF8);
           llScr = llScr << 5;

           llScr |= (b & 0x3);
           llScr = llScr<<2;

           b = *buffer++;
           llScr |= b;
           llScr = llScr << 8;

           llScr >>= 3;

           b = *buffer++;
           llScr |= (b & 0xF8);

           iScrExtension |= (b&0x03);
           iScrExtension = iScrExtension << 2;

           b = *buffer++;
           iScrExtension |= (b&0xFE);

           llScr = llScr*300+iScrExtension;//27MHZ

           b = *buffer++;
           iBitrate |= b;
           iBitrate = iBitrate << 8;

           b = *buffer++;
           iBitrate |= b;
           iBitrate = iBitrate << 8;

           iBitrate >>= 2;

           b = *buffer;
           iBitrate |= (b&0xFC);
           
           llScr /= (27*1000*1000i64);
           iLen -= 9;
           //DbgPrint("scr value is %I64d,bitrate is %d\n",llScr,iBitrate);
           //not use SCR values because  some PS file have only one SCR value
           continue;
       }
       else
       {
            if(!((bCode >= 0xbd && bCode < 0xCF)||(bCode >= 0xE0 && bCode < 0xEF) ))
                continue;

            if(iLen < 10)
            {
                DbgPrint("reach the end of buffer when finding PTS in PS stream\n");
                return false;
            }
            
            //pes header
            unsigned short wPesLen = *buffer++;
            wPesLen <<= 8;
            wPesLen |= (*buffer);
            buffer++;

            ULONG ulStuffingLength = 0;
            while(*buffer == 0xFF)//mpeg1 stuff
            {
                DbgPrint("mpeg1 stuff!\n");             
                if (ulStuffingLength < (iLen - 9))
                {
                    ulStuffingLength++;
                buffer ++;
            }
                else
                {
                    DbgPrint("reach the end of buffer when finding PTS in PS stream at stuffing\n");
                    return false;
                }
            }

            iLen = iLen - 2 - ulStuffingLength;

            if(bCode == 0xbe || bCode == 0xbf)//private stream
                    continue;

            buffer++;

            BYTE bPesFlags = *buffer++;
            buffer++;

            iLen -= 3;

            if((bPesFlags & 0x80) == 0x80)//pts present
            {
                BYTE b = *buffer++;

                LONGLONG llPts=0;
                llPts = (b&0xE0);
                llPts <<= 3;

                b = *buffer++;
                llPts |= b;
                llPts <<= 8;

                b= *buffer++;
                llPts |= (b & 0xFE);
                llPts <<= 7;

                b = *buffer++;
                llPts |= b;
                llPts <<= 8;

                llPts >>= 1;
                b= *buffer;
                llPts |= (b & 0xFE);

                //llPts /= (90*1000);
                //DbgPrint("pts value is %I64d\n",llPts/(90*1000));
                llPts = llPts*10000000i64/(90*1000);
                
                llCurPts = llPts;
                return true;
            }
       }
   }
   DbgPrint("not find Mpeg Start Code!\n");
   return false;
}

PUCHAR
CTsSynthesizer::
GetTsLocation (
               ULONG *pulBytesInBuffer
               )
{
    if (pulBytesInBuffer != NULL)
        *pulBytesInBuffer = 0;

    LARGE_INTEGER liPerformanceCounter;

    if (g_FileCompleteStatus == 1) {
        g_FileCompleteStatus = 0;
    }

    liPerformanceCounter.QuadPart = KeQueryPerformanceCounter(NULL).QuadPart 
        - m_liInitialPerformanceCounter.QuadPart;

    m_Cursor = NULL;
    //correct any buffer transfer about every 20ms which is lower than the minimum time for typical packets at highest bitrates
    //this keeps buffer latency low so that demux skewing can work smooth and no starvation effects happen after some time 
    if((m_llLastPts - m_llInitialPts - 200000i64) <=  liPerformanceCounter.QuadPart*10000000i64/m_liPerformanceFrequency.QuadPart
        && !m_lNewFrequencyChanging)
    {
        ULONG BufferRemaining;
        BufferRemaining = m_fileReader.Read(m_SynthesisBuffer, m_SampleSize);

        if (BufferRemaining) {
            m_Cursor = m_SynthesisBuffer;
            if (pulBytesInBuffer != NULL)
                *pulBytesInBuffer = BufferRemaining;
        }
        //DbgPrint("Send data--last pts %I64d\n",m_llLastPts);
        LONGLONG  llCurPts = 0;
        if(GetCurTimestamp((char *)m_SynthesisBuffer, m_SampleSize, llCurPts))
        {
            //allow max unsync 1.2 seconds
            if((llCurPts+12000000i64) < m_llLastPts || (llCurPts - m_llLastPts) >= 12000000i64)
            {
                if((llCurPts+12000000i64) < m_llLastPts)
                    DbgPrint("reinialize the pts ---------------------- --- V - %I64d\n",m_llLastPts-llCurPts);
                else
                    DbgPrint("reinialize the pts ---------------------- --- V + %I64d\n",llCurPts-m_llLastPts);
                    
                m_llLastPts = m_llInitialPts = llCurPts;
                m_liInitialPerformanceCounter = KeQueryPerformanceCounter(&m_liPerformanceFrequency);
            }
            else
            {
                if(llCurPts > m_llLastPts)
                {
                    m_llLastPts = llCurPts;
                }
            }        
        }
        else
        {
            DbgPrint("Get Pts In PS failed,m_llInitialPts = %I64d, m_llLastPts = %I64d\n",m_llInitialPts,m_llLastPts);
        }
    }
    else if(m_lNewFrequencyChanging)//to triger file read thread to new frequency!
    {
        m_fileReader.Read(m_SynthesisBuffer, m_SampleSize);
        DbgPrint("m_lNewFrequencyChanging == 1, read and discard one media block!\n");
    }

    return m_Cursor;
}

//
// PutPacket():
//
// Place a transport stream packet at the default cursor location.
// The cursor location must be set via GetTsLocation(PacketIndex).
//
void
CTsSynthesizer::
PutPacket (
           PUCHAR  TsPacket
           )

{
    //
    //  Copy the transport packet to the synthesis buffer.
    //
    RtlCopyMemory (
        m_Cursor,
        TsPacket,
        m_PacketSize
        );
    m_Cursor += m_PacketSize;
}

void
CTsSynthesizer::
SetSampleSize (
               ULONG PacketSize,
               ULONG PacketsPerBuffer
               )
{
    m_PacketSize = PacketSize;
    m_PacketsPerBuffer = PacketsPerBuffer;
    m_SampleSize = PacketSize * PacketsPerBuffer;
}

