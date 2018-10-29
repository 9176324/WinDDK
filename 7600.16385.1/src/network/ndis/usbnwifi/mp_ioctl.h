
/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    mpioctl.h

Abstract:

    Constants and types to access the NDISPROT driver.
    Users must also include ntddndis.h

Environment:

    User/Kernel mode.

Revision History:

    
--*/

#ifndef __MPIOCTL__H
#define __MPIOCTL__H


#define FSCTL_NDISMP_BASE      FILE_DEVICE_NETWORK

#define _NDISPROT_CTL_CODE(_Function, _Method, _Access)  \
            CTL_CODE(FSCTL_NDISMP_BASE, _Function, _Method, _Access)

#define IOCTL_NDISMP_GET_MAC_DATA   \
            _NDISPROT_CTL_CODE(0x200, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISMP_SET_MAC_DATA   \
            _NDISPROT_CTL_CODE(0x201, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISMP_GET_RF_DATA   \
            _NDISPROT_CTL_CODE(0x202, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISMP_SET_RF_DATA   \
            _NDISPROT_CTL_CODE(0x203, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISMP_GET_CCK_DATA   \
            _NDISPROT_CTL_CODE(0x204, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISMP_SET_CCK_DATA   \
            _NDISPROT_CTL_CODE(0x205, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISMP_GET_OFDM_DATA   \
            _NDISPROT_CTL_CODE(0x206, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISMP_SET_OFDM_DATA   \
            _NDISPROT_CTL_CODE(0x207, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

                                              
    
//
//  Structure to go with IOCTL_NDISPROT_QUERY_OID_VALUE.
//  The Data part is of variable length, determined by
//  the input buffer length passed to DeviceIoControl.
//
typedef struct _NDISMP_GET_DATA
{
    ULONG           Size;
    ULONG           Address;
    UCHAR           Data[sizeof(ULONG)];

} NDISMP_GET_DATA, *PNDISMP_GET_DATA;





#define NT_DEVICE_NAME          L"\\Device\\NdisWifi"
#define DOS_DEVICE_NAME         L"\\DosDevices\\NdisWifi"


#endif

