/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name: 

    util.h

Abstract

    Prototype, defines for util functions.

Author:

    Peter Binder (pbinder) 10/29/97

Revision History:
Date     Who       What
-------- --------- ------------------------------------------------------------
10/29/97 pbinder   birth
04/08/98 pbinder   taken from cidiag
04/22/98 pbinder   made into template
04/22/98 pbinder   taken for 1394test
05/04/98 pbinder   taken for win1394
--*/

HANDLE
OpenDevice(
    HANDLE  hWnd,
    PSTR    szDeviceName
    );

VOID
WriteTextToEditControl(
    HWND    hWndEdit, 
    PCHAR   str
    );

// Generic singly linked list routines

typedef struct _LIST_NODE {
    struct _LIST_NODE *pNext;
} LIST_NODE, *PLIST_NODE;

void 
InsertTailList(
    PLIST_NODE head, 
    PLIST_NODE entry
    );

BOOL 
RemoveEntryList(
    PLIST_NODE head, 
    PLIST_NODE entry
    );

void 
InsertHeadList(
    PLIST_NODE head, 
    PLIST_NODE entry
    );

BOOL
IsNodeOnList(
    PLIST_NODE head, 
    PLIST_NODE entry
    );


