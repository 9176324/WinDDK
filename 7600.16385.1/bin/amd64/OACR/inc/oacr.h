#pragma once
/*****************************************************************************

   Module  : OACR
   Owner   : HannesR

******************************************************************************

   Definitions and #defines for OACR support

*****************************************************************************/

// SAL annotations are #defined in this file
#include <sal.h>
#include <specstrings.h>
//#include "oacrsal.h"
#include "pftwarnings.h"

#if( _PREFAST_ )
#if( _MSC_VER < 1400 )
#define _PREFAST2_ 1
#else
#define _PREFAST8_ 1
#endif
#endif

#ifndef ARM
#define __oacr_noop __noop
#else
#define __oacr_noop
#endif

#if defined(__cplusplus)
#define __extern_c     extern "C"
#define __extern_cplus extern "C++"
#else
#define __extern_c
#define __extern_cplus
#endif

#define __noreturn __declspec( noreturn )

// size_t is used in some OACR macros
#if !defined(_W64)
#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
#define _W64 __w64
#else
#define _W64
#endif
#endif

#ifndef _SIZE_T_DEFINED
#ifdef  _WIN64
typedef unsigned __int64    size_t;
#else
typedef _W64 unsigned int   size_t;
#endif
#define _SIZE_T_DEFINED
#endif

// OACR keywords:
// annotations undertstood by PREfast plugins

#ifndef _Sealed_method_
// use _Sealed_method_ to specify C# style 'sealed' behavior for methods
#define _Sealed_method_ __oacr_sealed_method
#endif // _Sealed_method_


#ifndef _Sealed_class_
// use _Sealed_class_ to specify C# style 'sealed' behavior for classes
#define _Sealed_class_ __oacr_sealed_class
#endif // _Sealed_class_


#ifndef _SA_deprecated_
// use _SA_deprecated_ to mark functions that should not be called any more
// pass the replacement function as argument to the macro
#define _SA_deprecated_(NewFunctionName) __oacr_sa_deprecated(NewFunctionName)
#endif // _SA_deprecated_


#ifndef _Intl_deprecated_
// annotation used by warning UNMARKED_INTL_DEPRECATED_FUNCTION (25114)
// to mark deprecated functions using LCID parameters
// The annotation is used by Office to cleanup the use of LCIDs
#define _Intl_deprecated_ __oacr_intl_deprecated
#endif // _Intl_deprecated_


#ifndef _Oleo_deprecated_
// Office specifix annotation used by the OLEO effort
#define _Oleo_deprecated_( NewFunctionName ) __oacr_oleo_deprecated(NewFunctionName)
#endif // _Oleo_deprecated_


#ifndef _Rpc_
// use _Rpc_ for functions that used as remote procedure calls, the keyword will silence
// various OACR checks on formal parameters of the marked functions
#define _Rpc_ __oacr_rpc
#endif // _Rpc_


#ifndef _RequireNoThrow_
// use _RequireNoThrow_ inside a compound statement to indicate that the rest of the block
// should not emit exceptions.
#define _RequireNoThrow_ __oacr_requireNoThrow
#endif // _RequireNoThrow_


#ifndef _Canthrow_
// use _Canthrow_ for method declarations of template classes that have throwing and non-throwing
// specializations to suppress warning FUNC_COULD_BE_NOTHROW (25307).
#define _Canthrow_ __oacr_canthrow
#endif // __requireNoThrow


#ifndef _Genericfunctype_
// use _Genericfunctype_ for function typedefs used for arrays of functions of different function types.
// if the typedef is marked as _Genericfunctype_, OACR will not generate DIFFERENT_CALLING_CONVENTION (25018) warnings
// e.g. typedef _Genericfunctype_ void (*FUNCPTR)();
#if( OACR )
#define _Genericfunctype_ __oacr_genericfunctype
#else
#define _Genericfunctype_
#endif
#endif // _Genericfunctype_


#ifndef _Nothrowfunctype_
// use _Nothrowfunctype_ for function typedefs of non exception throwing function pointers
// e.g. typedef _Nothrowfunctype_ void (*FUNCPTR)();
// only assign non throwing functions to typedefs so marked
#define _Nothrowfunctype_ __oacr_nothrowfunctype
#endif // _Nothrowfunctype_


#ifndef _BindReturn_
// stronger than __checkReturn, typically used by functions that return
// a pointer to an allocated object
// e.g _BindReturn_ void* malloc( size_t size );
// void Bar()
// {
//    void* pv;
//    if( ( pv == malloc( 20 ) ) == NULL )      <<<< typo !!
//    ...
// }
#define _BindReturn_ __oacr_bindReturn
#endif // _BindReturn_


#ifndef _Memberinitializer_
// use _Memberinitializer_ for init functions that initialize all members of a class
// e.g.:
// class X
// {
//    int m_i:
//    int m_j:
//    _Memberinitializer_ void Init(){ m_i = m_j = 0; }
// public:
//    X(){ Init(); }
// };
#define _Memberinitializer_ __oacr_memberinitializer
#endif // _Memberinitializer_


#ifndef _Noheap_
// use _Noheap_ classes that should not be instantiated on the heap
// e.g.:
// _Noheap_ class CriticalSection
// {
// public:
//    CriticalSection();
//    ~CriticalSection();
// };
#define _Noheap_ __oacr_noheap
#endif // _Noheap_


#ifndef _Unsafe_string_api_
// use _Unsafe_string_api_ to phase out functions that pass unbound writable buffers
// e.g.
// _Unsafe_string_api_ void MyStrCpy( char* szTo, const char* szFrom );
#define _Unsafe_string_api_ __oacr_unsafe_string_api
#endif // _Unsafe_string_api_


#ifndef _Needsreview_
// use _Needsreview_ to mark functions whose calls need to be reviewed for a
// special reason.
// e.g. a wrapper function to another function that needs to be reviewed
//_Needsreview_ __inline BOOL MsoGetStringTypeExW(LCID Locale, DWORD dwInfoType, LPCWSTR lpSrcStr, int cchSrc, LPWORD lpCharType)
//{
//   return OACR_REVIEWED_CALL("hannesr", GetStringTypeExW(Locale, dwInfoType, lpSrcStr, cchSrc, lpCharType));
//}
// use the OACR_REVIEWED_CALL macro to silent the warning after making sure that the function is used properly
#define _Needsreview_ __oacr_needsreview
#endif // _Needsreview_


#ifndef _Notrunccast_
// use _Notrunccast_ to find unsafe truncating cast on allocating functions taking
// a 16 bit size parameter
// e.g.
// void* MyMalloc( _Notrunccast_ ushort size );
#define _Notrunccast_ __oacr_notrunccast
#endif // _Notrunccast_


#ifndef _Noinference_
// keyword to disable (wrong) Hungarian inference of __count annotations
// inference is disabled for the all formals of the function
// e.g. _Noinference_ void FreePv( void* pv, size_t cb );
#define _Noinference_     __oacr_noinference
#endif // _Noinference_


// deprecated pre ORCAS versions
#ifndef __sealed_method
#define __sealed_method                      _Sealed_method_
#endif
#ifndef __sealed_class
#define __sealed_class                       _Sealed_class_
#endif
#ifndef __sa_deprecated
#define __sa_deprecated(NewFunctionName)     _SA_deprecated_(NewFunctionName)
#endif
#ifndef __intl_deprecated
#define __intl_deprecated                    _Intl_deprecated_
#endif
#ifndef __oleo_deprecated
#define __oleo_deprecated( NewFunctionName ) _Oleo_deprecated_(NewFunctionName)
#endif
#ifndef __rpc
#define __rpc                                _Rpc_
#endif
#ifndef __requireNoThrow
#define __requireNoThrow                     _RequireNoThrow_
#endif
#ifndef __canthrow
#define __canthrow                           _Canthrow_
#endif
#ifndef __genericfunctype
#define __genericfunctype                    _Genericfunctype_
#endif
#ifndef __nothrowfunctype
#define __nothrowfunctype                    _Nothrowfunctype_
#endif
#ifndef __bindReturn
#define __bindReturn                         _BindReturn_
#endif
#ifndef __memberinitializer
#define __memberinitializer                  _Memberinitializer_
#endif
#ifndef __noheap
#define __noheap                             _Noheap_
#endif
#ifndef __nostack
#define __nostack                            // not supported
#endif
#ifndef __unsafe_string_api
#define __unsafe_string_api                  _Unsafe_string_api_
#endif
#ifndef __needsreview
#define __needsreview                        _Needsreview_
#endif
#ifndef __notrunccast
#define __notrunccast                        _Notrunccast_
#endif
#ifndef __noinference
#define __noinference                        _Noinference_
#endif
#ifndef __min_function
#define __min_function                       // deprecated, use range annotations
#endif
#ifndef __max_function
#define __max_function                       // deprecated, use range annotations
#endif
#ifndef __printf_format_string
#define __printf_format_string               _Printf_format_string_
#endif

#undef __callback
#define __callback                           _Callback_

#ifndef __sa_sealed // use __sealed_method and __sealed_class instead
#if( OACR )
#define __sa_sealed __declspec("_Sealed_")
#else
#define __sa_sealed
#endif
#endif // __sa_sealed

//-----------------

// OACR helper macros to suppress particular warnings

#ifndef OACR_USE_PTR
// use to suppress constness and related warnings:
// NONCONST_LOCAL (25003), NONCONST_PARAM( 25004), NONCONST_FUNCTION (25005), 
// NONCONST_LOCAL_BUFFERPTR (25032), NONCONST_BUFFER_PARAM (25033)
#if( OACR )
__extern_c void OACRUsePtr( void* p );
#define OACR_USE_PTR( p ) OACRUsePtr( p )
#else
#define OACR_USE_PTR( p ) __oacr_noop
#endif
#endif // OACR_USE_PTR

#ifndef OACR_MEMBER_IS_INITIALIZED_IN_CTOR
// Use to suppress warning UNINITIALIZED_DATAMEMBER (25070) (as issued from a _Memberinitializer_)
// for data members which are correctly initialized in the ctor.
// (Warning 25070 cannot automatically handle member initialization that
// is spread across both ctor initializer list and a _Memberinitializer_ method).
// NOTES: This is functionally to OACR_USE_PTR(&m).
//        Contrast with OACR_MEMBER_IS_INITIALIZED_IN_MEMBERINITIALIZER.
#define OACR_MEMBER_IS_INITIALIZED_IN_CTOR( m ) OACR_USE_PTR( &m )
#endif // OACR_MEMBER_IS_INITIALIZED_IN_CTOR

#ifndef OACR_MEMBER_IS_INITIALIZED_IN_MEMBERINITIALIZER
// Use to suppress warning UNINITIALIZED_DATAMEMBER (25070) (as issued from a ctor)
// for data members which are correctly initialized in the _Memberinitializer_ method.
// (Warning 25070 cannot automatically handle member initialization that
// is spread across both ctor initializer list and a _Memberinitializer_ method).
// NOTES: This is functionally equivalent to OACR_USE_PTR(&m).
//        Contrast with OACR_MEMBER_IS_INITIALIZED_IN_CTOR.
#define OACR_MEMBER_IS_INITIALIZED_IN_MEMBERINITIALIZER( m ) OACR_USE_PTR( &m )
#endif // OACR_MEMBER_IS_INITIALIZED_IN_MEMBERINITIALIZER

#ifndef OACR_POSSIBLE_THROW
// use to suppress __nothrow related warnings NOTHROW_FUNC_THROWS (25306) and FUNC_COULD_BE_NOTHROW (25307)
#if( OACR )
__extern_cplus void OACRPossibleThrow();
#define OACR_POSSIBLE_THROW OACRPossibleThrow()
#else
#define OACR_POSSIBLE_THROW __oacr_noop
#endif
#endif // OACR_POSSIBLE_THROW

#ifndef OACR_ASSUME_NOTHROW_BEGIN
// use with OACR_ASSUME_NOTHROW_END to suppress NOTHROW_FUNC_THROWS warnings around functions that don't throw in this context
// macro pair needs to be on the same scope
#if( OACR )
#define OACR_ASSUME_NOTHROW_BEGIN try{
#define OACR_ASSUME_NOTHROW_END }catch(...){}
#else
#define OACR_ASSUME_NOTHROW_BEGIN
#define OACR_ASSUME_NOTHROW_END
#endif
#endif // OACR_POSSIBLE_THROW

#ifndef UNREFERENCED_OACR
// use to function staticness and related warnings: STATIC_FUNCTION (25007)
#if( OACR )
#define UNREFERENCED_OACR( p ) __assume( p == p )
#else
#define UNREFERENCED_OACR( p )
#endif
#endif // UNREFERENCED_OACR


#ifndef OACR_OWN_PTR
// can be used for objects that attach themselves to an owner
// in their constructors
#if( OACR )
__extern_c void OACROwnPtr( const void* p );
#define OACR_OWN_PTR( p ) OACROwnPtr( p )
#else
#define OACR_OWN_PTR( p ) __oacr_noop
#endif
#endif // OACR_OWN_PTR


#ifndef OACR_PTR_NOT_NULL
// tells OACR that a pointer is not null at this point
#if( OACR )
#define OACR_PTR_NOT_NULL( p ) OACR_ASSUME( 0 != p )
#else
#define OACR_PTR_NOT_NULL( p ) __oacr_noop
#endif
#endif // OACR_PTR_NOT_NULL


#ifndef OACR_NOT_IMPLEMENTED_MEMBER
#if( OACR )
#define OACR_NOT_IMPLEMENTED_MEMBER OACR_USE_PTR( (void*)this )
#else
#define OACR_NOT_IMPLEMENTED_MEMBER
#endif
#endif // OACR_NOT_IMPLEMENTED_MEMBER


#ifndef OACR_DECLARE_FILLER
#if( OACR )
#define OACR_DECLARE_FILLER( type, inst ) type __filler##inst;
#else
#define OACR_DECLARE_FILLER( type, inst )
#endif
#endif // OACR_DECLARE_FILLER


// use this macro once you have inspected warnings FUNCTION_NEEDS_REVIEW (25028)
#ifndef OACR_REVIEWED_CALL
#if( OACR )
__extern_c void __OACRReviewedCall();
#define OACR_REVIEWED_CALL( reviewer, functionCall ) ( __OACRReviewedCall(), functionCall )
#else
#define OACR_REVIEWED_CALL( reviewer, functionCall ) functionCall
#endif
#endif // OACR_REVIEWED_CALL


// use this macro once you have inspected warnings URL_NEEDS_TO_BE_REVIEWED (25085)
#ifndef OACR_REVIEWED_URL
#if( OACR )
__extern_c void __OACRReviewedUrl();
#define OACR_REVIEWED_URL( reviewer, reviewedUrl ) ( __OACRReviewedUrl(), reviewedUrl )
#else
#define OACR_REVIEWED_URL( reviewer, reviewedUrl ) reviewedUrl
#endif
#endif // OACR_REVIEWED_URL

#if( OACR && defined(_WINDEF_) && 0 )

// redefine FALSE & TRUE for better HRESULT<->BOOL conversion detection
#ifdef FALSE
#undef FALSE
#define FALSE ((BOOL)0)
#endif

#ifdef TRUE
#undef TRUE
#define TRUE ((BOOL)1)
#endif

#endif

// Use the following macros to suppress and disable OACR warnings in the code
// Using the macros allows us to have the same source code being compiled with
// PREfast 2.x and Whidbey PREfast

// macro to tell OACR to not issue a specific warning for the following line of code
// use to suppress false positives from OACR
// e.g.
// if( fPointerNotNull )
//    OACR_WARNING_SUPRESS( DEREF_NULL_PTR, "pointer access is guarded by 'fPointerNotNull'" )
//    p->Foo();

#ifndef OACR_WARNING_PUSH
#if( OACR && !defined(OACR_NO_WARNING_MACROS) )
#if( 1400 <=_MSC_VER )
#define OACR_WARNING_PUSH __pragma ( warning( push ) )
#else
#define OACR_WARNING_PUSH __pragma ( prefast( push ) )
#endif
#else
#define OACR_WARNING_PUSH
#endif
#endif

#ifndef OACR_WARNING_POP
#if( OACR && !defined(OACR_NO_WARNING_MACROS) )
#if( 1400 <=_MSC_VER )
#define OACR_WARNING_POP __pragma ( warning( pop ) )
#else
#define OACR_WARNING_POP __pragma ( prefast( pop ) )
#endif
#else
#define OACR_WARNING_POP
#endif
#endif

#ifndef OACR_WARNING_ENABLE
#if( OACR && !defined(OACR_NO_WARNING_MACROS) )
#if( 1400 <=_MSC_VER )
#define OACR_WARNING_ENABLE( cWarning, comment ) __pragma ( warning( enable: __WARNING_##cWarning ) )
#else
#define OACR_WARNING_ENABLE( cWarning, comment )
#endif
#else
#define OACR_WARNING_ENABLE( cWarning, comment )
#endif
#endif

#ifndef OACR_WARNING_DISABLE
#if( OACR && !defined(OACR_NO_WARNING_MACROS) )
#if( 1400 <=_MSC_VER )
#define OACR_WARNING_DISABLE( cWarning, comment ) __pragma ( warning( disable: __WARNING_##cWarning ) )
#else
#define OACR_WARNING_DISABLE( cWarning, comment ) __pragma ( prefast( disable: __WARNING_##cWarning, comment ) )
#endif
#else
#define OACR_WARNING_DISABLE( cWarning, comment )
#endif
#endif

#ifndef OACR_WARNING_SUPPRESS
#if( OACR && !defined(OACR_NO_WARNING_MACROS) )
#if( 1400 <=_MSC_VER )
#define OACR_WARNING_SUPPRESS( cWarning, comment ) __pragma ( warning( suppress: __WARNING_##cWarning ) )
#else
#define OACR_WARNING_SUPPRESS( cWarning, comment ) __pragma ( prefast( suppress: __WARNING_##cWarning, comment) )
#endif
#else
#define OACR_WARNING_SUPPRESS( cWarning, comment )
#endif
#endif

// macro to tell OACR about conditions that are true.
// Use OACR_ASSUME instead of #pragma prefast(suppress,...) if possible
// e.g.:
// if( g_pRoot == NULL ) // global variable g_pRoot
// {
//    InitRoot();
//    OACR_ASSUME( NULL != g_pRoot ); 
// }
// g_pRoot->Traverse();    // without OACR_ASSUME this would cause warning 11
#if( OACR )
#define OACR_ASSUME( fCondition ) __assume( fCondition )

#elif defined(Assert)
#define OACR_ASSUME( fCondition ) Assert( fCondition )

#else
#define OACR_ASSUME( fCondition ) __oacr_noop

#endif

// macro to tell OACR that a string is null terminated at this point of execution
#if( OACR && defined( __post_z ) )
__extern_c void __OACRAssumeNullterminated( __post_z const char* sz );
#define OACR_ASSUME_NULLTERMINATED( string ) __OACRAssumeNullterminated( (const char*)string )

#else
#define OACR_ASSUME_NULLTERMINATED( string ) __oacr_noop

#endif

// macro to tell OACR that a pointer is null valid at this point of execution
#if( OACR && defined( __post_valid ) )
__extern_c void __OACRAssumeValid( __post_valid const void* pv );
#define OACR_ASSUME_VALID( ptr ) __OACRAssumeValid( ptr )

#else
#define OACR_ASSUME_VALID( ptr ) __oacr_noop

#endif


// macro to tell OACR that a buffer has a certain readable extent at this point of execution
// it can be used to silent noisy espX INCORRECT_ANNOTATION warnings
#if( OACR && defined( __post_bytecount ) )
__extern_c void __OACRAssumeByteCount( __post_bytecount(cb) const void* pv, size_t cb );
#define OACR_ASSUME_BYTECOUNT( pv, cb ) __OACRAssumeByteCount( pv, cb )

#else
#define OACR_ASSUME_BYTECOUNT( pv, cb ) __oacr_noop

#endif


// new intrinsinc functions in Whidbey compiler
#if( _PREFAST2_ )
__extern_c void* _AddressOfReturnAddress();
#endif

// OACR custom plugin specific extensions

//=======================================================================

#define _Callback_                         __oacr_callback

#undef __override
#define __override                         __oacr_override

//======================================================================
// OACR custom attributes

// OACR custom plugin specific extensions

#if( _USE_ATTRIBUTES_FOR_SAL )

#pragma push_macro( "SA" )
#pragma push_macro( "REPEATABLE" )

#ifdef __cplusplus
#define SA( id ) id
#define REPEATABLE [repeatable]
#else  // !__cplusplus
#define SA( id ) SA_##id
#define REPEATABLE
#endif  // !__cplusplus

enum OACRFunctionFlag
{
   eOACRCallback          = 0x0001,
   eOACRRpc               = 0x0002,
   eOACROverride          = 0x0004,
   eOACRSealed            = 0x0008,
   eOACRMemberInitializer = 0x0010,
   eOACRUnsafeStringApi   = 0x0020,
   eOACRNeedsReview       = 0x0040,
   eOACRIntlDeprecated    = 0x0080,
   eOACRNoInferrence      = 0x0100,
   //eOACRMinFunction     = 0x0200,
   //eOACRMaxFunction     = 0x0400,
   //eOACRCompilerGen     = 0x0800,
   eOACRCanThrow          = 0x1000,
   eOACRBindReturn        = 0x2000,
};

REPEATABLE
[source_annotation_attribute( SA( Method ) )]
struct OACRFunctionFlagsAttribute
{
#ifdef __cplusplus
	OACRFunctionFlagsAttribute();
#endif
	int Flags;
};

enum OACRFunctionProp
{
   eOACRDeprecated     = 0,
   eOACROleoDeprecated = 1,
};

typedef enum OACRFunctionProp OACRFunctionProp;

REPEATABLE
[source_annotation_attribute( SA( Method ) )]
struct OACRFunctionPropAttribute
{
#ifdef __cplusplus
	OACRFunctionPropAttribute();
#endif
	OACRFunctionProp Type;
   const char*      SzVal;
};

#pragma pop_macro( "REPEATABLE" )
#pragma pop_macro( "SA" )

typedef struct OACRFunctionFlagsAttribute SA_OACRFunctionFlags;
typedef struct OACRFunctionPropAttribute  SA_OACRFunctionProp;

//--------------------------------------------------

// use __declspecs until Whidbey-PREfast bug VS:479840 is fixed
#define __oacr_callback                         __declspec("_Callback_")
#define __oacr_override                         __declspec("_Override_")
//#define __oacr_callback          [method:SA_OACRFunctionFlags(Flags=eOACRCallback         )]
//#define __oacr_override          [method:SA_OACRFunctionFlags(Flags=eOACROverride         )]
#define __oacr_rpc               [method:SA_OACRFunctionFlags(Flags=eOACRRpc)]
#define __oacr_sealed_method     [method:SA_OACRFunctionFlags(Flags=eOACRSealed)]
#define __oacr_memberinitializer [method:SA_OACRFunctionFlags(Flags=eOACRMemberInitializer)]
#define __oacr_unsafe_string_api [method:SA_OACRFunctionFlags(Flags=eOACRUnsafeStringApi)]
#define __oacr_needsreview       [method:SA_OACRFunctionFlags(Flags=eOACRNeedsReview)]
#define __oacr_intl_deprecated   [method:SA_OACRFunctionFlags(Flags=eOACRIntlDeprecated)]
#define __oacr_noinference       [method:SA_OACRFunctionFlags(Flags=eOACRNoInferrence)]
#define __oacr_canthrow          [method:SA_OACRFunctionFlags(Flags=eOACRCanThrow)]
#define __oacr_bindReturn        [method:SA_OACRFunctionFlags(Flags=eOACRBindReturn)]

#define __oacr_sa_deprecated( NewFunctionName )   [method:SA_OACRFunctionProp(Type=eOACRDeprecated,    SzVal=#NewFunctionName)]
#define __oacr_oleo_deprecated( NewFunctionName ) [method:SA_OACRFunctionProp(Type=eOACROleoDeprecated,SzVal=#NewFunctionName)]

#define __oacr_genericfunctype  __declspec("_Genericfunctype_")
#define __oacr_nothrowfunctype  __declspec("_Nothrowfunctype_")
#define __oacr_noheap           __declspec("_Noheap_" )
#define __oacr_notrunccast      __declspec("_Notrunccast_" )
#define __oacr_sealed_class     __declspec("_Sealed_")
__extern_c int __RequireNoThrow();
#define __oacr_requireNoThrow __RequireNoThrow();

#elif( _USE_DECLSPECS_FOR_SAL )

#define __oacr_callback                         __declspec("_Callback_")
#define __oacr_override                         __declspec("_Override_")
#define __oacr_rpc                              __declspec("_Rpc_")
#define __oacr_sealed_method                    __declspec("_Sealed_")
#define __oacr_memberinitializer                __declspec("_Memberinitializer_")
#define __oacr_unsafe_string_api                __declspec("_Unsafe_string_api_")
#define __oacr_needsreview                      __declspec("_Needsreview_")
#define __oacr_intl_deprecated                  __declspec("_Intl_deprecated_")
#define __oacr_noinference                      __declspec("_Noinference_")
#define __oacr_canthrow                         __declspec("_Canthrow_")
#define __oacr_bindReturn                       __declspec("_BindReturn_")
#define __oacr_sa_deprecated(NewFunctionName)   __declspec("_SA_deprecated_("SPECSTRINGIZE(NewFunctionName)")")
#define __oacr_oleo_deprecated(NewFunctionName) __declspec("_Oleo_deprecated_("SPECSTRINGIZE(NewFunctionName)")")
#define __oacr_genericfunctype                  __declspec("_Genericfunctype_")
#define __oacr_nothrowfunctype                  __declspec("__nothrowfunctype") // TODO: change to _Nothrowfunctype_
#define __oacr_noheap                           __declspec("_Noheap_")
#define __oacr_notrunccast                      __declspec("_Notrunccast_")
#define __oacr_sealed_class                     __declspec("_Sealed_")
__extern_c int __RequireNoThrow();
#define __oacr_requireNoThrow __RequireNoThrow();

#else // neither [attributes] not __declspecs

#define __oacr_callback
#define __oacr_override
#define __oacr_rpc
#define __oacr_sealed_method
#define __oacr_memberinitializer
#define __oacr_unsafe_string_api
#define __oacr_needsreview
#define __oacr_intl_deprecated
#define __oacr_noinference
#define __oacr_canthrow
#define __oacr_bindReturn
#define __oacr_sa_deprecated(NewFunctionName)
#define __oacr_oleo_deprecated(NewFunctionName)
#define __oacr_genericfunctype
#define __oacr_nothrowfunctype
#define __oacr_noheap
#define __oacr_notrunccast
#define __oacr_sealed_class
#define __oacr_requireNoThrow

#endif

//#include "offoacr.h"
