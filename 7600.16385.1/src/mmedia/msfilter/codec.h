//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992-1999 Microsoft Corporation
//
//--------------------------------------------------------------------------;
//
//  codec.h
//
//  Description:
//      This file contains codec definitions, Win16/Win32 compatibility
//      definitions, and instance structure definitions.
//
//
//==========================================================================;

#ifndef _INC_CODEC
#define _INC_CODEC                  // #defined if codec.h has been included

#ifndef RC_INVOKED
#pragma pack(1)                     // assume byte packing throughout
#endif

#ifndef EXTERN_C
#ifdef __cplusplus
    #define EXTERN_C extern "C"
#else
    #define EXTERN_C extern
#endif
#endif

#ifdef __cplusplus
extern "C"                          // assume C declarations for C++
{
#endif


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
//
//  ACM Driver Version:
//
//  the version is a 32 bit number that is broken into three parts as
//  follows:
//
//      bits 24 - 31:   8 bit _major_ version number
//      bits 16 - 23:   8 bit _minor_ version number
//      bits  0 - 15:   16 bit build number
//
//  this is then displayed as follows (in decimal form):
//
//      bMajor = (BYTE)(dwVersion >> 24)
//      bMinor = (BYTE)(dwVersion >> 16) &
//      wBuild = LOWORD(dwVersion)
//
//  VERSION_ACM_DRIVER is the version of this driver.
//  VERSION_MSACM is the version of the ACM that this driver
//  was designed for (requires).
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

#ifdef WIN32
//
//  32-bit versions
//
#if (WINVER >= 0x0400)
 #define VERSION_ACM_DRIVER  MAKE_ACM_VERSION(4,  0, 0)
#else
 #define VERSION_ACM_DRIVER  MAKE_ACM_VERSION(3, 51, 0)
#endif
#define VERSION_MSACM       MAKE_ACM_VERSION(3, 50, 0)

#else
//
//  16-bit versions
//
#define VERSION_ACM_DRIVER  MAKE_ACM_VERSION(2, 1, 0)
#define VERSION_MSACM       MAKE_ACM_VERSION(2, 1, 0)

#endif


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
//
//  Win 16/32 portability stuff...
//
//
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

#ifndef WIN32
    #ifndef FNLOCAL
        #define FNLOCAL     NEAR PASCAL
        #define FNCLOCAL    NEAR _cdecl
        #define FNGLOBAL    FAR PASCAL
        #define FNCGLOBAL   FAR _cdecl
    #ifdef _WINDLL
        #define FNWCALLBACK FAR PASCAL _loadds
        #define FNEXPORT    FAR PASCAL _export
    #else
        #define FNWCALLBACK FAR PASCAL
        #define FNEXPORT    FAR PASCAL _export
    #endif
    #endif

    //
    //
    //
    //
    #ifndef FIELD_OFFSET
    #define FIELD_OFFSET(type, field)    ((LONG)&(((type *)0)->field))
    #endif

    //
    //  based code makes since only in win 16 (to try and keep stuff out of
    //  our fixed data segment...
    //
    #define BCODE           _based(_segname("_CODE"))

    #define HUGE            _huge


    //
    //  stuff for Unicode in Win 32--make it a noop in Win 16
    //
    #ifndef _TCHAR_DEFINED
        #define _TCHAR_DEFINED
        typedef char            TCHAR, *PTCHAR;
        typedef unsigned char   TBYTE, *PTUCHAR;

        typedef PSTR            PTSTR, PTCH;
        typedef LPSTR           LPTSTR, LPTCH;
        typedef LPCSTR          LPCTSTR;
    #endif

    #define TEXT(a)         a
    #define SIZEOF(x)       sizeof(x)
    #define SIZEOFACMSTR(x) sizeof(x)
#else
    #ifndef FNLOCAL
        #define FNLOCAL     _stdcall
        #define FNCLOCAL    _stdcall
        #define FNGLOBAL    _stdcall
        #define FNCGLOBAL   _stdcall
        #define FNWCALLBACK CALLBACK
        #define FNEXPORT    CALLBACK
    #endif

    #ifndef try
    #define try         __try
    #define leave       __leave
    #define except      __except
    #define finally     __finally
    #endif


    //
    //  there is no reason to have based stuff in win 32
    //
    #define BCODE                   CONST

    #define HUGE                    UNALIGNED
    #define HTASK                   HANDLE
    #define SELECTOROF(a)           (a)

    //
    //  for compiling Unicode
    //
    #ifdef UNICODE
        #define SIZEOF(x)   (sizeof(x)/sizeof(WCHAR))
    #else
        #define SIZEOF(x)   sizeof(x)
    #endif
    #define SIZEOFACMSTR(x)	(sizeof(x)/sizeof(WCHAR))
#endif


//
//  This define deals with unaligned data for Win32, and huge data for Win16.
//  Basically, any time you cast an HPBYTE to a non-byte variable (ie long or
//  short), you should cast it to (UNALIGNED *) for WIN32.  This will make
//  sure that there are no alignment problems for Win32 on MIPA and Alpha
//  machines.
//

typedef BYTE HUGE *HPBYTE;

#ifdef WIN32
    typedef short UNALIGNED *HPSHORT;
#else
    typedef short HUGE *HPSHORT;
#endif


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
//
//  misc defines for misc sizes and things...
//
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

//
//  bilingual. this allows the same identifier to be used in resource files
//  and code without having to decorate the id in your code.
//
#ifdef RC_INVOKED
    #define RCID(id)    id
#else
    #define RCID(id)    MAKEINTRESOURCE(id)
#endif


//
//
//
#define SIZEOF_ARRAY(ar)            (sizeof(ar)/sizeof((ar)[0]))


//
//  macros to compute block alignment and convert between samples and bytes
//  of PCM data. note that these macros assume:
//
//      wBitsPerSample  =  8 or 16
//      nChannels       =  1 or 2
//
//  the pwf argument is a pointer to a PCMWAVEFORMAT structure.
//
#define PCM_BLOCKALIGNMENT(pwfx)        (UINT)(((pwfx)->wBitsPerSample >> 3) << ((pwfx)->nChannels >> 1))
#define PCM_AVGBYTESPERSEC(pwfx)        (DWORD)((pwfx)->nSamplesPerSec * (pwfx)->nBlockAlign)
#define PCM_BYTESTOSAMPLES(pwfx, cb)    (DWORD)(cb / PCM_BLOCKALIGNMENT(pwfx))
#define PCM_SAMPLESTOBYTES(pwfx, dw)    (DWORD)(dw * PCM_BLOCKALIGNMENT(pwfx))



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
//
//
//
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

typedef struct tDRIVERINSTANCE
{
    //
    //  although not required, it is suggested that the first two members
    //  of this structure remain as fccType and DriverProc _in this order_.
    //  the reason for this is that the driver will be easier to combine
    //  with other types of drivers (defined by AVI) in the future.
    //
    FOURCC          fccType;        // type of driver: 'audc'
    DRIVERPROC      fnDriverProc;   // driver proc for the instance

    //
    //  the remaining members of this structure are entirely open to what
    //  your driver requires.
    //
    HDRVR           hdrvr;          // driver handle we were opened with
    HINSTANCE       hinst;          // DLL module handle.
    DWORD           vdwACM;         // current version of ACM opening you
    DWORD           fdwOpen;        // flags from open description

    LPDRVCONFIGINFO pdci;
    DWORD           fdwConfig;      // driver instance configuration flags

} DRIVERINSTANCE, *PDRIVERINSTANCE, FAR *LPDRIVERINSTANCE;



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
//
//
//
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;


//
//
//
//
typedef LRESULT (FNGLOBAL *STREAMCONVERTPROC)
(
    LPACMDRVSTREAMINSTANCE  padsi,
    LPACMDRVSTREAMHEADER    padsh
);


//
//
//
//
typedef struct tSTREAMINSTANCE
{
    STREAMCONVERTPROC   fnConvert;  // stream instance conversion proc
    DWORD               fdwConfig;  // stream instance configuration flags


    //
    //  only used on echo filter..
    //
    HPBYTE              hpbHistory;
    DWORD               dwPlace;
    DWORD	        	dwHistoryDone;
} STREAMINSTANCE, *PSTREAMINSTANCE, FAR *LPSTREAMINSTANCE;



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
//
//  resource id's
//
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

#define ICON_ACM_DRIVER             RCID(10)

#define IDS_ACM_DRIVER_SHORTNAME    (1)     // ACMDRIVERDETAILS.szShortName
#define IDS_ACM_DRIVER_LONGNAME     (2)     // ACMDRIVERDETAILS.szLongName
#define IDS_ACM_DRIVER_COPYRIGHT    (3)     // ACMDRIVERDETAILS.szCopyright
#define IDS_ACM_DRIVER_LICENSING    (4)     // ACMDRIVERDETAILS.szLicensing
#define IDS_ACM_DRIVER_FEATURES     (5)     // ACMDRIVERDETAILS.szFeatures

//
//  ACMFILTERTAGDETAILS.szFilterTag
//
//
#define IDS_ACM_DRIVER_TAG_NAME_VOLUME  (20)
#define IDS_ACM_DRIVER_FORMAT_VOLUME    (21)

//
//  ACMFILTERTAGDETAILS.szFilterTag
//
//
#define IDS_ACM_DRIVER_TAG_NAME_ECHO    (40)
#define IDS_ACM_DRIVER_FORMAT_ECHO      (41)

//
//  About dialog box IDs
//
#define IDD_ABOUT                   RCID(100)
#define IDC_STATIC                  -1
#define IDC_ABOUT_CATCHTHEWAVE      1001



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
//
//
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

#ifndef RC_INVOKED
#pragma pack()                      // revert to default packing
#endif

#ifdef __cplusplus
}                                   // end of extern "C" {
#endif

#endif // _INC_CODEC


