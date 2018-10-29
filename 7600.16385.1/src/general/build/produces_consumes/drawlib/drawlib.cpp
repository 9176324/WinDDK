/*++

Copyright (c) 2008  Microsoft Corporation

Module Name:

....drawlib.cpp

Abstract:

This is a minimal source code for drawlib sample

Environment:

    User mode

--*/

#include <drawlib.h>

//
// Call pixlib functions here
//

void SetMetrix(UINT uBytesPerPixel)
{
    CPixel::BytesPerPixel(uBytesPerPixel);

}

