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

#include "bdatuner.h"

#define POLYNOMIAL 0x04c11db7L 

BitReadHelper::BitReadHelper(IStorge *pStorage)
:m_pStorage(pStorage)
,m_bitlen(0)
,m_bitbuff(0)
{
}

void BitReadHelper::BitByteAlign()
{
    m_bitlen &= ~7;    
}

UINT64 BitReadHelper::GetPos()
{
    if(m_pStorage)
        return m_pStorage->GetPos() - (m_bitlen>>3);    
    return 0;
}



int BitReadHelper::BitFlush()
{
    m_bitbuff = 0;
    m_bitlen = 0;
    return 0;
}

UINT64 BitReadHelper::BitRead(int nBits, bool fPeek)
{
    if(!m_pStorage)
        return (UINT64)-1;
    
    while(m_bitlen < nBits)
    {
        m_bitbuff <<= 8;
        if(0 != m_pStorage->Read((BYTE*)&m_bitbuff, 1)) {return (UINT64)-1;} 
        m_bitlen += 8;
    }
    
    int bitlen = m_bitlen - nBits;
    
    UINT64 ret = (m_bitbuff >> bitlen) & ((1ui64 << nBits) - 1);
    
    if(!fPeek)
    {
        m_bitbuff &= ((1ui64 << bitlen) - 1);
        m_bitlen = bitlen;
    }
    
    return ret;
}

int BitReadHelper::ByteRead(BYTE *pData,UINT64 iLen)
{
    if(!m_pStorage)
        return -1;
        Seek(GetPos());
    return m_pStorage->Read(pData, (int)iLen);
}

int BitReadHelper::Seek(UINT64 pos)
{
    if(!m_pStorage)
        return -1;
    int iReturn = m_pStorage->Seek(pos);
    BitFlush();
    return iReturn;
}

BitReadHelper::~BitReadHelper()
{
    if(m_pStorage)
        delete m_pStorage;
}


CMemStorage::CMemStorage(BYTE *buffer,UINT64 llLen)
:m_pData(buffer)
,m_llTotalLenght(llLen)
,m_llPos(0)
{
}

int CMemStorage::Read(__out_bcount(iLen) BYTE *pDest,int iLen)
{
    if((m_llPos + iLen) > m_llTotalLenght)
        return -1;
    RtlCopyMemory(pDest,m_pData + m_llPos,iLen);
    m_llPos += iLen;
    return 0;
}

UINT64 CMemStorage::GetLength()const
{
    return m_llTotalLenght;
}

int CMemStorage::Seek(UINT64 pos)
{
    m_llPos = pos;
    return 0;
}

CMemStorage::~CMemStorage()
{
    
}

UINT64 CMemStorage::GetPos()
{
    return m_llPos;
}

#define MARKER if(bitReader.BitRead(1) != 1) {return(false);}

__int64 swap64(__int64 qw)
{
    BYTE *s,*d;
    s = (BYTE *)&qw;
    d = (BYTE *)&qw + sizeof(__int64)-1;
    for(;s < d;s++,d--)
    {
        *s ^= *d;
        *d ^= *s;
        *s ^= *d;
    }
    return qw;
}

WORD swap16(WORD w)
{
    BYTE *s,*d;
    s = (BYTE *)&w;
    d = (BYTE *)&w + sizeof(w)-1;
    for(;s < d;s++,d--)
    {
        *s ^= *d;
        *d ^= *s;
        *s ^= *d;
    }
    return w;
}

__int32 swap32(__int32 dw)
{
    BYTE *s,*d;
    s = (BYTE *)&dw;
    d = (BYTE *)&dw + sizeof(__int32)-1;
    for(;s < d;s++,d--)
    {
        *s ^= *d;
        *d ^= *s;
        *s ^= *d;
    }
    return dw;
}

bool CTsTimestamp::Read(trhdr &h,BitReadHelper &bitReader)
{
    RtlZeroMemory(&h,sizeof(h));
    
    if(bitReader.BitRead(8, true) != 0x47)
        return(false);

    h.sync = (BYTE)bitReader.BitRead(8);
    h.error = bitReader.BitRead(1);
    h.payloadstart = bitReader.BitRead(1);
    h.transportpriority = bitReader.BitRead(1);
    h.pid = bitReader.BitRead(13);
    h.scrambling = bitReader.BitRead(2);
    h.adapfield = bitReader.BitRead(1);
    h.payload = bitReader.BitRead(1);
    h.counter = bitReader.BitRead(4);

    h.bytes = 188 - 4;

    if(h.adapfield)
    {
        h.length = (BYTE)bitReader.BitRead(8);

        if(h.length > 0)
        {
            h.discontinuity = bitReader.BitRead(1);
            h.randomaccess = bitReader.BitRead(1);
            h.priority = bitReader.BitRead(1);
            h.PCR = bitReader.BitRead(1);
            h.OPCR = bitReader.BitRead(1);
            h.splicingpoint = bitReader.BitRead(1);
            h.privatedata = bitReader.BitRead(1);
            h.extension = bitReader.BitRead(1);
            int iPcrReaded = 0;
            if(h.PCR)
            {
                iPcrReaded += 6;
                h.pcrValue = bitReader.BitRead(33);
                bitReader.BitRead(6);
                h.pcrValue += bitReader.BitRead(9)/300;
            }

            for(int i = 1; i < h.length-iPcrReaded; i++)
                bitReader.BitRead(8);
        }

        h.bytes -= h.length+1;

        if(h.bytes < 0) {return false;}
    }    
    return true;
}

//base should be durations
const __int64 llDurationPerFrameIn90KHZ = 90*1000/25;//1.0/25;//1/25 second

bool CTsTimestamp::TsStreamProcess(BYTE *bBuffer,UINT64 len)
{
    m_pLastEITTime = NULL;
    m_pCRC32Start = NULL;
    UINT64 BlockLen = len;
    while(len >= 188)
    {
        TsPacketEnter(bBuffer,188);
        bBuffer += 188;
        len -= 188;
    }
    if (BlockLen < VIDEO_READ_BUFFER_SIZE)
    {
        //this is the last packet, each subsequent call start at the beginning of the file
        m_fRepeating = TRUE;
        m_pLastEITTime = NULL;
        for(int i=0;i<MAX_Program_Number;i++)
        {
            memset(m_programs[i].m_fStartUp,TRUE,sizeof(BOOL)*MAX_Pid_Number);
            memset(m_programs[i].m_dwTsCurrentPacket, 0, sizeof(DWORD)*MAX_Pid_Number);
        }    
    }
    return true;
}

void CTsTimestamp::InitParams(DWORD dwEITRestamp,DWORD dwPCRDriftPPM,DWORD dwTSPacketBlank)
{
    m_dwEITRestamp = dwEITRestamp;
    m_lPCRDriftPPM = (LONG)dwPCRDriftPPM;
    m_dwTSPacketBlank = dwTSPacketBlank;

    TIME_FIELDS tm_fields;

    tm_fields.Year = 1970;
    tm_fields.Month = 1;
    tm_fields.Day = 1;
    tm_fields.Hour = 0;
    tm_fields.Minute = 0;
    tm_fields.Second = 0;
    tm_fields.Milliseconds = 0;
    tm_fields.Weekday = 0;

    if (RtlTimeFieldsToTime(&tm_fields, &m_liTime1970) == FALSE)
    {
        m_liTime1970.QuadPart = 0;
    }
    m_TotalPBDASISectionsSchedule = 0;
    m_TotalPBDASISectionsPF = 0;
    m_bVersionExtend = 0;
}


void CTsTimestamp::Reset()
{
  for(int i=0;i<MAX_Program_Number;i++)
   {
    m_programs[i].Reset();
   }    
  m_iPatCurLen = 0;
  m_fRepeating = FALSE;
  m_iEITCurLen = 0;
  m_iDistanceToNextEIT_starttime = 0;
  if ((m_dwEITRestamp == 2) || (m_dwEITRestamp == 4))
  {
      m_wMJDDelta = 0;
  }

    m_TotalPBDASISectionsSchedule = 0;
    m_TotalPBDASISectionsPF = 0;
    m_bVersionExtend = 0;
    CleanUpSIDList();

}

void CTsTimestamp::ModifyTimestampBase()
{
    for(int i=0;i<MAX_Program_Number;i++)
    {
        m_programs[i].ModifyTimestampBase();
    }
}

void CProgram::ModifyTimestampBase()
{
    if(-1 != m_llBeginDts)
    {
        m_llCurrentDtsBase = (m_llCurrentDts - m_llBeginDts + llDurationPerFrameIn90KHZ);
        //DbgPrint("Modify current dts base = %d\n",m_llCurrentDtsBase);    
    }
    if(-1 != m_llBeginPts)
    {
        m_llCurrentPtsBase = (m_llCurrentPts - m_llBeginPts + llDurationPerFrameIn90KHZ);
        DbgPrint("program is %d,Modify current pts base = %I64d\n",(DWORD)m_wProgramNumber,m_llCurrentPtsBase);//llDurationPerFrameIn90KHZ;    
    }
    if(-1 != m_llBeginPcr)
    {
        m_llCurrentPcrDuration = (m_llCurrentPcrValue - m_llBeginPcr + llDurationPerFrameIn90KHZ);
        DbgPrint("program is %d,Modify current pcr base = %I64d\n",(DWORD)m_wProgramNumber,m_llCurrentPcrDuration);//llDurationPerFrameIn90KHZ;    
    }
}

void CProgram::Reset()
{
    m_llCurrentPtsBase = 0;
    m_llCurrentDtsBase = 0;
    m_llCurrentPts = 0;
    m_llCurrentDts = 0;
    m_llBeginPts = -1;
    m_llBeginDts = -1;      
    m_wPmtId = 0;
    m_wProgramNumber = 0;
    m_wCurCacheLen = 0;
    m_llCurrentPcrValue = 0;

    m_llCurrentPcrDuration = 0;
    m_llBeginPcr = -1;
    RtlZeroMemory(m_dwTsStopPacket,sizeof(DWORD)*MAX_Pid_Number);
    RtlZeroMemory(m_dwTsCurrentPacket,sizeof(DWORD)*MAX_Pid_Number);
    memset(m_bDiscontinuity_Indicated, true, sizeof(BOOL) * MAX_Pid_Number);
    RtlZeroMemory(m_bLastCounter,sizeof(BYTE)*MAX_Pid_Number);
    memset(m_fStartUp, true,sizeof(BOOL)*MAX_Pid_Number);
    RtlZeroMemory(&m_wEsPid,sizeof(m_wEsPid));
}

BOOL CProgram::IsDiscontinuity(BYTE bCounterVal,int iPidNumber)
{
    BYTE LastCounter = m_bLastCounter[iPidNumber];
    if(m_bDiscontinuity_Indicated[iPidNumber])
    {
        m_bLastCounter[iPidNumber] = bCounterVal;
        m_bDiscontinuity_Indicated[iPidNumber] = false;
        return false;
    }
    m_bLastCounter[iPidNumber]++;
    m_bLastCounter[iPidNumber] &= 0x0F;
    if (m_bLastCounter[iPidNumber] == bCounterVal)
    {
        return false;
    }
    if (LastCounter == bCounterVal)
    {
        //DbgPrint("duplicate packet or error in Service = %d\n",m_wProgramNumber);
        m_bLastCounter[iPidNumber] = bCounterVal;        
        return false;
    }
    DbgPrint("Transport Error in Service = %d\n",m_wProgramNumber);
    m_bLastCounter[iPidNumber] = bCounterVal;
    return true;
}

bool CTsTimestamp::IsPmtTable(WORD pid,int &iIndex)
{
   for(int i=0;i<MAX_Program_Number;i++)
   {
        if(0 == m_programs[i].m_wPmtId)
            break;
        if(pid == m_programs[i].m_wPmtId)
        {
            iIndex = i;
            return true;
        }
   }
   return false;
}


void CTsTimestamp::UpdateCurrentTimestampBase()
{
    for(int i=0;i<MAX_Program_Number;i++)
    {
        m_programs[i].UpdateCurrentTimestampBase();
    }
}

void CProgram::UpdateCurrentTimestampBase()
{
    if(-1 != m_llBeginDts)
    {
        m_llCurrentDtsBase += (m_llCurrentDts - m_llBeginDts + llDurationPerFrameIn90KHZ);
        //DbgPrint("update current dts base = %d\n",m_llCurrentDtsBase);    
    }
    if(-1 != m_llBeginPts)
    {
        m_llCurrentPtsBase += (m_llCurrentPts - m_llBeginPts + llDurationPerFrameIn90KHZ);
        DbgPrint("program is %d,update current pts base = %I64d\n",(DWORD)m_wProgramNumber,m_llCurrentPtsBase);    
    }
    if(-1 != m_llBeginPcr)
    {
        m_llCurrentPcrDuration += (m_llCurrentPcrValue - m_llBeginPcr + llDurationPerFrameIn90KHZ);
        DbgPrint("program is %d,update current pcr base = %I64d\n",(DWORD)m_wProgramNumber,m_llCurrentPcrDuration);//llDurationPerFrameIn90KHZ;    
    }
}

bool CTsTimestamp::Read(trsechdr& h,BitReadHelper &BitHelper)
{
    BYTE pointer_field = (BYTE)BitHelper.BitRead(8);
    h.section_above_length = pointer_field;
    
    if(pointer_field > 188)
           return false;
    
    while(pointer_field-- > 0) 
        BitHelper.BitRead(8);

    h.table_id = (BYTE)BitHelper.BitRead(8);
    h.section_syntax_indicator = (BYTE)BitHelper.BitRead(1);
    h.zero = (BYTE)BitHelper.BitRead(1);
    h.reserved1 = (BYTE)BitHelper.BitRead(2);
    h.section_length = (WORD)BitHelper.BitRead(12);

    h.section_above_length += 4;

    h.transport_stream_id = (WORD)BitHelper.BitRead(16);
    h.reserved2 = (BYTE)BitHelper.BitRead(2);
    h.version_number = (BYTE)BitHelper.BitRead(5);
    h.current_next_indicator = (BYTE)BitHelper.BitRead(1);
    h.section_number = (BYTE)BitHelper.BitRead(8);
    h.last_section_number = (BYTE)BitHelper.BitRead(8);
    return h.section_syntax_indicator == 1/* && h.zero == 0*/;//for atsc tables
}

bool CTsTimestamp::ReadShort(trsechdr& h,BitReadHelper &BitHelper)
{
    BYTE pointer_field = (BYTE)BitHelper.BitRead(8);
    h.section_above_length = pointer_field;

    if(pointer_field > 188)
        return false;

    while(pointer_field-- > 0) 
        BitHelper.BitRead(8);

    h.table_id = (BYTE)BitHelper.BitRead(8);
    h.section_syntax_indicator = (BYTE)BitHelper.BitRead(1);
    h.zero = (BYTE)BitHelper.BitRead(1);
    h.reserved1 = (BYTE)BitHelper.BitRead(2);
    h.section_length = (WORD)BitHelper.BitRead(12);

    h.section_above_length += 4;
    return true;
}

bool CTsTimestamp::TsPacketEnter(BYTE *bBuffer,UINT64 len)
{
    if(!bBuffer || !len)
        return false;
    
    BitReadHelper readHelper(new(NonPagedPool,MS_SAMPLE_TUNER_POOL_TAG) CMemStorage(bBuffer,len));
    trhdr hTsHeader;
    if(!Read(hTsHeader,readHelper))
    {
        return false;
    }

    int iProgramIndex =0;
    int iPIDIndex=0;
    DWORD dwPrefix = (DWORD)readHelper.BitRead(32);
    peshdr hPesheader;
    BOOL bIsEsPid = FALSE;
    CProgram *pcProgram = NULL;

    if(hTsHeader.pid == 0x1FFF)//NULL packet
    {
        return true;
    }
    if(hTsHeader.pid >= 0x20)//Elementary stream or Service section
    {
        bIsEsPid = IsEsPid(hTsHeader.pid,iProgramIndex,iPIDIndex);
        if (bIsEsPid)
        {
            pcProgram = &m_programs[iProgramIndex];
            if (pcProgram->IsDiscontinuity(hTsHeader.counter,iPIDIndex))
            {
                //info
            }
        }
    }

    if(bIsEsPid)
    {
        
        if (hTsHeader.payloadstart)
        {
            pcProgram->m_fStartUp[iPIDIndex] = false; 
            if (!m_fRepeating)
            {
                pcProgram->m_dwTsStopPacket[iPIDIndex]++; //count each Payload start
            }
            else
            {
                pcProgram->m_dwTsCurrentPacket[iPIDIndex]++; //count each Payload start
            }
        }

        if((dwPrefix & 0xFFFFFF00) == 0x00000100)//pes packet
        {        
            if(ReadAndUpdateTimestamp(hPesheader,readHelper,BYTE(dwPrefix & 0xFF),bBuffer,188 - hTsHeader.bytes,*pcProgram))
            {
                //update current pts and dts
                if(hPesheader.fpts &&  hPesheader.pts > pcProgram->m_llCurrentPts)
                    pcProgram->m_llCurrentPts = hPesheader.pts;
                if(hPesheader.fdts && hPesheader.dts > pcProgram->m_llCurrentDts)
                    pcProgram->m_llCurrentDts = hPesheader.dts;    
            }
        }
        if (pcProgram->m_dwTsCurrentPacket[iPIDIndex] && (pcProgram->m_dwTsStopPacket[iPIDIndex] == pcProgram->m_dwTsCurrentPacket[iPIDIndex]) && m_fRepeating)
        { //blend out the end packet as stuffing
            if (m_dwTSPacketBlank & 0x1)
            {
                *(bBuffer+1) = 0xFF;
                *(bBuffer+2) = 0x1F;
            }
            return false;
        }
        if (pcProgram->m_fStartUp[iPIDIndex])
        { //blend out the packet as stuffing
            if (m_dwTSPacketBlank & 0x2)
            {
                *(bBuffer+1) = 0xFF;
                *(bBuffer+2) = 0x1F;
            }
            return false;
        }
    }
    else //non pes packet
    {
        BYTE PointerField = 0;
        if(hTsHeader.pid == 0)//pat table
        {
             //update pat cache 
             if ((m_iPatCurLen + hTsHeader.bytes) <= MAX_SECTION_LENGTH)
             {
                 if (hTsHeader.payloadstart) 
                 {
                     PointerField = *(bBuffer + 188 - hTsHeader.bytes);
                     if(PointerField == 0)
                     {
                        RtlCopyMemory(m_bPatCache,bBuffer + 188 - hTsHeader.bytes,hTsHeader.bytes);
                        m_iPatCurLen = hTsHeader.bytes;
                     }
                     else //section ends in this transport packet and new begin here 
                     {
                         if ((m_iPatCurLen) && (PointerField < 184)) 
                         {
                             //copy the end to complete the section, the next section will be discarded, 
                             //which is not critical because the PAT arrive very often
                             RtlCopyMemory(m_bPatCache + m_iPatCurLen,bBuffer + 189 - hTsHeader.bytes,PointerField);
                             m_iPatCurLen = hTsHeader.bytes;
                         }
                         else
                         {
                             RtlCopyMemory(m_bPatCache + m_iPatCurLen,bBuffer + 188 - hTsHeader.bytes,hTsHeader.bytes);
                             m_iPatCurLen = hTsHeader.bytes;
                         }
                     }
                 }
                 else
                 {
                     if (m_iPatCurLen)
                     {
                         RtlCopyMemory(m_bPatCache + m_iPatCurLen,bBuffer + 188 - hTsHeader.bytes,hTsHeader.bytes);
                         m_iPatCurLen += hTsHeader.bytes;
                     }
                     else
                     {
                         return true;//discard the packet
                     }
                 }
             }
             else
             {
                 m_iPatCurLen = 0;//error or dropped packets
                 return false;
             }

             BitReadHelper patReader(new(NonPagedPool,MS_SAMPLE_TUNER_POOL_TAG) CMemStorage(m_bPatCache,m_iPatCurLen)); 
             trsechdr h2;
      
             Read(h2,patReader);
             if(h2.table_id == 0)
             {  
                int len = h2.section_length;
                len -= 5+4;
                if((h2.section_length + h2.section_above_length) <= m_iPatCurLen)
                {
                    m_iPatCurLen = 0;int j=0;
                    for(int i = len/4; i > 0; i--)
                    {
                        WORD program_number = (WORD)patReader.BitRead(16);
                        patReader.BitRead(3);
                        WORD pid = (WORD)patReader.BitRead(13);
                        if(program_number != 0)
                        {
                            m_programs[j].m_wPmtId = pid;
                            m_programs[j].m_wProgramNumber = program_number;
                            j++;
                            if (j == MAX_Program_Number)
                            {
                                DbgPrint("Too many Services in PAT!");
                                break;
                            }
                        }
                    }
                    m_wNrOfServices = (WORD)j;
                }      
            }
        }
        else if(IsPmtTable(hTsHeader.pid,iProgramIndex))//pmt
        {
          pcProgram = &m_programs[iProgramIndex];
          if ((pcProgram->m_wCurCacheLen + hTsHeader.bytes) <= MAX_SECTION_LENGTH)
          {
              if (hTsHeader.payloadstart) 
              {
                  PointerField = *(bBuffer + 188 - hTsHeader.bytes);
                  if(PointerField == 0)
                  {
                      RtlCopyMemory(pcProgram->m_bCache,bBuffer + 188 - hTsHeader.bytes,hTsHeader.bytes);
                      pcProgram->m_wCurCacheLen = (WORD)hTsHeader.bytes;
                  }
                  else //section ends in this transport packet and new begin here 
                  {
                      if ((pcProgram->m_wCurCacheLen) && (PointerField < 184)) 
                      {
                          //copy the end to complete the section, the next section will be discarded, 
                          //which is not critical because the PMT arrive very often
                          RtlCopyMemory(pcProgram->m_bCache + pcProgram->m_wCurCacheLen,bBuffer + 189 - hTsHeader.bytes,PointerField);
                          pcProgram->m_wCurCacheLen = (WORD)hTsHeader.bytes;
                      }
                      else
                      {
                          RtlCopyMemory(pcProgram->m_bCache + pcProgram->m_wCurCacheLen,bBuffer + 188 - hTsHeader.bytes,hTsHeader.bytes);
                          pcProgram->m_wCurCacheLen = (WORD)hTsHeader.bytes;
                      }
                  }
              }
              else
              {
                  if (pcProgram->m_wCurCacheLen)
                  {
                      RtlCopyMemory(pcProgram->m_bCache + pcProgram->m_wCurCacheLen,bBuffer + 188 - hTsHeader.bytes,hTsHeader.bytes);
                      pcProgram->m_wCurCacheLen += (WORD)hTsHeader.bytes;
                  }
                  else
                  {
                      return true;//discard the packet
                  }
              }
          }
          else
          {
              pcProgram->m_wCurCacheLen = 0;//error or dropped packets
              return false;
          }

          BitReadHelper pmtReader(new(NonPagedPool,MS_SAMPLE_TUNER_POOL_TAG) CMemStorage(pcProgram->m_bCache,pcProgram->m_wCurCacheLen)); 
          trsechdr h2;
          Read(h2,pmtReader);
          if(h2.table_id == 2)//pmt table
          {
            int len = h2.section_length;
            len -= 5+4; //remaining length (without header and CRC)

            if((h2.section_length + h2.section_above_length) <= pcProgram->m_wCurCacheLen)
            {
                RtlZeroMemory(pcProgram->m_wEsPid,  sizeof(pcProgram->m_wEsPid));
                pcProgram->m_wCurCacheLen = 0;
                pmtReader.BitRead(3);
                pmtReader.BitRead(13);
                pmtReader.BitRead(4);
                WORD program_info_length = (WORD)pmtReader.BitRead(12);
                len -= 4+program_info_length;

                while(program_info_length-- > 0) pmtReader.BitRead(8);

                for(int i = 0; i < MAX_Pid_Number && len >= 5; i++)
                {
                    pmtReader.BitRead(8);
                    pmtReader.BitRead(3);
                    WORD pid = (WORD)pmtReader.BitRead(13);
                    pmtReader.BitRead(4);
                    WORD ES_info_length = (WORD)pmtReader.BitRead(12);
                    len -= 5+ES_info_length;
                    while(ES_info_length > 0) 
                    {
                        pmtReader.BitRead(8);
                        ES_info_length--;
                    }
                    pcProgram->m_wEsPid[i]= pid;
                }
             }
           }
        }//end of pmt handling
        else if(hTsHeader.pid == 0x12)//EIT table
        {
            if(m_dwEITRestamp)
            {
                if (m_wMJDDelta || (m_dwEITRestamp & 8)) 
                {
                    return ProcessEITPacket(bBuffer,hTsHeader);
                }
                else
                {   //stuff the beginning
                    *(bBuffer+1) = 0xFF;
                    *(bBuffer+2) = 0x1F;
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
        else if(hTsHeader.pid == 0x14)//TDT/TOT table
        {
            return ProcessTDTPacket(bBuffer,hTsHeader);
        }
    }//end non pes packet
    
    if(hTsHeader.PCR)
    {
        if(bIsEsPid)
        {
            if(pcProgram->m_llCurrentPcrValue < hTsHeader.pcrValue)
            {
                 pcProgram->m_llCurrentPcrValue = hTsHeader.pcrValue;
            }
            if( -1 == pcProgram->m_llBeginPcr )
                pcProgram->m_llBeginPcr = hTsHeader.pcrValue;

            if(pcProgram->m_llBeginPcr > hTsHeader.pcrValue && pcProgram->m_llCurrentPcrDuration != 0)//the second pass
            {
                pcProgram->m_llBeginPcr = hTsHeader.pcrValue;
                DbgPrint("modify pcr --- ");
                pcProgram->ModifyTimestampBase();
            }
            
            if(pcProgram->m_llCurrentPcrDuration != 0)
            {
                __int64 llCurPcr = hTsHeader.pcrValue + pcProgram->m_llCurrentPcrDuration;// - llDurationPerFrameIn90KHZ*50;

                //todo: calc the artifical pcr drift in ppm (+/-) 
                if (m_lPCRDriftPPM)
                {
                    llCurPcr = llCurPcr + (pcProgram->m_llCurrentPcrDuration * m_lPCRDriftPPM) / 1000000; 
                }
                //DbgPrint("modify new pcr value = %I64u!\n",llCurPcr);
                //from 6 Bytes,33(90KHZ),6(reserved),9(27MHZ)
                //__int64 llPcrInTs = llCurPcr;
                *(DWORD *)(bBuffer + 6) = swap32((__int32)(llCurPcr>>1));
                *(BYTE *)(bBuffer + 10) |= (BYTE)((llCurPcr>>32) & 0x01);
                *(BYTE *)(bBuffer + 10) |= 0x7E;
                *(BYTE *)(bBuffer + 11) = 0;
             }
        }
    }
    return true;
}

bool CTsTimestamp::IsEsPid(WORD pid,int &iProgramIndex, int &iPidIndex)
{
    for(int i=0;i<MAX_Program_Number;i++)
    {
        if(0 == m_programs[i].m_wPmtId)
            break; 
        for(int j = 0;j<MAX_Pid_Number;j++)
        {
            if(0 == m_programs[i].m_wEsPid[j])
                break; 
            if(pid == m_programs[i].m_wEsPid[j])
             {
                 iProgramIndex = i;
                 iPidIndex = j;
                 return true;
             }
        }
    }
    return false;
}

void CTsTimestamp::DbgCurrentTimestamp()
{
    for(int i=0;i<MAX_Program_Number;i++)
    {
        if(0 == m_programs[i].m_wPmtId)
            break; 
        DbgPrint("6 -- program number is %d , pts duration is %I64d\n"
            ,(UINT)m_programs[i].m_wProgramNumber,m_programs[i].m_llCurrentPtsBase);
    }
}

bool CTsTimestamp::ReadAndUpdateTimestamp(peshdr &h,BitReadHelper &bitReader,BYTE code, BYTE * buffer,int dwTsHeaderPast,CProgram &it)
{
    if(dwTsHeaderPast <= 0 || dwTsHeaderPast >= 188)
        return false;

    RtlZeroMemory(&h, sizeof(h));

    if(!(code >= 0xbd && code < 0xf0))
        return(false);

    h.len = (WORD)bitReader.BitRead(16);

    if(code == 0xbe || code == 0xbf)//private stream
        return(true);

    h.type = (BYTE)bitReader.BitRead(2);
    if(2 != h.type)
    {
        return false;
    }

    h.scrambling = (BYTE)bitReader.BitRead(2);
    h.priority = (BYTE)bitReader.BitRead(1);
    h.alignment = (BYTE)bitReader.BitRead(1);
    h.copyright = (BYTE)bitReader.BitRead(1);
    h.original = (BYTE)bitReader.BitRead(1);
    h.fpts = (BYTE)bitReader.BitRead(1);
    h.fdts = (BYTE)bitReader.BitRead(1);
    h.escr = (BYTE)bitReader.BitRead(1);
    h.esrate = (BYTE)bitReader.BitRead(1);
    h.dsmtrickmode = (BYTE)bitReader.BitRead(1);
    h.morecopyright = (BYTE)bitReader.BitRead(1);
    h.crc = (BYTE)bitReader.BitRead(1);
    h.extension = (BYTE)bitReader.BitRead(1);
    h.hdrlen = (BYTE)bitReader.BitRead(8);
    //int iPast = 9;
    int iPast = dwTsHeaderPast;
    iPast += 9;

    if(h.fpts)
    {
        BYTE b = (BYTE)bitReader.BitRead(4);
        if(!(h.fdts && b == 3 || !h.fdts && b == 2)) {return(false);}

        h.pts = 0;
        h.pts |= (bitReader.BitRead(3) << 30); 
        MARKER; // 32..30

        h.pts |= (bitReader.BitRead(15) << 15); 
        MARKER; // 29..15

        h.pts |= bitReader.BitRead(15); 
        MARKER; // 14..0

        if(h.pts < it.m_llBeginPts && it.m_llCurrentPtsBase != 0)
        {
            DbgPrint("modify pts -- ");
            it.m_llBeginPts = h.pts;
            it.ModifyTimestampBase();
        }
        
        __int64 llNewPts = h.pts + it.m_llCurrentPcrDuration;//90khz,m_llCurrentPtsBase
        
        if(-1 == it.m_llBeginPts)
            it.m_llBeginPts = h.pts;
        
        *(buffer + iPast) &= 0xF1;
        *(buffer + iPast) |= (llNewPts>>29 & 0x0E);
        iPast++;

        *(WORD *)(buffer + iPast) &= 0x0100;
        *(WORD *)(buffer + iPast) |= swap16((WORD(llNewPts>>15))<<1);
        iPast += 2;

        *(WORD *)(buffer + iPast) &= 0x0100;
        *(WORD *)(buffer + iPast) |= swap16((WORD(llNewPts))<<1);
        iPast += 2;         
    }

    if(h.fdts)
    {
        if((BYTE)bitReader.BitRead(4) != 1) 
        {return(false);}

        h.dts = 0;
        h.dts |= bitReader.BitRead(3) << 30; MARKER; // 32..30
        h.dts |= bitReader.BitRead(15) << 15; MARKER; // 29..15
        h.dts |= bitReader.BitRead(15); MARKER; // 14..0

        if(-1 == it.m_llBeginDts)
            it.m_llBeginDts = h.dts;
        
        __int64 llNewDts = h.dts + it.m_llCurrentPcrDuration;//90khz,it.m_llCurrentDtsBasem_llCurrentPtsBase
        
        *(buffer + iPast) &= 0xF1;
        *(buffer + iPast) |= (llNewDts>>29 & 0x0E);
        iPast++;

        *(WORD *)(buffer + iPast) &= 0x0100;
        *(WORD *)(buffer + iPast) |= swap16((WORD(llNewDts>>15))<<1);
        iPast += 2;

        *(WORD *)(buffer + iPast) &= 0x0100;
        *(WORD *)(buffer + iPast) |= swap16((WORD(llNewDts))<<1);
        iPast += 2;
    }

    // skip to the end of header
    if(h.len) h.len -= 3+h.hdrlen;

    int left = h.hdrlen;
    if(h.fpts) left -= 5;
    if(h.fdts) left -= 5;

    if(left >= 188)
        return false;
    
    while(left-- > 0) bitReader.BitRead(8);

    return(true);
}

static unsigned long crc_table[256] = {0};

void CTsTimestamp::gen_crc_table()
/* generate the table of CRC remainders for all possible bytes */
{
    register int i, j;  register unsigned long crc_accum;

    for ( i = 0;  i < 256;  i++ )
    {
        crc_accum = ( (unsigned long) i << 24 );
        for ( j = 0;  j < 8;  j++ )
        {
            if ( crc_accum & 0x80000000L )
                crc_accum = ( crc_accum << 1 ) ^ POLYNOMIAL;
            else
                crc_accum = ( crc_accum << 1 );
        }
        crc_table[i] = crc_accum;
    }
}

unsigned long
CTsTimestamp::calc_crc(
         unsigned char *data_blk_ptr,
         unsigned long data_blk_size,
         unsigned long crc_last
         )
         /* calculate the CRC32 of a section where data_blk_size is the length of the section */
{
    unsigned long i, j;
    unsigned long crc_accum = crc_last;

    for ( j = 0;  j < data_blk_size;  j++, data_blk_ptr++ )
    {
        i = (( crc_accum >> 24) ^ (*data_blk_ptr)) & 0xff;
        crc_accum = (( crc_accum << 8 ) ^ crc_table[i]);
    }
    return crc_accum;
}

void CTsTimestamp::MJDUTCPlus(WORD &mjd, BYTE &utc_h_bcd, BYTE &utc_m_bcd)
{
    if (m_dwEITRestamp & 8)
    {
        return;//time correction disabled
    }
    int iMinute = (utc_m_bcd & 0xF) + (utc_m_bcd >> 4)*10; //calc decimal minute
    int iHour = (utc_h_bcd & 0xF) + (utc_h_bcd >> 4)*10; //calc decimal hour
    iMinute += m_iUTCDeltaM;
    iHour += m_iUTCDeltaH;

    if (iMinute > 59)
    {
        iMinute -= 60;
        iHour ++;
    }
    if (iHour > 23)
    {
        iHour -= 24;
        mjd --;
    }
    //convert back to bcd
    ULONG ulTemp = (iHour/10 << 4) + (iHour%10);
    utc_h_bcd = *((BYTE*)(&ulTemp));
    ulTemp = (iMinute/10 << 4) + (iMinute%10);
    utc_m_bcd =*((BYTE*)(&ulTemp)) ;
    
}

bool CTsTimestamp::ProcessTDTPacket(BYTE *bBuffer,trhdr &hTsHeader)
{
    if (m_dwEITRestamp & 8)
    {
        return true;//do not use TDT to correct the time
    }
    if (!m_wMJDDelta)
    {
        BYTE tdtBuffer[188];

        if (hTsHeader.payloadstart)
        {
            if ((hTsHeader.bytes > 0) && (hTsHeader.bytes <= sizeof(tdtBuffer)))
            {
                RtlCopyMemory(tdtBuffer,bBuffer + 188 - hTsHeader.bytes,hTsHeader.bytes);
            }
            else
            {
                return true;
            }
        }
        else
        {
            return true;
        }

        BitReadHelper tdtReader(new(NonPagedPool,MS_SAMPLE_TUNER_POOL_TAG) CMemStorage(tdtBuffer,hTsHeader.bytes)); 
        trsechdr h2;
        ReadShort(h2,tdtReader);
        if((h2.table_id == 0x70) || (h2.table_id == 0x73))//time and date table
        {
            WORD wMJDTransport = (WORD)tdtReader.BitRead(16);//16LSB is MJD, following 24bit UTC (4-bit BCD)
            BYTE wUTCTransportH = (BYTE)tdtReader.BitRead(8);
            BYTE wUTCTransportM = (BYTE)tdtReader.BitRead(8);
            LARGE_INTEGER liSystemTime;
            TIME_FIELDS SysTimeField;
            KeQuerySystemTime(&liSystemTime);
            RtlTimeToTimeFields(&liSystemTime,&SysTimeField);

            WORD wYear = SysTimeField.Year - 1900;//normalize to 1900 
            WORD L=0;
            if (SysTimeField.Month < 3)
            {
                L=1;
            }

            WORD wMJDSystem = 0;
            DWORD dwMJDSystem = 0;

            KFLOATING_SAVE kfs;

            KeSaveFloatingPointState(&kfs);
            dwMJDSystem = 14956 + SysTimeField.Day + (int) ((wYear - L)*365.25) + (int) ((SysTimeField.Month + 1 + L*12)*30.6001); 
            wMJDSystem = *(WORD*)(&dwMJDSystem);
            KeRestoreFloatingPointState(&kfs);

            if (!wMJDSystem)
            {
                return true;
            }

            m_wMJDDelta = wMJDSystem - wMJDTransport;
            m_iUTCDeltaM = (wUTCTransportM & 0xF) + (wUTCTransportM >> 4)*10; //calc decimal minute
            m_iUTCDeltaH = (wUTCTransportH & 0xF) + (wUTCTransportH >> 4)*10; //calc decimal hour
            m_iUTCDeltaM = SysTimeField.Minute - m_iUTCDeltaM; //calc delta to system time-minute 
            if (m_iUTCDeltaM < 0)
            {
                m_iUTCDeltaM += 60;
                m_iUTCDeltaH ++;
            }
            m_iUTCDeltaH = SysTimeField.Hour - m_iUTCDeltaH;  //calc delta to system time-hour
            if (m_iUTCDeltaH < 0)
            {
                m_iUTCDeltaH += 24;
                m_wMJDDelta ++;
            }
        }
    }
    return true;
}

bool CTsTimestamp::ProcessEITPacket(BYTE *bBuffer,trhdr &hTsHeader)
{
    BOOL bReProcess = FALSE;
    int iTSPayloadLen = hTsHeader.bytes;
    BYTE PointerField = 0;
    BYTE TSPacketPayloadMax = 184;
    do
    {
        if ((m_iEITCurLen + iTSPayloadLen) <= MAX_EITSECTION_LENGTH)
        {
            if (hTsHeader.payloadstart) 
            {
                PointerField = *(bBuffer + 188 - hTsHeader.bytes);

                if(PointerField == 0)
                {
                    RtlCopyMemory(m_bEITCache,bBuffer + 188 - iTSPayloadLen,iTSPayloadLen);
                    m_iEITCurLen = iTSPayloadLen;
                    m_iDistanceToNextEIT_starttime = 17; //move to first start_time
                    m_bLastEITVersion = hTsHeader.counter;
                }
                else //section ends in this transport packet and new begin here 
                {
                    if ((m_iEITCurLen) && (PointerField < 184) && (((m_bLastEITVersion+1)&0xF) == hTsHeader.counter)) 
                    {
                        //copy the end to complete the section, the next section start after the Pointer_Field,
                        iTSPayloadLen --; //remove the pointer field byte 
                        RtlCopyMemory(m_bEITCache + m_iEITCurLen,bBuffer + 188 - iTSPayloadLen,iTSPayloadLen);
                        m_iEITCurLen += iTSPayloadLen; //add the payload without the pointer field
                        bReProcess = TRUE;
                        TSPacketPayloadMax = 183;//section without pointerfield byte
                   }
                    else
                    {   //copy the start of the section
                        RtlCopyMemory(m_bEITCache,bBuffer + 188 - iTSPayloadLen,iTSPayloadLen);
                        m_iEITCurLen = iTSPayloadLen;
                        m_iDistanceToNextEIT_starttime = 17 + PointerField; //move to first start_time
                        m_bLastEITVersion = hTsHeader.counter;
                        bReProcess = FALSE;
                        TSPacketPayloadMax = 184;
                    }
                }
            }
            else
            {
                if (m_iEITCurLen)
                {
                    m_bLastEITVersion++; m_bLastEITVersion &= 0xF;
                    if (m_bLastEITVersion != hTsHeader.counter)
                    {
                        DbgPrint("Interrupted EIT Section received, TS discontinuity\n");
                        m_iEITCurLen = 0;
                        return false;
                    }
                    RtlCopyMemory(m_bEITCache + m_iEITCurLen,bBuffer + 188 - iTSPayloadLen,iTSPayloadLen);
                    m_iEITCurLen += iTSPayloadLen;
                }
                else
                {
                    DbgPrint("Drop EIT Section, no packet start\n");
                    m_iEITCurLen = 0;
                    return true;//discard the packet
                }
            }
        }
        else
        {
            m_iEITCurLen = 0;//error or dropped packets
            return false;
        }

        //BitReadHelper eitReader(new(NonPagedPool,MS_SAMPLE_TUNER_POOL_TAG) CMemStorage(m_bEITCache,m_iEITCurLen)); 
        trsechdr h2;
        BYTE pointer_field = m_bEITCache[0];
        h2.section_above_length = pointer_field;

        if(pointer_field > 188)
        {
            m_iEITCurLen = 0;
            return false;
        }

        h2.table_id = m_bEITCache[pointer_field+1];
        h2.section_length = (m_bEITCache[pointer_field+2] & 0xF) << 8;
        h2.section_length += m_bEITCache[pointer_field+3];
        h2.section_above_length += 4;
        //Read(h2,eitReader);
        if((h2.table_id >= 0x4E) && (h2.table_id < 0x70))//eit table EIT from 4E to 6F
        {
            int len = h2.section_length + h2.section_above_length;//complete section incl. above length
            BYTE *pCurrentBuffer = 0;
            int iRemainingBytes = 0;
            BYTE *pCRC32Start = 0;
            int iStuffingLength = 0;

            if (m_iEITCurLen >= len )
            {
                iStuffingLength = (m_iEITCurLen - len) + 4;//stuffing length + CRC32
            }
            if (h2.section_length < 23)
            {
                m_iEITCurLen = 0;
                return true; //empty EIT
            }

            do 
            {
                if (iStuffingLength && ((iStuffingLength + m_iDistanceToNextEIT_starttime) > iTSPayloadLen))
                {   //no further EIT data but the CRC32 left
                    if (iStuffingLength <= TSPacketPayloadMax)
                    {
                        pCRC32Start = bBuffer + 188 - iStuffingLength;
                    }
                    else
                    {
                        //the CRC32 is exactly cut off by the last packet, we have to memorize the last CRC start and do a special handling
                        pCRC32Start = 0;
                    }
                    iTSPayloadLen = 0;
                }
                else
                {
                    BYTE *pEndOfBuffer = bBuffer + 188;
                    pCurrentBuffer = pEndOfBuffer - iTSPayloadLen;
                    if (m_iDistanceToNextEIT_starttime >= 0)
                    {
                        m_pCRC32Start = pEndOfBuffer - 4;
                        if (iTSPayloadLen <= m_iDistanceToNextEIT_starttime)
                        {
                            m_iDistanceToNextEIT_starttime -= iTSPayloadLen;
                            return true;
                        }
                        iRemainingBytes = iTSPayloadLen - m_iDistanceToNextEIT_starttime;
                        //we need at least 2 byte for MJD and 2 Byte for hour/minute
                        //if this on the boundary to the next TS packet we have to process  
                        //next time
                        if (iRemainingBytes < 4)
                        {
                            m_iDistanceToNextEIT_starttime = 0 - iRemainingBytes;
                            m_pLastEITTime = pEndOfBuffer - iRemainingBytes; //remember the last start of the start_time
                            return true;
                        }
                        else
                        {
                            //process MJD+UTC and determine the next start_time position if possible
                            //remain the m_iDistanceToNextEIT_starttime negativ up to the descriptor loop length
                            pCurrentBuffer += m_iDistanceToNextEIT_starttime;
                            iTSPayloadLen -= m_iDistanceToNextEIT_starttime;

                            WORD wMJDCurrent = (*pCurrentBuffer)<<8; pCurrentBuffer++;                            
                            wMJDCurrent += (*pCurrentBuffer); pCurrentBuffer++;                           
                            wMJDCurrent += m_wMJDDelta; //correct date relative to systemtime

                            BYTE bCurrentHour = (*pCurrentBuffer); pCurrentBuffer++;
                            BYTE bCurrentMinute = (*pCurrentBuffer); 
                            pCurrentBuffer -=3;

                            MJDUTCPlus(wMJDCurrent,bCurrentHour,bCurrentMinute);
                            BYTE *pEITCacheStartTime = m_bEITCache + m_iEITCurLen - iTSPayloadLen; 
                            *pEITCacheStartTime = *pCurrentBuffer = (BYTE)(wMJDCurrent >> 8); pCurrentBuffer++; pEITCacheStartTime++;
                            *pEITCacheStartTime = *pCurrentBuffer = (BYTE) wMJDCurrent; pCurrentBuffer++; pEITCacheStartTime++;
                            *pEITCacheStartTime = *pCurrentBuffer = bCurrentHour; pCurrentBuffer++; pEITCacheStartTime++;
                            *pEITCacheStartTime = *pCurrentBuffer = bCurrentMinute; pCurrentBuffer++;
                            iTSPayloadLen -= 4;
                            m_iDistanceToNextEIT_starttime = -4;
                            continue;

                        }
                    }
                    else
                    {
                        if (m_iDistanceToNextEIT_starttime > (-4))
                        {   //special case the start_time is cut off at TS packet boundery
                            //calc from the cache buffer and write back into the original buffer at m_pLastEITTime
                            if (m_pLastEITTime)
                            {                            
                                pCurrentBuffer = m_pLastEITTime;
                                int iPreviousRest = 0 - m_iDistanceToNextEIT_starttime;
                                BYTE *pEITCacheStartTime = m_bEITCache + m_iEITCurLen - iTSPayloadLen + m_iDistanceToNextEIT_starttime; 

                                WORD wMJDCurrent = (*pEITCacheStartTime)<<8; 
                                wMJDCurrent += *(pEITCacheStartTime + 1);                            
                                wMJDCurrent += m_wMJDDelta; //correct date relative to systemtime

                                BYTE bCurrentHour = *(pEITCacheStartTime + 2);
                                BYTE bCurrentMinute = *(pEITCacheStartTime + 3); 

                                MJDUTCPlus(wMJDCurrent,bCurrentHour,bCurrentMinute);
                                *pEITCacheStartTime = *pCurrentBuffer = (BYTE)(wMJDCurrent >> 8); pCurrentBuffer++; pEITCacheStartTime++;
                                iPreviousRest--;
                                if (iPreviousRest == 0)
                                {
                                    pCurrentBuffer = pEndOfBuffer - iTSPayloadLen;
                                }
                                *pEITCacheStartTime = *pCurrentBuffer = (BYTE) wMJDCurrent; pCurrentBuffer++; pEITCacheStartTime++;
                                if (iPreviousRest)
                                {
                                    iPreviousRest--;
                                    if (iPreviousRest == 0)
                                    {
                                        pCurrentBuffer = pEndOfBuffer - iTSPayloadLen;
                                    }
                                }
                                else
                                {
                                    iTSPayloadLen--;
                                }
                                *pEITCacheStartTime = *pCurrentBuffer = bCurrentHour; pCurrentBuffer++; pEITCacheStartTime++;
                                if (iPreviousRest)
                                {
                                    iPreviousRest--;
                                    pCurrentBuffer = pEndOfBuffer - iTSPayloadLen;
                                }
                                else
                                {
                                    iTSPayloadLen--;
                                }
                                *pEITCacheStartTime = *pCurrentBuffer = bCurrentMinute; pCurrentBuffer++;
                                iTSPayloadLen --;
                            }
                            else
                            {
                                iTSPayloadLen -= (4+ m_iDistanceToNextEIT_starttime);
                            }
                            m_iDistanceToNextEIT_starttime = -4;

                        }
                        else //m_iDistanceToNextEIT_starttime -4 ... -8
                        {
                            //move to descriptor loop length and next start_time
                            //check remaining iTSPayloadLen
                            if (iTSPayloadLen > 5)
                            {   
                                BYTE *pLoopLenBuffer = m_bEITCache + m_iEITCurLen + m_iDistanceToNextEIT_starttime + 8 - iTSPayloadLen; //start of the descriptor loop length
                                WORD wDescriptorLoopLength = ((*pLoopLenBuffer) & 0xF)<<8;
                                pLoopLenBuffer++;
                                wDescriptorLoopLength += (*pLoopLenBuffer);
                                if (iStuffingLength)
                                {
                                    pCRC32Start = pEndOfBuffer - iStuffingLength;
                                    m_pCRC32Start = NULL;
                                }
                                else
                                {
                                    m_pCRC32Start = pCRC32Start = pEndOfBuffer - 4;
                                }

                                m_iDistanceToNextEIT_starttime = m_iDistanceToNextEIT_starttime + 10 + wDescriptorLoopLength + 2;
                                if (m_iDistanceToNextEIT_starttime > iTSPayloadLen)
                                {
                                    m_iDistanceToNextEIT_starttime -= iTSPayloadLen;
                                    return true;
                                }
                                else
                                {
                                    iTSPayloadLen -= m_iDistanceToNextEIT_starttime;
                                    m_iDistanceToNextEIT_starttime = 0;
                                    continue;
                                }
                            }
                            else
                            {
                                m_iDistanceToNextEIT_starttime -= iTSPayloadLen;
                                return true;
                            }
                        }
                    }
                }
            } while(iTSPayloadLen > iStuffingLength);

            if(len <= m_iEITCurLen)
            {
                //process the buffer
                //and recalc the CRC32
                //problem, we have to do this in the source buffer, which may wrap at the buffer boundary
                //the CRC32 is at pCRC32Start
                DWORD dwNewCRC = 0;
                if (m_dwEITRestamp & 7)//only if we use restamping we calc the CRC32
                {
                    dwNewCRC = calc_crc(m_bEITCache + h2.section_above_length -3, h2.section_length -1);
                dwNewCRC = swap32(dwNewCRC);
                }

                ConvertGuideToPBDASI((BYTE *)(m_bEITCache + h2.section_above_length -3), h2.section_length + 3);

                if (m_dwEITRestamp & 7)
                {
                if (pCRC32Start)
                {
                    *(DWORD *)pCRC32Start = dwNewCRC;
                }
                else
                {
                    if (m_pCRC32Start && (iStuffingLength > TSPacketPayloadMax))
                    {
                        int iPreviousBytes = iStuffingLength - TSPacketPayloadMax;
                        BYTE PayloadOffset = 4;
                        if (PointerField) PayloadOffset++;

                        if (PointerField > 3)
                        {
                            //Error in Section
                            DbgPrint("EIT CRC32 previous Buffer Error!\n");
                            m_iEITCurLen = 0;
                            return false;
                        }
                        m_pCRC32Start += (4 - iPreviousBytes);
                        *m_pCRC32Start = (BYTE)dwNewCRC; //set first CRC32 byte
                        iPreviousBytes--;m_pCRC32Start++;
                        dwNewCRC = dwNewCRC >> 8;
                        if (iPreviousBytes)
                        {
                            iPreviousBytes--;
                            *m_pCRC32Start = (BYTE)dwNewCRC;//set second CRC32 byte
                            m_pCRC32Start++;
                            if (iPreviousBytes == NULL)
                            {
                                m_pCRC32Start = bBuffer + PayloadOffset;//set next pos
                            }
                        }
                        else
                        {
                            m_pCRC32Start = bBuffer + PayloadOffset;
                            *m_pCRC32Start = (BYTE)dwNewCRC;//set second CRC32 byte
                            m_pCRC32Start++;//set next pos
                        }
                        dwNewCRC = dwNewCRC >> 8;
                        *m_pCRC32Start = (BYTE)dwNewCRC;//set 3rd CRC32 byte
                        m_pCRC32Start++;//set next pos
                        dwNewCRC = dwNewCRC >> 8;
                        if (iPreviousBytes)
                        {
                            m_pCRC32Start = bBuffer + PayloadOffset;
                        }
                        *m_pCRC32Start = (BYTE)dwNewCRC;//set 4rd CRC32 byte
                        m_pCRC32Start = NULL;
                    }
                }
                }//end if use restamping
                m_pCRC32Start = NULL;
                m_pLastEITTime = NULL;
                iTSPayloadLen = hTsHeader.bytes;
                m_iEITCurLen = 0;
            }
        }
    }while (bReProcess);
    return true;
}

void CTsTimestamp::ConvertGuideToPBDASI(BYTE *pEITBuffer, WORD wEITBufferLength)
{
    //convert the EIT data into a PBDA structure, allocate the memory and copy sections into it.
    if ((!pEITBuffer) || (wEITBufferLength < 30)) 
    {
        return;
    }
    BYTE bTableId=0;
    WORD wTableLength=0;
    WORD wServiceId=0;
    WORD wTSId=0;
    WORD wNetworkId=0;
    BYTE bVersionNr=0;
    BYTE bSectionNumber=0;
    BYTE bLastSectionNumber=0;
    BYTE bSegmentLastSectionNumber=0;
    BYTE bLastTableId=0;
    BYTE *pBuffer = pEITBuffer;

    bTableId = *pBuffer;
    pBuffer++;
    if ((bTableId >= 0x60) || (bTableId < 0x4E))//check legal Tables and block other Schedule EIT, 
    {//if you intend to enhance make sure the version array in SERVICEID_EIT_VERSION is big enough
        return;
    }

    wTableLength = (*pBuffer) & 0x0F;
    wTableLength = wTableLength << 8;
    pBuffer++;
    wTableLength += *pBuffer;
    pBuffer++;

    wServiceId = *(WORD *)pBuffer;//unswapped SID
    pBuffer++;pBuffer++;

    bVersionNr = ((*pBuffer) & 0x3E) >> 1;
    pBuffer++;

    bSectionNumber = *pBuffer;
    pBuffer++;

    bLastSectionNumber = *pBuffer;
    pBuffer++;

    wTSId = *(WORD *)pBuffer;//unswapped TSId
    pBuffer++;pBuffer++;
     
    wNetworkId = *(WORD *)pBuffer;//unswapped NWId
    pBuffer++;pBuffer++;

    bool bVersionCheckResultNow = false;

    //Version check to reduce performance
    if (CheckEITVersion(wServiceId, bTableId, bSectionNumber, bVersionNr, bVersionCheckResultNow) == true)
    {
        return; //already parse the section
    }

    bSegmentLastSectionNumber = *pBuffer;
    pBuffer++;

    bLastTableId = *pBuffer;
    pBuffer++;

    if (wEITBufferLength < wTableLength + 2)
    {
        DbgPrint("EIT %x Table Length Error!\n",bTableId);
        return;
    }

    WORD wAvailableBuffer = wEITBufferLength * 2; //PBDA size in worst case less then twice the double of SI

    PBDA_SI_BUFFER *pPBDASIBuffer = AllocatePBDASIData(wAvailableBuffer); 
    WORD wUsedBufferSize = 13; //minimum Headersize for PBDA-SI 

    if (!pPBDASIBuffer) return;//out of memory or Cache full

    if (bTableId < 0x50) //present/following
    {
        pPBDASIBuffer->argbData[0] = 0x81;
        if (!m_TotalPBDASISectionsPF)
        {
            m_TotalPBDASISectionsPF = (bLastSectionNumber + 1) * m_wNrOfServices;//bulk part of sections
        }
        //PBDA version is based on each Service, while EIT is devided into Sections and tableIds
        pPBDASIBuffer->argbData[1] = bSectionNumber;
        if (bTableId == 0x4e)
        {
            pPBDASIBuffer->argbData[2] = bVersionNr;
        }
        else
        {
            pPBDASIBuffer->argbData[2] = bVersionNr + 0x80;
        }
    }
    else //schedule
    {
        pPBDASIBuffer->argbData[0] = 0x80;
        if (!m_TotalPBDASISectionsSchedule)
        {
            m_TotalPBDASISectionsSchedule = (bLastSectionNumber + 1) * m_wNrOfServices * (bLastTableId - 0x4F);
        }
        //PBDA version is based on each Service, while EIT is devided into Sections and tableIds
        pPBDASIBuffer->argbData[1] = bSectionNumber;
        pPBDASIBuffer->argbData[2] = bVersionNr + ((bTableId - 0x50)<<5); 
    }

    //PBDA version is based on each Service, while EIT is devided into Sections and tableIds
    pPBDASIBuffer->argbData[1] = bSectionNumber;  
    pPBDASIBuffer->argbData[2] = bVersionNr;

    //convert to ServiceIDX
    pPBDASIBuffer->argbData[3] = pPBDASIBuffer->argbData[4] = 0;
    *(WORD *)(&pPBDASIBuffer->argbData[5]) = wNetworkId;
    *(WORD *)(&pPBDASIBuffer->argbData[7]) = wTSId;
    *(WORD *)(&pPBDASIBuffer->argbData[9]) = wServiceId;

    WORD *pEventCounter = (WORD *)(&pPBDASIBuffer->argbData[11]); //Eventcounter, have to be swapped at the end
    *pEventCounter = 0;

    wTableLength -= 15; //EIT loop length (without Header and CRC32)
    wAvailableBuffer -= 36; //the header minimum is 13 + 23 (without descriptors)
    while ((wUsedBufferSize < wAvailableBuffer) && (wTableLength > 10))
    {
        WORD wEITEventId = *(WORD *)pBuffer;//unswapped EventId
        pBuffer += 2; wTableLength -= 2;

        LARGE_INTEGER  liStartTime;
        ConvertEITStartTimeToUTC(pBuffer, &pPBDASIBuffer->argbData[wUsedBufferSize], &liStartTime);
        wUsedBufferSize += 8;
        pBuffer += 5; wTableLength -= 5;

        if (bTableId == 0x4e)
        {
            BYTE bRunningStatus = *(pBuffer + 3) >> 5;
            if (ConvertEITDurationToSeconds(pBuffer, &pPBDASIBuffer->argbData[wUsedBufferSize],liStartTime,bRunningStatus) == true)
            {
                m_bVersionExtend++;
                if (!m_bVersionExtend) m_bVersionExtend++;
                pPBDASIBuffer->argbData[2] += m_bVersionExtend; //extend the version to pass the insertion
            }
            else
            {
                if (bVersionCheckResultNow == true)
                {
                    ReleasePBDASIData(pPBDASIBuffer);//delete the allocated buffer because the version is identical and no running extend
                    return;
                }
            }
        }
        else
        {
            ConvertEITDurationToSeconds(pBuffer, &pPBDASIBuffer->argbData[wUsedBufferSize],liStartTime,0);
        }
        wUsedBufferSize += 4;
        pBuffer += 3; wTableLength -= 3;

        ConvertEITEventIDToPBDAEventId(wEITEventId, &pPBDASIBuffer->argbData[wUsedBufferSize]);
        wUsedBufferSize += 8;

        WORD wDescriptorLength = (*pBuffer) & 0x0F;
        wDescriptorLength = wDescriptorLength << 8;
        pBuffer++; wTableLength --;
        wDescriptorLength += *pBuffer;
        pBuffer++; wTableLength --;

        //copy EIT Descriptors into PBDA Buffer
        BYTE *pDescriptor_loop_count = &pPBDASIBuffer->argbData[wUsedBufferSize];
        wUsedBufferSize++;
        *pDescriptor_loop_count = 0;
        LONG lDescriptorBufferSize = wDescriptorLength;
        if (wDescriptorLength > wAvailableBuffer)
        {
            lDescriptorBufferSize = 0;
            DbgPrint("Descriptor Length=%d Error in PBDA Converter\n",lDescriptorBufferSize);
        }
        BYTE *pDescriptorStart = pBuffer;
        while (lDescriptorBufferSize > 2) 
        {
            BYTE bDescTag = *pBuffer;
            pBuffer++;
            BYTE bDescLen = *pBuffer;
            pBuffer++;
            WORD wCurrentDescriptorLength = bDescLen + 2;

            switch (bDescTag)
            {
                case 0x4D:  //Short_event_descriptor
                case 0x4E: 	//Extended_event_descriptor
                case 0x50: 	//Component_descriptor
                case 0x54: 	//Content_descriptor
                case 0x55: 	//Parental_rating_descriptor
                case 0x5E: 	//Multilingual_component_descriptor
                case 0x87: 	//Content_advisory_descriptor
                    if ((wUsedBufferSize + wCurrentDescriptorLength) < wAvailableBuffer)
                    {
                        memcpy(&pPBDASIBuffer->argbData[wUsedBufferSize],(pBuffer - 2),wCurrentDescriptorLength);
                        (*pDescriptor_loop_count)++;
                        wUsedBufferSize += wCurrentDescriptorLength;
                    }
                    else
                    {
                        DbgPrint("Buffer Error: EIT %x Descriptor length=%d cannot be copy to PBDA-SI Buffer\n",bDescTag, wCurrentDescriptorLength);
                    }
                    break;
                default:
                    break;
            }
            pBuffer += bDescLen; // move to next descriptor
            lDescriptorBufferSize -= wCurrentDescriptorLength;
        }
        wTableLength -= wDescriptorLength;

        if (lDescriptorBufferSize != 0)
        {
            pBuffer = pDescriptorStart + wDescriptorLength;
            DbgPrint("EIT %x Table Descriptor Length Error by %d Bytes!\n",bTableId, lDescriptorBufferSize);
        }
        
        (*pEventCounter)++;

        pPBDASIBuffer->argbData[wUsedBufferSize] = 0;//pbdaeit_descriptors_loop_count
        wUsedBufferSize ++;//add size of pbdaeit_descriptors_loop_count
        pPBDASIBuffer->argbData[wUsedBufferSize] = 1;//pbdaeit_descriptors_loop_count
        wUsedBufferSize ++;//add size of pbdaeit_descriptors_loop_count

        //todo: Inject pbda descriptor for Entitlement token descriptor 0x80
        pPBDASIBuffer->argbData[wUsedBufferSize] = 0x80;
        wUsedBufferSize++;

        pPBDASIBuffer->argbData[wUsedBufferSize] = 0;
        wUsedBufferSize ++;
        pPBDASIBuffer->argbData[wUsedBufferSize] = 8;
        wUsedBufferSize ++;

        *(DWORD *)(&pPBDASIBuffer->argbData[wUsedBufferSize]) = bTableId;
        wUsedBufferSize += 4;
        *(DWORD *)(&pPBDASIBuffer->argbData[wUsedBufferSize]) = wServiceId;
        wUsedBufferSize += 4;


    }
    // swap EventCounter
    *pEventCounter = swap16(*pEventCounter);
    
    pPBDASIBuffer->ulDataLength = wUsedBufferSize;

    InsertTailList(&m_PBDASIList, &pPBDASIBuffer->ListEntry);

    m_CurrentPBDASISections++;

}

bool CTsTimestamp::GetPBDASIGuideData(PPBDA_SI_BUFFER *pPBDABuffer)
{
//read the PBDA data from the Buffer and take it out of the cache, the caller has to release the memory 
    PLIST_ENTRY pEntry;

    if (!IsListEmpty(&m_PBDASIList))
    {

        pEntry = RemoveHeadList(&m_PBDASIList);
        *pPBDABuffer = CONTAINING_RECORD(pEntry,PBDA_SI_BUFFER, ListEntry);
        if (m_CurrentPBDASISections) m_CurrentPBDASISections--;
            
        return true;
    }

    return false;
}

void CTsTimestamp::GetNrOfTables(ULONG *pulNrOfCached, ULONG *pulTotalSections)
{
//return the number of cached EIT sections, and the number of all sections
    if ((!pulNrOfCached) || (!pulTotalSections))
    {
        return;
    }
    *pulNrOfCached = m_CurrentPBDASISections;
    *pulTotalSections = m_TotalPBDASISectionsSchedule + m_TotalPBDASISectionsPF;
}

PBDA_SI_BUFFER * CTsTimestamp::AllocatePBDASIData( ULONG ulEITLength )
{
    if (m_CurrentPBDASISections > 512)//maximum Cache size about 512, should be enough to cache 2 seconds
    {
        //DbgPrint("EIT Cache Full!\n");
        return NULL;
    }
    
    ULONG ulPBDASIBufferLength = ulEITLength * 2;  //The pbda buffer vs eit is at max twice the length
    PBDA_SI_BUFFER *pPBDASIBuffer = (PBDA_SI_BUFFER *) ExAllocatePoolWithTag(NonPagedPool,ulPBDASIBufferLength , MS_SAMPLE_TUNER_POOL_TAG);

    if (pPBDASIBuffer == NULL)
    {
        return NULL;         
    }

    pPBDASIBuffer->ulDataLength = ulPBDASIBufferLength;
    return pPBDASIBuffer;
}

void CTsTimestamp::ReleasePBDASIData(PBDA_SI_BUFFER *pPBDASIData)
{
    if (pPBDASIData)
    {
        ExFreePoolWithTag(pPBDASIData, MS_SAMPLE_TUNER_POOL_TAG);
    }
}

void CTsTimestamp::CleanUpPBDASIList()
{
    PLIST_ENTRY pEntry;
    PBDA_SI_BUFFER *pPBDASIBuffer;

    while (!IsListEmpty(&m_PBDASIList))
    {
        pEntry = RemoveHeadList(&m_PBDASIList);
        pPBDASIBuffer = CONTAINING_RECORD(pEntry,PBDA_SI_BUFFER, ListEntry);
        ReleasePBDASIData(pPBDASIBuffer);
    }
    m_TotalPBDASISectionsPF = 0;
    m_TotalPBDASISectionsSchedule = 0;
    m_CurrentPBDASISections = 0;
}


void CTsTimestamp::CleanUpSIDList()
{
    PLIST_ENTRY pEntry;
    SERVICEID_EIT_VERSION *pVersionBuffer;

    while (!IsListEmpty(&m_ServiceIdList))
    {
        pEntry = RemoveHeadList(&m_ServiceIdList);
        pVersionBuffer = CONTAINING_RECORD(pEntry,SERVICEID_EIT_VERSION, ListEntry);
        ExFreePoolWithTag(pVersionBuffer, MS_SAMPLE_TUNER_POOL_TAG);
    }
}

//check the EIT Version , return true if already processed, return false if new version
bool CTsTimestamp::CheckEITVersion(WORD wServiceID, BYTE bTableId, BYTE bSectionNr, BYTE bVersion, bool &bVersionCheckResultNow)
{
    PLIST_ENTRY pEntry = NULL;
    SERVICEID_EIT_VERSION *pVersionBuffer;
    
    WORD wVersionIndex = bTableId-0x4E;
    wVersionIndex = (wVersionIndex << 8) + bSectionNr;

    if (!IsListEmpty(&m_ServiceIdList))
    {
        pEntry = m_ServiceIdList.Flink;

        while (pEntry != NULL) 
        {
            pVersionBuffer = CONTAINING_RECORD(pEntry,SERVICEID_EIT_VERSION, ListEntry);
            if (pVersionBuffer->wServiceId == wServiceID)
            {
              
                if (pVersionBuffer->Version[wVersionIndex] == bVersion)
                {
                    //DbgPrint("->EITVersion duplicate Service %x Version %d",wServiceID,bVersion );
                    bVersionCheckResultNow = true;
                    if (bTableId == 0x4E)
                    {
                        return false; //no version check at this point, because a running status may extend
                    }
                    else
                    {
                        return true; //same version
                    }
                }
                else
                {
                    //DbgPrint("--->EITVersion changed Service %x Version %d",wServiceID,bVersion );
                    pVersionBuffer->Version[wVersionIndex] = bVersion; 
                }
                return false;
            }
            if (pEntry->Flink == &m_ServiceIdList)
            {
                pEntry = NULL;
            }
            else
            {
                pEntry = pEntry->Flink;
            }
        }
    }    
    SERVICEID_EIT_VERSION *pNewSIDBuffer = (SERVICEID_EIT_VERSION *) ExAllocatePoolWithTag(NonPagedPool,sizeof(SERVICEID_EIT_VERSION) , MS_SAMPLE_TUNER_POOL_TAG);

    if (pNewSIDBuffer == NULL)
    {
        DbgPrint("Out of memory in Version check!\n");
        return true;//no memory, through away         
    }
    pNewSIDBuffer->wServiceId = wServiceID;
    memset(pNewSIDBuffer->Version,255,sizeof(BYTE) * MAX_EIT_VERSION_ARRAY);
    pNewSIDBuffer->Version[wVersionIndex] = bVersion;
    InsertTailList(&m_ServiceIdList, &pNewSIDBuffer->ListEntry);
    //DbgPrint("EITVersion insert Service %x Version %d",wServiceID,bVersion );
    return false;
}

bool CTsTimestamp::ConvertEITStartTimeToUTC(BYTE *pEITStartTime, BYTE *pUTCStartTime, LARGE_INTEGER  *pliStartTime)
{
//start_time: This value is the start time of the event expressed in seconds since January 1, 1970 UTC
	DWORD dwMJD = (*pEITStartTime) << 8;
    pEITStartTime++;
    dwMJD += *pEITStartTime;
    pEITStartTime++;

	WORD wYear = (WORD)((dwMJD * 100 - 1507820)/36525);//Year'
	BYTE bMonth = (BYTE)(((dwMJD * 10 - 149561 - 10*(wYear * 36525 / 100)) *1000/306001)&0xFF);// Month'
	BYTE bDay = (BYTE)((dwMJD - 14956 - (wYear * 36525)/100 - (bMonth * 306001) / 10000)&0xFF);

	//modify year and month based on ETS300468 conversion
	if (bMonth==14 || bMonth==15)
	{
		wYear+=1;//year=year' + 1 + 1900
		bMonth-=13;//Month=Month'- 13
	}
	else
	{
		//year=year' + 1900
		bMonth-=1;//Month=Month' - 1
	}
    wYear += 1900;

    //convert bcd time to decimal
    CSHORT iHour = (*pEITStartTime & 0xF) + (*pEITStartTime >> 4)*10; //decimal hour
    pEITStartTime++;

    CSHORT iMinute = (*pEITStartTime & 0xF) + (*pEITStartTime >> 4)*10; //decimal minute
    pEITStartTime++;

    CSHORT iSeconds = (*pEITStartTime & 0xF) + (*pEITStartTime >> 4)*10; //decimal seconds
    pEITStartTime++;

    TIME_FIELDS tm_fields;
    LARGE_INTEGER  liEITTime;

    tm_fields.Year = wYear;
    tm_fields.Month = bMonth;
    tm_fields.Day = bDay;
    tm_fields.Hour = iHour;
    tm_fields.Minute = iMinute;
    tm_fields.Second = iSeconds;
    tm_fields.Milliseconds = 0;
    tm_fields.Weekday = 0;

    if (RtlTimeFieldsToTime(&tm_fields, &liEITTime) == FALSE)
    {
        return false;
    }
    
    LARGE_INTEGER  ullElapsedSeconds;

    if (pliStartTime)
    {
        pliStartTime->QuadPart = liEITTime.QuadPart;
    }

    ullElapsedSeconds.QuadPart = liEITTime.QuadPart - m_liTime1970.QuadPart;
    ullElapsedSeconds.QuadPart = ullElapsedSeconds.QuadPart / 10000000;//calc into seconds 

    // copy and swap ulElapsedSeconds to pUTCStartTime (8 Byte)
    *(DWORD *)pUTCStartTime = swap32(ullElapsedSeconds.HighPart);
    pUTCStartTime += 4;
    *(DWORD *)pUTCStartTime = swap32(ullElapsedSeconds.LowPart);
    pUTCStartTime += 4;

    return true;
}

bool CTsTimestamp::ConvertEITDurationToSeconds(BYTE *pEITDuration, BYTE *pSecDuration, LARGE_INTEGER liStartTime, BYTE bRunningStatus)
{
    //convert bcd time to decimal
    bool breturn = false;
    int iHour = (*pEITDuration & 0xF) + (*pEITDuration >> 4)*10; //decimal hour
    pEITDuration++;

    int iMinute = (*pEITDuration & 0xF) + (*pEITDuration >> 4)*10; //decimal minute
    pEITDuration++;

    int iSeconds = (*pEITDuration & 0xF) + (*pEITDuration >> 4)*10; //decimal seconds
    pEITDuration++;

    UINT32 uiDurationsSeconds = iHour * 3600 + iMinute * 60 + iSeconds;

    if (bRunningStatus == 4) //if the now running program is running it have to extend the duration
    {
        LARGE_INTEGER liSystemTime;
        KeQuerySystemTime(&liSystemTime);
        LARGE_INTEGER liEndTimeToSystemTime;
        liEndTimeToSystemTime.QuadPart = liStartTime.QuadPart/10000000 + uiDurationsSeconds - 120 - liSystemTime.QuadPart/10000000; 
        //at least one minute before end we signal an extension of at least one minute 
        if (liEndTimeToSystemTime.QuadPart < 0)
        {
            liEndTimeToSystemTime.QuadPart = (-1)*liEndTimeToSystemTime.QuadPart/60; //add only one minutes in granularity
            uiDurationsSeconds += liEndTimeToSystemTime.LowPart * 60;
            DbgPrint("Extending a program by %d minutes\n",liEndTimeToSystemTime.LowPart);
            breturn = true;
        }
    }

    *(UINT32 *)pSecDuration = swap32(uiDurationsSeconds);

    return breturn;
}

bool CTsTimestamp::ConvertEITEventIDToPBDAEventId(WORD wEITEventId, BYTE *pPBDAEventId)
{

    *(UINT64 *)pPBDAEventId = 0;
    pPBDAEventId += 6;

    *(WORD *)pPBDAEventId = wEITEventId; //wEventId is already swapped
    return true;
}


