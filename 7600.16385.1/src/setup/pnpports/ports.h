/** FILE: ports.h ********* Module Header ********************************
 *
 *  Control Panel System applet common definitions, resource ids, typedefs,
 *  external declarations and library routine function prototypes.
 *
 * History:
 *  15:30 on Thur  25 Apr 1991  -by-  Steve Cathcart   [stevecat]
 *        Took base code from Win 3.1 source
 *  10:30 on Tues  04 Feb 1992  -by-  Steve Cathcart   [stevecat]
 *        Updated code to latest Win 3.1 sources
 *  22:00 on Wed   17 Nov 1993  -by-  Steve Cathcart   [stevecat]
 *        Changes for product update
 *  17:00 on Mon   18 Sep 1995  -by-  Steve Cathcart   [stevecat]
 *        Changes for product update - SUR release NT v4.0
 *
 *
 *  Copyright (C) 1990-1995 Microsoft Corporation
 *
 *************************************************************************/
//==========================================================================
//                            Include Files
//==========================================================================

#ifndef PORTS_H
#define PORTS_H

#include <windows.h>
#include <tchar.h>
#include <cfgmgr32.h>
#include <setupapi.h>
#include <regstr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <strsafe.h>
#include "resource.h"

#include "portstr.h"

#define PORTS        4
#define MAXPORTS    32
#define KEYBZ       4096
#define BUFFER_SIZE 81

#define DEF_BAUD    3       //  1200
#define DEF_WORD    4       //  8 bits
#define DEF_PARITY  2       //  None
#define DEF_STOP    0       //  1
#define DEF_PORT    0       //  Null Port
#define DEF_SHAKE   2       //  None
#define PAR_EVEN    0
#define PAR_ODD     1
#define PAR_NONE    2
#define PAR_MARK    3
#define PAR_SPACE   4
#define STOP_1      0
#define STOP_15     1
#define STOP_2      2
#define FLOW_XON    0
#define FLOW_HARD   1
#define FLOW_NONE   2

#define MAX_COM_PORT  COMDB_MIN_PORTS_ARBITRATED   // Maximum number of COM ports NT supports
#define MIN_COM       1                            // Minimum new COM port number

#define POLL_PERIOD_DEFAULT_IDX 1 

//==========================================================================
//                        Definitions
//==========================================================================

//
//  General definitions
//

#define PATHMAX             MAX_PATH



//==========================================================================
//                           Typedefs
//==========================================================================

typedef struct {
    SP_DEVINFO_DATA  DeviceInfoData;

    TCHAR ComName[20];
    TCHAR Settings[20];
    
    ULONG BaseAddress;
} PORT_INFO, *PPORT_INFO;

typedef struct _PORTS_WIZARD_DATA {

    HDEVINFO          DeviceInfoSet;
    PSP_DEVINFO_DATA  pDeviceInfoData;

    ULONG BaseAddress;
    ULONG FirstComNumber;

    ULONG PortsCount;
    PPORT_INFO Ports;

    PUINT UsedComNumbers;
    UINT UsedComNumbersCount;

    BOOL IsMulti;
} PORTS_WIZARD_DATA, *PPORTS_WIZARD_DATA;



//==========================================================================
//                              Macros
//==========================================================================

#ifndef ARRAYSIZE
#define ARRAYSIZE(x)   (sizeof(x) / sizeof(*(x)))
#endif
#define ByteCountOf(x)  ((x) * sizeof(TCHAR))


//==========================================================================
//                         External Declarations
//==========================================================================
//
//  DATA


//
//  exported from cpl.c
//

extern HANDLE g_hInst;
extern TCHAR  g_szClose[ ];         //  "Close" string
extern TCHAR  g_szErrMem[ ];        //  Low memory message
extern TCHAR  g_szPortsApplet[ ];   //  "Ports Control Panel Applet" title
extern TCHAR  g_szNull[];           //  Null string

extern DWORD PollingPeriods[];

extern TCHAR  m_szRegParallelMap[];
extern TCHAR  m_szLPT[];
extern TCHAR  m_szPorts[];
extern TCHAR  m_szPortName[];

//==========================================================================
//                            Function Prototypes
//==========================================================================

//
//  ports.c
//
extern 
VOID
InitStrings(void);

typedef enum {
    PortTypeParallel,
    PortTypeSerial,
    PortTypeOther
} PortType;

extern
BOOL
GetPortType(
    IN HDEVINFO         DeviceInfoSet,
    IN PSP_DEVINFO_DATA DeviceInfoData,
    IN BOOLEAN          DoDrvKeyInstall
    );

//
//  util.c
//

extern 
VOID 
ErrMemDlg(HWND hParent);

extern 
int    
myatoi(LPCTSTR pszInt);

extern 
int    
MyMessageBox(HWND hWnd, 
                         DWORD wText, 
                         DWORD wCaption, 
                         DWORD wType, 
                         ...);

extern 
VOID   
SendWinIniChange(LPCTSTR szSection);

extern 
LPTSTR 
strscan(__in LPTSTR pszString, 
                LPCTSTR pszTarget);

extern 
VOID
StripBlanks(__inout LPTSTR pszString);

#endif // PORTS_H

