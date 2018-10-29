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
    File    : tstream.h
    Abstract: This class synthesizes a transport stream for output from the
              encoder filter.
****************************************************************************/
#include "fileread.h"

#define PACKET_SIZE 188
#define SAMPLE_BUFFER_SIZE (PACKET_SIZE * 312)
#define PID_MASK                        0x1FFF

class CTsSynthesizer {

protected:

    //
    // Frequency
    //
    ULONG m_ulFrequency;
    KSPIN_LOCK  m_FreqSpinLock;
    BOOL m_fFreqChanged;
    BOOL m_fFileReaderFreqChanged;
    WCHAR* m_pczDeviceInstanceID;

    //
    // Read Data Block for MPEG Files 
    //


    CFileReader m_fileReader;
    //PReadVideoStreamFile VideoStream;

    //
    // The packet size of the transport stream
    //
    ULONG m_PacketSize;

    //
    // The number of packets in a capture buffer
    //
    ULONG m_PacketsPerBuffer;

    //
    // The size of the actual data in the capture buffer
    //
    ULONG m_SampleSize;

    //
    // The synthesis buffer.  All transport stream samples are created in this
    // buffer.  This must be set with SetBuffer() before any sample creation
    // routines are called.
    //
    PUCHAR m_SynthesisBuffer;

    //
    // The default cursor.  This is a pointer into the synthesis buffer where
    // the next transport stream byte will be placed. 
    //
    PUCHAR m_Cursor;

    HANDLE m_hGuideHandle;

public:

    virtual void
      Start() {
      m_fileReader.Start();
    }

    virtual void 
      Stop() {
        m_fileReader.Stop();
        if (m_hGuideHandle)
        {
            ZwClose(m_hGuideHandle);
            m_hGuideHandle = NULL;
        }
    }

    //
    // SetSampleSize():
    //
    // Set the size of the synthesis buffer.
    //
    void
    SetSampleSize (
        ULONG PacketSize,
        ULONG PacketsPerBuffer
        );

    //
    // SetBuffer():
    //
    // Set the buffer the synthesizer generates images to.
    //
    void
    SetBuffer (
        PUCHAR SynthesisBuffer
        )
    {
        m_SynthesisBuffer = SynthesisBuffer;
    }

    //
    // GetTsLocation():
    //
    // Set the cursor to point at the given packet index.
    //
    virtual PUCHAR
    GetTsLocation (
        ULONG *pulBytesInBuffer,
        PBOOL pNewFile
        );
    
    //
    // PutPacket():
    //
    // Place a transport stream packet at the default cursor location.
    // The cursor location must be set via GetTsLocation(PacketIndex).
    //
    virtual void
    PutPacket (
        PUCHAR  TsPacket
        );
    
    //
    // SynthesizeTS():
    //
    // Synthesize the next transport stream buffer to be captured.
    //
    virtual void
    SynthesizeTS (
        );

    //
    // DEFAULT CONSTRUCTOR
    //
    CTsSynthesizer (
        ) :
        m_PacketSize (0),
        m_PacketsPerBuffer (0),
        m_SynthesisBuffer (NULL),
        m_LastGuideBufferLength (0),
        m_hGuideHandle (NULL)
    {
    }

    //
    // CONSTRUCTOR:
    //
    CTsSynthesizer (
    PKSDEVICE Device,
        ULONG PacketSize,
        ULONG PacketsPerBuffer,
        ULONG Frequency,
        IN __nullterminated WCHAR* pczDeviceInstanceID
        ); 

    //
    // DESTRUCTOR:
    //
    virtual
    ~CTsSynthesizer (
        )
    {
    }

    //
    //  Get Frequency
    //

    ULONG
    GetFrequency (
       );

    //
    //  Set Frequency
    //

    void
    SetFrequency (
       ULONG ulFrequency
       );

    void FileReaderFreqChanged()
    {
        KIRQL oldIrql;
        KeAcquireSpinLock (&m_FreqSpinLock, &oldIrql);
        m_fFileReaderFreqChanged = TRUE;
        KeReleaseSpinLock (&m_FreqSpinLock, oldIrql);
    }

    BYTE m_pLastGuideBuffer[8192];
    ULONG m_LastGuideBufferLength;
    ULONG m_LastGuidePercentage;
    ULONG m_ulNumberOfReadSections;
    NTSTATUS GetNextGuideSection(BYTE *pNextGuideSection,ULONG *pulSectionLength, ULONG *pulPercentage);
    BOOL IsGuideDataAvailable();

};


