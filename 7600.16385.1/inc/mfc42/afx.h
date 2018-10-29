// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFX_H__
#define __AFX_H__

#ifndef __cplusplus
	#error MFC requires C++ compilation (use a .cpp suffix)
#endif

/////////////////////////////////////////////////////////////////////////////

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif
#ifndef _AFX_FULLTYPEINFO
#pragma component(mintypeinfo, on)
#endif

#include <afxver_.h>        // Target version control

#ifndef _AFX_NOFORCE_LIBS

/////////////////////////////////////////////////////////////////////////////
// Win32 libraries

#ifndef _AFXDLL
	#ifndef _UNICODE
		#ifdef _DEBUG
			#pragma comment(lib, "nafxcwd.lib")
		#else
			#pragma comment(lib, "nafxcw.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment(lib, "uafxcwd.lib")
		#else
			#pragma comment(lib, "uafxcw.lib")
		#endif
	#endif
#else
	#ifndef _UNICODE
		#ifdef _DEBUG
			#pragma comment(lib, "mfc42d.lib")
			#pragma comment(lib, "mfcs42d.lib")
		#else
			#pragma comment(lib, "mfc42.lib")
			#pragma comment(lib, "mfcs42.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment(lib, "mfc42ud.lib")
			#pragma comment(lib, "mfcs42ud.lib")
		#else
			#pragma comment(lib, "mfc42u.lib")
			#pragma comment(lib, "mfcs42u.lib")
		#endif
	#endif
#endif

#ifdef _DLL
	#if !defined(_AFX_NO_DEBUG_CRT) && defined(_DEBUG)
		#pragma comment(lib, "msvcrtd.lib")
	#else
		#pragma comment(lib, "msvcrt.lib")
	#endif
#else
#ifdef _MT
	#if !defined(_AFX_NO_DEBUG_CRT) && defined(_DEBUG)
		#pragma comment(lib, "libcmtd.lib")
	#else
		#pragma comment(lib, "libcmt.lib")
	#endif
#else
	#if !defined(_AFX_NO_DEBUG_CRT) && defined(_DEBUG)
		#pragma comment(lib, "libcd.lib")
	#else
		#pragma comment(lib, "libc.lib")
	#endif
#endif
#endif

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "winspool.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comctl32.lib")

// force inclusion of NOLIB.OBJ for /disallowlib directives
#pragma comment(linker, "/include:__afxForceEXCLUDE")

// force inclusion of DLLMODUL.OBJ for _USRDLL
#ifdef _USRDLL
#pragma comment(linker, "/include:__afxForceUSRDLL")
#endif

// force inclusion of STDAFX.OBJ for precompiled types
#ifdef _AFXDLL
#pragma comment(linker, "/include:__afxForceSTDAFX")
#endif

#endif //!_AFX_NOFORCE_LIBS

/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file
//   in addition to standard primitive data types and various helper macros

struct CRuntimeClass;          // object type information

class CObject;                        // the root of all objects classes

	class CException;                 // the root of all exceptions
		class CArchiveException;      // archive exception
		class CFileException;         // file exception
		class CSimpleException;
			class CMemoryException;       // out-of-memory exception
			class CNotSupportedException; // feature not supported exception
            class CInvalidArgException; // one of the parameters to the function is invalid

	class CFile;                      // raw binary file
		class CStdioFile;             // buffered stdio text/binary file
		class CMemFile;               // memory based file

// Non CObject classes
class CString;                        // growable string type
class CTimeSpan;                      // time/date difference
class CTime;                          // absolute time/date
struct CFileStatus;                   // file status information
struct CMemoryState;                  // diagnostic memory support

class CArchive;                       // object persistence tool
class CDumpContext;                   // object diagnostic dumping

/////////////////////////////////////////////////////////////////////////////
// Other includes from standard "C" runtimes

#ifndef _INC_STRING
	#include <string.h>
#endif
#ifndef _INC_STDIO
	#include <stdio.h>
#endif
#ifndef _INC_STDLIB
	#include <stdlib.h>
#endif
#ifndef _INC_TIME
	#include <time.h>
#endif
#ifndef _INC_LIMITS
	#include <limits.h>
#endif
#ifndef _INC_STDDEF
	#include <stddef.h>
#endif
#ifndef _INC_STDARG
	#include <stdarg.h>
#endif

#ifndef _AFX_NO_DEBUG_CRT
#ifndef _INC_CRTDBG
	#include <crtdbg.h>
#endif
#endif // _AFX_NO_DEBUG_CRT

#ifdef _AFX_OLD_EXCEPTIONS
// use setjmp and helper functions instead of C++ keywords
#ifndef _INC_SETJMP
	#pragma warning(disable: 4611)
	#include <setjmp.h>
#endif
#endif

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

/////////////////////////////////////////////////////////////////////////////
// Basic types

// abstract iteration position
struct __POSITION { };
typedef __POSITION* POSITION;

struct _AFX_DOUBLE  { BYTE doubleBits[sizeof(double)]; };
struct _AFX_FLOAT   { BYTE floatBits[sizeof(float)]; };

// Standard constants
#undef FALSE
#undef TRUE
#undef NULL

#define FALSE   0
#define TRUE    1
#define NULL    0

/////////////////////////////////////////////////////////////////////////////
// Diagnostic support

#ifdef _DEBUG

BOOL AFXAPI AfxAssertFailedLine(LPCSTR lpszFileName, int nLine);

void AFX_CDECL AfxTrace(LPCTSTR lpszFormat, ...);
// Note: file names are still ANSI strings (filenames rarely need UNICODE)
void AFXAPI AfxAssertValidObject(const CObject* pOb,
				LPCSTR lpszFileName, int nLine);
void AFXAPI AfxDump(const CObject* pOb); // Dump an object from CodeView

#define TRACE              ::AfxTrace
#define THIS_FILE          __FILE__
#define ASSERT(f) \
	do \
	{ \
	if (!(f) && AfxAssertFailedLine(THIS_FILE, __LINE__)) \
		AfxDebugBreak(); \
	} while (0) \

#define VERIFY(f)          ASSERT(f)
#define ASSERT_VALID(pOb)  (::AfxAssertValidObject(pOb, THIS_FILE, __LINE__))
#define DEBUG_ONLY(f)      (f)

// The following trace macros are provided for backward compatiblity
//  (they also take a fixed number of parameters which provides
//   some amount of extra error checking)
#define TRACE0(sz)              ::AfxTrace(_T("%s"), _T(sz))
#define TRACE1(sz, p1)          ::AfxTrace(_T(sz), p1)
#define TRACE2(sz, p1, p2)      ::AfxTrace(_T(sz), p1, p2)
#define TRACE3(sz, p1, p2, p3)  ::AfxTrace(_T(sz), p1, p2, p3)

// These AFX_DUMP macros also provided for backward compatibility
#define AFX_DUMP0(dc, sz)   dc << _T(sz)
#define AFX_DUMP1(dc, sz, p1) dc << _T(sz) << p1

#else   // _DEBUG

#define ASSERT(f)          ((void)0)
#define VERIFY(f)          ((void)(f))
#define ASSERT_VALID(pOb)  ((void)0)
#define DEBUG_ONLY(f)      ((void)0)
inline void AFX_CDECL AfxTrace(LPCTSTR, ...) { }
#define TRACE              1 ? (void)0 : ::AfxTrace
#define TRACE0(sz)
#define TRACE1(sz, p1)
#define TRACE2(sz, p1, p2)
#define TRACE3(sz, p1, p2, p3)

#endif // !_DEBUG

#define ASSERT_POINTER(p, type) \
	ASSERT(((p) != NULL) && AfxIsValidAddress((p), sizeof(type), FALSE))

#define ASSERT_NULL_OR_POINTER(p, type) \
	ASSERT(((p) == NULL) || AfxIsValidAddress((p), sizeof(type), FALSE))

// Debug ASSERTs then throws. Retail throws if condition not met
#define ENSURE_THROW(cond, exception)	\
	do { ASSERT(cond); if (!(cond)){exception;} } while (false)
#define ENSURE(cond)		ENSURE_THROW(cond, ::AfxThrowInvalidArgException() )
#define ENSURE_ARG(cond)	ENSURE_THROW(cond, ::AfxThrowInvalidArgException() )

// Debug ASSERT_VALIDs then throws. Retail throws if pOb is NULL
#define ENSURE_VALID_THROW(pOb, exception)	\
	do { ASSERT_VALID(pOb); if (!(pOb)){exception;} } while (false)
#define ENSURE_VALID(pOb)	ENSURE_VALID_THROW(pOb, ::AfxThrowInvalidArgException() )

#define ENSURE_RETURN_HR(cond, hr)          	   \
	do { ASSERT(cond); if (!(cond)){return hr;} } while (false)
#define ENSURE_RETURN_VAL(cond, val)	ENSURE_RETURN_HR(cond, val)
#define ENSURE_RETURN(cond)	ENSURE_RETURN_HR(cond, E_FAIL)

/////////////////////////////////////////////////////////////////////////////
// Turn off warnings for /W4
// To resume any of these warning: #pragma warning(default: 4xxx)
// which should be placed after the AFX include files
#ifndef ALL_WARNINGS
// warnings generated with common MFC/Windows code
#pragma warning(disable: 4127)  // constant expression for TRACE/ASSERT
#pragma warning(disable: 4134)  // message map member fxn casts
#pragma warning(disable: 4201)  // nameless unions are part of C++
#pragma warning(disable: 4511)  // private copy constructors are good to have
#pragma warning(disable: 4512)  // private operator= are good to have
#pragma warning(disable: 4514)  // unreferenced inlines are common
#pragma warning(disable: 4710)  // private constructors are disallowed
#pragma warning(disable: 4705)  // statement has no effect in optimized code
#pragma warning(disable: 4191)  // pointer-to-function casting
// warnings caused by normal optimizations
#ifndef _DEBUG
#pragma warning(disable: 4701)  // local variable *may* be used without init
#pragma warning(disable: 4702)  // unreachable code caused by optimizations
#pragma warning(disable: 4791)  // loss of debugging info in release version
#pragma warning(disable: 4189)  // initialized but unused variable
#pragma warning(disable: 4390)  // empty controlled statement
#endif
// warnings specific to _AFXDLL version
#ifdef _AFXDLL
#pragma warning(disable: 4204)  // non-constant aggregate initializer
#endif
#ifdef _AFXDLL
#pragma warning(disable: 4275)  // deriving exported class from non-exported
#pragma warning(disable: 4251)  // using non-exported as public in exported
#endif
#endif //!ALL_WARNINGS

#ifdef _DEBUG
#define UNUSED(x)
#else
#define UNUSED(x) x
#endif
#define UNUSED_ALWAYS(x) x

/////////////////////////////////////////////////////////////////////////////
// Other implementation helpers

#define BEFORE_START_POSITION ((POSITION)-1L)

/////////////////////////////////////////////////////////////////////////////
// explicit initialization for general purpose classes

BOOL AFXAPI AfxInitialize(BOOL bDLL = FALSE, DWORD dwVersion = _MFC_VER);

#undef AFX_DATA
#define AFX_DATA AFX_CORE_DATA

/////////////////////////////////////////////////////////////////////////////
// Basic object model

struct CRuntimeClass
{
// Attributes
	LPCSTR m_lpszClassName;
	int m_nObjectSize;
	UINT m_wSchema; // schema number of the loaded class
	CObject* (PASCAL* m_pfnCreateObject)(); // NULL => abstract class
#ifdef _AFXDLL
	CRuntimeClass* (PASCAL* m_pfnGetBaseClass)();
#else
	CRuntimeClass* m_pBaseClass;
#endif

// Operations
	CObject* CreateObject();
	BOOL IsDerivedFrom(const CRuntimeClass* pBaseClass) const;

// Implementation
	void Store(CArchive& ar) const;
	static CRuntimeClass* PASCAL Load(CArchive& ar, UINT* pwSchemaNum);

	// CRuntimeClass objects linked together in simple list
	CRuntimeClass* m_pNextClass;       // linked list of registered classes
};

/////////////////////////////////////////////////////////////////////////////
// Strings

#ifndef _OLEAUTO_H_
#ifdef OLE2ANSI
	typedef LPSTR BSTR;
#else
	typedef LPWSTR BSTR;// must (semantically) match typedef in oleauto.h
#endif
#endif

struct CStringData
{
	long nRefs;             // reference count
	int nDataLength;        // length of data (including terminator)
	int nAllocLength;       // length of allocation
	// TCHAR data[nAllocLength]

	TCHAR* data()			// TCHAR* to managed data
		{ return (TCHAR*)(this+1); }
};

class CString
{
public:
// Constructors

	// constructs empty CString
	CString();
	// copy constructor
	CString(const CString& stringSrc);
	// from a single character
	CString(TCHAR ch, int nRepeat = 1);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	CString(unsigned short ch, int nRepeat = 1);
#endif
	// from an ANSI string (converts to TCHAR)
	CString(LPCSTR lpsz);
	// from a UNICODE string (converts to TCHAR)
	CString(LPCWSTR lpsz);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	CString(const unsigned short* lpsz);
#endif
#if _MFC_VER >= 0x0600
	// subset of characters from an ANSI string (converts to TCHAR)
	CString(LPCSTR lpch, int nLength);
	// subset of characters from a UNICODE string (converts to TCHAR)
	CString(LPCWSTR lpch, int nLength);
#else
	// subset of characters from a string (converts to TCHAR)
	CString(LPCTSTR lpch, int nLength);
#endif
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	CString(const unsigned short* lpch, int nLength);
#endif
	// from unsigned characters
	CString(const unsigned char* psz);

// Attributes & Operations

	// get data length
	int GetLength() const;
	// TRUE if zero length
	BOOL IsEmpty() const;
	// clear contents to empty
	void Empty();

	// return single character at zero-based index
	TCHAR GetAt(int nIndex) const;
	// return single character at zero-based index
	TCHAR operator[](int nIndex) const;
	// set a single character at zero-based index
	void SetAt(int nIndex, TCHAR ch);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	void SetAt(int nIndex, unsigned short ch);
#endif
	// return pointer to const string
	operator LPCTSTR() const;
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	// operator const unsigned short*() const;
	// deliberate not provided
	// It causes ambiguity.
#endif

	// overloaded assignment

	// ref-counted copy from another CString
    const CString& operator=(const CString& stringSrc);
	// set string content to single character
	const CString& operator=(TCHAR ch);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	const CString& operator=(unsigned short);
#endif
#ifdef _UNICODE
	const CString& operator=(char ch);
#endif
	// copy string content from ANSI string (converts to TCHAR)
	const CString& operator=(__in_z_opt LPCSTR lpsz);
	// copy string content from UNICODE string (converts to TCHAR)
	const CString& operator=(__in_z_opt LPCWSTR lpsz);
	// copy string content from unsigned chars
	const CString& operator=(const unsigned char* psz);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	const CString& operator=(const unsigned short* psz);
#endif

	// string concatenation

	// concatenate from another CString
	const CString& operator+=(const CString& string);

	// concatenate a single character
	const CString& operator+=(TCHAR ch);
#ifdef _UNICODE
	// concatenate an ANSI character after converting it to TCHAR
	const CString& operator+=(char ch);
#endif
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	const CString& operator+=(unsigned short ch);
#endif
	// concatenate a UNICODE character after converting it to TCHAR
	const CString& operator+=(LPCTSTR lpsz);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	const CString& operator+=(const unsigned short* lpsz);
#endif

	friend CString AFXAPI operator+(const CString& string1,
			const CString& string2);
	friend CString AFXAPI operator+(const CString& string, TCHAR ch);
	friend CString AFXAPI operator+(TCHAR ch, const CString& string);
#ifdef _UNICODE
	friend CString AFXAPI operator+(const CString& string, char ch);
	friend CString AFXAPI operator+(char ch, const CString& string);
#endif
	friend CString AFXAPI operator+(const CString& string, LPCTSTR lpsz);
	friend CString AFXAPI operator+(LPCTSTR lpsz, const CString& string);

#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	friend CString AFXAPI operator+(const CString& string, unsigned short ch);
	friend CString AFXAPI operator+(unsigned short ch, const CString& string);
	friend CString AFXAPI operator+(const CString& string, const unsigned short* lpsz);
	friend CString AFXAPI operator+(const unsigned short* lpsz, const CString& string);
#endif

	// string comparison

	// straight character comparison
	int Compare(LPCTSTR lpsz) const;
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	int Compare(const unsigned short* lpsz) const;
#endif
	// compare ignoring case
	int CompareNoCase(LPCTSTR lpsz) const;
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	int CompareNoCase(const unsigned short* lpsz) const;
#endif
	// NLS aware comparison, case sensitive
	int Collate(LPCTSTR lpsz) const;
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	int Collate(const unsigned short* lpsz) const;
#endif
#if _MFC_VER >= 0x0600
	// NLS aware comparison, case insensitive
	int CollateNoCase(LPCTSTR lpsz) const;
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	int CollateNoCase(const unsigned short* lpsz) const;
#endif
#endif

	// simple sub-string extraction

	// return nCount characters starting at zero-based nFirst
	CString Mid(int nFirst, int nCount) const;
	// return all characters starting at zero-based nFirst
	CString Mid(int nFirst) const;
	// return first nCount characters in string
	CString Left(int nCount) const;
	// return nCount characters from end of string
	CString Right(int nCount) const;

	//	characters from beginning that are also in passed string
	CString SpanIncluding(LPCTSTR lpszCharSet) const;
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	CString SpanIncluding(const unsigned short* lpszCharSet) const;
#endif
	// characters from beginning that are not also in passed string
	CString SpanExcluding(LPCTSTR lpszCharSet) const;
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	CString SpanExcluding(const unsigned short* lpszCharSet) const;
#endif

	// upper/lower/reverse conversion

	// NLS aware conversion to uppercase
	void MakeUpper();
	// NLS aware conversion to lowercase
	void MakeLower();
	// reverse string right-to-left
	void MakeReverse();

	// trimming whitespace (either side)

	// remove whitespace starting from right edge
	void TrimRight();
	// remove whitespace starting from left side
	void TrimLeft();

#if _MFC_VER >= 0x0600
	// trimming anything (either side)

	// remove continuous occurrences of chTarget starting from right
	void TrimRight(TCHAR chTarget);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	void TrimRight(unsigned short chTarget);
#endif
	// remove continuous occcurrences of characters in passed string,
	// starting from right
	void TrimRight(LPCTSTR lpszTargets);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	void TrimRight(const unsigned short* lpszTargets);
#endif
	// remove continuous occurrences of chTarget starting from left
	void TrimLeft(TCHAR chTarget);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	void TrimLeft(unsigned short chTarget);
#endif
	// remove continuous occcurrences of characters in
	// passed string, starting from left
	void TrimLeft(LPCTSTR lpszTargets);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	void TrimLeft(const unsigned short* lpszTargets);
#endif

	// advanced manipulation

	// replace occurrences of chOld with chNew
	int Replace(TCHAR chOld, TCHAR chNew);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	int Replace(unsigned short chOld, unsigned short chNew);
#endif
	// replace occurrences of substring lpszOld with lpszNew;
	// empty lpszNew removes instances of lpszOld
	int Replace(LPCTSTR lpszOld, LPCTSTR lpszNew);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	int Replace(const unsigned short* lpszOld, const unsigned short* lpszNew);
#endif
	// remove occurrences of chRemove
	int Remove(TCHAR chRemove);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	int Remove(unsigned short chRemove);
#endif
	// insert character at zero-based index; concatenates
	// if index is past end of string
	int Insert(int nIndex, TCHAR ch);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	int Insert(int nIndex, unsigned short ch);
#endif
	// insert substring at zero-based index; concatenates
	// if index is past end of string
	int Insert(int nIndex, __in_z LPCTSTR pstr);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	int Insert(int nIndex, __in_z const unsigned short* pstr);
#endif
	// delete nCount characters starting at zero-based index
	int Delete(int nIndex, int nCount = 1);
#endif

	// searching

	// find character starting at left, -1 if not found
	int Find(TCHAR ch) const;
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	int Find(unsigned short ch) const;
#endif
	// find character starting at right
	int ReverseFind(TCHAR ch) const;
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	int ReverseFind(unsigned short ch) const;
#endif
#if _MFC_VER >= 0x0600
	// find character starting at zero-based index and going right
	int Find(TCHAR ch, int nStart) const;
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	int Find(unsigned short ch, int nStart) const;
#endif
#endif
	// find first instance of any character in passed string
	int FindOneOf(LPCTSTR lpszCharSet) const;
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	int FindOneOf(const unsigned short* lpszCharSet) const;
#endif
	// find first instance of substring
	int Find(LPCTSTR lpszSub) const;
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	int Find(const unsigned short* lpszSub) const;
#endif
#if _MFC_VER >= 0x0600
	// find first instance of substring starting at zero-based index
	int Find(LPCTSTR lpszSub, int nStart) const;
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	int Find(const unsigned short* lpszSub, int nStart) const;
#endif
#endif

	// simple formatting

	// printf-like formatting using passed string
	void AFX_CDECL Format(LPCTSTR lpszFormat, ...);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	void AFX_CDECL Format(const unsigned short* lpszFormat, ...);
#endif
	// printf-like formatting using referenced string resource
	void AFX_CDECL Format(UINT nFormatID, ...);
#if _MFC_VER < 0x0600
protected:
#endif
	// printf-like formatting using variable arguments parameter
	void FormatV(LPCTSTR lpszFormat, va_list argList);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	void FormatV(const unsigned short* lpszFormat, va_list argList);
#endif
#if _MFC_VER < 0x0600
public:
#endif

	// formatting for localization (uses FormatMessage API)

	// format using FormatMessage API on passed string
	void AFX_CDECL FormatMessage(LPCTSTR lpszFormat, ...);
	// format using FormatMessage API on referenced string resource
	void AFX_CDECL FormatMessage(UINT nFormatID, ...);

	// input and output
#ifdef _DEBUG
	friend CDumpContext& AFXAPI operator<<(CDumpContext& dc,
				const CString& string);
#endif
	friend CArchive& AFXAPI operator<<(CArchive& ar, const CString& string);
	friend CArchive& AFXAPI operator>>(CArchive& ar, CString& string);

	// load from string resource
	BOOL LoadString(UINT nID);

#ifndef _UNICODE
	// ANSI <-> OEM support (convert string in place)

	// convert string from ANSI to OEM in-place
	void AnsiToOem();
	// convert string from OEM to ANSI in-place
	void OemToAnsi();
#endif

#ifndef _AFX_NO_BSTR_SUPPORT
	// OLE BSTR support (use for OLE automation)

	// return a BSTR initialized with this CString's data
	BSTR AllocSysString() const;
	// reallocates the passed BSTR, copies content of this CString to it
	BSTR SetSysString(BSTR* pbstr) const;
#endif

	// Access to string implementation buffer as "C" character array

	// get pointer to modifiable buffer at least as long as nMinBufLength
	LPTSTR GetBuffer(int nMinBufLength);
	// release buffer, setting length to nNewLength (or to first nul if -1)
	void ReleaseBuffer(int nNewLength = -1);
	// get pointer to modifiable buffer exactly as long as nNewLength
	LPTSTR GetBufferSetLength(int nNewLength);
	// release memory allocated to but unused by string
	void FreeExtra();

	// Use LockBuffer/UnlockBuffer to turn refcounting off

	// turn refcounting back on
	LPTSTR LockBuffer();
	// turn refcounting off
	void UnlockBuffer();

// Implementation
public:
	~CString();
	int GetAllocLength() const;

protected:
	LPTSTR m_pchData;   // pointer to ref counted string data

	// implementation helpers
	CStringData* GetData() const;
	void Init();
	void AllocCopy(CString& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;
	void AllocBuffer(int nLen);
	void AssignCopy(int nSrcLen, __in_ecount_opt(nSrcLen) LPCTSTR lpszSrcData);
	void ConcatCopy(int nSrc1Len, __in_ecount_opt(nSrc1Len) LPCTSTR lpszSrc1Data, int nSrc2Len, __in_ecount_opt(nSrc2Len) LPCTSTR lpszSrc2Data);
	void ConcatInPlace(int nSrcLen, __in_ecount_z(nSrcLen) LPCTSTR lpszSrcData);
	void CopyBeforeWrite();
	void AllocBeforeWrite(int nLen);
	void Release();
	static void PASCAL Release(CStringData* pData);
	static int PASCAL SafeStrlen(LPCTSTR lpsz);
#if _MFC_VER >= 0x0600
	static void FASTCALL FreeData(CStringData* pData);
#endif

#if (_MFC_VER >= 0x0600) && defined(UNICODE)

public:

	PCTSTR GetString() const;

	// trimming

	// Remove all leading and trailing whitespace
	CString& Trim();

	// Remove all leading and trailing occurrences of character 'chTarget'
	CString& Trim(__in TCHAR chTarget);
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	CString& Trim(__in unsigned short chTarget);
#endif

	// Remove all leading and trailing occurrences of any of the characters in the string 'pszTargets'
	CString& Trim(__in PCTSTR pszTargets);	
#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
	CString& Trim(__in const unsigned short* pszTargets);	
#endif

#if defined(__cplusplus_cli)
	explicit CString(System::String^ pString);
#endif
#endif
};

// Compare helpers
bool AFXAPI operator==(const CString& s1, const CString& s2);
bool AFXAPI operator==(const CString& s1, LPCTSTR s2);
bool AFXAPI operator==(LPCTSTR s1, const CString& s2);
bool AFXAPI operator!=(const CString& s1, const CString& s2);
bool AFXAPI operator!=(const CString& s1, LPCTSTR s2);
bool AFXAPI operator!=(LPCTSTR s1, const CString& s2);
bool AFXAPI operator<(const CString& s1, const CString& s2);
bool AFXAPI operator<(const CString& s1, LPCTSTR s2);
bool AFXAPI operator<(LPCTSTR s1, const CString& s2);
bool AFXAPI operator>(const CString& s1, const CString& s2);
bool AFXAPI operator>(const CString& s1, LPCTSTR s2);
bool AFXAPI operator>(LPCTSTR s1, const CString& s2);
bool AFXAPI operator<=(const CString& s1, const CString& s2);
bool AFXAPI operator<=(const CString& s1, LPCTSTR s2);
bool AFXAPI operator<=(LPCTSTR s1, const CString& s2);
bool AFXAPI operator>=(const CString& s1, const CString& s2);
bool AFXAPI operator>=(const CString& s1, LPCTSTR s2);
bool AFXAPI operator>=(LPCTSTR s1, const CString& s2);

#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)
bool AFXAPI operator==(const CString& s1, const unsigned short* s2);
bool AFXAPI operator==(const unsigned short* s1, const CString& s2);
bool AFXAPI operator!=(const CString& s1, const unsigned short* s2);
bool AFXAPI operator!=(const unsigned short* s1, const CString& s2);
bool AFXAPI operator<(const CString& s1, const unsigned short* s2);
bool AFXAPI operator<(const unsigned short* s1, const CString& s2);
bool AFXAPI operator>(const CString& s1, const unsigned short* s2);
bool AFXAPI operator>(const unsigned short* s1, const CString& s2);
bool AFXAPI operator<=(const CString& s1, const unsigned short* s2);
bool AFXAPI operator<=(const unsigned short* s1, const CString& s2);
bool AFXAPI operator>=(const CString& s1, const unsigned short* s2);
bool AFXAPI operator>=(const unsigned short* s1, const CString& s2);
#endif

// conversion helpers
int AFX_CDECL _wcstombsz(__out_bcount_z_opt(count) char* mbstr, __in_z const wchar_t* wcstr, UINT count);
int AFX_CDECL _mbstowcsz(__out_ecount_z_opt(count) wchar_t* wcstr, __in_z const char* mbstr, UINT count);

// Globals
extern AFX_DATA TCHAR afxChNil;
#if _MFC_VER >= 0x0600
#ifdef _AFXDLL
const CString& AFXAPI AfxGetEmptyString();
#define afxEmptyString AfxGetEmptyString()
#else
extern LPCTSTR _afxPchNil;
#define afxEmptyString ((CString&)*(CString*)&_afxPchNil)
#endif
#else   // _MFC_VER
const CString& AFXAPI AfxGetEmptyString();
#define afxEmptyString AfxGetEmptyString()
#endif  // _MFC_VER

/////////////////////////////////////////////////////////////////////////////
// class CObject is the root of all compliant objects

#ifdef _AFXDLL
class CObject
#else
class AFX_NOVTABLE CObject
#endif
{
public:

// Object model (types, destruction, allocation)
	virtual CRuntimeClass* GetRuntimeClass() const;
	virtual ~CObject();  // virtual destructors are necessary

	// Diagnostic allocations
	void* PASCAL operator new(size_t nSize) throw(...);
	void* PASCAL operator new(size_t, void* p) throw(...);
	void PASCAL operator delete(void* p);
#if _MSC_VER >= 1200
	void PASCAL operator delete(void* p, void* pPlace);
#endif

#if defined(_DEBUG) && !defined(_AFX_NO_DEBUG_CRT)
	// for file name/line number tracking using DEBUG_NEW
	void* PASCAL operator new(size_t nSize, LPCSTR lpszFileName, int nLine) throw(...);
#if (_MFC_VER >= 0x0600) && (_MSC_VER >= 1200)
	void PASCAL operator delete(void *p, LPCSTR lpszFileName, int nLine);
#endif
#endif

	// Disable the copy constructor and assignment by default so you will get
	//   compiler errors instead of unexpected behaviour if you pass objects
	//   by value or assign objects.
protected:
	CObject();
private:
	CObject(const CObject& objectSrc);              // no implementation
	void operator=(const CObject& objectSrc);       // no implementation

// Attributes
public:
	BOOL IsSerializable() const;
	BOOL IsKindOf(const CRuntimeClass* pClass) const;

// Overridables
	virtual void Serialize(CArchive& ar);

#if _MFC_VER < 0x0600 || defined(_DEBUG) || defined(_AFXDLL)
	// Diagnostic Support
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Implementation
public:
	static const AFX_DATA CRuntimeClass classCObject;
#ifdef _AFXDLL
	static CRuntimeClass* PASCAL _GetBaseClass();
#endif
};

// Helper macros
#define RUNTIME_CLASS(class_name) ((CRuntimeClass*)(&class_name::class##class_name))
#define ASSERT_KINDOF(class_name, object) \
	ASSERT((object)->IsKindOf(RUNTIME_CLASS(class_name)))

// RTTI helper macros/functions
const CObject* AFX_CDECL AfxDynamicDownCast(CRuntimeClass* pClass, const CObject* pObject);
CObject* AFX_CDECL AfxDynamicDownCast(CRuntimeClass* pClass, CObject* pObject);
#define DYNAMIC_DOWNCAST(class_name, object) \
	(class_name*)AfxDynamicDownCast(RUNTIME_CLASS(class_name), object)

#ifdef _DEBUG
const CObject* AFX_CDECL AfxStaticDownCast(CRuntimeClass* pClass, const CObject* pObject);
CObject* AFX_CDECL AfxStaticDownCast(CRuntimeClass* pClass, CObject* pObject);
#define STATIC_DOWNCAST(class_name, object) \
	((class_name*)AfxStaticDownCast(RUNTIME_CLASS(class_name), object))
#else
#define STATIC_DOWNCAST(class_name, object) ((class_name*)object)
#endif

//////////////////////////////////////////////////////////////////////////////
// Helper macros for declaring CRuntimeClass compatible classes

#ifdef _AFXDLL
#define DECLARE_DYNAMIC(class_name) \
protected: \
	static CRuntimeClass* PASCAL _GetBaseClass(); \
public: \
	static const AFX_DATA CRuntimeClass class##class_name; \
	virtual CRuntimeClass* GetRuntimeClass() const; \

#define _DECLARE_DYNAMIC(class_name) \
protected: \
	static CRuntimeClass* PASCAL _GetBaseClass(); \
public: \
	static AFX_DATA CRuntimeClass class##class_name; \
	virtual CRuntimeClass* GetRuntimeClass() const; \

#else
#define DECLARE_DYNAMIC(class_name) \
public: \
	static const AFX_DATA CRuntimeClass class##class_name; \
	virtual CRuntimeClass* GetRuntimeClass() const; \

#define _DECLARE_DYNAMIC(class_name) \
public: \
	static AFX_DATA CRuntimeClass class##class_name; \
	virtual CRuntimeClass* GetRuntimeClass() const; \

#endif

// not serializable, but dynamically constructable
#define DECLARE_DYNCREATE(class_name) \
	DECLARE_DYNAMIC(class_name) \
	static CObject* PASCAL CreateObject();

#define _DECLARE_DYNCREATE(class_name) \
	_DECLARE_DYNAMIC(class_name) \
	static CObject* PASCAL CreateObject();

#define DECLARE_SERIAL(class_name) \
	_DECLARE_DYNCREATE(class_name) \
	AFX_API friend CArchive& AFXAPI operator>>(CArchive& ar, class_name* &pOb);

// generate static object constructor for class registration
#if _MFC_VER >= 0x600
void AFXAPI AfxClassInit(CRuntimeClass* pNewClass);
struct AFX_CLASSINIT
{
#if defined(_IA64_) && !defined (_DEBUG) && defined(_AFX_DLL_IMPL)
	__declspec(noinline)
#endif  // defined(_IA64_) && !defined (_DEBUG) && defined(_AFX_DLL_IMPL)
	AFX_CLASSINIT(CRuntimeClass* pNewClass) { AfxClassInit(pNewClass); } 
};

struct AFX_CLASSINIT_COMPAT
	{ AFX_CLASSINIT_COMPAT(CRuntimeClass* pNewClass); };
#else
struct AFX_CLASSINIT
	{ AFX_CLASSINIT(CRuntimeClass* pNewClass); };
#endif

#ifdef _AFXDLL
#define IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew) \
	CRuntimeClass* PASCAL class_name::_GetBaseClass() \
		{ return RUNTIME_CLASS(base_class_name); } \
	AFX_COMDAT const AFX_DATADEF CRuntimeClass class_name::class##class_name = { \
		#class_name, sizeof(class class_name), wSchema, pfnNew, \
			&class_name::_GetBaseClass, NULL }; \
	CRuntimeClass* class_name::GetRuntimeClass() const \
		{ return RUNTIME_CLASS(class_name); } \

#define _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew) \
	CRuntimeClass* PASCAL class_name::_GetBaseClass() \
		{ return RUNTIME_CLASS(base_class_name); } \
	AFX_COMDAT AFX_DATADEF CRuntimeClass class_name::class##class_name = { \
		#class_name, sizeof(class class_name), wSchema, pfnNew, \
			&class_name::_GetBaseClass, NULL }; \
	CRuntimeClass* class_name::GetRuntimeClass() const \
		{ return RUNTIME_CLASS(class_name); } \

#else
#define IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew) \
	AFX_COMDAT const AFX_DATADEF CRuntimeClass class_name::class##class_name = { \
		#class_name, sizeof(class class_name), wSchema, pfnNew, \
			RUNTIME_CLASS(base_class_name), NULL }; \
	CRuntimeClass* class_name::GetRuntimeClass() const \
		{ return RUNTIME_CLASS(class_name); } \

#define _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew) \
	AFX_DATADEF CRuntimeClass class_name::class##class_name = { \
		#class_name, sizeof(class class_name), wSchema, pfnNew, \
			RUNTIME_CLASS(base_class_name), NULL }; \
	CRuntimeClass* class_name::GetRuntimeClass() const \
		{ return RUNTIME_CLASS(class_name); } \

#endif

#define IMPLEMENT_DYNAMIC(class_name, base_class_name) \
	IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF, NULL)

#define IMPLEMENT_DYNCREATE(class_name, base_class_name) \
	CObject* PASCAL class_name::CreateObject() \
		{ return new class_name; } \
	IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF, \
		class_name::CreateObject)

#if _MFC_VER >= 0x0600
#define IMPLEMENT_SERIAL(class_name, base_class_name, wSchema) \
	CObject* PASCAL class_name::CreateObject() \
		{ return new class_name; } \
	_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, \
		class_name::CreateObject) \
	AFX_CLASSINIT _init_##class_name(RUNTIME_CLASS(class_name)); \
	CArchive& AFXAPI operator>>(CArchive& ar, class_name* &pOb) \
		{ pOb = (class_name*) ar.ReadObject(RUNTIME_CLASS(class_name)); \
			return ar; } \

#else

#define IMPLEMENT_SERIAL(class_name, base_class_name, wSchema) \
	CObject* PASCAL class_name::CreateObject() \
		{ return new class_name; } \
	_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, \
		class_name::CreateObject) \
	static const AFX_CLASSINIT _init_##class_name(RUNTIME_CLASS(class_name)); \
	CArchive& AFXAPI operator>>(CArchive& ar, class_name* &pOb) \
		{ pOb = (class_name*) ar.ReadObject(RUNTIME_CLASS(class_name)); \
			return ar; } \

#endif

// optional bit for schema number that enables object versioning
#define VERSIONABLE_SCHEMA  (0x80000000)

/////////////////////////////////////////////////////////////////////////////
// other helpers

// zero fill everything after the vtbl pointer
#define AFX_ZERO_INIT_OBJECT(base_class) \
	memset(((base_class*)this)+1, 0, sizeof(*this) - sizeof(class base_class));


/////////////////////////////////////////////////////////////////////////////
// Exceptions

#ifdef _AFXDLL
class CException : public CObject
#else
class AFX_NOVTABLE CException : public CObject
#endif
{
	// abstract class for dynamic type checking
	DECLARE_DYNAMIC(CException)

public:
// Constructors
	CException();   // sets m_bAutoDelete = TRUE
	CException(BOOL bAutoDelete);   // sets m_bAutoDelete = bAutoDelete

// Operations
	void Delete();  // use to delete exception in 'catch' block

	virtual BOOL GetErrorMessage(__out_ecount_opt(nMaxError) LPTSTR lpszError, UINT nMaxError,
		__out_opt PUINT pnHelpContext = NULL);
	virtual int ReportError(UINT nType = MB_OK, UINT nMessageID = 0);

// Implementation (setting m_bAutoDelete to FALSE is advanced)
public:
	virtual ~CException();
	BOOL m_bAutoDelete;
#ifdef _DEBUG
	void PASCAL operator delete(void* pbData);
#if (_MSC_VER >= 1200) && (_MFC_VER >= 0x0600)
	void PASCAL operator delete(void* pbData, LPCSTR lpszFileName, int nLine);
#endif
protected:
	BOOL m_bReadyForDelete;
#endif
};

#ifdef _AFXDLL
class CSimpleException : public CException
#else
class AFX_NOVTABLE CSimpleException : public CException
#endif
{
	// base class for resource-critical MFC exceptions
	// handles ownership and initialization of an error message

public:
// Constructors
	CSimpleException();
	CSimpleException(BOOL bAutoDelete);

// Operations
	virtual BOOL GetErrorMessage(__out_ecount(nMaxError) LPTSTR lpszError, UINT nMaxError,
		__out_opt PUINT pnHelpContext = NULL);

// Implementation (setting m_bAutoDelete to FALSE is advanced)
public:
	virtual ~CSimpleException();
	BOOL m_bAutoDelete;

	void InitString();      // used during MFC initialization

protected:
	BOOL m_bInitialized;
	BOOL m_bLoaded;
	TCHAR m_szMessage[128];
	UINT m_nResourceID;

#ifdef _DEBUG
	BOOL m_bReadyForDelete;
#endif
};

// helper routines for non-C++ EH implementations
#ifdef _AFX_OLD_EXCEPTIONS
	BOOL AFXAPI AfxCatchProc(CRuntimeClass* pClass);
	void AFXAPI AfxThrow(CException* pException);
#else
	// for THROW_LAST auto-delete backward compatiblity
	void AFXAPI AfxThrowLastCleanup();
#endif

// other out-of-line helper functions
void AFXAPI AfxTryCleanup();

#ifndef _AFX_JUMPBUF
// Use portable 'jmp_buf' defined by ANSI by default.
#define _AFX_JUMPBUF jmp_buf
#endif

// Placed on frame for EXCEPTION linkage, or CException cleanup
struct AFX_EXCEPTION_LINK
{
#ifdef _AFX_OLD_EXCEPTIONS
	union
	{
		_AFX_JUMPBUF m_jumpBuf;
		struct
		{
			void (PASCAL* pfnCleanup)(AFX_EXCEPTION_LINK* pLink);
			void* pvData;       // extra data follows
		} m_callback;       // callback for cleanup (nType != 0)
	};
	UINT m_nType;               // 0 for setjmp, !=0 for user extension
#endif //!_AFX_OLD_EXCEPTIONS

	AFX_EXCEPTION_LINK* m_pLinkPrev;    // previous top, next in handler chain
	CException* m_pException;   // current exception (NULL in TRY block)

	AFX_EXCEPTION_LINK();       // for initialization and linking
	~AFX_EXCEPTION_LINK()       // for cleanup and unlinking
		{ AfxTryCleanup(); };
};

// Exception global state - never access directly
struct AFX_EXCEPTION_CONTEXT
{
	AFX_EXCEPTION_LINK* m_pLinkTop;

	// Note: most of the exception context is now in the AFX_EXCEPTION_LINK
};

#ifndef _PNH_DEFINED
typedef int (__cdecl * _PNH)( size_t );
#define _PNH_DEFINED
#endif

_PNH AFXAPI AfxGetNewHandler();
_PNH AFXAPI AfxSetNewHandler(_PNH pfnNewHandler);
int AFX_CDECL AfxNewHandler(size_t nSize);

void AFXAPI AfxAbort();

#ifdef _AFX_OLD_EXCEPTIONS

// Obsolete and non-portable: setting terminate handler
//  use CWinApp::ProcessWndProcException for Windows apps instead
//  can also use set_terminate which is part of C++ standard library
//      (these are provided for backward compatibility)
void AFXAPI AfxTerminate();
typedef void (AFXAPI* AFX_TERM_PROC)();
AFX_TERM_PROC AFXAPI AfxSetTerminate(AFX_TERM_PROC);

#endif

/////////////////////////////////////////////////////////////////////////////
// Exception macros using try, catch and throw
//  (for backward compatibility to previous versions of MFC)

#ifndef _AFX_OLD_EXCEPTIONS

#define TRY { AFX_EXCEPTION_LINK _afxExceptionLink; try {

#define AFX_BEGIN_DESTRUCTOR TRY {

#define CATCH(class, e) } catch (class* e) \
	{ ASSERT(e->IsKindOf(RUNTIME_CLASS(class))); \
		_afxExceptionLink.m_pException = e;

#define AND_CATCH(class, e) } catch (class* e) \
	{ ASSERT(e->IsKindOf(RUNTIME_CLASS(class))); \
		_afxExceptionLink.m_pException = e;

#define END_CATCH } }


#define THROW(e) throw e
#define THROW_LAST() (AfxThrowLastCleanup(), throw)

// Advanced macros for smaller code
#define CATCH_ALL(e) } catch (CException* e) \
	{ { ASSERT(e->IsKindOf(RUNTIME_CLASS(CException))); \
		_afxExceptionLink.m_pException = e;

#define AND_CATCH_ALL(e) } catch (CException* e) \
	{ { ASSERT(e->IsKindOf(RUNTIME_CLASS(CException))); \
		_afxExceptionLink.m_pException = e;

#define END_CATCH_ALL } } }

#define END_TRY } catch (CException* e) \
	{ ASSERT(e->IsKindOf(RUNTIME_CLASS(CException))); \
		_afxExceptionLink.m_pException = e; } }

#define AFX_END_DESTRUCTOR } CATCH_ALL(e) { if(e) e->Delete(); } END_CATCH_ALL

#else //_AFX_OLD_EXCEPTIONS

/////////////////////////////////////////////////////////////////////////////
// Exception macros using setjmp and longjmp
//  (for portability to compilers with no support for C++ exception handling)

#define TRY \
	{ AFX_EXCEPTION_LINK _afxExceptionLink; \
	if (::setjmp(_afxExceptionLink.m_jumpBuf) == 0)

#define CATCH(class, e) \
	else if (::AfxCatchProc(RUNTIME_CLASS(class))) \
	{ class* e = (class*)_afxExceptionLink.m_pException;

#define AND_CATCH(class, e) \
	} else if (::AfxCatchProc(RUNTIME_CLASS(class))) \
	{ class* e = (class*)_afxExceptionLink.m_pException;

#define END_CATCH \
	} else { ::AfxThrow(NULL); } }

#define THROW(e) AfxThrow(e)
#define THROW_LAST() AfxThrow(NULL)

// Advanced macros for smaller code
#define CATCH_ALL(e) \
	else { CException* e = _afxExceptionLink.m_pException;

#define AND_CATCH_ALL(e) \
	} else { CException* e = _afxExceptionLink.m_pException;

#define END_CATCH_ALL } }

#define END_TRY }

#endif //_AFX_OLD_EXCEPTIONS

/////////////////////////////////////////////////////////////////////////////
// Standard Exception classes

class CMemoryException : public CSimpleException
{
	DECLARE_DYNAMIC(CMemoryException)
public:
	CMemoryException();

// Implementation
public:
	CMemoryException(BOOL bAutoDelete);
	CMemoryException(BOOL bAutoDelete, UINT nResourceID);
	virtual ~CMemoryException();
};

class CNotSupportedException : public CSimpleException
{
	DECLARE_DYNAMIC(CNotSupportedException)
public:
	CNotSupportedException();

// Implementation
public:
	CNotSupportedException(BOOL bAutoDelete);
	CNotSupportedException(BOOL bAutoDelete, UINT nResourceID);
	virtual ~CNotSupportedException();
};

class CInvalidArgException : public CSimpleException
{
	DECLARE_DYNAMIC(CInvalidArgException)
public:
	CInvalidArgException();

// Implementation
public:
	CInvalidArgException(BOOL bAutoDelete);
	CInvalidArgException(BOOL bAutoDelete, UINT nResourceID);
	virtual ~CInvalidArgException();
};

class CArchiveException : public CException
{
	DECLARE_DYNAMIC(CArchiveException)
public:
	enum {
		none,
#ifdef __cplusplus_cli 
		genericException,
#else
		generic,
#endif
		readOnly,
		endOfFile,
		writeOnly,
		badIndex,
		badClass,
		badSchema
	};

#ifdef	__cplusplus_cli 
#pragma warning(push)
#pragma warning(disable:4996)
	__declspec(deprecated) 
	static const int __identifier(generic) = genericException;
#pragma warning(pop)
#endif

// Constructor
	CArchiveException(int cause = CArchiveException::none,
		LPCTSTR lpszArchiveName = NULL);

// Attributes
	int m_cause;
	CString m_strFileName;

// Implementation
public:
	virtual ~CArchiveException();
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL GetErrorMessage(__out_ecount_z(nMaxError) LPTSTR lpszError, UINT nMaxError,
		__out_opt PUINT pnHelpContext = NULL);
};

class CFileException : public CException
{
	DECLARE_DYNAMIC(CFileException)

public:
	enum {
		none,
#ifdef __cplusplus_cli 
		genericException,
#else
		generic,
#endif
		fileNotFound,
		badPath,
		tooManyOpenFiles,
		accessDenied,
		invalidFile,
		removeCurrentDir,
		directoryFull,
		badSeek,
		hardIO,
		sharingViolation,
		lockViolation,
		diskFull,
		endOfFile
	};

#ifdef	__cplusplus_cli 
#pragma warning(push)
#pragma warning(disable:4996)
	__declspec(deprecated) 
	static const int __identifier(generic) = genericException;
#pragma warning(pop)
#endif

// Constructor
	CFileException(int cause = CFileException::none, LONG lOsError = -1,
		LPCTSTR lpszArchiveName = NULL);

// Attributes
	int     m_cause;
	LONG    m_lOsError;
	CString m_strFileName;

// Operations
	// convert a OS dependent error code to a Cause
	static int PASCAL OsErrorToException(LONG lOsError);
	static int PASCAL ErrnoToException(int nErrno);

	// helper functions to throw exception after converting to a Cause
	static void PASCAL ThrowOsError(LONG lOsError, LPCTSTR lpszFileName = NULL);
	static void PASCAL ThrowErrno(int nErrno, LPCTSTR lpszFileName = NULL);

// Implementation
public:
	virtual ~CFileException();
#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
#endif
	virtual BOOL GetErrorMessage(__out_ecount(nMaxError) LPTSTR lpszError, UINT nMaxError,
		__out_opt PUINT pnHelpContext = NULL);
};

/////////////////////////////////////////////////////////////////////////////
// Standard exception throws

void __declspec(noreturn) AFXAPI AfxThrowMemoryException();
void __declspec(noreturn) AFXAPI AfxThrowNotSupportedException();
void __declspec(noreturn) AFXAPI AfxThrowInvalidArgException();
void __declspec(noreturn) AFXAPI AfxThrowArchiveException(int cause,
	LPCTSTR lpszArchiveName = NULL);
void __declspec(noreturn) AFXAPI AfxThrowFileException(int cause, LONG lOsError = -1,
	LPCTSTR lpszFileName = NULL);

/////////////////////////////////////////////////////////////////////////////
// File - raw unbuffered disk file I/O

class CFile : public CObject
{
	DECLARE_DYNAMIC(CFile)

public:
// Flag values
	enum OpenFlags {
		modeRead =          0x0000,
		modeWrite =         0x0001,
		modeReadWrite =     0x0002,
		shareCompat =       0x0000,
		shareExclusive =    0x0010,
		shareDenyWrite =    0x0020,
		shareDenyRead =     0x0030,
		shareDenyNone =     0x0040,
		modeNoInherit =     0x0080,
		modeCreate =        0x1000,
		modeNoTruncate =    0x2000,
		typeText =          0x4000, // typeText and typeBinary are used in
		typeBinary =   (int)0x8000 // derived classes only
		};

	enum Attribute {
		normal =    0x00,
		readOnly =  0x01,
		hidden =    0x02,
		system =    0x04,
		volume =    0x08,
		directory = 0x10,
		archive =   0x20
		};

	enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };

#ifdef _WIN64
	static const UINT_PTR hFileNull = (UINT_PTR)INVALID_HANDLE_VALUE;
#else
	enum { hFileNull = -1 };
#endif

// Constructors
	CFile();
	CFile(INT_PTR hFile);
	CFile(LPCTSTR lpszFileName, UINT nOpenFlags);

// Attributes
	UINT_PTR m_hFile;
	operator HFILE() const;

	virtual DWORD GetPosition() const;
	BOOL GetStatus(CFileStatus& rStatus) const;
	virtual CString GetFileName() const;
	virtual CString GetFileTitle() const;
	virtual CString GetFilePath() const;
	virtual void SetFilePath(LPCTSTR lpszNewName);

// Operations
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags,
		CFileException* pError = NULL);

	static void PASCAL Rename(LPCTSTR lpszOldName,
				LPCTSTR lpszNewName);
	static void PASCAL Remove(LPCTSTR lpszFileName);
	static BOOL PASCAL GetStatus(LPCTSTR lpszFileName,
				CFileStatus& rStatus);
	static void PASCAL SetStatus(LPCTSTR lpszFileName,
				const CFileStatus& status);

	DWORD SeekToEnd();
	void SeekToBegin();

	// backward compatible ReadHuge and WriteHuge
	DWORD ReadHuge(void* lpBuffer, DWORD dwCount);
	void WriteHuge(const void* lpBuffer, DWORD dwCount);

// Overridables
	virtual CFile* Duplicate() const;

	virtual LONG Seek(LONG lOff, UINT nFrom);
	virtual void SetLength(DWORD dwNewLen);
	virtual DWORD GetLength() const;

	virtual UINT Read(void* lpBuf, UINT nCount);
	virtual void Write(const void* lpBuf, UINT nCount);

	virtual void LockRange(DWORD dwPos, DWORD dwCount);
	virtual void UnlockRange(DWORD dwPos, DWORD dwCount);

	virtual void Abort();
	virtual void Flush();
	virtual void Close();

// Implementation
public:
	virtual ~CFile();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	enum BufferCommand { bufferRead, bufferWrite, bufferCommit, bufferCheck };
	virtual UINT GetBufferPtr(UINT nCommand, UINT nCount = 0,
		void** ppBufStart = NULL, void** ppBufMax = NULL);

protected:
	BOOL m_bCloseOnDelete;
	CString m_strFileName;
};

/////////////////////////////////////////////////////////////////////////////
// STDIO file implementation

class CStdioFile : public CFile
{
	DECLARE_DYNAMIC(CStdioFile)

public:
// Constructors
	CStdioFile();
	CStdioFile(FILE* pOpenStream);
	CStdioFile(LPCTSTR lpszFileName, UINT nOpenFlags);

// Attributes
	FILE* m_pStream;    // stdio FILE
						// m_hFile from base class is _fileno(m_pStream)

// Operations
	// reading and writing strings
	virtual void WriteString(__in_z LPCTSTR lpsz);
	virtual LPTSTR ReadString(__out_ecount_z(nMax) LPTSTR lpsz, UINT nMax);
	virtual BOOL ReadString(CString& rString);

// Implementation
public:
	virtual ~CStdioFile();
#ifdef _DEBUG
	void Dump(CDumpContext& dc) const;
#endif
	virtual DWORD GetPosition() const;
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags,
		CFileException* pError = NULL);
	virtual UINT Read(void* lpBuf, UINT nCount);
	virtual void Write(const void* lpBuf, UINT nCount);
	virtual LONG Seek(LONG lOff, UINT nFrom);
	virtual void Abort();
	virtual void Flush();
	virtual void Close();

	// Unsupported APIs
	virtual CFile* Duplicate() const;
	virtual void LockRange(DWORD dwPos, DWORD dwCount);
	virtual void UnlockRange(DWORD dwPos, DWORD dwCount);
};

////////////////////////////////////////////////////////////////////////////
// Memory based file implementation

class CMemFile : public CFile
{
	DECLARE_DYNAMIC(CMemFile)

public:
// Constructors
	CMemFile(UINT nGrowBytes = 1024);
	CMemFile(BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes = 0);

// Operations
	void Attach(BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes = 0);
	BYTE* Detach();

// Advanced Overridables
protected:
	virtual BYTE* Alloc(DWORD nBytes);
	virtual BYTE* Realloc(BYTE* lpMem, DWORD nBytes);
	virtual BYTE* Memcpy(__out_bcount(nBytes) BYTE* lpMemTarget, __in_bcount(nBytes) const BYTE* lpMemSource, UINT nBytes);
	virtual void Free(BYTE* lpMem);
	virtual void GrowFile(DWORD dwNewLen);

// Implementation
protected:
	UINT m_nGrowBytes;
	DWORD m_nPosition;
	DWORD m_nBufferSize;
	DWORD m_nFileSize;
	BYTE* m_lpBuffer;
	BOOL m_bAutoDelete;

public:
	virtual ~CMemFile();
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
	virtual void AssertValid() const;
#endif
	virtual DWORD GetPosition() const;
	BOOL GetStatus(CFileStatus& rStatus) const;
	virtual LONG Seek(LONG lOff, UINT nFrom);
	virtual void SetLength(DWORD dwNewLen);
	virtual UINT Read(void* lpBuf, UINT nCount);
	virtual void Write(const void* lpBuf, UINT nCount);
	virtual void Abort();
	virtual void Flush();
	virtual void Close();
	virtual UINT GetBufferPtr(UINT nCommand, UINT nCount = 0,
		void** ppBufStart = NULL, void** ppBufMax = NULL);

	// Unsupported APIs
	virtual CFile* Duplicate() const;
	virtual void LockRange(DWORD dwPos, DWORD dwCount);
	virtual void UnlockRange(DWORD dwPos, DWORD dwCount);
};

////////////////////////////////////////////////////////////////////////////
// Local file searches

class CFileFind : public CObject
{
public:
	CFileFind();
	virtual ~CFileFind();

// Attributes
public:
	DWORD GetLength() const;
#if defined(_X86_) || defined(_ALPHA_)
	__int64 GetLength64() const;
#endif
	virtual CString GetFileName() const;
	virtual CString GetFilePath() const;
	virtual CString GetFileTitle() const;
	virtual CString GetFileURL() const;
	virtual CString GetRoot() const;

	virtual BOOL GetLastWriteTime(FILETIME* pTimeStamp) const;
	virtual BOOL GetLastAccessTime(FILETIME* pTimeStamp) const;
	virtual BOOL GetCreationTime(FILETIME* pTimeStamp) const;
	virtual BOOL GetLastWriteTime(CTime& refTime) const;
	virtual BOOL GetLastAccessTime(CTime& refTime) const;
	virtual BOOL GetCreationTime(CTime& refTime) const;

	virtual BOOL MatchesMask(DWORD dwMask) const;

	virtual BOOL IsDots() const;
	// these aren't virtual because they all use MatchesMask(), which is
	BOOL IsReadOnly() const;
	BOOL IsDirectory() const;
	BOOL IsCompressed() const;
	BOOL IsSystem() const;
	BOOL IsHidden() const;
	BOOL IsTemporary() const;
	BOOL IsNormal() const;
	BOOL IsArchived() const;

// Operations
	void Close();
	virtual BOOL FindFile(LPCTSTR pstrName = NULL, DWORD dwUnused = 0);
	virtual BOOL FindNextFile();

protected:
	virtual void CloseContext();

// Implementation
protected:
	void* m_pFoundInfo;
	void* m_pNextInfo;
	HANDLE m_hContext;
	BOOL m_bGotLast;
	CString m_strRoot;
	TCHAR m_chDirSeparator;     // not '\\' for Internet classes

#ifdef _DEBUG
	void Dump(CDumpContext& dc) const;
	void AssertValid() const;
#endif

	DECLARE_DYNAMIC(CFileFind)
};

/////////////////////////////////////////////////////////////////////////////
// CTimeSpan and CTime

class CTimeSpan
{
public:

// Constructors
	CTimeSpan();
	CTimeSpan(time_t time);
	CTimeSpan(LONG lDays, int nHours, int nMins, int nSecs);

	CTimeSpan(const CTimeSpan& timeSpanSrc);
	const CTimeSpan& operator=(const CTimeSpan& timeSpanSrc);

// Attributes
	// extract parts
	LONG_PTR GetDays() const;   // total # of days
	LONG_PTR GetTotalHours() const;
	int GetHours() const;
	LONG_PTR GetTotalMinutes() const;
	int GetMinutes() const;
	LONG_PTR GetTotalSeconds() const;
	int GetSeconds() const;

// Operations
	// time math
	CTimeSpan operator-(CTimeSpan timeSpan) const;
	CTimeSpan operator+(CTimeSpan timeSpan) const;
	const CTimeSpan& operator+=(CTimeSpan timeSpan);
	const CTimeSpan& operator-=(CTimeSpan timeSpan);
	BOOL operator==(CTimeSpan timeSpan) const;
	BOOL operator!=(CTimeSpan timeSpan) const;
	BOOL operator<(CTimeSpan timeSpan) const;
	BOOL operator>(CTimeSpan timeSpan) const;
	BOOL operator<=(CTimeSpan timeSpan) const;
	BOOL operator>=(CTimeSpan timeSpan) const;

#ifdef _UNICODE
	// for compatibility with MFC 3.x
	CString Format(LPCSTR pFormat) const;
#endif
	CString Format(LPCTSTR pFormat) const;
	CString Format(UINT nID) const;

	// serialization
#ifdef _DEBUG
	friend CDumpContext& AFXAPI operator<<(CDumpContext& dc,CTimeSpan timeSpan);
#endif
	friend CArchive& AFXAPI operator<<(CArchive& ar, CTimeSpan timeSpan);
	friend CArchive& AFXAPI operator>>(CArchive& ar, CTimeSpan& rtimeSpan);

private:
	time_t m_timeSpan;
	friend class CTime;
};

class CTime
{
public:

// Constructors
	static CTime PASCAL GetCurrentTime();

	CTime();
	CTime(time_t time);
	CTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec,
		int nDST = -1);
	CTime(WORD wDosDate, WORD wDosTime, int nDST = -1);
	CTime(const CTime& timeSrc);

	CTime(const SYSTEMTIME& sysTime, int nDST = -1);
	CTime(const FILETIME& fileTime, int nDST = -1);
	const CTime& operator=(const CTime& timeSrc);
	const CTime& operator=(time_t t);

// Attributes
	struct tm* GetGmtTm(struct tm* ptm = NULL) const;
	struct tm* GetLocalTm(struct tm* ptm = NULL) const;
#if _MFC_VER >= 0x0600
	BOOL GetAsSystemTime(SYSTEMTIME& timeDest) const;
#endif

	time_t GetTime() const;
	int GetYear() const;
	int GetMonth() const;       // month of year (1 = Jan)
	int GetDay() const;         // day of month
	int GetHour() const;
	int GetMinute() const;
	int GetSecond() const;
	int GetDayOfWeek() const;   // 1=Sun, 2=Mon, ..., 7=Sat

// Operations
	// time math
	CTimeSpan operator-(CTime time) const;
	CTime operator-(CTimeSpan timeSpan) const;
	CTime operator+(CTimeSpan timeSpan) const;
	const CTime& operator+=(CTimeSpan timeSpan);
	const CTime& operator-=(CTimeSpan timeSpan);
	BOOL operator==(CTime time) const;
	BOOL operator!=(CTime time) const;
	BOOL operator<(CTime time) const;
	BOOL operator>(CTime time) const;
	BOOL operator<=(CTime time) const;
	BOOL operator>=(CTime time) const;

	// formatting using "C" strftime
	CString Format(LPCTSTR pFormat) const;
	CString FormatGmt(LPCTSTR pFormat) const;
	CString Format(UINT nFormatID) const;
	CString FormatGmt(UINT nFormatID) const;

#ifdef _UNICODE
	// for compatibility with MFC 3.x
	CString Format(LPCSTR pFormat) const;
	CString FormatGmt(LPCSTR pFormat) const;
#endif

	// serialization
#ifdef _DEBUG
	friend CDumpContext& AFXAPI operator<<(CDumpContext& dc, CTime time);
#endif
	friend CArchive& AFXAPI operator<<(CArchive& ar, CTime time);
	friend CArchive& AFXAPI operator>>(CArchive& ar, CTime& rtime);

private:
	time_t m_time;
};

/////////////////////////////////////////////////////////////////////////////
// File status

struct CFileStatus
{
	CTime m_ctime;          // creation date/time of file
	CTime m_mtime;          // last modification date/time of file
	CTime m_atime;          // last access date/time of file
	LONG m_size;            // logical size of file in bytes
	BYTE m_attribute;       // logical OR of CFile::Attribute enum values
	BYTE _m_padding;        // pad the structure to a WORD
	TCHAR m_szFullName[_MAX_PATH]; // absolute path name

#ifdef _DEBUG
	void Dump(CDumpContext& dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////
// Diagnostic memory management routines

// Low level sanity checks for memory blocks
BOOL AFXAPI AfxIsValidAddress(const void* lp,
			UINT_PTR nBytes, BOOL bReadWrite = TRUE);
BOOL AFXAPI AfxIsValidString(LPCWSTR lpsz, int nLength = -1);
BOOL AFXAPI AfxIsValidString(LPCSTR lpsz, int nLength = -1);

#if defined(_DEBUG) && !defined(_AFX_NO_DEBUG_CRT)

// Memory tracking allocation
void* AFX_CDECL operator new(size_t nSize, LPCSTR lpszFileName, int nLine) throw(...);
#define DEBUG_NEW new(THIS_FILE, __LINE__)
#if (_MSC_VER >= 1200) && (_MFC_VER >= 0x0600)
void AFX_CDECL operator delete(void* p, LPCSTR lpszFileName, int nLine);
#endif

void* AFXAPI AfxAllocMemoryDebug(size_t nSize, BOOL bIsObject,
	LPCSTR lpszFileName, int nLine);
void AFXAPI AfxFreeMemoryDebug(void* pbData, BOOL bIsObject);

// Dump any memory leaks since program started
BOOL AFXAPI AfxDumpMemoryLeaks();

// Return TRUE if valid memory block of nBytes
BOOL AFXAPI AfxIsMemoryBlock(const void* p, UINT nBytes,
	LONG* plRequestNumber = NULL);

// Return TRUE if memory is sane or print out what is wrong
BOOL AFXAPI AfxCheckMemory();

#define afxMemDF _crtDbgFlag

enum AfxMemDF // memory debug/diagnostic flags
{
	allocMemDF          = 0x01,         // turn on debugging allocator
	delayFreeMemDF      = 0x02,         // delay freeing memory
	checkAlwaysMemDF    = 0x04          // AfxCheckMemory on every alloc/free
};

#ifdef _UNICODE
#define AfxOutputDebugString(lpsz) \
	do \
	{ \
    USES_CONVERSION; \
		_RPT0(_CRT_WARN, W2CA(lpsz)); \
	} while (0)
#else
#define AfxOutputDebugString(lpsz) _RPT0(_CRT_WARN, lpsz)
#endif

// turn on/off tracking for a short while
BOOL AFXAPI AfxEnableMemoryTracking(BOOL bTrack);

// Advanced initialization: for overriding default diagnostics
BOOL AFXAPI AfxDiagnosticInit(void);

// A failure hook returns whether to permit allocation
typedef BOOL (AFXAPI* AFX_ALLOC_HOOK)(size_t nSize, BOOL bObject, LONG lRequestNumber);

// Set new hook, return old (never NULL)
AFX_ALLOC_HOOK AFXAPI AfxSetAllocHook(AFX_ALLOC_HOOK pfnAllocHook);

// Debugger hook on specified allocation request - Obsolete
void AFXAPI AfxSetAllocStop(LONG lRequestNumber);

// Memory state for snapshots/leak detection
struct CMemoryState
{
// Attributes
	enum blockUsage
	{
		freeBlock,    // memory not used
		objectBlock,  // contains a CObject derived class object
		bitBlock,     // contains ::operator new data
		crtBlock,
		ignoredBlock,
		nBlockUseMax  // total number of usages
	};

	_CrtMemState m_memState;
	LONG_PTR m_lCounts[nBlockUseMax];
	LONG_PTR m_lSizes[nBlockUseMax];
	LONG_PTR m_lHighWaterCount;
	LONG_PTR m_lTotalCount;

	CMemoryState();

// Operations
	void Checkpoint();  // fill with current state
	BOOL Difference(const CMemoryState& oldState,
					const CMemoryState& newState);  // fill with difference
	void UpdateData();

	// Output to afxDump
	void DumpStatistics() const;
	void DumpAllObjectsSince() const;
};

// Enumerate allocated objects or runtime classes
void AFXAPI AfxDoForAllObjects(void (AFX_CDECL *pfn)(CObject* pObject, void* pContext),
	void* pContext);
void AFXAPI AfxDoForAllClasses(void (AFX_CDECL *pfn)(const CRuntimeClass* pClass,
	void* pContext), void* pContext);

#else

// non-_DEBUG_ALLOC version that assume everything is OK
#define DEBUG_NEW new
#define AfxCheckMemory() TRUE
#define AfxIsMemoryBlock(p, nBytes) TRUE
#define AfxEnableMemoryTracking(bTrack) FALSE
#define AfxOutputDebugString(lpsz) ::OutputDebugString(lpsz)

// diagnostic initialization
#ifndef _DEBUG
#define AfxDiagnosticInit() TRUE
#else
BOOL AFXAPI AfxDiagnosticInit(void);
#endif

#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////
// Archives for serializing CObject data

// needed for implementation
class CPtrArray;
class CMapPtrToPtr;
class CDocument;

class CArchive
{
public:
// Flag values
	enum Mode { store = 0, load = 1, bNoFlushOnDelete = 2, bNoByteSwap = 4 };

	CArchive(CFile* pFile, UINT nMode, int nBufSize = 4096, void* lpBuf = NULL);
	~CArchive();

// Attributes
	BOOL IsLoading() const;
	BOOL IsStoring() const;
	BOOL IsByteSwapping() const;
	BOOL IsBufferEmpty() const;

	CFile* GetFile() const;
	UINT GetObjectSchema(); // only valid when reading a CObject*
	void SetObjectSchema(UINT nSchema);

	// pointer to document being serialized -- must set to serialize
	//  COleClientItems in a document!
	CDocument* m_pDocument;

// Operations
	UINT Read(void* lpBuf, UINT nMax);
	void Write(const void* lpBuf, UINT nMax);
	void Flush();
	void Close();
	void Abort();   // close and shutdown without exceptions

	// reading and writing strings
	void WriteString(LPCTSTR lpsz);
	LPTSTR ReadString(__out_xcount_opt(((int)(nMax)) * (((nMax >> 31) * -1) | 0x01)) LPTSTR lpsz, UINT nMax);
	BOOL ReadString(CString& rString);

public:
	// Object I/O is pointer based to avoid added construction overhead.
	// Use the Serialize member function directly for embedded objects.
	friend CArchive& AFXAPI operator<<(CArchive& ar, const CObject* pOb);

	friend CArchive& AFXAPI operator>>(CArchive& ar, CObject*& pOb);
	friend CArchive& AFXAPI operator>>(CArchive& ar, const CObject*& pOb);

	// insertion operations
	CArchive& operator<<(BYTE by);
	CArchive& operator<<(WORD w);
	CArchive& operator<<(LONG l);
	CArchive& operator<<(DWORD dw);
	CArchive& operator<<(ULONGLONG qw);
	CArchive& operator<<(float f);
	CArchive& operator<<(double d);

	CArchive& operator<<(int i);
	CArchive& operator<<(short w);
	CArchive& operator<<(char ch);
	CArchive& operator<<(unsigned u);

	// extraction operations
	CArchive& operator>>(BYTE& by);
	CArchive& operator>>(WORD& w);
	CArchive& operator>>(DWORD& dw);
	CArchive& operator>>(LONG& l);
	CArchive& operator>>(ULONGLONG& qw);
	CArchive& operator>>(float& f);
	CArchive& operator>>(double& d);

	CArchive& operator>>(int& i);
	CArchive& operator>>(short& w);
	CArchive& operator>>(char& ch);
	CArchive& operator>>(unsigned& u);

	// object read/write
	CObject* ReadObject(const CRuntimeClass* pClass);
	void WriteObject(const CObject* pOb);
	// advanced object mapping (used for forced references)
	void MapObject(const CObject* pOb);

	// advanced versioning support
	void WriteClass(const CRuntimeClass* pClassRef);
	CRuntimeClass* ReadClass(const CRuntimeClass* pClassRefRequested = NULL,
		UINT* pSchema = NULL, DWORD* pObTag = NULL);
	void SerializeClass(const CRuntimeClass* pClassRef);

	// advanced operations (used when storing/loading many objects)
	void SetStoreParams(UINT nHashSize = 2053, UINT nBlockSize = 128);
	void SetLoadParams(UINT nGrowBy = 1024);

// Implementation
public:
	BOOL m_bForceFlat;  // for COleClientItem implementation (default TRUE)
	BOOL m_bDirectBuffer;   // TRUE if m_pFile supports direct buffering
	void FillBuffer(UINT nBytesNeeded);
	void CheckCount();  // throw exception if m_nMapCount is too large

	// special functions for reading and writing (16-bit compatible) counts
	DWORD_PTR ReadCount();
	void WriteCount(DWORD_PTR dwCount);

	// public for advanced use
	UINT m_nObjectSchema;
	CString m_strFileName;

protected:
	// archive objects cannot be copied or assigned
	CArchive(const CArchive& arSrc);
	void operator=(const CArchive& arSrc);

	BOOL m_nMode;
	BOOL m_bUserBuf;
	int m_nBufSize;
	CFile* m_pFile;
	BYTE* m_lpBufCur;
	BYTE* m_lpBufMax;
	BYTE* m_lpBufStart;

	// array/map for CObject* and CRuntimeClass* load/store
	UINT m_nMapCount;
	union
	{
		CPtrArray* m_pLoadArray;
		CMapPtrToPtr* m_pStoreMap;
	};
	// map to keep track of mismatched schemas
	CMapPtrToPtr* m_pSchemaMap;

	// advanced parameters (controls performance with large archives)
	UINT m_nGrowSize;
	UINT m_nHashSize;
};

/////////////////////////////////////////////////////////////////////////////
// Diagnostic dumping

#if _MFC_VER >= 0x0600
// Note: AfxDumpStack is available in release builds, although it is always
//	statically linked so as to not negatively affect the size of MFC42.DLL.

#define AFX_STACK_DUMP_TARGET_TRACE			0x0001
#define AFX_STACK_DUMP_TARGET_CLIPBOARD	0x0002
#define AFX_STACK_DUMP_TARGET_BOTH			0x0003
#define AFX_STACK_DUMP_TARGET_ODS			0x0004
#ifdef _DEBUG
#define AFX_STACK_DUMP_TARGET_DEFAULT		AFX_STACK_DUMP_TARGET_TRACE
#else
#define AFX_STACK_DUMP_TARGET_DEFAULT		AFX_STACK_DUMP_TARGET_CLIPBOARD
#endif

void AFXAPI AfxDumpStack(DWORD dwFlags = AFX_STACK_DUMP_TARGET_DEFAULT);
#endif

class CDumpContext
{
public:
	CDumpContext(CFile* pFile = NULL);

// Attributes
	int GetDepth() const;      // 0 => this object, 1 => children objects
	void SetDepth(int nNewDepth);

// Operations
	CDumpContext& operator<<(LPCTSTR lpsz);
#ifdef _UNICODE
	CDumpContext& operator<<(LPCSTR lpsz);  // automatically widened
#else
	CDumpContext& operator<<(LPCWSTR lpsz); // automatically thinned
#endif
	CDumpContext& operator<<(const void* lp);
	CDumpContext& operator<<(const CObject* pOb);
	CDumpContext& operator<<(const CObject& ob);
	CDumpContext& operator<<(BYTE by);
	CDumpContext& operator<<(WORD w);
	CDumpContext& operator<<(UINT u);
	CDumpContext& operator<<(LONG l);
	CDumpContext& operator<<(DWORD dw);
	CDumpContext& operator<<(float f);
	CDumpContext& operator<<(double d);
	CDumpContext& operator<<(int n);
#ifdef _WIN64
    CDumpContext& operator<<(__int64 n);
    CDumpContext& operator<<(unsigned __int64 n);
#endif
	void HexDump(LPCTSTR lpszLine, BYTE* pby, int nBytes, int nWidth);
	void Flush();

// Implementation
protected:
	// dump context objects cannot be copied or assigned
	CDumpContext(const CDumpContext& dcSrc);
	void operator=(const CDumpContext& dcSrc);
	void OutputString(LPCTSTR lpsz);

	int m_nDepth;

public:
	CFile* m_pFile;
};

#ifdef _DEBUG
extern AFX_DATA CDumpContext afxDump;
extern AFX_DATA BOOL afxTraceEnabled;
#endif

/////////////////////////////////////////////////////////////////////////////
// Special include for Win32s compatibility

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#ifndef __AFXCOLL_H__
	#include <afxcoll.h>
	#ifndef __AFXSTATE_H__
		#include <afxstat_.h> // for _AFX_APP_STATE and _AFX_THREAD_STATE
	#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// Inline function declarations

#ifdef _AFX_ENABLE_INLINES
#define _AFX_INLINE AFX_INLINE

#if !defined(_AFX_CORE_IMPL) || !defined(_AFXDLL) || defined(_DEBUG)
#define _AFX_PUBLIC_INLINE AFX_INLINE
#else
#define _AFX_PUBLIC_INLINE
#endif

#include <afx.inl>
#endif

#if (_MFC_VER >= 0x0600) && defined(UNICODE)

inline PCTSTR CString::GetString() const
{
	return operator PCTSTR();
}

inline CString& CString::Trim()
{
	TrimRight();
	TrimLeft();
	return *this;
}

inline CString& CString::Trim(__in TCHAR chTarget)
{
	TrimRight(chTarget);
	TrimLeft(chTarget);
	return *this;
}

inline CString& CString::Trim(__in PCTSTR pszTargets)
{
	TrimRight(pszTargets);
	TrimLeft(pszTargets);
	return *this;
}

#if defined(__cplusplus_cli)

#include <vcclr.h>

inline CString::CString(System::String^ pString)
{
	Init();
	pin_ptr<const System::Char> pChar = PtrToStringChars(pString);
	const wchar_t* psz = pChar;
	*this = psz;
}

#endif
#endif

#if defined(_UNICODE) && defined(_NATIVE_WCHAR_T_DEFINED)

inline CString::CString(WCHAR ch, int nRepeat)                      { Init(); *this = CString((unsigned short)ch, nRepeat); }
inline CString::CString(PCWSTR lpsz)                                { Init(); *this = lpsz; }
inline CString::CString(PCWSTR lpch, int nLength)                   { Init(); *this = CString((const unsigned short*)lpch, nLength); }
inline void CString::SetAt(int nIndex, WCHAR ch)                    { SetAt(nIndex, (unsigned short)ch); }
inline const CString& CString::operator=(WCHAR ch)                  { return operator=((unsigned short)ch); }
inline const CString& CString::operator=(PCWSTR psz)                { return operator=((const unsigned short*)psz); }
inline const CString& CString::operator+=(WCHAR ch)                 { return operator+=((unsigned short)ch); }
inline const CString& CString::operator+=(PCWSTR lpsz)              { return operator+=((const unsigned short*)lpsz); }
inline CString AFXAPI operator+(const CString& string, WCHAR ch)    { return (string + ((unsigned short)ch)); }
inline CString AFXAPI operator+(WCHAR ch, const CString& string)    { return (((unsigned short)ch) + string); }
inline CString AFXAPI operator+(const CString& string, PCWSTR lpsz) { return (string + ((const unsigned short*)lpsz)); }
inline CString AFXAPI operator+(PCWSTR lpsz, const CString& string) { return (((const unsigned short*)lpsz) + string); }
inline CString CString::SpanIncluding(PCWSTR lpszCharSet) const     { return SpanIncluding((const unsigned short*)lpszCharSet); }
inline CString CString::SpanExcluding(PCWSTR lpszCharSet) const     { return SpanExcluding((const unsigned short*)lpszCharSet); }
inline void CString::TrimRight(WCHAR chTarget)                      { TrimRight((unsigned short)chTarget); }
inline void CString::TrimRight(PCWSTR lpszTargets)                  { TrimRight((const unsigned short*)lpszTargets); }
inline void CString::TrimLeft(WCHAR chTarget)                       { TrimLeft((unsigned short)chTarget); }
inline void CString::TrimLeft(PCWSTR lpszTargets)                   { TrimLeft((const unsigned short*)lpszTargets); }
inline int CString::Replace(WCHAR chOld, WCHAR chNew)               { return Replace((unsigned short)chOld, (unsigned short)chNew); }
inline int CString::Replace(PCWSTR lpszOld, PCWSTR lpszNew)         { return Replace((const unsigned short*)lpszOld, (const unsigned short*)lpszNew); }
inline int CString::Remove(WCHAR chRemove)                          { return Remove((unsigned short)chRemove); }
inline int CString::Insert(int nIndex, WCHAR ch)                    { return Insert(nIndex, (unsigned short)ch); }
inline int CString::Insert(int nIndex, __in_z PCWSTR pstr)          { return Insert(nIndex, (const unsigned short*)pstr); }
inline int CString::Find(WCHAR ch) const                            { return Find((unsigned short)ch); }
inline int CString::ReverseFind(WCHAR ch) const                     { return ReverseFind((unsigned short)ch); }
inline int CString::Find(WCHAR ch, int nStart) const                { return Find((unsigned short)ch); }
inline int CString::FindOneOf(PCWSTR lpszCharSet) const             { return FindOneOf((const unsigned short*)lpszCharSet); }
inline int CString::Find(PCWSTR lpszSub) const                      { return Find((const unsigned short*)lpszSub); }
inline int CString::Find(PCWSTR lpszSub, int nStart) const          { return Find((const unsigned short*)lpszSub, nStart); }
inline void AFX_CDECL CString::Format(PCWSTR lpszFormat, ...)       { va_list argList; va_start(argList, lpszFormat); FormatV((const unsigned short*)lpszFormat, argList); va_end(argList); }
inline void CString::FormatV(PCWSTR lpszFormat, va_list argList)    { FormatV((const unsigned short*)lpszFormat, argList); }
// Theoretically these belong here, but these functions are
//always inlined (independent of /Zc:wchar_t) in the Windows fork since
//they were introduced "late". See earlier in afx.h for their
//definition.
//inline CString& CString::Trim(__in WCHAR chTarget)                { return Trim((unsigned short)chTarget); }
//inline CString& CString::Trim(__in PCWSTR pszTargets)             { return Trim((const unsigned short*)pszTargtets); }
inline CString::operator PCWSTR() const                             { return m_pchData; }

#if !defined(_AFX_INLINE)

inline int CString::Compare(PCWSTR lpsz) const                      { return Compare((const unsigned short*)lpsz); }
inline int CString::CompareNoCase(PCWSTR lpsz) const                { return CompareNoCase((const unsigned short*)lpsz); }
inline int CString::Collate(PCWSTR lpsz) const                      { return Collate((const unsigned short*)lpsz); }
#if _MFC_VER >= 0x0600
inline int CString::CollateNoCase(PCWSTR lpsz) const                { return CollateNoCase((const unsigned short*)lpsz); }
#endif

inline bool AFXAPI operator==(const CString& s1, PCWSTR s2) { return (s1 == ((const unsigned short*)s2)); }
inline bool AFXAPI operator==(PCWSTR s1, const CString& s2) { return (((const unsigned short*)s1) == s2); }
inline bool AFXAPI operator!=(const CString& s1, PCWSTR s2) { return (s1 != ((const unsigned short*)s2)); }
inline bool AFXAPI operator!=(PCWSTR s1, const CString& s2) { return (((const unsigned short*)s1) != s2); }
inline bool AFXAPI operator<(const CString& s1, PCWSTR s2)  { return (s1 < ((const unsigned short*)s2)); }
inline bool AFXAPI operator<(PCWSTR s1, const CString& s2)  { return (((const unsigned short*)s1) < s2); }
inline bool AFXAPI operator>(const CString& s1, PCWSTR s2)  { return (s1 > ((const unsigned short*)s2)); }
inline bool AFXAPI operator>(PCWSTR s1, const CString& s2)  { return (((const unsigned short*)s1) > s2); }
inline bool AFXAPI operator<=(const CString& s1, PCWSTR s2) { return (s1 <= ((const unsigned short*)s2)); }
inline bool AFXAPI operator<=(PCWSTR s1, const CString& s2) { return (((const unsigned short*)s1) <= s2); }
inline bool AFXAPI operator>=(const CString& s1, PCWSTR s2) { return (s1 >= ((const unsigned short*)s2)); }
inline bool AFXAPI operator>=(PCWSTR s1, const CString& s2) { return (((const unsigned short*)s1) >= s2); }

#endif // _AFX_INLINE
#endif // _NATIVE_WCHAR_T_DEFINED

#undef AFX_DATA
#define AFX_DATA

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif
#ifndef _AFX_FULLTYPEINFO
#pragma component(mintypeinfo, off)
#endif

#endif // __AFX_H__

/////////////////////////////////////////////////////////////////////////////

