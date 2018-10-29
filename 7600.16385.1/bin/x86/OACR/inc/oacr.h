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

#ifndef __sealed_method
// use __sealed_method to specify C# style 'sealed' behavior for methods
#define __sealed_method __oacr_sealed_method
#endif // __sealed_method

#ifndef __sealed_class
// use __sealed_class to specify C# style 'sealed' behavior for classes
#define __sealed_class __oacr_sealed_class
#endif // __sealed_class

#ifndef __sa_sealed
// deprecated for Whidbey-PREfast; use __sealed_method and __sealed_class instead
#if( OACR )
#define __sa_sealed __declspec("__sa_sealed")
#else
#define __sa_sealed
#endif
#endif // __sa_sealed


#ifndef __sa_deprecated
// use __sa_deprecated to mark functions that should not be called any more
// pass the replacement function as argument to the macro
#define __sa_deprecated(NewFunctionName) __oacr_sa_deprecated(NewFunctionName)
#endif // __sa_deprecated


#ifndef __intl_deprecated
// annotation used by warning UNMARKED_INTL_DEPRECATED_FUNCTION
// to mark deprecated functions using LCID parameters
// The annotation is used by Office to cleanup the use of LCIDs
#define __intl_deprecated __oacr_intl_deprecated
#endif // __intl_deprecated


#ifndef __oleo_deprecated
// Office specifix annotation used by the OLEO effort
#define __oleo_deprecated( NewFunctionName ) __oacr_oleo_deprecated(NewFunctionName)
#endif // __oleo_deprecated


#ifndef __rpc
// use __rpc for functions that used as remote procedure calls, the keyword will silence
// various OACR checks on formal parameters of the marked functions
#define __rpc __oacr_rpc
#endif // __rpc


#ifndef __requireNoThrow
// use __requireNoThrow inside a compound statement to indicate that the rest of the block
// should not emit exceptions.
#define __requireNoThrow __oacr_requireNoThrow
#endif // __requireNoThrow


#ifndef __canthrow
// use __canthrow for method declarations of template classes that have throwing and non-throwing
// specializations to suppress warning FUNC_COULD_BE_NOTHROW.
#define __canthrow __oacr_canthrow
#endif // __requireNoThrow


#ifndef __genericfunctype
// use __genericfunctype for function typedefs used for arrays of functions of different function types.
// if the typedef is marked as __genericfunctype, OACR will not generate DANGEROUS_FUNCTIONCAST (5418) warnings
// e.g. typedef __genericfunctype void (*FUNCPTR)();
#if( OACR )
#define __genericfunctype __declspec("__genericfunctype")
#else
#define __genericfunctype
#endif
#endif // __genericfunctype


#ifndef __nothrowfunctype
// use __nothrowfunctype for function typedefs of non exception throwing function pointers
// e.g. typedef __nothrowfunctype void (*FUNCPTR)();
// only assign non throwing functions to typedefs so marked
#if( OACR )
#define __nothrowfunctype __declspec("__nothrowfunctype")
#else
#define __nothrowfunctype
#endif
#endif // __nothrowfunctype


#ifndef __bindReturn
// stronger than __checkReturn, typically used by functions that return
// a pointer to an allocated object
// e.g __bindReturn void* malloc( size_t size );
// void Bar()
// {
//    void* pv;
//    if( ( pv == malloc( 20 ) ) == NULL )      <<<< typo !!
//    ...
// }
#define __bindReturn __oacr_bindReturn
#endif // __bindReturn


#ifndef __memberinitializer
// use __memberinitializer for init functions that initialize all members of a class
// e.g.:
// class X
// {
//    int m_i:
//    int m_j:
//    __memberinitializer void Init(){ m_i = m_j = 0; }
// public:
//    X(){ Init(); }
// };
#define __memberinitializer __oacr_memberinitializer
#endif // __memberinitializer


#ifndef __noheap
// use __noheap classes that should not be instantiated on the heap
// e.g.:
// __noheap class CriticalSection
// {
// public:
//    CriticalSection();
//    ~CriticalSection();
// };
#define __noheap __oacr_noheap
#endif // __noheap


#ifndef __nostack
// use __noheap classes that should not be instantiated on the stack
// e.g.:
// __nostack class CIFoo
// {
//    ...
// };
#if( OACR )
#define __nostack __declspec("__nostack")
#else
#define __nostack
#endif
#endif // __nostack


#ifndef __unsafe_string_api
// use __unsafe_string_api to phase out functions that pass unbound writable buffers
// e.g.
// __unsafe_string_api void MyStrCpy( char* szTo, const char* szFrom );
#define __unsafe_string_api __oacr_unsafe_string_api
#endif // __unsafe_string_api


#ifndef __needsreview
// use __needsreview to mark functions whose calls need to be reviewed for a
// special reason.
// e.g. a wrapper function to another function that needs to be reviewed
//__inline __needsreview BOOL MsoGetStringTypeExW(LCID Locale, DWORD dwInfoType, LPCWSTR lpSrcStr, int cchSrc, LPWORD lpCharType)
//{
//   return OACR_REVIEWED_CALL("hannesr", GetStringTypeExW(Locale, dwInfoType, lpSrcStr, cchSrc, lpCharType));
//}
// use the OACR_REVIEWED_CALL macro to silent the warning after making sure that the function is used properly
#define __needsreview __oacr_needsreview
#endif // __needsreview


#ifndef __notrunccast
// use __notrunccast to find unsafe truncating cast on allocating functions taking
// a 16 bit size parameter
// e.g.
// void* MyMalloc( __notrunccast ushort size );
#define __notrunccast __oacr_notrunccast
#endif // __notrunccast


#ifndef __min_function
// use __min_function to annotate functions that compute the minimum of two numbers and follow the example's signature
// e.g.
// __min_function int Min( int iLeft, int iRight );
#define __min_function __oacr_min_function
#endif // __min_function


#ifndef __max_function
// use __max_function to annotate functions that compute the maximum of two numbers and follow the example's signature
// e.g.
// __max_function float Max( float iLeft, float iRight );
#define __max_function __oacr_max_function
#endif // __max_function


// OACR keywords for buffer sizes

#ifndef __noinference
// keyword to disable (wrong) Hungarian inference of __count annotations
// inference is disabled for the all formals of the function
// e.g. __noinference void FreePv( void* pv, size_t cb );
#define __noinference     __oacr_noinference
#endif // __noinference

// OACR helper macros to suppress particular warnings

#ifndef OACR_USE_PTR
// use to suppress constness and related warnings (5403, 5404, 5405 5432, 5433)
#if( OACR )
__extern_c void OACRUsePtr( void* p );
#define OACR_USE_PTR( p ) OACRUsePtr( p )
#else
#define OACR_USE_PTR( p ) __noop
#endif
#endif // OACR_USE_PTR

#ifndef OACR_MEMBER_IS_INITIALIZED_IN_CTOR
// Use to suppress warning 5470 (as issued from a __memberinitializer)
// for data members which are correctly initialized in the ctor.
// (Warning 5470 cannot automatically handle member initialization that
// is spread across both ctor initializer list and a __memberinitializer method).
// NOTES: This is functionally to OACR_USE_PTR(&m).
//        Contrast with OACR_MEMBER_IS_INITIALIZED_IN_MEMBERINITIALIZER.
#define OACR_MEMBER_IS_INITIALIZED_IN_CTOR( m ) OACR_USE_PTR( &m )
#endif // OACR_MEMBER_IS_INITIALIZED_IN_CTOR

#ifndef OACR_MEMBER_IS_INITIALIZED_IN_MEMBERINITIALIZER
// Use to suppress warning 5470 (as issued from a ctor)
// for data members which are correctly initialized in the __memberinitializer method.
// (Warning 5470 cannot automatically handle member initialization that
// is spread across both ctor initializer list and a __memberinitializer method).
// NOTES: This is functionally equivalent to OACR_USE_PTR(&m).
//        Contrast with OACR_MEMBER_IS_INITIALIZED_IN_CTOR.
#define OACR_MEMBER_IS_INITIALIZED_IN_MEMBERINITIALIZER( m ) OACR_USE_PTR( &m )
#endif // OACR_MEMBER_IS_INITIALIZED_IN_MEMBERINITIALIZER

#ifndef OACR_POSSIBLE_THROW
// use to suppress __nothrow related warnings 5706 and 5707
#if( OACR )
__extern_cplus void OACRPossibleThrow();
#define OACR_POSSIBLE_THROW OACRPossibleThrow()
#else
#define OACR_POSSIBLE_THROW __noop
#endif
#endif // OACR_POSSIBLE_THROW

#ifndef UNREFERENCED_OACR
// use to function staticness and related warnings (5407)
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
#define OACR_OWN_PTR( p ) __noop
#endif
#endif // OACR_OWN_PTR


#ifndef OACR_PTR_NOT_NULL
// tells OACR that a pointer is not null at this point
#if( OACR )
#define OACR_PTR_NOT_NULL( p ) OACR_ASSUME( 0 != p )
#else
#define OACR_PTR_NOT_NULL( p ) __noop
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


// use this macro once you have inspected warnings WARNING_FUNCTION_NEEDS_REVIEW (5428)
#ifndef OACR_REVIEWED_CALL
#if( OACR )
__extern_c void __OACRReviewedCall();
#define OACR_REVIEWED_CALL( reviewer, functionCall ) ( __OACRReviewedCall(), functionCall )
#else
#define OACR_REVIEWED_CALL( reviewer, functionCall ) functionCall
#endif
#endif // OACR_REVIEWED_CALL


// use this macro once you have inspected warnings WARNING_URL_NEEDS_TO_BE_REVIEWED (5485)
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

#ifndef OACR_SUPPRESSWARNING
#define OACR_SUPPRESSWARNING( cWarning, comment ) OACR_WARNING_SUPPRESS( cWarning, comment )
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
#define OACR_ASSUME( fCondition ) __noop

#endif

// macro to tell OACR that a string is null terminated at this point of execution
#if( OACR && defined( __post_z ) )
__extern_c void __OACRAssumeNullterminated( __post_z const char* sz );
#define OACR_ASSUME_NULLTERMINATED( string ) __OACRAssumeNullterminated( (const char*)string )

#else
#define OACR_ASSUME_NULLTERMINATED( fCondition ) __noop

#endif


// macro to tell OACR that a buffer has a certain readable extent at this point of execution
// it can be used to silent noisy espX INCORRECT_ANNOTATION warnings
#if( OACR && defined( __post_bytecount ) )
__extern_c void __OACRAssumeByteCount( __post_bytecount(cb) const void* pv, size_t cb );
#define OACR_ASSUME_BYTECOUNT( pv, cb ) __OACRAssumeByteCount( pv, cb )

#else
#define OACR_ASSUME_BYTECOUNT( pv, cb ) __noop

#endif


// new intrinsinc functions in Whidbey compiler
#if( OACR )
#if( _MSC_VER < 1400 )
__extern_c void* _AddressOfReturnAddress();
#endif
#endif

// OACR custom plugin specific extensions

//=======================================================================

#undef __callback
#define __callback                         __oacr_callback
#undef __override
#define __override                         __oacr_override

//======================================================================
// OACR custom attributes

// OACR custom plugin specific extensions

#if( __USE_ATTRIBUTES_FOR_SAL )

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
   eOACRMinFunction       = 0x0200,
   eOACRMaxFunction       = 0x0400,
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
   eOACROleoBanned     = 2,
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
#define __oacr_callback                         __declspec("__callback")
#define __oacr_override                         __declspec("__override")
//#define __oacr_callback          [method:SA_OACRFunctionFlags(Flags=eOACRCallback         )]
//#define __oacr_override          [method:SA_OACRFunctionFlags(Flags=eOACROverride         )]
#define __oacr_rpc               [method:SA_OACRFunctionFlags(Flags=eOACRRpc)]
#define __oacr_sealed_method     [method:SA_OACRFunctionFlags(Flags=eOACRSealed)]
#define __oacr_memberinitializer [method:SA_OACRFunctionFlags(Flags=eOACRMemberInitializer)]
#define __oacr_unsafe_string_api [method:SA_OACRFunctionFlags(Flags=eOACRUnsafeStringApi)]
#define __oacr_needsreview       [method:SA_OACRFunctionFlags(Flags=eOACRNeedsReview)]
#define __oacr_intl_deprecated   [method:SA_OACRFunctionFlags(Flags=eOACRIntlDeprecated)]
#define __oacr_noinference       [method:SA_OACRFunctionFlags(Flags=eOACRNoInferrence)]
#define __oacr_min_function      [method:SA_OACRFunctionFlags(Flags=eOACRMinFunction)]
#define __oacr_max_function      [method:SA_OACRFunctionFlags(Flags=eOACRMaxFunction)]
#define __oacr_canthrow          [method:SA_OACRFunctionFlags(Flags=eOACRCanThrow)]
#define __oacr_bindReturn        [method:SA_OACRFunctionFlags(Flags=eOACRBindReturn)]

#define __oacr_sa_deprecated( NewFunctionName )   [method:SA_OACRFunctionProp(Type=eOACRDeprecated,    SzVal=#NewFunctionName)]
#define __oacr_oleo_deprecated( NewFunctionName ) [method:SA_OACRFunctionProp(Type=eOACROleoDeprecated,SzVal=#NewFunctionName)]
#define __oacr_oleo_banned( NewFunctionName )     [method:SA_OACRFunctionProp(Type=eOACROleoBanned,    SzVal=#NewFunctionName)]

#define __oacr_noheap       __declspec("__noheap" )
#define __oacr_notrunccast  __declspec("__notrunccast" )
#define __oacr_sealed_class __declspec("__sa_sealed")
__extern_c int __RequireNoThrow();
#define __oacr_requireNoThrow __RequireNoThrow();

#elif( __USE_DECLSPECS_FOR_SAL )

#define __oacr_callback                         __declspec("__callback")
#define __oacr_override                         __declspec("__override")
#define __oacr_rpc                              __declspec("__rpc")
#define __oacr_sealed_method                    __declspec("__sa_sealed")
#define __oacr_memberinitializer                __declspec("__memberinitializer")
#define __oacr_unsafe_string_api                __declspec("__unsafe_string_api")
#define __oacr_needsreview                      __declspec("__needsreview")
#define __oacr_intl_deprecated                  __declspec("__intl_deprecated")
#define __oacr_noinference                      __declspec("__noinference")
#define __oacr_min_function                     __declspec("__min_function")
#define __oacr_max_function                     __declspec("__max_function")
#define __oacr_canthrow                         __declspec("__canthrow")
#define __oacr_bindReturn                       __declspec("__bindReturn")
#define __oacr_sa_deprecated(NewFunctionName)   __declspec("__sa_deprecated("SPECSTRINGIZE(NewFunctionName)")")
#define __oacr_oleo_deprecated(NewFunctionName) __declspec("__oleo_deprecated("SPECSTRINGIZE(NewFunctionName)")")
#define __oacr_oleo_banned(NewFunctionName)     __declspec("__oleo_banned("SPECSTRINGIZE(NewFunctionName)")")
#define __oacr_noheap                           __declspec("__noheap")
#define __oacr_notrunccast                      __declspec("__notrunccast")
#define __oacr_sealed_class                     __declspec("__sa_sealed")
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
#define __oacr_min_function
#define __oacr_max_function
#define __oacr_canthrow
#define __oacr_bindReturn
#define __oacr_sa_deprecated(NewFunctionName)
#define __oacr_oleo_deprecated(NewFunctionName)
#define __oacr_oleo_banned(NewFunctionName)
#define __oacr_noheap
#define __oacr_notrunccast
#define __oacr_sealed_class
#define __oacr_requireNoThrow

#endif

//#include "offoacr.h"
