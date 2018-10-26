
/*++

Copyright (c) 1992-1999 Microsoft Corporation

Module Name:

    rpcndr.h

Abstract:

    Definitions for stub data structures and prototypes of helper functions.

--*/

// This version of the rpcndr.h file corresponds to MIDL version 5.0.+
// used with NT5 beta1+ env from build #1700 on.


#ifndef __RPCNDR_H_VERSION__
#define __RPCNDR_H_VERSION__        ( 475 )
#endif // __RPCNDR_H_VERSION__


#ifndef __RPCNDR_H__
#define __RPCNDR_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __REQUIRED_RPCNDR_H_VERSION__
    #if ( __RPCNDR_H_VERSION__ < __REQUIRED_RPCNDR_H_VERSION__ )
        #error incorrect <rpcndr.h> version. Use the header that matches with the MIDL compiler.
    #endif
#endif


//
// Set the packing level for RPC structures for Dos, Windows and Mac.
//

#if defined(__RPC_DOS__) || defined(__RPC_WIN16__) || defined(__RPC_MAC__)
#pragma pack(2)
#endif

#if defined(__RPC_MAC__)
#define _MAC_
#endif

#if defined(__RPC_WIN64__)
#include <pshpack8.h>
#endif

#include <basetsd.h>
#include <rpcnsip.h>


#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************

     Network Computing Architecture (NCA) definition:

     Network Data Representation: (NDR) Label format:
     An unsigned long (32 bits) with the following layout:

     3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
     1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    +---------------+---------------+---------------+-------+-------+
    |   Reserved    |   Reserved    |Floating point | Int   | Char  |
    |               |               |Representation | Rep.  | Rep.  |
    +---------------+---------------+---------------+-------+-------+

     Where

         Reserved:

             Must be zero (0) for NCA 1.5 and NCA 2.0.

         Floating point Representation is:

             0 - IEEE
             1 - VAX
             2 - Cray
             3 - IBM

         Int Rep. is Integer Representation:

             0 - Big Endian
             1 - Little Endian

         Char Rep. is Character Representation:

             0 - ASCII
             1 - EBCDIC

     The Microsoft Local Data Representation (for all platforms which are
     of interest currently is edefined below:

 ****************************************************************************/

#define NDR_CHAR_REP_MASK               (unsigned long)0X0000000FL
#define NDR_INT_REP_MASK                (unsigned long)0X000000F0L
#define NDR_FLOAT_REP_MASK              (unsigned long)0X0000FF00L

#define NDR_LITTLE_ENDIAN               (unsigned long)0X00000010L
#define NDR_BIG_ENDIAN                  (unsigned long)0X00000000L

#define NDR_IEEE_FLOAT                  (unsigned long)0X00000000L
#define NDR_VAX_FLOAT                   (unsigned long)0X00000100L
#define NDR_IBM_FLOAT                   (unsigned long)0X00000300L

#define NDR_ASCII_CHAR                  (unsigned long)0X00000000L
#define NDR_EBCDIC_CHAR                 (unsigned long)0X00000001L

#if defined(__RPC_MAC__)
#define NDR_LOCAL_DATA_REPRESENTATION   (unsigned long)0X00000000L
#define NDR_LOCAL_ENDIAN                NDR_BIG_ENDIAN
#else
#define NDR_LOCAL_DATA_REPRESENTATION   (unsigned long)0X00000010L
#define NDR_LOCAL_ENDIAN                NDR_LITTLE_ENDIAN
#endif


/****************************************************************************
 *  Macros for targeted platforms
 ****************************************************************************/

#if (0x500 <= _WIN32_WINNT)
#define TARGET_IS_NT50_OR_LATER                   1
#else
#define TARGET_IS_NT50_OR_LATER                   0
#endif

#if (defined(_WIN32_DCOM) || 0x400 <= _WIN32_WINNT)
#define TARGET_IS_NT40_OR_LATER                   1
#else
#define TARGET_IS_NT40_OR_LATER                   0
#endif

#if (0x400 <= WINVER)
#define TARGET_IS_NT351_OR_WIN95_OR_LATER         1
#else
#define TARGET_IS_NT351_OR_WIN95_OR_LATER         0
#endif

/****************************************************************************
 *  Other MIDL base types / predefined types:
 ****************************************************************************/

#define small char
typedef unsigned char byte;
typedef unsigned char boolean;

#ifndef _HYPER_DEFINED
#define _HYPER_DEFINED

#if !defined(__RPC_DOS__) && !defined(__RPC_WIN16__) && !defined(__RPC_MAC__) && (!defined(_M_IX86) || (defined(_INTEGRAL_MAX_BITS) && _INTEGRAL_MAX_BITS >= 64))
#define  hyper           __int64
#define MIDL_uhyper  unsigned __int64
#else
typedef double  hyper;
typedef double MIDL_uhyper;
#endif

#endif // _HYPER_DEFINED

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#ifndef _SIZE_T_DEFINED
#ifdef __RPC_WIN64__
typedef unsigned __int64 size_t;
#else
typedef unsigned int     size_t;
#endif
#define _SIZE_T_DEFINED
#endif

#ifdef __RPC_DOS__
#define __RPC_CALLEE       __far __pascal
#endif

#ifdef __RPC_WIN16__
#define __RPC_CALLEE       __far __pascal __export
#endif

#ifdef __RPC_WIN32__
#if   (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#define __RPC_CALLEE       __stdcall
#else
#define __RPC_CALLEE
#endif
#endif

#ifdef __RPC_MAC__
#define __RPC_CALLEE __far
#endif

#ifndef __MIDL_USER_DEFINED
#define midl_user_allocate MIDL_user_allocate
#define midl_user_free     MIDL_user_free
#define __MIDL_USER_DEFINED
#endif

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void             __RPC_USER MIDL_user_free( void __RPC_FAR * );

#ifdef __RPC_WIN16__
#define RPC_VAR_ENTRY __export __cdecl
#else
#define RPC_VAR_ENTRY __cdecl
#endif


/* winnt only */
#if defined(_M_IX86) || defined(_M_ALPHA) || defined(_M_IA64)
#define __MIDL_DECLSPEC_DLLIMPORT   __declspec(dllimport)
#define __MIDL_DECLSPEC_DLLEXPORT   __declspec(dllexport)
#else
#define __MIDL_DECLSPEC_DLLIMPORT
#define __MIDL_DECLSPEC_DLLEXPORT
#endif




/****************************************************************************
 * Context handle management related definitions:
 *
 * Client and Server Contexts.
 *
 ****************************************************************************/

typedef void __RPC_FAR * NDR_CCONTEXT;

typedef struct
    {
    void __RPC_FAR * pad[2];
    void __RPC_FAR * userContext;
    } __RPC_FAR * NDR_SCONTEXT;

#define NDRSContextValue(hContext) (&(hContext)->userContext)

#define cbNDRContext 20         /* size of context on WIRE */

typedef void (__RPC_USER __RPC_FAR * NDR_RUNDOWN)(void __RPC_FAR * context);

typedef void (__RPC_USER __RPC_FAR * NDR_NOTIFY_ROUTINE)(void);
typedef void (__RPC_USER __RPC_FAR * NDR_NOTIFY2_ROUTINE)(boolean flag);

typedef struct _SCONTEXT_QUEUE {
    unsigned long   NumberOfObjects;
    NDR_SCONTEXT  * ArrayOfObjects;
    } SCONTEXT_QUEUE, __RPC_FAR * PSCONTEXT_QUEUE;

RPCRTAPI
RPC_BINDING_HANDLE
RPC_ENTRY
NDRCContextBinding (
    IN NDR_CCONTEXT     CContext
    );

RPCRTAPI
void
RPC_ENTRY
NDRCContextMarshall (
    IN  NDR_CCONTEXT    CContext,
    OUT void __RPC_FAR *pBuff
    );

RPCRTAPI
void
RPC_ENTRY
NDRCContextUnmarshall (
    OUT NDR_CCONTEXT __RPC_FAR *pCContext,
    IN  RPC_BINDING_HANDLE      hBinding,
    IN  void __RPC_FAR *        pBuff,
    IN  unsigned long           DataRepresentation
    );

RPCRTAPI
void
RPC_ENTRY
NDRSContextMarshall (
    IN  NDR_SCONTEXT    CContext,
    OUT void __RPC_FAR *pBuff,
    IN  NDR_RUNDOWN     userRunDownIn
    );

RPCRTAPI
NDR_SCONTEXT
RPC_ENTRY
NDRSContextUnmarshall (
    IN  void __RPC_FAR *pBuff,
    IN  unsigned long   DataRepresentation
    );

RPCRTAPI
void
RPC_ENTRY
NDRSContextMarshallEx (
    IN  RPC_BINDING_HANDLE  BindingHandle,
    IN  NDR_SCONTEXT        CContext,
    OUT void __RPC_FAR     *pBuff,
    IN  NDR_RUNDOWN         userRunDownIn
    );

RPCRTAPI
void
RPC_ENTRY
NDRSContextMarshall2 (
    IN  RPC_BINDING_HANDLE  BindingHandle,
    IN  NDR_SCONTEXT        CContext,
    OUT void __RPC_FAR     *pBuff,
    IN  NDR_RUNDOWN         userRunDownIn,
    IN  void __RPC_FAR     *CtxGuard,
    IN unsigned long Flags
    );

RPCRTAPI
NDR_SCONTEXT
RPC_ENTRY
NDRSContextUnmarshallEx (
    IN  RPC_BINDING_HANDLE  BindingHandle,
    IN  void __RPC_FAR     *pBuff,
    IN  unsigned long       DataRepresentation
    );

RPCRTAPI
NDR_SCONTEXT
RPC_ENTRY
NDRSContextUnmarshall2(
    IN  RPC_BINDING_HANDLE  BindingHandle,
    IN  void __RPC_FAR     *pBuff,
    IN  unsigned long       DataRepresentation,
    IN  void __RPC_FAR     *CtxGuard,
    IN unsigned long Flags
    );

RPCRTAPI
void
RPC_ENTRY
RpcSsDestroyClientContext (
    IN void __RPC_FAR * __RPC_FAR * ContextHandle
    );


/****************************************************************************
    NDR conversion related definitions.
 ****************************************************************************/

#define byte_from_ndr(source, target) \
    { \
    *(target) = *(*(char __RPC_FAR * __RPC_FAR *)&(source)->Buffer)++; \
    }

#define byte_array_from_ndr(Source, LowerIndex, UpperIndex, Target) \
    { \
    NDRcopy ( \
        (((char __RPC_FAR *)(Target))+(LowerIndex)), \
        (Source)->Buffer, \
        (unsigned int)((UpperIndex)-(LowerIndex))); \
    *(unsigned long __RPC_FAR *)&(Source)->Buffer += ((UpperIndex)-(LowerIndex)); \
    }

#define boolean_from_ndr(source, target) \
    { \
    *(target) = *(*(char __RPC_FAR * __RPC_FAR *)&(source)->Buffer)++; \
    }

#define boolean_array_from_ndr(Source, LowerIndex, UpperIndex, Target) \
    { \
    NDRcopy ( \
        (((char __RPC_FAR *)(Target))+(LowerIndex)), \
        (Source)->Buffer, \
        (unsigned int)((UpperIndex)-(LowerIndex))); \
    *(unsigned long __RPC_FAR *)&(Source)->Buffer += ((UpperIndex)-(LowerIndex)); \
    }

#define small_from_ndr(source, target) \
    { \
    *(target) = *(*(char __RPC_FAR * __RPC_FAR *)&(source)->Buffer)++; \
    }

#define small_from_ndr_temp(source, target, format) \
    { \
    *(target) = *(*(char __RPC_FAR * __RPC_FAR *)(source))++; \
    }

#define small_array_from_ndr(Source, LowerIndex, UpperIndex, Target) \
    { \
    NDRcopy ( \
        (((char __RPC_FAR *)(Target))+(LowerIndex)), \
        (Source)->Buffer, \
        (unsigned int)((UpperIndex)-(LowerIndex))); \
    *(unsigned long __RPC_FAR *)&(Source)->Buffer += ((UpperIndex)-(LowerIndex)); \
    }

/****************************************************************************
    Platform specific mapping of c-runtime functions.
 ****************************************************************************/

#ifdef __RPC_DOS__
#define MIDL_ascii_strlen(string) \
    _fstrlen(string)
#define MIDL_ascii_strcpy(target,source) \
    _fstrcpy(target,source)
#define MIDL_memset(s,c,n) \
    _fmemset(s,c,n)
#endif

#ifdef __RPC_WIN16__
#define MIDL_ascii_strlen(string) \
    _fstrlen(string)
#define MIDL_ascii_strcpy(target,source) \
    _fstrcpy(target,source)
#define MIDL_memset(s,c,n) \
    _fmemset(s,c,n)
#endif

#if defined(__RPC_WIN32__) || defined(__RPC_MAC__) || defined(__RPC_WIN64__)
#define MIDL_ascii_strlen(string) \
    strlen(string)
#define MIDL_ascii_strcpy(target,source) \
    strcpy(target,source)
#define MIDL_memset(s,c,n) \
    memset(s,c,n)
#endif

/****************************************************************************
    Ndr Library helper function prototypes for MIDL 1.0 ndr functions.
 ****************************************************************************/

RPCRTAPI
void
RPC_ENTRY
NDRcopy (
    IN void __RPC_FAR *pTarget,
    IN void __RPC_FAR *pSource,
    IN unsigned int size
    );

RPCRTAPI
size_t
RPC_ENTRY
MIDL_wchar_strlen (
    IN wchar_t __RPC_FAR *   s
    );

RPCRTAPI
void
RPC_ENTRY
MIDL_wchar_strcpy (
    OUT void __RPC_FAR *     t,
    IN wchar_t __RPC_FAR *   s
    );

RPCRTAPI
void
RPC_ENTRY
char_from_ndr (
    IN OUT PRPC_MESSAGE                           SourceMessage,
    OUT unsigned char __RPC_FAR *                 Target
    );

RPCRTAPI
void
RPC_ENTRY
char_array_from_ndr (
    IN OUT PRPC_MESSAGE                           SourceMessage,
    IN unsigned long                              LowerIndex,
    IN unsigned long                              UpperIndex,
    OUT unsigned char __RPC_FAR *                 Target
    );

RPCRTAPI
void
RPC_ENTRY
short_from_ndr (
    IN OUT PRPC_MESSAGE                           source,
    OUT unsigned short __RPC_FAR *                target
    );

RPCRTAPI
void
RPC_ENTRY
short_array_from_ndr(
    IN OUT PRPC_MESSAGE                           SourceMessage,
    IN unsigned long                              LowerIndex,
    IN unsigned long                              UpperIndex,
    OUT unsigned short __RPC_FAR *                Target
    );

RPCRTAPI
void
RPC_ENTRY
short_from_ndr_temp (
    IN OUT unsigned char __RPC_FAR * __RPC_FAR *  source,
    OUT unsigned short __RPC_FAR *                target,
    IN unsigned long                              format
    );

RPCRTAPI
void
RPC_ENTRY
long_from_ndr (
    IN OUT PRPC_MESSAGE                           source,
    OUT unsigned long __RPC_FAR *                 target
    );

RPCRTAPI
void
RPC_ENTRY
long_array_from_ndr(
    IN OUT PRPC_MESSAGE                           SourceMessage,
    IN unsigned long                              LowerIndex,
    IN unsigned long                              UpperIndex,
    OUT unsigned long __RPC_FAR *                 Target
    );

RPCRTAPI
void
RPC_ENTRY
long_from_ndr_temp (
    IN OUT unsigned char __RPC_FAR * __RPC_FAR *  source,
    OUT unsigned long __RPC_FAR *                 target,
    IN unsigned long                              format
    );

RPCRTAPI
void
RPC_ENTRY
enum_from_ndr(
    IN OUT PRPC_MESSAGE                           SourceMessage,
    OUT unsigned int __RPC_FAR *                  Target
    );

RPCRTAPI
void
RPC_ENTRY
float_from_ndr (
    IN OUT PRPC_MESSAGE                           SourceMessage,
    OUT void __RPC_FAR *                          Target
    );

RPCRTAPI
void
RPC_ENTRY
float_array_from_ndr (
    IN OUT PRPC_MESSAGE                           SourceMessage,
    IN unsigned long                              LowerIndex,
    IN unsigned long                              UpperIndex,
    OUT void __RPC_FAR *                          Target
    );

RPCRTAPI
void
RPC_ENTRY
double_from_ndr (
    IN OUT PRPC_MESSAGE                           SourceMessage,
    OUT void __RPC_FAR *                          Target
    );

RPCRTAPI
void
RPC_ENTRY
double_array_from_ndr (
    IN OUT PRPC_MESSAGE                           SourceMessage,
    IN unsigned long                              LowerIndex,
    IN unsigned long                              UpperIndex,
    OUT void __RPC_FAR *                          Target
    );

RPCRTAPI
void
RPC_ENTRY
hyper_from_ndr (
    IN OUT PRPC_MESSAGE                           source,
    OUT    hyper __RPC_FAR *                      target
    );

RPCRTAPI
void
RPC_ENTRY
hyper_array_from_ndr(
    IN OUT PRPC_MESSAGE                           SourceMessage,
    IN unsigned long                              LowerIndex,
    IN unsigned long                              UpperIndex,
    OUT          hyper __RPC_FAR *                Target
    );

RPCRTAPI
void
RPC_ENTRY
hyper_from_ndr_temp (
    IN OUT unsigned char __RPC_FAR * __RPC_FAR *  source,
    OUT             hyper __RPC_FAR *             target,
    IN   unsigned   long                          format
    );

RPCRTAPI
void
RPC_ENTRY
data_from_ndr (
    PRPC_MESSAGE                                  source,
    void __RPC_FAR *                              target,
    char __RPC_FAR *                              format,
    unsigned char                                 MscPak
    );

RPCRTAPI
void
RPC_ENTRY
data_into_ndr (
    void __RPC_FAR *                              source,
    PRPC_MESSAGE                                  target,
    char __RPC_FAR *                              format,
    unsigned char                                 MscPak
    );

RPCRTAPI
void
RPC_ENTRY
tree_into_ndr (
    void __RPC_FAR *                              source,
    PRPC_MESSAGE                                  target,
    char __RPC_FAR *                              format,
    unsigned char                                 MscPak
    );

RPCRTAPI
void
RPC_ENTRY
data_size_ndr (
    void __RPC_FAR *                              source,
    PRPC_MESSAGE                                  target,
    char __RPC_FAR *                              format,
    unsigned char                                 MscPak
    );

RPCRTAPI
void
RPC_ENTRY
tree_size_ndr (
    void __RPC_FAR *                              source,
    PRPC_MESSAGE                                  target,
    char __RPC_FAR *                              format,
    unsigned char                                 MscPak
    );

RPCRTAPI
void
RPC_ENTRY
tree_peek_ndr (
    PRPC_MESSAGE                                  source,
    unsigned char __RPC_FAR * __RPC_FAR *         buffer,
    char __RPC_FAR *                              format,
    unsigned char                                 MscPak
    );

RPCRTAPI
void __RPC_FAR *
RPC_ENTRY
midl_allocate (
    size_t      size
    );

/****************************************************************************
    MIDL 2.0 ndr definitions.
 ****************************************************************************/

typedef unsigned long error_status_t;

#define _midl_ma1( p, cast )    *(*( cast **)&p)++
#define _midl_ma2( p, cast )    *(*( cast **)&p)++
#define _midl_ma4( p, cast )    *(*( cast **)&p)++
#define _midl_ma8( p, cast )    *(*( cast **)&p)++

#define _midl_unma1( p, cast )  *(( cast *)p)++
#define _midl_unma2( p, cast )  *(( cast *)p)++
#define _midl_unma3( p, cast )  *(( cast *)p)++
#define _midl_unma4( p, cast )  *(( cast *)p)++

// Some alignment specific macros.

#define _midl_fa2( p )          (p = (RPC_BUFPTR )((ULONG_PTR)(p+1) & ~0x1))
#define _midl_fa4( p )          (p = (RPC_BUFPTR )((ULONG_PTR)(p+3) & ~0x3))
#define _midl_fa8( p )          (p = (RPC_BUFPTR )((ULONG_PTR)(p+7) & ~0x7))

#define _midl_addp( p, n )      (p += n)

// Marshalling macros

#define _midl_marsh_lhs( p, cast )  *(*( cast **)&p)++
#define _midl_marsh_up( mp, p )     *(*(unsigned long **)&mp)++ = (unsigned long)p
#define _midl_advmp( mp )           *(*(unsigned long **)&mp)++
#define _midl_unmarsh_up( p )       (*(*(unsigned long **)&p)++)


////////////////////////////////////////////////////////////////////////////
// Ndr macros.
////////////////////////////////////////////////////////////////////////////

#define NdrMarshConfStringHdr( p, s, l )    (_midl_ma4( p, unsigned long) = s, \
                                            _midl_ma4( p, unsigned long) = 0, \
                                            _midl_ma4( p, unsigned long) = l)

#define NdrUnMarshConfStringHdr(p, s, l)    ((s=_midl_unma4(p,unsigned long),\
                                            (_midl_addp(p,4)),               \
                                            (l=_midl_unma4(p,unsigned long))

#define NdrMarshCCtxtHdl(pc,p)  (NDRCContextMarshall( (NDR_CCONTEXT)pc, p ),p+20)

#define NdrUnMarshCCtxtHdl(pc,p,h,drep) \
        (NDRCContextUnmarshall((NDR_CONTEXT)pc,h,p,drep), p+20)

#define NdrUnMarshSCtxtHdl(pc, p,drep)  (pc = NdrSContextUnMarshall(p,drep ))


#define NdrMarshSCtxtHdl(pc,p,rd)   (NdrSContextMarshall((NDR_SCONTEXT)pc,p, (NDR_RUNDOWN)rd)


#define NdrFieldOffset(s,f)     (LONG_PTR)(& (((s __RPC_FAR *)0)->f))
#define NdrFieldPad(s,f,p,t)    ((unsigned long)(NdrFieldOffset(s,f) - NdrFieldOffset(s,p)) - sizeof(t))


#if defined(__RPC_MAC__)
#define NdrFcShort(s)   (unsigned char)(s >> 8), (unsigned char)(s & 0xff)
#define NdrFcLong(s)    (unsigned char)(s >> 24), (unsigned char)((s & 0x00ff0000) >> 16), \
                        (unsigned char)((s & 0x0000ff00) >> 8), (unsigned char)(s & 0xff)
#else
#define NdrFcShort(s)   (unsigned char)(s & 0xff), (unsigned char)(s >> 8)
#define NdrFcLong(s)    (unsigned char)(s & 0xff), (unsigned char)((s & 0x0000ff00) >> 8), \
                        (unsigned char)((s & 0x00ff0000) >> 16), (unsigned char)(s >> 24)
#endif //  Mac

//
// On the server side, the following exceptions are mapped to
// the bad stub data exception if -error stub_data is used.
//

#define RPC_BAD_STUB_DATA_EXCEPTION_FILTER  \
                 ( (RpcExceptionCode() == STATUS_ACCESS_VIOLATION)  || \
                   (RpcExceptionCode() == STATUS_DATATYPE_MISALIGNMENT) || \
                   (RpcExceptionCode() == RPC_X_BAD_STUB_DATA) || \
                   (RpcExceptionCode() == RPC_S_INVALID_BOUND) )

/////////////////////////////////////////////////////////////////////////////
// Some stub helper functions.
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// Stub helper structures.
////////////////////////////////////////////////////////////////////////////

struct _MIDL_STUB_MESSAGE;
struct _MIDL_STUB_DESC;
struct _FULL_PTR_XLAT_TABLES;

typedef unsigned char __RPC_FAR * RPC_BUFPTR;
typedef unsigned long             RPC_LENGTH;

// Expression evaluation callback routine prototype.
typedef void (__RPC_USER __RPC_FAR * EXPR_EVAL)( struct _MIDL_STUB_MESSAGE __RPC_FAR * );

typedef const unsigned char __RPC_FAR * PFORMAT_STRING;

/*
 * Multidimensional conformant/varying array struct.
 */
typedef struct
    {
    long                            Dimension;

    /* These fields MUST be (unsigned long *) */
    unsigned long __RPC_FAR *       BufferConformanceMark;
    unsigned long __RPC_FAR *       BufferVarianceMark;

    /* Count arrays, used for top level arrays in -Os stubs */
    unsigned long __RPC_FAR *       MaxCountArray;
    unsigned long __RPC_FAR *       OffsetArray;
    unsigned long __RPC_FAR *       ActualCountArray;
    } ARRAY_INFO, __RPC_FAR *PARRAY_INFO;

/*
 *  Pipe related definitions.
 */

typedef struct _NDR_PIPE_DESC *       PNDR_PIPE_DESC;
typedef struct _NDR_PIPE_MESSAGE *    PNDR_PIPE_MESSAGE;

typedef struct _NDR_ASYNC_MESSAGE *   PNDR_ASYNC_MESSAGE;
typedef struct _NDR_CORRELATION_INFO *PNDR_CORRELATION_INFO;

/*
 * MIDL Stub Message
 */
#if !defined(__RPC_DOS__) && !defined(__RPC_WIN16__) && !defined(__RPC_MAC__) && !defined(__RPC_WIN64__)
#include <pshpack4.h>
#endif

typedef struct _MIDL_STUB_MESSAGE
    {
    /* RPC message structure. */
    PRPC_MESSAGE                RpcMsg;

    /* Pointer into RPC message buffer. */
    unsigned char __RPC_FAR *   Buffer;

    /*
     * These are used internally by the Ndr routines to mark the beginning
     * and end of an incoming RPC buffer.
     */
    unsigned char __RPC_FAR *   BufferStart;
    unsigned char __RPC_FAR *   BufferEnd;

    /*
     * Used internally by the Ndr routines as a place holder in the buffer.
     * On the marshalling side it's used to mark the location where conformance
     * size should be marshalled.
     * On the unmarshalling side it's used to mark the location in the buffer
     * used during pointer unmarshalling to base pointer offsets off of.
     */
    unsigned char __RPC_FAR *   BufferMark;

    /* Set by the buffer sizing routines. */
    unsigned long               BufferLength;

    /* Set by the memory sizing routines. */
    unsigned long               MemorySize;

    /* Pointer to user memory. */
    unsigned char __RPC_FAR *   Memory;

    /* Is the Ndr routine begin called from a client side stub. */
    int                         IsClient;

    /* Can the buffer be re-used for memory on unmarshalling. */
    int                         ReuseBuffer;

    /* Holds the current pointer to an allocate all nodes memory block. */
    unsigned char __RPC_FAR *   AllocAllNodesMemory;

    /* Used for debugging asserts only, remove later. */
    unsigned char __RPC_FAR *   AllocAllNodesMemoryEnd;

    /*
     * Stuff needed while handling complex structures
     */

    /* Ignore imbeded pointers while computing buffer or memory sizes. */
    int                         IgnoreEmbeddedPointers;

    /*
     * This marks the location in the buffer where pointees of a complex
     * struct reside.
     */
    unsigned char __RPC_FAR *   PointerBufferMark;

    /*
     * Used to catch errors in SendReceive.
     */
    unsigned char               fBufferValid;

    /*
     * Obsolete unused field (formerly MaxContextHandleNumber).
     */
    unsigned char               uFlags;

    /*
     * Used internally by the Ndr routines.  Holds the max counts for
     * a conformant array.
     */
    ULONG_PTR                   MaxCount;

    /*
     * Used internally by the Ndr routines.  Holds the offsets for a varying
     * array.
     */
    unsigned long               Offset;

    /*
     * Used internally by the Ndr routines.  Holds the actual counts for
     * a varying array.
     */
    unsigned long               ActualCount;

    /* Allocation and Free routine to be used by the Ndr routines. */
    void __RPC_FAR *    (__RPC_FAR __RPC_API * pfnAllocate)(size_t);
    void                (__RPC_FAR __RPC_API * pfnFree)(void __RPC_FAR *);

    /*
     * Top of parameter stack.  Used for "single call" stubs during marshalling
     * to hold the beginning of the parameter list on the stack.  Needed to
     * extract parameters which hold attribute values for top level arrays and
     * pointers.
     */
    unsigned char __RPC_FAR *       StackTop;

    /*
     *  Fields used for the transmit_as and represent_as objects.
     *  For represent_as the mapping is: presented=local, transmit=named.
     */
    unsigned char __RPC_FAR *       pPresentedType;
    unsigned char __RPC_FAR *       pTransmitType;

    /*
     * When we first construct a binding on the client side, stick it
     * in the rpcmessage and later call RpcGetBuffer, the handle field
     * in the rpcmessage is changed. That's fine except that we need to
     * have that original handle for use in unmarshalling context handles
     * (the second argument in NDRCContextUnmarshall to be exact). So
     * stash the contructed handle here and extract it when needed.
     */
    handle_t                        SavedHandle;

    /*
     * Pointer back to the stub descriptor.  Use this to get all handle info.
     */
    const struct _MIDL_STUB_DESC __RPC_FAR *    StubDesc;

    /*
     * Full pointer stuff.
     */
    struct _FULL_PTR_XLAT_TABLES __RPC_FAR *    FullPtrXlatTables;

    unsigned long                   FullPtrRefId;

    /*
     * Used to be PointeeBufferLength
     */
    unsigned long                   ulUnused1;

    int                             fInDontFree       :1;
    int                             fDontCallFreeInst :1;
    int                             fInOnlyParam      :1;
    int                             fHasReturn        :1;
    int                             fHasExtensions    :1;
    int                             fHasNewCorrDesc   :1;
    int                             fUnused           :10;


    unsigned long                   dwDestContext;
    void __RPC_FAR *                pvDestContext;

    NDR_SCONTEXT *                  SavedContextHandles;

    long                            ParamNumber;

    struct IRpcChannelBuffer __RPC_FAR *    pRpcChannelBuffer;

    PARRAY_INFO                     pArrayInfo;

    /*
     * This is where the Beta2 stub message ends.
     */

    unsigned long __RPC_FAR *       SizePtrCountArray;
    unsigned long __RPC_FAR *       SizePtrOffsetArray;
    unsigned long __RPC_FAR *       SizePtrLengthArray;

    /*
     * Interpreter argument queue.  Used on server side only.
     */
    void __RPC_FAR *                pArgQueue;

    unsigned long                   dwStubPhase;

    /*
     * Pipe descriptor, defined for the 4.0 release.
     */
    PNDR_PIPE_DESC                  pPipeDesc;

    /*
     *  Async message pointer, correlation data - NT 5.0 features.
     */
    PNDR_ASYNC_MESSAGE              pAsyncMsg;
    PNDR_CORRELATION_INFO           pCorrInfo;
    unsigned char *                 pCorrMemory;

    void *                          pMemoryList;

    /*
     *  Fields up to this point present since the 3.50 release.
     */


    /*
     *  New reserved fields introduced for Windows 2000 Beta 3
     */

    ULONG_PTR                       w2kReserved[5];

    } MIDL_STUB_MESSAGE, __RPC_FAR *PMIDL_STUB_MESSAGE;

#if !defined(__RPC_DOS__) && !defined(__RPC_WIN16__) && !defined(__RPC_MAC__) && !defined(__RPC_WIN64__)
#include <poppack.h>
#endif

/*
 * Generic handle bind/unbind routine pair.
 */
typedef void __RPC_FAR *
        (__RPC_FAR __RPC_API * GENERIC_BINDING_ROUTINE)
        (void __RPC_FAR *);
typedef void
        (__RPC_FAR __RPC_API * GENERIC_UNBIND_ROUTINE)
        (void __RPC_FAR *, unsigned char __RPC_FAR *);

typedef struct _GENERIC_BINDING_ROUTINE_PAIR
    {
    GENERIC_BINDING_ROUTINE     pfnBind;
    GENERIC_UNBIND_ROUTINE      pfnUnbind;
    } GENERIC_BINDING_ROUTINE_PAIR, __RPC_FAR *PGENERIC_BINDING_ROUTINE_PAIR;

typedef struct __GENERIC_BINDING_INFO
    {
    void __RPC_FAR *            pObj;
    unsigned int                Size;
    GENERIC_BINDING_ROUTINE     pfnBind;
    GENERIC_UNBIND_ROUTINE      pfnUnbind;
    } GENERIC_BINDING_INFO, __RPC_FAR *PGENERIC_BINDING_INFO;

// typedef EXPR_EVAL - see above
// typedefs for xmit_as

#if (defined(_MSC_VER)) && !defined(MIDL_PASS)
// a Microsoft C++ compiler
#define NDR_SHAREABLE __inline
#else
#define NDR_SHAREABLE static
#endif


typedef void (__RPC_FAR __RPC_USER * XMIT_HELPER_ROUTINE)
    ( PMIDL_STUB_MESSAGE );

typedef struct _XMIT_ROUTINE_QUINTUPLE
    {
    XMIT_HELPER_ROUTINE     pfnTranslateToXmit;
    XMIT_HELPER_ROUTINE     pfnTranslateFromXmit;
    XMIT_HELPER_ROUTINE     pfnFreeXmit;
    XMIT_HELPER_ROUTINE     pfnFreeInst;
    } XMIT_ROUTINE_QUINTUPLE, __RPC_FAR *PXMIT_ROUTINE_QUINTUPLE;

typedef unsigned long
(__RPC_FAR __RPC_USER * USER_MARSHAL_SIZING_ROUTINE)
    (unsigned long __RPC_FAR *,
     unsigned long,
     void __RPC_FAR * );

typedef unsigned char __RPC_FAR *
(__RPC_FAR __RPC_USER * USER_MARSHAL_MARSHALLING_ROUTINE)
    (unsigned long __RPC_FAR *,
     unsigned char  __RPC_FAR * ,
     void __RPC_FAR * );

typedef unsigned char __RPC_FAR *
(__RPC_FAR __RPC_USER * USER_MARSHAL_UNMARSHALLING_ROUTINE)
    (unsigned long __RPC_FAR *,
     unsigned char  __RPC_FAR * ,
     void __RPC_FAR * );

typedef void (__RPC_FAR __RPC_USER * USER_MARSHAL_FREEING_ROUTINE)
    (unsigned long __RPC_FAR *,
     void __RPC_FAR * );

typedef struct _USER_MARSHAL_ROUTINE_QUADRUPLE
    {
    USER_MARSHAL_SIZING_ROUTINE          pfnBufferSize;
    USER_MARSHAL_MARSHALLING_ROUTINE     pfnMarshall;
    USER_MARSHAL_UNMARSHALLING_ROUTINE   pfnUnmarshall;
    USER_MARSHAL_FREEING_ROUTINE         pfnFree;
    } USER_MARSHAL_ROUTINE_QUADRUPLE;

#define USER_MARSHAL_CB_SIGNATURE 'USRC'

typedef enum _USER_MARSHAL_CB_TYPE
{
    USER_MARSHAL_CB_BUFFER_SIZE,
    USER_MARSHAL_CB_MARSHALL,
    USER_MARSHAL_CB_UNMARSHALL,
    USER_MARSHAL_CB_FREE
} USER_MARSHAL_CB_TYPE;

typedef struct _USER_MARSHAL_CB
{
    unsigned long       Flags;
    PMIDL_STUB_MESSAGE  pStubMsg;
    PFORMAT_STRING      pReserve;
    unsigned long	Signature;
    USER_MARSHAL_CB_TYPE CBType;
    PFORMAT_STRING      pFormat;
    PFORMAT_STRING      pTypeFormat;
} USER_MARSHAL_CB;


#define USER_CALL_CTXT_MASK(f)  ((f) & 0x00ff)
#define USER_CALL_AUX_MASK(f)   ((f) & 0xff00)
#define GET_USER_DATA_REP(f)    ((f) >> 16)

#define USER_CALL_IS_ASYNC      0x0100      /* aux flag: in an [async] call */
#define USER_CALL_NEW_CORRELATION_DESC 0x0200 

typedef struct _MALLOC_FREE_STRUCT
    {
    void __RPC_FAR *    (__RPC_FAR __RPC_USER * pfnAllocate)(size_t);
    void                (__RPC_FAR __RPC_USER * pfnFree)(void __RPC_FAR *);
    } MALLOC_FREE_STRUCT;

typedef struct _COMM_FAULT_OFFSETS
    {
    short       CommOffset;
    short       FaultOffset;
    } COMM_FAULT_OFFSETS;

/*
 * MIDL Stub Descriptor
 */

typedef struct _MIDL_STUB_DESC
    {

    void __RPC_FAR *    RpcInterfaceInformation;

    void __RPC_FAR *    (__RPC_FAR __RPC_API * pfnAllocate)(size_t);
    void                (__RPC_FAR __RPC_API * pfnFree)(void __RPC_FAR *);

    union
        {
        handle_t __RPC_FAR *            pAutoHandle;
        handle_t __RPC_FAR *            pPrimitiveHandle;
        PGENERIC_BINDING_INFO           pGenericBindingInfo;
        } IMPLICIT_HANDLE_INFO;

    const NDR_RUNDOWN __RPC_FAR *                   apfnNdrRundownRoutines;
    const GENERIC_BINDING_ROUTINE_PAIR __RPC_FAR *  aGenericBindingRoutinePairs;

    const EXPR_EVAL __RPC_FAR *                     apfnExprEval;

    const XMIT_ROUTINE_QUINTUPLE __RPC_FAR *        aXmitQuintuple;

    const unsigned char __RPC_FAR *                 pFormatTypes;

    int                                             fCheckBounds;

    /* Ndr library version. */
    unsigned long                                   Version;

    MALLOC_FREE_STRUCT __RPC_FAR *                  pMallocFreeStruct;

    long                                MIDLVersion;

    const COMM_FAULT_OFFSETS __RPC_FAR *    CommFaultOffsets;

    // New fields for version 3.0+
    const USER_MARSHAL_ROUTINE_QUADRUPLE __RPC_FAR * aUserMarshalQuadruple;

    // Notify routines - added for NT5, MIDL 5.0
    const NDR_NOTIFY_ROUTINE __RPC_FAR *            NotifyRoutineTable;

    /*
     * Reserved for future use.
     */

    ULONG_PTR                                mFlags;
    ULONG_PTR                                Reserved3;
    ULONG_PTR                                Reserved4;
    ULONG_PTR                                Reserved5;

    } MIDL_STUB_DESC;

typedef const MIDL_STUB_DESC __RPC_FAR * PMIDL_STUB_DESC;

typedef void __RPC_FAR * PMIDL_XMIT_TYPE;

/*
 * MIDL Stub Format String.  This is a const in the stub.
 */
#if !defined( RC_INVOKED )
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning( disable:4200 )
#endif
typedef struct _MIDL_FORMAT_STRING
    {
    short               Pad;
    unsigned char       Format[];
    } MIDL_FORMAT_STRING;
#if !defined( RC_INVOKED )
#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning( default:4200 )
#endif
#endif

/*
 * Stub thunk used for some interpreted server stubs.
 */
typedef void (__RPC_FAR __RPC_API * STUB_THUNK)( PMIDL_STUB_MESSAGE );

typedef long (__RPC_FAR __RPC_API * SERVER_ROUTINE)();

/*
 * Server Interpreter's information strucuture.
 */
typedef struct  _MIDL_SERVER_INFO_
    {
    PMIDL_STUB_DESC             pStubDesc;
    const SERVER_ROUTINE *      DispatchTable;
    PFORMAT_STRING              ProcString;
    const unsigned short *      FmtStringOffset;
    const STUB_THUNK *          ThunkTable;
    PFORMAT_STRING              LocalFormatTypes;
    PFORMAT_STRING              LocalProcString;
    const unsigned short *      LocalFmtStringOffset;
    } MIDL_SERVER_INFO, *PMIDL_SERVER_INFO;

/*
 * Stubless object proxy information structure.
 */
typedef struct _MIDL_STUBLESS_PROXY_INFO
    {
    PMIDL_STUB_DESC                     pStubDesc;
    PFORMAT_STRING                      ProcFormatString;
    const unsigned short __RPC_FAR *    FormatStringOffset;
    PFORMAT_STRING                      LocalFormatTypes;
    PFORMAT_STRING                      LocalProcString;
    const unsigned short __RPC_FAR *    LocalFmtStringOffset;
    } MIDL_STUBLESS_PROXY_INFO;

typedef MIDL_STUBLESS_PROXY_INFO __RPC_FAR * PMIDL_STUBLESS_PROXY_INFO;

/*
 * This is the return value from NdrClientCall.
 */
typedef union _CLIENT_CALL_RETURN
    {
    void __RPC_FAR *        Pointer;
    LONG_PTR                 Simple;
    } CLIENT_CALL_RETURN;

/*
 * Full pointer data structures.
 */

typedef enum
        {
        XLAT_SERVER = 1,
        XLAT_CLIENT
        } XLAT_SIDE;

/*
 * Stores the translation for the conversion from a full pointer into it's
 * corresponding ref id.
 */
typedef struct _FULL_PTR_TO_REFID_ELEMENT
    {
    struct _FULL_PTR_TO_REFID_ELEMENT __RPC_FAR *  Next;

    void __RPC_FAR *            Pointer;
    unsigned long       RefId;
    unsigned char       State;
    } FULL_PTR_TO_REFID_ELEMENT, __RPC_FAR *PFULL_PTR_TO_REFID_ELEMENT;

/*
 * Full pointer translation tables.
 */
typedef struct _FULL_PTR_XLAT_TABLES
    {
    /*
     * Ref id to pointer translation information.
     */
    struct
        {
        void __RPC_FAR *__RPC_FAR *             XlatTable;
        unsigned char __RPC_FAR *     StateTable;
        unsigned long       NumberOfEntries;
        } RefIdToPointer;

    /*
     * Pointer to ref id translation information.
     */
    struct
        {
        PFULL_PTR_TO_REFID_ELEMENT __RPC_FAR *  XlatTable;
        unsigned long                   NumberOfBuckets;
        unsigned long                   HashMask;
        } PointerToRefId;

    /*
     * Next ref id to use.
     */
    unsigned long           NextRefId;

    /*
     * Keep track of the translation size we're handling : server or client.
     * This tells us when we have to do reverse translations when we insert
     * new translations.  On the server we must insert a pointer-to-refid
     * translation whenever we insert a refid-to-pointer translation, and
     * vica versa for the client.
     */
    XLAT_SIDE               XlatSide;
    } FULL_PTR_XLAT_TABLES, __RPC_FAR *PFULL_PTR_XLAT_TABLES;

/***************************************************************************
 ** New MIDL 2.0 Ndr routine templates
 ***************************************************************************/

/*
 * Marshall routines
 */

RPCRTAPI
void
RPC_ENTRY
NdrSimpleTypeMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    unsigned char                       FormatChar
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrPointerMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Structures */

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrSimpleStructMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrConformantStructMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrConformantVaryingStructMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrComplexStructMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Arrays */

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrFixedArrayMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrConformantArrayMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrConformantVaryingArrayMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrVaryingArrayMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrComplexArrayMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Strings */

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrNonConformantStringMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrConformantStringMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Unions */

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrEncapsulatedUnionMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrNonEncapsulatedUnionMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Byte count pointer */

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrByteCountPointerMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Transmit as and represent as*/

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrXmitOrRepAsMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* User_marshal */

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrUserMarshalMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Cairo interface pointer */

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrInterfacePointerMarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Context handles */

RPCRTAPI
void
RPC_ENTRY
NdrClientContextMarshall(
    PMIDL_STUB_MESSAGE    pStubMsg,
    NDR_CCONTEXT          ContextHandle,
    int                   fCheck
    );

RPCRTAPI
void
RPC_ENTRY
NdrServerContextMarshall(
    PMIDL_STUB_MESSAGE    pStubMsg,
    NDR_SCONTEXT          ContextHandle,
    NDR_RUNDOWN           RundownRoutine
    );

RPCRTAPI
void
RPC_ENTRY
NdrServerContextNewMarshall(
    PMIDL_STUB_MESSAGE    pStubMsg,
    NDR_SCONTEXT          ContextHandle,
    NDR_RUNDOWN           RundownRoutine,
    PFORMAT_STRING        pFormat
    );

/*
 * Unmarshall routines
 */

RPCRTAPI
void
RPC_ENTRY
NdrSimpleTypeUnmarshall(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    unsigned char                       FormatChar
    );

RPCRTAPI
unsigned char * RPC_ENTRY
NdrRangeUnmarshall(
    PMIDL_STUB_MESSAGE  pStubMsg,
    unsigned char **    ppMemory,
    PFORMAT_STRING      pFormat,
    unsigned char       fMustAlloc
    );

RPCRTAPI
void
RPC_ENTRY
NdrCorrelationInitialize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned long *                     pMemory,
    unsigned long                       CacheSize,
    unsigned long                       flags
    );

RPCRTAPI
void
RPC_ENTRY
NdrCorrelationPass(
    PMIDL_STUB_MESSAGE                  pStubMsg
    );

RPCRTAPI
void
RPC_ENTRY
NdrCorrelationFree(
    PMIDL_STUB_MESSAGE                  pStubMsg
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrPointerUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

/* Structures */

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrSimpleStructUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrConformantStructUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrConformantVaryingStructUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrComplexStructUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

/* Arrays */

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrFixedArrayUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrConformantArrayUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrConformantVaryingArrayUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrVaryingArrayUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrComplexArrayUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

/* Strings */

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrNonConformantStringUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrConformantStringUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

/* Unions */

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrEncapsulatedUnionUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrNonEncapsulatedUnionUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

/* Byte count pointer */

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrByteCountPointerUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

/* Transmit as and represent as*/

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrXmitOrRepAsUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

/* User_marshal */

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrUserMarshalUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

/* Cairo interface pointer */

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrInterfacePointerUnmarshall(
    PMIDL_STUB_MESSAGE                      pStubMsg,
    unsigned char __RPC_FAR * __RPC_FAR *   ppMemory,
    PFORMAT_STRING                          pFormat,
    unsigned char                           fMustAlloc
    );

/* Context handles */

RPCRTAPI
void
RPC_ENTRY
NdrClientContextUnmarshall(
    PMIDL_STUB_MESSAGE          pStubMsg,
    NDR_CCONTEXT __RPC_FAR *    pContextHandle,
    RPC_BINDING_HANDLE          BindHandle
    );

RPCRTAPI
NDR_SCONTEXT
RPC_ENTRY
NdrServerContextUnmarshall(
    PMIDL_STUB_MESSAGE          pStubMsg
    );

/* New context handle flavors */

RPCRTAPI
NDR_SCONTEXT
RPC_ENTRY
NdrContextHandleInitialize(
    IN  PMIDL_STUB_MESSAGE  pStubMsg,
    IN  PFORMAT_STRING      pFormat
    );

RPCRTAPI
NDR_SCONTEXT
RPC_ENTRY
NdrServerContextNewUnmarshall(
    IN  PMIDL_STUB_MESSAGE  pStubMsg,
    IN  PFORMAT_STRING      pFormat
    );

/*
 * Buffer sizing routines
 */

RPCRTAPI
void
RPC_ENTRY
NdrPointerBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Structures */

RPCRTAPI
void
RPC_ENTRY
NdrSimpleStructBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
void
RPC_ENTRY
NdrConformantStructBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
void
RPC_ENTRY
NdrConformantVaryingStructBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
void
RPC_ENTRY
NdrComplexStructBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Arrays */

RPCRTAPI
void
RPC_ENTRY
NdrFixedArrayBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
void
RPC_ENTRY
NdrConformantArrayBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
void
RPC_ENTRY
NdrConformantVaryingArrayBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
void
RPC_ENTRY
NdrVaryingArrayBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
void
RPC_ENTRY
NdrComplexArrayBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Strings */

RPCRTAPI
void
RPC_ENTRY
NdrConformantStringBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
void
RPC_ENTRY
NdrNonConformantStringBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Unions */

RPCRTAPI
void
RPC_ENTRY
NdrEncapsulatedUnionBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
void
RPC_ENTRY
NdrNonEncapsulatedUnionBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Byte count pointer */

RPCRTAPI
void
RPC_ENTRY
NdrByteCountPointerBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Transmit as and represent as*/

RPCRTAPI
void
RPC_ENTRY
NdrXmitOrRepAsBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* User_marshal */

RPCRTAPI
void
RPC_ENTRY
NdrUserMarshalBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Cairo Interface pointer */

RPCRTAPI
void
RPC_ENTRY
NdrInterfacePointerBufferSize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

// Context Handle size
//
RPCRTAPI
void
RPC_ENTRY
NdrContextHandleSize(
    PMIDL_STUB_MESSAGE          pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/*
 * Memory sizing routines
 */

RPCRTAPI
unsigned long
RPC_ENTRY
NdrPointerMemorySize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

/* Structures */

RPCRTAPI
unsigned long
RPC_ENTRY
NdrSimpleStructMemorySize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned long
RPC_ENTRY
NdrConformantStructMemorySize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned long
RPC_ENTRY
NdrConformantVaryingStructMemorySize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned long
RPC_ENTRY
NdrComplexStructMemorySize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

/* Arrays */

RPCRTAPI
unsigned long
RPC_ENTRY
NdrFixedArrayMemorySize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned long
RPC_ENTRY
NdrConformantArrayMemorySize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned long
RPC_ENTRY
NdrConformantVaryingArrayMemorySize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned long
RPC_ENTRY
NdrVaryingArrayMemorySize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned long
RPC_ENTRY
NdrComplexArrayMemorySize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

/* Strings */

RPCRTAPI
unsigned long
RPC_ENTRY
NdrConformantStringMemorySize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned long
RPC_ENTRY
NdrNonConformantStringMemorySize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

/* Unions */

RPCRTAPI
unsigned long
RPC_ENTRY
NdrEncapsulatedUnionMemorySize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
unsigned long
RPC_ENTRY
NdrNonEncapsulatedUnionMemorySize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

/* Transmit as and represent as*/

RPCRTAPI
unsigned long
RPC_ENTRY
NdrXmitOrRepAsMemorySize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

/* User_marshal */

RPCRTAPI
unsigned long
RPC_ENTRY
NdrUserMarshalMemorySize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

/* Cairo Interface pointer */

RPCRTAPI
unsigned long
RPC_ENTRY
NdrInterfacePointerMemorySize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

/*
 * Freeing routines
 */

RPCRTAPI
void
RPC_ENTRY
NdrPointerFree(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Structures */

RPCRTAPI
void
RPC_ENTRY
NdrSimpleStructFree(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
void
RPC_ENTRY
NdrConformantStructFree(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
void
RPC_ENTRY
NdrConformantVaryingStructFree(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
void
RPC_ENTRY
NdrComplexStructFree(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Arrays */

RPCRTAPI
void
RPC_ENTRY
NdrFixedArrayFree(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
void
RPC_ENTRY
NdrConformantArrayFree(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
void
RPC_ENTRY
NdrConformantVaryingArrayFree(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
void
RPC_ENTRY
NdrVaryingArrayFree(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
void
RPC_ENTRY
NdrComplexArrayFree(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Unions */

RPCRTAPI
void
RPC_ENTRY
NdrEncapsulatedUnionFree(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

RPCRTAPI
void
RPC_ENTRY
NdrNonEncapsulatedUnionFree(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Byte count */

RPCRTAPI
void
RPC_ENTRY
NdrByteCountPointerFree(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Transmit as and represent as*/

RPCRTAPI
void
RPC_ENTRY
NdrXmitOrRepAsFree(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* User_marshal */

RPCRTAPI
void
RPC_ENTRY
NdrUserMarshalFree(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/* Cairo Interface pointer */

RPCRTAPI
void
RPC_ENTRY
NdrInterfacePointerFree(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pMemory,
    PFORMAT_STRING                      pFormat
    );

/*
 * Endian conversion routine.
 */

RPCRTAPI
void
RPC_ENTRY
NdrConvert2(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat,
    long                                NumberParams
    );

RPCRTAPI
void
RPC_ENTRY
NdrConvert(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pFormat
    );

#define USER_MARSHAL_FC_BYTE         1
#define USER_MARSHAL_FC_CHAR         2
#define USER_MARSHAL_FC_SMALL        3
#define USER_MARSHAL_FC_USMALL       4
#define USER_MARSHAL_FC_WCHAR        5
#define USER_MARSHAL_FC_SHORT        6
#define USER_MARSHAL_FC_USHORT       7
#define USER_MARSHAL_FC_LONG         8
#define USER_MARSHAL_FC_ULONG        9
#define USER_MARSHAL_FC_FLOAT       10
#define USER_MARSHAL_FC_HYPER       11
#define USER_MARSHAL_FC_DOUBLE      12

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrUserMarshalSimpleTypeConvert(
    unsigned long * pFlags,
    unsigned char * pBuffer,
    unsigned char   FormatChar
    );

/*
 * Auxilary routines
 */

RPCRTAPI
void
RPC_ENTRY
NdrClientInitializeNew(
    PRPC_MESSAGE                        pRpcMsg,
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PMIDL_STUB_DESC                     pStubDescriptor,
    unsigned int                        ProcNum
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrServerInitializeNew(
    PRPC_MESSAGE                        pRpcMsg,
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PMIDL_STUB_DESC                     pStubDescriptor
    );

RPCRTAPI
void
RPC_ENTRY
NdrServerInitializePartial(
    PRPC_MESSAGE                        pRpcMsg,
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PMIDL_STUB_DESC                     pStubDescriptor,
    unsigned long                       RequestedBufferSize
    );

RPCRTAPI
void
RPC_ENTRY
NdrClientInitialize(
    PRPC_MESSAGE                        pRpcMsg,
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PMIDL_STUB_DESC                     pStubDescriptor,
    unsigned int                        ProcNum
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrServerInitialize(
    PRPC_MESSAGE                        pRpcMsg,
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PMIDL_STUB_DESC                     pStubDescriptor
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrServerInitializeUnmarshall (
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PMIDL_STUB_DESC                     pStubDescriptor,
    PRPC_MESSAGE                        pRpcMsg
    );

RPCRTAPI
void
RPC_ENTRY
NdrServerInitializeMarshall (
    PRPC_MESSAGE                        pRpcMsg,
    PMIDL_STUB_MESSAGE                  pStubMsg
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrGetBuffer(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned long                       BufferLength,
    RPC_BINDING_HANDLE                  Handle
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrNsGetBuffer(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned long                       BufferLength,
    RPC_BINDING_HANDLE                  Handle
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrGetPipeBuffer(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned long                       BufferLength,
    RPC_BINDING_HANDLE                  Handle );

RPCRTAPI
void
RPC_ENTRY
NdrGetPartialBuffer(
    PMIDL_STUB_MESSAGE                  pStubMsg );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrSendReceive(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR*            pBufferEnd
    );

RPCRTAPI
unsigned char __RPC_FAR *
RPC_ENTRY
NdrNsSendReceive(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char __RPC_FAR *           pBufferEnd,
    RPC_BINDING_HANDLE __RPC_FAR *      pAutoHandle
    );

RPCRTAPI
void
RPC_ENTRY
NdrPipeSendReceive(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PNDR_PIPE_DESC                      pPipeDesc
    );

RPCRTAPI
void
RPC_ENTRY
NdrFreeBuffer(
    PMIDL_STUB_MESSAGE                  pStubMsg
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
NdrGetDcomProtocolVersion(
    PMIDL_STUB_MESSAGE   pStubMsg,
    RPC_VERSION *        pVersion );


/*
 * Pipe specific calls
 */

RPCRTAPI
void
RPC_ENTRY
NdrPipesInitialize(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    PFORMAT_STRING                      pParamDesc,
    PNDR_PIPE_DESC                      pPipeDesc,
    PNDR_PIPE_MESSAGE                   pPipeMsg,
    char             __RPC_FAR *        pStackTop,
    unsigned long                       NumberParams );

RPCRTAPI
void
RPC_ENTRY
NdrPipePull(
    char          __RPC_FAR *           pState,
    void          __RPC_FAR *           buf,
    unsigned long                       esize,
    unsigned long __RPC_FAR *           ecount );

RPCRTAPI
void
RPC_ENTRY
NdrPipePush(
    char          __RPC_FAR *           pState,
    void          __RPC_FAR *           buf,
    unsigned long                       ecount );

RPCRTAPI
void
RPC_ENTRY
NdrIsAppDoneWithPipes(
    PNDR_PIPE_DESC                      pPipeDesc
    );

RPCRTAPI
void
RPC_ENTRY
NdrPipesDone(
    PMIDL_STUB_MESSAGE                  pStubMsg
    );


/*
 * Interpeter calls.
 */

/* client */

CLIENT_CALL_RETURN RPC_VAR_ENTRY
NdrClientCall2(
    PMIDL_STUB_DESC                     pStubDescriptor,
    PFORMAT_STRING                      pFormat,
    ...
    );

CLIENT_CALL_RETURN RPC_VAR_ENTRY
NdrClientCall(
    PMIDL_STUB_DESC                     pStubDescriptor,
    PFORMAT_STRING                      pFormat,
    ...
    );

CLIENT_CALL_RETURN RPC_VAR_ENTRY
NdrAsyncClientCall(
    PMIDL_STUB_DESC     pStubDescriptor,
    PFORMAT_STRING      pFormat,
    ...
    );

CLIENT_CALL_RETURN RPC_VAR_ENTRY
NdrDcomAsyncClientCall(
    PMIDL_STUB_DESC     pStubDescriptor,
    PFORMAT_STRING      pFormat,
    ...
    );

/* server */
typedef enum {
    STUB_UNMARSHAL,
    STUB_CALL_SERVER,
    STUB_MARSHAL,
    STUB_CALL_SERVER_NO_HRESULT
}STUB_PHASE;

typedef enum {
    PROXY_CALCSIZE,
    PROXY_GETBUFFER,
    PROXY_MARSHAL,
    PROXY_SENDRECEIVE,
    PROXY_UNMARSHAL
}PROXY_PHASE;

struct IRpcStubBuffer;      // Forward declaration

// Raw RPC only
RPCRTAPI
void
RPC_ENTRY
NdrAsyncServerCall(
    PRPC_MESSAGE                        pRpcMsg
    );

// old dcom async scheme
RPCRTAPI
long
RPC_ENTRY
NdrAsyncStubCall(
    struct IRpcStubBuffer *             pThis,
    struct IRpcChannelBuffer *          pChannel,
    PRPC_MESSAGE                        pRpcMsg,
    unsigned long *                     pdwStubPhase
    );

// async uuid
RPCRTAPI
long
RPC_ENTRY
NdrDcomAsyncStubCall(
    struct IRpcStubBuffer            *  pThis,
    struct IRpcChannelBuffer         *  pChannel,
    PRPC_MESSAGE                        pRpcMsg,
    unsigned long                    *  pdwStubPhase
    );

RPCRTAPI
long
RPC_ENTRY
NdrStubCall2(
    struct IRpcStubBuffer __RPC_FAR *    pThis,
    struct IRpcChannelBuffer __RPC_FAR * pChannel,
    PRPC_MESSAGE                         pRpcMsg,
    unsigned long __RPC_FAR *            pdwStubPhase
    );

RPCRTAPI
void
RPC_ENTRY
NdrServerCall2(
    PRPC_MESSAGE                        pRpcMsg
    );

RPCRTAPI
long
RPC_ENTRY
NdrStubCall (
    struct IRpcStubBuffer __RPC_FAR *    pThis,
    struct IRpcChannelBuffer __RPC_FAR * pChannel,
    PRPC_MESSAGE                         pRpcMsg,
    unsigned long __RPC_FAR *            pdwStubPhase
    );

RPCRTAPI
void
RPC_ENTRY
NdrServerCall(
    PRPC_MESSAGE                        pRpcMsg
    );

RPCRTAPI
int
RPC_ENTRY
NdrServerUnmarshall(
    struct IRpcChannelBuffer __RPC_FAR * pChannel,
    PRPC_MESSAGE                         pRpcMsg,
    PMIDL_STUB_MESSAGE                   pStubMsg,
    PMIDL_STUB_DESC                      pStubDescriptor,
    PFORMAT_STRING                       pFormat,
    void __RPC_FAR *                     pParamList
    );

RPCRTAPI
void
RPC_ENTRY
NdrServerMarshall(
    struct IRpcStubBuffer __RPC_FAR *    pThis,
    struct IRpcChannelBuffer __RPC_FAR * pChannel,
    PMIDL_STUB_MESSAGE                   pStubMsg,
    PFORMAT_STRING                       pFormat
    );

/* Comm and Fault status */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
NdrMapCommAndFaultStatus(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned long __RPC_FAR *                       pCommStatus,
    unsigned long __RPC_FAR *                       pFaultStatus,
    RPC_STATUS                          Status
    );

/* Helper routines */

RPCRTAPI
int
RPC_ENTRY
NdrSH_UPDecision(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char           __RPC_FAR *__RPC_FAR *          pPtrInMem,
    RPC_BUFPTR                          pBuffer
    );

RPCRTAPI
int
RPC_ENTRY
NdrSH_TLUPDecision(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char           __RPC_FAR *__RPC_FAR *          pPtrInMem
    );

RPCRTAPI
int
RPC_ENTRY
NdrSH_TLUPDecisionBuffer(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char           __RPC_FAR *__RPC_FAR *          pPtrInMem
    );

RPCRTAPI
int
RPC_ENTRY
NdrSH_IfAlloc(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char           __RPC_FAR *__RPC_FAR *          pPtrInMem,
    unsigned long                       Count
    );

RPCRTAPI
int
RPC_ENTRY
NdrSH_IfAllocRef(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char           __RPC_FAR *__RPC_FAR *          pPtrInMem,
    unsigned long                       Count
    );

RPCRTAPI
int
RPC_ENTRY
NdrSH_IfAllocSet(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char           __RPC_FAR *__RPC_FAR *          pPtrInMem,
    unsigned long                       Count
    );

RPCRTAPI
RPC_BUFPTR
RPC_ENTRY
NdrSH_IfCopy(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char           __RPC_FAR *__RPC_FAR *          pPtrInMem,
    unsigned long                       Count
    );

RPCRTAPI
RPC_BUFPTR
RPC_ENTRY
NdrSH_IfAllocCopy(
    PMIDL_STUB_MESSAGE                  pStubMsg,
    unsigned char           __RPC_FAR *__RPC_FAR *          pPtrInMem,
    unsigned long                       Count
    );

RPCRTAPI
unsigned long
RPC_ENTRY
NdrSH_Copy(
    unsigned char           __RPC_FAR *         pStubMsg,
    unsigned char           __RPC_FAR *         pPtrInMem,
    unsigned long                       Count
    );

RPCRTAPI
void
RPC_ENTRY
NdrSH_IfFree(
    PMIDL_STUB_MESSAGE                  pMessage,
    unsigned char           __RPC_FAR *         pPtr );


RPCRTAPI
RPC_BUFPTR
RPC_ENTRY
NdrSH_StringMarshall(
    PMIDL_STUB_MESSAGE                  pMessage,
    unsigned char           __RPC_FAR *         pMemory,
    unsigned long                       Count,
    int                                 Size );

RPCRTAPI
RPC_BUFPTR
RPC_ENTRY
NdrSH_StringUnMarshall(
    PMIDL_STUB_MESSAGE                  pMessage,
    unsigned char           __RPC_FAR *__RPC_FAR *          pMemory,
    int                                 Size );

/****************************************************************************
    MIDL 2.0 memory package: rpc_ss_* rpc_sm_*
 ****************************************************************************/

typedef void __RPC_FAR * RPC_SS_THREAD_HANDLE;

typedef void __RPC_FAR * __RPC_API
RPC_CLIENT_ALLOC (
    IN size_t Size
    );

typedef void __RPC_API
RPC_CLIENT_FREE (
    IN void __RPC_FAR * Ptr
    );

/*++
     RpcSs* package
--*/

RPCRTAPI
void __RPC_FAR *
RPC_ENTRY
RpcSsAllocate (
    IN size_t Size
    );

RPCRTAPI
void
RPC_ENTRY
RpcSsDisableAllocate (
    void
    );

RPCRTAPI
void
RPC_ENTRY
RpcSsEnableAllocate (
    void
    );

RPCRTAPI
void
RPC_ENTRY
RpcSsFree (
    IN void __RPC_FAR * NodeToFree
    );

RPCRTAPI
RPC_SS_THREAD_HANDLE
RPC_ENTRY
RpcSsGetThreadHandle (
    void
    );

RPCRTAPI
void
RPC_ENTRY
RpcSsSetClientAllocFree (
    IN RPC_CLIENT_ALLOC __RPC_FAR * ClientAlloc,
    IN RPC_CLIENT_FREE __RPC_FAR * ClientFree
    );

RPCRTAPI
void
RPC_ENTRY
RpcSsSetThreadHandle (
    IN RPC_SS_THREAD_HANDLE Id
    );

RPCRTAPI
void
RPC_ENTRY
RpcSsSwapClientAllocFree (
    IN RPC_CLIENT_ALLOC __RPC_FAR * ClientAlloc,
    IN RPC_CLIENT_FREE __RPC_FAR * ClientFree,
    OUT RPC_CLIENT_ALLOC __RPC_FAR * __RPC_FAR * OldClientAlloc,
    OUT RPC_CLIENT_FREE __RPC_FAR * __RPC_FAR * OldClientFree
    );

/*++
     RpcSm* package
--*/

RPCRTAPI
void __RPC_FAR *
RPC_ENTRY
RpcSmAllocate (
    IN  size_t          Size,
    OUT RPC_STATUS __RPC_FAR *    pStatus
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcSmClientFree (
    IN  void __RPC_FAR * pNodeToFree
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcSmDestroyClientContext (
    IN void __RPC_FAR * __RPC_FAR * ContextHandle
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcSmDisableAllocate (
    void
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcSmEnableAllocate (
    void
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcSmFree (
    IN void __RPC_FAR * NodeToFree
    );

RPCRTAPI
RPC_SS_THREAD_HANDLE
RPC_ENTRY
RpcSmGetThreadHandle (
    OUT RPC_STATUS __RPC_FAR *    pStatus
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcSmSetClientAllocFree (
    IN RPC_CLIENT_ALLOC __RPC_FAR * ClientAlloc,
    IN RPC_CLIENT_FREE __RPC_FAR * ClientFree
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcSmSetThreadHandle (
    IN RPC_SS_THREAD_HANDLE Id
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcSmSwapClientAllocFree (
    IN RPC_CLIENT_ALLOC __RPC_FAR * ClientAlloc,
    IN RPC_CLIENT_FREE __RPC_FAR * ClientFree,
    OUT RPC_CLIENT_ALLOC __RPC_FAR * __RPC_FAR * OldClientAlloc,
    OUT RPC_CLIENT_FREE __RPC_FAR * __RPC_FAR * OldClientFree
    );

/*++
     Ndr stub entry points
--*/

RPCRTAPI
void
RPC_ENTRY
NdrRpcSsEnableAllocate(
    PMIDL_STUB_MESSAGE      pMessage );

RPCRTAPI
void
RPC_ENTRY
NdrRpcSsDisableAllocate(
    PMIDL_STUB_MESSAGE      pMessage );

RPCRTAPI
void
RPC_ENTRY
NdrRpcSmSetClientToOsf(
    PMIDL_STUB_MESSAGE      pMessage );

RPCRTAPI
void __RPC_FAR *
RPC_ENTRY
NdrRpcSmClientAllocate (
    IN size_t Size
    );

RPCRTAPI
void
RPC_ENTRY
NdrRpcSmClientFree (
    IN void __RPC_FAR * NodeToFree
    );

RPCRTAPI
void __RPC_FAR *
RPC_ENTRY
NdrRpcSsDefaultAllocate (
    IN size_t Size
    );

RPCRTAPI
void
RPC_ENTRY
NdrRpcSsDefaultFree (
    IN void __RPC_FAR * NodeToFree
    );

/****************************************************************************
    end of memory package: rpc_ss_* rpc_sm_*
 ****************************************************************************/

/****************************************************************************
 * Full Pointer APIs
 ****************************************************************************/

RPCRTAPI
PFULL_PTR_XLAT_TABLES
RPC_ENTRY
NdrFullPointerXlatInit(
    unsigned long           NumberOfPointers,
    XLAT_SIDE               XlatSide
    );

RPCRTAPI
void
RPC_ENTRY
NdrFullPointerXlatFree(
    PFULL_PTR_XLAT_TABLES   pXlatTables
    );

RPCRTAPI
int
RPC_ENTRY
NdrFullPointerQueryPointer(
    PFULL_PTR_XLAT_TABLES   pXlatTables,
    void __RPC_FAR *                    pPointer,
    unsigned char           QueryType,
    unsigned long __RPC_FAR *           pRefId
    );

RPCRTAPI
int
RPC_ENTRY
NdrFullPointerQueryRefId(
    PFULL_PTR_XLAT_TABLES   pXlatTables,
    unsigned long           RefId,
    unsigned char           QueryType,
    void __RPC_FAR *__RPC_FAR *                 ppPointer
    );

RPCRTAPI
void
RPC_ENTRY
NdrFullPointerInsertRefId(
    PFULL_PTR_XLAT_TABLES   pXlatTables,
    unsigned long           RefId,
    void __RPC_FAR *                    pPointer
    );

RPCRTAPI
int
RPC_ENTRY
NdrFullPointerFree(
    PFULL_PTR_XLAT_TABLES   pXlatTables,
    void __RPC_FAR *                    Pointer
    );

RPCRTAPI
void __RPC_FAR *
RPC_ENTRY
NdrAllocate(
    PMIDL_STUB_MESSAGE      pStubMsg,
    size_t                  Len
    );

RPCRTAPI
void
RPC_ENTRY
NdrClearOutParameters(
    PMIDL_STUB_MESSAGE      pStubMsg,
    PFORMAT_STRING          pFormat,
    void __RPC_FAR *        ArgAddr
    );


/****************************************************************************
 * Proxy APIs
 ****************************************************************************/

RPCRTAPI
void __RPC_FAR *
RPC_ENTRY
NdrOleAllocate (
    IN size_t Size
    );

RPCRTAPI
void
RPC_ENTRY
NdrOleFree (
    IN void __RPC_FAR * NodeToFree
    );

#ifdef CONST_VTABLE
#define CONST_VTBL const
#else
#define CONST_VTBL
#endif

/****************************************************************************
 * Special things for VC5 Com support
 ****************************************************************************/

#ifndef DECLSPEC_SELECTANY
#if (_MSC_VER >= 1100)
#define DECLSPEC_SELECTANY __declspec(selectany)
#else
#define DECLSPEC_SELECTANY
#endif
#endif

#ifndef DECLSPEC_NOVTABLE
#if (_MSC_VER >= 1100) && defined(__cplusplus)
#define DECLSPEC_NOVTABLE __declspec(novtable)
#else
#define DECLSPEC_NOVTABLE
#endif
#endif

#ifndef DECLSPEC_UUID
#if (_MSC_VER >= 1100) && defined(__cplusplus)
#define DECLSPEC_UUID(x) __declspec(uuid(x))
#else
#define DECLSPEC_UUID(x)
#endif
#endif

#define MIDL_INTERFACE(x)   struct DECLSPEC_UUID(x) DECLSPEC_NOVTABLE

#if _MSC_VER >= 1100
#define EXTERN_GUID(itf,l1,s1,s2,c1,c2,c3,c4,c5,c6,c7,c8)  \
  EXTERN_C const IID DECLSPEC_SELECTANY itf = {l1,s1,s2,{c1,c2,c3,c4,c5,c6,c7,c8}}
#else
#define EXTERN_GUID(itf,l1,s1,s2,c1,c2,c3,c4,c5,c6,c7,c8) EXTERN_C const IID itf
#endif

/****************************************************************************
 * UserMarshal information
 ****************************************************************************/

typedef struct _NDR_USER_MARSHAL_INFO_LEVEL1
{
    void *Buffer;
    unsigned long BufferSize;
    void *(__RPC_API * pfnAllocate)(size_t);
    void (__RPC_API * pfnFree)(void *);
    struct IRpcChannelBuffer * pRpcChannelBuffer;
    ULONG_PTR Reserved[5];
} NDR_USER_MARSHAL_INFO_LEVEL1;

#if !defined( RC_INVOKED )
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4201)
#endif

typedef struct _NDR_USER_MARSHAL_INFO 
{
    unsigned long InformationLevel;
    union {
        NDR_USER_MARSHAL_INFO_LEVEL1 Level1;
    };
} NDR_USER_MARSHAL_INFO;       

#if !defined( RC_INVOKED )
#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4201)
#endif
#endif

RPC_STATUS
RPC_ENTRY
NdrGetUserMarshalInfo (
    IN unsigned long *pFlags,
    IN unsigned long InformationLevel,
    OUT NDR_USER_MARSHAL_INFO *pMarshalInfo
    );

#ifdef __cplusplus
}
#endif

#if defined(__RPC_WIN64__)
#include <poppack.h>
#endif

// Reset the packing level for DOS, Windows and Mac.

#if defined(__RPC_DOS__) || defined(__RPC_WIN16__) || defined(__RPC_MAC__)
#pragma pack()
#endif

#endif /* __RPCNDR_H__ */

