/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:
    Hw_802_11.c

Abstract:
    Common hardware functions
    
Revision History:
      When        What
    ----------    ----------------------------------------------
    08-01-2005    Created

Notes:

--*/
#include "hw_pcomp.h"
#include "hw_80211.h"


BOOLEAN 
EqualOS(
    OctetString os1,
    OctetString os2)
{
    if( os1.Length!=os2.Length )
        return FALSE;

    if( os1.Length==0 )
        return FALSE;       

    return (memcmp(os1.Octet,os2.Octet,os1.Length)==0) ? TRUE:FALSE;
}

OctetString 
PacketGetElement(
    OctetString packet,
    ElementID ID)
{
    USHORT  offset;
    USHORT  length=packet.Length;
    OctetString ret={0,0};
    UCHAR   temp;

    switch(PacketGetType(packet))
    {
    case Type_Auth:
        offset = 6;
        break;
    case Type_Probe_Req:
        offset = 0;
        break;
    case Type_Beacon:
    case Type_Probe_Rsp:
        offset = 12;
        break;
    case Type_Reasoc_Req:
        offset = 10;
        break;
    case Type_Asoc_Req:
        offset = 4;
        break;
    case Type_Asoc_Rsp:
    case Type_Reasoc_Rsp:
        offset = 6;
        break;
    default:
        return ret;
    }

    offset += sMacHdrLng;

    do
    {
        if( length<offset )
        {
            return ret;
        }

        temp = packet.Octet[offset];
        if( temp == ID )
        {
            ret.Length = packet.Octet[offset+1];
            ret.Octet = packet.Octet + offset +2;
            break;
        }
        else
        {
            temp = packet.Octet[offset+1];
            offset += (temp+2);
        }
    }WHILE (1);

    return ret;
}

void
PacketMakeElement(
    OctetString     *packet,
    ElementID       ID,
    OctetString     info)
{
    PUCHAR      buf = packet->Octet + packet->Length;

    buf[0] = (UCHAR)ID;
    buf[1] = (UCHAR)info.Length;// + 2;
    NdisMoveMemory( buf + 2, info.Octet, info.Length);

    packet->Length += info.Length + 2;
}

void
PacketAppendData(
    OctetString     *packet,
    OctetString     data)
{
    PUCHAR      buf = packet->Octet + packet->Length;

    NdisMoveMemory( buf, data.Octet, data.Length);

    packet->Length = packet->Length + data.Length;
}

BOOLEAN
TimGetBcMcBit(
    OctetString Tim)
{
    return  ((Tim.Octet[2] & 0x01) == 0x01);
}

BOOLEAN 
TimGetAIDBit( 
    OctetString     Tim, 
    unsigned short  AID)
{
    USHORT  FirstStationInTim;
    
    FirstStationInTim = (Tim.Octet[2] & 0xFE) * 8;
    if( AID<FirstStationInTim || AID >= (FirstStationInTim+(Tim.Length-3)*8) )
    {   // Out of the range(too large)
        return FALSE;
    }
    else
    {
        BOOLEAN         result;
        unsigned short  offset;
        unsigned short  offset_byte;
        unsigned char       offset_bit;
        
        offset = AID - FirstStationInTim;
        
        offset_byte = offset >>3;
        offset_bit  = (unsigned char)(offset & 0x7);

        // Look up in the partial virtual bitmap
        result = Tim.Octet[3 + offset_byte]&(0x01<<offset_bit);

        return result;
    }
}

USHORT 
ComputeTxTime(
    __in USHORT   FrameLength,            //3in byte
    __in ULONG    DataRate,               //3in 0.5 Mbits
    __in BOOLEAN  bManagementFrame,
    __in BOOLEAN  bShortPreamble
    )
{
    USHORT          FrameTime;

    if( bManagementFrame || !bShortPreamble)
    {   // long preamble
        FrameTime = (USHORT)(144+48+(FrameLength*8*2/DataRate));        
    }
    else
    {   // Short preamble
        FrameTime = (USHORT)(72+24+(FrameLength*8*2/DataRate));
    }

    if( ( FrameLength*8*2 % DataRate ) != 0 )
        FrameTime ++;
    
    return FrameTime;
}

void
ComputePLCPLength(
    __in USHORT       FrameLength,        //3in byte
    __in ULONG        DataRate,           //3in 0.5 Mbits
    __out USHORT      *FrameTime,
    __out USHORT      *LengExt 
    )
{
    ULONG   FrameLen = FrameLength;

    *LengExt = 0;

    switch( DataRate )
    {
    case 2:
    case 4:
    case 11:
    case 22:
            *FrameTime = (USHORT)( ( (ULONGLONG)FrameLen*8*2 / DataRate ) );

            if( ( FrameLen*8*2 % DataRate ) != 0 )
            {
                *FrameTime = *FrameTime + 1;

                if( (DataRate==22) && ((FrameLen*8*2%DataRate)<=6) )
                    *LengExt = 1;
            }
            break;
    case 12:
    case 18:
    case 24:
    case 36:
    case 48:
    case 72:
    case 96:
    case 108:
        *FrameTime = (USHORT)FrameLen;     
        break;
    }
}

BOOLEAN
CompareSSID(
    PUCHAR ssidbuf1,
    USHORT  ssidlen1,
    PUCHAR ssidbuf2,
    USHORT  ssidlen2
)
{
    if( ( ssidlen1 == ssidlen2 ) &&
        ( memcmp(ssidbuf1, ssidbuf2, ssidlen1) == 0 )
    )
    {
        return TRUE;
    }
    
    return FALSE;
}




