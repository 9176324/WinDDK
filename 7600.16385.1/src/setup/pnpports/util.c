/** FILE: util.c *********** Module Header ********************************
 *
 *  Ports applet utility library routines. This file contains string,
 *  cursor, SendWinIniChange() routines.
 *
 * History:
 *  15:30 on Thur  25 Apr 1991  -by-  Steve Cathcart   [stevecat]
 *        Took base code from Win 3.1 source
 *  10:30 on Tues  04 Feb 1992  -by-  Steve Cathcart   [stevecat]
 *        Updated code to latest Win 3.1 sources
 *  15:30 on Thur  03 May 1994  -by-  Steve Cathcart   [stevecat]
 *        Increased  MyMessageBox buffers, Restart dialog changes
 *  17:00 on Mon   18 Sep 1995  -by-  Steve Cathcart   [stevecat]
 *        Changes for product update - SUR release NT v4.0
 *  Nov 1997                    -by-  Doron Holan       [stevecat]
 *        Removed obsolete cpl code
 *
 *  Copyright (C) 1990-1995 Microsoft Corporation
 *
 *************************************************************************/
/* Notes -

    Global Functions:

      U T I L I T Y

        ErrMemDlg () - display Memory Error message box
        myatoi () - local implementation of atoi for Unicode strings
        MyMessageBox () - display message to user, with parameters
        SendWinIniChange () - broadcast system change message via USER
        strscan () - Find a string within another string
        StripBlanks () - Strip leading and trailing blanks from a string


    Local Functions:

 */

//==========================================================================
//                                Include files
//==========================================================================

// C Runtime
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// Application specific
#include "ports.h"


#define INT_SIZE_LENGTH   20
#define LONG_SIZE_LENGTH  40


void 
ErrMemDlg(HWND hParent)
{
    MessageBox(hParent, g_szErrMem, g_szPortsApplet,
               MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
}

int 
myatoi(LPCTSTR pszInt)
{
    int   retval;
    TCHAR cSave;

    for (retval = 0; *pszInt; ++pszInt) {
        if ((cSave = (TCHAR) (*pszInt - TEXT('0'))) > (TCHAR) 9)
            break;

        retval = (int) (retval * 10 + (int) cSave);
    }
    return (retval);
}


int 
MyMessageBox(HWND hWnd, 
             DWORD wText, 
             DWORD wCaption, 
             DWORD wType, 
             ...)
{
    TCHAR   szText[4 * PATHMAX], 
            szCaption[2 * PATHMAX];
    int     ival;
    va_list parg;

    va_start(parg, wType);

    if (wText == INITS)
        goto NoMem;

    if (!LoadString(g_hInst, wText, szCaption, ARRAYSIZE(szCaption)))
        goto NoMem;

    StringCchVPrintf(szText, ARRAYSIZE(szText), szCaption, parg);

    if (!LoadString(g_hInst, wCaption, szCaption, ARRAYSIZE(szCaption)))
        goto NoMem;

    if ((ival = MessageBox(hWnd, szText, szCaption, wType)) == 0)
        goto NoMem;

    va_end(parg);

    return ival;

NoMem:
    va_end(parg);
    ErrMemDlg(hWnd);

    return 0;
}

void 
SendWinIniChange(LPCTSTR lpSection)
{
    SendNotifyMessage(HWND_BROADCAST,
                     WM_WININICHANGE, 
                     0L, 
                     (LPARAM) lpSection);
}

LPTSTR 
strscan(__in LPTSTR pszString, 
        LPCTSTR pszTarget)
{
    LPTSTR psz;

    if (psz = _tcsstr( pszString, pszTarget))
        return (psz);
    else
        return (pszString + lstrlen(pszString));
}


///////////////////////////////////////////////////////////////////////////////
//
//  StripBlanks()
//
//   Strips leading and trailing blanks from a string.
//   Alters the memory where the string sits.
//
///////////////////////////////////////////////////////////////////////////////

void 
StripBlanks(__inout LPTSTR pszString)
{
    LPTSTR  pszPosn;
    size_t cchString = lstrlen(pszString)+1;

    //
    //  strip leading blanks
    //

    pszPosn = pszString;

    while (*pszPosn == TEXT(' '))
        pszPosn++;

    if (pszPosn != pszString)
        StringCchCopy(pszString, cchString, pszPosn);

    //
    //  strip trailing blanks
    //

    if ((pszPosn = pszString + lstrlen(pszString)) != pszString) {
       pszPosn = CharPrev(pszString, pszPosn);

       while (*pszPosn == TEXT(' '))
           pszPosn = CharPrev(pszString, pszPosn);

       pszPosn = CharNext(pszPosn);

       *pszPosn = TEXT('\0');
    }
}

