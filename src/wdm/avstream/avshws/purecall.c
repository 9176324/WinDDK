/**************************************************************************

    AVStream Simulated Hardware Sample

    Copyright (c) 2001, Microsoft Corporation.

    File:

        purecall.c

    Abstract:

        This file contains the _purecall stub necessary for virtual function
        usage in drivers on 98 gold.

    History:

        created 9/16/02

**************************************************************************/

/*************************************************

    Function:

        _purecall

    Description:

        _purecall stub for virtual function usage

    Arguments:

        None

    Return Value:

        0

*************************************************/

int __cdecl 
_purecall (
    VOID
    ) 

{
    return 0;
}


