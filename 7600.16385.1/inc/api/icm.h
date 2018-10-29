/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    icm.h

Abstract:

    Public header file for Image Color Management

Revision History:

--*/

#ifndef _ICM_H_
#define _ICM_H_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// Support for named color profiles
//

typedef char COLOR_NAME[32];
typedef COLOR_NAME *PCOLOR_NAME, *LPCOLOR_NAME;

typedef struct tagNAMED_PROFILE_INFO{
        DWORD           dwFlags;
        DWORD           dwCount;
        DWORD           dwCountDevCoordinates;
        COLOR_NAME      szPrefix;
        COLOR_NAME      szSuffix;
} NAMED_PROFILE_INFO;
typedef NAMED_PROFILE_INFO *PNAMED_PROFILE_INFO, *LPNAMED_PROFILE_INFO;


//
// Color spaces
//
// The following color spaces are supported.
// Gray, RGB, CMYK, XYZ, Yxy, Lab, generic 3 channel color spaces where
// the profiles defines how to interpret the 3 channels, named color spaces
// which can either be indices into the space or have color names, and
// multichannel spaces with 1 byte per channel up to MAX_COLOR_CHANNELS.
//

#define MAX_COLOR_CHANNELS  8   // maximum number of HiFi color channels

struct GRAYCOLOR {
    WORD    gray;
};

struct RGBCOLOR {
    WORD    red;
    WORD    green;
    WORD    blue;
};

struct CMYKCOLOR {
    WORD    cyan;
    WORD    magenta;
    WORD    yellow;
    WORD    black;
};

struct XYZCOLOR {
    WORD    X;
    WORD    Y;
    WORD    Z;
};

struct YxyCOLOR {
    WORD    Y;
    WORD    x;
    WORD    y;
};

struct LabCOLOR {
    WORD    L;
    WORD    a;
    WORD    b;
};

struct GENERIC3CHANNEL {
    WORD    ch1;
    WORD    ch2;
    WORD    ch3;
};

struct NAMEDCOLOR {
    DWORD        dwIndex;
};

struct HiFiCOLOR {
    BYTE    channel[MAX_COLOR_CHANNELS];
};

#if _MSC_VER > 1200
#pragma warning(push)
#pragma warning(disable:4201)	// nameless struct/union
#endif

typedef union tagCOLOR {
    struct GRAYCOLOR        gray;
    struct RGBCOLOR         rgb;
    struct CMYKCOLOR        cmyk;
    struct XYZCOLOR         XYZ;
    struct YxyCOLOR         Yxy;
    struct LabCOLOR         Lab;
    struct GENERIC3CHANNEL  gen3ch;
    struct NAMEDCOLOR       named;
    struct HiFiCOLOR        hifi;

    struct {                // alignment padding.
        DWORD reserved1;
        VOID *reserved2;
    };

} COLOR;
typedef COLOR *PCOLOR, *LPCOLOR;

#if _MSC_VER > 1200
#pragma warning(pop)
#endif

typedef enum {
    COLOR_GRAY           =   1,
    COLOR_RGB,
    COLOR_XYZ,
    COLOR_Yxy,
    COLOR_Lab,
    COLOR_3_CHANNEL,        // WORD per channel
    COLOR_CMYK,
    COLOR_5_CHANNEL,        // BYTE per channel
    COLOR_6_CHANNEL,        //      - do -
    COLOR_7_CHANNEL,        //      - do -
    COLOR_8_CHANNEL,        //      - do -
    COLOR_NAMED,
} COLORTYPE;
typedef COLORTYPE *PCOLORTYPE, *LPCOLORTYPE;

//
// Rendering Intents
//
// + INTENT_PERCEPTUAL            = LCS_GM_IMAGES for LOGCOLORSPACE
//                                = DMICM_CONTRAST for DEVMODE
//                                = "Pictures" for SetupColorMathing/Printer UI
//
// + INTENT_RELATIVE_COLORIMETRIC = LCS_GM_GRAPHICS for LOGCOLORSPACE
//                                = DMICM_COLORIMETRIC for DEVMODE
//                                = "Proof" for SetupColorMatching/Printer UI
//
// + INTENT_SATURATION            = LCS_GM_BUSINESS for LOGCOLORSPACE
//                                = DMICM_SATURATE for DEVMODE
//                                = "Graphics" for SetupColorMatching/Printer UI
//
// + INTENT_ABSOLUTE_COLORIMETRIC = LCS_GM_ABS_COLORIMETRIC for LOGCOLORSPACE
//                                = DMICM_ABS_COLORIMETRIC for DEVMODE
//                                = "Match" for SetupColorMatching/Printer UI
//

#define INTENT_PERCEPTUAL               0
#define INTENT_RELATIVE_COLORIMETRIC    1
#define INTENT_SATURATION               2
#define INTENT_ABSOLUTE_COLORIMETRIC    3



#if NTDDI_VERSION >= NTDDI_VISTA

typedef enum {
    CPT_ICC,
    CPT_DMP,
    CPT_CAMP,
    CPT_GMMP
} COLORPROFILETYPE;
typedef COLORPROFILETYPE *PCOLORPROFILETYPE, *LPCOLORPROFILETYPE;

typedef enum {

    // intent
    CPST_PERCEPTUAL             = INTENT_PERCEPTUAL,
    CPST_RELATIVE_COLORIMETRIC  = INTENT_RELATIVE_COLORIMETRIC,
    CPST_SATURATION             = INTENT_SATURATION,
    CPST_ABSOLUTE_COLORIMETRIC  = INTENT_ABSOLUTE_COLORIMETRIC,

    // working space
    CPST_NONE,
    CPST_RGB_WORKING_SPACE,
    CPST_CUSTOM_WORKING_SPACE,

} COLORPROFILESUBTYPE;
typedef COLORPROFILESUBTYPE *PCOLORPROFILESUBTYPE, *LPCOLORPROFILESUBTYPE;

//
// Device color data type
//
typedef enum {
    COLOR_BYTE       =   1,     // BYTE per channel. data range [0, 255]
    COLOR_WORD,                 // WORD per channel. data range [0, 65535]
    COLOR_FLOAT,                // FLOAT per channel. IEEE 32-bit floating point
    COLOR_S2DOT13FIXED,          // WORD per channel. data range [-4, +4] using s2.13
    
//#if NTDDI_VERSION >= NTDDI_WIN7
    COLOR_10b_R10G10B10A2,	    // Packed WORD per channel.  data range [0, 1]
    COLOR_10b_R10G10B10A2_XR,   // Packed extended range WORD per channel.  data range [-1, 3] 
                                // using 4.0 scale and -1.0 bias.
    COLOR_FLOAT16               // FLOAT16 per channel.
//#endif //NTDDI_VERSION >= NTDDI_WIN7

} COLORDATATYPE;
typedef COLORDATATYPE *PCOLORDATATYPE, *LPCOLORDATATYPE;

#endif // NTDDI_VERSION >= NTDDI_VISTA

//
// Bitmap formats supported
//

typedef enum {

    //
    // 16bpp - 5 bits per channel. The most significant bit is ignored.
    //

    BM_x555RGB      = 0x0000,
    BM_x555XYZ      = 0x0101,
    BM_x555Yxy,
    BM_x555Lab,
    BM_x555G3CH,

    //
    // Packed 8 bits per channel => 8bpp for GRAY and
    // 24bpp for the 3 channel colors, more for hifi channels
    //

    BM_RGBTRIPLETS  = 0x0002,
    BM_BGRTRIPLETS  = 0x0004,
    BM_XYZTRIPLETS  = 0x0201,
    BM_YxyTRIPLETS,
    BM_LabTRIPLETS,
    BM_G3CHTRIPLETS,
    BM_5CHANNEL,
    BM_6CHANNEL,
    BM_7CHANNEL,
    BM_8CHANNEL,
    BM_GRAY,

    //
    // 32bpp - 8 bits per channel. The most significant byte is ignored
    // for the 3 channel colors.
    //

    BM_xRGBQUADS    = 0x0008,
    BM_xBGRQUADS    = 0x0010,
    BM_xG3CHQUADS   = 0x0304,
    BM_KYMCQUADS,
    BM_CMYKQUADS    = 0x0020,

    //
    // 32bpp - 10 bits per channel. The 2 most significant bits are ignored.
    //

    BM_10b_RGB      = 0x0009,
    BM_10b_XYZ      = 0x0401,
    BM_10b_Yxy,
    BM_10b_Lab,
    BM_10b_G3CH,

    //
    // 32bpp - named color indices (1-based)
    //

    BM_NAMED_INDEX,

    //
    // Packed 16 bits per channel => 16bpp for GRAY and
    // 48bpp for the 3 channel colors.
    //

    BM_16b_RGB      = 0x000A,
    BM_16b_XYZ      = 0x0501,
    BM_16b_Yxy,
    BM_16b_Lab,
    BM_16b_G3CH,
    BM_16b_GRAY,

    //
    // 16 bpp - 5 bits for Red & Blue, 6 bits for Green
    //

    BM_565RGB       = 0x0001,

//#if NTDDI_VERSION >= NTDDI_VISTA
    //
    // scRGB - 32 bits per channel floating point
    //         16 bits per channel floating point
    //

    BM_32b_scRGB            = 0x0601,
    BM_32b_scARGB           = 0x0602,
    BM_S2DOT13FIXED_scRGB   = 0x0603,
    BM_S2DOT13FIXED_scARGB  = 0x0604,
//#endif // NTDDI_VERSION >= NTDDI_VISTA

//#if NTDDI_VERSION >= NTDDI_WIN7
    BM_R10G10B10A2          = 0x0701,
    BM_R10G10B10A2_XR        = 0x0702,
    BM_R16G16B16A16_FLOAT   = 0x0703
//#endif // NTDDI_VERSION >= NTDDI_WIN7

} BMFORMAT;
typedef BMFORMAT *PBMFORMAT, *LPBMFORMAT;

//
// Callback function definition
//

typedef BOOL (WINAPI *PBMCALLBACKFN)(ULONG, ULONG, LPARAM);
typedef PBMCALLBACKFN LPBMCALLBACKFN;

//
// ICC profile header
//

typedef struct tagPROFILEHEADER {
    DWORD   phSize;             // profile size in bytes
    DWORD   phCMMType;          // CMM for this profile
    DWORD   phVersion;          // profile format version number
    DWORD   phClass;            // type of profile
    DWORD   phDataColorSpace;   // color space of data
    DWORD   phConnectionSpace;  // PCS
    DWORD   phDateTime[3];      // date profile was created
    DWORD   phSignature;        // magic number
    DWORD   phPlatform;         // primary platform
    DWORD   phProfileFlags;     // various bit settings
    DWORD   phManufacturer;     // device manufacturer
    DWORD   phModel;            // device model number
    DWORD   phAttributes[2];    // device attributes
    DWORD   phRenderingIntent;  // rendering intent
    CIEXYZ  phIlluminant;       // profile illuminant
    DWORD   phCreator;          // profile creator
    BYTE    phReserved[44];     // reserved for future use
} PROFILEHEADER;
typedef PROFILEHEADER *PPROFILEHEADER, *LPPROFILEHEADER;

//
// Profile class values
//

#define CLASS_MONITOR           'mntr'
#define CLASS_PRINTER           'prtr'
#define CLASS_SCANNER           'scnr'
#define CLASS_LINK              'link'
#define CLASS_ABSTRACT          'abst'
#define CLASS_COLORSPACE        'spac'
#define CLASS_NAMED             'nmcl'
#if NTDDI_VERSION >= NTDDI_VISTA
#define CLASS_CAMP              'camp'
#define CLASS_GMMP              'gmmp'
#endif // NTDDI_VERSION >= NTDDI_VISTA

//
// Color space values
//

#define SPACE_XYZ               'XYZ '
#define SPACE_Lab               'Lab '
#define SPACE_Luv               'Luv '
#define SPACE_YCbCr             'YCbr'
#define SPACE_Yxy               'Yxy '
#define SPACE_RGB               'RGB '
#define SPACE_GRAY              'GRAY'
#define SPACE_HSV               'HSV '
#define SPACE_HLS               'HLS '
#define SPACE_CMYK              'CMYK'
#define SPACE_CMY               'CMY '
#define SPACE_2_CHANNEL         '2CLR'
#define SPACE_3_CHANNEL         '3CLR'
#define SPACE_4_CHANNEL         '4CLR'
#define SPACE_5_CHANNEL         '5CLR'
#define SPACE_6_CHANNEL         '6CLR'
#define SPACE_7_CHANNEL         '7CLR'
#define SPACE_8_CHANNEL         '8CLR'

//
// Profile flag bitfield values
//

#define FLAG_EMBEDDEDPROFILE    0x00000001
#define FLAG_DEPENDENTONDATA    0x00000002

//
// Profile flag bits for vendor extensions (bytes 44..45 of the ICC profile header).
//

#if NTDDI_VERSION >= NTDDI_VISTA

//
// Enable support for chromatic adaptation.
//
#define FLAG_ENABLE_CHROMATIC_ADAPTATION 0x02000000

#endif // NTDDI_VERSION >= NTDDI_VISTA

//
// Profile attributes bitfield values
//

#define ATTRIB_TRANSPARENCY     0x00000001
#define ATTRIB_MATTE            0x00000002

//
// Profile data structure
//

typedef struct tagPROFILE {
    DWORD   dwType;             // profile type
    PVOID   pProfileData;       // filename or buffer containing profile
    DWORD   cbDataSize;         // size of profile data
} PROFILE;
typedef PROFILE *PPROFILE, *LPPROFILE;


//
// Profile types to be used in the PROFILE structure
//

#define PROFILE_FILENAME    1   // profile data is NULL terminated filename
#define PROFILE_MEMBUFFER   2   // profile data is a buffer containing
                                // the profile
//
// Desired access mode for opening profiles
//

#define PROFILE_READ        1   // opened for read access
#define PROFILE_READWRITE   2   // opened for read and write access

//
// Handles returned to applications
//

typedef HANDLE HPROFILE;        // handle to profile object
typedef HPROFILE *PHPROFILE;
typedef HANDLE HTRANSFORM;      // handle to color transform object

//
// CMM selection for CreateMultiProfileTransform and SelectCMM.
//

#define INDEX_DONT_CARE     0

#define CMM_FROM_PROFILE    INDEX_DONT_CARE // Use CMM specified in profile.
#define CMM_WINDOWS_DEFAULT 'Win '          // Use Windows default CMM always.

//
// Tags found in ICC profiles
//

typedef DWORD      TAGTYPE;
typedef TAGTYPE   *PTAGTYPE, *LPTAGTYPE;

//
// Profile enumeration data structure
//

#define ENUM_TYPE_VERSION    0x0300

typedef struct tagENUMTYPEA {
    DWORD   dwSize;             // structure size
    DWORD   dwVersion;          // structure version
    DWORD   dwFields;           // bit fields
    PCSTR   pDeviceName;        // device friendly name
    DWORD   dwMediaType;        // media type
    DWORD   dwDitheringMode;    // dithering mode
    DWORD   dwResolution[2];    // x and y resolutions
    DWORD   dwCMMType;          // cmm ID
    DWORD   dwClass;            // profile class
    DWORD   dwDataColorSpace;   // color space of data
    DWORD   dwConnectionSpace;  // pcs
    DWORD   dwSignature;        // magic number
    DWORD   dwPlatform;         // primary platform
    DWORD   dwProfileFlags;     // various bit settings in profile
    DWORD   dwManufacturer;     // manufacturer ID
    DWORD   dwModel;            // model ID
    DWORD   dwAttributes[2];    // device attributes
    DWORD   dwRenderingIntent;  // rendering intent
    DWORD   dwCreator;          // profile creator
    DWORD   dwDeviceClass;      // device class
} ENUMTYPEA, *PENUMTYPEA, *LPENUMTYPEA;


typedef struct tagENUMTYPEW {
    DWORD   dwSize;             // structure size
    DWORD   dwVersion;          // structure version
    DWORD   dwFields;           // bit fields
    PCWSTR  pDeviceName;        // device friendly name
    DWORD   dwMediaType;        // media type
    DWORD   dwDitheringMode;    // dithering mode
    DWORD   dwResolution[2];    // x and y resolutions
    DWORD   dwCMMType;          // cmm ID
    DWORD   dwClass;            // profile class
    DWORD   dwDataColorSpace;   // color space of data
    DWORD   dwConnectionSpace;  // pcs
    DWORD   dwSignature;        // magic number
    DWORD   dwPlatform;         // primary platform
    DWORD   dwProfileFlags;     // various bit settings in profile
    DWORD   dwManufacturer;     // manufacturer ID
    DWORD   dwModel;            // model ID
    DWORD   dwAttributes[2];    // device attributes
    DWORD   dwRenderingIntent;  // rendering intent
    DWORD   dwCreator;          // profile creator
    DWORD   dwDeviceClass;      // device class
} ENUMTYPEW, *PENUMTYPEW, *LPENUMTYPEW;

//
// Bitfields for enumeration record above
//

#define ET_DEVICENAME           0x00000001
#define ET_MEDIATYPE            0x00000002
#define ET_DITHERMODE           0x00000004
#define ET_RESOLUTION           0x00000008
#define ET_CMMTYPE              0x00000010
#define ET_CLASS                0x00000020
#define ET_DATACOLORSPACE       0x00000040
#define ET_CONNECTIONSPACE      0x00000080
#define ET_SIGNATURE            0x00000100
#define ET_PLATFORM             0x00000200
#define ET_PROFILEFLAGS         0x00000400
#define ET_MANUFACTURER         0x00000800
#define ET_MODEL                0x00001000
#define ET_ATTRIBUTES           0x00002000
#define ET_RENDERINGINTENT      0x00004000
#define ET_CREATOR              0x00008000
#define ET_DEVICECLASS          0x00010000

#if NTDDI_VERSION >= NTDDI_VISTA

//
// WCS profile management scope - system-wide and current-user
//

typedef enum
{
    WCS_PROFILE_MANAGEMENT_SCOPE_SYSTEM_WIDE,
    WCS_PROFILE_MANAGEMENT_SCOPE_CURRENT_USER
} WCS_PROFILE_MANAGEMENT_SCOPE;

#endif // NTDDI_VERSION >= NTDDI_VISTA

//
// Flags for creating color transforms
//

#define PROOF_MODE                  0x00000001
#define NORMAL_MODE                 0x00000002
#define BEST_MODE                   0x00000003
#define ENABLE_GAMUT_CHECKING       0x00010000
#define USE_RELATIVE_COLORIMETRIC   0x00020000
#define FAST_TRANSLATE              0x00040000
#if NTDDI_VERSION >= NTDDI_VISTA
#define PRESERVEBLACK               0x00100000
#define WCS_ALWAYS                  0x00200000
#define SEQUENTIAL_TRANSFORM        0x80800000
#endif // NTDDI_VERSION >= NTDDI_VISTA
#define RESERVED                    0x80000000

//
// Parameter for GetPS2ColorSpaceArray
//

#define CSA_A                   1
#define CSA_ABC                 2
#define CSA_DEF                 3
#define CSA_DEFG                4
#define CSA_GRAY                5
#define CSA_RGB                 6
#define CSA_CMYK                7
#define CSA_Lab                 8

//
// Parameter for CMGetInfo()
//

#define CMM_WIN_VERSION     0
#define CMM_IDENT           1
#define CMM_DRIVER_VERSION  2
#define CMM_DLL_VERSION     3
#define CMM_VERSION         4
#define CMM_DESCRIPTION     5
#define CMM_LOGOICON        6

//
// Parameter for CMTranslateRGBs()
//

#define CMS_FORWARD         0
#define CMS_BACKWARD        1

//
//  Constants for SetupColorMatching()
//

#define COLOR_MATCH_VERSION  0x0200

//
//  Constants for flags
//

#define CMS_DISABLEICM          1     // Disable color matching
#define CMS_ENABLEPROOFING      2     // Enable proofing

#define CMS_SETRENDERINTENT     4     // Use passed in value
#define CMS_SETPROOFINTENT      8
#define CMS_SETMONITORPROFILE   0x10  // Use passed in profile name initially
#define CMS_SETPRINTERPROFILE   0x20
#define CMS_SETTARGETPROFILE    0x40

#define CMS_USEHOOK             0x80  // Use hook procedure in lpfnHook
#define CMS_USEAPPLYCALLBACK    0x100 // Use callback procedure when applied
#define CMS_USEDESCRIPTION      0x200 // Use profile description in UI
                                      //   (default is filename)

#define CMS_DISABLEINTENT       0x400 // Disable intent selection (render & proofing) always
#define CMS_DISABLERENDERINTENT 0x800 // Disable rendering intent selection while in proofing mode
                                      // Only proofing intent selection is enabled.

//
//  Used to denote too-small buffers (output only)
//

#define CMS_MONITOROVERFLOW     0x80000000L
#define CMS_PRINTEROVERFLOW     0x40000000L
#define CMS_TARGETOVERFLOW      0x20000000L

//
//  Structures (both ANSI and Unicode)
//
struct _tagCOLORMATCHSETUPW;
struct _tagCOLORMATCHSETUPA;

typedef BOOL (WINAPI *PCMSCALLBACKW)(struct _tagCOLORMATCHSETUPW *,LPARAM);
typedef BOOL (WINAPI *PCMSCALLBACKA)(struct _tagCOLORMATCHSETUPA *,LPARAM);

typedef struct _tagCOLORMATCHSETUPW {

    DWORD   dwSize;                 //  Size of structure in bytes
    DWORD   dwVersion;              //  Set to COLOR_MATCH_VERSION

    DWORD   dwFlags;                //  See constants listed previously
    HWND    hwndOwner;              //  Window handle of owner

    PCWSTR  pSourceName;            //  Name of Image Source, defaults to "sRGB Color Space"
    PCWSTR  pDisplayName;           //  If null, defaults to first enumerated monitor
    PCWSTR  pPrinterName;           //  If null, defaults to default printer.

    DWORD   dwRenderIntent;         //  Rendering Intent
    DWORD   dwProofingIntent;       //  Rendering Intent for Proofing

    PWSTR   pMonitorProfile;        //  Monitor profile name
    DWORD   ccMonitorProfile;       //  Size of above in characters

    PWSTR   pPrinterProfile;        //  Printer profile name
    DWORD   ccPrinterProfile;       //  Size of above in characters

    PWSTR   pTargetProfile;         //  Target profile name
    DWORD   ccTargetProfile;        //  Size of above in characters

    DLGPROC lpfnHook;               //  Hook Procedure address
    LPARAM  lParam;                 //  Given to hook procedure at WM_INITDIALOG

    PCMSCALLBACKW lpfnApplyCallback;   //  Callback Procedure address when apply is pushed
    LPARAM        lParamApplyCallback; //  Given to callback Procedure for apply

}   COLORMATCHSETUPW, *PCOLORMATCHSETUPW, *LPCOLORMATCHSETUPW;

typedef struct _tagCOLORMATCHSETUPA {

    DWORD   dwSize;                 //  Size of structure in bytes
    DWORD   dwVersion;              //  Set to COLOR_MATCH_VERSION

    DWORD   dwFlags;                //  See constants listed previously
    HWND    hwndOwner;              //  Window handle of owner

    PCSTR   pSourceName;            //  Name of Image Source, defaults to  "This Document"
    PCSTR   pDisplayName;           //  If null, defaults to first enumerated monitor
    PCSTR   pPrinterName;           //  If null, defaults to default printer.

    DWORD   dwRenderIntent;         //  Rendering Intent
    DWORD   dwProofingIntent;       //  Rendering Intent for Proofing

    PSTR    pMonitorProfile;        //  Monitor profile name
    DWORD   ccMonitorProfile;       //  Size of above in characters

    PSTR    pPrinterProfile;        //  Printer profile name
    DWORD   ccPrinterProfile;       //  Size of above in characters

    PSTR    pTargetProfile;         //  Target profile name
    DWORD   ccTargetProfile;        //  Size of above in characters

    DLGPROC lpfnHook;               //  Hook Procedure address
    LPARAM  lParam;                 //  Given to hook procedure at WM_INITDIALOG

    PCMSCALLBACKA lpfnApplyCallback;   //  Callback Procedure address when apply is pushed
    LPARAM        lParamApplyCallback; //  Given to callback Procedure for apply

}   COLORMATCHSETUPA, *PCOLORMATCHSETUPA, *LPCOLORMATCHSETUPA;

//
// Windows API definitions
//

HPROFILE   WINAPI OpenColorProfileA(__in PPROFILE pProfile, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationMode);
HPROFILE   WINAPI OpenColorProfileW(__in PPROFILE pProfile, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationMode);
BOOL       WINAPI CloseColorProfile(__in_opt HPROFILE hProfile);
BOOL       WINAPI GetColorProfileFromHandle(__in HPROFILE hProfile, __out_bcount_opt(*pcbProfile) PBYTE pProfile, __inout PDWORD pcbProfile);
BOOL       WINAPI IsColorProfileValid(__in HPROFILE hProfile, __out PBOOL pbValid);
BOOL       WINAPI CreateProfileFromLogColorSpaceA(__in LPLOGCOLORSPACEA pLogColorSpace, __deref_out_xcount(GlobalSize(*pProfile)) PBYTE* pProfile);
BOOL       WINAPI CreateProfileFromLogColorSpaceW(__in LPLOGCOLORSPACEW pLogColorSpace, __deref_out_xcount(GlobalSize(*pProfile)) PBYTE* pProfile);
BOOL       WINAPI GetCountColorProfileElements(__in HPROFILE hProfile, __out PDWORD pnElementCount);
BOOL       WINAPI GetColorProfileHeader(__in HPROFILE hProfile, __out PPROFILEHEADER pHeader);
BOOL       WINAPI GetColorProfileElementTag(__in HPROFILE hProfile, DWORD dwIndex, __out PTAGTYPE pTag);
BOOL       WINAPI IsColorProfileTagPresent(__in HPROFILE hProfile, TAGTYPE tag, __out PBOOL pbPresent);
BOOL       WINAPI GetColorProfileElement(__in HPROFILE hProfile, TAGTYPE tag, DWORD dwOffset, __inout PDWORD pcbElement, __out_bcount_opt(*pcbElement) PVOID pElement, __out PBOOL pbReference);
BOOL       WINAPI SetColorProfileHeader(__in HPROFILE hProfile, __in_bcount(sizeof(PROFILEHEADER)) PPROFILEHEADER pHeader);
BOOL       WINAPI SetColorProfileElementSize(__in HPROFILE hProfile, TAGTYPE tagType, DWORD pcbElement);
BOOL       WINAPI SetColorProfileElement(__in HPROFILE hProfile, TAGTYPE tag, DWORD dwOffset, __in PDWORD pcbElement, __in_ecount(*pcbElement) PVOID pElement);
BOOL       WINAPI SetColorProfileElementReference(__in HPROFILE hProfile, TAGTYPE newTag, TAGTYPE refTag);
BOOL       WINAPI GetPS2ColorSpaceArray(__in HPROFILE hProfile, DWORD dwIntent, DWORD dwCSAType, __out_bcount_opt(*pcbPS2ColorSpaceArray) PBYTE pPS2ColorSpaceArray, __inout PDWORD pcbPS2ColorSpaceArray, __out PBOOL pbBinary);
BOOL       WINAPI GetPS2ColorRenderingIntent(__in HPROFILE hProfile, DWORD dwIntent, __out_bcount_opt(*pcbPS2ColorRenderingIntent) PBYTE pBuffer, __inout PDWORD pcbPS2ColorRenderingIntent);
BOOL       WINAPI GetPS2ColorRenderingDictionary(__in HPROFILE hProfile, DWORD dwIntent, __out_bcount_opt(*pcbPS2ColorRenderingDictionary) PBYTE pPS2ColorRenderingDictionary, __inout PDWORD pcbPS2ColorRenderingDictionary, __inout PBOOL pbBinary);
BOOL       WINAPI GetNamedProfileInfo(__in HPROFILE hProfile, __inout_bcount(sizeof(NAMED_PROFILE_INFO)) PNAMED_PROFILE_INFO pNamedProfileInfo);
BOOL       WINAPI ConvertColorNameToIndex(__in HPROFILE hProfile, __in_ecount(dwCount) PCOLOR_NAME paColorName, __out_ecount(dwCount) PDWORD paIndex, DWORD dwCount);
BOOL       WINAPI ConvertIndexToColorName(__in HPROFILE hProfile, __in_ecount(dwCount) PDWORD paIndex, __out_ecount(dwCount) PCOLOR_NAME paColorName, DWORD dwCount);
BOOL       WINAPI CreateDeviceLinkProfile(__in_ecount(nProfiles) PHPROFILE hProfile, DWORD nProfiles, __in_ecount(nIntents) PDWORD padwIntent, DWORD nIntents, DWORD dwFlags, __deref_out_xcount(GlobalSize(*pProfileData)) PBYTE* pProfileData, DWORD indexPreferredCMM);
HTRANSFORM WINAPI CreateColorTransformA(__in LPLOGCOLORSPACEA pLogColorSpace, __in HPROFILE hDestProfile, __in HPROFILE hTargetProfile, DWORD dwFlags);
HTRANSFORM WINAPI CreateColorTransformW(__in LPLOGCOLORSPACEW pLogColorSpace, __in HPROFILE hDestProfile, __in HPROFILE hTargetProfile, DWORD dwFlags);
HTRANSFORM WINAPI CreateMultiProfileTransform(__in_ecount(nProfiles) PHPROFILE pahProfiles, DWORD nProfiles, __in_ecount(nIntents) PDWORD padwIntent, DWORD nIntents, DWORD dwFlags, DWORD indexPreferredCMM);
BOOL       WINAPI DeleteColorTransform(__inout HTRANSFORM hxform);
BOOL       WINAPI TranslateBitmapBits(__in HTRANSFORM hColorTransform, __in PVOID pSrcBits, BMFORMAT bmInput, DWORD dwWidth, DWORD dwHeight, DWORD dwInputStride, __out PVOID pDestBits, BMFORMAT bmOutput, DWORD dwOutputStride, __in_opt __callback PBMCALLBACKFN pfnCallBack, __in_opt LPARAM ulCallbackData);
BOOL       WINAPI CheckBitmapBits(__in HTRANSFORM hColorTransform, __in PVOID pSrcBits, BMFORMAT bmInput, DWORD dwWidth, DWORD dwHeight, DWORD dwStride, __out_ecount(dwWidth * dwHeight) PBYTE paResult, __in_opt __callback PBMCALLBACKFN pfnCallback, __in_opt LPARAM lpCallbackData);
BOOL       WINAPI TranslateColors(__in HTRANSFORM hColorTransform, __in_ecount(nColors) PCOLOR paInputColors, DWORD nColors, COLORTYPE ctInput, __out_ecount(nColors) PCOLOR paOutputColors, COLORTYPE ctOutput);
BOOL       WINAPI CheckColors(__in HTRANSFORM hColorTransform, __in_ecount(nColors) PCOLOR paInputColors, DWORD nColors, COLORTYPE ctInput, __out_ecount(nColors) PBYTE paResult);
DWORD      WINAPI GetCMMInfo(__in HTRANSFORM hColorTransform, __in DWORD);
BOOL       WINAPI RegisterCMMA(__in_opt PCSTR pMachineName, __in DWORD cmmID, __in PCSTR pCMMdll);
BOOL       WINAPI RegisterCMMW(__in_opt PCWSTR pMachineName, __in DWORD cmmID, __in PCWSTR pCMMdll);
BOOL       WINAPI UnregisterCMMA(__in_opt PCSTR pMachineName, __in DWORD cmmID);
BOOL       WINAPI UnregisterCMMW(__in_opt PCWSTR pMachineName, __in DWORD cmmID);
BOOL       WINAPI SelectCMM(DWORD dwCMMType);

BOOL
WINAPI
GetColorDirectoryA(
    __in_opt PCSTR pMachineName,
    __out_bcount_part_opt(*pdwSize, *pdwSize) PSTR pBuffer,
    __inout PDWORD pdwSize
    );

BOOL
WINAPI
GetColorDirectoryW(
    __in_opt PCWSTR pMachineName,
    __out_bcount_part_opt(*pdwSize, *pdwSize) PWSTR pBuffer,
    __inout PDWORD pdwSize
    );


BOOL       WINAPI InstallColorProfileA(__in_opt PCSTR  pMachineName, __in PCSTR  pProfileName);
BOOL       WINAPI InstallColorProfileW(__in_opt PCWSTR pMachineName, __in PCWSTR pProfileName);
BOOL       WINAPI UninstallColorProfileA(__in_opt PCSTR  pMachineName, __in PCSTR pProfileName,  BOOL bDelete);
BOOL       WINAPI UninstallColorProfileW(__in_opt PCWSTR pMachineName, __in PCWSTR pProfileName, BOOL bDelete);
BOOL       WINAPI EnumColorProfilesA(__in_opt PCSTR  pMachineName, __in PENUMTYPEA pEnumRecord, __out_bcount_opt(*pdwSizeOfEnumerationBuffer) PBYTE pEnumerationBuffer, __inout PDWORD pdwSizeOfEnumerationBuffer, __out_opt PDWORD pnProfiles);
BOOL       WINAPI EnumColorProfilesW(__in_opt PCWSTR pMachineName, __in PENUMTYPEW pEnumRecord, __out_bcount_opt(*pdwSizeOfEnumerationBuffer) PBYTE pEnumerationBuffer, __inout PDWORD pdwSizeOfEnumerationBuffer, __out_opt PDWORD pnProfiles);
BOOL       WINAPI SetStandardColorSpaceProfileA(__in_opt PCSTR pMachineName,  DWORD dwProfileID, __in PCSTR  pProfilename);
BOOL       WINAPI SetStandardColorSpaceProfileW(__in_opt PCWSTR pMachineName, DWORD dwProfileID, __in PCWSTR pProfileName);

BOOL
WINAPI
GetStandardColorSpaceProfileA(
    __in_opt PCSTR pMachineName,
    __in DWORD dwSCS,
    __out_bcount_part_opt(*pcbSize, *pcbSize) PSTR pBuffer,
    __inout PDWORD pcbSize
    );

BOOL
WINAPI
GetStandardColorSpaceProfileW(
    __in_opt PCWSTR pMachineName,
    __in DWORD dwSCS,
    __out_bcount_part_opt(*pcbSize, *pcbSize) PWSTR pBuffer,
    __inout PDWORD pcbSize
    );

BOOL       WINAPI AssociateColorProfileWithDeviceA(__in_opt PCSTR pMachineName, __in PCSTR pProfileName, __in PCSTR pDeviceName);
BOOL       WINAPI AssociateColorProfileWithDeviceW(__in_opt PCWSTR pMachineName, __in PCWSTR pProfileName, __in PCWSTR pDeviceName);
BOOL       WINAPI DisassociateColorProfileFromDeviceA(__in_opt PCSTR pMachineName, __in PCSTR pProfileName, __in PCSTR pDeviceName);
BOOL       WINAPI DisassociateColorProfileFromDeviceW(__in_opt PCWSTR pMachineName, __in PCWSTR pProfileName, __in PCWSTR pDeviceName);
BOOL       WINAPI SetupColorMatchingW(__inout_bcount(sizeof(COLORMATCHSETUPW)) PCOLORMATCHSETUPW pcms);
BOOL       WINAPI SetupColorMatchingA(__inout_bcount(sizeof(COLORMATCHSETUPA)) PCOLORMATCHSETUPA pcms);

#if NTDDI_VERSION >= NTDDI_VISTA

BOOL
WINAPI
WcsAssociateColorProfileWithDevice(
    __in WCS_PROFILE_MANAGEMENT_SCOPE scope,
    __in PCWSTR pProfileName,
    __in PCWSTR pDeviceName
    );

BOOL
WINAPI
WcsDisassociateColorProfileFromDevice(
    __in WCS_PROFILE_MANAGEMENT_SCOPE scope,
    __in PCWSTR pProfileName,
    __in PCWSTR pDeviceName
    );

BOOL
WINAPI
WcsEnumColorProfilesSize(
    __in WCS_PROFILE_MANAGEMENT_SCOPE scope,
    __in PENUMTYPEW pEnumRecord,
    __out PDWORD pdwSize
    );

BOOL
WINAPI
WcsEnumColorProfiles(
    __in WCS_PROFILE_MANAGEMENT_SCOPE scope,
    __in PENUMTYPEW pEnumRecord,
    __out_bcount(dwSize) PBYTE pBuffer,
    __in DWORD dwSize,
    __out_opt PDWORD pnProfiles
    );

BOOL
WINAPI
WcsGetDefaultColorProfileSize(
    __in WCS_PROFILE_MANAGEMENT_SCOPE scope,
    __in_opt PCWSTR pDeviceName,
    __in COLORPROFILETYPE cptColorProfileType,
    __in COLORPROFILESUBTYPE cpstColorProfileSubType,
    __in DWORD dwProfileID,
    __out PDWORD pcbProfileName
    );

BOOL
WINAPI
WcsGetDefaultColorProfile(
    __in WCS_PROFILE_MANAGEMENT_SCOPE scope,
    __in_opt PCWSTR pDeviceName,
    __in COLORPROFILETYPE cptColorProfileType,
    __in COLORPROFILESUBTYPE cpstColorProfileSubType,
    __in DWORD dwProfileID,
    __in DWORD cbProfileName,
    __out_bcount(cbProfileName) LPWSTR pProfileName
    );

BOOL
WINAPI
WcsSetDefaultColorProfile(
    __in WCS_PROFILE_MANAGEMENT_SCOPE scope,
    __in_opt PCWSTR pDeviceName,
    __in COLORPROFILETYPE cptColorProfileType,
    __in COLORPROFILESUBTYPE cpstColorProfileSubType,
    __in DWORD dwProfileID,
    __in_opt LPCWSTR pProfileName
    );

BOOL
WINAPI
WcsSetDefaultRenderingIntent(
    __in WCS_PROFILE_MANAGEMENT_SCOPE scope,
    __in DWORD dwRenderingIntent
    );

BOOL
WINAPI
WcsGetDefaultRenderingIntent(
    __in WCS_PROFILE_MANAGEMENT_SCOPE scope,
    __out PDWORD pdwRenderingIntent
    );

BOOL
WINAPI
WcsGetUsePerUserProfiles(
    __in LPCWSTR pDeviceName,
    __in DWORD dwDeviceClass,
    __out PBOOL pUsePerUserProfiles
    );

BOOL
WINAPI
WcsSetUsePerUserProfiles(
    __in LPCWSTR pDeviceName,
    __in DWORD dwDeviceClass,
    __in BOOL usePerUserProfiles
    );

BOOL
WINAPI
WcsTranslateColors(
    __in HTRANSFORM hColorTransform,
    __in DWORD nColors,
    __in DWORD nInputChannels,
    __in COLORDATATYPE cdtInput,
    __in DWORD cbInput,
    __in_bcount(cbInput) PVOID pInputData,
    __in DWORD nOutputChannels,
    __in COLORDATATYPE cdtOutput,
    __in DWORD cbOutput,
    __out_bcount(cbOutput) PVOID pOutputData
    );

BOOL
WINAPI
WcsCheckColors(
    __in HTRANSFORM hColorTransform,
    __in DWORD nColors,
    __in DWORD nInputChannels,
    __in COLORDATATYPE cdtInput,
    __in DWORD cbInput,
    __in_bcount(cbInput) PVOID pInputData,
    __out_ecount(nColors) PBYTE paResult
    );

//
// The APIs SetupColorMatchingA and SetupColorMatchingW are
// deprecated as of Windows Vista. The implementations in icmui.dll
// now SetLastError to ERROR_NOT_SUPPORTED, and return FALSE.
//
#pragma deprecated (SetupColorMatchingA)
#pragma deprecated (SetupColorMatchingW)

#endif // NTDDI_VERSION >= NTDDI_VISTA

#ifdef UNICODE

#define ENUMTYPE                            ENUMTYPEW
#define PENUMTYPE                           PENUMTYPEW
#define COLORMATCHSETUP                     COLORMATCHSETUPW
#define PCOLORMATCHSETUP                    PCOLORMATCHSETUPW
#define LPCOLORMATCHSETUP                   LPCOLORMATCHSETUPW
#define PCMSCALLBACK                        PCMSCALLBACKW
#define CreateColorTransform                CreateColorTransformW
#define OpenColorProfile                    OpenColorProfileW
#define CreateProfileFromLogColorSpace      CreateProfileFromLogColorSpaceW
#define RegisterCMM                         RegisterCMMW
#define UnregisterCMM                       UnregisterCMMW
#define GetColorDirectory                   GetColorDirectoryW
#define InstallColorProfile                 InstallColorProfileW
#define UninstallColorProfile               UninstallColorProfileW
#define AssociateColorProfileWithDevice     AssociateColorProfileWithDeviceW
#define DisassociateColorProfileFromDevice  DisassociateColorProfileFromDeviceW
#define EnumColorProfiles                   EnumColorProfilesW
#define SetStandardColorSpaceProfile        SetStandardColorSpaceProfileW
#define GetStandardColorSpaceProfile        GetStandardColorSpaceProfileW
#define SetupColorMatching                  SetupColorMatchingW
#if NTDDI_VERSION >= NTDDI_VISTA
#define WcsOpenColorProfile                 WcsOpenColorProfileW
#endif // NTDDI_VERSION >= NTDDI_VISTA

#else

#define ENUMTYPE                            ENUMTYPEA
#define PENUMTYPE                           PENUMTYPEA
#define COLORMATCHSETUP                     COLORMATCHSETUPA
#define PCOLORMATCHSETUP                    PCOLORMATCHSETUPA
#define LPCOLORMATCHSETUP                   LPCOLORMATCHSETUPA
#define PCMSCALLBACK                        PCMSCALLBACKA
#define CreateColorTransform                CreateColorTransformA
#define OpenColorProfile                    OpenColorProfileA
#define CreateProfileFromLogColorSpace      CreateProfileFromLogColorSpaceA
#define RegisterCMM                         RegisterCMMA
#define UnregisterCMM                       UnregisterCMMA
#define GetColorDirectory                   GetColorDirectoryA
#define InstallColorProfile                 InstallColorProfileA
#define UninstallColorProfile               UninstallColorProfileA
#define AssociateColorProfileWithDevice     AssociateColorProfileWithDeviceA
#define DisassociateColorProfileFromDevice  DisassociateColorProfileFromDeviceA
#define EnumColorProfiles                   EnumColorProfilesA
#define SetStandardColorSpaceProfile        SetStandardColorSpaceProfileA
#define GetStandardColorSpaceProfile        GetStandardColorSpaceProfileA
#define SetupColorMatching                  SetupColorMatchingA
#if NTDDI_VERSION >= NTDDI_VISTA
#define WcsOpenColorProfile                 WcsOpenColorProfileA
#endif // NTDDI_VERSION >= NTDDI_VISTA

#endif  // !UNICODE

//
// Transform returned by CMM
//

typedef HANDLE  HCMTRANSFORM;

//
// Pointer to ICC color profile data.
//

typedef PVOID   LPDEVCHARACTER;

//
// CMM API definition
//

BOOL WINAPI CMCheckColors(
    __in                    HCMTRANSFORM hcmTransform,  // transform handle
    __in_ecount(nColors)    LPCOLOR lpaInputColors,     // array of COLORs
                            DWORD nColors,              // COLOR array size
                            COLORTYPE ctInput,          // input color type
    __out_ecount(nColors)   LPBYTE lpaResult            // buffer for results
    );

BOOL WINAPI CMCheckRGBs(
    __in                                HCMTRANSFORM hcmTransform,  // transform handle
    __in                                LPVOID lpSrcBits,           // source bitmap bits
                                        BMFORMAT bmInput,           // source bitmap format
                                        DWORD dwWidth,              // source bitmap width
                                        DWORD dwHeight,             // source bitmap hight
                                        DWORD dwStride,             // source bitmap delta
    __out_ecount(dwWidth * dwHeight)    LPBYTE lpaResult,           // buffer for results
    __callback                          PBMCALLBACKFN pfnCallback,  // pointer to callback function
                                        LPARAM ulCallbackData       // caller-defined parameter to callback
    );

BOOL WINAPI CMConvertColorNameToIndex(
    __in                  HPROFILE hProfile,
    __in_ecount(dwCount)  PCOLOR_NAME paColorName,
    __out_ecount(dwCount) PDWORD paIndex,
                          DWORD dwCount
    );

BOOL WINAPI CMConvertIndexToColorName(
    __in                  HPROFILE hProfile,
    __in_ecount(dwCount)  PDWORD paIndex,
    __out_ecount(dwCount) PCOLOR_NAME paColorName,
                          DWORD dwCount
    );

BOOL WINAPI CMCreateDeviceLinkProfile(
    __in_ecount(nProfiles)                          PHPROFILE pahProfiles,    // array of profile handles
                                                    DWORD nProfiles,          // profile handle array size
    __in_ecount(nIntents)                           PDWORD padwIntents,       // array of rendering intents
                                                    DWORD nIntents,           // intent array size
                                                    DWORD dwFlags,            // transform creation flags
    __deref_out_xcount(GlobalSize(*lpProfileData))  LPBYTE *lpProfileData     // pointer to pointer to buffer
    );

HCMTRANSFORM WINAPI CMCreateMultiProfileTransform(
    __in_ecount(nProfiles) PHPROFILE pahProfiles,    // array of profile handles
                           DWORD nProfiles,          // profile handle array size
    __in_ecount(nIntents)  PDWORD padwIntents,       // array of rendering intents
                           DWORD nIntents,           // intent array size
                           DWORD dwFlags             // transform creation flags
    );

BOOL WINAPI CMCreateProfileW(
    __inout LPLOGCOLORSPACEW lpColorSpace,  // pointer to a logical color space
    __out   LPDEVCHARACTER   *lpProfileData // pointer to pointer to buffer
    );

//
// The CMM APIs CMCreateTransform, CMCreateTransformW, and CMCreateTransformExt are
// deprecated as of Windows Vista, and CMM implementors are no longer required to
// implement them. The implementations in the default CMM (icm32.dll) in Windows Vista
// now SetLastError to ERROR_NOT_SUPPORTED, and return an invalid transform handle
// (a value < 256).
//
HCMTRANSFORM WINAPI CMCreateTransform(
    __in LPLOGCOLORSPACEA lpColorSpace,       // pointer to logical color space
    __in LPDEVCHARACTER lpDevCharacter,       // profile data
    __in LPDEVCHARACTER lpTargetDevCharacter  // target profile data
    );

HCMTRANSFORM WINAPI CMCreateTransformW(
    __in LPLOGCOLORSPACEW lpColorSpace,       // pointer to logical color space
    __in LPDEVCHARACTER lpDevCharacter,       // profile data
    __in LPDEVCHARACTER lpTargetDevCharacter  // target profile data
    );

HCMTRANSFORM WINAPI CMCreateTransformExt(
    __in LPLOGCOLORSPACEA lpColorSpace,        // pointer to logical color space
    __in LPDEVCHARACTER lpDevCharacter,        // profile data
    __in LPDEVCHARACTER lpTargetDevCharacter,  // target profile data
    __in DWORD dwFlags                         // creation flags
    );

//
// The CMM APIs CMCheckColorsInGamut, CMCreateProfile, CMTranslateRGB and CMTranslateRGBs are
// deprecated as of Windows 7, and CMM implementors are no longer required to
// implement them. The implementations in the default CMM (icm32.dll) in Windows 7
// now SetLastError to ERROR_NOT_SUPPORTED, and return FALSE.
//

BOOL WINAPI CMCheckColorsInGamut(
    __in                 HCMTRANSFORM    hcmTransform,  // transform handle
    __in_ecount(nCount)  RGBTRIPLE       *lpaRGBTriple, // RGB triple array
    __out_bcount(nCount) LPBYTE          lpaResult,     // buffer for results
                         UINT            nCount         // result buffer size
    );

BOOL WINAPI CMCreateProfile(
    __inout LPLOGCOLORSPACEA lpColorSpace,  // pointer to a logical color space
    __out   LPDEVCHARACTER   *lpProfileData // pointer to pointer to buffer
    );

BOOL WINAPI CMTranslateRGB(
    __in  HCMTRANSFORM hcmTransform,
          COLORREF ColorRef,
    __out LPCOLORREF lpColorRef,
          DWORD dwFlags
    );

BOOL WINAPI CMTranslateRGBs(
    __in  HCMTRANSFORM hcmTransform,
    __in  LPVOID lpSrcBits,
          BMFORMAT bmInput,
          DWORD dwWidth,
          DWORD dwHeight,
          DWORD dwStride,
    __out LPVOID lpDestBits,
          BMFORMAT bmOutput,
          DWORD dwTranslateDirection
    );

#if NTDDI_VERSION >= NTDDI_VISTA
#pragma deprecated (CMCreateTransform)
#pragma deprecated (CMCreateTransformW)
#pragma deprecated (CMCreateTransformExt)
#pragma deprecated (CMCheckColorsInGamut)
#pragma deprecated (CMCreateProfile)
#pragma deprecated (CMTranslateRGB)
#endif // NTDDI_VERSION >= NTDDI_VISTA

#if NTDDI_VERSION > NTDDI_VISTA
#pragma deprecated (CMTranslateRGBs)
#endif // NTDDI_VERSION > NTDDI_VISTA

HCMTRANSFORM WINAPI CMCreateTransformExtW(
         __in LPLOGCOLORSPACEW lpColorSpace,        // pointer to logical color space
         __in LPDEVCHARACTER lpDevCharacter,        // profile data
         __in LPDEVCHARACTER lpTargetDevCharacter,  // target profile data
         __in DWORD dwFlags                              // creation flags
    );

BOOL WINAPI CMDeleteTransform(
    __inout HCMTRANSFORM hcmTransform             // transform handle to be deleted.
    );

DWORD WINAPI CMGetInfo(
    DWORD dwInfo
    );

BOOL WINAPI CMGetNamedProfileInfo(
    __in    HPROFILE hProfile,                    // profile handle
    __inout PNAMED_PROFILE_INFO pNamedProfileInfo // pointer to named profile info
    );

BOOL WINAPI CMGetPS2ColorRenderingDictionary(
    __in                        HPROFILE hProfile,
                                DWORD dwIntent,
    __out_bcount_opt(*lpcbSize) LPBYTE lpBuffer,
    __inout                     LPDWORD lpcbSize,
    __inout                     LPBOOL lpbBinary
    );

BOOL WINAPI CMGetPS2ColorRenderingIntent(
    __in                        HPROFILE hProfile,
                                DWORD dwIntent,
    __out_bcount_opt(*lpcbSize) LPBYTE lpBuffer,
    __inout                     LPDWORD lpcbSize
    );

BOOL WINAPI CMGetPS2ColorSpaceArray(
    __in                        HPROFILE hProfile,
                                DWORD dwIntent,
                                DWORD dwCSAType,
    __out_bcount_opt(*lpcbSize) LPBYTE lpBuffer,
    __inout                     LPDWORD lpcbSize,
    __inout                     LPBOOL lpbBinary
    );

BOOL WINAPI CMIsProfileValid(
    __in    HPROFILE hProfile,  // proflle handle
    __out   LPBOOL lpbValid     // buffer for result.
    );

BOOL WINAPI CMTranslateColors(
    __in                  HCMTRANSFORM hcmTransform,          // transform handle
    __in_ecount(nColors)  LPCOLOR lpaInputColors,             // pointer to input color array
                          DWORD nColors,                      // number of color in color array
                          COLORTYPE ctInput,                  // input color type
    __out_ecount(nColors) LPCOLOR lpaOutputColors,            // pointer to output color array
                          COLORTYPE ctOutput                  // output color type
    );

BOOL WINAPI CMTranslateRGBsExt(
    __in       HCMTRANSFORM hcmTransform,
    __in       LPVOID lpSrcBits,
               BMFORMAT bmInput,
               DWORD dwWidth,
               DWORD dwHeight,
               DWORD dwInputStride,
    __out      LPVOID lpDestBits,
               BMFORMAT bmOutput,
               DWORD dwOutputStride,
    __callback LPBMCALLBACKFN lpfnCallback,
               LPARAM ulCallbackData
    );

#if NTDDI_VERSION >= NTDDI_VISTA

//
//    Windows Color System APIs
//


//
// Passed in as a bit-flag for dwFlags, this instructs WcsOpenColorProfile
// to ignore any embedded Wcs information in the WcsProfiles tag if the input
// profile is an ICC one.
//

#define DONT_USE_EMBEDDED_WCS_PROFILES 0x00000001L


HPROFILE WINAPI WcsOpenColorProfileA(
    __in        PPROFILE pCDMPProfile,
    __in_opt    PPROFILE pCAMPProfile,
    __in_opt    PPROFILE pGMMPProfile,
    __in        DWORD    dwDesireAccess,
    __in        DWORD    dwShareMode,
    __in        DWORD    dwCreationMode,
    __in        DWORD    dwFlags
    );

HPROFILE WINAPI WcsOpenColorProfileW(
    __in        PPROFILE pCDMPProfile,
    __in_opt    PPROFILE pCAMPProfile,
    __in_opt    PPROFILE pGMMPProfile,
    __in        DWORD    dwDesireAccess,
    __in        DWORD    dwShareMode,
    __in        DWORD    dwCreationMode,
    __in        DWORD    dwFlags
    );

//
// Flags for WcsCreateIccProfile.
//
// By default, the original WCS profiles used for the conversion are embedded
// in the output ICC profile in a WcsProfilesTag, resulting in an ICC profile
// which is compatible with ICC software, yet still has the original WCS
// profile data available to code which is designed to parse it.
//
// This flag is set to produce an ICC profile without any of the original
// WCS profile data embedded. The WcsProfilesTag will not be present in the
// output profile. This will result in a smaller output profile, however the
// original WCS profile data will not be available to code that would be able
// to parse it instead of the ICC data.
//
//
// These flags should be ORed in with one of the four ICC INTENT_ flags
// Thus, they must use different bits from the INTENT_ flags. Currrently, the INTENT_ flags
// use the values 0-3, or the low order two bits. Hence we use the third lowest bit.
//


#define WCS_DEFAULT 0x00000000L
#define WCS_ICCONLY 0x00010000L

HPROFILE WINAPI WcsCreateIccProfile(
    __in HPROFILE hWcsProfile,
    __in DWORD dwOptions
    );

//
// ICC TAGTYPE for the embedded Wcs information.
//

#define WCS_EMBEDDED_TAG_SIGNATURE 'MS00'
#define WCS_EMBEDDED_TAG_TYPE_SIGNATURE 'MS10'

#endif // NTDDI_VERSION >= NTDDI_VISTA

#if NTDDI_VERSION >= NTDDI_WIN7

BOOL WcsGetCalibrationManagementState (
    __out BOOL *pbIsEnabled
    );

BOOL WcsSetCalibrationManagementState (
    __in BOOL bIsEnabled
    );
    
#endif // NTDDI_VERSION >= NTDDI_WIN7
    

#ifdef __cplusplus
}
#endif

#endif  // ifndef _ICM_H_


