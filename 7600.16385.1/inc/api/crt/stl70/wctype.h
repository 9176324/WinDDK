/***
*wctype.h - declarations for wide character functions
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*       Created from wchar.h January 1996 by P.J. Plauger
*
*Purpose:
*       This file contains the types, macros and function declarations for
*       all ctype-style wide-character functions.  They may also be declared in
*       wchar.h.
*       [ISO]
*
*       Note: keep in sync with ctype.h and wchar.h.
*
*       [Public]
*
****/


#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_WCTYPE
#define _INC_WCTYPE

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif

#include <crtdefs.h>

#ifdef  _MSC_VER
#pragma pack(push,_CRT_PACKING)
#endif  /* _MSC_VER */

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#ifndef _WCTYPE_T_DEFINED
typedef unsigned short wint_t;
typedef unsigned short wctype_t;
#define _WCTYPE_T_DEFINED
#endif

#ifndef WEOF
#define WEOF ((wint_t)0xFFFF)
#endif

/*
 * This declaration allows the user access to the ctype look-up
 * array _ctype defined in ctype.obj by simply including ctype.h
 */
#if !defined(_M_CEE_PURE)
/* No data exports in pure code */
_CRTDATA(extern const unsigned short _wctype[];)
#endif /* !defined(_M_CEE_PURE) */

#if !defined(_M_CEE_PURE)
/* No data exports in pure code */
_CRTDATA(extern const unsigned short *_pctype;)
_CRTDATA(extern const wctype_t *_pwctype;)
#endif /* !defined(_M_CEE_PURE) */
_CRTIMP const unsigned short * __cdecl __pctype_func(void);
#ifndef __PCTYPE_FUNC
#ifdef _MT
#define __PCTYPE_FUNC   __pctype_func()
#else
#define __PCTYPE_FUNC  _pctype
#endif  /* _MT */
#endif  /* __PCTYPE_FUNC */


/* set bit masks for the possible character types */

#define _UPPER          0x1     /* upper case letter */
#define _LOWER          0x2     /* lower case letter */
#define _DIGIT          0x4     /* digit[0-9] */
#define _SPACE          0x8     /* tab, carriage return, newline, */
                                /* vertical tab or form feed */
#define _PUNCT          0x10    /* punctuation character */
#define _CONTROL        0x20    /* control character */
#define _BLANK          0x40    /* space char */
#define _HEX            0x80    /* hexadecimal digit */

#define _LEADBYTE       0x8000                  /* multibyte leadbyte */
#define _ALPHA          (0x0100|_UPPER|_LOWER)  /* alphabetic character */


/* Function prototypes */

#ifndef _WCTYPE_DEFINED

/* Character classification function prototypes */
/* also declared in ctype.h */

_CRTIMP int __cdecl iswalpha(wint_t);
_CRTIMP int __cdecl iswupper(wint_t);
_CRTIMP int __cdecl iswlower(wint_t);
_CRTIMP int __cdecl iswdigit(wint_t);
_CRTIMP int __cdecl iswxdigit(wint_t);
_CRTIMP int __cdecl iswspace(wint_t);
_CRTIMP int __cdecl iswpunct(wint_t);
_CRTIMP int __cdecl iswalnum(wint_t);
_CRTIMP int __cdecl iswprint(wint_t);
_CRTIMP int __cdecl iswgraph(wint_t);
_CRTIMP int __cdecl iswcntrl(wint_t);
_CRTIMP int __cdecl iswascii(wint_t);
_CRTIMP int __cdecl isleadbyte(int);

_CRTIMP wint_t __cdecl towupper(wint_t);
_CRTIMP wint_t __cdecl towlower(wint_t);

_CRTIMP int __cdecl iswctype(wint_t, wctype_t);

_CRTIMP int __cdecl __iswcsymf(wint_t);
_CRTIMP int __cdecl __iswcsym(wint_t);

#ifndef _CRT_IS_WCTYPE_NO_DEPRECATE
#define _CRT_IS_WCTYPE_DEPRECATE __declspec(deprecated)
#else
#define _CRT_IS_WCTYPE_DEPRECATE
#endif

_CRT_IS_WCTYPE_DEPRECATE _CRTIMP int __cdecl is_wctype(wint_t, wctype_t);

#define _WCTYPE_DEFINED
#endif

#ifndef _WCTYPE_INLINE_DEFINED
#if !defined (__cplusplus) || defined (_M_CEE_PURE)
#define iswalpha(_c)    ( iswctype(_c,_ALPHA) )
#define iswupper(_c)    ( iswctype(_c,_UPPER) )
#define iswlower(_c)    ( iswctype(_c,_LOWER) )
#define iswdigit(_c)    ( iswctype(_c,_DIGIT) )
#define iswxdigit(_c)   ( iswctype(_c,_HEX) )
#define iswspace(_c)    ( iswctype(_c,_SPACE) )
#define iswpunct(_c)    ( iswctype(_c,_PUNCT) )
#define iswalnum(_c)    ( iswctype(_c,_ALPHA|_DIGIT) )
#define iswprint(_c)    ( iswctype(_c,_BLANK|_PUNCT|_ALPHA|_DIGIT) )
#define iswgraph(_c)    ( iswctype(_c,_PUNCT|_ALPHA|_DIGIT) )
#define iswcntrl(_c)    ( iswctype(_c,_CONTROL) )
#define iswascii(_c)    ( (unsigned)(_c) < 0x80 )

#define isleadbyte(c) (__pctype_func()[(unsigned char)(c)] & _LEADBYTE)
#else   /* __cplusplus */
inline int __cdecl iswalpha(wint_t _C) {return (iswctype(_C,_ALPHA)); }
inline int __cdecl iswupper(wint_t _C) {return (iswctype(_C,_UPPER)); }
inline int __cdecl iswlower(wint_t _C) {return (iswctype(_C,_LOWER)); }
inline int __cdecl iswdigit(wint_t _C) {return (iswctype(_C,_DIGIT)); }
inline int __cdecl iswxdigit(wint_t _C) {return (iswctype(_C,_HEX)); }
inline int __cdecl iswspace(wint_t _C) {return (iswctype(_C,_SPACE)); }
inline int __cdecl iswpunct(wint_t _C) {return (iswctype(_C,_PUNCT)); }
inline int __cdecl iswalnum(wint_t _C) {return (iswctype(_C,_ALPHA|_DIGIT)); }
inline int __cdecl iswprint(wint_t _C)
        {return (iswctype(_C,_BLANK|_PUNCT|_ALPHA|_DIGIT)); }
inline int __cdecl iswgraph(wint_t _C)
        {return (iswctype(_C,_PUNCT|_ALPHA|_DIGIT)); }
inline int __cdecl iswcntrl(wint_t _C) {return (iswctype(_C,_CONTROL)); }
inline int __cdecl iswascii(wint_t _C) {return ((unsigned)(_C) < 0x80); }

inline int __cdecl isleadbyte(int _C)
        {return (__pctype_func()[(unsigned char)(_C)] & _LEADBYTE); }
#endif  /* __cplusplus */
#define _WCTYPE_INLINE_DEFINED
#endif  /* _WCTYPE_INLINE_DEFINED */

typedef wchar_t wctrans_t;
_CRTIMP2_FUNCTION(wint_t) towctrans(wint_t, wctrans_t);
_CRTIMP2_FUNCTION(wctrans_t) wctrans(const char *);
_CRTIMP2_FUNCTION(wctype_t) wctype(const char *);


#ifdef  __cplusplus
}
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_WCTYPE */
/* 88bf0570-3001-4e78-a5f2-be5765546192 */ 

