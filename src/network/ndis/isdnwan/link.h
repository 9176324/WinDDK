/*
�����������������������������������������������������������������������������

    (C) Copyright 1998
        All rights reserved.

�����������������������������������������������������������������������������

  Portions of this software are:

    (C) Copyright 1995, 1999 TriplePoint, Inc. -- http://www.TriplePoint.com
        License to use this software is granted under the terms outlined in
        the TriplePoint Software Services Agreement.

    (C) Copyright 1992 Microsoft Corp. -- http://www.Microsoft.com
        License to use this software is granted under the terms outlined in
        the Microsoft Windows Device Driver Development Kit.

�����������������������������������������������������������������������������

@doc INTERNAL Link Link_h

@module Link.h |

    This module defines the interface to the <t NDISLINK_OBJECT>.

@head3 Contents |
@index class,mfunc,func,msg,mdata,struct,enum | Link_h

@end
�����������������������������������������������������������������������������
*/

#ifndef _LINK_H
#define _LINK_H

#define LINK_OBJECT_TYPE        ((ULONG)'L')+\
                                ((ULONG)'I'<<8)+\
                                ((ULONG)'N'<<16)+\
                                ((ULONG)'K'<<24)

/*
�����������������������������������������������������������������������������

    Function prototypes.

*/

VOID LinkLineUp(
    IN PBCHANNEL_OBJECT         pBChannel
    );

VOID LinkLineDown(
    IN PBCHANNEL_OBJECT         pBChannel
    );

VOID LinkLineError(
    IN PBCHANNEL_OBJECT         pBChannel,
    IN ULONG                    Errors
    );

#endif // _LINK_H
