/***
*io.h - declarations for low-level file handling and I/O functions
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file contains the function declarations for the low-level
*       file handling and I/O functions.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_IO
#define _INC_IO

#include <crtdefs.h>

#ifdef  _MSC_VER
/*
 * Currently, all MS C compilers for Win32 platforms default to 8 byte
 * alignment.
 */
#pragma pack(push,8)
#endif  /* _MSC_VER */

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef _FSIZE_T_DEFINED
typedef unsigned long _fsize_t; /* Could be 64 bits for Win32 */
#define _FSIZE_T_DEFINED
#endif

#ifndef _FINDDATA_T_DEFINED

struct _finddata32_t {
        unsigned    attrib;
        __time32_t  time_create;    /* -1 for FAT file systems */
        __time32_t  time_access;    /* -1 for FAT file systems */
        __time32_t  time_write;
        _fsize_t    size;
        char        name[260];
};

#if     _INTEGRAL_MAX_BITS >= 64

struct _finddata32i64_t {
        unsigned    attrib;
        __time32_t  time_create;    /* -1 for FAT file systems */
        __time32_t  time_access;    /* -1 for FAT file systems */
        __time32_t  time_write;
        __int64     size;
        char        name[260];
};

struct _finddata64i32_t {
        unsigned    attrib;
        __time64_t  time_create;    /* -1 for FAT file systems */
        __time64_t  time_access;    /* -1 for FAT file systems */
        __time64_t  time_write;
        _fsize_t    size;
        char        name[260];
};

struct __finddata64_t {
        unsigned    attrib;
        __time64_t  time_create;    /* -1 for FAT file systems */
        __time64_t  time_access;    /* -1 for FAT file systems */
        __time64_t  time_write;
        __int64     size;
        char        name[260];
};

#endif


struct _finddata_t {
        unsigned    attrib;
        time_t      time_create;    /* -1 for FAT file systems */
        time_t      time_access;    /* -1 for FAT file systems */
        time_t      time_write;
        _fsize_t    size;
        char        name[260];
};

#if     _INTEGRAL_MAX_BITS >= 64

struct _finddatai64_t {
        unsigned    attrib;
        time_t      time_create;    /* -1 for FAT file systems */
        time_t      time_access;    /* -1 for FAT file systems */
        time_t      time_write;
        __int64     size;
        char        name[260];
};

#endif

#define _FINDDATA_T_DEFINED
#endif /* _FINDDATA_T_DEFINED */

#ifndef _WFINDDATA_T_DEFINED

struct _wfinddata32_t {
        unsigned    attrib;
        __time32_t  time_create;    /* -1 for FAT file systems */
        __time32_t  time_access;    /* -1 for FAT file systems */
        __time32_t  time_write;
        _fsize_t    size;
        wchar_t     name[260];
};

#if     _INTEGRAL_MAX_BITS >= 64

struct _wfinddata32i64_t {
        unsigned    attrib;
        __time32_t  time_create;    /* -1 for FAT file systems */
        __time32_t  time_access;    /* -1 for FAT file systems */
        __time32_t  time_write;
        __int64     size;
        wchar_t     name[260];
};

struct _wfinddata64i32_t {
        unsigned    attrib;
        __time64_t  time_create;    /* -1 for FAT file systems */
        __time64_t  time_access;    /* -1 for FAT file systems */
        __time64_t  time_write;
        _fsize_t    size;
        wchar_t     name[260];
};

struct _wfinddata64_t {
        unsigned    attrib;
        __time64_t  time_create;    /* -1 for FAT file systems */
        __time64_t  time_access;    /* -1 for FAT file systems */
        __time64_t  time_write;
        __int64     size;
        wchar_t     name[260];
};

#endif


struct _wfinddata_t {
        unsigned    attrib;
        time_t      time_create;    /* -1 for FAT file systems */
        time_t      time_access;    /* -1 for FAT file systems */
        time_t      time_write;
        _fsize_t    size;
        wchar_t     name[260];
};

#if     _INTEGRAL_MAX_BITS >= 64

struct _wfinddatai64_t {
        unsigned    attrib;
        time_t      time_create;    /* -1 for FAT file systems */
        time_t      time_access;    /* -1 for FAT file systems */
        time_t      time_write;
        __int64     size;
        wchar_t     name[260];
};

#endif


#define _WFINDDATA_T_DEFINED
#endif /* _WFINDDATA_T_DEFINED */

/* File attribute constants for _findfirst() */

#define _A_NORMAL       0x00    /* Normal file - No read/write restrictions */
#define _A_RDONLY       0x01    /* Read only file */
#define _A_HIDDEN       0x02    /* Hidden file */
#define _A_SYSTEM       0x04    /* System file */
#define _A_SUBDIR       0x10    /* Subdirectory */
#define _A_ARCH         0x20    /* Archive file */

/* function prototypes */

#ifndef _SIZE_T_DEFINED
#ifdef  _WIN64
typedef unsigned __int64    size_t;
#else
typedef _W64 unsigned int   size_t;
#endif
#define _SIZE_T_DEFINED
#endif

_CRTIMP __checkReturn int __cdecl _access(__in_z const char * _Filename, __in int _AccessMode);
_CRTIMP __checkReturn_wat errno_t __cdecl _access_s(__in_z const char * _Filename, __in int _AccessMode);
_CRTIMP __checkReturn int __cdecl _chmod(__in_z const char * _Filename, __in int _Mode);
/* note that the newly added _chsize_s takes a 64 bit value */
_CRTIMP __checkReturn int __cdecl _chsize(__in int _FileHandle, __in long _Size);
_CRTIMP __checkReturn_wat errno_t __cdecl _chsize_s(__in int _FileHandle,__in __int64 _Size);
_CRTIMP __checkReturn_opt int __cdecl _close(__in int _FileHandle);
_CRTIMP __checkReturn_opt int __cdecl _commit(__in int _FileHandle);
_CRT_INSECURE_DEPRECATE(_sopen_s) _CRTIMP __checkReturn int __cdecl _creat(__in_z const char * _Filename, __in int _PermissionMode);
_CRTIMP __checkReturn int __cdecl _dup(__in int _FileHandle);
_CRTIMP __checkReturn int __cdecl _dup2(__in int _FileHandleSrc, __in int _FileHandleDst);
_CRTIMP __checkReturn int __cdecl _eof(__in int _FileHandle);
_CRTIMP __checkReturn long __cdecl _filelength(__in int _FileHandle);
_CRTIMP __checkReturn intptr_t __cdecl _findfirst32(__in_z const char * _Filename, __out struct _finddata32_t * _FindData);
_CRTIMP __checkReturn int __cdecl _findnext32(__in intptr_t _FindHandle, __out struct _finddata32_t * _FindData);
_CRTIMP __checkReturn_opt int __cdecl _findclose(__in intptr_t _FindHandle);
_CRTIMP __checkReturn int __cdecl _isatty(__in int _FileHandle);
_CRTIMP int __cdecl _locking(__in int _FileHandle, __in int _LockMode, __in long _NumOfBytes);
_CRTIMP __checkReturn_opt long __cdecl _lseek(__in int _FileHandle, __in long _Offset, __in int _Origin);
_CRTIMP __checkReturn_wat errno_t __cdecl _mktemp_s(__inout_ecount_z(_Size) char * _TemplateName, __in size_t _Size);
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(errno_t, _mktemp_s, __inout_ecount_z(_Size) char, _TemplateName)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(__checkReturn char *, __RETURN_POLICY_DST, _CRTIMP, _mktemp, __inout_z char, _TemplateName)
_CRTIMP __checkReturn int __cdecl _pipe(__inout_ecount(2) int * _PtHandles, __in unsigned int _PipeSize, __in int _TextMode);
_CRTIMP __checkReturn int __cdecl _read(__in int _FileHandle, __out_bcount(_MaxCharCount) void * _DstBuf, __in unsigned int _MaxCharCount);

#ifndef _CRT_DIRECTORY_DEFINED
#define _CRT_DIRECTORY_DEFINED
_CRTIMP __checkReturn int __cdecl remove(__in_z const char * _Filename);
_CRTIMP __checkReturn int __cdecl rename(__in_z const char * _OldFilename, __in_z const char * _NewFilename);
_CRTIMP __checkReturn int __cdecl _unlink(__in_z const char * _Filename);
#if !__STDC__
_CRT_NONSTDC_DEPRECATE(_unlink) _CRTIMP __checkReturn int __cdecl unlink(__in_z const char * _Filename);
#endif
#endif

_CRTIMP __checkReturn int __cdecl _setmode(__in int _FileHandle, __in int _Mode);
_CRTIMP __checkReturn long __cdecl _tell(__in int _FileHandle);
_CRT_INSECURE_DEPRECATE(_umask_s) _CRTIMP int __cdecl _umask(__in int _Mode);
_CRTIMP __checkReturn_wat errno_t __cdecl _umask_s(__in int _NewMode, __out int * _OldMode);
_CRTIMP int __cdecl _write(__in int _FileHandle, __in_bcount(_MaxCharCount) const void * _Buf, __in unsigned int _MaxCharCount);

_CRTIMP intptr_t __cdecl _findfirst(const char *, struct _finddata_t *);
_CRTIMP int __cdecl _findnext(intptr_t, struct _finddata_t *);

#if     _INTEGRAL_MAX_BITS >= 64
_CRTIMP __checkReturn __int64 __cdecl _filelengthi64(__in int _FileHandle);
_CRTIMP __checkReturn intptr_t __cdecl _findfirst32i64(__in_z const char * _Filename, __out struct _finddata32i64_t * _FindData);
_CRTIMP __checkReturn intptr_t __cdecl _findfirst64i32(__in_z const char * _Filename, __out struct _finddata64i32_t * _FindData);
_CRTIMP __checkReturn intptr_t __cdecl _findfirst64(__in_z const char * _Filename, __out struct __finddata64_t * _FindData);
_CRTIMP __checkReturn int __cdecl _findnext32i64(__in intptr_t _FindHandle, __out struct _finddata32i64_t * _FindData);
_CRTIMP __checkReturn int __cdecl _findnext64i32(__in intptr_t _FindHandle, __out struct _finddata64i32_t * _FindData);
_CRTIMP __checkReturn int __cdecl _findnext64(__in intptr_t _FindHandle, __out struct __finddata64_t * _FindData);
_CRTIMP __checkReturn_opt __int64 __cdecl _lseeki64(__in int _FileHandle, __in __int64 _Offset, __in int _Origin);
_CRTIMP __checkReturn __int64 __cdecl _telli64(__in int _FileHandle);

_CRTIMP intptr_t __cdecl _findfirsti64(const char *, struct _finddatai64_t *);
_CRTIMP int __cdecl _findnexti64(intptr_t, struct _finddatai64_t *);

#endif

_CRTIMP __checkReturn_wat errno_t __cdecl _sopen_s(__out int * _FileHandle, __in_z const char * _Filename,__in int _OpenFlag, __in int _ShareFlag, __in int _PermissionMode);

_CRT_INSECURE_DEPRECATE(_sopen_s) _CRTIMP __checkReturn int __cdecl _open(__in_z const char * _Filename, __in int _OpenFlag, ...);
_CRT_INSECURE_DEPRECATE(_sopen_s) _CRTIMP __checkReturn int __cdecl _sopen(__in_z const char * _Filename, __in int _OpenFlag, int _ShareFlag, ...);

#ifndef _WIO_DEFINED

/* wide function prototypes, also declared in wchar.h  */

_CRTIMP __checkReturn int __cdecl _waccess(__in_z const wchar_t * _Filename, __in int _AccessMode);
_CRTIMP __checkReturn_wat errno_t __cdecl _waccess_s(__in_z const wchar_t * _Filename, __in int _AccessMode);
_CRTIMP __checkReturn int __cdecl _wchmod(__in_z const wchar_t * _Filename, __in int _Mode);
_CRT_INSECURE_DEPRECATE(_wsopen_s) _CRTIMP __checkReturn int __cdecl _wcreat(__in_z const wchar_t * _Filename, __in int _PermissionMode);
_CRTIMP __checkReturn intptr_t __cdecl _wfindfirst32(__in_z const wchar_t * _Filename, __out struct _wfinddata32_t * _FindData);
_CRTIMP __checkReturn int __cdecl _wfindnext32(__in intptr_t _FindHandle, __out struct _wfinddata32_t * _FindData);
_CRTIMP __checkReturn int __cdecl _wunlink(__in_z const wchar_t * _Filename);
_CRTIMP __checkReturn int __cdecl _wrename(__in_z const wchar_t * _NewFilename, __in_z const wchar_t * _OldFilename);
_CRTIMP errno_t __cdecl _wmktemp_s(__inout_ecount_z(_SizeInWords) wchar_t * _TemplateName, __in size_t _SizeInWords);
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(errno_t, _wmktemp_s, __inout_ecount_z(_Size) wchar_t, _TemplateName)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(wchar_t *, __RETURN_POLICY_DST, _CRTIMP, _wmktemp, __inout_z wchar_t, _TemplateName)

_CRTIMP intptr_t __cdecl _wfindfirst(const wchar_t *, struct _wfinddata_t *);
_CRTIMP int __cdecl _wfindnext(intptr_t, struct _wfinddata_t *);

#if     _INTEGRAL_MAX_BITS >= 64
_CRTIMP __checkReturn intptr_t __cdecl _wfindfirst32i64(__in_z const wchar_t * _Filename, __out struct _wfinddata32i64_t * _FindData);
_CRTIMP __checkReturn intptr_t __cdecl _wfindfirst64i32(__in_z const wchar_t * _Filename, __out struct _wfinddata64i32_t * _FindData);
_CRTIMP __checkReturn intptr_t __cdecl _wfindfirst64(__in_z const wchar_t * _Filename, __out struct _wfinddata64_t * _FindData);
_CRTIMP __checkReturn int __cdecl _wfindnext32i64(__in intptr_t _FindHandle, __out struct _wfinddata32i64_t * _FindData);
_CRTIMP __checkReturn int __cdecl _wfindnext64i32(__in intptr_t _FindHandle, __out struct _wfinddata64i32_t * _FindData);
_CRTIMP __checkReturn int __cdecl _wfindnext64(__in intptr_t _FindHandle, __out struct _wfinddata64_t * _FindData);

_CRTIMP intptr_t __cdecl _wfindfirsti64(const wchar_t *, struct _wfinddatai64_t *);
_CRTIMP int __cdecl _wfindnexti64(intptr_t, struct _wfinddatai64_t *);

#endif

_CRTIMP __checkReturn_wat errno_t __cdecl _wsopen_s(__out int * _FileHandle, __in_z const wchar_t * _Filename, __in int _OpenFlag, __in int _ShareFlag, __in int _PermissionFlag);

_CRTIMP __checkReturn int __cdecl _wopen(__in_z const wchar_t * _Filename, __in int _OpenFlag, __in int _ModeFlag);
_CRTIMP int __cdecl _wsopen(__in_z const wchar_t * _Filename, __in int _OpenFlag, __in int _ShareFlag, __in int _ModeFlag);

#define _WIO_DEFINED
#endif /* _WIO_DEFINED */

int  __cdecl __lock_fhandle(__in int _Filehandle);
void __cdecl _unlock_fhandle(__in int _Filehandle);


_CRTIMP intptr_t __cdecl _get_osfhandle(__in int _FileHandle);
_CRTIMP int __cdecl _open_osfhandle(__in intptr_t _OSFileHandle, __in int _Flags);

#if     !__STDC__

/* Non-ANSI names for compatibility */

#pragma warning(push)
#pragma warning(disable: 4141) /* Using deprecated twice */ 
_CRT_NONSTDC_DEPRECATE(_access) _CRTIMP __checkReturn int __cdecl access(__in_z const char * _Filename, __in int _AccessMode);
_CRT_NONSTDC_DEPRECATE(_chmod) _CRTIMP __checkReturn int __cdecl chmod(__in_z const char * _Filename, int _AccessMode);
_CRT_NONSTDC_DEPRECATE(_chsize) _CRTIMP __checkReturn int __cdecl chsize(__in int _FileHandle, __in long _Size);
_CRT_NONSTDC_DEPRECATE(_close) _CRTIMP __checkReturn_opt int __cdecl close(__in int _FileHandle);
_CRT_NONSTDC_DEPRECATE(_creat) _CRT_INSECURE_DEPRECATE(_sopen_s) _CRTIMP __checkReturn int __cdecl creat(__in_z const char * _Filename, __in int _PermissionMode);
_CRT_NONSTDC_DEPRECATE(_dup) _CRTIMP __checkReturn int __cdecl dup(__in int _FileHandle);
_CRT_NONSTDC_DEPRECATE(_dup2) _CRTIMP __checkReturn int __cdecl dup2(__in int _FileHandleSrc, __in int _FileHandleDst);
_CRT_NONSTDC_DEPRECATE(_eof) _CRTIMP __checkReturn int __cdecl eof(__in int _FileHandle);
_CRT_NONSTDC_DEPRECATE(_filelength) _CRTIMP __checkReturn long __cdecl filelength(__in int _FileHandle);
_CRT_NONSTDC_DEPRECATE(_isatty) _CRTIMP __checkReturn int __cdecl isatty(__in int _FileHandle);
_CRT_NONSTDC_DEPRECATE(_locking) _CRTIMP __checkReturn int __cdecl locking(__in int _FileHandle, __in int _LockMode, __in long _NumOfBytes);
_CRT_NONSTDC_DEPRECATE(_lseek) _CRTIMP __checkReturn_opt long __cdecl lseek(__in int _FileHandle, __in long _Offset, __in int _Origin);
_CRT_NONSTDC_DEPRECATE(_mktemp) _CRT_INSECURE_DEPRECATE(_mktemp_s) _CRTIMP char * __cdecl mktemp(__inout_z char * _TemplateName);
_CRT_NONSTDC_DEPRECATE(_open) _CRT_INSECURE_DEPRECATE(_sopen_s) _CRTIMP __checkReturn int __cdecl open(__in_z const char * _Filename, __in int _OpenFlag, ...);
_CRT_NONSTDC_DEPRECATE(_read) _CRTIMP int __cdecl read(int _FileHandle, __out_bcount(_MaxCharCount) void * _DstBuf, __in unsigned int _MaxCharCount);
_CRT_NONSTDC_DEPRECATE(_setmode) _CRTIMP __checkReturn int __cdecl setmode(__in int _FileHandle, __in int _Mode);
_CRT_NONSTDC_DEPRECATE(_sopen) _CRT_INSECURE_DEPRECATE(_sopen_s) _CRTIMP int __cdecl sopen(const char * _Filename, __in int _OpenFlag, __in int _ShareFlag, ...);
_CRT_NONSTDC_DEPRECATE(_tell) _CRTIMP __checkReturn long __cdecl tell(__in int _FileHandle);
_CRT_NONSTDC_DEPRECATE(_umask) _CRT_INSECURE_DEPRECATE(_umask_s) _CRTIMP int __cdecl umask(__in int _Mode);
_CRT_NONSTDC_DEPRECATE(_write) _CRTIMP __checkReturn int __cdecl write(__in int _Filehandle, __in_bcount(_MaxCharCount) const void * _Buf, __in unsigned int _MaxCharCount);
#pragma warning(pop)

#endif  /* __STDC__ */

#ifdef  __cplusplus
}
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_IO */
/* 88bf0570-3001-4e78-a5f2-be5765546192 */ 

