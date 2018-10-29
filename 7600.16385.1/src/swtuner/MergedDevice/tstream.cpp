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

#include "BDATuner.h"

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
  m_fileReader(Device, pczDeviceInstanceID, &g_RegistryPath)
{
  m_ulFrequency = Frequency;
  m_pczDeviceInstanceID = pczDeviceInstanceID;
  KeInitializeSpinLock (&m_FreqSpinLock);
  m_LastGuideBufferLength = 0;
  m_hGuideHandle = NULL;
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

//
// SetFrequency():
//
// Set the frequency on the file reader if it changes. Try to wait
// for the file reader to pick up the frequency before returning.
// In the case of PBDA, reset all the stream slots and the requested
// pid flag array.
//
void
CTsSynthesizer::
SetFrequency(ULONG ulFrequency)
{
    KIRQL oldIrql;
    KeAcquireSpinLock (&m_FreqSpinLock, &oldIrql);
    m_ulFrequency = ulFrequency;
    KeReleaseSpinLock (&m_FreqSpinLock, oldIrql);
    if (m_fileReader.GetFrequency() != ulFrequency)
    {
        m_fileReader.SetFrequency(ulFrequency);
        if (m_fileReader.IsReadStarted())
        {
            BOOL fFileReaderFreqChanged;
    KeAcquireSpinLock (&m_FreqSpinLock, &oldIrql);
            m_fFileReaderFreqChanged = FALSE;
            fFileReaderFreqChanged = FALSE;
    KeReleaseSpinLock (&m_FreqSpinLock, oldIrql);

            // Wait a little more after file reader changes the frequency
            WORD wCount = 0;

            m_ulNumberOfReadSections = 0;
            m_LastGuidePercentage = 0;

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
                    break;
                }
            }

            if (m_hGuideHandle)
            {
                ZwClose(m_hGuideHandle);
                m_hGuideHandle = NULL;
            }
        }
    }
}

//
// GetTsLocation():
//
// Read in the stream packets through the file reader, process the packets
// if necessary and return the sample back.
//


PUCHAR
CTsSynthesizer::
GetTsLocation (
    ULONG *pulBytesInBuffer,
    PBOOL pNewFile
    )
{
//    DbgPrint( "GetTSLocation\n");
  
    if (pulBytesInBuffer != NULL)
        *pulBytesInBuffer = 0;

    m_Cursor = NULL;
    {
        ULONG BufferRemaining;
        BufferRemaining = m_fileReader.Read(m_SynthesisBuffer, m_SampleSize, pNewFile);

        if (BufferRemaining) {
            m_Cursor = m_SynthesisBuffer;
            if (pulBytesInBuffer != NULL)
                *pulBytesInBuffer = BufferRemaining;
        } 
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

//read the next PBDA-SI section from the current file
//or return the last section which was read to determine the length
NTSTATUS CTsSynthesizer::GetNextGuideSection(BYTE *pNextGuideSection,ULONG *pulSectionLength, ULONG *pulPercentage)
{
    NTSTATUS Status = STATUS_SUCCESS;
    
    if (!pulSectionLength)
    {
        DbgPrint("CTsSynthesizer::GetNextGuideSection failed no Length");
        return STATUS_UNSUCCESSFUL;
    }

    if (m_hGuideHandle==NULL)
    {
        Status = m_fileReader.OpenEPGStream(&m_hGuideHandle, m_ulFrequency, TRUE);
        if (Status != STATUS_SUCCESS)
        {
            DbgPrint("CTsSynthesizer::GetNextGuideSection failed OpenEPGStream");
            return Status;
        }
    }
    BYTE *pBuffer = m_pLastGuideBuffer;
    ULONG *pBufferLength = &m_LastGuideBufferLength;

    if (m_LastGuideBufferLength)
    {
        if ((*pulSectionLength >= m_LastGuideBufferLength) && (pNextGuideSection))
        {
            memcpy(pNextGuideSection,pBuffer,m_LastGuideBufferLength); //return the previous read Buffer
            *pulSectionLength = m_LastGuideBufferLength; //return the previous Length
            m_LastGuideBufferLength = 0; //reset the Buffer
        }
        else
        {
            *pulSectionLength = m_LastGuideBufferLength; //return the previous Length
        }
        if (pulPercentage) *pulPercentage = m_LastGuidePercentage;
        return Status;
    }
    
    if (*pulSectionLength > 0)
    {
        pBuffer = pNextGuideSection; //copy direct into the buffer
        pBufferLength = pulSectionLength;
    }
    ULONG ulSavedSections=0;
    ULONG ulTotalSections=0;
    m_fileReader.GetCurrentEPGStatistic(&ulSavedSections, &ulTotalSections);
    if (ulTotalSections == 0)
    {
        if (pulPercentage) *pulPercentage = 0;
        m_LastGuidePercentage = 0;
        if (m_hGuideHandle)
        {
            //calc the percentage based on the file size
            IO_STATUS_BLOCK iostatus;
            FILE_STANDARD_INFORMATION sfinfo;
            Status = ZwQueryInformationFile(m_hGuideHandle, &iostatus, &sfinfo, sizeof(sfinfo),FileStandardInformation);
            if (Status == STATUS_SUCCESS)
            {
                DWORD dwFileSize = sfinfo.EndOfFile.LowPart;
                FILE_POSITION_INFORMATION fpinfo;
                Status = ZwQueryInformationFile(m_hGuideHandle, &iostatus, &fpinfo, sizeof(fpinfo),FilePositionInformation);
                if ((Status == STATUS_SUCCESS) && dwFileSize)
                {
                    DWORD dwFilePosition = fpinfo.CurrentByteOffset.LowPart;
                    m_LastGuidePercentage = dwFilePosition * 100 / dwFileSize;

                }
                else
                {
                    DbgPrint("CTsSynthesizer::GetNextGuideSection ZwQueryInformationFile failed");
                    m_LastGuidePercentage = 100;
                }
            }
            else
            {
                DbgPrint("CTsSynthesizer::GetNextGuideSection ZwQueryInformationFile failed");
                m_LastGuidePercentage = 100;
            }
        }
    }
    else
    {
        m_LastGuidePercentage = ulSavedSections * 100 / ulTotalSections;
    }
    if (pulPercentage)
    {
        *pulPercentage = m_LastGuidePercentage;
    }

    //copy to calling buffer or local member
    Status = m_fileReader.ReadNextPBDASIBufferFromFile(m_hGuideHandle, pBufferLength, pBuffer);
    if (Status == STATUS_SUCCESS)
    {
        m_ulNumberOfReadSections++;
    }
    else
    {
        DbgPrint("CTsSynthesizer::GetNextGuideSection ReadNextPBDASIBuffer failed");
    }

    return Status;

}

BOOL CTsSynthesizer::IsGuideDataAvailable()
{
    ULONG ulSavedSections=0;
    ULONG ulTotalSections=0;
    m_fileReader.GetCurrentEPGStatistic(&ulSavedSections, &ulTotalSections);
    DbgPrint("IsGuideDataAvailable %d of %d PBDA-SI Sections stored. %d already read\n",ulSavedSections,ulTotalSections, m_ulNumberOfReadSections);
    if ((ulSavedSections > m_ulNumberOfReadSections) && ulTotalSections)
    {
        return TRUE;
    }
    return FALSE;
}

