/***
*malloc.h - declarations and definitions for memory allocation functions
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Contains the function declarations for memory allocation functions;
*       also defines manifest constants and types used by the heap routines.
*       [System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_MALLOC
#define _INC_MALLOC

#include <limits.h>

#ifdef  _MSC_VER
/*
 * Currently, all MS C compilers for Win32 platforms default to 8 byte
 * alignment.
 */
#pragma pack(push,_CRT_PACKING)
#endif  /* _MSC_VER */

#ifdef  __cplusplus
extern "C" {
#endif

/* Maximum heap request the heap manager will attempt */

#ifdef  _WIN64
#define _HEAP_MAXREQ    0xFFFFFFFFFFFFFFE0
#else
#define _HEAP_MAXREQ    0xFFFFFFE0
#endif

/* _STATIC_ASSERT is for enforcing boolean/integral conditions at compile time. */

#ifndef _STATIC_ASSERT
#define _STATIC_ASSERT(expr) typedef char __static_assert_t[ (expr) ]
#endif

/* Constants for _heapchk/_heapset/_heapwalk routines */

#define _HEAPEMPTY      (-1)
#define _HEAPOK         (-2)
#define _HEAPBADBEGIN   (-3)
#define _HEAPBADNODE    (-4)
#define _HEAPEND        (-5)
#define _HEAPBADPTR     (-6)
#define _FREEENTRY      0
#define _USEDENTRY      1

#ifndef _HEAPINFO_DEFINED
typedef struct _heapinfo {
        int * _pentry;
        size_t _size;
        int _useflag;
        } _HEAPINFO;
#define _HEAPINFO_DEFINED
#endif

/* External variable declarations */
#if !defined(_M_CEE_PURE)
extern _CRT_INSECURE_DEPRECATE_GLOBALS(_get_amblksiz) unsigned int _amblksiz;
#else
_CRT_INSECURE_DEPRECATE_GLOBALS(_get_amblksiz) _CRTIMP unsigned int * __cdecl __p__amblksiz(void);
#define _amblksiz (*__p__amblksiz())
#endif /* !defined(_M_CEE_PURE) */

#define _mm_free(a)      _aligned_free(a)
#define _mm_malloc(a, b)    _aligned_malloc(a, b)

/* Function prototypes */

#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
#pragma push_macro("calloc")
#pragma push_macro("free")
#pragma push_macro("malloc")
#pragma push_macro("realloc")
#pragma push_macro("_recalloc")
#pragma push_macro("_aligned_free")
#pragma push_macro("_aligned_malloc")
#pragma push_macro("_aligned_offset_malloc")
#pragma push_macro("_aligned_realloc")
#pragma push_macro("_aligned_recalloc")
#pragma push_macro("_aligned_offset_realloc")
#pragma push_macro("_aligned_offset_recalloc")
#pragma push_macro("_freea")
#undef calloc
#undef free
#undef malloc
#undef realloc
#undef _recalloc
#undef _aligned_free
#undef _aligned_malloc
#undef _aligned_offset_malloc
#undef _aligned_realloc
#undef _aligned_recalloc
#undef _aligned_offset_realloc
#undef _aligned_offset_recalloc
#undef _freea
#endif

#ifndef _CRT_ALLOCATION_DEFINED
#define _CRT_ALLOCATION_DEFINED
_CRTIMP _CRT_JIT_INTRINSIC  _CRTNOALIAS _CRTRESTRICT __checkReturn __out_bcount_opt(_NumOfElements* _SizeOfElements)    void * __cdecl calloc(__in size_t _NumOfElements, __in size_t _SizeOfElements);
_CRTIMP                     _CRTNOALIAS                                                                             void   __cdecl free(__inout_opt void * _Memory);
_CRTIMP _CRT_JIT_INTRINSIC  _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_Size)                              void * __cdecl malloc(__in size_t _Size);
_CRTIMP                     _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_NewSize)                           void * __cdecl realloc(__in_opt void * _Memory, __in size_t _NewSize);
_CRTIMP                     _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_Size*_Count)                       void * __cdecl _recalloc(__in_opt void * _Memory, __in size_t _Count, __in size_t _Size);
_CRTIMP                     _CRTNOALIAS                                                                             void   __cdecl _aligned_free(__inout_opt void * _Memory);
_CRTIMP                     _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_Size)                              void * __cdecl _aligned_malloc(__in size_t _Size, __in size_t _Alignment);
_CRTIMP                     _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_Size)                              void * __cdecl _aligned_offset_malloc(__in size_t _Size, __in size_t _Alignment, __in size_t _Offset);
_CRTIMP                     _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_Size)                              void * __cdecl _aligned_realloc(__in_opt void * _Memory, __in size_t _Size, __in size_t _Alignment);
_CRTIMP                     _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_Size*_Count)                       void * __cdecl _aligned_recalloc(__in_opt void * _Memory, __in size_t _Count, __in size_t _Size, __in size_t _Alignment);
_CRTIMP                     _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_Size)                              void * __cdecl _aligned_offset_realloc(__in_opt void * _Memory, __in size_t _Size, __in size_t _Alignment, __in size_t _Offset);
_CRTIMP                     _CRTNOALIAS _CRTRESTRICT __checkReturn __bcount_opt(_Size*_Count)                       void * __cdecl _aligned_offset_recalloc(__in_opt void * _Memory, __in size_t _Count, __in size_t _Size, __in size_t _Alignment, __in size_t _Offset);
#endif

#if (defined(_DEBUG) || defined(_SYSCRT_DEBUG)) && defined(_CRTDBG_MAP_ALLOC)
#pragma pop_macro("calloc")
#pragma pop_macro("free")
#pragma pop_macro("malloc")
#pragma pop_macro("realloc")
#pragma pop_macro("_recalloc")
#pragma pop_macro("_aligned_free")
#pragma pop_macro("_aligned_malloc")
#pragma pop_macro("_aligned_offset_malloc")
#pragma pop_macro("_aligned_realloc")
#pragma pop_macro("_aligned_recalloc")
#pragma pop_macro("_aligned_offset_realloc")
#pragma pop_macro("_aligned_offset_recalloc")
#pragma pop_macro("_freea")
#endif

int     __cdecl _resetstkoflw (void);
int __cdecl _resetstkoflw_downlevel(void);

#define _MAX_WAIT_MALLOC_CRT 60000

unsigned long __cdecl _set_malloc_crt_max_wait(__in unsigned long _NewValue);

#if (defined(_DEBUG) || defined(_SYSCRT_DEBUG)) && defined(_CRTDBG_MAP_ALLOC)
#pragma push_macro("_expand")
#pragma push_macro("_msize")
#undef _expand
#undef _msize
#endif

_CRTIMP __checkReturn __bcount_opt(_NewSize) void *  __cdecl _expand(__in void * _Memory, __in size_t _NewSize);
_CRTIMP __checkReturn size_t  __cdecl _msize(__in void * _Memory);

#if (defined(_DEBUG) || defined(_SYSCRT_DEBUG)) && defined(_CRTDBG_MAP_ALLOC)
#pragma pop_macro("_expand")
#pragma pop_macro("_msize")
#endif

__bcount(_Size) void *          __cdecl _alloca(__in size_t _Size);
_CRTIMP __checkReturn size_t  __cdecl _get_sbh_threshold(void);
_CRTIMP int     __cdecl _set_sbh_threshold(__in size_t _NewValue);
_CRTIMP errno_t __cdecl _set_amblksiz(__in size_t _Value);
_CRTIMP errno_t __cdecl _get_amblksiz(__out size_t * _Value);
_CRTIMP __checkReturn int     __cdecl _heapadd(__in void * _Memory, __in size_t _Size);
_CRTIMP __checkReturn int     __cdecl _heapchk(void);
_CRTIMP __checkReturn int     __cdecl _heapmin(void);
_CRTIMP int     __cdecl _heapset(__in unsigned int _Fill);
_CRTIMP _CRT_MANAGED_HEAP_DEPRECATE int     __cdecl _heapwalk(__inout _HEAPINFO * _EntryInfo);
_CRTIMP size_t  __cdecl _heapused(size_t * _Used, size_t * _Commit);

_CRTIMP intptr_t __cdecl _get_heap_handle(void);

#define _ALLOCA_S_THRESHOLD     1024
#define _ALLOCA_S_STACK_MARKER  0xCCCC
#define _ALLOCA_S_HEAP_MARKER   0xDDDD

#if defined(_M_IX86)
#define _ALLOCA_S_MARKER_SIZE   8
#elif defined(_M_IA64)
#define _ALLOCA_S_MARKER_SIZE   16
#elif defined(_M_AMD64)
#define _ALLOCA_S_MARKER_SIZE   16
#endif

_STATIC_ASSERT(sizeof(unsigned int) <= _ALLOCA_S_MARKER_SIZE);

#if !defined(__midl) && !defined(RC_INVOKED)
__inline void *_MarkAllocaS(__out_opt __crt_typefix(unsigned int*) void *_Ptr, unsigned int _Marker)
{
    if (_Ptr)
    {
        *((unsigned int*)_Ptr) = _Marker;
        _Ptr = (char*)_Ptr + _ALLOCA_S_MARKER_SIZE;
    }
    return _Ptr;
}
#endif

#if defined(_DEBUG) || defined(_SYSCRT_DEBUG)
#if !defined(_CRTDBG_MAP_ALLOC)
#undef _malloca
#define _malloca(size) \
__pragma(warning(suppress: 6255)) \
        _MarkAllocaS(malloc((size) + _ALLOCA_S_MARKER_SIZE), _ALLOCA_S_HEAP_MARKER)
#endif
#else
#undef _malloca
#define _malloca(size) \
__pragma(warning(suppress: 6255)) \
    ((((size) + _ALLOCA_S_MARKER_SIZE) <= _ALLOCA_S_THRESHOLD) ? \
        _MarkAllocaS(_alloca((size) + _ALLOCA_S_MARKER_SIZE), _ALLOCA_S_STACK_MARKER) : \
        _MarkAllocaS(malloc((size) + _ALLOCA_S_MARKER_SIZE), _ALLOCA_S_HEAP_MARKER))
#endif

#undef _alloca_s
#define _alloca_s _malloca

#undef _FREEA_INLINE
#define _FREEA_INLINE

#ifdef _FREEA_INLINE
/* _freea must be in the header so that its allocator matches _malloca */
#if !defined(__midl) && !defined(RC_INVOKED)
#undef _freea
_CRTNOALIAS __inline void __CRTDECL _freea(__inout_opt void * _Memory)
{
    unsigned int _Marker;
    if (_Memory)
    {
        _Memory = (char*)_Memory - _ALLOCA_S_MARKER_SIZE;
        _Marker = *(unsigned int *)_Memory;
        if (_Marker == _ALLOCA_S_HEAP_MARKER)
        {
            free(_Memory);
        }
#if defined(_ASSERTE)
        else if (_Marker != _ALLOCA_S_STACK_MARKER)
        {
            _ASSERTE(("Corrupted pointer passed to _freea", 0));
        }
#endif
    }
}
#endif
#endif

#if     !__STDC__
/* Non-ANSI names for compatibility */
#define alloca  _alloca
#endif  /* __STDC__*/

#ifdef  HEAPHOOK
#ifndef _HEAPHOOK_DEFINED
/* hook function type */
typedef int (__cdecl * _HEAPHOOK)(int, size_t, void *, void **);
#define _HEAPHOOK_DEFINED
#endif  /* _HEAPHOOK_DEFINED */

/* set hook function */
_CRTIMP _HEAPHOOK __cdecl _setheaphook(__in_opt _HEAPHOOK _NewHook);

/* hook function must handle these types */
#define _HEAP_MALLOC    1
#define _HEAP_CALLOC    2
#define _HEAP_FREE      3
#define _HEAP_REALLOC   4
#define _HEAP_MSIZE     5
#define _HEAP_EXPAND    6
#endif  /* HEAPHOOK */


#ifdef  __cplusplus
}
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_MALLOC */
/* 88bf0570-3001-4e78-a5f2-be5765546192 */ 

