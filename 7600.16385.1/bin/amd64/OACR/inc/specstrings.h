/***************************************************************\
*                                                               *
* SpecStrings.h - markers for documenting the semantics of APIs *
*                                                               *
* Version 1.0                                                   *
*                                                               *
* Copyright (c) Microsoft Corporation. All rights reserved.     *
*                                                               *
\***************************************************************/

// -------------------------------------------------------------------------------
// Introduction
//
// SpecStrings.h provides a set of annotations to describe how a function uses its
// parameters - the assumptions it makes about them, and the guarantees it makes
// upon finishing.
//
// Annotations may be placed before either a function parameter's type or its return
// type, and describe the function's behavior regarding the parameter or return value.
// There are two classes of annotations: buffer annotations and advanced annotations.
// Buffer annotations describe how functions use their pointer parameters, and
// advanced annotations either describe complex/unusual buffer behavior, or provide
// additional information about a parameter that is not otherwise expressible.
//
// -------------------------------------------------------------------------------
// Buffer Annotations
//
// The most important annotations in SpecStrings.h provide a consistent way to annotate
// buffer parameters or return values for a function. Each of these annotations describes
// a single buffer (which could be a string, a fixed-length or variable-length array,
// or just a pointer) that the function interacts with: where it is, how large it is,
// how much is initialized, and what the function does with it.
//
// The appropriate macro for a given buffer can be constructed using the table below.
// Just pick the appropriate values from each category, and combine them together
// with a leading underscore. Some combinations of values do not make sense as buffer
// annotations. Only meaningful annotations can be added to your code; for a list of
// these, see the buffer annotation definitions section.
//
// Only a single buffer annotation should be used for each parameter.
//
// |------------|------------|---------|--------|----------|---------------|
// |   Level    |   Usage    |  Size   | Output | Optional |  Parameters   |
// |------------|------------|---------|--------|----------|---------------|
// | <>         | <>         | <>      | <>     | <>       | <>            |
// | _deref     | _in        | _ecount | _full  | _opt     | (size)        |
// | _deref_opt | _out       | _bcount | _part  |          | (size,length) |
// |            | _inout     |         |        |          |               |
// |            |            |         |        |          |               |
// |------------|------------|---------|--------|----------|---------------|
//
// Level: Describes the buffer pointer's level of indirection from the parameter or
//          return value 'p'.
//
// <>         : p is the buffer pointer.
// _deref     : *p is the buffer pointer. p must not be NULL.
// _deref_opt : *p may be the buffer pointer. p may be NULL, in which case the rest of
//                the annotation is ignored.
//
// Usage: Describes how the function uses the buffer.
//
// <>     : The buffer is not accessed. If used on the return value or with _deref, the
//            function will provide the buffer, and it will be uninitialized at exit.
//            Otherwise, the caller must provide the buffer. This should only be used
//            for alloc and free functions.
// _in    : The function will only read from the buffer. The caller must provide the
//            buffer and initialize it.
// _out   : The function will only write to the buffer. If used on the return value or
//            with _deref, the function will provide the buffer and initialize it.
//            Otherwise, the caller must provide the buffer, and the function will
//            initialize it.
// _inout : The function may freely read from and write to the buffer. The caller must
//            provide the buffer and initialize it. If used with _deref, the buffer may
//            be reallocated by the function.
//
// Size: Describes the total size of the buffer. This may be less than the space actually
//         allocated for the buffer, in which case it describes the accessible amount.
//
// <>      : No buffer size is given. If the type specifies the buffer size (such as
//             with LPSTR and LPWSTR), that amount is used. Otherwise, the buffer is one
//             element long. Must be used with _in, _out, or _inout.
// _ecount : The buffer size is an explicit element count.
// _bcount : The buffer size is an explicit byte count.
//
// Output: Describes how much of the buffer will be initialized by the function. For
//           _inout buffers, this also describes how much is initialized at entry. Omit this
//           category for _in buffers; they must be fully initialized by the caller.
//
// <>    : The type specifies how much is initialized. For instance, a function initializing
//           an LPWSTR must NULL-terminate the string.
// _full : The function initializes the entire buffer.
// _part : The function initializes part of the buffer, and explicitly indicates how much.
//
// Optional: Describes if the buffer itself is optional.
//
// <>   : The pointer to the buffer must not be NULL.
// _opt : The pointer to the buffer might be NULL. It will be checked before being dereferenced.
//
// Parameters: Gives explicit counts for the size and length of the buffer.
//
// <>            : There is no explicit count. Use when neither _ecount nor _bcount is used.
// (size)        : Only the buffer's total size is given. Use with _ecount or _bcount but not _part.
// (size,length) : The buffer's total size and initialized length are given. Use with _ecount_part
//                   and _bcount_part.
//
// -------------------------------------------------------------------------------
// Buffer Annotation Examples
//
// LWSTDAPI_(BOOL) StrToIntExA(
//     LPCSTR pszString,                    // No annotation required, const implies __in.
//     DWORD dwFlags,
//     __out int *piRet                     // A pointer whose dereference will be filled in.
// );
//
// void MyPaintingFunction(
//     __in HWND hwndControl,               // An initialized read-only parameter.
//     __in_opt HDC hdcOptional,            // An initialized read-only parameter that might be NULL.
//     __inout IPropertyStore *ppsStore     // An initialized parameter that may be freely used
//                                          //   and modified.
// );
//
// LWSTDAPI_(BOOL) PathCompactPathExA(
//     __out_ecount(cchMax) LPSTR pszOut,   // A string buffer with cch elements that will
//                                          //   be NULL terminated on exit.
//     LPCSTR pszSrc,                       // No annotation required, const implies __in.
//     UINT cchMax,
//     DWORD dwFlags
// );
//
// HRESULT SHLocalAllocBytes(
//     size_t cb,
//     __deref_bcount(cb) T **ppv           // A pointer whose dereference will be set to an
//                                          //   uninitialized buffer with cb bytes.
// );
//
// __inout_bcount_full(cb) : A buffer with cb elements that is fully initialized at
//     entry and exit, and may be written to by this function.
//
// __out_ecount_part(count, *countOut) : A buffer with count elements that will be
//     partially initialized by this function. The function indicates how much it
//     initialized by setting *countOut.
//
// -------------------------------------------------------------------------------
// Advanced Annotations
//
// Advanced annotations describe behavior that is not expressible with the regular
// buffer macros. These may be used either to annotate buffer parameters that involve
// complex or conditional behavior, or to enrich existing annotations with additional
// information.
//
// __success(expr) f :
//     <expr> indicates whether function f succeeded or not. If <expr> is true at exit,
//     all the function's guarantees (as given by other annotations) must hold. If <expr>
//     is false at exit, the caller should not expect any of the function's guarantees
//     to hold. If not used, the function must always satisfy its guarantees. Added
//     automatically to functions that indicate success in standard ways, such as by
//     returning an HRESULT.
//
// __out_awcount(expr, size) p :
//     Pointer p is a buffer whose size may be given in either bytes or elements. If
//     <expr> is true, this acts like __out_bcount. If <expr> is false, this acts
//     like __out_ecount. This should only be used to annotate old APIs.
//
// __in_awcount(expr, size) p :
//     Pointer p is a buffer whose size may be given in either bytes or elements. If
//     <expr> is true, this acts like __in_bcount. If <expr> is false, this acts
//     like __in_ecount. This should only be used to annotate old APIs.
//
// __nullterminated p :
//     Pointer p is a buffer that may be read or written up to and including the first
//     NULL character or pointer. May be used on typedefs, which marks valid (properly
//     initialized) instances of that type as being NULL-terminated.
//
// __nullnullterminated p :
//     Pointer p is a buffer that may be read or written up to and including the first
//     sequence of two NULL characters or pointers. May be used on typedefs, which marks
//     valid instances of that type as being double-NULL terminated.
//
// __reserved v :
//     Value v must be 0/NULL, reserved for future use.
//
// __checkReturn v :
//     Return value v must not be ignored by callers of this function.
//
// __typefix(ctype) v :
//     Value v should be treated as an instance of ctype, rather than its declared type.
//
// __override f :
//     Specify C#-style 'override' behaviour for overriding virtual methods.
//
// __callback f :
//     Function f can be used as a function pointer.
//
// __format_string p :
//     Pointer p is a string that contains % markers in the style of printf.
//
// __blocksOn(resource) f :
//     Function f blocks on the resource 'resource'.
//
// __fallthrough :
//     Annotates switch statement labels where fall-through is desired, to distinguish
//     from forgotten break statements.
//
// __in_range(constant_low_bnd, constant_up_bnd) i :
//     Precondition that integer i must be in the inclusive numeric range [constant_low_bnd, constant_up_bnd].
//     constant_low_bnd and constant_up_bnd are compile time constants known to the programmer.
//
// __in_bound i :
//     Precondition that integer i must be bound, but the exact range can't be specified at compile time.
//     __in_range should be used if the range can be explicitly stated.
//
// __out_range(constant_low_bnd, constant_up_bnd) i :
//     Postcondition that integer i must be in the inclusive numeric range [constant_low_bnd, constant_up_bnd].
//     constant_low_bnd and constant_up_bnd are compile time constants known to the programmer.
//
// __out_bound i :
//     Postcondition that integer i must be bound, but the exact range can't be specified at compile time.
//     __out_range should be used if the range can be explicitly stated.
//
// __deref_out_range(constant_low_bnd, constant_up_bnd) pi :
//     Postcondition that integer *pi must be in the inclusive numeric range [constant_low_bnd, constant_up_bnd].
//     constant_low_bnd and constant_up_bnd are compile time constants known to the programmer.
//
// __deref_out_bound pi :
//     Postcondition that integer *pi must be bound, but the exact range can't be specified at compile time.
//     __deref_out_range should be used if the range can be explicitly stated.
//
// __assume_bound i:
//     Assume that the integer i is bound to some known range. This can be used to suppress
//     integer overflow warnings on integral expressions that are known to be bound due to 
//     reasons not explicit in the code.
//
// __allocator :
//     Function allocates memory using an integral size argument
// -------------------------------------------------------------------------------
// Advanced Annotation Examples
//
// __success(return == TRUE) LWSTDAPI_(BOOL) 
// PathCanonicalizeA(__out_ecount(MAX_PATH) LPSTR pszBuf, LPCSTR pszPath) :
//     pszBuf is only guaranteed to be NULL-terminated when TRUE is returned.
//
// typedef __nullterminated WCHAR* LPWSTR : Initialized LPWSTRs are NULL-terminated strings.
//
// __out_ecount(cch) __typefix(LPWSTR) void *psz : psz is a buffer parameter which will be
//     a NULL-terminated WCHAR string at exit, and which initially contains cch WCHARs.
//
// -------------------------------------------------------------------------------

#if _MSC_VER > 1000
#pragma once
#endif  // #if _MSC_VER > 1000

#define __specstrings

#ifdef  __cplusplus
#ifndef __nothrow
# define __nothrow __declspec(nothrow)
#endif
extern "C" {
#else
#ifndef __nothrow
# define __nothrow
#endif
#endif  // #ifdef __cplusplus

// OFFICEDEV: BEGIN (HannesR)

#ifndef __USE_ATTRIBUTES_FOR_SAL
#define __USE_ATTRIBUTES_FOR_SAL 0
#endif

#if defined( _PREFAST_) && !__USE_ATTRIBUTES_FOR_SAL

#if( 1400 <= _MSC_VER ) && !defined __USE_DECLSPECS_FOR_SAL
#define __USE_ATTRIBUTES_FOR_SAL 1
#elif !defined __USE_DECLSPECS_FOR_SAL
#define __USE_DECLSPECS_FOR_SAL  1
#endif

#else

#define __USE_DECLSPECS_FOR_SAL  0

#endif

//
// SAL Middle layer -------------------------------------------------------------
//
// this set of annotations allow more flexibility to annotate special cases
// of parameter semantics. They can be used instead or augment the common
// in|out|inout annotations.
// Each macro has tree version defined:
// - Whidbey attribute implementation
// - PREfast 2.1 __declspec implementation
// - regular foreground compilation
//

#if ( 1400 <= _MSC_VER )&& !defined( MIDL_PASS ) && __USE_ATTRIBUTES_FOR_SAL

//BEGIN #include "sourceannotations.h" // attribute declarations

#ifndef _SIZE_T_DEFINED
#ifdef  _WIN64
typedef unsigned __int64    size_t;
#else
typedef __w64 unsigned int  size_t;
#endif  // !_WIN64
#define _SIZE_T_DEFINED
#endif  // !_SIZE_T_DEFINED

#if !defined(_WCHAR_T_DEFINED) && !defined(_NATIVE_WCHAR_T_DEFINED)
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#pragma push_macro( "SA" )
#pragma push_macro( "REPEATABLE" )

#ifdef __cplusplus
#define SA( id ) id
#define REPEATABLE [repeatable]
#else  // !__cplusplus
#define SA( id ) SA_##id
#define REPEATABLE
#endif  // !__cplusplus

#ifdef __cplusplus
namespace vc_attributes
{
#endif  // __cplusplus

enum SA( YesNoMaybe )
{
	// Choose values that we can detect as invalid if they are or'd together
	SA( No ) = 0x0fff0001,
	SA( Maybe ) = 0x0fff0010,
	SA( Yes ) = 0x0fff0100
};

typedef enum SA( YesNoMaybe ) SA( YesNoMaybe );

enum SA( AccessType )
{
	SA( NoAccess ) = 0,
	SA( Read ) = 1,
	SA( Write ) = 2,
	SA( ReadWrite ) = 3
};

typedef enum SA( AccessType ) SA( AccessType );

#ifndef SAL_NO_ATTRIBUTE_DECLARATIONS

REPEATABLE
[source_annotation_attribute( SA( Parameter ) )]
struct PreAttribute
{
#ifdef __cplusplus
	PreAttribute();
#endif

	unsigned int Deref;
	SA( YesNoMaybe ) Valid;
	SA( YesNoMaybe ) Null;
	SA( YesNoMaybe ) Tainted;
	SA( YesNoMaybe ) NeedsRelease;
	SA( AccessType ) Access;
	size_t ValidElementsConst;
	size_t ValidBytesConst;
	const wchar_t* ValidElements;
	const wchar_t* ValidBytes;
	const wchar_t* ValidElementsLength;
	const wchar_t* ValidBytesLength;
	size_t WritableElementsConst;
	size_t WritableBytesConst;
	const wchar_t* WritableElements;
	const wchar_t* WritableBytes;
	const wchar_t* WritableElementsLength;
	const wchar_t* WritableBytesLength;
	size_t ElementsSizeConst;
	const wchar_t* ElementsSize;
	SA( YesNoMaybe ) NullTerminated;
	const wchar_t* Condition;
};

REPEATABLE
[source_annotation_attribute( SA( Parameter )|SA( ReturnValue ) )]
struct PostAttribute
{
#ifdef __cplusplus
	PostAttribute();
#endif

	unsigned int Deref;
	SA( YesNoMaybe ) Valid;
	SA( YesNoMaybe ) Null;
	SA( YesNoMaybe ) Tainted;
	SA( YesNoMaybe ) NeedsRelease;
	SA( AccessType ) Access;
	size_t ValidElementsConst;
	size_t ValidBytesConst;
	const wchar_t* ValidElements;
	const wchar_t* ValidBytes;
	const wchar_t* ValidElementsLength;
	const wchar_t* ValidBytesLength;
	size_t WritableElementsConst;
	size_t WritableBytesConst;
	const wchar_t* WritableElements;
	const wchar_t* WritableBytes;
	const wchar_t* WritableElementsLength;
	const wchar_t* WritableBytesLength;
	size_t ElementsSizeConst;
	const wchar_t* ElementsSize;
	SA( YesNoMaybe ) NullTerminated;
	SA( YesNoMaybe ) MustCheck;
	const wchar_t* Condition;
};

[source_annotation_attribute( SA( Parameter ) )]
struct FormatStringAttribute
{
#ifdef __cplusplus
	FormatStringAttribute();
#endif

	const wchar_t* Style;
	const wchar_t* UnformattedAlternative;
};

REPEATABLE
[source_annotation_attribute( SA( ReturnValue ) )]
struct InvalidCheckAttribute
{
#ifdef __cplusplus
	InvalidCheckAttribute();
#endif

	long Value;
};

#endif  // !SAL_NO_ATTRIBUTE_DECLARATIONS

#ifdef __cplusplus
};  // namespace vc_attributes
#endif  // __cplusplus

#pragma pop_macro( "REPEATABLE" )
#pragma pop_macro( "SA" )

#ifdef __cplusplus

#define SA_All All
#define SA_Class Class
#define SA_Constructor Constructor
#define SA_Delegate Delegate
#define SA_Enum Enum
#define SA_Event Event
#define SA_Field Field
#define SA_GenericParameter GenericParameter
#define SA_Interface Interface
#define SA_Method Method
#define SA_Module Module
#define SA_Parameter Parameter
#define SA_Property Property
#define SA_ReturnValue ReturnValue
#define SA_Struct Struct

typedef ::vc_attributes::YesNoMaybe SA_YesNoMaybe;
const ::vc_attributes::YesNoMaybe SA_Yes = ::vc_attributes::Yes;
const ::vc_attributes::YesNoMaybe SA_No = ::vc_attributes::No;
const ::vc_attributes::YesNoMaybe SA_Maybe = ::vc_attributes::Maybe;

typedef ::vc_attributes::AccessType SA_AccessType;
const ::vc_attributes::AccessType SA_NoAccess = ::vc_attributes::NoAccess;
const ::vc_attributes::AccessType SA_Read = ::vc_attributes::Read;
const ::vc_attributes::AccessType SA_Write = ::vc_attributes::Write;
const ::vc_attributes::AccessType SA_ReadWrite = ::vc_attributes::ReadWrite;

#ifndef SAL_NO_ATTRIBUTE_DECLARATIONS
typedef ::vc_attributes::PreAttribute SA_Pre;
typedef ::vc_attributes::PostAttribute SA_Post;
typedef ::vc_attributes::FormatStringAttribute SA_FormatString;
typedef ::vc_attributes::InvalidCheckAttribute SA_InvalidCheck;
#endif //!SAL_NO_ATTRIBUTE_DECLARATIONS

#else  // !__cplusplus

typedef struct PreAttribute SA_Pre;
typedef struct PreAttribute PreAttribute;

typedef struct PostAttribute SA_Post;
typedef struct PostAttribute PostAttribute;

typedef struct FormatStringAttribute SA_FormatString;
typedef struct FormatStringAttribute FormatStringAttribute;

typedef struct InvalidCheckAttribute SA_InvalidCheck;
typedef struct InvalidCheckAttribute InvalidCheckAttribute;

#endif  // __cplusplus
//END #include "sourceannotations.h" // attribute declarations

//
// __pre annotation ---
//
// describing conditions that must be met before the call of the function

// e.g. int strlen( __pre_z const char* sz );
// buffer is a zero terminated string
#define __pre_z                          [SA_Pre(Null=SA_No,   Valid=SA_Yes,NullTerminated=SA_Yes)]
#define __pre_opt_z                      [SA_Pre(Null=SA_Maybe,Valid=SA_Yes,NullTerminated=SA_Yes)]

// e.g. void FreeMemory( __pre_bytecap(cb) __post_invalid void* pv, size_t cb );
// buffer capacity described by another parameter
#define __pre_cap(cap)                   [SA_Pre(Null=SA_No,   WritableElements=     #cap)]
#define __pre_opt_cap(cap)               [SA_Pre(Null=SA_Maybe,WritableElements     =#cap)]
#define __pre_bytecap(cap)               [SA_Pre(Null=SA_No,   WritableBytes        =#cap)]
#define __pre_opt_bytecap(cap)           [SA_Pre(Null=SA_Maybe,WritableBytes        =#cap)]

// buffer capacity described by a constant expression
#define __pre_cap_c(constExpr)           [SA_Pre(Null=SA_No,   WritableElementsConst=constExpr)]
#define __pre_opt_cap_c(constExpr)       [SA_Pre(Null=SA_Maybe,WritableElementsConst=constExpr)]
#define __pre_bytecap_c(constExpr)       [SA_Pre(Null=SA_No,   WritableBytesConst   =constExpr)]
#define __pre_opt_bytecap_c(constExpr)   [SA_Pre(Null=SA_Maybe,WritableBytesConst   =constExpr)]

// buffer capacity described by a complex condition
#define __pre_cap_x(string)              [SA_Pre(Null=SA_No,   WritableElements="\n"#string)]
#define __pre_opt_cap_x(string)          [SA_Pre(Null=SA_Maybe,WritableElements="\n"#string)]
#define __pre_bytecap_x(string)          [SA_Pre(Null=SA_No,   WritableBytes   ="\n"#string)]
#define __pre_opt_bytecap_x(string)      [SA_Pre(Null=SA_Maybe,WritableBytes   ="\n"#string)]

// buffer capacity described by the difference to another pointer parameter
#define __pre_ptrdiff_cap(ptr)           [SA_Pre(Null=SA_No   ,WritableElements="\n__ptrdiff("#ptr")")]
#define __pre_opt_ptrdiff_cap(ptr)       [SA_Pre(Null=SA_Maybe,WritableElements="\n__ptrdiff("#ptr")")]

// e.g. void AppendStr( __pre_z const char* szFrom, __pre_z_cap(cchTo) __post_z char* szTo, size_t cchTo );
#define __pre_z_cap(cap)                 [SA_Pre(Null=SA_No,   Valid=SA_Yes,NullTerminated=SA_Yes,WritableElements=#cap)]
#define __pre_opt_z_cap(cap)             [SA_Pre(Null=SA_Maybe,Valid=SA_Yes,NullTerminated=SA_Yes,WritableElements=#cap)]
#define __pre_z_bytecap(cap)             [SA_Pre(Null=SA_No,   Valid=SA_Yes,NullTerminated=SA_Yes,WritableBytes   =#cap)]
#define __pre_opt_z_bytecap(cap)         [SA_Pre(Null=SA_Maybe,Valid=SA_Yes,NullTerminated=SA_Yes,WritableBytes   =#cap)]

#define __pre_z_cap_c(constExpr)         [SA_Pre(Null=SA_No,   Valid=SA_Yes,NullTerminated=SA_Yes,WritableElementsConst=constExpr)]
#define __pre_opt_z_cap_c(constExpr)     [SA_Pre(Null=SA_Maybe,Valid=SA_Yes,NullTerminated=SA_Yes,WritableElementsConst=constExpr)]
#define __pre_z_bytecap_c(constExpr)     [SA_Pre(Null=SA_No,   Valid=SA_Yes,NullTerminated=SA_Yes,WritableBytesConst   =constExpr)]
#define __pre_opt_z_bytecap_c(constExpr) [SA_Pre(Null=SA_Maybe,Valid=SA_Yes,NullTerminated=SA_Yes,WritableBytesConst   =constExpr)]

// e.g. void AppendCharRange( __pre_count(cchFrom) const char* rgFrom, size_t cchFrom, __out_z_cap(cchTo) char* szTo, size_t cchTo );
// Valid buffer extent described by another parameter
#define __pre_count(count)               [SA_Pre(Null=SA_No,   Valid=SA_Yes,ValidElements=#count)]
#define __pre_opt_count(count)           [SA_Pre(Null=SA_Maybe,Valid=SA_Yes,ValidElements=#count)]
#define __pre_bytecount(count)           [SA_Pre(Null=SA_No,   Valid=SA_Yes,ValidBytes   =#count)]
#define __pre_opt_bytecount(count)       [SA_Pre(Null=SA_Maybe,Valid=SA_Yes,ValidBytes   =#count)]

// Valid buffer extent described by a constant expression
#define __pre_count_c(constExpr)         [SA_Pre(Null=SA_No,   Valid=SA_Yes,ValidElementsConst=constExpr)]
#define __pre_opt_count_c(constExpr)     [SA_Pre(Null=SA_Maybe,Valid=SA_Yes,ValidElementsConst=constExpr)]
#define __pre_bytecount_c(constExpr)     [SA_Pre(Null=SA_No,   Valid=SA_Yes,ValidBytesConst   =constExpr)]
#define __pre_opt_bytecount_c(constExpr) [SA_Pre(Null=SA_Maybe,Valid=SA_Yes,ValidBytesConst   =constExpr)]

// Valid buffer extent described by a complex expression
#define __pre_count_x(string)            [SA_Pre(Null=SA_No,   Valid=SA_Yes,ValidElements="\n"#string)]
#define __pre_opt_count_x(string)        [SA_Pre(Null=SA_Maybe,Valid=SA_Yes,ValidElements="\n"#string)]
#define __pre_bytecount_x(string)        [SA_Pre(Null=SA_No,   Valid=SA_Yes,ValidBytes   ="\n"#string)]
#define __pre_opt_bytecount_x(string)    [SA_Pre(Null=SA_Maybe,Valid=SA_Yes,ValidBytes   ="\n"#string)]

// Valid buffer extent described by the difference to another pointer parameter
#define __pre_ptrdiff_count(ptr)         [SA_Pre(Null=SA_No,   Valid=SA_Yes,ValidElements="\n__ptrdiff("#ptr")")]
#define __pre_opt_ptrdiff_count(ptr)     [SA_Pre(Null=SA_Maybe,Valid=SA_Yes,ValidElements="\n__ptrdiff("#ptr")")]

// valid size unknown or indicated by type (e.g.:LPSTR)
#define __pre_valid                      [SA_Pre(Null=SA_No,   Valid=SA_Yes)]
#define __pre_opt_valid                  [SA_Pre(Null=SA_Maybe,Valid=SA_Yes)]

// used with allocated but not yet initialized objects
#define __pre_notnull                    [SA_Pre(Null=SA_No   )]
#define __pre_maybenull                  [SA_Pre(Null=SA_Maybe)]
#define __pre_null                       [SA_Pre(Null=SA_Yes  )]

// restrict access rights
#define __pre_readonly                   [SA_Pre(Access=SA_Read)]
#define __pre_writeonly                  [SA_Pre(Access=SA_Write)]
//
// __post annotations ---
//
// describing conditions that hold after the function call

// void CopyStr( __in_z const char* szFrom, __pre_cap(cch) __post_z char* szFrom, size_t cchFrom );
// buffer will be a zero-terminated string after the call
#define __post_z                        [SA_Post(Valid=SA_Yes,NullTerminated=SA_Yes)]

// e.g. SIZE_T HeapSize( __in HANDLE hHeap, DWORD dwFlags, __pre_notnull __post_bytecap(return) LPCVOID lpMem );
#define __post_cap(cap)                 [SA_Post( WritableElements     =#cap  )]
#define __post_bytecap(cap)             [SA_Post( WritableBytes        =#cap  )]

// e.g. int strlen( __in_z __post_count(return+1) const char* sz );
#define __post_count(count)             [SA_Post(Valid=SA_Yes,ValidElements=#count)]
#define __post_bytecount(count)         [SA_Post(Valid=SA_Yes,ValidBytes   =#count)]

#define __post_count_c(constExpr)       [SA_Post(Valid=SA_Yes,ValidElementsConst=constExpr)]
#define __post_bytecount_c(constExpr)   [SA_Post(Valid=SA_Yes,ValidBytesConst   =constExpr)]

#define __post_count_x(string)          [SA_Post(Valid=SA_Yes,ValidElements="\n"string)]
#define __post_bytecount_x(string)      [SA_Post(Valid=SA_Yes,ValidBytes   ="\n"string)]

// e.g. size_t CopyStr( __in_z const char* szFrom, __pre_cap(cch) __post_z_count(return+1) char* szFrom, size_t cchFrom );
#define __post_z_count(count)           [SA_Post(Valid=SA_Yes,NullTerminated=SA_Yes,ValidElements=#count)]
#define __post_z_bytecount(count)       [SA_Post(Valid=SA_Yes,NullTerminated=SA_Yes,ValidBytes   =#count)]

#define __post_z_count_c(constExpr)     [SA_Post(Valid=SA_Yes,NullTerminated=SA_Yes,ValidElementsConst=constExpr)]
#define __post_z_bytecount_c(constExpr) [SA_Post(Valid=SA_Yes,NullTerminated=SA_Yes,ValidBytesConst   =constExpr)]

// e.g. void free( __post_invalid void* pv );
#define __post_valid                    [SA_Post(Valid=SA_Yes )]
#define __post_invalid                  [SA_Post(Valid=SA_No )]

// e.g. void ThrowExceptionIfNull( __post_notnull const void* pv );
#define __post_notnull                  [SA_Post(Null=SA_No )]

//
// __ret annotations
//
// describing conditions that hold for return values after the call

// e.g. __ret_z CString::operator const wchar_t*() const throw();
#define __ret_z                          [returnvalue:SA_Post(Null=SA_No,   Valid=SA_Yes,NullTerminated=SA_Yes)]
#define __ret_opt_z                      [returnvalue:SA_Post(Null=SA_Maybe,Valid=SA_Yes,NullTerminated=SA_Yes)]

// e.g. __ret_opt_bytecap(cb) void* AllocateMemory( size_t cb );
// Buffer capacity is described by another parameter
#define __ret_cap(cap)                   [returnvalue:SA_Post(Null=SA_No,   WritableElements=#cap)]
#define __ret_opt_cap(cap)               [returnvalue:SA_Post(Null=SA_Maybe,WritableElements=#cap)]
#define __ret_bytecap(cap)               [returnvalue:SA_Post(Null=SA_No,   WritableBytes   =#cap)]
#define __ret_opt_bytecap(cap)           [returnvalue:SA_Post(Null=SA_Maybe,WritableBytes   =#cap)]

// Buffer capacity is described by a constant expression
#define __ret_cap_c(constExpr)           [returnvalue:SA_Post(Null=SA_No,   WritableElementsConst=constExpr)]
#define __ret_opt_cap_c(constExpr)       [returnvalue:SA_Post(Null=SA_Maybe,WritableElementsConst=constExpr)]
#define __ret_bytecap_c(constExpr)       [returnvalue:SA_Post(Null=SA_No,   WritableBytesConst   =constExpr)]
#define __ret_opt_bytecap_c(constExpr)   [returnvalue:SA_Post(Null=SA_Maybe,WritableBytesConst   =constExpr)]

// Buffer capacity is described by a complex condition
#define __ret_cap_x(string)              [returnvalue:SA_Post(Null=SA_No,   WritableElements="\n"#string)]
#define __ret_opt_cap_x(string)          [returnvalue:SA_Post(Null=SA_Maybe,WritableElements="\n"#string)]
#define __ret_bytecap_x(string)          [returnvalue:SA_Post(Null=SA_No,   WritableBytes   ="\n"#string)]
#define __ret_opt_bytecap_x(string)      [returnvalue:SA_Post(Null=SA_Maybe,WritableBytes   ="\n"#string)]

// e.g. __ret_opt_bytecount(cb) void* AllocateZeroInitializedMemory( size_t cb );
// Valid Buffer extent is described by another parameter
#define __ret_count(count)               [returnvalue:SA_Post(Null=SA_No,   Valid=SA_Yes,ValidElements=#count)]
#define __ret_opt_count(count)           [returnvalue:SA_Post(Null=SA_Maybe,Valid=SA_Yes,ValidElements=#count)]
#define __ret_bytecount(count)           [returnvalue:SA_Post(Null=SA_No,   Valid=SA_Yes,ValidBytes   =#count)]
#define __ret_opt_bytecount(count)       [returnvalue:SA_Post(Null=SA_Maybe,Valid=SA_Yes,ValidBytes   =#count)]

// Valid Buffer extent is described by a constant expression
#define __ret_count_c(constExpr)         [returnvalue:SA_Post(Null=SA_No,   Valid=SA_Yes,ValidElementsConst=constExpr)]
#define __ret_opt_count_c(constExpr)     [returnvalue:SA_Post(Null=SA_Maybe,Valid=SA_Yes,ValidElementsConst=constExpr)]
#define __ret_bytecount_c(constExpr)     [returnvalue:SA_Post(Null=SA_No,   Valid=SA_Yes,ValidBytesConst   =constExpr)]
#define __ret_opt_bytecount_c(constExpr) [returnvalue:SA_Post(Null=SA_Maybe,Valid=SA_Yes,ValidBytesConst   =constExpr)]

// Valid Buffer extent is described by a complex expression
#define __ret_count_x(string)            [returnvalue:SA_Post(Null=SA_No,   Valid=SA_Yes,ValidElements="\n"#string)]
#define __ret_opt_count_x(string)        [returnvalue:SA_Post(Null=SA_Maybe,Valid=SA_Yes,ValidElements="\n"#string)]
#define __ret_bytecount_x(string)        [returnvalue:SA_Post(Null=SA_No,   Valid=SA_Yes,ValidBytes   ="\n"#string)]
#define __ret_opt_bytecount_x(string)    [returnvalue:SA_Post(Null=SA_Maybe,Valid=SA_Yes,ValidBytes   ="\n"#string)]

// e.g. __ret_opt_valid LPSTR void* CloneSTR( __pre_valid LPSTR src );
#define __ret_valid                      [returnvalue:SA_Post(Null=SA_No,   Valid=SA_Yes)]
#define __ret_opt_valid                  [returnvalue:SA_Post(Null=SA_Maybe,Valid=SA_Yes)]

// used with allocated but not yet initialized objects
#define __ret_notnull                    [returnvalue:SA_Post(Null=SA_No   )]
#define __ret_maybenull                  [returnvalue:SA_Post(Null=SA_Maybe)]
#define __ret_null                       [returnvalue:SA_Post(Null=SA_Yes  )]

//
// __deref_pre ---
//
// describing conditions for array elements of dereferenced pointer parameters that must be met before the call

// e.g. void SaveStringArray( __in_count(cStrings) __deref_pre_z const wchar_t* const rgpwch[] );
#define __deref_pre_z                          [SA_Pre(Deref=1,Null=SA_No   ,Valid=SA_Yes,NullTerminated=SA_Yes)]
#define __deref_pre_opt_z                      [SA_Pre(Deref=1,Null=SA_Maybe,Valid=SA_Yes,NullTerminated=SA_Yes)]

// e.g. void FillInArrayOfStr32( __in_count(cStrings) __deref_pre_cap_c(32) __deref_post_z wchar_t* const rgpwch[] );
// buffer capacity is described by another parameter
#define __deref_pre_cap(cap)                   [SA_Pre(Deref=1,Null=SA_No   ,WritableElements=#cap  )]
#define __deref_pre_opt_cap(cap)               [SA_Pre(Deref=1,Null=SA_Maybe,WritableElements=#cap  )]
#define __deref_pre_bytecap(cap)               [SA_Pre(Deref=1,Null=SA_No   ,WritableBytes   =#cap  )]
#define __deref_pre_opt_bytecap(cap)           [SA_Pre(Deref=1,Null=SA_Maybe,WritableBytes   =#cap  )]

// buffer capacity is described by a constant expression
#define __deref_pre_cap_c(constExpr)           [SA_Pre(Deref=1,Null=SA_No   ,WritableElementsConst=constExpr)]
#define __deref_pre_opt_cap_c(constExpr)       [SA_Pre(Deref=1,Null=SA_Maybe,WritableElementsConst=constExpr)]
#define __deref_pre_bytecap_c(constExpr)       [SA_Pre(Deref=1,Null=SA_No   ,WritableBytesConst   =constExpr)]
#define __deref_pre_opt_bytecap_c(constExpr)   [SA_Pre(Deref=1,Null=SA_Maybe,WritableBytesConst   =constExpr)]

// buffer capacity is described by a complex condition
#define __deref_pre_cap_x(string)              [SA_Pre(Deref=1,Null=SA_No   ,WritableElements="\n"#string)]
#define __deref_pre_opt_cap_x(string)          [SA_Pre(Deref=1,Null=SA_Maybe,WritableElements="\n"#string)]
#define __deref_pre_bytecap_x(string)          [SA_Pre(Deref=1,Null=SA_No   ,WritableBytes   ="\n"#string)]
#define __deref_pre_opt_bytecap_x(string)      [SA_Pre(Deref=1,Null=SA_Maybe,WritableBytes   ="\n"#string)]

// convenience macros for nullterminated buffers with given capacity
#define __deref_pre_z_cap(cap)                 [SA_Pre(Deref=1,Null=SA_No   ,NullTerminated=SA_Yes,WritableElements=#cap  )]
#define __deref_pre_opt_z_cap(cap)             [SA_Pre(Deref=1,Null=SA_Maybe,NullTerminated=SA_Yes,WritableElements=#cap  )]
#define __deref_pre_z_bytecap(cap)             [SA_Pre(Deref=1,Null=SA_No   ,NullTerminated=SA_Yes,WritableBytes   =#cap  )]
#define __deref_pre_opt_z_bytecap(cap)         [SA_Pre(Deref=1,Null=SA_Maybe,NullTerminated=SA_Yes,WritableBytes   =#cap  )]

#define __deref_pre_z_cap_c(constExpr)         [SA_Pre(Deref=1,Null=SA_No   ,NullTerminated=SA_Yes,WritableElementsConst=constExpr)]
#define __deref_pre_opt_z_cap_c(constExpr)     [SA_Pre(Deref=1,Null=SA_Maybe,NullTerminated=SA_Yes,WritableElementsConst=constExpr)]
#define __deref_pre_z_bytecap_c(constExpr)     [SA_Pre(Deref=1,Null=SA_No   ,NullTerminated=SA_Yes,WritableBytesConst   =constExpr)]
#define __deref_pre_opt_z_bytecap_c(constExpr) [SA_Pre(Deref=1,Null=SA_Maybe,NullTerminated=SA_Yes,WritableBytesConst   =constExpr)]

// e.g. void SaveMatrix( __in_count(n) __deref_pre_count(n) const Elem** matrix, size_t n ); 
// valid buffer extent is described by another parameter
#define __deref_pre_count(count)               [SA_Pre(Deref=1,Null=SA_No   ,Valid=SA_Yes,ValidElements=#count)]
#define __deref_pre_opt_count(count)           [SA_Pre(Deref=1,Null=SA_Maybe,Valid=SA_Yes,ValidElements=#count)]
#define __deref_pre_bytecount(count)           [SA_Pre(Deref=1,Null=SA_No   ,Valid=SA_Yes,ValidBytes   =#count)]
#define __deref_pre_opt_bytecount(count)       [SA_Pre(Deref=1,Null=SA_Maybe,Valid=SA_Yes,ValidBytes   =#count)]

// valid buffer extent is described by a constant expression
#define __deref_pre_count_c(constExpr)         [SA_Pre(Deref=1,Null=SA_No   ,Valid=SA_Yes,ValidElementsConst=constExpr)]
#define __deref_pre_opt_count_c(constExpr)     [SA_Pre(Deref=1,Null=SA_Maybe,Valid=SA_Yes,ValidElementsConst=constExpr)]
#define __deref_pre_bytecount_c(constExpr)     [SA_Pre(Deref=1,Null=SA_No   ,Valid=SA_Yes,ValidBytesConst   =constExpr)]
#define __deref_pre_opt_bytecount_c(constExpr) [SA_Pre(Deref=1,Null=SA_Maybe,Valid=SA_Yes,ValidBytesConst   =constExpr)]

// valid buffer extent is described by a complex expression
#define __deref_pre_count_x(count)             [SA_Pre(Deref=1,Null=SA_No   ,Valid=SA_Yes,ValidElements="\n"#count)]
#define __deref_pre_opt_count_x(count)         [SA_Pre(Deref=1,Null=SA_Maybe,Valid=SA_Yes,ValidElements="\n"#count)]
#define __deref_pre_bytecount_x(count)         [SA_Pre(Deref=1,Null=SA_No   ,Valid=SA_Yes,ValidBytes   ="\n"#count)]
#define __deref_pre_opt_bytecount_x(count)     [SA_Pre(Deref=1,Null=SA_Maybe,Valid=SA_Yes,ValidBytes   ="\n"#count)]

// e.g. void PrintStringArray( __in_count(cElems) __deref_pre_valid LPCSTR rgStr[], size_t cElems );
#define __deref_pre_valid                      [SA_Pre(Deref=1,Null=SA_No   ,Valid=SA_Yes)]
#define __deref_pre_opt_valid                  [SA_Pre(Deref=1,Null=SA_Maybe,Valid=SA_Yes)]

#define __deref_pre_notnull                    [SA_Pre(Deref=1,Null=SA_No   )]
#define __deref_pre_maybenull                  [SA_Pre(Deref=1,Null=SA_Maybe)]
#define __deref_pre_null                       [SA_Pre(Deref=1,Null=SA_Yes  )]

// restrict access rights
#define __deref_pre_readonly                   [SA_Pre(Deref=1,Access=SA_Read)]
#define __deref_pre_writeonly                  [SA_Pre(Deref=1,Access=SA_Write)]

//
// __deref_post ---
//
// describing conditions for array elements or dereferenced pointer parameters that hold after the call

// e.g. void CloneString( __in_z const Wchar_t* wzIn __out __deref_post_z wchar_t** pWzOut );
#define __deref_post_z                          [SA_Post(Deref=1,Null=SA_No   ,Valid=SA_Yes,NullTerminated=SA_Yes)]
#define __deref_post_opt_z                      [SA_Post(Deref=1,Null=SA_Maybe,Valid=SA_Yes,NullTerminated=SA_Yes)]

// e.g. HRESULT HrAllocateMemory( size_t cb, __out __deref_post_bytecap(cb) void** ppv );
// buffer capacity is described by another parameter
#define __deref_post_cap(cap)                   [SA_Post(Deref=1,Null=SA_No   ,WritableElements=#cap)]
#define __deref_post_opt_cap(cap)               [SA_Post(Deref=1,Null=SA_Maybe,WritableElements=#cap)]
#define __deref_post_bytecap(cap)               [SA_Post(Deref=1,Null=SA_No   ,WritableBytes   =#cap)]
#define __deref_post_opt_bytecap(cap)           [SA_Post(Deref=1,Null=SA_Maybe,WritableBytes   =#cap)]

// buffer capacity is described by a constant expression
#define __deref_post_cap_c(constExpr)           [SA_Post(Deref=1,Null=SA_No   ,WritableElementsConst=constExpr)]
#define __deref_post_opt_cap_c(constExpr)       [SA_Post(Deref=1,Null=SA_Maybe,WritableElementsConst=constExpr)]
#define __deref_post_bytecap_c(constExpr)       [SA_Post(Deref=1,Null=SA_No   ,WritableBytesConst   =constExpr)]
#define __deref_post_opt_bytecap_c(constExpr)   [SA_Post(Deref=1,Null=SA_Maybe,WritableBytesConst   =constExpr)]

// buffer capacity is described by a complex expression
#define __deref_post_cap_x(string)              [SA_Post(Deref=1,Null=SA_No   ,WritableElements="\n"#string)]
#define __deref_post_opt_cap_x(string)          [SA_Post(Deref=1,Null=SA_Maybe,WritableElements="\n"#string)]
#define __deref_post_bytecap_x(string)          [SA_Post(Deref=1,Null=SA_No   ,WritableBytes   ="\n"#string)]
#define __deref_post_opt_bytecap_x(string)      [SA_Post(Deref=1,Null=SA_Maybe,WritableBytes   ="\n"#string)]

// convenience macros for nullterminated buffers with given capacity
#define __deref_post_z_cap(cap)                 [SA_Post(Deref=1,Null=SA_No   ,NullTerminated=SA_Yes,WritableElements=#cap  )]
#define __deref_post_opt_z_cap(cap)             [SA_Post(Deref=1,Null=SA_Maybe,NullTerminated=SA_Yes,WritableElements=#cap  )]
#define __deref_post_z_bytecap(cap)             [SA_Post(Deref=1,Null=SA_No   ,NullTerminated=SA_Yes,WritableBytes   =#cap  )]
#define __deref_post_opt_z_bytecap(cap)         [SA_Post(Deref=1,Null=SA_Maybe,NullTerminated=SA_Yes,WritableBytes   =#cap  )]

#define __deref_post_z_cap_c(constExpr)         [SA_Post(Deref=1,Null=SA_No   ,NullTerminated=SA_Yes,WritableElementsConst=constExpr)]
#define __deref_post_opt_z_cap_c(constExpr)     [SA_Post(Deref=1,Null=SA_Maybe,NullTerminated=SA_Yes,WritableElementsConst=constExpr)]
#define __deref_post_z_bytecap_c(constExpr)     [SA_Post(Deref=1,Null=SA_No   ,NullTerminated=SA_Yes,WritableBytesConst   =constExpr)]
#define __deref_post_opt_z_bytecap_c(constExpr) [SA_Post(Deref=1,Null=SA_Maybe,NullTerminated=SA_Yes,WritableBytesConst   =constExpr)]

// e.g. HRESULT HrAllocateZeroInitializedMemory( size_t cb, __out __deref_post_bytecount(cb) void** ppv );
// valid buffer extent is described by another parameter
#define __deref_post_count(count)               [SA_Post(Deref=1,Null=SA_No   ,Valid=SA_Yes,ValidElements=#count)]
#define __deref_post_opt_count(count)           [SA_Post(Deref=1,Null=SA_Maybe,Valid=SA_Yes,ValidElements=#count)]
#define __deref_post_bytecount(count)           [SA_Post(Deref=1,Null=SA_No   ,Valid=SA_Yes,ValidBytes   =#count)]
#define __deref_post_opt_bytecount(count)       [SA_Post(Deref=1,Null=SA_Maybe,Valid=SA_Yes,ValidBytes   =#count)]

// buffer capacity is described by a constant expression
#define __deref_post_count_c(constExpr)         [SA_Post(Deref=1,Null=SA_No   ,Valid=SA_Yes,ValidElementsConst=constExpr)]
#define __deref_post_opt_count_c(constExpr)     [SA_Post(Deref=1,Null=SA_Maybe,Valid=SA_Yes,ValidElementsConst=constExpr)]
#define __deref_post_bytecount_c(constExpr)     [SA_Post(Deref=1,Null=SA_No   ,Valid=SA_Yes,ValidBytesConst   =constExpr)]
#define __deref_post_opt_bytecount_c(constExpr) [SA_Post(Deref=1,Null=SA_Maybe,Valid=SA_Yes,ValidBytesConst   =constExpr)]

// buffer capacity is described by a complex expression
#define __deref_post_count_x(string)            [SA_Post(Deref=1,Null=SA_No   ,Valid=SA_Yes,ValidElements="\n"#string)]
#define __deref_post_opt_count_x(string)        [SA_Post(Deref=1,Null=SA_Maybe,Valid=SA_Yes,ValidElements="\n"#string)]
#define __deref_post_bytecount_x(string)        [SA_Post(Deref=1,Null=SA_No   ,Valid=SA_Yes,ValidBytes   ="\n"#string)]
#define __deref_post_opt_bytecount_x(string)    [SA_Post(Deref=1,Null=SA_Maybe,Valid=SA_Yes,ValidBytes   ="\n"#string)]

// e.g. void GetStrings( __out_count(cElems) __deref_post_valid LPSTR const rgStr[], size_t cElems );
#define __deref_post_valid                      [SA_Post(Deref=1,Null=SA_No   ,Valid=SA_Yes)]
#define __deref_post_opt_valid                  [SA_Post(Deref=1,Null=SA_Maybe,Valid=SA_Yes)]

#define __deref_post_notnull                    [SA_Post(Deref=1,Null=SA_No   )]
#define __deref_post_maybenull                  [SA_Post(Deref=1,Null=SA_Maybe)]
#define __deref_post_null                       [SA_Post(Deref=1,Null=SA_Yes  )]

//
// __deref_ret ---
//

#define __deref_ret_z                           [returnvalue:SA_Post(Deref=1,Null=SA_No,   Valid=SA_Yes,NullTerminated=SA_No)]
#define __deref_ret_opt_z                       [returnvalue:SA_Post(Deref=1,Null=SA_Maybe,Valid=SA_Yes,NullTerminated=SA_No)]

#elif __USE_DECLSPECS_FOR_SAL

// deprecated versions of SAL macros using __declspec compiler extensions

// pre ----

#define __pre_z                          __pre_valid     __pre __nullterminated
#define __pre_opt_z                      __pre_opt_valid __pre __nullterminated

#define __pre_cap(cap)                   __pre __notnull    __pre __elem_writableTo(cap)
#define __pre_opt_cap(cap)               __pre __maybenull  __pre __elem_writableTo(cap)
#define __pre_bytecap(cap)               __pre __notnull    __pre __byte_writableTo(cap)
#define __pre_opt_bytecap(cap)           __pre __maybenull  __pre __byte_writableTo(cap)

#define __pre_cap_c(constExpr)           __pre __notnull    __pre __elem_writableTo(constExpr) __const_macro
#define __pre_opt_cap_c(constExpr)       __pre __maybenull  __pre __elem_writableTo(constExpr) __const_macro
#define __pre_bytecap_c(constExpr)       __pre __notnull    __pre __byte_writableTo(constExpr) __const_macro
#define __pre_opt_bytecap_c(constExpr)   __pre __maybenull  __pre __byte_writableTo(constExpr) __const_macro

#define __pre_cap_x(string)              __pre __notnull    __pre __inexpressible_writableTo(string)
#define __pre_opt_cap_x(string)          __pre __maybenull  __pre __inexpressible_writableTo(string)
#define __pre_bytecap_x(string)          __pre __notnull    __pre __inexpressible_writableTo(string)
#define __pre_opt_bytecap_x(string)      __pre __maybenull  __pre __inexpressible_writableTo(string)

#define __pre_ptrdiff_cap(ptr)           __pre __notnull    __pre __ptrdiff_writableTo(ptr)
#define __pre_opt_ptrdiff_cap(ptr)       __pre __maybenull  __pre __ptrdiff_writableTo(ptr)

#define __pre_z_cap(cap)                 __pre_z     __pre __elem_writableTo(cap)
#define __pre_opt_z_cap(cap)             __pre_opt_z __pre __elem_writableTo(cap)
#define __pre_z_bytecap(cap)             __pre_z     __pre __byte_writableTo(cap)
#define __pre_opt_z_bytecap(cap)         __pre_opt_z __pre __byte_writableTo(cap)

#define __pre_z_cap_c(constExpr)         __pre_z     __pre __elem_writableTo(constExpr) __const_macro
#define __pre_opt_z_cap_c(constExpr)     __pre_opt_z __pre __elem_writableTo(constExpr) __const_macro
#define __pre_z_bytecap_c(constExpr)     __pre_z     __pre __byte_writableTo(constExpr) __const_macro
#define __pre_opt_z_bytecap_c(constExpr) __pre_opt_z __pre __byte_writableTo(constExpr) __const_macro

#define __pre_count(count)               __pre_valid     __pre __elem_readableTo(count)
#define __pre_opt_count(count)           __pre_opt_valid __pre __elem_readableTo(count)
#define __pre_bytecount(count)           __pre_valid     __pre __byte_readableTo(count)
#define __pre_opt_bytecount(count)       __pre_opt_valid __pre __byte_readableTo(count)

#define __pre_count_c(constExpr)         __pre_valid     __pre __elem_readableTo(constExpr) __const_macro
#define __pre_opt_count_c(constExpr)     __pre_opt_valid __pre __elem_readableTo(constExpr) __const_macro
#define __pre_bytecount_c(constExpr)     __pre_valid     __pre __byte_readableTo(constExpr) __const_macro
#define __pre_opt_bytecount_c(constExpr) __pre_opt_valid __pre __byte_readableTo(constExpr) __const_macro

#define __pre_count_x(string)            __pre_valid     __pre __inexpressible_readableTo(string)
#define __pre_opt_count_x(string)        __pre_opt_valid __pre __inexpressible_readableTo(string)
#define __pre_bytecount_x(string)        __pre_valid     __pre __inexpressible_readableTo(string)
#define __pre_opt_bytecount_x(string)    __pre_opt_valid __pre __inexpressible_readableTo(string)

#define __pre_ptrdiff_count(ptr)         __pre_valid     __pre __ptrdiff_readableTo(ptr)
#define __pre_opt_ptrdiff_count(ptr)     __pre_opt_valid __pre __ptrdiff_readableTo(ptr)

#define __pre_valid                      __pre __valid
#define __pre_opt_valid                  __pre __valid __pre __exceptthat __maybenull

#define __pre_notnull                    __pre __notnull
#define __pre_maybenull                  __pre __maybenull
#define __pre_null                       __pre __null

#define __pre_readonly                   __pre __readonly
#define __pre_writeonly

// post ---

#define __post_z                        __post_valid __post __nullterminated

#define __post_cap(cap)                 __post __elem_writableTo(cap)
#define __post_bytecap(cap)             __post __byte_writableTo(cap)

#define __post_count(count)             __post_valid __post __elem_readableTo(count) 
#define __post_bytecount(count)         __post_valid __post __byte_readableTo(count)

#define __post_count_c(constExpr)       __post_valid __post __elem_readableTo(constExpr) __const_macro
#define __post_bytecount_c(constExpr)   __post_valid __post __byte_readableTo(constExpr) __const_macro

#define __post_count_x(string)          __post_valid __post __inexpressible_readableTo(string) 
#define __post_bytecount_x(string)      __post_valid __post __inexpressible_readableTo(string)

#define __post_z_count(count)           __post_valid __post __elem_readableTo(count)  __post __nullterminated
#define __post_z_bytecount(count)       __post_valid __post __byte_readableTo(count)  __post __nullterminated

#define __post_z_count_c(constExpr)     __post_valid __post __elem_readableTo(constExpr) __post __nullterminated __const_macro
#define __post_z_bytecount_c(constExpr) __post_valid __post __byte_readableTo(constExpr) __post __nullterminated __const_macro

#define __post_valid                    __post __valid
#define __post_invalid                  __post __notvalid

#define __post_notnull                  __post __notnull

// return ---

#define __ret_z                          __post __notnull   __post __valid                                 __post __nullterminated
#define __ret_opt_z                      __post __maybenull __post __valid __post __exceptthat __maybenull __post __nullterminated

#define __ret_cap(cap)                   __post __notnull   __post __elem_writableTo(cap)
#define __ret_opt_cap(cap)               __post __maybenull __post __elem_writableTo(cap)
#define __ret_bytecap(cap)               __post __notnull   __post __byte_writableTo(cap)
#define __ret_opt_bytecap(cap)           __post __maybenull __post __byte_writableTo(cap)

#define __ret_cap_c(constExpr)           __post __notnull   __post __elem_writableTo(constExpr) __const_macro
#define __ret_opt_cap_c(constExpr)       __post __maybenull __post __elem_writableTo(constExpr) __const_macro
#define __ret_bytecap_c(constExpr)       __post __notnull   __post __byte_writableTo(constExpr) __const_macro
#define __ret_opt_bytecap_c(constExpr)   __post __maybenull __post __byte_writableTo(constExpr) __const_macro

#define __ret_cap_x(string)              __post __notnull   __post __inexpressible_writableTo(string)
#define __ret_opt_cap_x(string)          __post __maybenull __post __inexpressible_writableTo(string)
#define __ret_bytecap_x(string)          __post __notnull   __post __inexpressible_writableTo(string)
#define __ret_opt_bytecap_x(string)      __post __maybenull __post __inexpressible_writableTo(string)

#define __ret_count(count)               __post __notnull   __post __valid                                 __post __elem_readableTo(count)
#define __ret_opt_count(count)           __post __maybenull __post __valid __post __exceptthat __maybenull __post __elem_readableTo(count)
#define __ret_bytecount(count)           __post __notnull   __post __valid                                 __post __byte_readableTo(count)
#define __ret_opt_bytecount(count)       __post __maybenull __post __valid __post __exceptthat __maybenull __post __byte_readableTo(count)

#define __ret_bytecount_c(constExpr)     __post __notnull   __post __valid                                 __post __byte_readableTo(constExpr) __const_macro
#define __ret_opt_bytecount_c(constExpr) __post __maybenull __post __valid __post __exceptthat __maybenull __post __byte_readableTo(constExpr) __const_macro
#define __ret_count_c(constExpr)         __post __notnull   __post __valid                                 __post __elem_readableTo(constExpr) __const_macro
#define __ret_opt_count_c(constExpr)     __post __maybenull __post __valid __post __exceptthat __maybenull __post __elem_readableTo(constExpr) __const_macro

#define __ret_count_x(string)            __post __notnull   __post __valid                                 __post __elem_readableTo(string)
#define __ret_opt_count_x(string)        __post __maybenull __post __valid __post __exceptthat __maybenull __post __elem_readableTo(string)
#define __ret_bytecount_x(string)        __post __notnull   __post __valid                                 __post __byte_readableTo(string)
#define __ret_opt_bytecount_x(string)    __post __maybenull __post __valid __post __exceptthat __maybenull __post __byte_readableTo(string)

#define __ret_valid                      __post __notnull   __post __valid
#define __ret_opt_valid                  __post __maybenull __post __valid __post __exceptthat __maybenull

#define __ret_notnull                    __post __notnull
#define __ret_maybenull                  __post __maybenull
#define __ret_null                       __post __null

// __deref_pre ---

#define __deref_pre_z                          __pre __deref __notnull   __pre __deref __valid                                        __pre __deref __nullterminated
#define __deref_pre_opt_z                      __pre __deref __maybenull __pre __deref __valid __pre __deref __exceptthat __maybenull __pre __deref __nullterminated

#define __deref_pre_cap(cap)                   __pre __deref __notnull   __pre __deref __elem_writableTo(cap)
#define __deref_pre_opt_cap(cap)               __pre __deref __maybenull __pre __deref __elem_writableTo(cap)
#define __deref_pre_bytecap(cap)               __pre __deref __notnull   __pre __deref __byte_writableTo(cap)
#define __deref_pre_opt_bytecap(cap)           __pre __deref __maybenull __pre __deref __byte_writableTo(cap)

#define __deref_pre_cap_c(constExpr)           __pre __deref __notnull   __pre __deref __elem_writableTo(constExpr) __const_macro
#define __deref_pre_opt_cap_c(constExpr)       __pre __deref __maybenull __pre __deref __elem_writableTo(constExpr) __const_macro
#define __deref_pre_bytecap_c(constExpr)       __pre __deref __notnull   __pre __deref __byte_writableTo(constExpr) __const_macro
#define __deref_pre_opt_bytecap_c(constExpr)   __pre __deref __maybenull __pre __deref __byte_writableTo(constExpr) __const_macro

#define __deref_pre_cap_x(string)              __pre __deref __notnull   __pre __deref __inexpressible_writableTo(string)
#define __deref_pre_opt_cap_x(string)          __pre __deref __maybenull __pre __deref __inexpressible_writableTo(string)
#define __deref_pre_bytecap_x(string)          __pre __deref __notnull   __pre __deref __inexpressible_writableTo(string)
#define __deref_pre_opt_bytecap_x(string)      __pre __deref __maybenull __pre __deref __inexpressible_writableTo(string)

#define __deref_pre_z_cap(cap)                 __pre __deref __notnull   __pre __deref __valid                                        __pre __deref __nullterminated __pre __deref __elem_writableTo(cap)
#define __deref_pre_opt_z_cap(cap)             __pre __deref __maybenull __pre __deref __valid __pre __deref __exceptthat __maybenull __pre __deref __nullterminated __pre __deref __elem_writableTo(cap)
#define __deref_pre_z_bytecap(cap)             __pre __deref __notnull   __pre __deref __valid                                        __pre __deref __nullterminated __pre __deref __byte_writableTo(cap)
#define __deref_pre_opt_z_bytecap(cap)         __pre __deref __maybenull __pre __deref __valid __pre __deref __exceptthat __maybenull __pre __deref __nullterminated __pre __deref __byte_writableTo(cap)

#define __deref_pre_z_cap_c(constExpr)         __pre __deref __notnull   __pre __deref __valid                                        __pre __deref __nullterminated __pre __deref __elem_writableTo(constExpr) __const_macro
#define __deref_pre_opt_z_cap_c(constExpr)     __pre __deref __maybenull __pre __deref __valid __pre __deref __exceptthat __maybenull __pre __deref __nullterminated __pre __deref __elem_writableTo(constExpr) __const_macro
#define __deref_pre_z_bytecap_c(constExpr)     __pre __deref __notnull   __pre __deref __valid                                        __pre __deref __nullterminated __pre __deref __byte_writableTo(constExpr) __const_macro
#define __deref_pre_opt_z_bytecap_c(constExpr) __pre __deref __maybenull __pre __deref __valid __pre __deref __exceptthat __maybenull __pre __deref __nullterminated __pre __deref __byte_writableTo(constExpr) __const_macro

#define __deref_pre_count(count)               __pre __deref __notnull   __pre __deref __valid                                         __pre __deref __elem_readableTo(count)
#define __deref_pre_opt_count(count)           __pre __deref __maybenull __pre __deref __valid __pre __deref __exceptthat __maybenull  __pre __deref __elem_readableTo(count)
#define __deref_pre_bytecount(count)           __pre __deref __notnull   __pre __deref __valid                                         __pre __deref __byte_readableTo(count)
#define __deref_pre_opt_bytecount(count)       __pre __deref __maybenull __pre __deref __valid __pre __deref __exceptthat __maybenull  __pre __deref __byte_readableTo(count)

#define __deref_pre_count_c(constExpr)         __pre __deref __notnull   __pre __deref __valid                                         __pre __deref __elem_readableTo(constExpr) __const_macro
#define __deref_pre_opt_count_c(constExpr)     __pre __deref __maybenull __pre __deref __valid __pre __deref __exceptthat __maybenull  __pre __deref __elem_readableTo(constExpr) __const_macro
#define __deref_pre_bytecount_c(constExpr)     __pre __deref __notnull   __pre __deref __valid                                         __pre __deref __byte_readableTo(constExpr) __const_macro
#define __deref_pre_opt_bytecount_c(constExpr) __pre __deref __maybenull __pre __deref __valid __pre __deref __exceptthat __maybenull  __pre __deref __byte_readableTo(constExpr) __const_macro

#define __deref_pre_count_x(string)            __pre __deref __notnull   __pre __deref __valid                                         __pre __deref __inexpressible_readableTo(string)
#define __deref_pre_opt_count_x(string)        __pre __deref __maybenull __pre __deref __valid __pre __deref __exceptthat __maybenull  __pre __deref __inexpressible_readableTo(string)
#define __deref_pre_bytecount_x(string)        __pre __deref __notnull   __pre __deref __valid                                         __pre __deref __inexpressible_readableTo(string)
#define __deref_pre_opt_bytecount_x(string)    __pre __deref __maybenull __pre __deref __valid __pre __deref __exceptthat __maybenull  __pre __deref __inexpressible_readableTo(string)

#define __deref_pre_valid                      __pre __deref __notnull   __pre __deref __valid
#define __deref_pre_opt_valid                  __pre __deref __maybenull __pre __deref __valid

#define __deref_pre_notnull                    __pre __deref __notnull
#define __deref_pre_maybenull                  __pre __deref __maybenull
#define __deref_pre_null                       __pre __deref __null

// __post_deref ---

#define __deref_post_z                          __post __deref __notnull   __post __deref __valid __post __deref __nullterminated
#define __deref_post_opt_z                      __post __deref __maybenull __post __deref __valid __post __deref __nullterminated

#define __deref_post_cap(cap)                   __post __deref __notnull   __post __deref __elem_writableTo(cap)
#define __deref_post_opt_cap(cap)               __post __deref __maybenull __post __deref __elem_writableTo(cap)
#define __deref_post_bytecap(cap)               __post __deref __notnull   __post __deref __byte_writableTo(cap)
#define __deref_post_opt_bytecap(cap)           __post __deref __maybenull __post __deref __byte_writableTo(cap)

#define __deref_post_cap_c(constExpr)           __post __deref __notnull   __post __deref __elem_writableTo(constExpr) __const_macro
#define __deref_post_opt_cap_c(constExpr)       __post __deref __maybenull __post __deref __elem_writableTo(constExpr) __const_macro
#define __deref_post_bytecap_c(constExpr)       __post __deref __notnull   __post __deref __byte_writableTo(constExpr) __const_macro
#define __deref_post_opt_bytecap_c(constExpr)   __post __deref __maybenull __post __deref __byte_writableTo(constExpr) __const_macro

#define __deref_post_cap_x(string)              __post __deref __notnull   __post __deref __inexpressible_writableTo(string)
#define __deref_post_opt_cap_x(string)          __post __deref __maybenull __post __deref __inexpressible_writableTo(string)
#define __deref_post_bytecap_x(string)          __post __deref __notnull   __post __deref __inexpressible_writableTo(string)
#define __deref_post_opt_bytecap_x(string)      __post __deref __maybenull __post __deref __inexpressible_writableTo(string)

#define __deref_post_z_cap(cap)                 __post __deref __notnull   __post __deref __elem_writableTo(cap) __post __deref __nullterminated
#define __deref_post_opt_z_cap(cap)             __post __deref __maybenull __post __deref __elem_writableTo(cap) __post __deref __nullterminated
#define __deref_post_z_bytecap(cap)             __post __deref __notnull   __post __deref __byte_writableTo(cap) __post __deref __nullterminated
#define __deref_post_opt_z_bytecap(cap)         __post __deref __maybenull __post __deref __byte_writableTo(cap) __post __deref __nullterminated

#define __deref_post_z_cap_c(constExpr)         __post __deref __notnull   __post __deref __elem_writableTo(constExpr) __post __deref __nullterminated __const_macro
#define __deref_post_opt_z_cap_c(constExpr)     __post __deref __maybenull __post __deref __elem_writableTo(constExpr) __post __deref __nullterminated __const_macro
#define __deref_post_z_bytecap_c(constExpr)     __post __deref __notnull   __post __deref __byte_writableTo(constExpr) __post __deref __nullterminated __const_macro
#define __deref_post_opt_z_bytecap_c(constExpr) __post __deref __maybenull __post __deref __byte_writableTo(constExpr) __post __deref __nullterminated __const_macro

#define __deref_post_count(count)               __post __deref __notnull   __post __deref __valid                                          __post __deref __elem_readableTo(count)
#define __deref_post_opt_count(count)           __post __deref __maybenull __post __deref __valid __post __deref __exceptthat __maybenull  __post __deref __elem_readableTo(count)
#define __deref_post_bytecount(count)           __post __deref __notnull   __post __deref __valid                                          __post __deref __byte_readableTo(count)
#define __deref_post_opt_bytecount(count)       __post __deref __maybenull __post __deref __valid __post __deref __exceptthat __maybenull  __post __deref __byte_readableTo(count)

#define __deref_post_count_c(constExpr)         __post __deref __notnull   __post __deref __valid                                          __post __deref __elem_readableTo(constExpr) __const_macro
#define __deref_post_opt_count_c(constExpr)     __post __deref __maybenull __post __deref __valid __post __deref __exceptthat __maybenull  __post __deref __elem_readableTo(constExpr) __const_macro
#define __deref_post_bytecount_c(constExpr)     __post __deref __notnull   __post __deref __valid                                          __post __deref __byte_readableTo(constExpr) __const_macro
#define __deref_post_opt_bytecount_c(constExpr) __post __deref __maybenull __post __deref __valid __post __deref __exceptthat __maybenull  __post __deref __byte_readableTo(constExpr) __const_macro

#define __deref_post_count_x(string)            __post __deref __notnull   __post __deref __valid                                          __post __deref __inexpressible_readableTo(string)
#define __deref_post_opt_count_x(string)        __post __deref __maybenull __post __deref __valid __post __deref __exceptthat __maybenull  __post __deref __inexpressible_readableTo(string)
#define __deref_post_bytecount_x(string)        __post __deref __notnull   __post __deref __valid                                          __post __deref __inexpressible_readableTo(string)
#define __deref_post_opt_bytecount_x(string)    __post __deref __maybenull __post __deref __valid __post __deref __exceptthat __maybenull  __post __deref __inexpressible_readableTo(string)

#define __deref_post_valid                      __post __deref __notnull   __post __deref __valid
#define __deref_post_opt_valid                  __post __deref __maybenull __post __deref __valid

#define __deref_post_notnull                    __post __deref __notnull
#define __deref_post_maybenull                  __post __deref __maybenull
#define __deref_post_null                       __post __deref __null

#define __deref_ret_z                           __post __deref __notnull   __post __deref __valid __post __deref __nullterminated
#define __deref_ret_opt_z                       __post __deref __maybenull __post __deref __valid __post __deref __nullterminated

// helper macros to implement __declspec version

#ifndef SPECSTRINGIZE
#define SPECSTRINGIZE( x ) #x
#endif

#define __pre                     __declspec("SAL_pre")
#define __post                    __declspec("SAL_post")
#define __deref                   __declspec("SAL_deref")
#define __exceptthat              __declspec("SAL_except")

#define __null                    __declspec("SAL_null")
#define __notnull                 __declspec("SAL_notnull")
#define __maybenull               __declspec("SAL_maybenull")

#define __valid                   __declspec("SAL_valid")
#define __notvalid                __declspec("SAL_notvalid")

#define __elem_readableTo(size)           __declspec("SAL_readableTo(elementCount("SPECSTRINGIZE(size)"))")
#define __byte_readableTo(size)           __declspec("SAL_readableTo(byteCount("SPECSTRINGIZE(size)"))")
#define __inexpressible_readableTo(size)  __declspec("SAL_readableTo(inexpressibleCount('"SPECSTRINGIZE(size)"'))")    
#define __ptrdiff_readableTo(size)        __declspec("SAL_readableTo(endpointer("SPECSTRINGIZE(size)"))")    

#define __elem_writableTo(size)           __declspec("SAL_writableTo(elementCount("SPECSTRINGIZE(size)"))")
#define __byte_writableTo(size)           __declspec("SAL_writableTo(byteCount("SPECSTRINGIZE(size)"))")
#define __inexpressible_writableTo(size)  __declspec("SAL_writableTo(inexpressibleCount('"SPECSTRINGIZE(size)"'))")
#define __ptrdiff_writableTo(size)        __declspec("SAL_writableTo(endpointer("SPECSTRINGIZE(size) "))")

#define __readonly                __declspec("SAL_readonly")

#define __const_macro             __declspec("__const_macro")


#else // SAL macros are #defined to nothing for the regular build


#define __pre_z
#define __pre_opt_z
#define __pre_cap(cap)
#define __pre_opt_cap(cap)
#define __pre_bytecap(cap)
#define __pre_opt_bytecap(cap)
#define __pre_cap_c(constExpr)
#define __pre_opt_cap_c(constExpr)
#define __pre_bytecap_c(constExpr)
#define __pre_opt_bytecap_c(constExpr)
#define __pre_cap_x(string)
#define __pre_opt_cap_x(string)
#define __pre_bytecap_x(string)
#define __pre_opt_bytecap_x(string)
#define __pre_ptrdiff_cap(ptr)
#define __pre_opt_ptrdiff_cap(ptr)
#define __pre_z_cap(cap)
#define __pre_opt_z_cap(cap)
#define __pre_z_bytecap(cap)
#define __pre_opt_z_bytecap(cap)
#define __pre_z_cap_c(constExpr)
#define __pre_opt_z_cap_c(constExpr)
#define __pre_z_bytecap_c(constExpr)
#define __pre_opt_z_bytecap_c(constExpr)
#define __pre_count(count)
#define __pre_opt_count(count)
#define __pre_bytecount(count)
#define __pre_opt_bytecount(count)
#define __pre_count_c(constExpr)
#define __pre_opt_count_c(constExpr)
#define __pre_bytecount_c(constExpr)
#define __pre_opt_bytecount_c(constExpr)
#define __pre_count_x(string)
#define __pre_opt_count_x(string)
#define __pre_bytecount_x(string)
#define __pre_opt_bytecount_x(string)
#define __pre_ptrdiff_count(ptr)
#define __pre_opt_ptrdiff_count(ptr)
#define __pre_valid
#define __pre_opt_valid
#define __pre_notnull
#define __pre_maybenull
#define __pre_null
#define __pre_readonly
#define __pre_writeonly

#define __post_z
#define __post_cap(cap)
#define __post_bytecap(cap)
#define __post_count(count)
#define __post_bytecount(count)
#define __post_count_c(constExpr)
#define __post_bytecount_c(constExpr)
#define __post_count_x(string)
#define __post_bytecount_x(string)
#define __post_z_count(count)
#define __post_z_bytecount(count)
#define __post_z_count_c(constExpr)
#define __post_z_bytecount_c(constExpr)
#define __post_valid
#define __post_invalid
#define __post_notnull

#define __ret_z
#define __ret_opt_z
#define __ret_cap(cap)
#define __ret_opt_cap(cap)
#define __ret_bytecap(cap)
#define __ret_opt_bytecap(cap)
#define __ret_cap_c(constExpr)
#define __ret_opt_cap_c(constExpr)
#define __ret_bytecap_c(constExpr)
#define __ret_opt_bytecap_c(constExpr)
#define __ret_cap_x(string)
#define __ret_opt_cap_x(string)
#define __ret_bytecap_x(string)
#define __ret_opt_bytecap_x(string)
#define __ret_count(count)
#define __ret_opt_count(count)
#define __ret_bytecount(count)
#define __ret_opt_bytecount(count)
#define __ret_count_c(constExpr)
#define __ret_opt_count_c(constExpr)
#define __ret_bytecount_c(constExpr)
#define __ret_opt_bytecount_c(constExpr)
#define __ret_count_x(string)
#define __ret_opt_count_x(string)
#define __ret_bytecount_x(string)
#define __ret_opt_bytecount_x(string)
#define __ret_valid
#define __ret_opt_valid
#define __ret_notnull
#define __ret_maybenull
#define __ret_null

#define __deref_pre_z
#define __deref_pre_opt_z
#define __deref_pre_cap(cap)
#define __deref_pre_opt_cap(cap)
#define __deref_pre_bytecap(cap)
#define __deref_pre_opt_bytecap(cap)
#define __deref_pre_cap_c(constExpr)
#define __deref_pre_opt_cap_c(constExpr)
#define __deref_pre_bytecap_c(constExpr)
#define __deref_pre_opt_bytecap_c(constExpr)
#define __deref_pre_cap_x(string)
#define __deref_pre_opt_cap_x(string)
#define __deref_pre_bytecap_x(string)
#define __deref_pre_opt_bytecap_x(string)
#define __deref_pre_z_cap(cap)
#define __deref_pre_opt_z_cap(cap)
#define __deref_pre_z_bytecap(cap)
#define __deref_pre_opt_z_bytecap(cap)
#define __deref_pre_z_cap_c(constExpr)
#define __deref_pre_opt_z_cap_c(constExpr)
#define __deref_pre_z_bytecap_c(constExpr)
#define __deref_pre_opt_z_bytecap_c(constExpr)
#define __deref_pre_count(count)
#define __deref_pre_opt_count(count)
#define __deref_pre_bytecount(count)
#define __deref_pre_opt_bytecount(count)
#define __deref_pre_count_c(constExpr)
#define __deref_pre_opt_count_c(constExpr)
#define __deref_pre_bytecount_c(constExpr)
#define __deref_pre_opt_bytecount_c(constExpr)
#define __deref_pre_count_x(count)
#define __deref_pre_opt_count_x(count)
#define __deref_pre_bytecount_x(count)
#define __deref_pre_opt_bytecount_x(count)
#define __deref_pre_valid
#define __deref_pre_opt_valid
#define __deref_pre_notnull
#define __deref_pre_maybenull
#define __deref_pre_null

#define __deref_post_z
#define __deref_post_opt_z
#define __deref_post_cap(cap)
#define __deref_post_opt_cap(cap)
#define __deref_post_bytecap(cap)
#define __deref_post_opt_bytecap(cap)
#define __deref_post_cap_c(constExpr)
#define __deref_post_opt_cap_c(constExpr)
#define __deref_post_bytecap_c(constExpr)
#define __deref_post_opt_bytecap_c(constExpr)
#define __deref_post_cap_x(string)
#define __deref_post_opt_cap_x(string)
#define __deref_post_bytecap_x(string)
#define __deref_post_opt_bytecap_x(string)
#define __deref_post_z_cap(cap)
#define __deref_post_opt_z_cap(cap)
#define __deref_post_z_bytecap(cap)
#define __deref_post_opt_z_bytecap(cap)
#define __deref_post_z_cap_c(constExpr)
#define __deref_post_opt_z_cap_c(constExpr)
#define __deref_post_z_bytecap_c(constExpr)
#define __deref_post_opt_z_bytecap_c(constExpr)
#define __deref_post_count(count)
#define __deref_post_opt_count(count)
#define __deref_post_bytecount(count)
#define __deref_post_opt_bytecount(count)
#define __deref_post_count_c(constExpr)
#define __deref_post_opt_count_c(constExpr)
#define __deref_post_bytecount_c(constExpr)
#define __deref_post_opt_bytecount_c(constExpr)
#define __deref_post_count_x(string)
#define __deref_post_opt_count_x(string)
#define __deref_post_bytecount_x(string)
#define __deref_post_opt_bytecount_x(string)
#define __deref_post_valid
#define __deref_post_opt_valid
#define __deref_post_notnull
#define __deref_post_maybenull
#define __deref_post_null
#define __deref_ret_z
#define __deref_ret_opt_z

#endif // #if ( 1400 <= _MSC_VER ) && __USE_ATTRIBUTES_FOR_SAL

// Convenience macros for more concise annotations

//
// __prepost ---
//
// describing conditions that hold before and after the function call

#define __prepost_z                      __pre_z     __post_z
#define __prepost_opt_z                  __pre_opt_z __post_z

#define __prepost_count(count)           __pre_count(count)           __post_count(count)
#define __prepost_opt_count(count)       __pre_opt_count(count)       __post_count(count)
#define __prepost_bytecount(count)       __pre_bytecount(count)       __post_bytecount(count)
#define __prepost_opt_bytecount(count)   __pre_opt_bytecount(count)   __post_bytecount(count)
#define __prepost_count_x(count)         __pre_count_x(count)         __post_count_x(count)
#define __prepost_opt_count_x(count)     __pre_opt_count_x(count)     __post_count_x(count)
#define __prepost_bytecount_x(count)     __pre_bytecount_x(count)     __post_bytecount_x(count)
#define __prepost_opt_bytecount_x(count) __pre_opt_bytecount_x(count) __post_bytecount_x(count)

#define __prepost_valid                  __pre_valid     __post_valid
#define __prepost_opt_valid              __pre_opt_valid __post_valid

//
// __deref_<both> ---
//
// short version for __deref_pre_<ann> __deref_post_<ann>
// describing conditions for array elements or dereferenced pointer parameters that hold befor and after the call

#define __deref_prepost_z                       __deref_pre_z     __deref_post_z
#define __deref_prepost_opt_z                   __deref_pre_opt_z __deref_post_opt_z

#define __deref_prepost_cap(cap)                __deref_pre_cap(cap)                __deref_post_cap(cap)
#define __deref_prepost_opt_cap(cap)            __deref_pre_opt_cap(cap)            __deref_post_opt_cap(cap)
#define __deref_prepost_bytecap(cap)            __deref_pre_bytecap(cap)            __deref_post_bytecap(cap)
#define __deref_prepost_opt_bytecap(cap)        __deref_pre_opt_bytecap(cap)        __deref_post_opt_bytecap(cap)

#define __deref_prepost_cap_x(cap)              __deref_pre_cap_x(cap)              __deref_post_cap_x(cap)             
#define __deref_prepost_opt_cap_x(cap)          __deref_pre_opt_cap_x(cap)          __deref_post_opt_cap_x(cap)         
#define __deref_prepost_bytecap_x(cap)          __deref_pre_bytecap_x(cap)          __deref_post_bytecap_x(cap)             
#define __deref_prepost_opt_bytecap_x(cap)      __deref_pre_opt_bytecap_x(cap)      __deref_post_opt_bytecap_x(cap)         

#define __deref_prepost_z_cap(cap)              __deref_pre_z_cap(cap)              __deref_post_z_cap(cap)             
#define __deref_prepost_opt_z_cap(cap)          __deref_pre_opt_z_cap(cap)          __deref_post_opt_z_cap(cap)         
#define __deref_prepost_z_bytecap(cap)          __deref_pre_z_bytecap(cap)          __deref_post_z_bytecap(cap)         
#define __deref_prepost_opt_z_bytecap(cap)      __deref_pre_opt_z_bytecap(cap)      __deref_post_opt_z_bytecap(cap)     

#define __deref_prepost_count(count)            __deref_pre_count(count)            __deref_post_count(count)
#define __deref_prepost_opt_count(count)        __deref_pre_opt_count(count)        __deref_post_opt_count(count)
#define __deref_prepost_bytecount(count)        __deref_pre_bytecount(count)        __deref_post_bytecount(count)
#define __deref_prepost_opt_bytecount(count)    __deref_pre_opt_bytecount(count)    __deref_post_opt_bytecount(count)

#define __deref_prepost_count_x(count)          __deref_pre_count_x(count)          __deref_post_count_x(count)
#define __deref_prepost_opt_count_x(count)      __deref_pre_opt_count_x(count)      __deref_post_opt_count_x(count)
#define __deref_prepost_bytecount_x(count)      __deref_pre_bytecount_x(count)      __deref_post_bytecount_x(count)
#define __deref_prepost_opt_bytecount_x(count)  __deref_pre_opt_bytecount_x(count)  __deref_post_opt_bytecount_x(count)

#define __deref_prepost_valid                   __deref_pre_valid     __deref_post_valid
#define __deref_prepost_opt_valid               __deref_pre_opt_valid __deref_post_opt_valid

// -------------------------------------------------------------------------------
// Deprecated SAL version 1 macros
//
// -------------------------------------------------------------------------------

#define __ecount(size)                                          __pre_cap(size)       // __notnull __elem_writableTo(size)
#define __bcount(size)                                          __pre_bytecap(size)   //__notnull __byte_writableTo(size)
#define __xcount(size)                                          __pre_cap_x(size)     // __notnull __inexpressible_writableTo(size)
//#define __in                                                    __pre_valid __pre_readonly
#define __in_ecount(size)                    /*?*/              __pre_count(size)  __pre_readonly            //__in __pre __elem_readableTo(size)
#define __in_bcount(size)                    /*?*/              __pre_bytecount(size)  __pre_readonly        // __in  __pre __byte_readableTo(size)
#define __in_xcount(size)                                       __pre_count_x(size)  __pre_readonly          // __in __pre __inexpressible_readableTo(size)
//#define __in_z                                                  __pre_z  __pre_readonly
#define __in_ecount_z(size)                  /*?*/              __pre_z __pre_count(size)  __pre_readonly    //__in_ecount(size) __pre __nullterminated
#define __in_bcount_z(size)                  /*?*/              __pre_z __pre_bytecount(size) __pre_readonly // __in_bcount(size) __pre __nullterminated
//#define __out                                                   __pre_cap_c(1) __post_valid
#define __out_ecount(size)                                      __pre_cap(size) __post_valid                 // __ecount(size) __post __valid __refparam
#define __out_bcount(size)                                      __pre_bytecap(size) __post_valid             // __bcount(size) __post __valid __refparam
#define __out_xcount(size)                                      __pre_cap_x(size) __post_valid               // __xcount(size) __post __valid __refparam
#define __out_ecount_part(size,length)                          __pre_cap(size) __post_count(length)         // __out_ecount(size) __post __elem_readableTo(length)
#define __out_bcount_part(size,length)                          __pre_bytecap(size) __post_bytecount(length) // __out_bcount(size) __post __byte_readableTo(length)
#define __out_xcount_part(size,length)                          __pre_cap_x(size) __post_count_x(length)     // __out_xcount(size) __post __inexpressible_readableTo(length)
#define __out_ecount_full(size)                                 __pre_cap(size) __post_count(size)           // __out_ecount_part(size,size)
#define __out_bcount_full(size)                                 __pre_bytecap(size) __post_bytecount(size)   // __out_bcount_part(size,size)
#define __out_xcount_full(size)                                 __pre_cap_x(size) __post_count_x(size)       //__out_xcount_part(size,size)
#define __out_z                                                 __pre_notnull __post_z                       //   __post __valid __refparam __post __nullterminated
#define __out_ecount_z(size)                                    __pre_cap(size) __post_z                     // __ecount(size) __post __valid __refparam __post __nullterminated
#define __out_bcount_z(size)                                    __pre_bytecap(size) __post_z                 // __bcount(size) __post __valid __refparam __post __nullterminated
//#define __inout                                                 __pre_count_c(1) __post_valid                   // __pre __valid __post __valid __refparam
#define __inout_ecount(size)                                    __pre_valid __pre_cap(size) __post_valid        // __out_ecount(size) __pre __valid
#define __inout_bcount(size)                                    __pre_valid __pre_bytecap(size) __post_valid    // __out_bcount(size) __pre __valid
#define __inout_xcount(size)                                    __pre_valid __pre_cap_x(size) __post_valid      // __out_xcount(size) __pre __valid
#define __inout_ecount_part(size,length)     /*?*/              __pre_cap(size) __prepost_count(length)         // __out_ecount_part(size,length) __pre __valid __pre __elem_readableTo(length)
#define __inout_bcount_part(size,length)     /*?*/              __pre_bytecap(size) __prepost_bytecount(length) // __out_bcount_part(size,length) __pre __valid __pre __byte_readableTo(length)
#define __inout_xcount_part(size,length)                        __pre_cap_x(size) __prepost_count_x(length)     // __out_xcount_part(size,length) __pre __valid __pre __inexpressible_readableTo(length)
#define __inout_ecount_full(size)                               __prepost_count(length)                         // __inout_ecount_part(size,size)
#define __inout_bcount_full(size)                               __prepost_bytecount(length)                     // __inout_bcount_part(size,size)
#define __inout_xcount_full(size)                               __prepost_count_x(length)                       // __inout_xcount_part(size,size)
//#define __inout_z                                               __prepost_z                                     // __inout __pre __nullterminated __post __nullterminated
#define __inout_ecount_z(size)                                  __pre_z_cap(size) __post_z                      // __inout_ecount(size) __pre __nullterminated __post __nullterminated
#define __inout_bcount_z(size)                                  __pre_z_bytecap(size) __post_z                  // __inout_bcount(size) __pre __nullterminated __post __nullterminated

#define __ecount_opt(size)                                      __pre_opt_cap(size)     // __ecount(size) __exceptthat __maybenull
#define __bcount_opt(size)                                      __pre_opt_bytecap(size) // __bcount(size) __exceptthat __maybenull
#define __xcount_opt(size)                                      __pre_opt_cap_x(size)   // __xcount(size)                              __exceptthat __maybenull
//#define __in_opt                                                __pre_opt_valid __pre_readonly                   // __in                                        __exceptthat __maybenull
#define __in_ecount_opt(size)                /*?*/              __pre_opt_count(size) __pre_readonly             // __in_ecount(size)                           __exceptthat __maybenull
#define __in_bcount_opt(size)                /*?*/              __pre_opt_bytecount(size) __pre_readonly         // __in_bcount(size)                           __exceptthat __maybenull
#define __in_xcount_opt(size)                                   __pre_opt_count_x(size) __pre_readonly           // __in_xcount(size)                           __exceptthat __maybenull
#define __in_z_opt                                              __pre_opt_z __pre_readonly                       // __in_opt __pre __nullterminated 
#define __in_ecount_z_opt(size)              /*?*/              __pre_opt_z __pre_count(size) __pre_readonly     // __in_ecount_opt(size) __pre __nullterminated 
#define __in_bcount_z_opt(size)              /*?*/              __pre_opt_z __pre_bytecount(size) __pre_readonly // __in_bcount_opt(size) __pre __nullterminated
//#define __out_opt                                               __pre_opt_cap_c(1) __post_valid                  // __out                                       __exceptthat __maybenull
#define __out_ecount_opt(size)                                  __pre_opt_cap(size) __post_valid                 // __out_ecount(size)                          __exceptthat __maybenull
#define __out_bcount_opt(size)                                  __pre_opt_bytecap(size) __post_valid             // __out_bcount(size)                          __exceptthat __maybenull
#define __out_xcount_opt(size)                                  __pre_opt_cap_x(size) __post_valid               // __out_xcount(size)                          __exceptthat __maybenull
#define __out_ecount_part_opt(size,length)                      __pre_opt_cap(size) __post_count(length)         // __out_ecount_part(size,length)              __exceptthat __maybenull
#define __out_bcount_part_opt(size,length)                      __pre_opt_bytecap(size) __post_bytecount(length) // __out_bcount_part(size,length)              __exceptthat __maybenull
#define __out_xcount_part_opt(size,length)                      __pre_opt_cap_x(size) __post_count_x(length)     // __out_xcount_part(size,length)              __exceptthat __maybenull
#define __out_ecount_full_opt(size)                             __pre_opt_cap(size) __post_count(size)           // __out_ecount_full(size)                     __exceptthat __maybenull
#define __out_bcount_full_opt(size)                             __pre_opt_bytecap(size) __post_bytecount(size)   // __out_bcount_full(size)                     __exceptthat __maybenull
#define __out_xcount_full_opt(size)                             __pre_opt_cap_x(size) __post_count_x(size)       // __out_xcount_full(size)                     __exceptthat __maybenull
#define __out_z_opt                                             __pre_maybenull __post_z                         // __post __valid __refparam __post __nullterminated __exceptthat __maybenull
#define __out_ecount_z_opt(size)                                __pre_opt_cap(size) __post_z                     // __out_ecount_opt(size) __post __nullterminated
#define __out_bcount_z_opt(size)                                __pre_opt_bytecap(size) __post_z                 // __out_bcount_opt(size) __post __nullterminated
//#define __inout_opt                                             __pre_opt_count_c(1) __post_valid                    //__inout                                     __exceptthat __maybenull
#define __inout_ecount_opt(size)                                __pre_opt_valid __pre_opt_cap(size) __post_valid     // __inout_ecount(size)                        __exceptthat __maybenull
#define __inout_bcount_opt(size)                                __pre_opt_valid __pre_opt_bytecap(size) __post_valid // __inout_bcount(size)                        __exceptthat __maybenull
#define __inout_xcount_opt(size)                                __pre_opt_valid __pre_opt_cap_x(size) __post_valid   // __inout_xcount(size)                        __exceptthat __maybenull
#define __inout_ecount_part_opt(size,length) /*?*/              __pre_opt_cap(size) __pre_opt_count(length) __post_count(length) // __inout_ecount_part(size,length)            __exceptthat __maybenull
#define __inout_bcount_part_opt(size,length) /*?*/              __pre_opt_bytecap(size) __pre_opt_bytecount(length) __post_bytecount(length)// __inout_bcount_part(size,length)            __exceptthat __maybenull
#define __inout_xcount_part_opt(size,length)                    __pre_opt_cap_x(size) __pre_opt_count_x(length) __post_count_x(length) // __inout_xcount_part(size,length)            __exceptthat __maybenull
#define __inout_ecount_full_opt(size)                           __pre_opt_count(length) __post_count(length)         // __inout_ecount_full(size)                   __exceptthat __maybenull
#define __inout_bcount_full_opt(size)                           __pre_opt_bytecount(length) __post_bytecount(length) // __inout_bcount_full(size)                   __exceptthat __maybenull
#define __inout_xcount_full_opt(size)                           __pre_opt_count_x(length) __post_count_x(length)     // __inout_xcount_full(size)                   __exceptthat __maybenull
#define __inout_z_opt                                           __pre_opt_z __post_z                                 // __inout_opt __pre __nullterminated __post __nullterminated
#define __inout_ecount_z_opt(size)                              __pre_opt_z_cap(size) __post_z                       // __inout_ecount_opt(size) __pre __nullterminated __post __nullterminated
#define __inout_bcount_z_opt(size)                              __pre_opt_z_bytecap(size) __post_z                   // __inout_bcount_opt(size) __pre __nullterminated __post __nullterminated

#define __deref_ecount(size)                                    __out __deref_post_cap(size)     // __ecount(1) __post __elem_readableTo(1) __post __deref __notnull __post __deref __elem_writableTo(size)
#define __deref_bcount(size)                                    __out __deref_post_bytecap(size) // __ecount(1) __post __elem_readableTo(1) __post __deref __notnull __post __deref __byte_writableTo(size)
#define __deref_xcount(size)                                    __out __deref_post_cap_x(size)   // __ecount(1) __post __elem_readableTo(1) __post __deref __notnull __post __deref __inexpressible_writableTo(size)
#define __deref_in                                              __in __deref_pre_count_c(1)      // __in __pre __deref __deref __readonly
#define __deref_in_ecount(size)                                 __in __deref_pre_count(size)     // __deref_in __pre __deref __elem_readableTo(size)
#define __deref_in_bcount(size)                                 __in __deref_pre_bytecount(size) // __deref_in __pre __deref __byte_readableTo(size)
#define __deref_in_xcount(size)                                 __in __deref_pre_count_x(size)   // __deref_in __pre __deref __inexpressible_readableTo(size)
// #define __deref_out                                             __out __deref_post_count_c(1)                   // __deref_ecount(1) __post __deref __valid __refparam
#define __deref_out_ecount(size)                                __out __deref_post_cap(size)                    // __deref_ecount(size) __post __deref __valid __refparam
#define __deref_out_bcount(size)                                __out __deref_post_bytecap(size)                // __deref_bcount(size) __post __deref __valid __refparam
#define __deref_out_xcount(size)                                __out __deref_post_cap_x(size)                  // __deref_xcount(size) __post __deref __valid __refparam
#define __deref_out_ecount_part(size,length)                    __out __deref_post_cap(size) __deref_post_count(length)         // __deref_out_ecount(size) __post __deref __elem_readableTo(length)
#define __deref_out_bcount_part(size,length)                    __out __deref_post_bytecap(size) __deref_post_bytecount(length) // __deref_out_bcount(size) __post __deref __byte_readableTo(length)
#define __deref_out_xcount_part(size,length)                    __out __deref_post_cap_x(size) __deref_post_count_x(length)     // __deref_out_xcount(size) __post __deref __inexpressible_readableTo(length)
#define __deref_out_ecount_full(size)                           __out __deref_post_count(size)                  // __deref_out_ecount_part(size,size)
#define __deref_out_bcount_full(size)                           __out __deref_post_bytecount(size)              // __deref_out_bcount_part(size,size)
#define __deref_out_xcount_full(size)                           __out __deref_post_count_x(size)                // __deref_out_xcount_part(size,size)
#define __deref_out_z                                           __out __deref_post_z                            // __post __deref __valid __refparam __post __deref __nullterminated
#define __deref_out_ecount_z(size)                              __out __deref_post_cap(size) __deref_post_z     // __deref_out_ecount(size) __post __deref __nullterminated  
#define __deref_out_bcount_z(size)                              __out __deref_post_bytecap(size) __deref_post_z // __deref_out_ecount(size) __post __deref __nullterminated  
#define __deref_inout                                           __inout __deref_pre_count_c(1) __deref_post_count_c(1)                  // __notnull __elem_readableTo(1) __pre __deref __valid __post __deref __valid __refparam
#define __deref_inout_ecount(size)                              __inout __deref_prepost_cap(size)       // __deref_inout __pre __deref __elem_writableTo(size) __post __deref __elem_writableTo(size)
#define __deref_inout_bcount(size)                              __inout __deref_prepost_bytecap(size)   // __deref_inout __pre __deref __byte_writableTo(size) __post __deref __byte_writableTo(size)
#define __deref_inout_xcount(size)                              __inout __deref_prepost_cap_x(size)     // __deref_inout __pre __deref __inexpressible_writableTo(size) __post __deref __inexpressible_writableTo(size)
#define __deref_inout_ecount_part(size,length)/*?*/             __inout __deref_prepost_cap(size) __deref_prepost_count(length)         // __deref_inout_ecount(size) __pre __deref __elem_readableTo(length) __post __deref __elem_readableTo(length)
#define __deref_inout_bcount_part(size,length)/*?*/             __inout __deref_prepost_bytecap(size) __deref_prepost_bytecount(length) // __deref_inout_bcount(size) __pre __deref __byte_readableTo(length) __post __deref __byte_readableTo(length)
#define __deref_inout_xcount_part(size,length)                  __inout __deref_prepost_cap_x(size) __deref_prepost_count_x(length)     // __deref_inout_xcount(size) __pre __deref __inexpressible_readableTo(length) __post __deref __inexpressible_readableTo(length)
#define __deref_inout_ecount_full(size)                         __inout __deref_prepost_count(size)     // __deref_inout_ecount_part(size,size)
#define __deref_inout_bcount_full(size)                         __inout __deref_prepost_bytecount(size) // __deref_inout_bcount_part(size,size)
#define __deref_inout_xcount_full(size)                         __inout __deref_prepost_count_x(size)   // __deref_inout_xcount_part(size,size)
#define __deref_inout_z                                         __inout __deref_prepost_z               // __deref_inout __pre __deref __nullterminated __post __deref __nullterminated
#define __deref_inout_ecount_z(size)                            __inout __deref_prepost_z_cap(size)     // __deref_inout_ecount(size) __pre __deref __nullterminated __post __deref __nullterminated   
#define __deref_inout_bcount_z(size)                            __inout __deref_prepost_z_bytecap(size) // __deref_inout_ecount(size) __pre __deref __nullterminated __post __deref __nullterminated  

#define __deref_ecount_opt(size)             /*?*/              __out __deref_post_opt_cap(size)     // __deref_ecount(size)                        __post __deref __exceptthat __maybenull
#define __deref_bcount_opt(size)             /*?*/              __out __deref_post_opt_bytecap(size) // __deref_bcount(size)                        __post __deref __exceptthat __maybenull
#define __deref_xcount_opt(size)                                __out __deref_post_opt_cap_x(size)   // __deref_xcount(size)                        __post __deref __exceptthat __maybenull
#define __deref_in_opt                                          __in __deref_pre_opt_count_c(1)      // __deref_in                                  __pre __deref __exceptthat __maybenull
#define __deref_in_ecount_opt(size)                             __in __deref_pre_opt_count(size)     // __deref_in_ecount(size)                     __pre __deref __exceptthat __maybenull
#define __deref_in_bcount_opt(size)                             __in __deref_pre_opt_bytecount(size) // __deref_in_bcount(size)                     __pre __deref __exceptthat __maybenull
#define __deref_in_xcount_opt(size)                             __in __deref_pre_opt_count_x(size)   // __deref_in_xcount(size)                     __pre __deref __exceptthat __maybenull
// #define __deref_out_opt                                         __out __deref_post_opt_count_c(1)                       // __deref_out                                 __post __deref __exceptthat __maybenull
#define __deref_out_ecount_opt(size)                            __out __deref_post_opt_cap(size)                        // __deref_out_ecount(size)                    __post __deref __exceptthat __maybenull
#define __deref_out_bcount_opt(size)                            __out __deref_post_opt_bytecap(size)                    // __deref_out_bcount(size)                    __post __deref __exceptthat __maybenull
#define __deref_out_xcount_opt(size)                            __out __deref_post_opt_cap_x(size)                      // __deref_out_xcount(size)                    __post __deref __exceptthat __maybenull
#define __deref_out_ecount_part_opt(size,length)                __out __deref_post_opt_cap(size) __deref_post_opt_count(length)         // __deref_out_ecount_part(size,length)        __post __deref __exceptthat __maybenull
#define __deref_out_bcount_part_opt(size,length)                __out __deref_post_opt_bytecap(size) __deref_post_opt_bytecount(length) // __deref_out_bcount_part(size,length)        __post __deref __exceptthat __maybenull
#define __deref_out_xcount_part_opt(size,length)                __out __deref_post_opt_cap_x(size) __deref_post_opt_count_x(length)     // __deref_out_xcount_part(size,length)        __post __deref __exceptthat __maybenull
#define __deref_out_ecount_full_opt(size)                       __out __deref_post_opt_count(size)                      // __deref_out_ecount_full(size)               __post __deref __exceptthat __maybenull
#define __deref_out_bcount_full_opt(size)                       __out __deref_post_opt_bytecount(size)                  // __deref_out_bcount_full(size)               __post __deref __exceptthat __maybenull
#define __deref_out_xcount_full_opt(size)                       __out __deref_post_opt_count_x(size)                    // __deref_out_xcount_full(size)               __post __deref __exceptthat __maybenull
#define __deref_out_z_opt                                       __out __deref_out_opt_z                                 // __post __deref __valid __refparam __execeptthat __maybenull __post __deref __nullterminated
#define __deref_out_ecount_z_opt(size)                          __out __deref_post_opt_cap(size) __deref_post_opt_z     // __deref_out_ecount_opt(size) __post __deref __nullterminated
#define __deref_out_bcount_z_opt(size)                          __out __deref_post_opt_bytecap(size) __deref_post_opt_z // __deref_out_bcount_opt(size) __post __deref __nullterminated
#define __deref_inout_opt                                       __inout __deref_pre_opt_count_c(1) __deref_post_opt_count_c(1)                  // __deref_inout                               __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull
#define __deref_inout_ecount_opt(size)                          __inout __deref_prepost_opt_cap(size)       // __deref_inout_ecount(size)                  __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull
#define __deref_inout_bcount_opt(size)                          __inout __deref_prepost_opt_bytecap(size)   // __deref_inout_bcount(size)                  __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull
#define __deref_inout_xcount_opt(size)                          __inout __deref_prepost_opt_cap_x(size)     // __deref_inout_xcount(size)                  __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull
#define __deref_inout_ecount_part_opt(size,length)              __inout __deref_prepost_opt_cap(size) __deref_prepost_opt_count(length)         // __deref_inout_ecount_part(size,length)      __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull
#define __deref_inout_bcount_part_opt(size,length)              __inout __deref_prepost_opt_bytecap(size) __deref_prepost_opt_bytecount(length) // __deref_inout_bcount_part(size,length)      __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull
#define __deref_inout_xcount_part_opt(size,length)              __inout __deref_prepost_opt_cap_x(size) __deref_prepost_opt_count_x(length)     // __deref_inout_xcount_part(size,length)      __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull
#define __deref_inout_ecount_full_opt(size)                     __inout __deref_prepost_opt_count(size)     // __deref_inout_ecount_full(size)             __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull
#define __deref_inout_bcount_full_opt(size)                     __inout __deref_prepost_opt_bytecount(size) // __deref_inout_bcount_full(size)             __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull
#define __deref_inout_xcount_full_opt(size)                     __inout __deref_prepost_opt_count_x(size)   // __deref_inout_xcount_full(size)             __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull
#define __deref_inout_z_opt                                     __inout __deref_prepost_opt_z               // __deref_inout_opt __pre __deref __nullterminated __post __deref __nullterminated
#define __deref_inout_ecount_z_opt(size)                        __inout __deref_prepost_opt_z_cap(size)     // __deref_inout_ecount_opt(size) __pre __deref __nullterminated __post __deref __nullterminated
#define __deref_inout_bcount_z_opt(size)                        __inout __deref_prepost_opt_z_bytecap(size) // __deref_inout_bcount_opt(size) __pre __deref __nullterminated __post __deref __nullterminated

#define __deref_opt_ecount(size)                                __out_opt __deref_post_cap(size)     // __deref_ecount(size)                        __exceptthat __maybenull
#define __deref_opt_bcount(size)                                __out_opt __deref_post_bytecap(size) // __deref_bcount(size)                        __exceptthat __maybenull
#define __deref_opt_xcount(size)                                __out_opt __deref_post_cap_x(size)   // __deref_xcount(size)                        __exceptthat __maybenull
#define __deref_opt_in                                          __in_opt __deref_pre_count_c(1)      // __deref_in                                  __exceptthat __maybenull
#define __deref_opt_in_ecount(size)                             __in_opt __deref_pre_count(size)     // __deref_in_ecount(size)                     __exceptthat __maybenull
#define __deref_opt_in_bcount(size)                             __in_opt __deref_pre_bytecount(size) // __deref_in_bcount(size)                     __exceptthat __maybenull
#define __deref_opt_in_xcount(size)                             __in_opt __deref_pre_count_x(size)   // __deref_in_xcount(size)                     __exceptthat __maybenull
#define __deref_opt_out                                         __out_opt __deref_post_count_c(1)                   // __deref_out                                 __exceptthat __maybenull
#define __deref_opt_out_ecount(size)                            __out_opt __deref_post_cap(size)                    // __deref_out_ecount(size)                    __exceptthat __maybenull
#define __deref_opt_out_bcount(size)                            __out_opt __deref_post_bytecap(size)                // __deref_out_bcount(size)                    __exceptthat __maybenull
#define __deref_opt_out_xcount(size)                            __out_opt __deref_post_cap_x(size)                  // __deref_out_xcount(size)                    __exceptthat __maybenull
#define __deref_opt_out_ecount_part(size,length)                __out_opt __deref_post_cap(size) __deref_post_count(length)         // __deref_out_ecount_part(size,length)        __exceptthat __maybenull
#define __deref_opt_out_bcount_part(size,length)                __out_opt __deref_post_bytecap(size) __deref_post_bytecount(length) // __deref_out_bcount_part(size,length)        __exceptthat __maybenull
#define __deref_opt_out_xcount_part(size,length)                __out_opt __deref_post_cap_x(size) __deref_post_count_x(length)     // __deref_out_xcount_part(size,length)        __exceptthat __maybenull
#define __deref_opt_out_ecount_full(size)                       __out_opt __deref_post_count(size)                  // __deref_out_ecount_full(size)               __exceptthat __maybenull
#define __deref_opt_out_bcount_full(size)                       __out_opt __deref_post_bytecount(size)              // __deref_out_bcount_full(size)               __exceptthat __maybenull
#define __deref_opt_out_xcount_full(size)                       __out_opt __deref_post_count_x(size)                // __deref_out_xcount_full(size)               __exceptthat __maybenull
#define __deref_opt_out_z                                       __out_opt __deref_post_z                            // __deref_opt_out __pre __deref __nullterminated __post __deref __nullterminated
#define __deref_opt_out_ecount_z(size)                          __out_opt __deref_post_cap(size) __deref_post_z     // missing
#define __deref_opt_out_bcount_z(size)                          __out_opt __deref_post_bytecap(size) __deref_post_z // missing
#define __deref_opt_inout                                       __inout_opt __deref_pre_count_c(1) __deref_post_count_c(1)                  // __deref_inout                               __exceptthat __maybenull
#define __deref_opt_inout_ecount(size)                          __inout_opt __deref_prepost_cap(size)       // __deref_inout_ecount(size)                  __exceptthat __maybenull
#define __deref_opt_inout_bcount(size)                          __inout_opt __deref_prepost_bytecap(size)   // __deref_inout_bcount(size)                  __exceptthat __maybenull
#define __deref_opt_inout_xcount(size)                          __inout_opt __deref_prepost_cap_x(size)     // __deref_inout_xcount(size)                  __exceptthat __maybenull
#define __deref_opt_inout_ecount_part(size,length)              __inout_opt __deref_prepost_cap(size) __deref_prepost_count(length)         // __deref_inout_ecount_part(size,length)      __exceptthat __maybenull
#define __deref_opt_inout_bcount_part(size,length)              __inout_opt __deref_prepost_bytecap(size) __deref_prepost_bytecount(length) // __deref_inout_bcount_part(size,length)      __exceptthat __maybenull
#define __deref_opt_inout_xcount_part(size,length)              __inout_opt __deref_prepost_cap_x(size) __deref_prepost_count_x(length)     // __deref_inout_xcount_part(size,length)      __exceptthat __maybenull
#define __deref_opt_inout_ecount_full(size)                     __inout_opt __deref_prepost_count(size)     // __deref_inout_ecount_full(size)             __exceptthat __maybenull
#define __deref_opt_inout_bcount_full(size)                     __inout_opt __deref_prepost_bytecount(size) // __deref_inout_bcount_full(size)             __exceptthat __maybenull
#define __deref_opt_inout_xcount_full(size)                     __inout_opt __deref_prepost_count_x(size)   // __deref_inout_xcount_full(size)             __exceptthat __maybenull
#define __deref_opt_inout_z                                     __inout_opt __deref_prepost_z               // __deref_opt_inout __pre __deref __nullterminated __post __deref __nullterminated
#define __deref_opt_inout_ecount_z(size)                        __inout_opt __deref_prepost_z_cap(size)     // __deref_opt_inout_ecount(size) __pre __deref __nullterminated __post __deref __nullterminated
#define __deref_opt_inout_bcount_z(size)                        __inout_opt __deref_prepost_z_bytecap(size) // __deref_opt_inout_bcount(size) __pre __deref __nullterminated __post __deref __nullterminated

#define __deref_opt_ecount_opt(size)                            __out_opt __deref_post_opt_cap(size)     // __deref_ecount_opt(size)                    __exceptthat __maybenull
#define __deref_opt_bcount_opt(size)                            __out_opt __deref_post_opt_bytecap(size) // __deref_bcount_opt(size)                    __exceptthat __maybenull
#define __deref_opt_xcount_opt(size)                            __out_opt __deref_post_opt_cap_x(size)   // __deref_xcount_opt(size)                    __exceptthat __maybenull
#define __deref_opt_in_opt                                      __in_opt __deref_pre_opt_count_c(1)      // __deref_in_opt                              __exceptthat __maybenull
#define __deref_opt_in_ecount_opt(size)                         __in_opt __deref_pre_opt_count(size)     // __deref_in_ecount_opt(size)                 __exceptthat __maybenull
#define __deref_opt_in_bcount_opt(size)                         __in_opt __deref_pre_opt_bytecount(size) // __deref_in_bcount_opt(size)                 __exceptthat __maybenull
#define __deref_opt_in_xcount_opt(size)                         __in_opt __deref_pre_opt_count_x(size)   // __deref_in_xcount_opt(size)                 __exceptthat __maybenull
#define __deref_opt_out_opt                                     __out_opt __deref_post_opt_count_c(1)                       // __deref_out_opt                             __exceptthat __maybenull
#define __deref_opt_out_ecount_opt(size)                        __out_opt __deref_post_opt_cap(size)                        // __deref_out_ecount_opt(size)                __exceptthat __maybenull
#define __deref_opt_out_bcount_opt(size)                        __out_opt __deref_post_opt_bytecap(size)                    // __deref_out_bcount_opt(size)                __exceptthat __maybenull
#define __deref_opt_out_xcount_opt(size)                        __out_opt __deref_post_opt_cap_x(size)                      // __deref_out_xcount_opt(size)                __exceptthat __maybenull
#define __deref_opt_out_ecount_part_opt(size,length)            __out_opt __deref_post_opt_cap(size) __deref_post_count(length)         // __deref_out_ecount_part_opt(size,length)    __exceptthat __maybenull
#define __deref_opt_out_bcount_part_opt(size,length)            __out_opt __deref_post_opt_bytecap(size) __deref_post_bytecount(length) // __deref_out_bcount_part_opt(size,length)    __exceptthat __maybenull
#define __deref_opt_out_xcount_part_opt(size,length)            __out_opt __deref_post_opt_cap_x(size) __deref_post_count_x(length)     // __deref_out_xcount_part_opt(size,length)    __exceptthat __maybenull
#define __deref_opt_out_ecount_full_opt(size)                   __out_opt __deref_post_opt_count(size)                      // __deref_out_ecount_full_opt(size)           __exceptthat __maybenull
#define __deref_opt_out_bcount_full_opt(size)                   __out_opt __deref_post_opt_bytecount(size)                  // __deref_out_bcount_full_opt(size)           __exceptthat __maybenull
#define __deref_opt_out_xcount_full_opt(size)                   __out_opt __deref_post_opt_count_x(size)                    // __deref_out_xcount_full_opt(size)           __exceptthat __maybenull
#define __deref_opt_out_z_opt                                   __out_opt __deref_post_opt_z                                // __post __deref __valid __refparam __exceptthat __maybenull __pre __deref __exceptthat __maybenull __post __deref __exceptthat __maybenull __post __deref __nullterminated
#define __deref_opt_out_ecount_z_opt(size)                      __out_opt __deref_post_opt_cap(size) __deref_post_opt_z     // __deref_opt_out_ecount_opt(size) __post __deref __nullterminated
#define __deref_opt_out_bcount_z_opt(size)                      __out_opt __deref_post_opt_bytecap(size) __deref_post_opt_z // __deref_opt_out_bcount_opt(size) __post __deref __nullterminated
#define __deref_opt_inout_opt                                   __inout_opt __deref_pre_opt_count_c(1) __deref_post_opt_count_c(1)                  // __deref_inout_opt                           __exceptthat __maybenull
#define __deref_opt_inout_ecount_opt(size)                      __inout_opt __deref_prepost_opt_cap(size)       // __deref_inout_ecount_opt(size)              __exceptthat __maybenull
#define __deref_opt_inout_bcount_opt(size)                      __inout_opt __deref_prepost_opt_bytecap(size)   // __deref_inout_bcount_opt(size)              __exceptthat __maybenull
#define __deref_opt_inout_xcount_opt(size)                      __inout_opt __deref_prepost_opt_cap_x(size)     // __deref_inout_xcount_opt(size)              __exceptthat __maybenull
#define __deref_opt_inout_ecount_part_opt(size,length)          __inout_opt __deref_prepost_opt_cap(size) __deref_prepost_opt_count(length)         // __deref_inout_ecount_part_opt(size,length)  __exceptthat __maybenull
#define __deref_opt_inout_bcount_part_opt(size,length)          __inout_opt __deref_prepost_opt_bytecap(size) __deref_prepost_opt_bytecount(length) // __deref_inout_bcount_part_opt(size,length)  __exceptthat __maybenull
#define __deref_opt_inout_xcount_part_opt(size,length)          __inout_opt __deref_prepost_opt_cap_x(size) __deref_prepost_opt_count_x(length)     // __deref_inout_xcount_part_opt(size,length)  __exceptthat __maybenull
#define __deref_opt_inout_ecount_full_opt(size)                 __inout_opt __deref_prepost_opt_count(size)     // __deref_inout_ecount_full_opt(size)         __exceptthat __maybenull
#define __deref_opt_inout_bcount_full_opt(size)                 __inout_opt __deref_prepost_opt_bytecount(size) // __deref_inout_bcount_full_opt(size)         __exceptthat __maybenull
#define __deref_opt_inout_xcount_full_opt(size)                 __inout_opt __deref_prepost_opt_count_x(size)   // __deref_inout_xcount_full_opt(size)         __exceptthat __maybenull
#define __deref_opt_inout_z_opt                                 __inout_opt __deref_prepost_opt_z               // __deref_opt_inout_opt  __pre __deref __nullterminated __post __deref __nullterminated             
#define __deref_opt_inout_ecount_z_opt(size)                    __inout_opt __deref_prepost_opt_z_cap(size)     // __deref_opt_inout_ecount_opt(size)  __pre __deref __nullterminated __post __deref __nullterminated
#define __deref_opt_inout_bcount_z_opt(size)                    __inout_opt __deref_prepost_opt_z_bytecap(size) // __deref_opt_inout_bcount_opt(size)  __pre __deref __nullterminated __post __deref __nullterminated

//
// Proposal for a new high level SAL annotation layer ----------------------------
//
// The names of the new macros are chosen in a way so they do not confict with
// the old SAL macros.
//
// Annotations starting with __in, __out or __inout should be sufficient to annotate
// most function parameters. The __pre and __post macros are more powerful but ought
// to be used in rare cases only


// 'in' parameters --------------------------

// input pointer parameter
// e.g. void SetPoint( __in const POINT* pPT );
#define __in                           __pre_valid  __pre_readonly
#define __in_opt                       __pre_opt_valid  __pre_readonly

// nullterminated 'in' parameters.
// e.g. void CopyStr( __in_z const char* szFrom, __out_z_cap(cchTo) char* szTo, size_t cchTo );
#define __in_z                         __pre_z  __pre_readonly
#define __in_opt_z                     __pre_opt_z __pre_readonly

// 'input' buffers with given size

// e.g. void SetCharRange( __in_count(cch) const char* rgch, size_t cch )
// valid buffer extent described by another parameter
#define __in_count(count)              __pre_count(count) __pre_readonly
#define __in_opt_count(count)          __pre_opt_count(count) __pre_readonly
#define __in_bytecount(count)          __pre_bytecount(count) __pre_readonly
#define __in_opt_bytecount(count)      __pre_opt_bytecount(count) __pre_readonly

// valid buffer extent described by a constant extression
#define __in_count_c(constExpr)         __pre_count_c(constExpr) __pre_readonly
#define __in_opt_count_c(constExpr)     __pre_opt_count_c(constExpr) __pre_readonly
#define __in_bytecount_c(constExpr)     __pre_bytecount_c(constExpr) __pre_readonly
#define __in_opt_bytecount_c(constExpr) __pre_opt_bytecount_c(constExpr) __pre_readonly

// 'x' version for complex expressions that are not supported by the current compiler version
// e.g. void Set3ColMatrix( __in_count_x(3*cRows) const Elem* matrix, int cRows );
#define __in_count_x(count)             __pre_count_x(count) __pre_readonly
#define __in_opt_count_x(count)         __pre_opt_count_x(count) __pre_readonly
#define __in_bytecount_x(count)         __pre_bytecount_x(count) __pre_readonly
#define __in_opt_bytecount_x(count)     __pre_opt_bytecount_x(count) __pre_readonly

// 'out' parameters --------------------------

// output pointer parameter
// e.g. void GetPoint( __out POINT* pPT );
#define __out                            __pre_cap_c(1)     __post_valid
#define __out_opt                        __pre_opt_cap_c(1) __post_valid

// 'out' with buffer size
// e.g. void GetIndeces( __out_cap(cIndeces) int* rgIndeces, size_t cIndices );
// buffer capacity is described by another parameter
#define __out_cap(cap)                   __pre_cap(cap)                 __post_valid
#define __out_opt_cap(cap)               __pre_opt_cap(cap)             __post_valid
#define __out_bytecap(cap)               __pre_bytecap(cap)             __post_valid
#define __out_opt_bytecap(cap)           __pre_opt_bytecap(cap)         __post_valid

// buffer capacity is described by a constant expression
#define __out_cap_c(constExpr)           __pre_cap_c(constExpr)         __post_valid
#define __out_opt_cap_c(constExpr)       __pre_opt_cap_c(constExpr)     __post_valid
#define __out_bytecap_c(constExpr)       __pre_bytecap_c(constExpr)     __post_valid
#define __out_opt_bytecap_c(constExpr)   __pre_opt_bytecap_c(constExpr) __post_valid

// buffer capacity is described by a complex expression
#define __out_cap_x(string)              __pre_cap_x(string)            __post_valid
#define __out_opt_cap_x(string)          __pre_opt_cap_x(string)        __post_valid
#define __out_bytecap_x(string)          __pre_bytecap_x(string)        __post_valid
#define __out_opt_bytecap_x(string)      __pre_opt_bytecap_x(string)    __post_valid

// a zero terminated string is filled into a buffer of given capacity
// e.g. void CopyStr( __in_z const char* szFrom, __out_z_cap(cchTo) char* szTo, size_t cchTo );
// buffer capacity is described by another parameter
#define __out_z_cap(cap)                 __pre_cap(cap)                 __post_z
#define __out_opt_z_cap(cap)             __pre_opt_cap(cap)             __post_z
#define __out_z_bytecap(cap)             __pre_bytecap(cap)             __post_z
#define __out_opt_z_bytecap(cap)         __pre_opt_bytecap(cap)         __post_z

// buffer capacity is described by a constant expression
#define __out_z_cap_c(constExpr)         __pre_cap_c(constExpr)         __post_z
#define __out_opt_z_cap_c(constExpr)     __pre_opt_cap_c(constExpr)     __post_z
#define __out_z_bytecap_c(constExpr)     __pre_bytecap_c(constExpr)     __post_z
#define __out_opt_z_bytecap_c(constExpr) __pre_opt_bytecap_c(constExpr) __post_z

// buffer capacity is described by a complex expression
#define __out_z_cap_x(string)            __pre_cap_x(string)            __post_z
#define __out_opt_z_cap_x(string)        __pre_opt_cap_x(string)        __post_z
#define __out_z_bytecap_x(string)        __pre_bytecap_x(string)        __post_z
#define __out_opt_z_bytecap_x(string)    __pre_opt_bytecap_x(string)    __post_z

// a zero terminated string is filled into a buffer of given capacity
// e.g. size_t CopyCharRange( __in_count(cchFrom) const char* rgchFrom, size_t cchFrom, __out_cap_post_count(cchTo,return)) char* rgchTo, size_t cchTo );
#define __out_cap_post_count(cap,count)               __pre_cap(cap)         __post_count(count)
#define __out_opt_cap_post_count(cap,count)           __pre_opt_cap(cap)     __post_count(count)
#define __out_bytecap_post_bytecount(cap,count)       __pre_bytecap(cap)     __post_bytecount(count)
#define __out_opt_bytecap_post_bytecount(cap,count)   __pre_opt_bytecap(cap) __post_bytecount(count)

// a zero terminated string is filled into a buffer of given capacity
// e.g. size_t CopyStr( __in_z const char* szFrom, __out_z_cap_post_count(cchTo,return+1) char* szTo, size_t cchTo );
#define __out_z_cap_post_count(cap,count)             __pre_cap(cap)         __post_z_count(count)
#define __out_opt_z_cap_post_count(cap,count)         __pre_opt_cap(cap)     __post_z_count(count)
#define __out_z_bytecap_post_bytecount(cap,count)     __pre_bytecap(cap)     __post_z_bytecount(count)
#define __out_opt_z_bytecap_post_bytecount(cap,count) __pre_opt_bytecap(cap) __post_z_bytecount(count)

// inout parameters ----------------------------

// inout pointer parameter
// e.g. void ModifyPoint( __inout POINT* pPT );
#define __inout                          __prepost_valid
#define __inout_opt                      __prepost_opt_valid

// string buffers
// e.g. void toupper( __inout_z char* sz );
#define __inout_z                        __prepost_z
#define __inout_opt_z                    __prepost_opt_z

// 'inout' buffers with initialized elements before and after the call
// e.g. void ModifyIndices( __inout_ecount(cIndices) int* rgIndeces, size_t cIndices );
#define __inout_count(count)                __pre_count(count)
#define __inout_opt_count(count)            __pre_opt_count(count)
#define __inout_bytecount(count)            __pre_bytecount(count)
#define __inout_opt_bytecount(count)        __pre_opt_bytecount(count)

#define __inout_count_c(constExpr)          __pre_count_c(constExpr)
#define __inout_opt_count_c(constExpr)      __pre_opt_count_c(constExpr)
#define __inout_bytecount_c(constExpr)      __pre_bytecount_c(constExpr)
#define __inout_opt_bytecount_c(constExpr)  __pre_opt_bytecount_c(constExpr)

#define __inout_count_x(count)              __pre_count_x(count)
#define __inout_opt_count_x(count)          __pre_opt_count_x(count)
#define __inout_bytecount_x(count)          __pre_bytecount_x(count)
#define __inout_opt_bytecount_x(count)      __pre_opt_bytecount_x(count)

// e.g. void AppendToLPSTR( __in LPCSTR szFrom, __inout_cap(cchTo) LPSTR* szTo, size_t cchTo );
#define __inout_cap(cap)                 __pre_valid     __pre_cap(cap)                 __post_valid
#define __inout_opt_cap(cap)             __pre_opt_valid __pre_opt_cap(cap)             __post_valid
#define __inout_bytecap(cap)             __pre_valid     __pre_bytecap(cap)             __post_valid
#define __inout_opt_bytecap(cap)         __pre_opt_valid __pre_opt_bytecap(cap)         __post_valid

#define __inout_cap_c(constExpr)         __pre_valid     __pre_cap_c(constExpr)         __post_valid
#define __inout_opt_cap_c(constExpr)     __pre_opt_valid __pre_opt_cap_c(constExpr)     __post_valid
#define __inout_bytecap_c(constExpr)     __pre_valid     __pre_bytecap_c(constExpr)     __post_valid
#define __inout_opt_bytecap_c(constExpr) __pre_opt_valid __pre_opt_bytecap_c(constExpr) __post_valid

#define __inout_cap_x(string)            __pre_valid     __pre_cap_x(string)            __post_valid
#define __inout_opt_cap_x(string)        __pre_opt_valid __pre_opt_cap_x(string)        __post_valid
#define __inout_bytecap_x(string)        __pre_valid     __pre_bytecap_x(string)        __post_valid
#define __inout_opt_bytecap_x(string)    __pre_opt_valid __pre_opt_bytecap_x(string)    __post_valid

// inout string buffers with writable size
// e.g. void AppendStr( __in_z const char* szFrom, __inout_z_cap(cchTo) char* szTo, size_t cchTo );
#define __inout_z_cap(cap)                 __pre_z_cap(cap)                 __post_z
#define __inout_opt_z_cap(cap)             __pre_opt_z_cap(cap)             __post_z
#define __inout_z_bytecap(cap)             __pre_z_bytecap(cap)             __post_z
#define __inout_opt_z_bytecap(cap)         __pre_opt_z_bytecap(cap)         __post_z

#define __inout_z_cap_c(constExpr)         __pre_z_cap_c(constExpr)         __post_z
#define __inout_opt_z_cap_c(constExpr)     __pre_opt_z_cap_c(constExpr)     __post_z
#define __inout_z_bytecap_c(constExpr)     __pre_z_bytecap_c(constExpr)     __post_z
#define __inout_opt_z_bytecap_c(constExpr) __pre_opt_z_bytecap_c(constExpr) __post_z

#define __inout_z_cap_x(string)            __pre_z_cap_x(string)            __post_z
#define __inout_opt_z_cap_x(string)        __pre_opt_z_cap_x(string)        __post_z
#define __inout_z_bytecap_x(string)        __pre_z_bytecap_x(string)        __post_z
#define __inout_opt_z_bytecap_x(string)    __pre_opt_z_bytecap_x(string)    __post_z


// return values -------------------------------

// returning pointers to valid objects
#define __ret                  __ret_count_c(1)
#define __ret_opt              __ret_opt_count_c(1)

#define __checkReturn          __inner_checkReturn

// More __ret annotations are defined below

// Pointer to pointers -------------------------

// e.g.  HRESULT HrCreatePoint( __deref_out_opt POINT** ppPT );
#define __deref_out            __out __deref_post_valid
#define __deref_out_opt        __out __deref_post_opt_valid

// e.g.  void CloneString( __in_z const wchar_t* wzFrom, __deref_out_z wchar_t** pWzTo );
#define __deref_out_z      __out __deref_post_z
#define __deref_out_opt_z  __out __deref_post_opt_z

// __deref_pre_* and __deref_post* annotations can be used to annotate more complicated case


// -------------------------------------------------------------------------------
// Advanced Annotation Definitions
//
// Any of these may be used to directly annotate functions, and may be used in
// combination with each other or with regular buffer macros. For an explanation
// of each annotation, see the advanced annotations section.
// -------------------------------------------------------------------------------

#if !defined(MIDL_PASS) && defined(_PREFAST_) && _MSC_VER >= 1000

    // In the primitive __declspec("SAL_*") annotations "SAL" stands for Standard
    // Annotation Language.  These __declspec("SAL_*") annotations are the
    // primitives the compiler understands and all high-level SpecString MACROs
    // will decompose into these primivates.

    #define SPECSTRINGIZE( x ) #x


    //
    // __inner_*
    //
    // Helper macros that directly correspond to certain high-level annotations.
    //
    //

    //
    // __inner_bound i
    // __inner_range(constant_low_bound, constant_up_bound) i
    //
    // Annotates an integer i. States that the integer belongs to known numeric range.
    // __inner_range includes the explicit range as [constant_low_bound, constant_up_bound].
    // __inner_bound does not include the range.
    //
    #define __inner_bound                     __declspec("SAL_bound")
    #define __inner_in_bound                  __declspec("SAL_pre") __inner_bound
    #define __inner_out_bound                 __declspec("SAL_post") __inner_bound
    #define __inner_deref_out_bound           __declspec("SAL_post") __declspec("SAL_deref") __inner_bound

    #define __inner_range(lb,ub)              __declspec("SAL_range(" SPECSTRINGIZE(lb) "," SPECSTRINGIZE(ub) ")")
    #define __inner_in_range(lb,ub)           __declspec("SAL_pre") __inner_range(lb,ub)
    #define __inner_out_range(lb,ub)          __declspec("SAL_post") __inner_range(lb,ub)
    #define __inner_deref_out_range(lb,ub)    __declspec("SAL_post") __declspec("SAL_deref") __inner_range(lb,ub)

    // Macros to classify the entrypoints and indicate their category.
    //
    //
    // Pre-defined control point categories include: RPC, LPC, DeviceDriver, UserToKernel, ISAPI, COM.
    //
    #define __inner_control_entrypoint(category) __declspec("SAL_entrypoint(controlEntry, "SPECSTRINGIZE(category)")")

    //
    // Pre-defined data entry point categories include: Registry, File, Network.
    //
    #define __inner_data_entrypoint(category)    __declspec("SAL_entrypoint(dataEntry, "SPECSTRINGIZE(category)")")

    #define __inner_success(expr)               __declspec("SAL_success("SPECSTRINGIZE(expr)")")
    #define __inner_typefix(ctype)              __declspec("SAL_typefix("SPECSTRINGIZE(ctype)")")
    #define __inner_override                    __declspec("__override")
    #define __inner_callback                    __declspec("__callback")
    #define __inner_blocksOn(resource)          __declspec("SAL_blocksOn("SPECSTRINGIZE(resource)")")
    #define __inner_fallthrough_dec             __nothrow void __FallThrough();
    #define __inner_fallthrough                 __FallThrough();
    #define __inner_assume_bound_dec            __inline __nothrow void __AssumeBoundInt(__inner_out_bound int i) {i;}
    #define __inner_assume_bound(i)             __AssumeBoundInt(i);
    #define __inner_allocator                   __declspec("SAL_allocator")

#if( __USE_ATTRIBUTES_FOR_SAL )
    #define __inner_checkReturn                 [returnvalue:SA_Post(MustCheck=SA_Yes)]
    #define __inner_format_string               [SA_FormatString]
    #define __inner_nullterminated
    #define __inner_nullnullterminated
#else
    #define __inner_checkReturn                 __declspec("SAL_checkReturn")
    #define __inner_format_string
    #define __inner_nullterminated              __declspec("SAL_readableTo(sentinel(0))")
    #define __inner_nullnullterminated          __inexpressible_writableTo("buffer terminated by consecutive NULL characters")
#endif

#else
    #define __inner_bound
    #define __inner_in_bound
    #define __inner_out_bound
    #define __inner_deref_out_bound
    #define __inner_range(lb,ub)
    #define __inner_in_range(lb,ub)       
    #define __inner_out_range(lb,ub)      
    #define __inner_deref_out_range(lb,ub)
    #define __inner_control_entrypoint(category)
    #define __inner_data_entrypoint(category)
    #define __inner_success(expr)
    #define __inner_typefix(ctype)
    #define __inner_override
    #define __inner_callback
    #define __inner_blocksOn(resource)
    #define __inner_fallthrough_dec
    #define __inner_fallthrough
    #define __inner_assume_bound_dec 
    #define __inner_assume_bound(i)     
    #define __inner_allocator
    #define __inner_checkReturn
    #define __inner_format_string
    #define __inner_nullterminated
    #define __inner_nullnullterminated
#endif // #if (_MSC_VER >= 1000) && !defined(MIDL_PASS) && defined(_PREFAST_)


#if( __USE_ATTRIBUTES_FOR_SAL )

#define __out_awcount(expr,size) __pre_notnull __post_valid
#define __in_awcount(expr,size)  __pre_valid

#else

//
// __precond(<expr>)
//
// When <expr> is true, the next annotation applies in the precondition state
// (currently not enabled)
//
#define __precond(expr)

#define __out_awcount(expr,size)            __precond(expr)    __pre_cap(size) \
                                            __precond(!(expr)) __pre_cap_x((size)*2) \
                                            __post_valid
#define __in_awcount(expr,size)             __pre_deref_readonly \
                                            __precond(expr)    __pre_count(size) \
                                            __precond(!(expr)) __pre_count_x((size)*2)
#endif // #if( __USE_ATTRIBUTES_FOR_SAL )

#define __success(expr)                     __inner_success(expr)
#ifndef __nullterminated
#define __nullterminated                    __inner_nullterminated
#endif
#define __nullnullterminated                __inner_nullnullterminated
#define __reserved                          __pre_null
#define __typefix(ctype)                    __inner_typefix(ctype)
#ifndef __override
#define __override                          __inner_override
#endif
#ifndef __callback
#define __callback                          __inner_callback
#endif
#define __format_string                     __inner_format_string
#define __blocksOn(resource)                __inner_blocksOn(resource)
#define __control_entrypoint(category)      __inner_control_entrypoint(category)
#define __data_entrypoint(category)         __inner_data_entrypoint(category)
#define __allocator                         __inner_allocator
#define __bound                             __inner_bound
#define __in_bound                          __inner_in_bound
#define __out_bound                         __inner_out_bound
#define __deref_out_bound                   __inner_deref_out_bound
#define __range(lb,ub)                      __inner_range(lb,ub)
#define __in_range(lb,ub)                   __inner_in_range(lb,ub)
#define __out_range(lb,ub)                  __inner_out_range(lb,ub)
#define __deref_out_range(lb,ub)            __inner_deref_out_range(lb,ub)

#ifndef __fallthrough
    __inner_fallthrough_dec
    #define __fallthrough __inner_fallthrough
#endif

#ifndef __assume_bound
    __inner_assume_bound_dec
    #define __assume_bound(i) __inner_assume_bound(i)
#endif

// Temporary annotations until Windows headers have been cleaned up
// Those annotations are deprecated and should not be used.
// Using those annotations might lead to wrong analysis results.
// Only annotations from sal.h should be used

#ifndef __null
#define __null             __pre_null
#endif

#if( !__USE_ATTRIBUTES_FOR_SAL )

#ifndef __maybenull
#define __maybenull        __pre_maybenull
#endif

#ifndef __notnull
#define __notnull          __pre_notnull
#endif

#ifndef __writableTo
#define __writableTo(size) __pre_cap(size)
#endif

#endif

#if defined( _PREFAST_ ) && __USE_ATTRIBUTES_FOR_SAL

#define __post             
#define __opt              __pre_maybenull

#elif defined(_PREFAST_) && !__USE_ATTRIBUTES_FOR_SAL

#define __opt  __exceptthat __maybenull

#else

#define __post
#define __opt

#endif

// OFFICEDEV: END

#ifdef  __cplusplus
}
#endif

