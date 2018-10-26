/*++

Copyright (c) 1997  Microsoft Corporation

Module Name:

    SIOCTL.H

Abstract:

  
    Defines the IOCTL codes that will be used by this driver.  The IOCTL code
    contains a command identifier, plus other information about the device, 
    the type of access with which the file must have been opened, 
    and the type of buffering.

Author:

    Eliyas Yakub - July 1997.

Environment:

    Kernel mode only.

Notes:

Revision History:

--*/

//
// Device type           -- in the "User Defined" range."
//
#define SIOCTL_TYPE 40000
//
// The IOCTL function codes from 0x800 to 0xFFF are for customer use.
//
#define IOCTL_SIOCTL_METHOD_IN_DIRECT \
    CTL_CODE( SIOCTL_TYPE, 0x900, METHOD_IN_DIRECT, FILE_ANY_ACCESS  )

#define IOCTL_SIOCTL_METHOD_OUT_DIRECT \
    CTL_CODE( SIOCTL_TYPE, 0x901, METHOD_OUT_DIRECT , FILE_ANY_ACCESS  )

#define IOCTL_SIOCTL_METHOD_BUFFERED \
    CTL_CODE( SIOCTL_TYPE, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS  )

#define IOCTL_SIOCTL_METHOD_NEITHER \
    CTL_CODE( SIOCTL_TYPE, 0x903, METHOD_NEITHER , FILE_ANY_ACCESS  )


#define DRIVER_FUNC_INSTALL     0x01
#define DRIVER_FUNC_REMOVE      0x02

#define DRIVER_NAME       "SIoctl"
