/*++

Copyright (c) 2008  Microsoft Corporation

Module Name:

...drawlib.h

Abstract:

This is a minimal header file for pixlib

Environment:

    User mode

--*/

#ifdef __cplusplus

#ifndef __PIXEL_H__
#define __PIXEL_H__

typedef unsigned int UINT;
//
// This is a dummy class that implements a helper for 
// working with pixels.

class CPixel
{
public:
// Allocate helpers

    static UINT BytesPerPixel(UINT Format);
// ...

}; // CPixel

#endif
#endif

