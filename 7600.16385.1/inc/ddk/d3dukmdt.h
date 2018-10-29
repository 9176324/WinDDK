/******************************Module*Header************************************\
*
* Module Name: d3dukmdt.h
*
* Content: Longhorn Display Driver Model (LDDM) user/kernel mode
*          shared data type definitions.
*
* Copyright (c) 2003 Microsoft Corporation.  All rights reserved.
\*******************************************************************************/
#ifndef _D3DUKMDT_H_
#define _D3DUKMDT_H_

#if !defined(_D3DKMDT_H)       && \
    !defined(_D3DKMTHK_H_)     && \
    !defined(_D3DUMDDI_H_)     && \
    !defined(__DXGKRNLETW_H__)
   #error This header should not be included directly!
#endif

#pragma warning(push)
#pragma warning(disable:4201) // anonymous unions warning


//
// WDDM DDI Interface Version
//

#define DXGKDDI_INTERFACE_VERSION_VISTA     0x1052
#define DXGKDDI_INTERFACE_VERSION_VISTA_SP1 0x1053
#define DXGKDDI_INTERFACE_VERSION_WIN7      0x2005

#if !defined(DXGKDDI_INTERFACE_VERSION)
#define DXGKDDI_INTERFACE_VERSION           DXGKDDI_INTERFACE_VERSION_WIN7
#endif // !defined(DXGKDDI_INTERFACE_VERSION)

#define D3D_UMD_INTERFACE_VERSION_VISTA     0x000C
#define D3D_UMD_INTERFACE_VERSION_WIN7      0x2003

#if !defined(D3D_UMD_INTERFACE_VERSION)
#define D3D_UMD_INTERFACE_VERSION           D3D_UMD_INTERFACE_VERSION_WIN7
#endif // !defined(D3D_UMD_INTERFACE_VERSION)

//
// Available only for Vista (LONGHORN) and later and for
// multiplatform tools such as debugger extensions
//
#if (NTDDI_VERSION >= NTDDI_LONGHORN) || defined(D3DKMDT_SPECIAL_MULTIPLATFORM_TOOL)

typedef ULONGLONG D3DGPU_VIRTUAL_ADDRESS;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Purpose: Video present source unique identification number descriptor type
//

typedef UINT  D3DDDI_VIDEO_PRESENT_SOURCE_ID;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Purpose: Video present source unique identification number descriptor type.
//
typedef UINT  D3DDDI_VIDEO_PRESENT_TARGET_ID;

//
// DDI level handle that represents a kernel mode object (allocation, device, etc)
//
typedef UINT D3DKMT_HANDLE;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Purpose: Video present target mode fractional frequency descriptor type.
//
// Remarks: Fractional value used to represent vertical and horizontal frequencies of a video mode
//          (i.e. VSync and HSync). Vertical frequencies are stored in Hz. Horizontal frequencies
//          are stored in Hz.
//          The dynamic range of this encoding format, given 10^-7 resolution is {0..(2^32 - 1) / 10^7},
//          which translates to {0..428.4967296} [Hz] for vertical frequencies and {0..428.4967296} [Hz]
//          for horizontal frequencies. This sub-microseconds precision range should be acceptable even
//          for a pro-video application (error in one microsecond for video signal synchronization would
//          imply a time drift with a cycle of 10^7/(60*60*24) = 115.741 days.
//
//          If rational number with a finite fractional sequence, use denominator of form 10^(length of fractional sequence).
//          If rational number without a finite fractional sequence, or a sequence exceeding the precision allowed by the
//          dynamic range of the denominator, or an irrational number, use an appropriate ratio of integers which best
//          represents the value.
//
typedef struct _D3DDDI_RATIONAL
{
    UINT    Numerator;
    UINT    Denominator;
} D3DDDI_RATIONAL;

typedef struct _D3DDDI_ALLOCATIONINFO
{
    D3DKMT_HANDLE                   hAllocation;           // out: Private driver data for allocation
    CONST VOID*                     pSystemMem;            // in: Pointer to pre-allocated sysmem
    VOID*                           pPrivateDriverData;    // in(out optional): Private data for each allocation
    UINT                            PrivateDriverDataSize; // in: Size of the private data
    D3DDDI_VIDEO_PRESENT_SOURCE_ID  VidPnSourceId;         // in: VidPN source ID if this is a primary
    union
    {
        struct
        {
            UINT    Primary         : 1;    // 0x00000001
            UINT    Reserved        :31;    // 0xFFFFFFFE
        };
        UINT        Value;
    } Flags;
} D3DDDI_ALLOCATIONINFO;

#if ((DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WIN7) || \
     (D3D_UMD_INTERFACE_VERSION >= D3D_UMD_INTERFACE_VERSION_WIN7))

typedef struct _D3DDDI_ALLOCATIONINFO2
{
    D3DKMT_HANDLE                   hAllocation;           // out: Private driver data for allocation
    CONST VOID*                     pSystemMem;            // in: Pointer to pre-allocated sysmem
    VOID*                           pPrivateDriverData;    // in(out optional): Private data for each allocation
    UINT                            PrivateDriverDataSize; // in: Size of the private data
    D3DDDI_VIDEO_PRESENT_SOURCE_ID  VidPnSourceId;         // in: VidPN source ID if this is a primary
    union
    {
        struct
        {
            UINT    Primary         : 1;    // 0x00000001
            UINT    Reserved        :31;    // 0xFFFFFFFE
        };
        UINT        Value;
    } Flags;
    D3DGPU_VIRTUAL_ADDRESS          GpuVirtualAddress;    // out: GPU Virtual address of the allocation created.
    ULONG_PTR                       Reserved[6];          // Reserved
} D3DDDI_ALLOCATIONINFO2;

#endif

typedef struct _D3DDDI_OPENALLOCATIONINFO
{
    D3DKMT_HANDLE   hAllocation;                // in: Handle for this allocation in this process
    CONST VOID*     pPrivateDriverData;         // in: Ptr to driver private buffer for this allocations
    UINT            PrivateDriverDataSize;      // in: Size in bytes of driver private buffer for this allocations
} D3DDDI_OPENALLOCATIONINFO;

#if ((DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WIN7) || \
     (D3D_UMD_INTERFACE_VERSION >= D3D_UMD_INTERFACE_VERSION_WIN7))

typedef struct _D3DDDI_OPENALLOCATIONINFO2
{
    D3DKMT_HANDLE   hAllocation;                // in: Handle for this allocation in this process
    CONST VOID*     pPrivateDriverData;         // in: Ptr to driver private buffer for this allocations
    UINT            PrivateDriverDataSize;      // in: Size in bytes of driver private buffer for this allocations
    D3DGPU_VIRTUAL_ADDRESS GpuVirtualAddress;   // out: GPU Virtual address of the allocation opened.
    ULONG_PTR       Reserved[6];                // Reserved
} D3DDDI_OPENALLOCATIONINFO2;

#endif

typedef struct _D3DDDI_ALLOCATIONLIST
{
    D3DKMT_HANDLE       hAllocation;
    union
    {
        struct
        {
            UINT            WriteOperation  	: 1;    // 0x00000001
            UINT            DoNotRetireInstance : 1;	// 0x00000002
            UINT            Reserved        	:30;    // 0xFFFFFFFC
        };
        UINT                Value;
    };
} D3DDDI_ALLOCATIONLIST;

typedef struct _D3DDDI_PATCHLOCATIONLIST
{
    UINT                AllocationIndex;
    union
    {
        struct
        {
            UINT            SlotId          : 24;   // 0x00FFFFFF
            UINT            Reserved        : 8;    // 0xFF000000
        };
        UINT                Value;
    };
    UINT                DriverId;
    UINT                AllocationOffset;
    UINT                PatchOffset;
    UINT                SplitOffset;
} D3DDDI_PATCHLOCATIONLIST;

typedef struct _D3DDDICB_LOCKFLAGS
{
    union
    {
        struct
        {
            UINT    ReadOnly            : 1;    // 0x00000001
            UINT    WriteOnly           : 1;    // 0x00000002
            UINT    DonotWait           : 1;    // 0x00000004
            UINT    IgnoreSync          : 1;    // 0x00000008
            UINT    LockEntire          : 1;    // 0x00000010
            UINT    DonotEvict          : 1;    // 0x00000020
            UINT    AcquireAperture     : 1;    // 0x00000040
            UINT    Discard             : 1;    // 0x00000080
            UINT    NoExistingReference : 1;    // 0x00000100
            UINT    UseAlternateVA      : 1;    // 0x00000200
            UINT    IgnoreReadSync      : 1;    // 0x00000400
            UINT    Reserved            :21;    // 0xFFFFF800
        };
        UINT        Value;
    };
} D3DDDICB_LOCKFLAGS;

typedef struct _D3DDDI_ESCAPEFLAGS
{
    union
    {
        struct
        {
            UINT    HardwareAccess      : 1;    // 0x00000001
            UINT    Reserved            :31;    // 0xFFFFFFFE
        };
        UINT        Value;
    };
} D3DDDI_ESCAPEFLAGS;

typedef struct _D3DDDI_CREATECONTEXTFLAGS
{
    union
    {
        struct
        {
            UINT    NullRendering : 1;      // 0x00000001
            UINT    Reserved      : 31;     // 0xFFFFFFFE
        };
        UINT Value;
    };
} D3DDDI_CREATECONTEXTFLAGS;

/* Formats
 * Most of these names have the following convention:
 *      A = Alpha
 *      R = Red
 *      G = Green
 *      B = Blue
 *      X = Unused Bits
 *      P = Palette
 *      L = Luminance
 *      U = dU coordinate for BumpMap
 *      V = dV coordinate for BumpMap
 *      S = Stencil
 *      D = Depth (e.g. Z or W buffer)
 *      C = Computed from other channels (typically on certain read operations)
 *
 *      Further, the order of the pieces are from MSB first; hence
 *      D3DFMT_A8L8 indicates that the high byte of this two byte
 *      format is alpha.
 *
 *      D16 indicates:
 *           - An integer 16-bit value.
 *           - An app-lockable surface.
 *
 *      All Depth/Stencil formats except D3DFMT_D16_LOCKABLE indicate:
 *          - no particular bit ordering per pixel, and
 *          - are not app lockable, and
 *          - the driver is allowed to consume more than the indicated
 *            number of bits per Depth channel (but not Stencil channel).
 */
#ifndef MAKEFOURCC
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |       \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */


typedef enum _D3DDDIFORMAT
{

    D3DDDIFMT_UNKNOWN           =  0,

    D3DDDIFMT_R8G8B8            = 20,
    D3DDDIFMT_A8R8G8B8          = 21,
    D3DDDIFMT_X8R8G8B8          = 22,
    D3DDDIFMT_R5G6B5            = 23,
    D3DDDIFMT_X1R5G5B5          = 24,
    D3DDDIFMT_A1R5G5B5          = 25,
    D3DDDIFMT_A4R4G4B4          = 26,
    D3DDDIFMT_R3G3B2            = 27,
    D3DDDIFMT_A8                = 28,
    D3DDDIFMT_A8R3G3B2          = 29,
    D3DDDIFMT_X4R4G4B4          = 30,
    D3DDDIFMT_A2B10G10R10       = 31,
    D3DDDIFMT_A8B8G8R8          = 32,
    D3DDDIFMT_X8B8G8R8          = 33,
    D3DDDIFMT_G16R16            = 34,
    D3DDDIFMT_A2R10G10B10       = 35,
    D3DDDIFMT_A16B16G16R16      = 36,

    D3DDDIFMT_A8P8              = 40,
    D3DDDIFMT_P8                = 41,

    D3DDDIFMT_L8                = 50,
    D3DDDIFMT_A8L8              = 51,
    D3DDDIFMT_A4L4              = 52,

    D3DDDIFMT_V8U8              = 60,
    D3DDDIFMT_L6V5U5            = 61,
    D3DDDIFMT_X8L8V8U8          = 62,
    D3DDDIFMT_Q8W8V8U8          = 63,
    D3DDDIFMT_V16U16            = 64,
    D3DDDIFMT_W11V11U10         = 65,
    D3DDDIFMT_A2W10V10U10       = 67,

    D3DDDIFMT_UYVY              = MAKEFOURCC('U', 'Y', 'V', 'Y'),
    D3DDDIFMT_R8G8_B8G8         = MAKEFOURCC('R', 'G', 'B', 'G'),
    D3DDDIFMT_YUY2              = MAKEFOURCC('Y', 'U', 'Y', '2'),
    D3DDDIFMT_G8R8_G8B8         = MAKEFOURCC('G', 'R', 'G', 'B'),
    D3DDDIFMT_DXT1              = MAKEFOURCC('D', 'X', 'T', '1'),
    D3DDDIFMT_DXT2              = MAKEFOURCC('D', 'X', 'T', '2'),
    D3DDDIFMT_DXT3              = MAKEFOURCC('D', 'X', 'T', '3'),
    D3DDDIFMT_DXT4              = MAKEFOURCC('D', 'X', 'T', '4'),
    D3DDDIFMT_DXT5              = MAKEFOURCC('D', 'X', 'T', '5'),

    D3DDDIFMT_D16_LOCKABLE      = 70,
    D3DDDIFMT_D32               = 71,
    D3DDDIFMT_D15S1             = 73,
    D3DDDIFMT_D24S8             = 75,
    D3DDDIFMT_D24X8             = 77,
    D3DDDIFMT_D24X4S4           = 79,
    D3DDDIFMT_D16               = 80,

    D3DDDIFMT_D32F_LOCKABLE     = 82,
    D3DDDIFMT_D24FS8            = 83,

    D3DDDIFMT_D32_LOCKABLE      = 84,
    D3DDDIFMT_S8_LOCKABLE       = 85,

    D3DDDIFMT_S1D15             = 72,
    D3DDDIFMT_S8D24             = 74,
    D3DDDIFMT_X8D24             = 76,
    D3DDDIFMT_X4S4D24           = 78,

    D3DDDIFMT_L16               = 81,

    D3DDDIFMT_VERTEXDATA        =100,
    D3DDDIFMT_INDEX16           =101,
    D3DDDIFMT_INDEX32           =102,

    D3DDDIFMT_Q16W16V16U16      =110,

    D3DDDIFMT_MULTI2_ARGB8      = MAKEFOURCC('M','E','T','1'),

    // Floating point surface formats

    // s10e5 formats (16-bits per channel)
    D3DDDIFMT_R16F              = 111,
    D3DDDIFMT_G16R16F           = 112,
    D3DDDIFMT_A16B16G16R16F     = 113,

    // IEEE s23e8 formats (32-bits per channel)
    D3DDDIFMT_R32F              = 114,
    D3DDDIFMT_G32R32F           = 115,
    D3DDDIFMT_A32B32G32R32F     = 116,

    D3DDDIFMT_CxV8U8            = 117,

    // Monochrome 1 bit per pixel format
    D3DDDIFMT_A1                = 118,

    // 2.8 biased fixed point
    D3DDDIFMT_A2B10G10R10_XR_BIAS = 119,

    // Decode compressed buffer formats
    D3DDDIFMT_DXVACOMPBUFFER_BASE     = 150,
    D3DDDIFMT_PICTUREPARAMSDATA       = D3DDDIFMT_DXVACOMPBUFFER_BASE+0,    // 150
    D3DDDIFMT_MACROBLOCKDATA          = D3DDDIFMT_DXVACOMPBUFFER_BASE+1,    // 151
    D3DDDIFMT_RESIDUALDIFFERENCEDATA  = D3DDDIFMT_DXVACOMPBUFFER_BASE+2,    // 152
    D3DDDIFMT_DEBLOCKINGDATA          = D3DDDIFMT_DXVACOMPBUFFER_BASE+3,    // 153
    D3DDDIFMT_INVERSEQUANTIZATIONDATA = D3DDDIFMT_DXVACOMPBUFFER_BASE+4,    // 154
    D3DDDIFMT_SLICECONTROLDATA        = D3DDDIFMT_DXVACOMPBUFFER_BASE+5,    // 155
    D3DDDIFMT_BITSTREAMDATA           = D3DDDIFMT_DXVACOMPBUFFER_BASE+6,    // 156
    D3DDDIFMT_MOTIONVECTORBUFFER      = D3DDDIFMT_DXVACOMPBUFFER_BASE+7,    // 157
    D3DDDIFMT_FILMGRAINBUFFER         = D3DDDIFMT_DXVACOMPBUFFER_BASE+8,    // 158
    D3DDDIFMT_DXVA_RESERVED9          = D3DDDIFMT_DXVACOMPBUFFER_BASE+9,    // 159
    D3DDDIFMT_DXVA_RESERVED10         = D3DDDIFMT_DXVACOMPBUFFER_BASE+10,   // 160
    D3DDDIFMT_DXVA_RESERVED11         = D3DDDIFMT_DXVACOMPBUFFER_BASE+11,   // 161
    D3DDDIFMT_DXVA_RESERVED12         = D3DDDIFMT_DXVACOMPBUFFER_BASE+12,   // 162
    D3DDDIFMT_DXVA_RESERVED13         = D3DDDIFMT_DXVACOMPBUFFER_BASE+13,   // 163
    D3DDDIFMT_DXVA_RESERVED14         = D3DDDIFMT_DXVACOMPBUFFER_BASE+14,   // 164
    D3DDDIFMT_DXVA_RESERVED15         = D3DDDIFMT_DXVACOMPBUFFER_BASE+15,   // 165
    D3DDDIFMT_DXVA_RESERVED16         = D3DDDIFMT_DXVACOMPBUFFER_BASE+16,   // 166
    D3DDDIFMT_DXVA_RESERVED17         = D3DDDIFMT_DXVACOMPBUFFER_BASE+17,   // 167
    D3DDDIFMT_DXVA_RESERVED18         = D3DDDIFMT_DXVACOMPBUFFER_BASE+18,   // 168
    D3DDDIFMT_DXVA_RESERVED19         = D3DDDIFMT_DXVACOMPBUFFER_BASE+19,   // 169
    D3DDDIFMT_DXVA_RESERVED20         = D3DDDIFMT_DXVACOMPBUFFER_BASE+20,   // 170
    D3DDDIFMT_DXVA_RESERVED21         = D3DDDIFMT_DXVACOMPBUFFER_BASE+21,   // 171
    D3DDDIFMT_DXVA_RESERVED22         = D3DDDIFMT_DXVACOMPBUFFER_BASE+22,   // 172
    D3DDDIFMT_DXVA_RESERVED23         = D3DDDIFMT_DXVACOMPBUFFER_BASE+23,   // 173
    D3DDDIFMT_DXVA_RESERVED24         = D3DDDIFMT_DXVACOMPBUFFER_BASE+24,   // 174
    D3DDDIFMT_DXVA_RESERVED25         = D3DDDIFMT_DXVACOMPBUFFER_BASE+25,   // 175
    D3DDDIFMT_DXVA_RESERVED26         = D3DDDIFMT_DXVACOMPBUFFER_BASE+26,   // 176
    D3DDDIFMT_DXVA_RESERVED27         = D3DDDIFMT_DXVACOMPBUFFER_BASE+27,   // 177
    D3DDDIFMT_DXVA_RESERVED28         = D3DDDIFMT_DXVACOMPBUFFER_BASE+28,   // 178
    D3DDDIFMT_DXVA_RESERVED29         = D3DDDIFMT_DXVACOMPBUFFER_BASE+29,   // 179
    D3DDDIFMT_DXVA_RESERVED30         = D3DDDIFMT_DXVACOMPBUFFER_BASE+30,   // 180
    D3DDDIFMT_DXVA_RESERVED31         = D3DDDIFMT_DXVACOMPBUFFER_BASE+31,   // 181
    D3DDDIFMT_DXVACOMPBUFFER_MAX      = D3DDDIFMT_DXVA_RESERVED31,

    D3DDDIFMT_BINARYBUFFER            = 199,

    D3DDDIFMT_FORCE_UINT        =0x7fffffff
} D3DDDIFORMAT;

typedef struct _D3DDDIRECT
{
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} D3DDDIRECT;

typedef struct _D3DDDI_KERNELOVERLAYINFO
{
    D3DKMT_HANDLE        hAllocation;           // in: Allocation to be displayed
    D3DDDIRECT           DstRect;               // in: Dest rect
    D3DDDIRECT           SrcRect;               // in: Source rect
    VOID*                pPrivateDriverData;    // in: Private driver data
    UINT                 PrivateDriverDataSize; // in: Size of private driver data
} D3DDDI_KERNELOVERLAYINFO;

typedef enum _D3DDDI_GAMMARAMP_TYPE
{
    D3DDDI_GAMMARAMP_UNINITIALIZED = 0,
    D3DDDI_GAMMARAMP_DEFAULT       = 1,
    D3DDDI_GAMMARAMP_RGB256x3x16   = 2,
    D3DDDI_GAMMARAMP_DXGI_1        = 3,
} D3DDDI_GAMMARAMP_TYPE;

typedef struct _D3DDDI_GAMMA_RAMP_RGB256x3x16
{
    USHORT  Red[256];
    USHORT  Green[256];
    USHORT  Blue[256];
} D3DDDI_GAMMA_RAMP_RGB256x3x16;

typedef struct D3DDDI_DXGI_RGB
{
    float   Red;
    float   Green;
    float   Blue;
} D3DDDI_DXGI_RGB;

typedef struct _D3DDDI_GAMMA_RAMP_DXGI_1
{
    D3DDDI_DXGI_RGB    Scale;
    D3DDDI_DXGI_RGB    Offset;
    D3DDDI_DXGI_RGB    GammaCurve[1025];
} D3DDDI_GAMMA_RAMP_DXGI_1;


// Used as a value for D3DDDI_VIDEO_PRESENT_SOURCE_ID and D3DDDI_VIDEO_PRESENT_TARGET_ID types to specify
// that the respective video present source/target ID hasn't been initialized.
#define D3DDDI_ID_UNINITIALIZED (UINT)(~0)

// TODO:[mmilirud] Define this as (UINT)(~1) to avoid collision with valid source ID equal to 0.
//
// Used as a value for D3DDDI_VIDEO_PRESENT_SOURCE_ID and D3DDDI_VIDEO_PRESENT_TARGET_ID types to specify
// that the respective video present source/target ID isn't applicable for the given execution context.
#define D3DDDI_ID_NOTAPPLICABLE (UINT)(0)

// Used as a value for D3DDDI_VIDEO_PRESENT_SOURCE_ID and D3DDDI_VIDEO_PRESENT_TARGET_ID types to specify
// that the respective video present source/target ID describes every VidPN source/target in question.
#define D3DDDI_ID_ALL (UINT)(~2)

//
// Hardcoded VidPnSource count
//
#define D3DKMDT_MAX_VIDPN_SOURCES_BITCOUNT      4
#define D3DKMDT_MAX_VIDPN_SOURCES               (1 << D3DKMDT_MAX_VIDPN_SOURCES_BITCOUNT)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Purpose: Multi-sampling method descriptor type.
//
// Remarks: Driver is free to partition its quality levels for a given multi-sampling method into as many
//          increments as it likes, with the condition that each incremental step does noticably improve
//          quality of the presented image.
//
typedef struct _D3DDDI_MULTISAMPLINGMETHOD
{
    // Number of sub-pixels employed in this multi-sampling method (e.g. 2 for 2x and 8 for 8x multi-sampling)
    UINT  NumSamples;

    // Upper bound on the quality range supported for this multi-sampling method. The range starts from 0
    // and goes upto and including the reported maximum quality setting.
    UINT  NumQualityLevels;
}
D3DDDI_MULTISAMPLINGMETHOD;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Purpose: Video signal scan line ordering descriptor type.
//
// Remarks: Scan-line ordering of the video mode, specifies whether each field contains the entire
//          content of a frame, or only half of it (i.e. even/odd lines interchangeably).
//          Note that while for standard interlaced modes, what field comes first can be inferred
//          from the mode, specifying this characteristic explicitly with an enum both frees up the
//          client from having to maintain mode-based look-up tables and is extensible for future
//          standard modes not listed in the D3DKMDT_VIDEO_SIGNAL_STANDARD enum.
//
typedef enum _D3DDDI_VIDEO_SIGNAL_SCANLINE_ORDERING
{
    D3DDDI_VSSLO_UNINITIALIZED              = 0,
    D3DDDI_VSSLO_PROGRESSIVE                = 1,
    D3DDDI_VSSLO_INTERLACED_UPPERFIELDFIRST = 2,
    D3DDDI_VSSLO_INTERLACED_LOWERFIELDFIRST = 3,
    D3DDDI_VSSLO_OTHER                      = 255
}
D3DDDI_VIDEO_SIGNAL_SCANLINE_ORDERING;


typedef enum D3DDDI_FLIPINTERVAL_TYPE
{
    D3DDDI_FLIPINTERVAL_IMMEDIATE = 0,
    D3DDDI_FLIPINTERVAL_ONE       = 1,
    D3DDDI_FLIPINTERVAL_TWO       = 2,
    D3DDDI_FLIPINTERVAL_THREE     = 3,
    D3DDDI_FLIPINTERVAL_FOUR      = 4,
} D3DDDI_FLIPINTERVAL_TYPE;


typedef enum _D3DDDI_POOL
{
     D3DDDIPOOL_SYSTEMMEM      = 1,
     D3DDDIPOOL_VIDEOMEMORY    = 2,
     D3DDDIPOOL_LOCALVIDMEM    = 3,
     D3DDDIPOOL_NONLOCALVIDMEM = 4,
} D3DDDI_POOL;


typedef enum _D3DDDIMULTISAMPLE_TYPE
{
    D3DDDIMULTISAMPLE_NONE         =  0,
    D3DDDIMULTISAMPLE_NONMASKABLE  =  1,
    D3DDDIMULTISAMPLE_2_SAMPLES    =  2,
    D3DDDIMULTISAMPLE_3_SAMPLES    =  3,
    D3DDDIMULTISAMPLE_4_SAMPLES    =  4,
    D3DDDIMULTISAMPLE_5_SAMPLES    =  5,
    D3DDDIMULTISAMPLE_6_SAMPLES    =  6,
    D3DDDIMULTISAMPLE_7_SAMPLES    =  7,
    D3DDDIMULTISAMPLE_8_SAMPLES    =  8,
    D3DDDIMULTISAMPLE_9_SAMPLES    =  9,
    D3DDDIMULTISAMPLE_10_SAMPLES   = 10,
    D3DDDIMULTISAMPLE_11_SAMPLES   = 11,
    D3DDDIMULTISAMPLE_12_SAMPLES   = 12,
    D3DDDIMULTISAMPLE_13_SAMPLES   = 13,
    D3DDDIMULTISAMPLE_14_SAMPLES   = 14,
    D3DDDIMULTISAMPLE_15_SAMPLES   = 15,
    D3DDDIMULTISAMPLE_16_SAMPLES   = 16,

    D3DDDIMULTISAMPLE_FORCE_UINT   = 0x7fffffff
} D3DDDIMULTISAMPLE_TYPE;

typedef struct _D3DDDI_RESOURCEFLAGS
{
    union
    {
        struct
        {
            UINT    RenderTarget            : 1;    // 0x00000001
            UINT    ZBuffer                 : 1;    // 0x00000002
            UINT    Dynamic                 : 1;    // 0x00000004
            UINT    HintStatic              : 1;    // 0x00000008
            UINT    AutogenMipmap           : 1;    // 0x00000010
            UINT    DMap                    : 1;    // 0x00000020
            UINT    WriteOnly               : 1;    // 0x00000040
            UINT    NotLockable             : 1;    // 0x00000080
            UINT    Points                  : 1;    // 0x00000100
            UINT    RtPatches               : 1;    // 0x00000200
            UINT    NPatches                : 1;    // 0x00000400
            UINT    SharedResource          : 1;    // 0x00000800
            UINT    DiscardRenderTarget     : 1;    // 0x00001000
            UINT    Video                   : 1;    // 0x00002000
            UINT    CaptureBuffer           : 1;    // 0x00004000
            UINT    Primary                 : 1;    // 0x00008000
            UINT    Texture                 : 1;    // 0x00010000
            UINT    CubeMap                 : 1;    // 0x00020000
            UINT    Volume                  : 1;    // 0x00040000
            UINT    VertexBuffer            : 1;    // 0x00080000
            UINT    IndexBuffer             : 1;    // 0x00100000
            UINT    DecodeRenderTarget      : 1;    // 0x00200000
            UINT    DecodeCompressedBuffer  : 1;    // 0x00400000
            UINT    VideoProcessRenderTarget: 1;    // 0x00800000
            UINT    CpuOptimized            : 1;    // 0x01000000
            UINT    MightDrawFromLocked     : 1;    // 0x02000000
            UINT    Overlay                 : 1;    // 0x04000000
            UINT    MatchGdiPrimary         : 1;    // 0x08000000
            UINT    InterlacedRefresh       : 1;    // 0x10000000
            UINT    TextApi                 : 1;    // 0x20000000
            UINT    RestrictedContent       : 1;    // 0x40000000
            UINT    RestrictSharedAccess    : 1;    // 0x80000000
        };
        UINT        Value;
    };
} D3DDDI_RESOURCEFLAGS;

typedef struct _D3DDDI_SURFACEINFO
{
    UINT                Width;              // in: For linear, surface and volume
    UINT                Height;             // in: For surface and volume
    UINT                Depth;              // in: For volume
    CONST VOID*         pSysMem;
    UINT                SysMemPitch;
    UINT                SysMemSlicePitch;
} D3DDDI_SURFACEINFO;

typedef enum _D3DDDI_ROTATION
{
    D3DDDI_ROTATION_IDENTITY        = 1,    // No rotation.
    D3DDDI_ROTATION_90              = 2,    // Rotated 90 degrees.
    D3DDDI_ROTATION_180             = 3,    // Rotated 180 degrees.
    D3DDDI_ROTATION_270             = 4     // Rotated 270 degrees.
} D3DDDI_ROTATION;

typedef enum D3DDDI_SCANLINEORDERING
{
    D3DDDI_SCANLINEORDERING_UNKNOWN                    = 0,
    D3DDDI_SCANLINEORDERING_PROGRESSIVE                = 1,
    D3DDDI_SCANLINEORDERING_INTERLACED                 = 2,
} D3DDDI_SCANLINEORDERING;

typedef struct _D3DDDIARG_CREATERESOURCE
{
    D3DDDIFORMAT                    Format;
    D3DDDI_POOL                     Pool;
    D3DDDIMULTISAMPLE_TYPE          MultisampleType;
    UINT                            MultisampleQuality;
    CONST D3DDDI_SURFACEINFO*       pSurfList;          // in: List of sub resource objects to create
    UINT                            SurfCount;          // in: Number of sub resource objects
    UINT                            MipLevels;
    UINT                            Fvf;                // in: FVF format for vertex buffers
    D3DDDI_VIDEO_PRESENT_SOURCE_ID  VidPnSourceId;      // in: VidPnSourceId on which the primary surface is created
    D3DDDI_RATIONAL                 RefreshRate;        // in: RefreshRate that this primary surface is to be used with
    HANDLE                          hResource;          // in/out: D3D runtime handle/UM driver handle
    D3DDDI_RESOURCEFLAGS            Flags;
    D3DDDI_ROTATION                 Rotation;           // in: The orientation of the resource. (0, 90, 180, 270)
} D3DDDIARG_CREATERESOURCE;

typedef struct _D3DDDICB_SIGNALFLAGS
{
    union
    {
        struct
        {
            UINT SignalAtSubmission : 1;
            UINT Reserved           : 31;
        };
        UINT Value;
    };
} D3DDDICB_SIGNALFLAGS;

#define D3DDDI_MAX_OBJECT_WAITED_ON 32
#define D3DDDI_MAX_OBJECT_SIGNALED  32

typedef enum _D3DDDI_SYNCHRONIZATIONOBJECT_TYPE
{
    D3DDDI_SYNCHRONIZATION_MUTEX    = 1,
    D3DDDI_SEMAPHORE                = 2,
    D3DDDI_FENCE                    = 3,
    D3DDDI_CPU_NOTIFICATION         = 4,
} D3DDDI_SYNCHRONIZATIONOBJECT_TYPE;

typedef struct _D3DDDI_SYNCHRONIZATIONOBJECTINFO
{
    D3DDDI_SYNCHRONIZATIONOBJECT_TYPE    Type;      // in: Type of synchronization object to create.
    union
    {
        struct
        {
            BOOL InitialState;                      // in: Initial state of a synchronization mutex.
        } SynchronizationMutex;

        struct
        {
            UINT MaxCount;                          // in: Max count of the semaphore.
            UINT InitialCount;                      // in: Initial count of the semaphore.
        } Semaphore;


        struct
        {
            UINT Reserved[16];                      // Reserved for future use.
        } Reserved;
    };
} D3DDDI_SYNCHRONIZATIONOBJECTINFO;

typedef struct _D3DDDI_SYNCHRONIZATIONOBJECT_FLAGS
{
    UINT Shared   :  1;
    UINT Reserved : 31;
} D3DDDI_SYNCHRONIZATIONOBJECT_FLAGS;

typedef struct _D3DDDI_SYNCHRONIZATIONOBJECTINFO2
{
    D3DDDI_SYNCHRONIZATIONOBJECT_TYPE    Type;      // in: Type of synchronization object to create.
    D3DDDI_SYNCHRONIZATIONOBJECT_FLAGS   Flags;     // in: flags.
    union
    {
        struct
        {
            BOOL InitialState;                      // in: Initial state of a synchronization mutex.
        } SynchronizationMutex;

        struct
        {
            UINT MaxCount;                          // in: Max count of the semaphore.
            UINT InitialCount;                      // in: Initial count of the semaphore.
        } Semaphore;

        struct
        {
            UINT64 FenceValue;                      // in: inital fence value.
        } Fence;

        struct
        {
            HANDLE Event;                           // in: Handle to the event
        } CPUNotification;


        struct
        {
            UINT64 Reserved[8];                     // Reserved for future use.
        } Reserved;
    };

    D3DKMT_HANDLE  SharedHandle;                    // out: global shared handle (when requested to be shared)

} D3DDDI_SYNCHRONIZATIONOBJECTINFO2;

//
// Defines the maximum number of context a particular command buffer can
// be broadcast to.
//
#define D3DDDI_MAX_BROADCAST_CONTEXT        64

//
// Allocation priorities.
//
#define D3DDDI_ALLOCATIONPRIORITY_MINIMUM       0x28000000
#define D3DDDI_ALLOCATIONPRIORITY_LOW           0x50000000
#define D3DDDI_ALLOCATIONPRIORITY_NORMAL        0x78000000
#define D3DDDI_ALLOCATIONPRIORITY_HIGH          0xa0000000
#define D3DDDI_ALLOCATIONPRIORITY_MAXIMUM       0xc8000000

#endif // (NTDDI_VERSION >= NTDDI_LONGHORN) || defined(D3DKMDT_SPECIAL_MULTIPLATFORM_TOOL)

#pragma warning(pop)

#endif /* _D3DUKMDT_H_ */


