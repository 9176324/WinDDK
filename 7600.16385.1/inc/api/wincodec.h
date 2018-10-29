

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* Compiler settings for wincodec.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

/* verify that the <rpcsal.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __wincodec_h__
#define __wincodec_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IWICPalette_FWD_DEFINED__
#define __IWICPalette_FWD_DEFINED__
typedef interface IWICPalette IWICPalette;
#endif 	/* __IWICPalette_FWD_DEFINED__ */


#ifndef __IWICBitmapSource_FWD_DEFINED__
#define __IWICBitmapSource_FWD_DEFINED__
typedef interface IWICBitmapSource IWICBitmapSource;
#endif 	/* __IWICBitmapSource_FWD_DEFINED__ */


#ifndef __IWICFormatConverter_FWD_DEFINED__
#define __IWICFormatConverter_FWD_DEFINED__
typedef interface IWICFormatConverter IWICFormatConverter;
#endif 	/* __IWICFormatConverter_FWD_DEFINED__ */


#ifndef __IWICBitmapScaler_FWD_DEFINED__
#define __IWICBitmapScaler_FWD_DEFINED__
typedef interface IWICBitmapScaler IWICBitmapScaler;
#endif 	/* __IWICBitmapScaler_FWD_DEFINED__ */


#ifndef __IWICBitmapClipper_FWD_DEFINED__
#define __IWICBitmapClipper_FWD_DEFINED__
typedef interface IWICBitmapClipper IWICBitmapClipper;
#endif 	/* __IWICBitmapClipper_FWD_DEFINED__ */


#ifndef __IWICBitmapFlipRotator_FWD_DEFINED__
#define __IWICBitmapFlipRotator_FWD_DEFINED__
typedef interface IWICBitmapFlipRotator IWICBitmapFlipRotator;
#endif 	/* __IWICBitmapFlipRotator_FWD_DEFINED__ */


#ifndef __IWICBitmapLock_FWD_DEFINED__
#define __IWICBitmapLock_FWD_DEFINED__
typedef interface IWICBitmapLock IWICBitmapLock;
#endif 	/* __IWICBitmapLock_FWD_DEFINED__ */


#ifndef __IWICBitmap_FWD_DEFINED__
#define __IWICBitmap_FWD_DEFINED__
typedef interface IWICBitmap IWICBitmap;
#endif 	/* __IWICBitmap_FWD_DEFINED__ */


#ifndef __IWICColorContext_FWD_DEFINED__
#define __IWICColorContext_FWD_DEFINED__
typedef interface IWICColorContext IWICColorContext;
#endif 	/* __IWICColorContext_FWD_DEFINED__ */


#ifndef __IWICColorTransform_FWD_DEFINED__
#define __IWICColorTransform_FWD_DEFINED__
typedef interface IWICColorTransform IWICColorTransform;
#endif 	/* __IWICColorTransform_FWD_DEFINED__ */


#ifndef __IWICFastMetadataEncoder_FWD_DEFINED__
#define __IWICFastMetadataEncoder_FWD_DEFINED__
typedef interface IWICFastMetadataEncoder IWICFastMetadataEncoder;
#endif 	/* __IWICFastMetadataEncoder_FWD_DEFINED__ */


#ifndef __IWICStream_FWD_DEFINED__
#define __IWICStream_FWD_DEFINED__
typedef interface IWICStream IWICStream;
#endif 	/* __IWICStream_FWD_DEFINED__ */


#ifndef __IWICEnumMetadataItem_FWD_DEFINED__
#define __IWICEnumMetadataItem_FWD_DEFINED__
typedef interface IWICEnumMetadataItem IWICEnumMetadataItem;
#endif 	/* __IWICEnumMetadataItem_FWD_DEFINED__ */


#ifndef __IWICMetadataQueryReader_FWD_DEFINED__
#define __IWICMetadataQueryReader_FWD_DEFINED__
typedef interface IWICMetadataQueryReader IWICMetadataQueryReader;
#endif 	/* __IWICMetadataQueryReader_FWD_DEFINED__ */


#ifndef __IWICMetadataQueryWriter_FWD_DEFINED__
#define __IWICMetadataQueryWriter_FWD_DEFINED__
typedef interface IWICMetadataQueryWriter IWICMetadataQueryWriter;
#endif 	/* __IWICMetadataQueryWriter_FWD_DEFINED__ */


#ifndef __IWICBitmapEncoder_FWD_DEFINED__
#define __IWICBitmapEncoder_FWD_DEFINED__
typedef interface IWICBitmapEncoder IWICBitmapEncoder;
#endif 	/* __IWICBitmapEncoder_FWD_DEFINED__ */


#ifndef __IWICBitmapFrameEncode_FWD_DEFINED__
#define __IWICBitmapFrameEncode_FWD_DEFINED__
typedef interface IWICBitmapFrameEncode IWICBitmapFrameEncode;
#endif 	/* __IWICBitmapFrameEncode_FWD_DEFINED__ */


#ifndef __IWICBitmapDecoder_FWD_DEFINED__
#define __IWICBitmapDecoder_FWD_DEFINED__
typedef interface IWICBitmapDecoder IWICBitmapDecoder;
#endif 	/* __IWICBitmapDecoder_FWD_DEFINED__ */


#ifndef __IWICBitmapSourceTransform_FWD_DEFINED__
#define __IWICBitmapSourceTransform_FWD_DEFINED__
typedef interface IWICBitmapSourceTransform IWICBitmapSourceTransform;
#endif 	/* __IWICBitmapSourceTransform_FWD_DEFINED__ */


#ifndef __IWICBitmapFrameDecode_FWD_DEFINED__
#define __IWICBitmapFrameDecode_FWD_DEFINED__
typedef interface IWICBitmapFrameDecode IWICBitmapFrameDecode;
#endif 	/* __IWICBitmapFrameDecode_FWD_DEFINED__ */


#ifndef __IWICProgressiveLevelControl_FWD_DEFINED__
#define __IWICProgressiveLevelControl_FWD_DEFINED__
typedef interface IWICProgressiveLevelControl IWICProgressiveLevelControl;
#endif 	/* __IWICProgressiveLevelControl_FWD_DEFINED__ */


#ifndef __IWICProgressCallback_FWD_DEFINED__
#define __IWICProgressCallback_FWD_DEFINED__
typedef interface IWICProgressCallback IWICProgressCallback;
#endif 	/* __IWICProgressCallback_FWD_DEFINED__ */


#ifndef __IWICBitmapCodecProgressNotification_FWD_DEFINED__
#define __IWICBitmapCodecProgressNotification_FWD_DEFINED__
typedef interface IWICBitmapCodecProgressNotification IWICBitmapCodecProgressNotification;
#endif 	/* __IWICBitmapCodecProgressNotification_FWD_DEFINED__ */


#ifndef __IWICComponentInfo_FWD_DEFINED__
#define __IWICComponentInfo_FWD_DEFINED__
typedef interface IWICComponentInfo IWICComponentInfo;
#endif 	/* __IWICComponentInfo_FWD_DEFINED__ */


#ifndef __IWICFormatConverterInfo_FWD_DEFINED__
#define __IWICFormatConverterInfo_FWD_DEFINED__
typedef interface IWICFormatConverterInfo IWICFormatConverterInfo;
#endif 	/* __IWICFormatConverterInfo_FWD_DEFINED__ */


#ifndef __IWICBitmapCodecInfo_FWD_DEFINED__
#define __IWICBitmapCodecInfo_FWD_DEFINED__
typedef interface IWICBitmapCodecInfo IWICBitmapCodecInfo;
#endif 	/* __IWICBitmapCodecInfo_FWD_DEFINED__ */


#ifndef __IWICBitmapEncoderInfo_FWD_DEFINED__
#define __IWICBitmapEncoderInfo_FWD_DEFINED__
typedef interface IWICBitmapEncoderInfo IWICBitmapEncoderInfo;
#endif 	/* __IWICBitmapEncoderInfo_FWD_DEFINED__ */


#ifndef __IWICBitmapDecoderInfo_FWD_DEFINED__
#define __IWICBitmapDecoderInfo_FWD_DEFINED__
typedef interface IWICBitmapDecoderInfo IWICBitmapDecoderInfo;
#endif 	/* __IWICBitmapDecoderInfo_FWD_DEFINED__ */


#ifndef __IWICPixelFormatInfo_FWD_DEFINED__
#define __IWICPixelFormatInfo_FWD_DEFINED__
typedef interface IWICPixelFormatInfo IWICPixelFormatInfo;
#endif 	/* __IWICPixelFormatInfo_FWD_DEFINED__ */


#ifndef __IWICPixelFormatInfo2_FWD_DEFINED__
#define __IWICPixelFormatInfo2_FWD_DEFINED__
typedef interface IWICPixelFormatInfo2 IWICPixelFormatInfo2;
#endif 	/* __IWICPixelFormatInfo2_FWD_DEFINED__ */


#ifndef __IWICImagingFactory_FWD_DEFINED__
#define __IWICImagingFactory_FWD_DEFINED__
typedef interface IWICImagingFactory IWICImagingFactory;
#endif 	/* __IWICImagingFactory_FWD_DEFINED__ */


#ifndef __IWICDevelopRawNotificationCallback_FWD_DEFINED__
#define __IWICDevelopRawNotificationCallback_FWD_DEFINED__
typedef interface IWICDevelopRawNotificationCallback IWICDevelopRawNotificationCallback;
#endif 	/* __IWICDevelopRawNotificationCallback_FWD_DEFINED__ */


#ifndef __IWICDevelopRaw_FWD_DEFINED__
#define __IWICDevelopRaw_FWD_DEFINED__
typedef interface IWICDevelopRaw IWICDevelopRaw;
#endif 	/* __IWICDevelopRaw_FWD_DEFINED__ */


/* header files for imported files */
#include "wtypes.h"
#include "propidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_wincodec_0000_0000 */
/* [local] */ 

#include <intsafe.h>
#define WINCODEC_SDK_VERSION 0x0236
DEFINE_GUID(CLSID_WICImagingFactory, 0xcacaf262, 0x9370, 0x4615, 0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0xa);
DEFINE_GUID(GUID_VendorMicrosoft, 0xf0e749ca, 0xedef, 0x4589, 0xa7, 0x3a, 0xee, 0xe, 0x62, 0x6a, 0x2a, 0x2b);
DEFINE_GUID(GUID_VendorMicrosoftBuiltIn, 0x257a30fd, 0x6b6, 0x462b, 0xae, 0xa4, 0x63, 0xf7, 0xb, 0x86, 0xe5, 0x33);
DEFINE_GUID(CLSID_WICBmpDecoder,  0x6b462062, 0x7cbf, 0x400d, 0x9f, 0xdb, 0x81, 0x3d, 0xd1, 0x0f, 0x27, 0x78);
DEFINE_GUID(CLSID_WICPngDecoder,  0x389ea17b, 0x5078, 0x4cde, 0xb6, 0xef, 0x25, 0xc1, 0x51, 0x75, 0xc7, 0x51);
DEFINE_GUID(CLSID_WICIcoDecoder,  0xc61bfcdf, 0x2e0f, 0x4aad, 0xa8, 0xd7, 0xe0, 0x6b, 0xaf, 0xeb, 0xcd, 0xfe);
DEFINE_GUID(CLSID_WICJpegDecoder, 0x9456a480, 0xe88b, 0x43ea, 0x9e, 0x73, 0x0b, 0x2d, 0x9b, 0x71, 0xb1, 0xca);
DEFINE_GUID(CLSID_WICGifDecoder,  0x381dda3c, 0x9ce9, 0x4834, 0xa2, 0x3e, 0x1f, 0x98, 0xf8, 0xfc, 0x52, 0xbe);
DEFINE_GUID(CLSID_WICTiffDecoder, 0xb54e85d9, 0xfe23, 0x499f, 0x8b, 0x88, 0x6a, 0xce, 0xa7, 0x13, 0x75, 0x2b);
DEFINE_GUID(CLSID_WICWmpDecoder,  0xa26cec36, 0x234c, 0x4950, 0xae, 0x16, 0xe3, 0x4a, 0xac, 0xe7, 0x1d, 0x0d);
DEFINE_GUID(CLSID_WICBmpEncoder,  0x69be8bb4, 0xd66d, 0x47c8, 0x86, 0x5a, 0xed, 0x15, 0x89, 0x43, 0x37, 0x82);
DEFINE_GUID(CLSID_WICPngEncoder,  0x27949969, 0x876a, 0x41d7, 0x94, 0x47, 0x56, 0x8f, 0x6a, 0x35, 0xa4, 0xdc);
DEFINE_GUID(CLSID_WICJpegEncoder, 0x1a34f5c1, 0x4a5a, 0x46dc, 0xb6, 0x44, 0x1f, 0x45, 0x67, 0xe7, 0xa6, 0x76);
DEFINE_GUID(CLSID_WICGifEncoder,  0x114f5598, 0x0b22, 0x40a0, 0x86, 0xa1, 0xc8, 0x3e, 0xa4, 0x95, 0xad, 0xbd);
DEFINE_GUID(CLSID_WICTiffEncoder, 0x0131be10, 0x2001, 0x4c5f, 0xa9, 0xb0, 0xcc, 0x88, 0xfa, 0xb6, 0x4c, 0xe8);
DEFINE_GUID(CLSID_WICWmpEncoder,  0xac4ce3cb, 0xe1c1, 0x44cd, 0x82, 0x15, 0x5a, 0x16, 0x65, 0x50, 0x9e, 0xc2);
DEFINE_GUID(GUID_ContainerFormatBmp,  0x0af1d87e, 0xfcfe, 0x4188, 0xbd, 0xeb, 0xa7, 0x90, 0x64, 0x71, 0xcb, 0xe3);
DEFINE_GUID(GUID_ContainerFormatPng,  0x1b7cfaf4, 0x713f, 0x473c, 0xbb, 0xcd, 0x61, 0x37, 0x42, 0x5f, 0xae, 0xaf);
DEFINE_GUID(GUID_ContainerFormatIco,  0xa3a860c4, 0x338f, 0x4c17, 0x91, 0x9a, 0xfb, 0xa4, 0xb5, 0x62, 0x8f, 0x21);
DEFINE_GUID(GUID_ContainerFormatJpeg, 0x19e4a5aa, 0x5662, 0x4fc5, 0xa0, 0xc0, 0x17, 0x58, 0x02, 0x8e, 0x10, 0x57);
DEFINE_GUID(GUID_ContainerFormatTiff, 0x163bcc30, 0xe2e9, 0x4f0b, 0x96, 0x1d, 0xa3, 0xe9, 0xfd, 0xb7, 0x88, 0xa3);
DEFINE_GUID(GUID_ContainerFormatGif,  0x1f8a5601, 0x7d4d, 0x4cbd, 0x9c, 0x82, 0x1b, 0xc8, 0xd4, 0xee, 0xb9, 0xa5);
DEFINE_GUID(GUID_ContainerFormatWmp,  0x57a37caa, 0x367a, 0x4540, 0x91, 0x6b, 0xf1, 0x83, 0xc5, 0x09, 0x3a, 0x4b);
DEFINE_GUID(CLSID_WICImagingCategories, 0xfae3d380, 0xfea4, 0x4623, 0x8c, 0x75, 0xc6, 0xb6, 0x11, 0x10, 0xb6, 0x81);
DEFINE_GUID(CATID_WICBitmapDecoders,    0x7ed96837, 0x96f0, 0x4812, 0xb2, 0x11, 0xf1, 0x3c, 0x24, 0x11, 0x7e, 0xd3);
DEFINE_GUID(CATID_WICBitmapEncoders,    0xac757296, 0x3522, 0x4e11, 0x98, 0x62, 0xc1, 0x7b, 0xe5, 0xa1, 0x76, 0x7e);
DEFINE_GUID(CATID_WICPixelFormats,      0x2b46e70f, 0xcda7, 0x473e, 0x89, 0xf6, 0xdc, 0x96, 0x30, 0xa2, 0x39, 0x0b);
DEFINE_GUID(CATID_WICFormatConverters,  0x7835eae8, 0xbf14, 0x49d1, 0x93, 0xce, 0x53, 0x3a, 0x40, 0x7b, 0x22, 0x48);
DEFINE_GUID(CATID_WICMetadataReader,    0x05af94d8, 0x7174, 0x4cd2, 0xbe, 0x4a, 0x41, 0x24, 0xb8, 0x0e, 0xe4, 0xb8);
DEFINE_GUID(CATID_WICMetadataWriter,    0xabe3b9a4, 0x257d, 0x4b97, 0xbd, 0x1a, 0x29, 0x4a, 0xf4, 0x96, 0x22, 0x2e);
DEFINE_GUID(CLSID_WICDefaultFormatConverter, 0x1a3f11dc, 0xb514, 0x4b17, 0x8c, 0x5f, 0x21, 0x54, 0x51, 0x38, 0x52, 0xf1);
DEFINE_GUID(CLSID_WICFormatConverterHighColor, 0xac75d454, 0x9f37, 0x48f8, 0xb9, 0x72, 0x4e, 0x19, 0xbc, 0x85, 0x60, 0x11);
DEFINE_GUID(CLSID_WICFormatConverterNChannel, 0xc17cabb2, 0xd4a3, 0x47d7, 0xa5, 0x57, 0x33, 0x9b, 0x2e, 0xfb, 0xd4, 0xf1);
DEFINE_GUID(CLSID_WICFormatConverterWMPhoto, 0x9cb5172b, 0xd600, 0x46ba, 0xab, 0x77, 0x77, 0xbb, 0x7e, 0x3a, 0x00, 0xd9);























typedef UINT32 WICColor;

typedef /* [public] */ struct WICRect
    {
    INT X;
    INT Y;
    INT Width;
    INT Height;
    } 	WICRect;

typedef BYTE* WICInProcPointer;
#if 0
typedef /* [wire_marshal] */ void *WICInProcPointer;

#endif
typedef /* [public] */ 
enum WICColorContextType
    {	WICColorContextUninitialized	= 0,
	WICColorContextProfile	= 0x1,
	WICColorContextExifColorSpace	= 0x2
    } 	WICColorContextType;

typedef /* [public] */ REFGUID REFWICPixelFormatGUID;

typedef /* [public] */ GUID WICPixelFormatGUID;

#define GUID_WICPixelFormatUndefined GUID_WICPixelFormatDontCare
DEFINE_GUID(GUID_WICPixelFormatDontCare, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x00);
DEFINE_GUID(GUID_WICPixelFormat1bppIndexed, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x01);
DEFINE_GUID(GUID_WICPixelFormat2bppIndexed, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x02);
DEFINE_GUID(GUID_WICPixelFormat4bppIndexed, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x03);
DEFINE_GUID(GUID_WICPixelFormat8bppIndexed, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x04);
DEFINE_GUID(GUID_WICPixelFormatBlackWhite, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x05);
DEFINE_GUID(GUID_WICPixelFormat2bppGray,   0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x06);
DEFINE_GUID(GUID_WICPixelFormat4bppGray,   0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x07);
DEFINE_GUID(GUID_WICPixelFormat8bppGray,   0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x08);
DEFINE_GUID(GUID_WICPixelFormat8bppAlpha, 0xe6cd0116, 0xeeba, 0x4161, 0xaa, 0x85, 0x27, 0xdd, 0x9f, 0xb3, 0xa8, 0x95);
DEFINE_GUID(GUID_WICPixelFormat16bppBGR555, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x09);
DEFINE_GUID(GUID_WICPixelFormat16bppBGR565, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x0a);
DEFINE_GUID(GUID_WICPixelFormat16bppBGRA5551, 0x05ec7c2b, 0xf1e6, 0x4961, 0xad, 0x46, 0xe1, 0xcc, 0x81, 0x0a, 0x87, 0xd2);
DEFINE_GUID(GUID_WICPixelFormat16bppGray,   0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x0b);
DEFINE_GUID(GUID_WICPixelFormat24bppBGR, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x0c);
DEFINE_GUID(GUID_WICPixelFormat24bppRGB, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x0d);
DEFINE_GUID(GUID_WICPixelFormat32bppBGR,   0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x0e);
DEFINE_GUID(GUID_WICPixelFormat32bppBGRA,  0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x0f);
DEFINE_GUID(GUID_WICPixelFormat32bppPBGRA, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x10);
DEFINE_GUID(GUID_WICPixelFormat32bppGrayFloat,  0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x11);
DEFINE_GUID(GUID_WICPixelFormat32bppRGBA, 0xf5c7ad2d, 0x6a8d, 0x43dd, 0xa7, 0xa8, 0xa2, 0x99, 0x35, 0x26, 0x1a, 0xe9);
DEFINE_GUID(GUID_WICPixelFormat32bppPRGBA, 0x3cc4a650, 0xa527, 0x4d37, 0xa9, 0x16, 0x31, 0x42, 0xc7, 0xeb, 0xed, 0xba);
DEFINE_GUID(GUID_WICPixelFormat48bppRGB, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x15);
DEFINE_GUID(GUID_WICPixelFormat48bppBGR, 0xe605a384, 0xb468, 0x46ce, 0xbb, 0x2e, 0x36, 0xf1, 0x80, 0xe6, 0x43, 0x13);
DEFINE_GUID(GUID_WICPixelFormat64bppRGBA,  0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x16);
DEFINE_GUID(GUID_WICPixelFormat64bppBGRA,  0x1562ff7c, 0xd352, 0x46f9, 0x97, 0x9e, 0x42, 0x97, 0x6b, 0x79, 0x22, 0x46);
DEFINE_GUID(GUID_WICPixelFormat64bppPRGBA, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x17);
DEFINE_GUID(GUID_WICPixelFormat64bppPBGRA, 0x8c518e8e, 0xa4ec, 0x468b, 0xae, 0x70, 0xc9, 0xa3, 0x5a, 0x9c, 0x55, 0x30);
DEFINE_GUID(GUID_WICPixelFormat16bppGrayFixedPoint, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x13);
DEFINE_GUID(GUID_WICPixelFormat32bppBGR101010, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x14);
DEFINE_GUID(GUID_WICPixelFormat48bppRGBFixedPoint, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x12);
DEFINE_GUID(GUID_WICPixelFormat48bppBGRFixedPoint, 0x49ca140e, 0xcab6, 0x493b, 0x9d, 0xdf, 0x60, 0x18, 0x7c, 0x37, 0x53, 0x2a);
DEFINE_GUID(GUID_WICPixelFormat96bppRGBFixedPoint, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x18);
DEFINE_GUID(GUID_WICPixelFormat128bppRGBAFloat,  0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x19);
DEFINE_GUID(GUID_WICPixelFormat128bppPRGBAFloat, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x1a);
DEFINE_GUID(GUID_WICPixelFormat128bppRGBFloat,   0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x1b);
DEFINE_GUID(GUID_WICPixelFormat32bppCMYK, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x1c);
DEFINE_GUID(GUID_WICPixelFormat64bppRGBAFixedPoint, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x1d);
DEFINE_GUID(GUID_WICPixelFormat64bppBGRAFixedPoint, 0x356de33c, 0x54d2, 0x4a23, 0xbb, 0x4, 0x9b, 0x7b, 0xf9, 0xb1, 0xd4, 0x2d);
DEFINE_GUID(GUID_WICPixelFormat64bppRGBFixedPoint, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x40);
DEFINE_GUID(GUID_WICPixelFormat128bppRGBAFixedPoint, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x1e);
DEFINE_GUID(GUID_WICPixelFormat128bppRGBFixedPoint, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x41);
DEFINE_GUID(GUID_WICPixelFormat64bppRGBAHalf, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x3a);
DEFINE_GUID(GUID_WICPixelFormat64bppRGBHalf, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x42);
DEFINE_GUID(GUID_WICPixelFormat48bppRGBHalf, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x3b);
DEFINE_GUID(GUID_WICPixelFormat32bppRGBE, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x3d);
DEFINE_GUID(GUID_WICPixelFormat16bppGrayHalf, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x3e);
DEFINE_GUID(GUID_WICPixelFormat32bppGrayFixedPoint, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x3f);
DEFINE_GUID(GUID_WICPixelFormat32bppRGBA1010102, 0x25238D72, 0xFCF9, 0x4522, 0xb5, 0x14, 0x55, 0x78, 0xe5, 0xad, 0x55, 0xe0);
DEFINE_GUID(GUID_WICPixelFormat32bppRGBA1010102XR, 0x00DE6B9A, 0xC101, 0x434b, 0xb5, 0x02, 0xd0, 0x16, 0x5e, 0xe1, 0x12, 0x2c);
DEFINE_GUID(GUID_WICPixelFormat64bppCMYK, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x1f);
DEFINE_GUID(GUID_WICPixelFormat24bpp3Channels, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x20);
DEFINE_GUID(GUID_WICPixelFormat32bpp4Channels, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x21);
DEFINE_GUID(GUID_WICPixelFormat40bpp5Channels, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x22);
DEFINE_GUID(GUID_WICPixelFormat48bpp6Channels, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x23);
DEFINE_GUID(GUID_WICPixelFormat56bpp7Channels, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x24);
DEFINE_GUID(GUID_WICPixelFormat64bpp8Channels, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x25);
DEFINE_GUID(GUID_WICPixelFormat48bpp3Channels, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x26);
DEFINE_GUID(GUID_WICPixelFormat64bpp4Channels, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x27);
DEFINE_GUID(GUID_WICPixelFormat80bpp5Channels, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x28);
DEFINE_GUID(GUID_WICPixelFormat96bpp6Channels, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x29);
DEFINE_GUID(GUID_WICPixelFormat112bpp7Channels, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x2a);
DEFINE_GUID(GUID_WICPixelFormat128bpp8Channels, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x2b);
DEFINE_GUID(GUID_WICPixelFormat40bppCMYKAlpha, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x2c);
DEFINE_GUID(GUID_WICPixelFormat80bppCMYKAlpha, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x2d);
DEFINE_GUID(GUID_WICPixelFormat32bpp3ChannelsAlpha, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x2e);
DEFINE_GUID(GUID_WICPixelFormat40bpp4ChannelsAlpha, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x2f);
DEFINE_GUID(GUID_WICPixelFormat48bpp5ChannelsAlpha, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x30);
DEFINE_GUID(GUID_WICPixelFormat56bpp6ChannelsAlpha, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x31);
DEFINE_GUID(GUID_WICPixelFormat64bpp7ChannelsAlpha, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x32);
DEFINE_GUID(GUID_WICPixelFormat72bpp8ChannelsAlpha, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x33);
DEFINE_GUID(GUID_WICPixelFormat64bpp3ChannelsAlpha, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x34);
DEFINE_GUID(GUID_WICPixelFormat80bpp4ChannelsAlpha, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x35);
DEFINE_GUID(GUID_WICPixelFormat96bpp5ChannelsAlpha, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x36);
DEFINE_GUID(GUID_WICPixelFormat112bpp6ChannelsAlpha, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x37);
DEFINE_GUID(GUID_WICPixelFormat128bpp7ChannelsAlpha, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x38);
DEFINE_GUID(GUID_WICPixelFormat144bpp8ChannelsAlpha, 0x6fddc324, 0x4e03, 0x4bfe, 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x39);
typedef /* [public] */ 
enum WICBitmapCreateCacheOption
    {	WICBitmapNoCache	= 0,
	WICBitmapCacheOnDemand	= 0x1,
	WICBitmapCacheOnLoad	= 0x2,
	WICBITMAPCREATECACHEOPTION_FORCE_DWORD	= 0x7fffffff
    } 	WICBitmapCreateCacheOption;

typedef /* [public] */ 
enum WICDecodeOptions
    {	WICDecodeMetadataCacheOnDemand	= 0,
	WICDecodeMetadataCacheOnLoad	= 0x1,
	WICMETADATACACHEOPTION_FORCE_DWORD	= 0x7fffffff
    } 	WICDecodeOptions;

typedef /* [public] */ 
enum WICBitmapEncoderCacheOption
    {	WICBitmapEncoderCacheInMemory	= 0,
	WICBitmapEncoderCacheTempFile	= 0x1,
	WICBitmapEncoderNoCache	= 0x2,
	WICBITMAPENCODERCACHEOPTION_FORCE_DWORD	= 0x7fffffff
    } 	WICBitmapEncoderCacheOption;

typedef /* [public] */ 
enum WICComponentType
    {	WICDecoder	= 0x1,
	WICEncoder	= 0x2,
	WICPixelFormatConverter	= 0x4,
	WICMetadataReader	= 0x8,
	WICMetadataWriter	= 0x10,
	WICPixelFormat	= 0x20,
	WICAllComponents	= 0x3f,
	WICCOMPONENTTYPE_FORCE_DWORD	= 0x7fffffff
    } 	WICComponentType;

typedef /* [public] */ 
enum WICComponentEnumerateOptions
    {	WICComponentEnumerateDefault	= 0,
	WICComponentEnumerateRefresh	= 0x1,
	WICComponentEnumerateDisabled	= 0x80000000,
	WICComponentEnumerateUnsigned	= 0x40000000,
	WICComponentEnumerateBuiltInOnly	= 0x20000000,
	WICCOMPONENTENUMERATEOPTIONS_FORCE_DWORD	= 0x7fffffff
    } 	WICComponentEnumerateOptions;

typedef /* [public] */ struct WICBitmapPattern
    {
    ULARGE_INTEGER Position;
    ULONG Length;
    BYTE *Pattern;
    BYTE *Mask;
    BOOL EndOfStream;
    } 	WICBitmapPattern;

typedef /* [public] */ 
enum WICBitmapInterpolationMode
    {	WICBitmapInterpolationModeNearestNeighbor	= 0,
	WICBitmapInterpolationModeLinear	= 0x1,
	WICBitmapInterpolationModeCubic	= 0x2,
	WICBitmapInterpolationModeFant	= 0x3,
	WICBITMAPINTERPOLATIONMODE_FORCE_DWORD	= 0x7fffffff
    } 	WICBitmapInterpolationMode;

typedef /* [public] */ 
enum WICBitmapPaletteType
    {	WICBitmapPaletteTypeCustom	= 0,
	WICBitmapPaletteTypeMedianCut	= 0x1,
	WICBitmapPaletteTypeFixedBW	= 0x2,
	WICBitmapPaletteTypeFixedHalftone8	= 0x3,
	WICBitmapPaletteTypeFixedHalftone27	= 0x4,
	WICBitmapPaletteTypeFixedHalftone64	= 0x5,
	WICBitmapPaletteTypeFixedHalftone125	= 0x6,
	WICBitmapPaletteTypeFixedHalftone216	= 0x7,
	WICBitmapPaletteTypeFixedWebPalette	= WICBitmapPaletteTypeFixedHalftone216,
	WICBitmapPaletteTypeFixedHalftone252	= 0x8,
	WICBitmapPaletteTypeFixedHalftone256	= 0x9,
	WICBitmapPaletteTypeFixedGray4	= 0xa,
	WICBitmapPaletteTypeFixedGray16	= 0xb,
	WICBitmapPaletteTypeFixedGray256	= 0xc,
	WICBITMAPPALETTETYPE_FORCE_DWORD	= 0x7fffffff
    } 	WICBitmapPaletteType;

typedef /* [public] */ 
enum WICBitmapDitherType
    {	WICBitmapDitherTypeNone	= 0,
	WICBitmapDitherTypeSolid	= 0,
	WICBitmapDitherTypeOrdered4x4	= 0x1,
	WICBitmapDitherTypeOrdered8x8	= 0x2,
	WICBitmapDitherTypeOrdered16x16	= 0x3,
	WICBitmapDitherTypeSpiral4x4	= 0x4,
	WICBitmapDitherTypeSpiral8x8	= 0x5,
	WICBitmapDitherTypeDualSpiral4x4	= 0x6,
	WICBitmapDitherTypeDualSpiral8x8	= 0x7,
	WICBitmapDitherTypeErrorDiffusion	= 0x8,
	WICBITMAPDITHERTYPE_FORCE_DWORD	= 0x7fffffff
    } 	WICBitmapDitherType;

typedef /* [public] */ 
enum WICBitmapAlphaChannelOption
    {	WICBitmapUseAlpha	= 0,
	WICBitmapUsePremultipliedAlpha	= 0x1,
	WICBitmapIgnoreAlpha	= 0x2,
	WICBITMAPALPHACHANNELOPTIONS_FORCE_DWORD	= 0x7fffffff
    } 	WICBitmapAlphaChannelOption;

typedef /* [public] */ 
enum WICBitmapTransformOptions
    {	WICBitmapTransformRotate0	= 0,
	WICBitmapTransformRotate90	= 0x1,
	WICBitmapTransformRotate180	= 0x2,
	WICBitmapTransformRotate270	= 0x3,
	WICBitmapTransformFlipHorizontal	= 0x8,
	WICBitmapTransformFlipVertical	= 0x10,
	WICBITMAPTRANSFORMOPTIONS_FORCE_DWORD	= 0x7fffffff
    } 	WICBitmapTransformOptions;

typedef /* [public] */ 
enum WICBitmapLockFlags
    {	WICBitmapLockRead	= 0x1,
	WICBitmapLockWrite	= 0x2,
	WICBITMAPLOCKFLAGS_FORCE_DWORD	= 0x7fffffff
    } 	WICBitmapLockFlags;

typedef /* [public] */ 
enum WICBitmapDecoderCapabilities
    {	WICBitmapDecoderCapabilitySameEncoder	= 0x1,
	WICBitmapDecoderCapabilityCanDecodeAllImages	= 0x2,
	WICBitmapDecoderCapabilityCanDecodeSomeImages	= 0x4,
	WICBitmapDecoderCapabilityCanEnumerateMetadata	= 0x8,
	WICBitmapDecoderCapabilityCanDecodeThumbnail	= 0x10,
	WICBITMAPDECODERCAPABILITIES_FORCE_DWORD	= 0x7fffffff
    } 	WICBitmapDecoderCapabilities;

typedef /* [public] */ 
enum WICProgressOperation
    {	WICProgressOperationCopyPixels	= 0x1,
	WICProgressOperationWritePixels	= 0x2,
	WICProgressOperationAll	= 0xffff,
	WICPROGRESSOPERATION_FORCE_DWORD	= 0x7fffffff
    } 	WICProgressOperation;

typedef /* [public] */ 
enum WICProgressNotification
    {	WICProgressNotificationBegin	= 0x10000,
	WICProgressNotificationEnd	= 0x20000,
	WICProgressNotificationFrequent	= 0x40000,
	WICProgressNotificationAll	= 0xffff0000,
	WICPROGRESSNOTIFICATION_FORCE_DWORD	= 0x7fffffff
    } 	WICProgressNotification;

typedef /* [public] */ 
enum WICComponentSigning
    {	WICComponentSigned	= 0x1,
	WICComponentUnsigned	= 0x2,
	WICComponentSafe	= 0x4,
	WICComponentDisabled	= 0x80000000,
	WICCOMPONENTSIGNING_FORCE_DWORD	= 0x7fffffff
    } 	WICComponentSigning;

typedef /* [public] */ 
enum WICGifLogicalScreenDescriptorProperties
    {	WICGifLogicalScreenSignature	= 0x1,
	WICGifLogicalScreenDescriptorWidth	= 0x2,
	WICGifLogicalScreenDescriptorHeight	= 0x3,
	WICGifLogicalScreenDescriptorGlobalColorTableFlag	= 0x4,
	WICGifLogicalScreenDescriptorColorResolution	= 0x5,
	WICGifLogicalScreenDescriptorSortFlag	= 0x6,
	WICGifLogicalScreenDescriptorGlobalColorTableSize	= 0x7,
	WICGifLogicalScreenDescriptorBackgroundColorIndex	= 0x8,
	WICGifLogicalScreenDescriptorPixelAspectRatio	= 0x9,
	WICGifLogicalScreenDescriptorProperties_FORCE_DWORD	= 0x7fffffff
    } 	WICGifLogicalScreenDescriptorProperties;

typedef /* [public] */ 
enum WICGifImageDescriptorProperties
    {	WICGifImageDescriptorLeft	= 0x1,
	WICGifImageDescriptorTop	= 0x2,
	WICGifImageDescriptorWidth	= 0x3,
	WICGifImageDescriptorHeight	= 0x4,
	WICGifImageDescriptorLocalColorTableFlag	= 0x5,
	WICGifImageDescriptorInterlaceFlag	= 0x6,
	WICGifImageDescriptorSortFlag	= 0x7,
	WICGifImageDescriptorLocalColorTableSize	= 0x8,
	WICGifImageDescriptorProperties_FORCE_DWORD	= 0x7fffffff
    } 	WICGifImageDescriptorProperties;

typedef /* [public] */ 
enum WICGifGraphicControlExtensionProperties
    {	WICGifGraphicControlExtensionDisposal	= 0x1,
	WICGifGraphicControlExtensionUserInputFlag	= 0x2,
	WICGifGraphicControlExtensionTransparencyFlag	= 0x3,
	WICGifGraphicControlExtensionDelay	= 0x4,
	WICGifGraphicControlExtensionTransparentColorIndex	= 0x5,
	WICGifGraphicControlExtensionProperties_FORCE_DWORD	= 0x7fffffff
    } 	WICGifGraphicControlExtensionProperties;

typedef /* [public] */ 
enum WICGifApplicationExtensionProperties
    {	WICGifApplicationExtensionApplication	= 0x1,
	WICGifApplicationExtensionData	= 0x2,
	WICGifApplicationExtensionProperties_FORCE_DWORD	= 0x7fffffff
    } 	WICGifApplicationExtensionProperties;

typedef /* [public] */ 
enum WICGifCommentExtensionProperties
    {	WICGifCommentExtensionText	= 0x1,
	WICGifCommentExtensionProperties_FORCE_DWORD	= 0x7fffffff
    } 	WICGifCommentExtensionProperties;

typedef /* [public] */ 
enum WICJpegCommentProperties
    {	WICJpegCommentText	= 0x1,
	WICJpegCommentProperties_FORCE_DWORD	= 0x7fffffff
    } 	WICJpegCommentProperties;

typedef /* [public] */ 
enum WICJpegLuminanceProperties
    {	WICJpegLuminanceTable	= 0x1,
	WICJpegLuminanceProperties_FORCE_DWORD	= 0x7fffffff
    } 	WICJpegLuminanceProperties;

typedef /* [public] */ 
enum WICJpegChrominanceProperties
    {	WICJpegChrominanceTable	= 0x1,
	WICJpegChrominanceProperties_FORCE_DWORD	= 0x7fffffff
    } 	WICJpegChrominanceProperties;

typedef /* [public] */ 
enum WIC8BIMIptcProperties
    {	WIC8BIMIptcPString	= 0,
	WIC8BIMIptcEmbeddedIPTC	= 0x1,
	WIC8BIMIptcProperties_FORCE_DWORD	= 0x7fffffff
    } 	WIC8BIMIptcProperties;

typedef /* [public] */ 
enum WIC8BIMResolutionInfoProperties
    {	WIC8BIMResolutionInfoPString	= 0x1,
	WIC8BIMResolutionInfoHResolution	= 0x2,
	WIC8BIMResolutionInfoHResolutionUnit	= 0x3,
	WIC8BIMResolutionInfoWidthUnit	= 0x4,
	WIC8BIMResolutionInfoVResolution	= 0x5,
	WIC8BIMResolutionInfoVResolutionUnit	= 0x6,
	WIC8BIMResolutionInfoHeightUnit	= 0x7,
	WIC8BIMResolutionInfoProperties_FORCE_DWORD	= 0x7fffffff
    } 	WIC8BIMResolutionInfoProperties;

typedef /* [public] */ 
enum WIC8BIMIptcDigestProperties
    {	WIC8BIMIptcDigestPString	= 0x1,
	WIC8BIMIptcDigestIptcDigest	= 0x2,
	WIC8BIMIptcDigestProperties_FORCE_DWORD	= 0x7fffffff
    } 	WIC8BIMIptcDigestProperties;

typedef /* [public] */ 
enum WICPngGamaProperties
    {	WICPngGamaGamma	= 0x1,
	WICPngGamaProperties_FORCE_DWORD	= 0x7fffffff
    } 	WICPngGamaProperties;

typedef /* [public] */ 
enum WICPngBkgdProperties
    {	WICPngBkgdBackgroundColor	= 0x1,
	WICPngBkgdProperties_FORCE_DWORD	= 0x7fffffff
    } 	WICPngBkgdProperties;

typedef /* [public] */ 
enum WICPngItxtProperties
    {	WICPngItxtKeyword	= 0x1,
	WICPngItxtCompressionFlag	= 0x2,
	WICPngItxtLanguageTag	= 0x3,
	WICPngItxtTranslatedKeyword	= 0x4,
	WICPngItxtText	= 0x5,
	WICPngItxtProperties_FORCE_DWORD	= 0x7fffffff
    } 	WICPngItxtProperties;

typedef /* [public] */ 
enum WICPngChrmProperties
    {	WICPngChrmWhitePointX	= 0x1,
	WICPngChrmWhitePointY	= 0x2,
	WICPngChrmRedX	= 0x3,
	WICPngChrmRedY	= 0x4,
	WICPngChrmGreenX	= 0x5,
	WICPngChrmGreenY	= 0x6,
	WICPngChrmBlueX	= 0x7,
	WICPngChrmBlueY	= 0x8,
	WICPngChrmProperties_FORCE_DWORD	= 0x7fffffff
    } 	WICPngChrmProperties;

typedef /* [public] */ 
enum WICPngHistProperties
    {	WICPngHistFrequencies	= 0x1,
	WICPngHistProperties_FORCE_DWORD	= 0x7fffffff
    } 	WICPngHistProperties;

typedef /* [public] */ 
enum WICPngIccpProperties
    {	WICPngIccpProfileName	= 0x1,
	WICPngIccpProfileData	= 0x2,
	WICPngIccpProperties_FORCE_DWORD	= 0x7fffffff
    } 	WICPngIccpProperties;

typedef /* [public] */ 
enum WICPngSrgbProperties
    {	WICPngSrgbRenderingIntent	= 0x1,
	WICPngSrgbProperties_FORCE_DWORD	= 0x7fffffff
    } 	WICPngSrgbProperties;

typedef /* [public] */ 
enum WICPngTimeProperties
    {	WICPngTimeYear	= 0x1,
	WICPngTimeMonth	= 0x2,
	WICPngTimeDay	= 0x3,
	WICPngTimeHour	= 0x4,
	WICPngTimeMinute	= 0x5,
	WICPngTimeSecond	= 0x6,
	WICPngTimeProperties_FORCE_DWORD	= 0x7fffffff
    } 	WICPngTimeProperties;

typedef /* [public] */ 
enum WICSectionAccessLevel
    {	WICSectionAccessLevelRead	= 0x1,
	WICSectionAccessLevelReadWrite	= 0x3,
	WICSectionAccessLevel_FORCE_DWORD	= 0x7fffffff
    } 	WICSectionAccessLevel;

typedef /* [public] */ 
enum WICPixelFormatNumericRepresentation
    {	WICPixelFormatNumericRepresentationUnspecified	= 0,
	WICPixelFormatNumericRepresentationIndexed	= 0x1,
	WICPixelFormatNumericRepresentationUnsignedInteger	= 0x2,
	WICPixelFormatNumericRepresentationSignedInteger	= 0x3,
	WICPixelFormatNumericRepresentationFixed	= 0x4,
	WICPixelFormatNumericRepresentationFloat	= 0x5,
	WICPixelFormatNumericRepresentation_FORCE_DWORD	= 0x7fffffff
    } 	WICPixelFormatNumericRepresentation;



extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0000_v0_0_s_ifspec;

#ifndef __IWICPalette_INTERFACE_DEFINED__
#define __IWICPalette_INTERFACE_DEFINED__

/* interface IWICPalette */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICPalette;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000040-a8f2-4877-ba0a-fd2b6645fb94")
    IWICPalette : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InitializePredefined( 
            /* [in] */ WICBitmapPaletteType ePaletteType,
            /* [in] */ BOOL fAddTransparentColor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InitializeCustom( 
            /* [size_is][in] */ __RPC__in_ecount_full(cCount) WICColor *pColors,
            /* [in] */ UINT cCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InitializeFromBitmap( 
            /* [in] */ __RPC__in_opt IWICBitmapSource *pISurface,
            /* [in] */ UINT cCount,
            /* [in] */ BOOL fAddTransparentColor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InitializeFromPalette( 
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetType( 
            /* [out] */ __RPC__out WICBitmapPaletteType *pePaletteType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetColorCount( 
            /* [out] */ __RPC__out UINT *pcCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetColors( 
            /* [in] */ UINT cCount,
            /* [size_is][out] */ __RPC__out_ecount_full(cCount) WICColor *pColors,
            /* [out] */ __RPC__out UINT *pcActualColors) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsBlackWhite( 
            /* [out] */ __RPC__out BOOL *pfIsBlackWhite) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsGrayscale( 
            /* [out] */ __RPC__out BOOL *pfIsGrayscale) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HasAlpha( 
            /* [out] */ __RPC__out BOOL *pfHasAlpha) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICPaletteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICPalette * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICPalette * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICPalette * This);
        
        HRESULT ( STDMETHODCALLTYPE *InitializePredefined )( 
            __RPC__in IWICPalette * This,
            /* [in] */ WICBitmapPaletteType ePaletteType,
            /* [in] */ BOOL fAddTransparentColor);
        
        HRESULT ( STDMETHODCALLTYPE *InitializeCustom )( 
            __RPC__in IWICPalette * This,
            /* [size_is][in] */ __RPC__in_ecount_full(cCount) WICColor *pColors,
            /* [in] */ UINT cCount);
        
        HRESULT ( STDMETHODCALLTYPE *InitializeFromBitmap )( 
            __RPC__in IWICPalette * This,
            /* [in] */ __RPC__in_opt IWICBitmapSource *pISurface,
            /* [in] */ UINT cCount,
            /* [in] */ BOOL fAddTransparentColor);
        
        HRESULT ( STDMETHODCALLTYPE *InitializeFromPalette )( 
            __RPC__in IWICPalette * This,
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            __RPC__in IWICPalette * This,
            /* [out] */ __RPC__out WICBitmapPaletteType *pePaletteType);
        
        HRESULT ( STDMETHODCALLTYPE *GetColorCount )( 
            __RPC__in IWICPalette * This,
            /* [out] */ __RPC__out UINT *pcCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetColors )( 
            __RPC__in IWICPalette * This,
            /* [in] */ UINT cCount,
            /* [size_is][out] */ __RPC__out_ecount_full(cCount) WICColor *pColors,
            /* [out] */ __RPC__out UINT *pcActualColors);
        
        HRESULT ( STDMETHODCALLTYPE *IsBlackWhite )( 
            __RPC__in IWICPalette * This,
            /* [out] */ __RPC__out BOOL *pfIsBlackWhite);
        
        HRESULT ( STDMETHODCALLTYPE *IsGrayscale )( 
            __RPC__in IWICPalette * This,
            /* [out] */ __RPC__out BOOL *pfIsGrayscale);
        
        HRESULT ( STDMETHODCALLTYPE *HasAlpha )( 
            __RPC__in IWICPalette * This,
            /* [out] */ __RPC__out BOOL *pfHasAlpha);
        
        END_INTERFACE
    } IWICPaletteVtbl;

    interface IWICPalette
    {
        CONST_VTBL struct IWICPaletteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICPalette_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICPalette_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICPalette_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICPalette_InitializePredefined(This,ePaletteType,fAddTransparentColor)	\
    ( (This)->lpVtbl -> InitializePredefined(This,ePaletteType,fAddTransparentColor) ) 

#define IWICPalette_InitializeCustom(This,pColors,cCount)	\
    ( (This)->lpVtbl -> InitializeCustom(This,pColors,cCount) ) 

#define IWICPalette_InitializeFromBitmap(This,pISurface,cCount,fAddTransparentColor)	\
    ( (This)->lpVtbl -> InitializeFromBitmap(This,pISurface,cCount,fAddTransparentColor) ) 

#define IWICPalette_InitializeFromPalette(This,pIPalette)	\
    ( (This)->lpVtbl -> InitializeFromPalette(This,pIPalette) ) 

#define IWICPalette_GetType(This,pePaletteType)	\
    ( (This)->lpVtbl -> GetType(This,pePaletteType) ) 

#define IWICPalette_GetColorCount(This,pcCount)	\
    ( (This)->lpVtbl -> GetColorCount(This,pcCount) ) 

#define IWICPalette_GetColors(This,cCount,pColors,pcActualColors)	\
    ( (This)->lpVtbl -> GetColors(This,cCount,pColors,pcActualColors) ) 

#define IWICPalette_IsBlackWhite(This,pfIsBlackWhite)	\
    ( (This)->lpVtbl -> IsBlackWhite(This,pfIsBlackWhite) ) 

#define IWICPalette_IsGrayscale(This,pfIsGrayscale)	\
    ( (This)->lpVtbl -> IsGrayscale(This,pfIsGrayscale) ) 

#define IWICPalette_HasAlpha(This,pfHasAlpha)	\
    ( (This)->lpVtbl -> HasAlpha(This,pfHasAlpha) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICPalette_INTERFACE_DEFINED__ */


#ifndef __IWICBitmapSource_INTERFACE_DEFINED__
#define __IWICBitmapSource_INTERFACE_DEFINED__

/* interface IWICBitmapSource */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICBitmapSource;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000120-a8f2-4877-ba0a-fd2b6645fb94")
    IWICBitmapSource : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetSize( 
            /* [out] */ __RPC__out UINT *puiWidth,
            /* [out] */ __RPC__out UINT *puiHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPixelFormat( 
            /* [out] */ __RPC__out WICPixelFormatGUID *pPixelFormat) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetResolution( 
            /* [out] */ __RPC__out double *pDpiX,
            /* [out] */ __RPC__out double *pDpiY) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CopyPalette( 
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CopyPixels( 
            /* [unique][in] */ __RPC__in_opt const WICRect *prc,
            /* [in] */ UINT cbStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][out] */ __RPC__out_ecount_full(cbBufferSize) BYTE *pbBuffer) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICBitmapSourceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICBitmapSource * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICBitmapSource * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICBitmapSource * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            __RPC__in IWICBitmapSource * This,
            /* [out] */ __RPC__out UINT *puiWidth,
            /* [out] */ __RPC__out UINT *puiHeight);
        
        HRESULT ( STDMETHODCALLTYPE *GetPixelFormat )( 
            __RPC__in IWICBitmapSource * This,
            /* [out] */ __RPC__out WICPixelFormatGUID *pPixelFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetResolution )( 
            __RPC__in IWICBitmapSource * This,
            /* [out] */ __RPC__out double *pDpiX,
            /* [out] */ __RPC__out double *pDpiY);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPalette )( 
            __RPC__in IWICBitmapSource * This,
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPixels )( 
            __RPC__in IWICBitmapSource * This,
            /* [unique][in] */ __RPC__in_opt const WICRect *prc,
            /* [in] */ UINT cbStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][out] */ __RPC__out_ecount_full(cbBufferSize) BYTE *pbBuffer);
        
        END_INTERFACE
    } IWICBitmapSourceVtbl;

    interface IWICBitmapSource
    {
        CONST_VTBL struct IWICBitmapSourceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICBitmapSource_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICBitmapSource_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICBitmapSource_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICBitmapSource_GetSize(This,puiWidth,puiHeight)	\
    ( (This)->lpVtbl -> GetSize(This,puiWidth,puiHeight) ) 

#define IWICBitmapSource_GetPixelFormat(This,pPixelFormat)	\
    ( (This)->lpVtbl -> GetPixelFormat(This,pPixelFormat) ) 

#define IWICBitmapSource_GetResolution(This,pDpiX,pDpiY)	\
    ( (This)->lpVtbl -> GetResolution(This,pDpiX,pDpiY) ) 

#define IWICBitmapSource_CopyPalette(This,pIPalette)	\
    ( (This)->lpVtbl -> CopyPalette(This,pIPalette) ) 

#define IWICBitmapSource_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)	\
    ( (This)->lpVtbl -> CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICBitmapSource_INTERFACE_DEFINED__ */


#ifndef __IWICFormatConverter_INTERFACE_DEFINED__
#define __IWICFormatConverter_INTERFACE_DEFINED__

/* interface IWICFormatConverter */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICFormatConverter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000301-a8f2-4877-ba0a-fd2b6645fb94")
    IWICFormatConverter : public IWICBitmapSource
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ __RPC__in_opt IWICBitmapSource *pISource,
            /* [in] */ __RPC__in REFWICPixelFormatGUID dstFormat,
            /* [in] */ WICBitmapDitherType dither,
            /* [unique][in] */ __RPC__in_opt IWICPalette *pIPalette,
            /* [in] */ double alphaThresholdPercent,
            /* [in] */ WICBitmapPaletteType paletteTranslate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanConvert( 
            /* [in] */ __RPC__in REFWICPixelFormatGUID srcPixelFormat,
            /* [in] */ __RPC__in REFWICPixelFormatGUID dstPixelFormat,
            /* [out] */ __RPC__out BOOL *pfCanConvert) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICFormatConverterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICFormatConverter * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICFormatConverter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICFormatConverter * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            __RPC__in IWICFormatConverter * This,
            /* [out] */ __RPC__out UINT *puiWidth,
            /* [out] */ __RPC__out UINT *puiHeight);
        
        HRESULT ( STDMETHODCALLTYPE *GetPixelFormat )( 
            __RPC__in IWICFormatConverter * This,
            /* [out] */ __RPC__out WICPixelFormatGUID *pPixelFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetResolution )( 
            __RPC__in IWICFormatConverter * This,
            /* [out] */ __RPC__out double *pDpiX,
            /* [out] */ __RPC__out double *pDpiY);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPalette )( 
            __RPC__in IWICFormatConverter * This,
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPixels )( 
            __RPC__in IWICFormatConverter * This,
            /* [unique][in] */ __RPC__in_opt const WICRect *prc,
            /* [in] */ UINT cbStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][out] */ __RPC__out_ecount_full(cbBufferSize) BYTE *pbBuffer);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            __RPC__in IWICFormatConverter * This,
            /* [in] */ __RPC__in_opt IWICBitmapSource *pISource,
            /* [in] */ __RPC__in REFWICPixelFormatGUID dstFormat,
            /* [in] */ WICBitmapDitherType dither,
            /* [unique][in] */ __RPC__in_opt IWICPalette *pIPalette,
            /* [in] */ double alphaThresholdPercent,
            /* [in] */ WICBitmapPaletteType paletteTranslate);
        
        HRESULT ( STDMETHODCALLTYPE *CanConvert )( 
            __RPC__in IWICFormatConverter * This,
            /* [in] */ __RPC__in REFWICPixelFormatGUID srcPixelFormat,
            /* [in] */ __RPC__in REFWICPixelFormatGUID dstPixelFormat,
            /* [out] */ __RPC__out BOOL *pfCanConvert);
        
        END_INTERFACE
    } IWICFormatConverterVtbl;

    interface IWICFormatConverter
    {
        CONST_VTBL struct IWICFormatConverterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICFormatConverter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICFormatConverter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICFormatConverter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICFormatConverter_GetSize(This,puiWidth,puiHeight)	\
    ( (This)->lpVtbl -> GetSize(This,puiWidth,puiHeight) ) 

#define IWICFormatConverter_GetPixelFormat(This,pPixelFormat)	\
    ( (This)->lpVtbl -> GetPixelFormat(This,pPixelFormat) ) 

#define IWICFormatConverter_GetResolution(This,pDpiX,pDpiY)	\
    ( (This)->lpVtbl -> GetResolution(This,pDpiX,pDpiY) ) 

#define IWICFormatConverter_CopyPalette(This,pIPalette)	\
    ( (This)->lpVtbl -> CopyPalette(This,pIPalette) ) 

#define IWICFormatConverter_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)	\
    ( (This)->lpVtbl -> CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer) ) 


#define IWICFormatConverter_Initialize(This,pISource,dstFormat,dither,pIPalette,alphaThresholdPercent,paletteTranslate)	\
    ( (This)->lpVtbl -> Initialize(This,pISource,dstFormat,dither,pIPalette,alphaThresholdPercent,paletteTranslate) ) 

#define IWICFormatConverter_CanConvert(This,srcPixelFormat,dstPixelFormat,pfCanConvert)	\
    ( (This)->lpVtbl -> CanConvert(This,srcPixelFormat,dstPixelFormat,pfCanConvert) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICFormatConverter_INTERFACE_DEFINED__ */


#ifndef __IWICBitmapScaler_INTERFACE_DEFINED__
#define __IWICBitmapScaler_INTERFACE_DEFINED__

/* interface IWICBitmapScaler */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICBitmapScaler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000302-a8f2-4877-ba0a-fd2b6645fb94")
    IWICBitmapScaler : public IWICBitmapSource
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ __RPC__in_opt IWICBitmapSource *pISource,
            /* [in] */ UINT uiWidth,
            /* [in] */ UINT uiHeight,
            /* [in] */ WICBitmapInterpolationMode mode) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICBitmapScalerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICBitmapScaler * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICBitmapScaler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICBitmapScaler * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            __RPC__in IWICBitmapScaler * This,
            /* [out] */ __RPC__out UINT *puiWidth,
            /* [out] */ __RPC__out UINT *puiHeight);
        
        HRESULT ( STDMETHODCALLTYPE *GetPixelFormat )( 
            __RPC__in IWICBitmapScaler * This,
            /* [out] */ __RPC__out WICPixelFormatGUID *pPixelFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetResolution )( 
            __RPC__in IWICBitmapScaler * This,
            /* [out] */ __RPC__out double *pDpiX,
            /* [out] */ __RPC__out double *pDpiY);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPalette )( 
            __RPC__in IWICBitmapScaler * This,
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPixels )( 
            __RPC__in IWICBitmapScaler * This,
            /* [unique][in] */ __RPC__in_opt const WICRect *prc,
            /* [in] */ UINT cbStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][out] */ __RPC__out_ecount_full(cbBufferSize) BYTE *pbBuffer);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            __RPC__in IWICBitmapScaler * This,
            /* [in] */ __RPC__in_opt IWICBitmapSource *pISource,
            /* [in] */ UINT uiWidth,
            /* [in] */ UINT uiHeight,
            /* [in] */ WICBitmapInterpolationMode mode);
        
        END_INTERFACE
    } IWICBitmapScalerVtbl;

    interface IWICBitmapScaler
    {
        CONST_VTBL struct IWICBitmapScalerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICBitmapScaler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICBitmapScaler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICBitmapScaler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICBitmapScaler_GetSize(This,puiWidth,puiHeight)	\
    ( (This)->lpVtbl -> GetSize(This,puiWidth,puiHeight) ) 

#define IWICBitmapScaler_GetPixelFormat(This,pPixelFormat)	\
    ( (This)->lpVtbl -> GetPixelFormat(This,pPixelFormat) ) 

#define IWICBitmapScaler_GetResolution(This,pDpiX,pDpiY)	\
    ( (This)->lpVtbl -> GetResolution(This,pDpiX,pDpiY) ) 

#define IWICBitmapScaler_CopyPalette(This,pIPalette)	\
    ( (This)->lpVtbl -> CopyPalette(This,pIPalette) ) 

#define IWICBitmapScaler_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)	\
    ( (This)->lpVtbl -> CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer) ) 


#define IWICBitmapScaler_Initialize(This,pISource,uiWidth,uiHeight,mode)	\
    ( (This)->lpVtbl -> Initialize(This,pISource,uiWidth,uiHeight,mode) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICBitmapScaler_INTERFACE_DEFINED__ */


#ifndef __IWICBitmapClipper_INTERFACE_DEFINED__
#define __IWICBitmapClipper_INTERFACE_DEFINED__

/* interface IWICBitmapClipper */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICBitmapClipper;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E4FBCF03-223D-4e81-9333-D635556DD1B5")
    IWICBitmapClipper : public IWICBitmapSource
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ __RPC__in_opt IWICBitmapSource *pISource,
            /* [in] */ __RPC__in const WICRect *prc) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICBitmapClipperVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICBitmapClipper * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICBitmapClipper * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICBitmapClipper * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            __RPC__in IWICBitmapClipper * This,
            /* [out] */ __RPC__out UINT *puiWidth,
            /* [out] */ __RPC__out UINT *puiHeight);
        
        HRESULT ( STDMETHODCALLTYPE *GetPixelFormat )( 
            __RPC__in IWICBitmapClipper * This,
            /* [out] */ __RPC__out WICPixelFormatGUID *pPixelFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetResolution )( 
            __RPC__in IWICBitmapClipper * This,
            /* [out] */ __RPC__out double *pDpiX,
            /* [out] */ __RPC__out double *pDpiY);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPalette )( 
            __RPC__in IWICBitmapClipper * This,
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPixels )( 
            __RPC__in IWICBitmapClipper * This,
            /* [unique][in] */ __RPC__in_opt const WICRect *prc,
            /* [in] */ UINT cbStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][out] */ __RPC__out_ecount_full(cbBufferSize) BYTE *pbBuffer);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            __RPC__in IWICBitmapClipper * This,
            /* [in] */ __RPC__in_opt IWICBitmapSource *pISource,
            /* [in] */ __RPC__in const WICRect *prc);
        
        END_INTERFACE
    } IWICBitmapClipperVtbl;

    interface IWICBitmapClipper
    {
        CONST_VTBL struct IWICBitmapClipperVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICBitmapClipper_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICBitmapClipper_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICBitmapClipper_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICBitmapClipper_GetSize(This,puiWidth,puiHeight)	\
    ( (This)->lpVtbl -> GetSize(This,puiWidth,puiHeight) ) 

#define IWICBitmapClipper_GetPixelFormat(This,pPixelFormat)	\
    ( (This)->lpVtbl -> GetPixelFormat(This,pPixelFormat) ) 

#define IWICBitmapClipper_GetResolution(This,pDpiX,pDpiY)	\
    ( (This)->lpVtbl -> GetResolution(This,pDpiX,pDpiY) ) 

#define IWICBitmapClipper_CopyPalette(This,pIPalette)	\
    ( (This)->lpVtbl -> CopyPalette(This,pIPalette) ) 

#define IWICBitmapClipper_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)	\
    ( (This)->lpVtbl -> CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer) ) 


#define IWICBitmapClipper_Initialize(This,pISource,prc)	\
    ( (This)->lpVtbl -> Initialize(This,pISource,prc) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICBitmapClipper_INTERFACE_DEFINED__ */


#ifndef __IWICBitmapFlipRotator_INTERFACE_DEFINED__
#define __IWICBitmapFlipRotator_INTERFACE_DEFINED__

/* interface IWICBitmapFlipRotator */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICBitmapFlipRotator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5009834F-2D6A-41ce-9E1B-17C5AFF7A782")
    IWICBitmapFlipRotator : public IWICBitmapSource
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ __RPC__in_opt IWICBitmapSource *pISource,
            /* [in] */ WICBitmapTransformOptions options) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICBitmapFlipRotatorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICBitmapFlipRotator * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICBitmapFlipRotator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICBitmapFlipRotator * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            __RPC__in IWICBitmapFlipRotator * This,
            /* [out] */ __RPC__out UINT *puiWidth,
            /* [out] */ __RPC__out UINT *puiHeight);
        
        HRESULT ( STDMETHODCALLTYPE *GetPixelFormat )( 
            __RPC__in IWICBitmapFlipRotator * This,
            /* [out] */ __RPC__out WICPixelFormatGUID *pPixelFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetResolution )( 
            __RPC__in IWICBitmapFlipRotator * This,
            /* [out] */ __RPC__out double *pDpiX,
            /* [out] */ __RPC__out double *pDpiY);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPalette )( 
            __RPC__in IWICBitmapFlipRotator * This,
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPixels )( 
            __RPC__in IWICBitmapFlipRotator * This,
            /* [unique][in] */ __RPC__in_opt const WICRect *prc,
            /* [in] */ UINT cbStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][out] */ __RPC__out_ecount_full(cbBufferSize) BYTE *pbBuffer);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            __RPC__in IWICBitmapFlipRotator * This,
            /* [in] */ __RPC__in_opt IWICBitmapSource *pISource,
            /* [in] */ WICBitmapTransformOptions options);
        
        END_INTERFACE
    } IWICBitmapFlipRotatorVtbl;

    interface IWICBitmapFlipRotator
    {
        CONST_VTBL struct IWICBitmapFlipRotatorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICBitmapFlipRotator_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICBitmapFlipRotator_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICBitmapFlipRotator_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICBitmapFlipRotator_GetSize(This,puiWidth,puiHeight)	\
    ( (This)->lpVtbl -> GetSize(This,puiWidth,puiHeight) ) 

#define IWICBitmapFlipRotator_GetPixelFormat(This,pPixelFormat)	\
    ( (This)->lpVtbl -> GetPixelFormat(This,pPixelFormat) ) 

#define IWICBitmapFlipRotator_GetResolution(This,pDpiX,pDpiY)	\
    ( (This)->lpVtbl -> GetResolution(This,pDpiX,pDpiY) ) 

#define IWICBitmapFlipRotator_CopyPalette(This,pIPalette)	\
    ( (This)->lpVtbl -> CopyPalette(This,pIPalette) ) 

#define IWICBitmapFlipRotator_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)	\
    ( (This)->lpVtbl -> CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer) ) 


#define IWICBitmapFlipRotator_Initialize(This,pISource,options)	\
    ( (This)->lpVtbl -> Initialize(This,pISource,options) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICBitmapFlipRotator_INTERFACE_DEFINED__ */


#ifndef __IWICBitmapLock_INTERFACE_DEFINED__
#define __IWICBitmapLock_INTERFACE_DEFINED__

/* interface IWICBitmapLock */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICBitmapLock;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000123-a8f2-4877-ba0a-fd2b6645fb94")
    IWICBitmapLock : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetSize( 
            /* [out] */ __RPC__out UINT *puiWidth,
            /* [out] */ __RPC__out UINT *puiHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStride( 
            /* [out] */ __RPC__out UINT *pcbStride) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDataPointer( 
            /* [out] */ __RPC__out UINT *pcbBufferSize,
            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*pcbBufferSize) WICInProcPointer *ppbData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPixelFormat( 
            /* [out] */ __RPC__out WICPixelFormatGUID *pPixelFormat) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICBitmapLockVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICBitmapLock * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICBitmapLock * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICBitmapLock * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            __RPC__in IWICBitmapLock * This,
            /* [out] */ __RPC__out UINT *puiWidth,
            /* [out] */ __RPC__out UINT *puiHeight);
        
        HRESULT ( STDMETHODCALLTYPE *GetStride )( 
            __RPC__in IWICBitmapLock * This,
            /* [out] */ __RPC__out UINT *pcbStride);
        
        HRESULT ( STDMETHODCALLTYPE *GetDataPointer )( 
            __RPC__in IWICBitmapLock * This,
            /* [out] */ __RPC__out UINT *pcbBufferSize,
            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*pcbBufferSize) WICInProcPointer *ppbData);
        
        HRESULT ( STDMETHODCALLTYPE *GetPixelFormat )( 
            __RPC__in IWICBitmapLock * This,
            /* [out] */ __RPC__out WICPixelFormatGUID *pPixelFormat);
        
        END_INTERFACE
    } IWICBitmapLockVtbl;

    interface IWICBitmapLock
    {
        CONST_VTBL struct IWICBitmapLockVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICBitmapLock_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICBitmapLock_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICBitmapLock_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICBitmapLock_GetSize(This,puiWidth,puiHeight)	\
    ( (This)->lpVtbl -> GetSize(This,puiWidth,puiHeight) ) 

#define IWICBitmapLock_GetStride(This,pcbStride)	\
    ( (This)->lpVtbl -> GetStride(This,pcbStride) ) 

#define IWICBitmapLock_GetDataPointer(This,pcbBufferSize,ppbData)	\
    ( (This)->lpVtbl -> GetDataPointer(This,pcbBufferSize,ppbData) ) 

#define IWICBitmapLock_GetPixelFormat(This,pPixelFormat)	\
    ( (This)->lpVtbl -> GetPixelFormat(This,pPixelFormat) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICBitmapLock_INTERFACE_DEFINED__ */


#ifndef __IWICBitmap_INTERFACE_DEFINED__
#define __IWICBitmap_INTERFACE_DEFINED__

/* interface IWICBitmap */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICBitmap;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000121-a8f2-4877-ba0a-fd2b6645fb94")
    IWICBitmap : public IWICBitmapSource
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Lock( 
            /* [unique][in] */ __RPC__in_opt const WICRect *prcLock,
            /* [in] */ DWORD flags,
            /* [out] */ __RPC__deref_out_opt IWICBitmapLock **ppILock) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPalette( 
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetResolution( 
            /* [in] */ double dpiX,
            /* [in] */ double dpiY) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICBitmapVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICBitmap * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICBitmap * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICBitmap * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            __RPC__in IWICBitmap * This,
            /* [out] */ __RPC__out UINT *puiWidth,
            /* [out] */ __RPC__out UINT *puiHeight);
        
        HRESULT ( STDMETHODCALLTYPE *GetPixelFormat )( 
            __RPC__in IWICBitmap * This,
            /* [out] */ __RPC__out WICPixelFormatGUID *pPixelFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetResolution )( 
            __RPC__in IWICBitmap * This,
            /* [out] */ __RPC__out double *pDpiX,
            /* [out] */ __RPC__out double *pDpiY);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPalette )( 
            __RPC__in IWICBitmap * This,
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPixels )( 
            __RPC__in IWICBitmap * This,
            /* [unique][in] */ __RPC__in_opt const WICRect *prc,
            /* [in] */ UINT cbStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][out] */ __RPC__out_ecount_full(cbBufferSize) BYTE *pbBuffer);
        
        HRESULT ( STDMETHODCALLTYPE *Lock )( 
            __RPC__in IWICBitmap * This,
            /* [unique][in] */ __RPC__in_opt const WICRect *prcLock,
            /* [in] */ DWORD flags,
            /* [out] */ __RPC__deref_out_opt IWICBitmapLock **ppILock);
        
        HRESULT ( STDMETHODCALLTYPE *SetPalette )( 
            __RPC__in IWICBitmap * This,
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette);
        
        HRESULT ( STDMETHODCALLTYPE *SetResolution )( 
            __RPC__in IWICBitmap * This,
            /* [in] */ double dpiX,
            /* [in] */ double dpiY);
        
        END_INTERFACE
    } IWICBitmapVtbl;

    interface IWICBitmap
    {
        CONST_VTBL struct IWICBitmapVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICBitmap_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICBitmap_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICBitmap_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICBitmap_GetSize(This,puiWidth,puiHeight)	\
    ( (This)->lpVtbl -> GetSize(This,puiWidth,puiHeight) ) 

#define IWICBitmap_GetPixelFormat(This,pPixelFormat)	\
    ( (This)->lpVtbl -> GetPixelFormat(This,pPixelFormat) ) 

#define IWICBitmap_GetResolution(This,pDpiX,pDpiY)	\
    ( (This)->lpVtbl -> GetResolution(This,pDpiX,pDpiY) ) 

#define IWICBitmap_CopyPalette(This,pIPalette)	\
    ( (This)->lpVtbl -> CopyPalette(This,pIPalette) ) 

#define IWICBitmap_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)	\
    ( (This)->lpVtbl -> CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer) ) 


#define IWICBitmap_Lock(This,prcLock,flags,ppILock)	\
    ( (This)->lpVtbl -> Lock(This,prcLock,flags,ppILock) ) 

#define IWICBitmap_SetPalette(This,pIPalette)	\
    ( (This)->lpVtbl -> SetPalette(This,pIPalette) ) 

#define IWICBitmap_SetResolution(This,dpiX,dpiY)	\
    ( (This)->lpVtbl -> SetResolution(This,dpiX,dpiY) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICBitmap_INTERFACE_DEFINED__ */


#ifndef __IWICColorContext_INTERFACE_DEFINED__
#define __IWICColorContext_INTERFACE_DEFINED__

/* interface IWICColorContext */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICColorContext;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3C613A02-34B2-44ea-9A7C-45AEA9C6FD6D")
    IWICColorContext : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InitializeFromFilename( 
            /* [in] */ __RPC__in LPCWSTR wzFilename) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InitializeFromMemory( 
            /* [size_is][in] */ __RPC__in_ecount_full(cbBufferSize) const BYTE *pbBuffer,
            /* [in] */ UINT cbBufferSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InitializeFromExifColorSpace( 
            /* [in] */ UINT value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetType( 
            /* [out] */ __RPC__out WICColorContextType *pType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetProfileBytes( 
            /* [in] */ UINT cbBuffer,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cbBuffer) BYTE *pbBuffer,
            /* [out] */ __RPC__out UINT *pcbActual) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetExifColorSpace( 
            /* [out] */ __RPC__out UINT *pValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICColorContextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICColorContext * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICColorContext * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICColorContext * This);
        
        HRESULT ( STDMETHODCALLTYPE *InitializeFromFilename )( 
            __RPC__in IWICColorContext * This,
            /* [in] */ __RPC__in LPCWSTR wzFilename);
        
        HRESULT ( STDMETHODCALLTYPE *InitializeFromMemory )( 
            __RPC__in IWICColorContext * This,
            /* [size_is][in] */ __RPC__in_ecount_full(cbBufferSize) const BYTE *pbBuffer,
            /* [in] */ UINT cbBufferSize);
        
        HRESULT ( STDMETHODCALLTYPE *InitializeFromExifColorSpace )( 
            __RPC__in IWICColorContext * This,
            /* [in] */ UINT value);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            __RPC__in IWICColorContext * This,
            /* [out] */ __RPC__out WICColorContextType *pType);
        
        HRESULT ( STDMETHODCALLTYPE *GetProfileBytes )( 
            __RPC__in IWICColorContext * This,
            /* [in] */ UINT cbBuffer,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cbBuffer) BYTE *pbBuffer,
            /* [out] */ __RPC__out UINT *pcbActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetExifColorSpace )( 
            __RPC__in IWICColorContext * This,
            /* [out] */ __RPC__out UINT *pValue);
        
        END_INTERFACE
    } IWICColorContextVtbl;

    interface IWICColorContext
    {
        CONST_VTBL struct IWICColorContextVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICColorContext_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICColorContext_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICColorContext_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICColorContext_InitializeFromFilename(This,wzFilename)	\
    ( (This)->lpVtbl -> InitializeFromFilename(This,wzFilename) ) 

#define IWICColorContext_InitializeFromMemory(This,pbBuffer,cbBufferSize)	\
    ( (This)->lpVtbl -> InitializeFromMemory(This,pbBuffer,cbBufferSize) ) 

#define IWICColorContext_InitializeFromExifColorSpace(This,value)	\
    ( (This)->lpVtbl -> InitializeFromExifColorSpace(This,value) ) 

#define IWICColorContext_GetType(This,pType)	\
    ( (This)->lpVtbl -> GetType(This,pType) ) 

#define IWICColorContext_GetProfileBytes(This,cbBuffer,pbBuffer,pcbActual)	\
    ( (This)->lpVtbl -> GetProfileBytes(This,cbBuffer,pbBuffer,pcbActual) ) 

#define IWICColorContext_GetExifColorSpace(This,pValue)	\
    ( (This)->lpVtbl -> GetExifColorSpace(This,pValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICColorContext_INTERFACE_DEFINED__ */


#ifndef __IWICColorTransform_INTERFACE_DEFINED__
#define __IWICColorTransform_INTERFACE_DEFINED__

/* interface IWICColorTransform */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICColorTransform;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B66F034F-D0E2-40ab-B436-6DE39E321A94")
    IWICColorTransform : public IWICBitmapSource
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ __RPC__in_opt IWICBitmapSource *pIBitmapSource,
            /* [in] */ __RPC__in_opt IWICColorContext *pIContextSource,
            /* [in] */ __RPC__in_opt IWICColorContext *pIContextDest,
            /* [in] */ __RPC__in REFWICPixelFormatGUID pixelFmtDest) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICColorTransformVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICColorTransform * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICColorTransform * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICColorTransform * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            __RPC__in IWICColorTransform * This,
            /* [out] */ __RPC__out UINT *puiWidth,
            /* [out] */ __RPC__out UINT *puiHeight);
        
        HRESULT ( STDMETHODCALLTYPE *GetPixelFormat )( 
            __RPC__in IWICColorTransform * This,
            /* [out] */ __RPC__out WICPixelFormatGUID *pPixelFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetResolution )( 
            __RPC__in IWICColorTransform * This,
            /* [out] */ __RPC__out double *pDpiX,
            /* [out] */ __RPC__out double *pDpiY);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPalette )( 
            __RPC__in IWICColorTransform * This,
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPixels )( 
            __RPC__in IWICColorTransform * This,
            /* [unique][in] */ __RPC__in_opt const WICRect *prc,
            /* [in] */ UINT cbStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][out] */ __RPC__out_ecount_full(cbBufferSize) BYTE *pbBuffer);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            __RPC__in IWICColorTransform * This,
            /* [in] */ __RPC__in_opt IWICBitmapSource *pIBitmapSource,
            /* [in] */ __RPC__in_opt IWICColorContext *pIContextSource,
            /* [in] */ __RPC__in_opt IWICColorContext *pIContextDest,
            /* [in] */ __RPC__in REFWICPixelFormatGUID pixelFmtDest);
        
        END_INTERFACE
    } IWICColorTransformVtbl;

    interface IWICColorTransform
    {
        CONST_VTBL struct IWICColorTransformVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICColorTransform_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICColorTransform_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICColorTransform_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICColorTransform_GetSize(This,puiWidth,puiHeight)	\
    ( (This)->lpVtbl -> GetSize(This,puiWidth,puiHeight) ) 

#define IWICColorTransform_GetPixelFormat(This,pPixelFormat)	\
    ( (This)->lpVtbl -> GetPixelFormat(This,pPixelFormat) ) 

#define IWICColorTransform_GetResolution(This,pDpiX,pDpiY)	\
    ( (This)->lpVtbl -> GetResolution(This,pDpiX,pDpiY) ) 

#define IWICColorTransform_CopyPalette(This,pIPalette)	\
    ( (This)->lpVtbl -> CopyPalette(This,pIPalette) ) 

#define IWICColorTransform_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)	\
    ( (This)->lpVtbl -> CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer) ) 


#define IWICColorTransform_Initialize(This,pIBitmapSource,pIContextSource,pIContextDest,pixelFmtDest)	\
    ( (This)->lpVtbl -> Initialize(This,pIBitmapSource,pIContextSource,pIContextDest,pixelFmtDest) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICColorTransform_INTERFACE_DEFINED__ */


#ifndef __IWICFastMetadataEncoder_INTERFACE_DEFINED__
#define __IWICFastMetadataEncoder_INTERFACE_DEFINED__

/* interface IWICFastMetadataEncoder */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICFastMetadataEncoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B84E2C09-78C9-4AC4-8BD3-524AE1663A2F")
    IWICFastMetadataEncoder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Commit( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMetadataQueryWriter( 
            /* [out] */ __RPC__deref_out_opt IWICMetadataQueryWriter **ppIMetadataQueryWriter) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICFastMetadataEncoderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICFastMetadataEncoder * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICFastMetadataEncoder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICFastMetadataEncoder * This);
        
        HRESULT ( STDMETHODCALLTYPE *Commit )( 
            __RPC__in IWICFastMetadataEncoder * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetMetadataQueryWriter )( 
            __RPC__in IWICFastMetadataEncoder * This,
            /* [out] */ __RPC__deref_out_opt IWICMetadataQueryWriter **ppIMetadataQueryWriter);
        
        END_INTERFACE
    } IWICFastMetadataEncoderVtbl;

    interface IWICFastMetadataEncoder
    {
        CONST_VTBL struct IWICFastMetadataEncoderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICFastMetadataEncoder_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICFastMetadataEncoder_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICFastMetadataEncoder_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICFastMetadataEncoder_Commit(This)	\
    ( (This)->lpVtbl -> Commit(This) ) 

#define IWICFastMetadataEncoder_GetMetadataQueryWriter(This,ppIMetadataQueryWriter)	\
    ( (This)->lpVtbl -> GetMetadataQueryWriter(This,ppIMetadataQueryWriter) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICFastMetadataEncoder_INTERFACE_DEFINED__ */


#ifndef __IWICStream_INTERFACE_DEFINED__
#define __IWICStream_INTERFACE_DEFINED__

/* interface IWICStream */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICStream;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("135FF860-22B7-4ddf-B0F6-218F4F299A43")
    IWICStream : public IStream
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InitializeFromIStream( 
            /* [in] */ __RPC__in_opt IStream *pIStream) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InitializeFromFilename( 
            /* [in] */ __RPC__in LPCWSTR wzFileName,
            /* [in] */ DWORD dwDesiredAccess) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InitializeFromMemory( 
            /* [size_is][in] */ __RPC__in_ecount_full(cbBufferSize) WICInProcPointer pbBuffer,
            /* [in] */ DWORD cbBufferSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InitializeFromIStreamRegion( 
            /* [in] */ __RPC__in_opt IStream *pIStream,
            /* [in] */ ULARGE_INTEGER ulOffset,
            /* [in] */ ULARGE_INTEGER ulMaxSize) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICStreamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICStream * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICStream * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICStream * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Read )( 
            IWICStream * This,
            /* [annotation] */ 
            __out_bcount_part(cb, *pcbRead)  void *pv,
            /* [in] */ ULONG cb,
            /* [annotation] */ 
            __out_opt  ULONG *pcbRead);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Write )( 
            IWICStream * This,
            /* [annotation] */ 
            __in_bcount(cb)  const void *pv,
            /* [in] */ ULONG cb,
            /* [annotation] */ 
            __out_opt  ULONG *pcbWritten);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Seek )( 
            IWICStream * This,
            /* [in] */ LARGE_INTEGER dlibMove,
            /* [in] */ DWORD dwOrigin,
            /* [annotation] */ 
            __out_opt  ULARGE_INTEGER *plibNewPosition);
        
        HRESULT ( STDMETHODCALLTYPE *SetSize )( 
            __RPC__in IWICStream * This,
            /* [in] */ ULARGE_INTEGER libNewSize);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *CopyTo )( 
            IWICStream * This,
            /* [unique][in] */ IStream *pstm,
            /* [in] */ ULARGE_INTEGER cb,
            /* [annotation] */ 
            __out_opt  ULARGE_INTEGER *pcbRead,
            /* [annotation] */ 
            __out_opt  ULARGE_INTEGER *pcbWritten);
        
        HRESULT ( STDMETHODCALLTYPE *Commit )( 
            __RPC__in IWICStream * This,
            /* [in] */ DWORD grfCommitFlags);
        
        HRESULT ( STDMETHODCALLTYPE *Revert )( 
            __RPC__in IWICStream * This);
        
        HRESULT ( STDMETHODCALLTYPE *LockRegion )( 
            __RPC__in IWICStream * This,
            /* [in] */ ULARGE_INTEGER libOffset,
            /* [in] */ ULARGE_INTEGER cb,
            /* [in] */ DWORD dwLockType);
        
        HRESULT ( STDMETHODCALLTYPE *UnlockRegion )( 
            __RPC__in IWICStream * This,
            /* [in] */ ULARGE_INTEGER libOffset,
            /* [in] */ ULARGE_INTEGER cb,
            /* [in] */ DWORD dwLockType);
        
        HRESULT ( STDMETHODCALLTYPE *Stat )( 
            __RPC__in IWICStream * This,
            /* [out] */ __RPC__out STATSTG *pstatstg,
            /* [in] */ DWORD grfStatFlag);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            __RPC__in IWICStream * This,
            /* [out] */ __RPC__deref_out_opt IStream **ppstm);
        
        HRESULT ( STDMETHODCALLTYPE *InitializeFromIStream )( 
            __RPC__in IWICStream * This,
            /* [in] */ __RPC__in_opt IStream *pIStream);
        
        HRESULT ( STDMETHODCALLTYPE *InitializeFromFilename )( 
            __RPC__in IWICStream * This,
            /* [in] */ __RPC__in LPCWSTR wzFileName,
            /* [in] */ DWORD dwDesiredAccess);
        
        HRESULT ( STDMETHODCALLTYPE *InitializeFromMemory )( 
            __RPC__in IWICStream * This,
            /* [size_is][in] */ __RPC__in_ecount_full(cbBufferSize) WICInProcPointer pbBuffer,
            /* [in] */ DWORD cbBufferSize);
        
        HRESULT ( STDMETHODCALLTYPE *InitializeFromIStreamRegion )( 
            __RPC__in IWICStream * This,
            /* [in] */ __RPC__in_opt IStream *pIStream,
            /* [in] */ ULARGE_INTEGER ulOffset,
            /* [in] */ ULARGE_INTEGER ulMaxSize);
        
        END_INTERFACE
    } IWICStreamVtbl;

    interface IWICStream
    {
        CONST_VTBL struct IWICStreamVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICStream_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICStream_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICStream_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICStream_Read(This,pv,cb,pcbRead)	\
    ( (This)->lpVtbl -> Read(This,pv,cb,pcbRead) ) 

#define IWICStream_Write(This,pv,cb,pcbWritten)	\
    ( (This)->lpVtbl -> Write(This,pv,cb,pcbWritten) ) 


#define IWICStream_Seek(This,dlibMove,dwOrigin,plibNewPosition)	\
    ( (This)->lpVtbl -> Seek(This,dlibMove,dwOrigin,plibNewPosition) ) 

#define IWICStream_SetSize(This,libNewSize)	\
    ( (This)->lpVtbl -> SetSize(This,libNewSize) ) 

#define IWICStream_CopyTo(This,pstm,cb,pcbRead,pcbWritten)	\
    ( (This)->lpVtbl -> CopyTo(This,pstm,cb,pcbRead,pcbWritten) ) 

#define IWICStream_Commit(This,grfCommitFlags)	\
    ( (This)->lpVtbl -> Commit(This,grfCommitFlags) ) 

#define IWICStream_Revert(This)	\
    ( (This)->lpVtbl -> Revert(This) ) 

#define IWICStream_LockRegion(This,libOffset,cb,dwLockType)	\
    ( (This)->lpVtbl -> LockRegion(This,libOffset,cb,dwLockType) ) 

#define IWICStream_UnlockRegion(This,libOffset,cb,dwLockType)	\
    ( (This)->lpVtbl -> UnlockRegion(This,libOffset,cb,dwLockType) ) 

#define IWICStream_Stat(This,pstatstg,grfStatFlag)	\
    ( (This)->lpVtbl -> Stat(This,pstatstg,grfStatFlag) ) 

#define IWICStream_Clone(This,ppstm)	\
    ( (This)->lpVtbl -> Clone(This,ppstm) ) 


#define IWICStream_InitializeFromIStream(This,pIStream)	\
    ( (This)->lpVtbl -> InitializeFromIStream(This,pIStream) ) 

#define IWICStream_InitializeFromFilename(This,wzFileName,dwDesiredAccess)	\
    ( (This)->lpVtbl -> InitializeFromFilename(This,wzFileName,dwDesiredAccess) ) 

#define IWICStream_InitializeFromMemory(This,pbBuffer,cbBufferSize)	\
    ( (This)->lpVtbl -> InitializeFromMemory(This,pbBuffer,cbBufferSize) ) 

#define IWICStream_InitializeFromIStreamRegion(This,pIStream,ulOffset,ulMaxSize)	\
    ( (This)->lpVtbl -> InitializeFromIStreamRegion(This,pIStream,ulOffset,ulMaxSize) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICStream_INTERFACE_DEFINED__ */


#ifndef __IWICEnumMetadataItem_INTERFACE_DEFINED__
#define __IWICEnumMetadataItem_INTERFACE_DEFINED__

/* interface IWICEnumMetadataItem */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICEnumMetadataItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DC2BB46D-3F07-481E-8625-220C4AEDBB33")
    IWICEnumMetadataItem : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(celt) PROPVARIANT *rgeltSchema,
            /* [size_is][out][in] */ __RPC__inout_ecount_full(celt) PROPVARIANT *rgeltId,
            /* [size_is][optional][out][in] */ __RPC__inout_ecount_full(celt) PROPVARIANT *rgeltValue,
            /* [optional][out] */ __RPC__out ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ __RPC__deref_out_opt IWICEnumMetadataItem **ppIEnumMetadataItem) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICEnumMetadataItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICEnumMetadataItem * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICEnumMetadataItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICEnumMetadataItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            __RPC__in IWICEnumMetadataItem * This,
            /* [in] */ ULONG celt,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(celt) PROPVARIANT *rgeltSchema,
            /* [size_is][out][in] */ __RPC__inout_ecount_full(celt) PROPVARIANT *rgeltId,
            /* [size_is][optional][out][in] */ __RPC__inout_ecount_full(celt) PROPVARIANT *rgeltValue,
            /* [optional][out] */ __RPC__out ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            __RPC__in IWICEnumMetadataItem * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            __RPC__in IWICEnumMetadataItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            __RPC__in IWICEnumMetadataItem * This,
            /* [out] */ __RPC__deref_out_opt IWICEnumMetadataItem **ppIEnumMetadataItem);
        
        END_INTERFACE
    } IWICEnumMetadataItemVtbl;

    interface IWICEnumMetadataItem
    {
        CONST_VTBL struct IWICEnumMetadataItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICEnumMetadataItem_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICEnumMetadataItem_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICEnumMetadataItem_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICEnumMetadataItem_Next(This,celt,rgeltSchema,rgeltId,rgeltValue,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgeltSchema,rgeltId,rgeltValue,pceltFetched) ) 

#define IWICEnumMetadataItem_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IWICEnumMetadataItem_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IWICEnumMetadataItem_Clone(This,ppIEnumMetadataItem)	\
    ( (This)->lpVtbl -> Clone(This,ppIEnumMetadataItem) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICEnumMetadataItem_INTERFACE_DEFINED__ */


#ifndef __IWICMetadataQueryReader_INTERFACE_DEFINED__
#define __IWICMetadataQueryReader_INTERFACE_DEFINED__

/* interface IWICMetadataQueryReader */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICMetadataQueryReader;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("30989668-E1C9-4597-B395-458EEDB808DF")
    IWICMetadataQueryReader : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetContainerFormat( 
            /* [out] */ __RPC__out GUID *pguidContainerFormat) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLocation( 
            /* [in] */ UINT cchMaxLength,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchMaxLength) WCHAR *wzNamespace,
            /* [out] */ __RPC__out UINT *pcchActualLength) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMetadataByName( 
            /* [in] */ __RPC__in LPCWSTR wzName,
            /* [unique][out][in] */ __RPC__inout_opt PROPVARIANT *pvarValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEnumerator( 
            /* [out] */ __RPC__deref_out_opt IEnumString **ppIEnumString) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICMetadataQueryReaderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICMetadataQueryReader * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICMetadataQueryReader * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICMetadataQueryReader * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainerFormat )( 
            __RPC__in IWICMetadataQueryReader * This,
            /* [out] */ __RPC__out GUID *pguidContainerFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetLocation )( 
            __RPC__in IWICMetadataQueryReader * This,
            /* [in] */ UINT cchMaxLength,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchMaxLength) WCHAR *wzNamespace,
            /* [out] */ __RPC__out UINT *pcchActualLength);
        
        HRESULT ( STDMETHODCALLTYPE *GetMetadataByName )( 
            __RPC__in IWICMetadataQueryReader * This,
            /* [in] */ __RPC__in LPCWSTR wzName,
            /* [unique][out][in] */ __RPC__inout_opt PROPVARIANT *pvarValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetEnumerator )( 
            __RPC__in IWICMetadataQueryReader * This,
            /* [out] */ __RPC__deref_out_opt IEnumString **ppIEnumString);
        
        END_INTERFACE
    } IWICMetadataQueryReaderVtbl;

    interface IWICMetadataQueryReader
    {
        CONST_VTBL struct IWICMetadataQueryReaderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICMetadataQueryReader_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICMetadataQueryReader_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICMetadataQueryReader_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICMetadataQueryReader_GetContainerFormat(This,pguidContainerFormat)	\
    ( (This)->lpVtbl -> GetContainerFormat(This,pguidContainerFormat) ) 

#define IWICMetadataQueryReader_GetLocation(This,cchMaxLength,wzNamespace,pcchActualLength)	\
    ( (This)->lpVtbl -> GetLocation(This,cchMaxLength,wzNamespace,pcchActualLength) ) 

#define IWICMetadataQueryReader_GetMetadataByName(This,wzName,pvarValue)	\
    ( (This)->lpVtbl -> GetMetadataByName(This,wzName,pvarValue) ) 

#define IWICMetadataQueryReader_GetEnumerator(This,ppIEnumString)	\
    ( (This)->lpVtbl -> GetEnumerator(This,ppIEnumString) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICMetadataQueryReader_INTERFACE_DEFINED__ */


#ifndef __IWICMetadataQueryWriter_INTERFACE_DEFINED__
#define __IWICMetadataQueryWriter_INTERFACE_DEFINED__

/* interface IWICMetadataQueryWriter */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICMetadataQueryWriter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A721791A-0DEF-4d06-BD91-2118BF1DB10B")
    IWICMetadataQueryWriter : public IWICMetadataQueryReader
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetMetadataByName( 
            /* [in] */ __RPC__in LPCWSTR wzName,
            /* [in] */ __RPC__in const PROPVARIANT *pvarValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveMetadataByName( 
            /* [in] */ __RPC__in LPCWSTR wzName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICMetadataQueryWriterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICMetadataQueryWriter * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICMetadataQueryWriter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICMetadataQueryWriter * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainerFormat )( 
            __RPC__in IWICMetadataQueryWriter * This,
            /* [out] */ __RPC__out GUID *pguidContainerFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetLocation )( 
            __RPC__in IWICMetadataQueryWriter * This,
            /* [in] */ UINT cchMaxLength,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchMaxLength) WCHAR *wzNamespace,
            /* [out] */ __RPC__out UINT *pcchActualLength);
        
        HRESULT ( STDMETHODCALLTYPE *GetMetadataByName )( 
            __RPC__in IWICMetadataQueryWriter * This,
            /* [in] */ __RPC__in LPCWSTR wzName,
            /* [unique][out][in] */ __RPC__inout_opt PROPVARIANT *pvarValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetEnumerator )( 
            __RPC__in IWICMetadataQueryWriter * This,
            /* [out] */ __RPC__deref_out_opt IEnumString **ppIEnumString);
        
        HRESULT ( STDMETHODCALLTYPE *SetMetadataByName )( 
            __RPC__in IWICMetadataQueryWriter * This,
            /* [in] */ __RPC__in LPCWSTR wzName,
            /* [in] */ __RPC__in const PROPVARIANT *pvarValue);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveMetadataByName )( 
            __RPC__in IWICMetadataQueryWriter * This,
            /* [in] */ __RPC__in LPCWSTR wzName);
        
        END_INTERFACE
    } IWICMetadataQueryWriterVtbl;

    interface IWICMetadataQueryWriter
    {
        CONST_VTBL struct IWICMetadataQueryWriterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICMetadataQueryWriter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICMetadataQueryWriter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICMetadataQueryWriter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICMetadataQueryWriter_GetContainerFormat(This,pguidContainerFormat)	\
    ( (This)->lpVtbl -> GetContainerFormat(This,pguidContainerFormat) ) 

#define IWICMetadataQueryWriter_GetLocation(This,cchMaxLength,wzNamespace,pcchActualLength)	\
    ( (This)->lpVtbl -> GetLocation(This,cchMaxLength,wzNamespace,pcchActualLength) ) 

#define IWICMetadataQueryWriter_GetMetadataByName(This,wzName,pvarValue)	\
    ( (This)->lpVtbl -> GetMetadataByName(This,wzName,pvarValue) ) 

#define IWICMetadataQueryWriter_GetEnumerator(This,ppIEnumString)	\
    ( (This)->lpVtbl -> GetEnumerator(This,ppIEnumString) ) 


#define IWICMetadataQueryWriter_SetMetadataByName(This,wzName,pvarValue)	\
    ( (This)->lpVtbl -> SetMetadataByName(This,wzName,pvarValue) ) 

#define IWICMetadataQueryWriter_RemoveMetadataByName(This,wzName)	\
    ( (This)->lpVtbl -> RemoveMetadataByName(This,wzName) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICMetadataQueryWriter_INTERFACE_DEFINED__ */


#ifndef __IWICBitmapEncoder_INTERFACE_DEFINED__
#define __IWICBitmapEncoder_INTERFACE_DEFINED__

/* interface IWICBitmapEncoder */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICBitmapEncoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000103-a8f2-4877-ba0a-fd2b6645fb94")
    IWICBitmapEncoder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ __RPC__in_opt IStream *pIStream,
            /* [in] */ WICBitmapEncoderCacheOption cacheOption) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetContainerFormat( 
            /* [out] */ __RPC__out GUID *pguidContainerFormat) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEncoderInfo( 
            /* [out] */ __RPC__deref_out_opt IWICBitmapEncoderInfo **ppIEncoderInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetColorContexts( 
            /* [in] */ UINT cCount,
            /* [size_is][in] */ __RPC__in_ecount_full(cCount) IWICColorContext **ppIColorContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPalette( 
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetThumbnail( 
            /* [in] */ __RPC__in_opt IWICBitmapSource *pIThumbnail) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPreview( 
            /* [in] */ __RPC__in_opt IWICBitmapSource *pIPreview) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateNewFrame( 
            /* [out] */ __RPC__deref_out_opt IWICBitmapFrameEncode **ppIFrameEncode,
            /* [unique][out][in] */ __RPC__deref_opt_inout_opt IPropertyBag2 **ppIEncoderOptions) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Commit( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMetadataQueryWriter( 
            /* [out] */ __RPC__deref_out_opt IWICMetadataQueryWriter **ppIMetadataQueryWriter) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICBitmapEncoderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICBitmapEncoder * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICBitmapEncoder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICBitmapEncoder * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            __RPC__in IWICBitmapEncoder * This,
            /* [in] */ __RPC__in_opt IStream *pIStream,
            /* [in] */ WICBitmapEncoderCacheOption cacheOption);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainerFormat )( 
            __RPC__in IWICBitmapEncoder * This,
            /* [out] */ __RPC__out GUID *pguidContainerFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetEncoderInfo )( 
            __RPC__in IWICBitmapEncoder * This,
            /* [out] */ __RPC__deref_out_opt IWICBitmapEncoderInfo **ppIEncoderInfo);
        
        HRESULT ( STDMETHODCALLTYPE *SetColorContexts )( 
            __RPC__in IWICBitmapEncoder * This,
            /* [in] */ UINT cCount,
            /* [size_is][in] */ __RPC__in_ecount_full(cCount) IWICColorContext **ppIColorContext);
        
        HRESULT ( STDMETHODCALLTYPE *SetPalette )( 
            __RPC__in IWICBitmapEncoder * This,
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette);
        
        HRESULT ( STDMETHODCALLTYPE *SetThumbnail )( 
            __RPC__in IWICBitmapEncoder * This,
            /* [in] */ __RPC__in_opt IWICBitmapSource *pIThumbnail);
        
        HRESULT ( STDMETHODCALLTYPE *SetPreview )( 
            __RPC__in IWICBitmapEncoder * This,
            /* [in] */ __RPC__in_opt IWICBitmapSource *pIPreview);
        
        HRESULT ( STDMETHODCALLTYPE *CreateNewFrame )( 
            __RPC__in IWICBitmapEncoder * This,
            /* [out] */ __RPC__deref_out_opt IWICBitmapFrameEncode **ppIFrameEncode,
            /* [unique][out][in] */ __RPC__deref_opt_inout_opt IPropertyBag2 **ppIEncoderOptions);
        
        HRESULT ( STDMETHODCALLTYPE *Commit )( 
            __RPC__in IWICBitmapEncoder * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetMetadataQueryWriter )( 
            __RPC__in IWICBitmapEncoder * This,
            /* [out] */ __RPC__deref_out_opt IWICMetadataQueryWriter **ppIMetadataQueryWriter);
        
        END_INTERFACE
    } IWICBitmapEncoderVtbl;

    interface IWICBitmapEncoder
    {
        CONST_VTBL struct IWICBitmapEncoderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICBitmapEncoder_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICBitmapEncoder_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICBitmapEncoder_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICBitmapEncoder_Initialize(This,pIStream,cacheOption)	\
    ( (This)->lpVtbl -> Initialize(This,pIStream,cacheOption) ) 

#define IWICBitmapEncoder_GetContainerFormat(This,pguidContainerFormat)	\
    ( (This)->lpVtbl -> GetContainerFormat(This,pguidContainerFormat) ) 

#define IWICBitmapEncoder_GetEncoderInfo(This,ppIEncoderInfo)	\
    ( (This)->lpVtbl -> GetEncoderInfo(This,ppIEncoderInfo) ) 

#define IWICBitmapEncoder_SetColorContexts(This,cCount,ppIColorContext)	\
    ( (This)->lpVtbl -> SetColorContexts(This,cCount,ppIColorContext) ) 

#define IWICBitmapEncoder_SetPalette(This,pIPalette)	\
    ( (This)->lpVtbl -> SetPalette(This,pIPalette) ) 

#define IWICBitmapEncoder_SetThumbnail(This,pIThumbnail)	\
    ( (This)->lpVtbl -> SetThumbnail(This,pIThumbnail) ) 

#define IWICBitmapEncoder_SetPreview(This,pIPreview)	\
    ( (This)->lpVtbl -> SetPreview(This,pIPreview) ) 

#define IWICBitmapEncoder_CreateNewFrame(This,ppIFrameEncode,ppIEncoderOptions)	\
    ( (This)->lpVtbl -> CreateNewFrame(This,ppIFrameEncode,ppIEncoderOptions) ) 

#define IWICBitmapEncoder_Commit(This)	\
    ( (This)->lpVtbl -> Commit(This) ) 

#define IWICBitmapEncoder_GetMetadataQueryWriter(This,ppIMetadataQueryWriter)	\
    ( (This)->lpVtbl -> GetMetadataQueryWriter(This,ppIMetadataQueryWriter) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICBitmapEncoder_INTERFACE_DEFINED__ */


#ifndef __IWICBitmapFrameEncode_INTERFACE_DEFINED__
#define __IWICBitmapFrameEncode_INTERFACE_DEFINED__

/* interface IWICBitmapFrameEncode */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICBitmapFrameEncode;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000105-a8f2-4877-ba0a-fd2b6645fb94")
    IWICBitmapFrameEncode : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [unique][in] */ __RPC__in_opt IPropertyBag2 *pIEncoderOptions) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSize( 
            /* [in] */ UINT uiWidth,
            /* [in] */ UINT uiHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetResolution( 
            /* [in] */ double dpiX,
            /* [in] */ double dpiY) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPixelFormat( 
            /* [out][in] */ __RPC__inout WICPixelFormatGUID *pPixelFormat) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetColorContexts( 
            /* [in] */ UINT cCount,
            /* [size_is][in] */ __RPC__in_ecount_full(cCount) IWICColorContext **ppIColorContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPalette( 
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetThumbnail( 
            /* [in] */ __RPC__in_opt IWICBitmapSource *pIThumbnail) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WritePixels( 
            /* [in] */ UINT lineCount,
            /* [in] */ UINT cbStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][in] */ __RPC__in_ecount_full(cbBufferSize) BYTE *pbPixels) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WriteSource( 
            /* [in] */ __RPC__in_opt IWICBitmapSource *pIBitmapSource,
            /* [unique][in] */ __RPC__in_opt WICRect *prc) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Commit( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMetadataQueryWriter( 
            /* [out] */ __RPC__deref_out_opt IWICMetadataQueryWriter **ppIMetadataQueryWriter) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICBitmapFrameEncodeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICBitmapFrameEncode * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICBitmapFrameEncode * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICBitmapFrameEncode * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            __RPC__in IWICBitmapFrameEncode * This,
            /* [unique][in] */ __RPC__in_opt IPropertyBag2 *pIEncoderOptions);
        
        HRESULT ( STDMETHODCALLTYPE *SetSize )( 
            __RPC__in IWICBitmapFrameEncode * This,
            /* [in] */ UINT uiWidth,
            /* [in] */ UINT uiHeight);
        
        HRESULT ( STDMETHODCALLTYPE *SetResolution )( 
            __RPC__in IWICBitmapFrameEncode * This,
            /* [in] */ double dpiX,
            /* [in] */ double dpiY);
        
        HRESULT ( STDMETHODCALLTYPE *SetPixelFormat )( 
            __RPC__in IWICBitmapFrameEncode * This,
            /* [out][in] */ __RPC__inout WICPixelFormatGUID *pPixelFormat);
        
        HRESULT ( STDMETHODCALLTYPE *SetColorContexts )( 
            __RPC__in IWICBitmapFrameEncode * This,
            /* [in] */ UINT cCount,
            /* [size_is][in] */ __RPC__in_ecount_full(cCount) IWICColorContext **ppIColorContext);
        
        HRESULT ( STDMETHODCALLTYPE *SetPalette )( 
            __RPC__in IWICBitmapFrameEncode * This,
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette);
        
        HRESULT ( STDMETHODCALLTYPE *SetThumbnail )( 
            __RPC__in IWICBitmapFrameEncode * This,
            /* [in] */ __RPC__in_opt IWICBitmapSource *pIThumbnail);
        
        HRESULT ( STDMETHODCALLTYPE *WritePixels )( 
            __RPC__in IWICBitmapFrameEncode * This,
            /* [in] */ UINT lineCount,
            /* [in] */ UINT cbStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][in] */ __RPC__in_ecount_full(cbBufferSize) BYTE *pbPixels);
        
        HRESULT ( STDMETHODCALLTYPE *WriteSource )( 
            __RPC__in IWICBitmapFrameEncode * This,
            /* [in] */ __RPC__in_opt IWICBitmapSource *pIBitmapSource,
            /* [unique][in] */ __RPC__in_opt WICRect *prc);
        
        HRESULT ( STDMETHODCALLTYPE *Commit )( 
            __RPC__in IWICBitmapFrameEncode * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetMetadataQueryWriter )( 
            __RPC__in IWICBitmapFrameEncode * This,
            /* [out] */ __RPC__deref_out_opt IWICMetadataQueryWriter **ppIMetadataQueryWriter);
        
        END_INTERFACE
    } IWICBitmapFrameEncodeVtbl;

    interface IWICBitmapFrameEncode
    {
        CONST_VTBL struct IWICBitmapFrameEncodeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICBitmapFrameEncode_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICBitmapFrameEncode_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICBitmapFrameEncode_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICBitmapFrameEncode_Initialize(This,pIEncoderOptions)	\
    ( (This)->lpVtbl -> Initialize(This,pIEncoderOptions) ) 

#define IWICBitmapFrameEncode_SetSize(This,uiWidth,uiHeight)	\
    ( (This)->lpVtbl -> SetSize(This,uiWidth,uiHeight) ) 

#define IWICBitmapFrameEncode_SetResolution(This,dpiX,dpiY)	\
    ( (This)->lpVtbl -> SetResolution(This,dpiX,dpiY) ) 

#define IWICBitmapFrameEncode_SetPixelFormat(This,pPixelFormat)	\
    ( (This)->lpVtbl -> SetPixelFormat(This,pPixelFormat) ) 

#define IWICBitmapFrameEncode_SetColorContexts(This,cCount,ppIColorContext)	\
    ( (This)->lpVtbl -> SetColorContexts(This,cCount,ppIColorContext) ) 

#define IWICBitmapFrameEncode_SetPalette(This,pIPalette)	\
    ( (This)->lpVtbl -> SetPalette(This,pIPalette) ) 

#define IWICBitmapFrameEncode_SetThumbnail(This,pIThumbnail)	\
    ( (This)->lpVtbl -> SetThumbnail(This,pIThumbnail) ) 

#define IWICBitmapFrameEncode_WritePixels(This,lineCount,cbStride,cbBufferSize,pbPixels)	\
    ( (This)->lpVtbl -> WritePixels(This,lineCount,cbStride,cbBufferSize,pbPixels) ) 

#define IWICBitmapFrameEncode_WriteSource(This,pIBitmapSource,prc)	\
    ( (This)->lpVtbl -> WriteSource(This,pIBitmapSource,prc) ) 

#define IWICBitmapFrameEncode_Commit(This)	\
    ( (This)->lpVtbl -> Commit(This) ) 

#define IWICBitmapFrameEncode_GetMetadataQueryWriter(This,ppIMetadataQueryWriter)	\
    ( (This)->lpVtbl -> GetMetadataQueryWriter(This,ppIMetadataQueryWriter) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICBitmapFrameEncode_INTERFACE_DEFINED__ */


#ifndef __IWICBitmapDecoder_INTERFACE_DEFINED__
#define __IWICBitmapDecoder_INTERFACE_DEFINED__

/* interface IWICBitmapDecoder */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICBitmapDecoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9EDDE9E7-8DEE-47ea-99DF-E6FAF2ED44BF")
    IWICBitmapDecoder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE QueryCapability( 
            /* [in] */ __RPC__in_opt IStream *pIStream,
            /* [out] */ __RPC__out DWORD *pdwCapability) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ __RPC__in_opt IStream *pIStream,
            /* [in] */ WICDecodeOptions cacheOptions) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetContainerFormat( 
            /* [out] */ __RPC__out GUID *pguidContainerFormat) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDecoderInfo( 
            /* [out] */ __RPC__deref_out_opt IWICBitmapDecoderInfo **ppIDecoderInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CopyPalette( 
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMetadataQueryReader( 
            /* [out] */ __RPC__deref_out_opt IWICMetadataQueryReader **ppIMetadataQueryReader) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPreview( 
            /* [out] */ __RPC__deref_out_opt IWICBitmapSource **ppIBitmapSource) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetColorContexts( 
            /* [in] */ UINT cCount,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cCount) IWICColorContext **ppIColorContexts,
            /* [out] */ __RPC__out UINT *pcActualCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetThumbnail( 
            /* [out] */ __RPC__deref_out_opt IWICBitmapSource **ppIThumbnail) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFrameCount( 
            /* [out] */ __RPC__out UINT *pCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFrame( 
            /* [in] */ UINT index,
            /* [out] */ __RPC__deref_out_opt IWICBitmapFrameDecode **ppIBitmapFrame) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICBitmapDecoderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICBitmapDecoder * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICBitmapDecoder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICBitmapDecoder * This);
        
        HRESULT ( STDMETHODCALLTYPE *QueryCapability )( 
            __RPC__in IWICBitmapDecoder * This,
            /* [in] */ __RPC__in_opt IStream *pIStream,
            /* [out] */ __RPC__out DWORD *pdwCapability);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            __RPC__in IWICBitmapDecoder * This,
            /* [in] */ __RPC__in_opt IStream *pIStream,
            /* [in] */ WICDecodeOptions cacheOptions);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainerFormat )( 
            __RPC__in IWICBitmapDecoder * This,
            /* [out] */ __RPC__out GUID *pguidContainerFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetDecoderInfo )( 
            __RPC__in IWICBitmapDecoder * This,
            /* [out] */ __RPC__deref_out_opt IWICBitmapDecoderInfo **ppIDecoderInfo);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPalette )( 
            __RPC__in IWICBitmapDecoder * This,
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette);
        
        HRESULT ( STDMETHODCALLTYPE *GetMetadataQueryReader )( 
            __RPC__in IWICBitmapDecoder * This,
            /* [out] */ __RPC__deref_out_opt IWICMetadataQueryReader **ppIMetadataQueryReader);
        
        HRESULT ( STDMETHODCALLTYPE *GetPreview )( 
            __RPC__in IWICBitmapDecoder * This,
            /* [out] */ __RPC__deref_out_opt IWICBitmapSource **ppIBitmapSource);
        
        HRESULT ( STDMETHODCALLTYPE *GetColorContexts )( 
            __RPC__in IWICBitmapDecoder * This,
            /* [in] */ UINT cCount,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cCount) IWICColorContext **ppIColorContexts,
            /* [out] */ __RPC__out UINT *pcActualCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetThumbnail )( 
            __RPC__in IWICBitmapDecoder * This,
            /* [out] */ __RPC__deref_out_opt IWICBitmapSource **ppIThumbnail);
        
        HRESULT ( STDMETHODCALLTYPE *GetFrameCount )( 
            __RPC__in IWICBitmapDecoder * This,
            /* [out] */ __RPC__out UINT *pCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetFrame )( 
            __RPC__in IWICBitmapDecoder * This,
            /* [in] */ UINT index,
            /* [out] */ __RPC__deref_out_opt IWICBitmapFrameDecode **ppIBitmapFrame);
        
        END_INTERFACE
    } IWICBitmapDecoderVtbl;

    interface IWICBitmapDecoder
    {
        CONST_VTBL struct IWICBitmapDecoderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICBitmapDecoder_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICBitmapDecoder_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICBitmapDecoder_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICBitmapDecoder_QueryCapability(This,pIStream,pdwCapability)	\
    ( (This)->lpVtbl -> QueryCapability(This,pIStream,pdwCapability) ) 

#define IWICBitmapDecoder_Initialize(This,pIStream,cacheOptions)	\
    ( (This)->lpVtbl -> Initialize(This,pIStream,cacheOptions) ) 

#define IWICBitmapDecoder_GetContainerFormat(This,pguidContainerFormat)	\
    ( (This)->lpVtbl -> GetContainerFormat(This,pguidContainerFormat) ) 

#define IWICBitmapDecoder_GetDecoderInfo(This,ppIDecoderInfo)	\
    ( (This)->lpVtbl -> GetDecoderInfo(This,ppIDecoderInfo) ) 

#define IWICBitmapDecoder_CopyPalette(This,pIPalette)	\
    ( (This)->lpVtbl -> CopyPalette(This,pIPalette) ) 

#define IWICBitmapDecoder_GetMetadataQueryReader(This,ppIMetadataQueryReader)	\
    ( (This)->lpVtbl -> GetMetadataQueryReader(This,ppIMetadataQueryReader) ) 

#define IWICBitmapDecoder_GetPreview(This,ppIBitmapSource)	\
    ( (This)->lpVtbl -> GetPreview(This,ppIBitmapSource) ) 

#define IWICBitmapDecoder_GetColorContexts(This,cCount,ppIColorContexts,pcActualCount)	\
    ( (This)->lpVtbl -> GetColorContexts(This,cCount,ppIColorContexts,pcActualCount) ) 

#define IWICBitmapDecoder_GetThumbnail(This,ppIThumbnail)	\
    ( (This)->lpVtbl -> GetThumbnail(This,ppIThumbnail) ) 

#define IWICBitmapDecoder_GetFrameCount(This,pCount)	\
    ( (This)->lpVtbl -> GetFrameCount(This,pCount) ) 

#define IWICBitmapDecoder_GetFrame(This,index,ppIBitmapFrame)	\
    ( (This)->lpVtbl -> GetFrame(This,index,ppIBitmapFrame) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICBitmapDecoder_INTERFACE_DEFINED__ */


#ifndef __IWICBitmapSourceTransform_INTERFACE_DEFINED__
#define __IWICBitmapSourceTransform_INTERFACE_DEFINED__

/* interface IWICBitmapSourceTransform */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICBitmapSourceTransform;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3B16811B-6A43-4ec9-B713-3D5A0C13B940")
    IWICBitmapSourceTransform : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CopyPixels( 
            /* [unique][in] */ __RPC__in_opt const WICRect *prc,
            /* [in] */ UINT uiWidth,
            /* [in] */ UINT uiHeight,
            /* [unique][in] */ __RPC__in_opt WICPixelFormatGUID *pguidDstFormat,
            /* [in] */ WICBitmapTransformOptions dstTransform,
            /* [in] */ UINT nStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][out] */ __RPC__out_ecount_full(cbBufferSize) BYTE *pbBuffer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetClosestSize( 
            /* [out][in] */ __RPC__inout UINT *puiWidth,
            /* [out][in] */ __RPC__inout UINT *puiHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetClosestPixelFormat( 
            /* [out][in] */ __RPC__inout WICPixelFormatGUID *pguidDstFormat) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DoesSupportTransform( 
            /* [in] */ WICBitmapTransformOptions dstTransform,
            /* [out] */ __RPC__out BOOL *pfIsSupported) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICBitmapSourceTransformVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICBitmapSourceTransform * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICBitmapSourceTransform * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICBitmapSourceTransform * This);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPixels )( 
            __RPC__in IWICBitmapSourceTransform * This,
            /* [unique][in] */ __RPC__in_opt const WICRect *prc,
            /* [in] */ UINT uiWidth,
            /* [in] */ UINT uiHeight,
            /* [unique][in] */ __RPC__in_opt WICPixelFormatGUID *pguidDstFormat,
            /* [in] */ WICBitmapTransformOptions dstTransform,
            /* [in] */ UINT nStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][out] */ __RPC__out_ecount_full(cbBufferSize) BYTE *pbBuffer);
        
        HRESULT ( STDMETHODCALLTYPE *GetClosestSize )( 
            __RPC__in IWICBitmapSourceTransform * This,
            /* [out][in] */ __RPC__inout UINT *puiWidth,
            /* [out][in] */ __RPC__inout UINT *puiHeight);
        
        HRESULT ( STDMETHODCALLTYPE *GetClosestPixelFormat )( 
            __RPC__in IWICBitmapSourceTransform * This,
            /* [out][in] */ __RPC__inout WICPixelFormatGUID *pguidDstFormat);
        
        HRESULT ( STDMETHODCALLTYPE *DoesSupportTransform )( 
            __RPC__in IWICBitmapSourceTransform * This,
            /* [in] */ WICBitmapTransformOptions dstTransform,
            /* [out] */ __RPC__out BOOL *pfIsSupported);
        
        END_INTERFACE
    } IWICBitmapSourceTransformVtbl;

    interface IWICBitmapSourceTransform
    {
        CONST_VTBL struct IWICBitmapSourceTransformVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICBitmapSourceTransform_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICBitmapSourceTransform_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICBitmapSourceTransform_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICBitmapSourceTransform_CopyPixels(This,prc,uiWidth,uiHeight,pguidDstFormat,dstTransform,nStride,cbBufferSize,pbBuffer)	\
    ( (This)->lpVtbl -> CopyPixels(This,prc,uiWidth,uiHeight,pguidDstFormat,dstTransform,nStride,cbBufferSize,pbBuffer) ) 

#define IWICBitmapSourceTransform_GetClosestSize(This,puiWidth,puiHeight)	\
    ( (This)->lpVtbl -> GetClosestSize(This,puiWidth,puiHeight) ) 

#define IWICBitmapSourceTransform_GetClosestPixelFormat(This,pguidDstFormat)	\
    ( (This)->lpVtbl -> GetClosestPixelFormat(This,pguidDstFormat) ) 

#define IWICBitmapSourceTransform_DoesSupportTransform(This,dstTransform,pfIsSupported)	\
    ( (This)->lpVtbl -> DoesSupportTransform(This,dstTransform,pfIsSupported) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICBitmapSourceTransform_INTERFACE_DEFINED__ */


#ifndef __IWICBitmapFrameDecode_INTERFACE_DEFINED__
#define __IWICBitmapFrameDecode_INTERFACE_DEFINED__

/* interface IWICBitmapFrameDecode */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICBitmapFrameDecode;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3B16811B-6A43-4ec9-A813-3D930C13B940")
    IWICBitmapFrameDecode : public IWICBitmapSource
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetMetadataQueryReader( 
            /* [out] */ __RPC__deref_out_opt IWICMetadataQueryReader **ppIMetadataQueryReader) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetColorContexts( 
            /* [in] */ UINT cCount,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cCount) IWICColorContext **ppIColorContexts,
            /* [out] */ __RPC__out UINT *pcActualCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetThumbnail( 
            /* [out] */ __RPC__deref_out_opt IWICBitmapSource **ppIThumbnail) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICBitmapFrameDecodeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICBitmapFrameDecode * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICBitmapFrameDecode * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICBitmapFrameDecode * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            __RPC__in IWICBitmapFrameDecode * This,
            /* [out] */ __RPC__out UINT *puiWidth,
            /* [out] */ __RPC__out UINT *puiHeight);
        
        HRESULT ( STDMETHODCALLTYPE *GetPixelFormat )( 
            __RPC__in IWICBitmapFrameDecode * This,
            /* [out] */ __RPC__out WICPixelFormatGUID *pPixelFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetResolution )( 
            __RPC__in IWICBitmapFrameDecode * This,
            /* [out] */ __RPC__out double *pDpiX,
            /* [out] */ __RPC__out double *pDpiY);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPalette )( 
            __RPC__in IWICBitmapFrameDecode * This,
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPixels )( 
            __RPC__in IWICBitmapFrameDecode * This,
            /* [unique][in] */ __RPC__in_opt const WICRect *prc,
            /* [in] */ UINT cbStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][out] */ __RPC__out_ecount_full(cbBufferSize) BYTE *pbBuffer);
        
        HRESULT ( STDMETHODCALLTYPE *GetMetadataQueryReader )( 
            __RPC__in IWICBitmapFrameDecode * This,
            /* [out] */ __RPC__deref_out_opt IWICMetadataQueryReader **ppIMetadataQueryReader);
        
        HRESULT ( STDMETHODCALLTYPE *GetColorContexts )( 
            __RPC__in IWICBitmapFrameDecode * This,
            /* [in] */ UINT cCount,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cCount) IWICColorContext **ppIColorContexts,
            /* [out] */ __RPC__out UINT *pcActualCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetThumbnail )( 
            __RPC__in IWICBitmapFrameDecode * This,
            /* [out] */ __RPC__deref_out_opt IWICBitmapSource **ppIThumbnail);
        
        END_INTERFACE
    } IWICBitmapFrameDecodeVtbl;

    interface IWICBitmapFrameDecode
    {
        CONST_VTBL struct IWICBitmapFrameDecodeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICBitmapFrameDecode_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICBitmapFrameDecode_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICBitmapFrameDecode_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICBitmapFrameDecode_GetSize(This,puiWidth,puiHeight)	\
    ( (This)->lpVtbl -> GetSize(This,puiWidth,puiHeight) ) 

#define IWICBitmapFrameDecode_GetPixelFormat(This,pPixelFormat)	\
    ( (This)->lpVtbl -> GetPixelFormat(This,pPixelFormat) ) 

#define IWICBitmapFrameDecode_GetResolution(This,pDpiX,pDpiY)	\
    ( (This)->lpVtbl -> GetResolution(This,pDpiX,pDpiY) ) 

#define IWICBitmapFrameDecode_CopyPalette(This,pIPalette)	\
    ( (This)->lpVtbl -> CopyPalette(This,pIPalette) ) 

#define IWICBitmapFrameDecode_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)	\
    ( (This)->lpVtbl -> CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer) ) 


#define IWICBitmapFrameDecode_GetMetadataQueryReader(This,ppIMetadataQueryReader)	\
    ( (This)->lpVtbl -> GetMetadataQueryReader(This,ppIMetadataQueryReader) ) 

#define IWICBitmapFrameDecode_GetColorContexts(This,cCount,ppIColorContexts,pcActualCount)	\
    ( (This)->lpVtbl -> GetColorContexts(This,cCount,ppIColorContexts,pcActualCount) ) 

#define IWICBitmapFrameDecode_GetThumbnail(This,ppIThumbnail)	\
    ( (This)->lpVtbl -> GetThumbnail(This,ppIThumbnail) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICBitmapFrameDecode_INTERFACE_DEFINED__ */


#ifndef __IWICProgressiveLevelControl_INTERFACE_DEFINED__
#define __IWICProgressiveLevelControl_INTERFACE_DEFINED__

/* interface IWICProgressiveLevelControl */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICProgressiveLevelControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DAAC296F-7AA5-4dbf-8D15-225C5976F891")
    IWICProgressiveLevelControl : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetLevelCount( 
            /* [retval][out] */ __RPC__out UINT *pcLevels) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentLevel( 
            /* [retval][out] */ __RPC__out UINT *pnLevel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCurrentLevel( 
            /* [in] */ UINT nLevel) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICProgressiveLevelControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICProgressiveLevelControl * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICProgressiveLevelControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICProgressiveLevelControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetLevelCount )( 
            __RPC__in IWICProgressiveLevelControl * This,
            /* [retval][out] */ __RPC__out UINT *pcLevels);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentLevel )( 
            __RPC__in IWICProgressiveLevelControl * This,
            /* [retval][out] */ __RPC__out UINT *pnLevel);
        
        HRESULT ( STDMETHODCALLTYPE *SetCurrentLevel )( 
            __RPC__in IWICProgressiveLevelControl * This,
            /* [in] */ UINT nLevel);
        
        END_INTERFACE
    } IWICProgressiveLevelControlVtbl;

    interface IWICProgressiveLevelControl
    {
        CONST_VTBL struct IWICProgressiveLevelControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICProgressiveLevelControl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICProgressiveLevelControl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICProgressiveLevelControl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICProgressiveLevelControl_GetLevelCount(This,pcLevels)	\
    ( (This)->lpVtbl -> GetLevelCount(This,pcLevels) ) 

#define IWICProgressiveLevelControl_GetCurrentLevel(This,pnLevel)	\
    ( (This)->lpVtbl -> GetCurrentLevel(This,pnLevel) ) 

#define IWICProgressiveLevelControl_SetCurrentLevel(This,nLevel)	\
    ( (This)->lpVtbl -> SetCurrentLevel(This,nLevel) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICProgressiveLevelControl_INTERFACE_DEFINED__ */


#ifndef __IWICProgressCallback_INTERFACE_DEFINED__
#define __IWICProgressCallback_INTERFACE_DEFINED__

/* interface IWICProgressCallback */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICProgressCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4776F9CD-9517-45FA-BF24-E89C5EC5C60C")
    IWICProgressCallback : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Notify( 
            /* [in] */ ULONG uFrameNum,
            /* [in] */ WICProgressOperation operation,
            /* [in] */ double dblProgress) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICProgressCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICProgressCallback * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICProgressCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICProgressCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *Notify )( 
            __RPC__in IWICProgressCallback * This,
            /* [in] */ ULONG uFrameNum,
            /* [in] */ WICProgressOperation operation,
            /* [in] */ double dblProgress);
        
        END_INTERFACE
    } IWICProgressCallbackVtbl;

    interface IWICProgressCallback
    {
        CONST_VTBL struct IWICProgressCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICProgressCallback_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICProgressCallback_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICProgressCallback_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICProgressCallback_Notify(This,uFrameNum,operation,dblProgress)	\
    ( (This)->lpVtbl -> Notify(This,uFrameNum,operation,dblProgress) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICProgressCallback_INTERFACE_DEFINED__ */


#ifndef __IWICBitmapCodecProgressNotification_INTERFACE_DEFINED__
#define __IWICBitmapCodecProgressNotification_INTERFACE_DEFINED__

/* interface IWICBitmapCodecProgressNotification */
/* [uuid][object] */ 

typedef /* [unique] */  __RPC_unique_pointer HRESULT ( __stdcall *PFNProgressNotification )( 
    __RPC__in LPVOID pvData,
    ULONG uFrameNum,
    WICProgressOperation operation,
    double dblProgress);


EXTERN_C const IID IID_IWICBitmapCodecProgressNotification;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("64C1024E-C3CF-4462-8078-88C2B11C46D9")
    IWICBitmapCodecProgressNotification : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE RegisterProgressNotification( 
            /* [annotation][unique][in] */ 
            __in_opt  PFNProgressNotification pfnProgressNotification,
            /* [annotation][unique][in] */ 
            __in_opt  LPVOID pvData,
            /* [in] */ DWORD dwProgressFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICBitmapCodecProgressNotificationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICBitmapCodecProgressNotification * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICBitmapCodecProgressNotification * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICBitmapCodecProgressNotification * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *RegisterProgressNotification )( 
            IWICBitmapCodecProgressNotification * This,
            /* [annotation][unique][in] */ 
            __in_opt  PFNProgressNotification pfnProgressNotification,
            /* [annotation][unique][in] */ 
            __in_opt  LPVOID pvData,
            /* [in] */ DWORD dwProgressFlags);
        
        END_INTERFACE
    } IWICBitmapCodecProgressNotificationVtbl;

    interface IWICBitmapCodecProgressNotification
    {
        CONST_VTBL struct IWICBitmapCodecProgressNotificationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICBitmapCodecProgressNotification_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICBitmapCodecProgressNotification_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICBitmapCodecProgressNotification_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICBitmapCodecProgressNotification_RegisterProgressNotification(This,pfnProgressNotification,pvData,dwProgressFlags)	\
    ( (This)->lpVtbl -> RegisterProgressNotification(This,pfnProgressNotification,pvData,dwProgressFlags) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IWICBitmapCodecProgressNotification_Remote_RegisterProgressNotification_Proxy( 
    __RPC__in IWICBitmapCodecProgressNotification * This,
    /* [unique][in] */ __RPC__in_opt IWICProgressCallback *pICallback,
    /* [in] */ DWORD dwProgressFlags);


void __RPC_STUB IWICBitmapCodecProgressNotification_Remote_RegisterProgressNotification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWICBitmapCodecProgressNotification_INTERFACE_DEFINED__ */


#ifndef __IWICComponentInfo_INTERFACE_DEFINED__
#define __IWICComponentInfo_INTERFACE_DEFINED__

/* interface IWICComponentInfo */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICComponentInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("23BC3F0A-698B-4357-886B-F24D50671334")
    IWICComponentInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetComponentType( 
            /* [out] */ __RPC__out WICComponentType *pType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCLSID( 
            /* [out] */ __RPC__out CLSID *pclsid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSigningStatus( 
            /* [out] */ __RPC__out DWORD *pStatus) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAuthor( 
            /* [in] */ UINT cchAuthor,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchAuthor) WCHAR *wzAuthor,
            /* [out] */ __RPC__out UINT *pcchActual) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVendorGUID( 
            /* [out] */ __RPC__out GUID *pguidVendor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVersion( 
            /* [in] */ UINT cchVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchVersion) WCHAR *wzVersion,
            /* [out] */ __RPC__out UINT *pcchActual) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSpecVersion( 
            /* [in] */ UINT cchSpecVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchSpecVersion) WCHAR *wzSpecVersion,
            /* [out] */ __RPC__out UINT *pcchActual) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFriendlyName( 
            /* [in] */ UINT cchFriendlyName,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchFriendlyName) WCHAR *wzFriendlyName,
            /* [out] */ __RPC__out UINT *pcchActual) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICComponentInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICComponentInfo * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICComponentInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICComponentInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetComponentType )( 
            __RPC__in IWICComponentInfo * This,
            /* [out] */ __RPC__out WICComponentType *pType);
        
        HRESULT ( STDMETHODCALLTYPE *GetCLSID )( 
            __RPC__in IWICComponentInfo * This,
            /* [out] */ __RPC__out CLSID *pclsid);
        
        HRESULT ( STDMETHODCALLTYPE *GetSigningStatus )( 
            __RPC__in IWICComponentInfo * This,
            /* [out] */ __RPC__out DWORD *pStatus);
        
        HRESULT ( STDMETHODCALLTYPE *GetAuthor )( 
            __RPC__in IWICComponentInfo * This,
            /* [in] */ UINT cchAuthor,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchAuthor) WCHAR *wzAuthor,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetVendorGUID )( 
            __RPC__in IWICComponentInfo * This,
            /* [out] */ __RPC__out GUID *pguidVendor);
        
        HRESULT ( STDMETHODCALLTYPE *GetVersion )( 
            __RPC__in IWICComponentInfo * This,
            /* [in] */ UINT cchVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchVersion) WCHAR *wzVersion,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetSpecVersion )( 
            __RPC__in IWICComponentInfo * This,
            /* [in] */ UINT cchSpecVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchSpecVersion) WCHAR *wzSpecVersion,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetFriendlyName )( 
            __RPC__in IWICComponentInfo * This,
            /* [in] */ UINT cchFriendlyName,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchFriendlyName) WCHAR *wzFriendlyName,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        END_INTERFACE
    } IWICComponentInfoVtbl;

    interface IWICComponentInfo
    {
        CONST_VTBL struct IWICComponentInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICComponentInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICComponentInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICComponentInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICComponentInfo_GetComponentType(This,pType)	\
    ( (This)->lpVtbl -> GetComponentType(This,pType) ) 

#define IWICComponentInfo_GetCLSID(This,pclsid)	\
    ( (This)->lpVtbl -> GetCLSID(This,pclsid) ) 

#define IWICComponentInfo_GetSigningStatus(This,pStatus)	\
    ( (This)->lpVtbl -> GetSigningStatus(This,pStatus) ) 

#define IWICComponentInfo_GetAuthor(This,cchAuthor,wzAuthor,pcchActual)	\
    ( (This)->lpVtbl -> GetAuthor(This,cchAuthor,wzAuthor,pcchActual) ) 

#define IWICComponentInfo_GetVendorGUID(This,pguidVendor)	\
    ( (This)->lpVtbl -> GetVendorGUID(This,pguidVendor) ) 

#define IWICComponentInfo_GetVersion(This,cchVersion,wzVersion,pcchActual)	\
    ( (This)->lpVtbl -> GetVersion(This,cchVersion,wzVersion,pcchActual) ) 

#define IWICComponentInfo_GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual)	\
    ( (This)->lpVtbl -> GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual) ) 

#define IWICComponentInfo_GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual)	\
    ( (This)->lpVtbl -> GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICComponentInfo_INTERFACE_DEFINED__ */


#ifndef __IWICFormatConverterInfo_INTERFACE_DEFINED__
#define __IWICFormatConverterInfo_INTERFACE_DEFINED__

/* interface IWICFormatConverterInfo */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICFormatConverterInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9F34FB65-13F4-4f15-BC57-3726B5E53D9F")
    IWICFormatConverterInfo : public IWICComponentInfo
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPixelFormats( 
            /* [in] */ UINT cFormats,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cFormats) WICPixelFormatGUID *pPixelFormatGUIDs,
            /* [out] */ __RPC__out UINT *pcActual) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateInstance( 
            /* [out] */ __RPC__deref_out_opt IWICFormatConverter **ppIConverter) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICFormatConverterInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICFormatConverterInfo * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICFormatConverterInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICFormatConverterInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetComponentType )( 
            __RPC__in IWICFormatConverterInfo * This,
            /* [out] */ __RPC__out WICComponentType *pType);
        
        HRESULT ( STDMETHODCALLTYPE *GetCLSID )( 
            __RPC__in IWICFormatConverterInfo * This,
            /* [out] */ __RPC__out CLSID *pclsid);
        
        HRESULT ( STDMETHODCALLTYPE *GetSigningStatus )( 
            __RPC__in IWICFormatConverterInfo * This,
            /* [out] */ __RPC__out DWORD *pStatus);
        
        HRESULT ( STDMETHODCALLTYPE *GetAuthor )( 
            __RPC__in IWICFormatConverterInfo * This,
            /* [in] */ UINT cchAuthor,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchAuthor) WCHAR *wzAuthor,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetVendorGUID )( 
            __RPC__in IWICFormatConverterInfo * This,
            /* [out] */ __RPC__out GUID *pguidVendor);
        
        HRESULT ( STDMETHODCALLTYPE *GetVersion )( 
            __RPC__in IWICFormatConverterInfo * This,
            /* [in] */ UINT cchVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchVersion) WCHAR *wzVersion,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetSpecVersion )( 
            __RPC__in IWICFormatConverterInfo * This,
            /* [in] */ UINT cchSpecVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchSpecVersion) WCHAR *wzSpecVersion,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetFriendlyName )( 
            __RPC__in IWICFormatConverterInfo * This,
            /* [in] */ UINT cchFriendlyName,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchFriendlyName) WCHAR *wzFriendlyName,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetPixelFormats )( 
            __RPC__in IWICFormatConverterInfo * This,
            /* [in] */ UINT cFormats,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cFormats) WICPixelFormatGUID *pPixelFormatGUIDs,
            /* [out] */ __RPC__out UINT *pcActual);
        
        HRESULT ( STDMETHODCALLTYPE *CreateInstance )( 
            __RPC__in IWICFormatConverterInfo * This,
            /* [out] */ __RPC__deref_out_opt IWICFormatConverter **ppIConverter);
        
        END_INTERFACE
    } IWICFormatConverterInfoVtbl;

    interface IWICFormatConverterInfo
    {
        CONST_VTBL struct IWICFormatConverterInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICFormatConverterInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICFormatConverterInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICFormatConverterInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICFormatConverterInfo_GetComponentType(This,pType)	\
    ( (This)->lpVtbl -> GetComponentType(This,pType) ) 

#define IWICFormatConverterInfo_GetCLSID(This,pclsid)	\
    ( (This)->lpVtbl -> GetCLSID(This,pclsid) ) 

#define IWICFormatConverterInfo_GetSigningStatus(This,pStatus)	\
    ( (This)->lpVtbl -> GetSigningStatus(This,pStatus) ) 

#define IWICFormatConverterInfo_GetAuthor(This,cchAuthor,wzAuthor,pcchActual)	\
    ( (This)->lpVtbl -> GetAuthor(This,cchAuthor,wzAuthor,pcchActual) ) 

#define IWICFormatConverterInfo_GetVendorGUID(This,pguidVendor)	\
    ( (This)->lpVtbl -> GetVendorGUID(This,pguidVendor) ) 

#define IWICFormatConverterInfo_GetVersion(This,cchVersion,wzVersion,pcchActual)	\
    ( (This)->lpVtbl -> GetVersion(This,cchVersion,wzVersion,pcchActual) ) 

#define IWICFormatConverterInfo_GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual)	\
    ( (This)->lpVtbl -> GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual) ) 

#define IWICFormatConverterInfo_GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual)	\
    ( (This)->lpVtbl -> GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual) ) 


#define IWICFormatConverterInfo_GetPixelFormats(This,cFormats,pPixelFormatGUIDs,pcActual)	\
    ( (This)->lpVtbl -> GetPixelFormats(This,cFormats,pPixelFormatGUIDs,pcActual) ) 

#define IWICFormatConverterInfo_CreateInstance(This,ppIConverter)	\
    ( (This)->lpVtbl -> CreateInstance(This,ppIConverter) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICFormatConverterInfo_INTERFACE_DEFINED__ */


#ifndef __IWICBitmapCodecInfo_INTERFACE_DEFINED__
#define __IWICBitmapCodecInfo_INTERFACE_DEFINED__

/* interface IWICBitmapCodecInfo */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICBitmapCodecInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E87A44C4-B76E-4c47-8B09-298EB12A2714")
    IWICBitmapCodecInfo : public IWICComponentInfo
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetContainerFormat( 
            /* [out] */ __RPC__out GUID *pguidContainerFormat) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPixelFormats( 
            /* [in] */ UINT cFormats,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cFormats) GUID *pguidPixelFormats,
            /* [out] */ __RPC__out UINT *pcActual) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetColorManagementVersion( 
            /* [in] */ UINT cchColorManagementVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchColorManagementVersion) WCHAR *wzColorManagementVersion,
            /* [out] */ __RPC__out UINT *pcchActual) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDeviceManufacturer( 
            /* [in] */ UINT cchDeviceManufacturer,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchDeviceManufacturer) WCHAR *wzDeviceManufacturer,
            /* [out] */ __RPC__out UINT *pcchActual) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDeviceModels( 
            /* [in] */ UINT cchDeviceModels,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchDeviceModels) WCHAR *wzDeviceModels,
            /* [out] */ __RPC__out UINT *pcchActual) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMimeTypes( 
            /* [in] */ UINT cchMimeTypes,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchMimeTypes) WCHAR *wzMimeTypes,
            /* [out] */ __RPC__out UINT *pcchActual) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFileExtensions( 
            /* [in] */ UINT cchFileExtensions,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchFileExtensions) WCHAR *wzFileExtensions,
            /* [out] */ __RPC__out UINT *pcchActual) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DoesSupportAnimation( 
            /* [out] */ __RPC__out BOOL *pfSupportAnimation) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DoesSupportChromakey( 
            /* [out] */ __RPC__out BOOL *pfSupportChromakey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DoesSupportLossless( 
            /* [out] */ __RPC__out BOOL *pfSupportLossless) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DoesSupportMultiframe( 
            /* [out] */ __RPC__out BOOL *pfSupportMultiframe) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MatchesMimeType( 
            /* [in] */ __RPC__in LPCWSTR wzMimeType,
            /* [out] */ __RPC__out BOOL *pfMatches) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICBitmapCodecInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICBitmapCodecInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICBitmapCodecInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetComponentType )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [out] */ __RPC__out WICComponentType *pType);
        
        HRESULT ( STDMETHODCALLTYPE *GetCLSID )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [out] */ __RPC__out CLSID *pclsid);
        
        HRESULT ( STDMETHODCALLTYPE *GetSigningStatus )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [out] */ __RPC__out DWORD *pStatus);
        
        HRESULT ( STDMETHODCALLTYPE *GetAuthor )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [in] */ UINT cchAuthor,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchAuthor) WCHAR *wzAuthor,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetVendorGUID )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [out] */ __RPC__out GUID *pguidVendor);
        
        HRESULT ( STDMETHODCALLTYPE *GetVersion )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [in] */ UINT cchVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchVersion) WCHAR *wzVersion,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetSpecVersion )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [in] */ UINT cchSpecVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchSpecVersion) WCHAR *wzSpecVersion,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetFriendlyName )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [in] */ UINT cchFriendlyName,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchFriendlyName) WCHAR *wzFriendlyName,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainerFormat )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [out] */ __RPC__out GUID *pguidContainerFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetPixelFormats )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [in] */ UINT cFormats,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cFormats) GUID *pguidPixelFormats,
            /* [out] */ __RPC__out UINT *pcActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetColorManagementVersion )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [in] */ UINT cchColorManagementVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchColorManagementVersion) WCHAR *wzColorManagementVersion,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetDeviceManufacturer )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [in] */ UINT cchDeviceManufacturer,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchDeviceManufacturer) WCHAR *wzDeviceManufacturer,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetDeviceModels )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [in] */ UINT cchDeviceModels,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchDeviceModels) WCHAR *wzDeviceModels,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetMimeTypes )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [in] */ UINT cchMimeTypes,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchMimeTypes) WCHAR *wzMimeTypes,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetFileExtensions )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [in] */ UINT cchFileExtensions,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchFileExtensions) WCHAR *wzFileExtensions,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *DoesSupportAnimation )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [out] */ __RPC__out BOOL *pfSupportAnimation);
        
        HRESULT ( STDMETHODCALLTYPE *DoesSupportChromakey )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [out] */ __RPC__out BOOL *pfSupportChromakey);
        
        HRESULT ( STDMETHODCALLTYPE *DoesSupportLossless )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [out] */ __RPC__out BOOL *pfSupportLossless);
        
        HRESULT ( STDMETHODCALLTYPE *DoesSupportMultiframe )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [out] */ __RPC__out BOOL *pfSupportMultiframe);
        
        HRESULT ( STDMETHODCALLTYPE *MatchesMimeType )( 
            __RPC__in IWICBitmapCodecInfo * This,
            /* [in] */ __RPC__in LPCWSTR wzMimeType,
            /* [out] */ __RPC__out BOOL *pfMatches);
        
        END_INTERFACE
    } IWICBitmapCodecInfoVtbl;

    interface IWICBitmapCodecInfo
    {
        CONST_VTBL struct IWICBitmapCodecInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICBitmapCodecInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICBitmapCodecInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICBitmapCodecInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICBitmapCodecInfo_GetComponentType(This,pType)	\
    ( (This)->lpVtbl -> GetComponentType(This,pType) ) 

#define IWICBitmapCodecInfo_GetCLSID(This,pclsid)	\
    ( (This)->lpVtbl -> GetCLSID(This,pclsid) ) 

#define IWICBitmapCodecInfo_GetSigningStatus(This,pStatus)	\
    ( (This)->lpVtbl -> GetSigningStatus(This,pStatus) ) 

#define IWICBitmapCodecInfo_GetAuthor(This,cchAuthor,wzAuthor,pcchActual)	\
    ( (This)->lpVtbl -> GetAuthor(This,cchAuthor,wzAuthor,pcchActual) ) 

#define IWICBitmapCodecInfo_GetVendorGUID(This,pguidVendor)	\
    ( (This)->lpVtbl -> GetVendorGUID(This,pguidVendor) ) 

#define IWICBitmapCodecInfo_GetVersion(This,cchVersion,wzVersion,pcchActual)	\
    ( (This)->lpVtbl -> GetVersion(This,cchVersion,wzVersion,pcchActual) ) 

#define IWICBitmapCodecInfo_GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual)	\
    ( (This)->lpVtbl -> GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual) ) 

#define IWICBitmapCodecInfo_GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual)	\
    ( (This)->lpVtbl -> GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual) ) 


#define IWICBitmapCodecInfo_GetContainerFormat(This,pguidContainerFormat)	\
    ( (This)->lpVtbl -> GetContainerFormat(This,pguidContainerFormat) ) 

#define IWICBitmapCodecInfo_GetPixelFormats(This,cFormats,pguidPixelFormats,pcActual)	\
    ( (This)->lpVtbl -> GetPixelFormats(This,cFormats,pguidPixelFormats,pcActual) ) 

#define IWICBitmapCodecInfo_GetColorManagementVersion(This,cchColorManagementVersion,wzColorManagementVersion,pcchActual)	\
    ( (This)->lpVtbl -> GetColorManagementVersion(This,cchColorManagementVersion,wzColorManagementVersion,pcchActual) ) 

#define IWICBitmapCodecInfo_GetDeviceManufacturer(This,cchDeviceManufacturer,wzDeviceManufacturer,pcchActual)	\
    ( (This)->lpVtbl -> GetDeviceManufacturer(This,cchDeviceManufacturer,wzDeviceManufacturer,pcchActual) ) 

#define IWICBitmapCodecInfo_GetDeviceModels(This,cchDeviceModels,wzDeviceModels,pcchActual)	\
    ( (This)->lpVtbl -> GetDeviceModels(This,cchDeviceModels,wzDeviceModels,pcchActual) ) 

#define IWICBitmapCodecInfo_GetMimeTypes(This,cchMimeTypes,wzMimeTypes,pcchActual)	\
    ( (This)->lpVtbl -> GetMimeTypes(This,cchMimeTypes,wzMimeTypes,pcchActual) ) 

#define IWICBitmapCodecInfo_GetFileExtensions(This,cchFileExtensions,wzFileExtensions,pcchActual)	\
    ( (This)->lpVtbl -> GetFileExtensions(This,cchFileExtensions,wzFileExtensions,pcchActual) ) 

#define IWICBitmapCodecInfo_DoesSupportAnimation(This,pfSupportAnimation)	\
    ( (This)->lpVtbl -> DoesSupportAnimation(This,pfSupportAnimation) ) 

#define IWICBitmapCodecInfo_DoesSupportChromakey(This,pfSupportChromakey)	\
    ( (This)->lpVtbl -> DoesSupportChromakey(This,pfSupportChromakey) ) 

#define IWICBitmapCodecInfo_DoesSupportLossless(This,pfSupportLossless)	\
    ( (This)->lpVtbl -> DoesSupportLossless(This,pfSupportLossless) ) 

#define IWICBitmapCodecInfo_DoesSupportMultiframe(This,pfSupportMultiframe)	\
    ( (This)->lpVtbl -> DoesSupportMultiframe(This,pfSupportMultiframe) ) 

#define IWICBitmapCodecInfo_MatchesMimeType(This,wzMimeType,pfMatches)	\
    ( (This)->lpVtbl -> MatchesMimeType(This,wzMimeType,pfMatches) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICBitmapCodecInfo_INTERFACE_DEFINED__ */


#ifndef __IWICBitmapEncoderInfo_INTERFACE_DEFINED__
#define __IWICBitmapEncoderInfo_INTERFACE_DEFINED__

/* interface IWICBitmapEncoderInfo */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICBitmapEncoderInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("94C9B4EE-A09F-4f92-8A1E-4A9BCE7E76FB")
    IWICBitmapEncoderInfo : public IWICBitmapCodecInfo
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateInstance( 
            /* [out] */ __RPC__deref_out_opt IWICBitmapEncoder **ppIBitmapEncoder) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICBitmapEncoderInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICBitmapEncoderInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICBitmapEncoderInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetComponentType )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [out] */ __RPC__out WICComponentType *pType);
        
        HRESULT ( STDMETHODCALLTYPE *GetCLSID )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [out] */ __RPC__out CLSID *pclsid);
        
        HRESULT ( STDMETHODCALLTYPE *GetSigningStatus )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [out] */ __RPC__out DWORD *pStatus);
        
        HRESULT ( STDMETHODCALLTYPE *GetAuthor )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [in] */ UINT cchAuthor,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchAuthor) WCHAR *wzAuthor,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetVendorGUID )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [out] */ __RPC__out GUID *pguidVendor);
        
        HRESULT ( STDMETHODCALLTYPE *GetVersion )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [in] */ UINT cchVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchVersion) WCHAR *wzVersion,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetSpecVersion )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [in] */ UINT cchSpecVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchSpecVersion) WCHAR *wzSpecVersion,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetFriendlyName )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [in] */ UINT cchFriendlyName,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchFriendlyName) WCHAR *wzFriendlyName,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainerFormat )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [out] */ __RPC__out GUID *pguidContainerFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetPixelFormats )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [in] */ UINT cFormats,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cFormats) GUID *pguidPixelFormats,
            /* [out] */ __RPC__out UINT *pcActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetColorManagementVersion )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [in] */ UINT cchColorManagementVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchColorManagementVersion) WCHAR *wzColorManagementVersion,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetDeviceManufacturer )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [in] */ UINT cchDeviceManufacturer,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchDeviceManufacturer) WCHAR *wzDeviceManufacturer,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetDeviceModels )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [in] */ UINT cchDeviceModels,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchDeviceModels) WCHAR *wzDeviceModels,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetMimeTypes )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [in] */ UINT cchMimeTypes,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchMimeTypes) WCHAR *wzMimeTypes,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetFileExtensions )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [in] */ UINT cchFileExtensions,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchFileExtensions) WCHAR *wzFileExtensions,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *DoesSupportAnimation )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [out] */ __RPC__out BOOL *pfSupportAnimation);
        
        HRESULT ( STDMETHODCALLTYPE *DoesSupportChromakey )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [out] */ __RPC__out BOOL *pfSupportChromakey);
        
        HRESULT ( STDMETHODCALLTYPE *DoesSupportLossless )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [out] */ __RPC__out BOOL *pfSupportLossless);
        
        HRESULT ( STDMETHODCALLTYPE *DoesSupportMultiframe )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [out] */ __RPC__out BOOL *pfSupportMultiframe);
        
        HRESULT ( STDMETHODCALLTYPE *MatchesMimeType )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [in] */ __RPC__in LPCWSTR wzMimeType,
            /* [out] */ __RPC__out BOOL *pfMatches);
        
        HRESULT ( STDMETHODCALLTYPE *CreateInstance )( 
            __RPC__in IWICBitmapEncoderInfo * This,
            /* [out] */ __RPC__deref_out_opt IWICBitmapEncoder **ppIBitmapEncoder);
        
        END_INTERFACE
    } IWICBitmapEncoderInfoVtbl;

    interface IWICBitmapEncoderInfo
    {
        CONST_VTBL struct IWICBitmapEncoderInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICBitmapEncoderInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICBitmapEncoderInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICBitmapEncoderInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICBitmapEncoderInfo_GetComponentType(This,pType)	\
    ( (This)->lpVtbl -> GetComponentType(This,pType) ) 

#define IWICBitmapEncoderInfo_GetCLSID(This,pclsid)	\
    ( (This)->lpVtbl -> GetCLSID(This,pclsid) ) 

#define IWICBitmapEncoderInfo_GetSigningStatus(This,pStatus)	\
    ( (This)->lpVtbl -> GetSigningStatus(This,pStatus) ) 

#define IWICBitmapEncoderInfo_GetAuthor(This,cchAuthor,wzAuthor,pcchActual)	\
    ( (This)->lpVtbl -> GetAuthor(This,cchAuthor,wzAuthor,pcchActual) ) 

#define IWICBitmapEncoderInfo_GetVendorGUID(This,pguidVendor)	\
    ( (This)->lpVtbl -> GetVendorGUID(This,pguidVendor) ) 

#define IWICBitmapEncoderInfo_GetVersion(This,cchVersion,wzVersion,pcchActual)	\
    ( (This)->lpVtbl -> GetVersion(This,cchVersion,wzVersion,pcchActual) ) 

#define IWICBitmapEncoderInfo_GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual)	\
    ( (This)->lpVtbl -> GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual) ) 

#define IWICBitmapEncoderInfo_GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual)	\
    ( (This)->lpVtbl -> GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual) ) 


#define IWICBitmapEncoderInfo_GetContainerFormat(This,pguidContainerFormat)	\
    ( (This)->lpVtbl -> GetContainerFormat(This,pguidContainerFormat) ) 

#define IWICBitmapEncoderInfo_GetPixelFormats(This,cFormats,pguidPixelFormats,pcActual)	\
    ( (This)->lpVtbl -> GetPixelFormats(This,cFormats,pguidPixelFormats,pcActual) ) 

#define IWICBitmapEncoderInfo_GetColorManagementVersion(This,cchColorManagementVersion,wzColorManagementVersion,pcchActual)	\
    ( (This)->lpVtbl -> GetColorManagementVersion(This,cchColorManagementVersion,wzColorManagementVersion,pcchActual) ) 

#define IWICBitmapEncoderInfo_GetDeviceManufacturer(This,cchDeviceManufacturer,wzDeviceManufacturer,pcchActual)	\
    ( (This)->lpVtbl -> GetDeviceManufacturer(This,cchDeviceManufacturer,wzDeviceManufacturer,pcchActual) ) 

#define IWICBitmapEncoderInfo_GetDeviceModels(This,cchDeviceModels,wzDeviceModels,pcchActual)	\
    ( (This)->lpVtbl -> GetDeviceModels(This,cchDeviceModels,wzDeviceModels,pcchActual) ) 

#define IWICBitmapEncoderInfo_GetMimeTypes(This,cchMimeTypes,wzMimeTypes,pcchActual)	\
    ( (This)->lpVtbl -> GetMimeTypes(This,cchMimeTypes,wzMimeTypes,pcchActual) ) 

#define IWICBitmapEncoderInfo_GetFileExtensions(This,cchFileExtensions,wzFileExtensions,pcchActual)	\
    ( (This)->lpVtbl -> GetFileExtensions(This,cchFileExtensions,wzFileExtensions,pcchActual) ) 

#define IWICBitmapEncoderInfo_DoesSupportAnimation(This,pfSupportAnimation)	\
    ( (This)->lpVtbl -> DoesSupportAnimation(This,pfSupportAnimation) ) 

#define IWICBitmapEncoderInfo_DoesSupportChromakey(This,pfSupportChromakey)	\
    ( (This)->lpVtbl -> DoesSupportChromakey(This,pfSupportChromakey) ) 

#define IWICBitmapEncoderInfo_DoesSupportLossless(This,pfSupportLossless)	\
    ( (This)->lpVtbl -> DoesSupportLossless(This,pfSupportLossless) ) 

#define IWICBitmapEncoderInfo_DoesSupportMultiframe(This,pfSupportMultiframe)	\
    ( (This)->lpVtbl -> DoesSupportMultiframe(This,pfSupportMultiframe) ) 

#define IWICBitmapEncoderInfo_MatchesMimeType(This,wzMimeType,pfMatches)	\
    ( (This)->lpVtbl -> MatchesMimeType(This,wzMimeType,pfMatches) ) 


#define IWICBitmapEncoderInfo_CreateInstance(This,ppIBitmapEncoder)	\
    ( (This)->lpVtbl -> CreateInstance(This,ppIBitmapEncoder) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICBitmapEncoderInfo_INTERFACE_DEFINED__ */


#ifndef __IWICBitmapDecoderInfo_INTERFACE_DEFINED__
#define __IWICBitmapDecoderInfo_INTERFACE_DEFINED__

/* interface IWICBitmapDecoderInfo */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICBitmapDecoderInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D8CD007F-D08F-4191-9BFC-236EA7F0E4B5")
    IWICBitmapDecoderInfo : public IWICBitmapCodecInfo
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetPatterns( 
            /* [in] */ UINT cbSizePatterns,
            /* [annotation][unique][size_is][out] */ 
            __out_bcount_part_opt(cbSizePatterns, *pcbPatternsActual)  WICBitmapPattern *pPatterns,
            /* [annotation][unique][out] */ 
            __inout_opt  UINT *pcPatterns,
            /* [annotation][out] */ 
            __inout_opt  UINT *pcbPatternsActual) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MatchesPattern( 
            /* [in] */ __RPC__in_opt IStream *pIStream,
            /* [out] */ __RPC__out BOOL *pfMatches) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateInstance( 
            /* [out] */ __RPC__deref_out_opt IWICBitmapDecoder **ppIBitmapDecoder) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICBitmapDecoderInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICBitmapDecoderInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICBitmapDecoderInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetComponentType )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [out] */ __RPC__out WICComponentType *pType);
        
        HRESULT ( STDMETHODCALLTYPE *GetCLSID )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [out] */ __RPC__out CLSID *pclsid);
        
        HRESULT ( STDMETHODCALLTYPE *GetSigningStatus )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [out] */ __RPC__out DWORD *pStatus);
        
        HRESULT ( STDMETHODCALLTYPE *GetAuthor )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [in] */ UINT cchAuthor,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchAuthor) WCHAR *wzAuthor,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetVendorGUID )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [out] */ __RPC__out GUID *pguidVendor);
        
        HRESULT ( STDMETHODCALLTYPE *GetVersion )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [in] */ UINT cchVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchVersion) WCHAR *wzVersion,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetSpecVersion )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [in] */ UINT cchSpecVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchSpecVersion) WCHAR *wzSpecVersion,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetFriendlyName )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [in] */ UINT cchFriendlyName,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchFriendlyName) WCHAR *wzFriendlyName,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainerFormat )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [out] */ __RPC__out GUID *pguidContainerFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetPixelFormats )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [in] */ UINT cFormats,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cFormats) GUID *pguidPixelFormats,
            /* [out] */ __RPC__out UINT *pcActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetColorManagementVersion )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [in] */ UINT cchColorManagementVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchColorManagementVersion) WCHAR *wzColorManagementVersion,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetDeviceManufacturer )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [in] */ UINT cchDeviceManufacturer,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchDeviceManufacturer) WCHAR *wzDeviceManufacturer,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetDeviceModels )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [in] */ UINT cchDeviceModels,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchDeviceModels) WCHAR *wzDeviceModels,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetMimeTypes )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [in] */ UINT cchMimeTypes,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchMimeTypes) WCHAR *wzMimeTypes,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetFileExtensions )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [in] */ UINT cchFileExtensions,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchFileExtensions) WCHAR *wzFileExtensions,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *DoesSupportAnimation )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [out] */ __RPC__out BOOL *pfSupportAnimation);
        
        HRESULT ( STDMETHODCALLTYPE *DoesSupportChromakey )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [out] */ __RPC__out BOOL *pfSupportChromakey);
        
        HRESULT ( STDMETHODCALLTYPE *DoesSupportLossless )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [out] */ __RPC__out BOOL *pfSupportLossless);
        
        HRESULT ( STDMETHODCALLTYPE *DoesSupportMultiframe )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [out] */ __RPC__out BOOL *pfSupportMultiframe);
        
        HRESULT ( STDMETHODCALLTYPE *MatchesMimeType )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [in] */ __RPC__in LPCWSTR wzMimeType,
            /* [out] */ __RPC__out BOOL *pfMatches);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetPatterns )( 
            IWICBitmapDecoderInfo * This,
            /* [in] */ UINT cbSizePatterns,
            /* [annotation][unique][size_is][out] */ 
            __out_bcount_part_opt(cbSizePatterns, *pcbPatternsActual)  WICBitmapPattern *pPatterns,
            /* [annotation][unique][out] */ 
            __inout_opt  UINT *pcPatterns,
            /* [annotation][out] */ 
            __inout_opt  UINT *pcbPatternsActual);
        
        HRESULT ( STDMETHODCALLTYPE *MatchesPattern )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [in] */ __RPC__in_opt IStream *pIStream,
            /* [out] */ __RPC__out BOOL *pfMatches);
        
        HRESULT ( STDMETHODCALLTYPE *CreateInstance )( 
            __RPC__in IWICBitmapDecoderInfo * This,
            /* [out] */ __RPC__deref_out_opt IWICBitmapDecoder **ppIBitmapDecoder);
        
        END_INTERFACE
    } IWICBitmapDecoderInfoVtbl;

    interface IWICBitmapDecoderInfo
    {
        CONST_VTBL struct IWICBitmapDecoderInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICBitmapDecoderInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICBitmapDecoderInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICBitmapDecoderInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICBitmapDecoderInfo_GetComponentType(This,pType)	\
    ( (This)->lpVtbl -> GetComponentType(This,pType) ) 

#define IWICBitmapDecoderInfo_GetCLSID(This,pclsid)	\
    ( (This)->lpVtbl -> GetCLSID(This,pclsid) ) 

#define IWICBitmapDecoderInfo_GetSigningStatus(This,pStatus)	\
    ( (This)->lpVtbl -> GetSigningStatus(This,pStatus) ) 

#define IWICBitmapDecoderInfo_GetAuthor(This,cchAuthor,wzAuthor,pcchActual)	\
    ( (This)->lpVtbl -> GetAuthor(This,cchAuthor,wzAuthor,pcchActual) ) 

#define IWICBitmapDecoderInfo_GetVendorGUID(This,pguidVendor)	\
    ( (This)->lpVtbl -> GetVendorGUID(This,pguidVendor) ) 

#define IWICBitmapDecoderInfo_GetVersion(This,cchVersion,wzVersion,pcchActual)	\
    ( (This)->lpVtbl -> GetVersion(This,cchVersion,wzVersion,pcchActual) ) 

#define IWICBitmapDecoderInfo_GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual)	\
    ( (This)->lpVtbl -> GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual) ) 

#define IWICBitmapDecoderInfo_GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual)	\
    ( (This)->lpVtbl -> GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual) ) 


#define IWICBitmapDecoderInfo_GetContainerFormat(This,pguidContainerFormat)	\
    ( (This)->lpVtbl -> GetContainerFormat(This,pguidContainerFormat) ) 

#define IWICBitmapDecoderInfo_GetPixelFormats(This,cFormats,pguidPixelFormats,pcActual)	\
    ( (This)->lpVtbl -> GetPixelFormats(This,cFormats,pguidPixelFormats,pcActual) ) 

#define IWICBitmapDecoderInfo_GetColorManagementVersion(This,cchColorManagementVersion,wzColorManagementVersion,pcchActual)	\
    ( (This)->lpVtbl -> GetColorManagementVersion(This,cchColorManagementVersion,wzColorManagementVersion,pcchActual) ) 

#define IWICBitmapDecoderInfo_GetDeviceManufacturer(This,cchDeviceManufacturer,wzDeviceManufacturer,pcchActual)	\
    ( (This)->lpVtbl -> GetDeviceManufacturer(This,cchDeviceManufacturer,wzDeviceManufacturer,pcchActual) ) 

#define IWICBitmapDecoderInfo_GetDeviceModels(This,cchDeviceModels,wzDeviceModels,pcchActual)	\
    ( (This)->lpVtbl -> GetDeviceModels(This,cchDeviceModels,wzDeviceModels,pcchActual) ) 

#define IWICBitmapDecoderInfo_GetMimeTypes(This,cchMimeTypes,wzMimeTypes,pcchActual)	\
    ( (This)->lpVtbl -> GetMimeTypes(This,cchMimeTypes,wzMimeTypes,pcchActual) ) 

#define IWICBitmapDecoderInfo_GetFileExtensions(This,cchFileExtensions,wzFileExtensions,pcchActual)	\
    ( (This)->lpVtbl -> GetFileExtensions(This,cchFileExtensions,wzFileExtensions,pcchActual) ) 

#define IWICBitmapDecoderInfo_DoesSupportAnimation(This,pfSupportAnimation)	\
    ( (This)->lpVtbl -> DoesSupportAnimation(This,pfSupportAnimation) ) 

#define IWICBitmapDecoderInfo_DoesSupportChromakey(This,pfSupportChromakey)	\
    ( (This)->lpVtbl -> DoesSupportChromakey(This,pfSupportChromakey) ) 

#define IWICBitmapDecoderInfo_DoesSupportLossless(This,pfSupportLossless)	\
    ( (This)->lpVtbl -> DoesSupportLossless(This,pfSupportLossless) ) 

#define IWICBitmapDecoderInfo_DoesSupportMultiframe(This,pfSupportMultiframe)	\
    ( (This)->lpVtbl -> DoesSupportMultiframe(This,pfSupportMultiframe) ) 

#define IWICBitmapDecoderInfo_MatchesMimeType(This,wzMimeType,pfMatches)	\
    ( (This)->lpVtbl -> MatchesMimeType(This,wzMimeType,pfMatches) ) 


#define IWICBitmapDecoderInfo_GetPatterns(This,cbSizePatterns,pPatterns,pcPatterns,pcbPatternsActual)	\
    ( (This)->lpVtbl -> GetPatterns(This,cbSizePatterns,pPatterns,pcPatterns,pcbPatternsActual) ) 

#define IWICBitmapDecoderInfo_MatchesPattern(This,pIStream,pfMatches)	\
    ( (This)->lpVtbl -> MatchesPattern(This,pIStream,pfMatches) ) 

#define IWICBitmapDecoderInfo_CreateInstance(This,ppIBitmapDecoder)	\
    ( (This)->lpVtbl -> CreateInstance(This,ppIBitmapDecoder) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IWICBitmapDecoderInfo_Remote_GetPatterns_Proxy( 
    __RPC__in IWICBitmapDecoderInfo * This,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*pcPatterns) WICBitmapPattern **ppPatterns,
    /* [out] */ __RPC__out UINT *pcPatterns);


void __RPC_STUB IWICBitmapDecoderInfo_Remote_GetPatterns_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWICBitmapDecoderInfo_INTERFACE_DEFINED__ */


#ifndef __IWICPixelFormatInfo_INTERFACE_DEFINED__
#define __IWICPixelFormatInfo_INTERFACE_DEFINED__

/* interface IWICPixelFormatInfo */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICPixelFormatInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E8EDA601-3D48-431a-AB44-69059BE88BBE")
    IWICPixelFormatInfo : public IWICComponentInfo
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetFormatGUID( 
            /* [out] */ __RPC__out GUID *pFormat) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetColorContext( 
            /* [out] */ __RPC__deref_out_opt IWICColorContext **ppIColorContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBitsPerPixel( 
            /* [out] */ __RPC__out UINT *puiBitsPerPixel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetChannelCount( 
            /* [out] */ __RPC__out UINT *puiChannelCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetChannelMask( 
            /* [in] */ UINT uiChannelIndex,
            /* [in] */ UINT cbMaskBuffer,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cbMaskBuffer) BYTE *pbMaskBuffer,
            /* [out] */ __RPC__out UINT *pcbActual) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICPixelFormatInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICPixelFormatInfo * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICPixelFormatInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICPixelFormatInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetComponentType )( 
            __RPC__in IWICPixelFormatInfo * This,
            /* [out] */ __RPC__out WICComponentType *pType);
        
        HRESULT ( STDMETHODCALLTYPE *GetCLSID )( 
            __RPC__in IWICPixelFormatInfo * This,
            /* [out] */ __RPC__out CLSID *pclsid);
        
        HRESULT ( STDMETHODCALLTYPE *GetSigningStatus )( 
            __RPC__in IWICPixelFormatInfo * This,
            /* [out] */ __RPC__out DWORD *pStatus);
        
        HRESULT ( STDMETHODCALLTYPE *GetAuthor )( 
            __RPC__in IWICPixelFormatInfo * This,
            /* [in] */ UINT cchAuthor,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchAuthor) WCHAR *wzAuthor,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetVendorGUID )( 
            __RPC__in IWICPixelFormatInfo * This,
            /* [out] */ __RPC__out GUID *pguidVendor);
        
        HRESULT ( STDMETHODCALLTYPE *GetVersion )( 
            __RPC__in IWICPixelFormatInfo * This,
            /* [in] */ UINT cchVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchVersion) WCHAR *wzVersion,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetSpecVersion )( 
            __RPC__in IWICPixelFormatInfo * This,
            /* [in] */ UINT cchSpecVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchSpecVersion) WCHAR *wzSpecVersion,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetFriendlyName )( 
            __RPC__in IWICPixelFormatInfo * This,
            /* [in] */ UINT cchFriendlyName,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchFriendlyName) WCHAR *wzFriendlyName,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetFormatGUID )( 
            __RPC__in IWICPixelFormatInfo * This,
            /* [out] */ __RPC__out GUID *pFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetColorContext )( 
            __RPC__in IWICPixelFormatInfo * This,
            /* [out] */ __RPC__deref_out_opt IWICColorContext **ppIColorContext);
        
        HRESULT ( STDMETHODCALLTYPE *GetBitsPerPixel )( 
            __RPC__in IWICPixelFormatInfo * This,
            /* [out] */ __RPC__out UINT *puiBitsPerPixel);
        
        HRESULT ( STDMETHODCALLTYPE *GetChannelCount )( 
            __RPC__in IWICPixelFormatInfo * This,
            /* [out] */ __RPC__out UINT *puiChannelCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetChannelMask )( 
            __RPC__in IWICPixelFormatInfo * This,
            /* [in] */ UINT uiChannelIndex,
            /* [in] */ UINT cbMaskBuffer,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cbMaskBuffer) BYTE *pbMaskBuffer,
            /* [out] */ __RPC__out UINT *pcbActual);
        
        END_INTERFACE
    } IWICPixelFormatInfoVtbl;

    interface IWICPixelFormatInfo
    {
        CONST_VTBL struct IWICPixelFormatInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICPixelFormatInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICPixelFormatInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICPixelFormatInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICPixelFormatInfo_GetComponentType(This,pType)	\
    ( (This)->lpVtbl -> GetComponentType(This,pType) ) 

#define IWICPixelFormatInfo_GetCLSID(This,pclsid)	\
    ( (This)->lpVtbl -> GetCLSID(This,pclsid) ) 

#define IWICPixelFormatInfo_GetSigningStatus(This,pStatus)	\
    ( (This)->lpVtbl -> GetSigningStatus(This,pStatus) ) 

#define IWICPixelFormatInfo_GetAuthor(This,cchAuthor,wzAuthor,pcchActual)	\
    ( (This)->lpVtbl -> GetAuthor(This,cchAuthor,wzAuthor,pcchActual) ) 

#define IWICPixelFormatInfo_GetVendorGUID(This,pguidVendor)	\
    ( (This)->lpVtbl -> GetVendorGUID(This,pguidVendor) ) 

#define IWICPixelFormatInfo_GetVersion(This,cchVersion,wzVersion,pcchActual)	\
    ( (This)->lpVtbl -> GetVersion(This,cchVersion,wzVersion,pcchActual) ) 

#define IWICPixelFormatInfo_GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual)	\
    ( (This)->lpVtbl -> GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual) ) 

#define IWICPixelFormatInfo_GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual)	\
    ( (This)->lpVtbl -> GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual) ) 


#define IWICPixelFormatInfo_GetFormatGUID(This,pFormat)	\
    ( (This)->lpVtbl -> GetFormatGUID(This,pFormat) ) 

#define IWICPixelFormatInfo_GetColorContext(This,ppIColorContext)	\
    ( (This)->lpVtbl -> GetColorContext(This,ppIColorContext) ) 

#define IWICPixelFormatInfo_GetBitsPerPixel(This,puiBitsPerPixel)	\
    ( (This)->lpVtbl -> GetBitsPerPixel(This,puiBitsPerPixel) ) 

#define IWICPixelFormatInfo_GetChannelCount(This,puiChannelCount)	\
    ( (This)->lpVtbl -> GetChannelCount(This,puiChannelCount) ) 

#define IWICPixelFormatInfo_GetChannelMask(This,uiChannelIndex,cbMaskBuffer,pbMaskBuffer,pcbActual)	\
    ( (This)->lpVtbl -> GetChannelMask(This,uiChannelIndex,cbMaskBuffer,pbMaskBuffer,pcbActual) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICPixelFormatInfo_INTERFACE_DEFINED__ */


#ifndef __IWICPixelFormatInfo2_INTERFACE_DEFINED__
#define __IWICPixelFormatInfo2_INTERFACE_DEFINED__

/* interface IWICPixelFormatInfo2 */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICPixelFormatInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A9DB33A2-AF5F-43C7-B679-74F5984B5AA4")
    IWICPixelFormatInfo2 : public IWICPixelFormatInfo
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SupportsTransparency( 
            /* [out] */ __RPC__out BOOL *pfSupportsTransparency) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNumericRepresentation( 
            /* [out] */ __RPC__out WICPixelFormatNumericRepresentation *pNumericRepresentation) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICPixelFormatInfo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICPixelFormatInfo2 * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICPixelFormatInfo2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICPixelFormatInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetComponentType )( 
            __RPC__in IWICPixelFormatInfo2 * This,
            /* [out] */ __RPC__out WICComponentType *pType);
        
        HRESULT ( STDMETHODCALLTYPE *GetCLSID )( 
            __RPC__in IWICPixelFormatInfo2 * This,
            /* [out] */ __RPC__out CLSID *pclsid);
        
        HRESULT ( STDMETHODCALLTYPE *GetSigningStatus )( 
            __RPC__in IWICPixelFormatInfo2 * This,
            /* [out] */ __RPC__out DWORD *pStatus);
        
        HRESULT ( STDMETHODCALLTYPE *GetAuthor )( 
            __RPC__in IWICPixelFormatInfo2 * This,
            /* [in] */ UINT cchAuthor,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchAuthor) WCHAR *wzAuthor,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetVendorGUID )( 
            __RPC__in IWICPixelFormatInfo2 * This,
            /* [out] */ __RPC__out GUID *pguidVendor);
        
        HRESULT ( STDMETHODCALLTYPE *GetVersion )( 
            __RPC__in IWICPixelFormatInfo2 * This,
            /* [in] */ UINT cchVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchVersion) WCHAR *wzVersion,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetSpecVersion )( 
            __RPC__in IWICPixelFormatInfo2 * This,
            /* [in] */ UINT cchSpecVersion,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchSpecVersion) WCHAR *wzSpecVersion,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetFriendlyName )( 
            __RPC__in IWICPixelFormatInfo2 * This,
            /* [in] */ UINT cchFriendlyName,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cchFriendlyName) WCHAR *wzFriendlyName,
            /* [out] */ __RPC__out UINT *pcchActual);
        
        HRESULT ( STDMETHODCALLTYPE *GetFormatGUID )( 
            __RPC__in IWICPixelFormatInfo2 * This,
            /* [out] */ __RPC__out GUID *pFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetColorContext )( 
            __RPC__in IWICPixelFormatInfo2 * This,
            /* [out] */ __RPC__deref_out_opt IWICColorContext **ppIColorContext);
        
        HRESULT ( STDMETHODCALLTYPE *GetBitsPerPixel )( 
            __RPC__in IWICPixelFormatInfo2 * This,
            /* [out] */ __RPC__out UINT *puiBitsPerPixel);
        
        HRESULT ( STDMETHODCALLTYPE *GetChannelCount )( 
            __RPC__in IWICPixelFormatInfo2 * This,
            /* [out] */ __RPC__out UINT *puiChannelCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetChannelMask )( 
            __RPC__in IWICPixelFormatInfo2 * This,
            /* [in] */ UINT uiChannelIndex,
            /* [in] */ UINT cbMaskBuffer,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cbMaskBuffer) BYTE *pbMaskBuffer,
            /* [out] */ __RPC__out UINT *pcbActual);
        
        HRESULT ( STDMETHODCALLTYPE *SupportsTransparency )( 
            __RPC__in IWICPixelFormatInfo2 * This,
            /* [out] */ __RPC__out BOOL *pfSupportsTransparency);
        
        HRESULT ( STDMETHODCALLTYPE *GetNumericRepresentation )( 
            __RPC__in IWICPixelFormatInfo2 * This,
            /* [out] */ __RPC__out WICPixelFormatNumericRepresentation *pNumericRepresentation);
        
        END_INTERFACE
    } IWICPixelFormatInfo2Vtbl;

    interface IWICPixelFormatInfo2
    {
        CONST_VTBL struct IWICPixelFormatInfo2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICPixelFormatInfo2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICPixelFormatInfo2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICPixelFormatInfo2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICPixelFormatInfo2_GetComponentType(This,pType)	\
    ( (This)->lpVtbl -> GetComponentType(This,pType) ) 

#define IWICPixelFormatInfo2_GetCLSID(This,pclsid)	\
    ( (This)->lpVtbl -> GetCLSID(This,pclsid) ) 

#define IWICPixelFormatInfo2_GetSigningStatus(This,pStatus)	\
    ( (This)->lpVtbl -> GetSigningStatus(This,pStatus) ) 

#define IWICPixelFormatInfo2_GetAuthor(This,cchAuthor,wzAuthor,pcchActual)	\
    ( (This)->lpVtbl -> GetAuthor(This,cchAuthor,wzAuthor,pcchActual) ) 

#define IWICPixelFormatInfo2_GetVendorGUID(This,pguidVendor)	\
    ( (This)->lpVtbl -> GetVendorGUID(This,pguidVendor) ) 

#define IWICPixelFormatInfo2_GetVersion(This,cchVersion,wzVersion,pcchActual)	\
    ( (This)->lpVtbl -> GetVersion(This,cchVersion,wzVersion,pcchActual) ) 

#define IWICPixelFormatInfo2_GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual)	\
    ( (This)->lpVtbl -> GetSpecVersion(This,cchSpecVersion,wzSpecVersion,pcchActual) ) 

#define IWICPixelFormatInfo2_GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual)	\
    ( (This)->lpVtbl -> GetFriendlyName(This,cchFriendlyName,wzFriendlyName,pcchActual) ) 


#define IWICPixelFormatInfo2_GetFormatGUID(This,pFormat)	\
    ( (This)->lpVtbl -> GetFormatGUID(This,pFormat) ) 

#define IWICPixelFormatInfo2_GetColorContext(This,ppIColorContext)	\
    ( (This)->lpVtbl -> GetColorContext(This,ppIColorContext) ) 

#define IWICPixelFormatInfo2_GetBitsPerPixel(This,puiBitsPerPixel)	\
    ( (This)->lpVtbl -> GetBitsPerPixel(This,puiBitsPerPixel) ) 

#define IWICPixelFormatInfo2_GetChannelCount(This,puiChannelCount)	\
    ( (This)->lpVtbl -> GetChannelCount(This,puiChannelCount) ) 

#define IWICPixelFormatInfo2_GetChannelMask(This,uiChannelIndex,cbMaskBuffer,pbMaskBuffer,pcbActual)	\
    ( (This)->lpVtbl -> GetChannelMask(This,uiChannelIndex,cbMaskBuffer,pbMaskBuffer,pcbActual) ) 


#define IWICPixelFormatInfo2_SupportsTransparency(This,pfSupportsTransparency)	\
    ( (This)->lpVtbl -> SupportsTransparency(This,pfSupportsTransparency) ) 

#define IWICPixelFormatInfo2_GetNumericRepresentation(This,pNumericRepresentation)	\
    ( (This)->lpVtbl -> GetNumericRepresentation(This,pNumericRepresentation) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICPixelFormatInfo2_INTERFACE_DEFINED__ */


#ifndef __IWICImagingFactory_INTERFACE_DEFINED__
#define __IWICImagingFactory_INTERFACE_DEFINED__

/* interface IWICImagingFactory */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICImagingFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ec5ec8a9-c395-4314-9c77-54d7a935ff70")
    IWICImagingFactory : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateDecoderFromFilename( 
            /* [in] */ __RPC__in LPCWSTR wzFilename,
            /* [unique][in] */ __RPC__in_opt const GUID *pguidVendor,
            /* [in] */ DWORD dwDesiredAccess,
            /* [in] */ WICDecodeOptions metadataOptions,
            /* [retval][out] */ __RPC__deref_out_opt IWICBitmapDecoder **ppIDecoder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateDecoderFromStream( 
            /* [in] */ __RPC__in_opt IStream *pIStream,
            /* [unique][in] */ __RPC__in_opt const GUID *pguidVendor,
            /* [in] */ WICDecodeOptions metadataOptions,
            /* [retval][out] */ __RPC__deref_out_opt IWICBitmapDecoder **ppIDecoder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateDecoderFromFileHandle( 
            /* [in] */ ULONG_PTR hFile,
            /* [unique][in] */ __RPC__in_opt const GUID *pguidVendor,
            /* [in] */ WICDecodeOptions metadataOptions,
            /* [retval][out] */ __RPC__deref_out_opt IWICBitmapDecoder **ppIDecoder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateComponentInfo( 
            /* [in] */ __RPC__in REFCLSID clsidComponent,
            /* [out] */ __RPC__deref_out_opt IWICComponentInfo **ppIInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateDecoder( 
            /* [in] */ __RPC__in REFGUID guidContainerFormat,
            /* [unique][in] */ __RPC__in_opt const GUID *pguidVendor,
            /* [retval][out] */ __RPC__deref_out_opt IWICBitmapDecoder **ppIDecoder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateEncoder( 
            /* [in] */ __RPC__in REFGUID guidContainerFormat,
            /* [unique][in] */ __RPC__in_opt const GUID *pguidVendor,
            /* [retval][out] */ __RPC__deref_out_opt IWICBitmapEncoder **ppIEncoder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreatePalette( 
            /* [out] */ __RPC__deref_out_opt IWICPalette **ppIPalette) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateFormatConverter( 
            /* [out] */ __RPC__deref_out_opt IWICFormatConverter **ppIFormatConverter) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateBitmapScaler( 
            /* [out] */ __RPC__deref_out_opt IWICBitmapScaler **ppIBitmapScaler) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateBitmapClipper( 
            /* [out] */ __RPC__deref_out_opt IWICBitmapClipper **ppIBitmapClipper) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateBitmapFlipRotator( 
            /* [out] */ __RPC__deref_out_opt IWICBitmapFlipRotator **ppIBitmapFlipRotator) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateStream( 
            /* [out] */ __RPC__deref_out_opt IWICStream **ppIWICStream) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateColorContext( 
            /* [out] */ __RPC__deref_out_opt IWICColorContext **ppIWICColorContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateColorTransformer( 
            /* [out] */ __RPC__deref_out_opt IWICColorTransform **ppIWICColorTransform) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateBitmap( 
            /* [in] */ UINT uiWidth,
            /* [in] */ UINT uiHeight,
            /* [in] */ __RPC__in REFWICPixelFormatGUID pixelFormat,
            /* [in] */ WICBitmapCreateCacheOption option,
            /* [out] */ __RPC__deref_out_opt IWICBitmap **ppIBitmap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateBitmapFromSource( 
            /* [in] */ __RPC__in_opt IWICBitmapSource *pIBitmapSource,
            /* [in] */ WICBitmapCreateCacheOption option,
            /* [out] */ __RPC__deref_out_opt IWICBitmap **ppIBitmap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateBitmapFromSourceRect( 
            /* [in] */ __RPC__in_opt IWICBitmapSource *pIBitmapSource,
            /* [in] */ UINT x,
            /* [in] */ UINT y,
            /* [in] */ UINT width,
            /* [in] */ UINT height,
            /* [out] */ __RPC__deref_out_opt IWICBitmap **ppIBitmap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateBitmapFromMemory( 
            /* [in] */ UINT uiWidth,
            /* [in] */ UINT uiHeight,
            /* [in] */ __RPC__in REFWICPixelFormatGUID pixelFormat,
            /* [in] */ UINT cbStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][in] */ __RPC__in_ecount_full(cbBufferSize) BYTE *pbBuffer,
            /* [out] */ __RPC__deref_out_opt IWICBitmap **ppIBitmap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateBitmapFromHBITMAP( 
            /* [in] */ __RPC__in HBITMAP hBitmap,
            /* [unique][in] */ __RPC__in_opt HPALETTE hPalette,
            /* [in] */ WICBitmapAlphaChannelOption options,
            /* [out] */ __RPC__deref_out_opt IWICBitmap **ppIBitmap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateBitmapFromHICON( 
            /* [in] */ __RPC__in HICON hIcon,
            /* [out] */ __RPC__deref_out_opt IWICBitmap **ppIBitmap) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateComponentEnumerator( 
            /* [in] */ DWORD componentTypes,
            /* [in] */ DWORD options,
            /* [out] */ __RPC__deref_out_opt IEnumUnknown **ppIEnumUnknown) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateFastMetadataEncoderFromDecoder( 
            /* [in] */ __RPC__in_opt IWICBitmapDecoder *pIDecoder,
            /* [out] */ __RPC__deref_out_opt IWICFastMetadataEncoder **ppIFastEncoder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateFastMetadataEncoderFromFrameDecode( 
            /* [in] */ __RPC__in_opt IWICBitmapFrameDecode *pIFrameDecoder,
            /* [out] */ __RPC__deref_out_opt IWICFastMetadataEncoder **ppIFastEncoder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateQueryWriter( 
            /* [in] */ __RPC__in REFGUID guidMetadataFormat,
            /* [unique][in] */ __RPC__in_opt const GUID *pguidVendor,
            /* [out] */ __RPC__deref_out_opt IWICMetadataQueryWriter **ppIQueryWriter) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateQueryWriterFromReader( 
            /* [in] */ __RPC__in_opt IWICMetadataQueryReader *pIQueryReader,
            /* [unique][in] */ __RPC__in_opt const GUID *pguidVendor,
            /* [out] */ __RPC__deref_out_opt IWICMetadataQueryWriter **ppIQueryWriter) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICImagingFactoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICImagingFactory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICImagingFactory * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateDecoderFromFilename )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ __RPC__in LPCWSTR wzFilename,
            /* [unique][in] */ __RPC__in_opt const GUID *pguidVendor,
            /* [in] */ DWORD dwDesiredAccess,
            /* [in] */ WICDecodeOptions metadataOptions,
            /* [retval][out] */ __RPC__deref_out_opt IWICBitmapDecoder **ppIDecoder);
        
        HRESULT ( STDMETHODCALLTYPE *CreateDecoderFromStream )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ __RPC__in_opt IStream *pIStream,
            /* [unique][in] */ __RPC__in_opt const GUID *pguidVendor,
            /* [in] */ WICDecodeOptions metadataOptions,
            /* [retval][out] */ __RPC__deref_out_opt IWICBitmapDecoder **ppIDecoder);
        
        HRESULT ( STDMETHODCALLTYPE *CreateDecoderFromFileHandle )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ ULONG_PTR hFile,
            /* [unique][in] */ __RPC__in_opt const GUID *pguidVendor,
            /* [in] */ WICDecodeOptions metadataOptions,
            /* [retval][out] */ __RPC__deref_out_opt IWICBitmapDecoder **ppIDecoder);
        
        HRESULT ( STDMETHODCALLTYPE *CreateComponentInfo )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ __RPC__in REFCLSID clsidComponent,
            /* [out] */ __RPC__deref_out_opt IWICComponentInfo **ppIInfo);
        
        HRESULT ( STDMETHODCALLTYPE *CreateDecoder )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ __RPC__in REFGUID guidContainerFormat,
            /* [unique][in] */ __RPC__in_opt const GUID *pguidVendor,
            /* [retval][out] */ __RPC__deref_out_opt IWICBitmapDecoder **ppIDecoder);
        
        HRESULT ( STDMETHODCALLTYPE *CreateEncoder )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ __RPC__in REFGUID guidContainerFormat,
            /* [unique][in] */ __RPC__in_opt const GUID *pguidVendor,
            /* [retval][out] */ __RPC__deref_out_opt IWICBitmapEncoder **ppIEncoder);
        
        HRESULT ( STDMETHODCALLTYPE *CreatePalette )( 
            __RPC__in IWICImagingFactory * This,
            /* [out] */ __RPC__deref_out_opt IWICPalette **ppIPalette);
        
        HRESULT ( STDMETHODCALLTYPE *CreateFormatConverter )( 
            __RPC__in IWICImagingFactory * This,
            /* [out] */ __RPC__deref_out_opt IWICFormatConverter **ppIFormatConverter);
        
        HRESULT ( STDMETHODCALLTYPE *CreateBitmapScaler )( 
            __RPC__in IWICImagingFactory * This,
            /* [out] */ __RPC__deref_out_opt IWICBitmapScaler **ppIBitmapScaler);
        
        HRESULT ( STDMETHODCALLTYPE *CreateBitmapClipper )( 
            __RPC__in IWICImagingFactory * This,
            /* [out] */ __RPC__deref_out_opt IWICBitmapClipper **ppIBitmapClipper);
        
        HRESULT ( STDMETHODCALLTYPE *CreateBitmapFlipRotator )( 
            __RPC__in IWICImagingFactory * This,
            /* [out] */ __RPC__deref_out_opt IWICBitmapFlipRotator **ppIBitmapFlipRotator);
        
        HRESULT ( STDMETHODCALLTYPE *CreateStream )( 
            __RPC__in IWICImagingFactory * This,
            /* [out] */ __RPC__deref_out_opt IWICStream **ppIWICStream);
        
        HRESULT ( STDMETHODCALLTYPE *CreateColorContext )( 
            __RPC__in IWICImagingFactory * This,
            /* [out] */ __RPC__deref_out_opt IWICColorContext **ppIWICColorContext);
        
        HRESULT ( STDMETHODCALLTYPE *CreateColorTransformer )( 
            __RPC__in IWICImagingFactory * This,
            /* [out] */ __RPC__deref_out_opt IWICColorTransform **ppIWICColorTransform);
        
        HRESULT ( STDMETHODCALLTYPE *CreateBitmap )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ UINT uiWidth,
            /* [in] */ UINT uiHeight,
            /* [in] */ __RPC__in REFWICPixelFormatGUID pixelFormat,
            /* [in] */ WICBitmapCreateCacheOption option,
            /* [out] */ __RPC__deref_out_opt IWICBitmap **ppIBitmap);
        
        HRESULT ( STDMETHODCALLTYPE *CreateBitmapFromSource )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ __RPC__in_opt IWICBitmapSource *pIBitmapSource,
            /* [in] */ WICBitmapCreateCacheOption option,
            /* [out] */ __RPC__deref_out_opt IWICBitmap **ppIBitmap);
        
        HRESULT ( STDMETHODCALLTYPE *CreateBitmapFromSourceRect )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ __RPC__in_opt IWICBitmapSource *pIBitmapSource,
            /* [in] */ UINT x,
            /* [in] */ UINT y,
            /* [in] */ UINT width,
            /* [in] */ UINT height,
            /* [out] */ __RPC__deref_out_opt IWICBitmap **ppIBitmap);
        
        HRESULT ( STDMETHODCALLTYPE *CreateBitmapFromMemory )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ UINT uiWidth,
            /* [in] */ UINT uiHeight,
            /* [in] */ __RPC__in REFWICPixelFormatGUID pixelFormat,
            /* [in] */ UINT cbStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][in] */ __RPC__in_ecount_full(cbBufferSize) BYTE *pbBuffer,
            /* [out] */ __RPC__deref_out_opt IWICBitmap **ppIBitmap);
        
        HRESULT ( STDMETHODCALLTYPE *CreateBitmapFromHBITMAP )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ __RPC__in HBITMAP hBitmap,
            /* [unique][in] */ __RPC__in_opt HPALETTE hPalette,
            /* [in] */ WICBitmapAlphaChannelOption options,
            /* [out] */ __RPC__deref_out_opt IWICBitmap **ppIBitmap);
        
        HRESULT ( STDMETHODCALLTYPE *CreateBitmapFromHICON )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ __RPC__in HICON hIcon,
            /* [out] */ __RPC__deref_out_opt IWICBitmap **ppIBitmap);
        
        HRESULT ( STDMETHODCALLTYPE *CreateComponentEnumerator )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ DWORD componentTypes,
            /* [in] */ DWORD options,
            /* [out] */ __RPC__deref_out_opt IEnumUnknown **ppIEnumUnknown);
        
        HRESULT ( STDMETHODCALLTYPE *CreateFastMetadataEncoderFromDecoder )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ __RPC__in_opt IWICBitmapDecoder *pIDecoder,
            /* [out] */ __RPC__deref_out_opt IWICFastMetadataEncoder **ppIFastEncoder);
        
        HRESULT ( STDMETHODCALLTYPE *CreateFastMetadataEncoderFromFrameDecode )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ __RPC__in_opt IWICBitmapFrameDecode *pIFrameDecoder,
            /* [out] */ __RPC__deref_out_opt IWICFastMetadataEncoder **ppIFastEncoder);
        
        HRESULT ( STDMETHODCALLTYPE *CreateQueryWriter )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ __RPC__in REFGUID guidMetadataFormat,
            /* [unique][in] */ __RPC__in_opt const GUID *pguidVendor,
            /* [out] */ __RPC__deref_out_opt IWICMetadataQueryWriter **ppIQueryWriter);
        
        HRESULT ( STDMETHODCALLTYPE *CreateQueryWriterFromReader )( 
            __RPC__in IWICImagingFactory * This,
            /* [in] */ __RPC__in_opt IWICMetadataQueryReader *pIQueryReader,
            /* [unique][in] */ __RPC__in_opt const GUID *pguidVendor,
            /* [out] */ __RPC__deref_out_opt IWICMetadataQueryWriter **ppIQueryWriter);
        
        END_INTERFACE
    } IWICImagingFactoryVtbl;

    interface IWICImagingFactory
    {
        CONST_VTBL struct IWICImagingFactoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICImagingFactory_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICImagingFactory_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICImagingFactory_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICImagingFactory_CreateDecoderFromFilename(This,wzFilename,pguidVendor,dwDesiredAccess,metadataOptions,ppIDecoder)	\
    ( (This)->lpVtbl -> CreateDecoderFromFilename(This,wzFilename,pguidVendor,dwDesiredAccess,metadataOptions,ppIDecoder) ) 

#define IWICImagingFactory_CreateDecoderFromStream(This,pIStream,pguidVendor,metadataOptions,ppIDecoder)	\
    ( (This)->lpVtbl -> CreateDecoderFromStream(This,pIStream,pguidVendor,metadataOptions,ppIDecoder) ) 

#define IWICImagingFactory_CreateDecoderFromFileHandle(This,hFile,pguidVendor,metadataOptions,ppIDecoder)	\
    ( (This)->lpVtbl -> CreateDecoderFromFileHandle(This,hFile,pguidVendor,metadataOptions,ppIDecoder) ) 

#define IWICImagingFactory_CreateComponentInfo(This,clsidComponent,ppIInfo)	\
    ( (This)->lpVtbl -> CreateComponentInfo(This,clsidComponent,ppIInfo) ) 

#define IWICImagingFactory_CreateDecoder(This,guidContainerFormat,pguidVendor,ppIDecoder)	\
    ( (This)->lpVtbl -> CreateDecoder(This,guidContainerFormat,pguidVendor,ppIDecoder) ) 

#define IWICImagingFactory_CreateEncoder(This,guidContainerFormat,pguidVendor,ppIEncoder)	\
    ( (This)->lpVtbl -> CreateEncoder(This,guidContainerFormat,pguidVendor,ppIEncoder) ) 

#define IWICImagingFactory_CreatePalette(This,ppIPalette)	\
    ( (This)->lpVtbl -> CreatePalette(This,ppIPalette) ) 

#define IWICImagingFactory_CreateFormatConverter(This,ppIFormatConverter)	\
    ( (This)->lpVtbl -> CreateFormatConverter(This,ppIFormatConverter) ) 

#define IWICImagingFactory_CreateBitmapScaler(This,ppIBitmapScaler)	\
    ( (This)->lpVtbl -> CreateBitmapScaler(This,ppIBitmapScaler) ) 

#define IWICImagingFactory_CreateBitmapClipper(This,ppIBitmapClipper)	\
    ( (This)->lpVtbl -> CreateBitmapClipper(This,ppIBitmapClipper) ) 

#define IWICImagingFactory_CreateBitmapFlipRotator(This,ppIBitmapFlipRotator)	\
    ( (This)->lpVtbl -> CreateBitmapFlipRotator(This,ppIBitmapFlipRotator) ) 

#define IWICImagingFactory_CreateStream(This,ppIWICStream)	\
    ( (This)->lpVtbl -> CreateStream(This,ppIWICStream) ) 

#define IWICImagingFactory_CreateColorContext(This,ppIWICColorContext)	\
    ( (This)->lpVtbl -> CreateColorContext(This,ppIWICColorContext) ) 

#define IWICImagingFactory_CreateColorTransformer(This,ppIWICColorTransform)	\
    ( (This)->lpVtbl -> CreateColorTransformer(This,ppIWICColorTransform) ) 

#define IWICImagingFactory_CreateBitmap(This,uiWidth,uiHeight,pixelFormat,option,ppIBitmap)	\
    ( (This)->lpVtbl -> CreateBitmap(This,uiWidth,uiHeight,pixelFormat,option,ppIBitmap) ) 

#define IWICImagingFactory_CreateBitmapFromSource(This,pIBitmapSource,option,ppIBitmap)	\
    ( (This)->lpVtbl -> CreateBitmapFromSource(This,pIBitmapSource,option,ppIBitmap) ) 

#define IWICImagingFactory_CreateBitmapFromSourceRect(This,pIBitmapSource,x,y,width,height,ppIBitmap)	\
    ( (This)->lpVtbl -> CreateBitmapFromSourceRect(This,pIBitmapSource,x,y,width,height,ppIBitmap) ) 

#define IWICImagingFactory_CreateBitmapFromMemory(This,uiWidth,uiHeight,pixelFormat,cbStride,cbBufferSize,pbBuffer,ppIBitmap)	\
    ( (This)->lpVtbl -> CreateBitmapFromMemory(This,uiWidth,uiHeight,pixelFormat,cbStride,cbBufferSize,pbBuffer,ppIBitmap) ) 

#define IWICImagingFactory_CreateBitmapFromHBITMAP(This,hBitmap,hPalette,options,ppIBitmap)	\
    ( (This)->lpVtbl -> CreateBitmapFromHBITMAP(This,hBitmap,hPalette,options,ppIBitmap) ) 

#define IWICImagingFactory_CreateBitmapFromHICON(This,hIcon,ppIBitmap)	\
    ( (This)->lpVtbl -> CreateBitmapFromHICON(This,hIcon,ppIBitmap) ) 

#define IWICImagingFactory_CreateComponentEnumerator(This,componentTypes,options,ppIEnumUnknown)	\
    ( (This)->lpVtbl -> CreateComponentEnumerator(This,componentTypes,options,ppIEnumUnknown) ) 

#define IWICImagingFactory_CreateFastMetadataEncoderFromDecoder(This,pIDecoder,ppIFastEncoder)	\
    ( (This)->lpVtbl -> CreateFastMetadataEncoderFromDecoder(This,pIDecoder,ppIFastEncoder) ) 

#define IWICImagingFactory_CreateFastMetadataEncoderFromFrameDecode(This,pIFrameDecoder,ppIFastEncoder)	\
    ( (This)->lpVtbl -> CreateFastMetadataEncoderFromFrameDecode(This,pIFrameDecoder,ppIFastEncoder) ) 

#define IWICImagingFactory_CreateQueryWriter(This,guidMetadataFormat,pguidVendor,ppIQueryWriter)	\
    ( (This)->lpVtbl -> CreateQueryWriter(This,guidMetadataFormat,pguidVendor,ppIQueryWriter) ) 

#define IWICImagingFactory_CreateQueryWriterFromReader(This,pIQueryReader,pguidVendor,ppIQueryWriter)	\
    ( (This)->lpVtbl -> CreateQueryWriterFromReader(This,pIQueryReader,pguidVendor,ppIQueryWriter) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICImagingFactory_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_wincodec_0000_0031 */
/* [local] */ 

HRESULT WINAPI WICConvertBitmapSource(
     __in REFWICPixelFormatGUID dstFormat, // Destination pixel format
     __in_ecount(1) IWICBitmapSource  *pISrc,    // Source bitmap
     __deref_out_ecount(1) IWICBitmapSource **ppIDst   // Destination bitmap, a copy or addrefed source
     );
HRESULT WINAPI WICCreateBitmapFromSection(
     __in UINT width,
     __in UINT height,
     __in REFWICPixelFormatGUID pixelFormat,
     __in HANDLE hSection,
     __in UINT stride,
     __in UINT offset,
     __deref_out_ecount(1) IWICBitmap **ppIBitmap
     );
HRESULT WINAPI WICCreateBitmapFromSectionEx(
     __in UINT width,
     __in UINT height,
     __in REFWICPixelFormatGUID pixelFormat,
     __in HANDLE hSection,
     __in UINT stride,
     __in UINT offset,
     __in WICSectionAccessLevel desiredAccessLevel,
     __deref_out_ecount(1) IWICBitmap **ppIBitmap
     );
HRESULT WINAPI WICMapGuidToShortName(
    __in_ecount(1) REFGUID guid,
    __in UINT cchName,
    __inout_ecount_opt(cchName) WCHAR *wzName,
    __out_ecount(1) UINT *pcchActual
   );
HRESULT WINAPI WICMapShortNameToGuid(
    __in_ecount(1) const WCHAR *wzName,
    __out_ecount(1) GUID *pguid
   );
HRESULT WINAPI WICMapSchemaToName(
    __in_ecount(1) REFGUID guidMetadataFormat,
    __in_ecount(1) LPWSTR pwzSchema,
    __in UINT cchName,
    __inout_ecount_opt(cchName) WCHAR *wzName,
    __out_ecount(1) UINT *pcchActual
    );
#define FACILITY_WINCODEC_ERR 0x898
#define WINCODEC_ERR_BASE 0x2000
#define MAKE_WINCODECHR(sev, code) MAKE_HRESULT(sev, FACILITY_WINCODEC_ERR, (WINCODEC_ERR_BASE + code))
#define MAKE_WINCODECHR_ERR(code) MAKE_WINCODECHR(1, code)
#define WINCODEC_ERR_GENERIC_ERROR                    E_FAIL
#define WINCODEC_ERR_INVALIDPARAMETER                 E_INVALIDARG
#define WINCODEC_ERR_OUTOFMEMORY                      E_OUTOFMEMORY
#define WINCODEC_ERR_NOTIMPLEMENTED                   E_NOTIMPL
#define WINCODEC_ERR_ABORTED                          E_ABORT
#define WINCODEC_ERR_ACCESSDENIED                     E_ACCESSDENIED
#define WINCODEC_ERR_VALUEOVERFLOW                    INTSAFE_E_ARITHMETIC_OVERFLOW
#define WINCODEC_ERR_WRONGSTATE                       MAKE_WINCODECHR_ERR(0xf04)
#define WINCODEC_ERR_VALUEOUTOFRANGE                  MAKE_WINCODECHR_ERR(0xf05)
#define WINCODEC_ERR_UNKNOWNIMAGEFORMAT               MAKE_WINCODECHR_ERR(0xf07)
#define WINCODEC_ERR_UNSUPPORTEDVERSION               MAKE_WINCODECHR_ERR(0xf0B)
#define WINCODEC_ERR_NOTINITIALIZED                   MAKE_WINCODECHR_ERR(0xf0C)
#define WINCODEC_ERR_ALREADYLOCKED                    MAKE_WINCODECHR_ERR(0xf0D)
#define WINCODEC_ERR_PROPERTYNOTFOUND                 MAKE_WINCODECHR_ERR(0xf40)
#define WINCODEC_ERR_PROPERTYNOTSUPPORTED             MAKE_WINCODECHR_ERR(0xf41)
#define WINCODEC_ERR_PROPERTYSIZE                     MAKE_WINCODECHR_ERR(0xf42)
#define WINCODEC_ERR_CODECPRESENT                     MAKE_WINCODECHR_ERR(0xf43)
#define WINCODEC_ERR_CODECNOTHUMBNAIL                 MAKE_WINCODECHR_ERR(0xf44)
#define WINCODEC_ERR_PALETTEUNAVAILABLE               MAKE_WINCODECHR_ERR(0xf45)
#define WINCODEC_ERR_CODECTOOMANYSCANLINES            MAKE_WINCODECHR_ERR(0xf46)
#define WINCODEC_ERR_INTERNALERROR                    MAKE_WINCODECHR_ERR(0xf48)
#define WINCODEC_ERR_SOURCERECTDOESNOTMATCHDIMENSIONS MAKE_WINCODECHR_ERR(0xf49)
#define WINCODEC_ERR_COMPONENTNOTFOUND                MAKE_WINCODECHR_ERR(0xf50)
#define WINCODEC_ERR_IMAGESIZEOUTOFRANGE              MAKE_WINCODECHR_ERR(0xf51)
#define WINCODEC_ERR_TOOMUCHMETADATA                  MAKE_WINCODECHR_ERR(0xf52)
#define WINCODEC_ERR_BADIMAGE                         MAKE_WINCODECHR_ERR(0xf60)
#define WINCODEC_ERR_BADHEADER                        MAKE_WINCODECHR_ERR(0xf61)
#define WINCODEC_ERR_FRAMEMISSING                     MAKE_WINCODECHR_ERR(0xf62)
#define WINCODEC_ERR_BADMETADATAHEADER                MAKE_WINCODECHR_ERR(0xf63)
#define WINCODEC_ERR_BADSTREAMDATA                    MAKE_WINCODECHR_ERR(0xf70)
#define WINCODEC_ERR_STREAMWRITE                      MAKE_WINCODECHR_ERR(0xf71)
#define WINCODEC_ERR_STREAMREAD                       MAKE_WINCODECHR_ERR(0xf72)
#define WINCODEC_ERR_STREAMNOTAVAILABLE               MAKE_WINCODECHR_ERR(0xf73)
#define WINCODEC_ERR_UNSUPPORTEDPIXELFORMAT           MAKE_WINCODECHR_ERR(0xf80)
#define WINCODEC_ERR_UNSUPPORTEDOPERATION             MAKE_WINCODECHR_ERR(0xf81)
#define WINCODEC_ERR_INVALIDREGISTRATION              MAKE_WINCODECHR_ERR(0xf8A)
#define WINCODEC_ERR_COMPONENTINITIALIZEFAILURE       MAKE_WINCODECHR_ERR(0xf8B)
#define WINCODEC_ERR_INSUFFICIENTBUFFER               MAKE_WINCODECHR_ERR(0xf8C)
#define WINCODEC_ERR_DUPLICATEMETADATAPRESENT         MAKE_WINCODECHR_ERR(0xf8D)
#define WINCODEC_ERR_PROPERTYUNEXPECTEDTYPE           MAKE_WINCODECHR_ERR(0xf8E)
#define WINCODEC_ERR_UNEXPECTEDSIZE                   MAKE_WINCODECHR_ERR(0xf8F)
#define WINCODEC_ERR_INVALIDQUERYREQUEST              MAKE_WINCODECHR_ERR(0xf90)
#define WINCODEC_ERR_UNEXPECTEDMETADATATYPE           MAKE_WINCODECHR_ERR(0xf91)
#define WINCODEC_ERR_REQUESTONLYVALIDATMETADATAROOT   MAKE_WINCODECHR_ERR(0xf92)
#define WINCODEC_ERR_INVALIDQUERYCHARACTER            MAKE_WINCODECHR_ERR(0xf93)
#define WINCODEC_ERR_WIN32ERROR                       MAKE_WINCODECHR_ERR(0xf94)
#define WINCODEC_ERR_INVALIDPROGRESSIVELEVEL          MAKE_WINCODECHR_ERR(0xf95)
typedef /* [public] */ 
enum WICTiffCompressionOption
    {	WICTiffCompressionDontCare	= 0,
	WICTiffCompressionNone	= 0x1,
	WICTiffCompressionCCITT3	= 0x2,
	WICTiffCompressionCCITT4	= 0x3,
	WICTiffCompressionLZW	= 0x4,
	WICTiffCompressionRLE	= 0x5,
	WICTiffCompressionZIP	= 0x6,
	WICTiffCompressionLZWHDifferencing	= 0x7,
	WICTIFFCOMPRESSIONOPTION_FORCE_DWORD	= 0x7fffffff
    } 	WICTiffCompressionOption;

typedef /* [public] */ 
enum WICJpegYCrCbSubsamplingOption
    {	WICJpegYCrCbSubsamplingDefault	= 0,
	WICJpegYCrCbSubsampling420	= 0x1,
	WICJpegYCrCbSubsampling422	= 0x2,
	WICJpegYCrCbSubsampling444	= 0x3,
	WICJPEGYCRCBSUBSAMPLING_FORCE_DWORD	= 0x7fffffff
    } 	WICJpegYCrCbSubsamplingOption;

typedef /* [public] */ 
enum WICPngFilterOption
    {	WICPngFilterUnspecified	= 0,
	WICPngFilterNone	= 0x1,
	WICPngFilterSub	= 0x2,
	WICPngFilterUp	= 0x3,
	WICPngFilterAverage	= 0x4,
	WICPngFilterPaeth	= 0x5,
	WICPngFilterAdaptive	= 0x6,
	WICPNGFILTEROPTION_FORCE_DWORD	= 0x7fffffff
    } 	WICPngFilterOption;

typedef /* [public] */ 
enum WICNamedWhitePoint
    {	WICWhitePointDefault	= 0x1,
	WICWhitePointDaylight	= 0x2,
	WICWhitePointCloudy	= 0x4,
	WICWhitePointShade	= 0x8,
	WICWhitePointTungsten	= 0x10,
	WICWhitePointFluorescent	= 0x20,
	WICWhitePointFlash	= 0x40,
	WICWhitePointUnderwater	= 0x80,
	WICWhitePointCustom	= 0x100,
	WICWhitePointAutoWhiteBalance	= 0x200,
	WICWhitePointAsShot	= WICWhitePointDefault,
	WICNAMEDWHITEPOINT_FORCE_DWORD	= 0x7fffffff
    } 	WICNamedWhitePoint;

typedef /* [public] */ 
enum WICRawCapabilities
    {	WICRawCapabilityNotSupported	= 0,
	WICRawCapabilityGetSupported	= 0x1,
	WICRawCapabilityFullySupported	= 0x2,
	WICRAWCAPABILITIES_FORCE_DWORD	= 0x7fffffff
    } 	WICRawCapabilities;

typedef /* [public] */ 
enum WICRawRotationCapabilities
    {	WICRawRotationCapabilityNotSupported	= 0,
	WICRawRotationCapabilityGetSupported	= 0x1,
	WICRawRotationCapabilityNinetyDegreesSupported	= 0x2,
	WICRawRotationCapabilityFullySupported	= 0x3,
	WICRAWROTATIONCAPABILITIES_FORCE_DWORD	= 0x7fffffff
    } 	WICRawRotationCapabilities;

typedef /* [public] */ struct WICRawCapabilitiesInfo
    {
    UINT cbSize;
    UINT CodecMajorVersion;
    UINT CodecMinorVersion;
    WICRawCapabilities ExposureCompensationSupport;
    WICRawCapabilities ContrastSupport;
    WICRawCapabilities RGBWhitePointSupport;
    WICRawCapabilities NamedWhitePointSupport;
    UINT NamedWhitePointSupportMask;
    WICRawCapabilities KelvinWhitePointSupport;
    WICRawCapabilities GammaSupport;
    WICRawCapabilities TintSupport;
    WICRawCapabilities SaturationSupport;
    WICRawCapabilities SharpnessSupport;
    WICRawCapabilities NoiseReductionSupport;
    WICRawCapabilities DestinationColorProfileSupport;
    WICRawCapabilities ToneCurveSupport;
    WICRawRotationCapabilities RotationSupport;
    WICRawCapabilities RenderModeSupport;
    } 	WICRawCapabilitiesInfo;

typedef /* [public] */ 
enum WICRawParameterSet
    {	WICAsShotParameterSet	= 0x1,
	WICUserAdjustedParameterSet	= 0x2,
	WICAutoAdjustedParameterSet	= 0x3,
	WICRAWPARAMETERSET_FORCE_DWORD	= 0x7fffffff
    } 	WICRawParameterSet;

typedef /* [public] */ 
enum WICRawRenderMode
    {	WICRawRenderModeDraft	= 0x1,
	WICRawRenderModeNormal	= 0x2,
	WICRawRenderModeBestQuality	= 0x3,
	WICRAWRENDERMODE_FORCE_DWORD	= 0x7fffffff
    } 	WICRawRenderMode;

typedef /* [public] */ struct WICRawToneCurvePoint
    {
    double Input;
    double Output;
    } 	WICRawToneCurvePoint;

typedef /* [public] */ struct WICRawToneCurve
    {
    UINT cPoints;
    WICRawToneCurvePoint aPoints[ 1 ];
    } 	WICRawToneCurve;

#define WICRawChangeNotification_ExposureCompensation       0x00000001
#define WICRawChangeNotification_NamedWhitePoint            0x00000002
#define WICRawChangeNotification_KelvinWhitePoint           0x00000004
#define WICRawChangeNotification_RGBWhitePoint              0x00000008
#define WICRawChangeNotification_Contrast                   0x00000010
#define WICRawChangeNotification_Gamma                      0x00000020
#define WICRawChangeNotification_Sharpness                  0x00000040
#define WICRawChangeNotification_Saturation                 0x00000080
#define WICRawChangeNotification_Tint                       0x00000100
#define WICRawChangeNotification_NoiseReduction             0x00000200
#define WICRawChangeNotification_DestinationColorContext    0x00000400
#define WICRawChangeNotification_ToneCurve                  0x00000800
#define WICRawChangeNotification_Rotation                   0x00001000
#define WICRawChangeNotification_RenderMode                 0x00002000


extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0031_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wincodec_0000_0031_v0_0_s_ifspec;

#ifndef __IWICDevelopRawNotificationCallback_INTERFACE_DEFINED__
#define __IWICDevelopRawNotificationCallback_INTERFACE_DEFINED__

/* interface IWICDevelopRawNotificationCallback */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICDevelopRawNotificationCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("95c75a6e-3e8c-4ec2-85a8-aebcc551e59b")
    IWICDevelopRawNotificationCallback : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Notify( 
            /* [in] */ UINT NotificationMask) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICDevelopRawNotificationCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICDevelopRawNotificationCallback * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICDevelopRawNotificationCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICDevelopRawNotificationCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *Notify )( 
            __RPC__in IWICDevelopRawNotificationCallback * This,
            /* [in] */ UINT NotificationMask);
        
        END_INTERFACE
    } IWICDevelopRawNotificationCallbackVtbl;

    interface IWICDevelopRawNotificationCallback
    {
        CONST_VTBL struct IWICDevelopRawNotificationCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICDevelopRawNotificationCallback_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICDevelopRawNotificationCallback_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICDevelopRawNotificationCallback_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICDevelopRawNotificationCallback_Notify(This,NotificationMask)	\
    ( (This)->lpVtbl -> Notify(This,NotificationMask) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWICDevelopRawNotificationCallback_INTERFACE_DEFINED__ */


#ifndef __IWICDevelopRaw_INTERFACE_DEFINED__
#define __IWICDevelopRaw_INTERFACE_DEFINED__

/* interface IWICDevelopRaw */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWICDevelopRaw;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("fbec5e44-f7be-4b65-b7f8-c0c81fef026d")
    IWICDevelopRaw : public IWICBitmapFrameDecode
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE QueryRawCapabilitiesInfo( 
            /* [out][in] */ WICRawCapabilitiesInfo *pInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LoadParameterSet( 
            /* [in] */ WICRawParameterSet ParameterSet) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentParameterSet( 
            /* [out] */ __RPC__deref_out_opt IPropertyBag2 **ppCurrentParameterSet) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetExposureCompensation( 
            /* [in] */ double ev) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetExposureCompensation( 
            /* [out] */ __RPC__out double *pEV) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetWhitePointRGB( 
            /* [in] */ UINT Red,
            /* [in] */ UINT Green,
            /* [in] */ UINT Blue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetWhitePointRGB( 
            /* [out] */ __RPC__out UINT *pRed,
            /* [out] */ __RPC__out UINT *pGreen,
            /* [out] */ __RPC__out UINT *pBlue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetNamedWhitePoint( 
            /* [in] */ WICNamedWhitePoint WhitePoint) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNamedWhitePoint( 
            /* [out] */ __RPC__out WICNamedWhitePoint *pWhitePoint) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetWhitePointKelvin( 
            /* [in] */ UINT WhitePointKelvin) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetWhitePointKelvin( 
            /* [out] */ __RPC__out UINT *pWhitePointKelvin) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetKelvinRangeInfo( 
            /* [out] */ __RPC__out UINT *pMinKelvinTemp,
            /* [out] */ __RPC__out UINT *pMaxKelvinTemp,
            /* [out] */ __RPC__out UINT *pKelvinTempStepValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetContrast( 
            /* [in] */ double Contrast) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetContrast( 
            /* [out] */ __RPC__out double *pContrast) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetGamma( 
            /* [in] */ double Gamma) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetGamma( 
            /* [out] */ __RPC__out double *pGamma) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSharpness( 
            /* [in] */ double Sharpness) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSharpness( 
            /* [out] */ __RPC__out double *pSharpness) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSaturation( 
            /* [in] */ double Saturation) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSaturation( 
            /* [out] */ __RPC__out double *pSaturation) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTint( 
            /* [in] */ double Tint) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTint( 
            /* [out] */ __RPC__out double *pTint) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetNoiseReduction( 
            /* [in] */ double NoiseReduction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNoiseReduction( 
            /* [out] */ __RPC__out double *pNoiseReduction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDestinationColorContext( 
            /* [unique][in] */ __RPC__in_opt IWICColorContext *pColorContext) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE SetToneCurve( 
            /* [in] */ UINT cbToneCurveSize,
            /* [annotation][in] */ 
            __in_bcount(cbToneCurveSize)  const WICRawToneCurve *pToneCurve) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetToneCurve( 
            /* [in] */ UINT cbToneCurveBufferSize,
            /* [annotation][unique][out] */ 
            __out_bcount_part_opt(cbToneCurveBufferSize, *pcbActualToneCurveBufferSize)  WICRawToneCurve *pToneCurve,
            /* [annotation][unique][out] */ 
            __inout_opt  UINT *pcbActualToneCurveBufferSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetRotation( 
            /* [in] */ double Rotation) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRotation( 
            /* [out] */ __RPC__out double *pRotation) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetRenderMode( 
            /* [in] */ WICRawRenderMode RenderMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRenderMode( 
            /* [out] */ __RPC__out WICRawRenderMode *pRenderMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetNotificationCallback( 
            /* [unique][in] */ __RPC__in_opt IWICDevelopRawNotificationCallback *pCallback) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWICDevelopRawVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __RPC__in IWICDevelopRaw * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __RPC__in IWICDevelopRaw * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __RPC__in IWICDevelopRaw * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__out UINT *puiWidth,
            /* [out] */ __RPC__out UINT *puiHeight);
        
        HRESULT ( STDMETHODCALLTYPE *GetPixelFormat )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__out WICPixelFormatGUID *pPixelFormat);
        
        HRESULT ( STDMETHODCALLTYPE *GetResolution )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__out double *pDpiX,
            /* [out] */ __RPC__out double *pDpiY);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPalette )( 
            __RPC__in IWICDevelopRaw * This,
            /* [in] */ __RPC__in_opt IWICPalette *pIPalette);
        
        HRESULT ( STDMETHODCALLTYPE *CopyPixels )( 
            __RPC__in IWICDevelopRaw * This,
            /* [unique][in] */ __RPC__in_opt const WICRect *prc,
            /* [in] */ UINT cbStride,
            /* [in] */ UINT cbBufferSize,
            /* [size_is][out] */ __RPC__out_ecount_full(cbBufferSize) BYTE *pbBuffer);
        
        HRESULT ( STDMETHODCALLTYPE *GetMetadataQueryReader )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__deref_out_opt IWICMetadataQueryReader **ppIMetadataQueryReader);
        
        HRESULT ( STDMETHODCALLTYPE *GetColorContexts )( 
            __RPC__in IWICDevelopRaw * This,
            /* [in] */ UINT cCount,
            /* [size_is][unique][out][in] */ __RPC__inout_ecount_full_opt(cCount) IWICColorContext **ppIColorContexts,
            /* [out] */ __RPC__out UINT *pcActualCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetThumbnail )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__deref_out_opt IWICBitmapSource **ppIThumbnail);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *QueryRawCapabilitiesInfo )( 
            IWICDevelopRaw * This,
            /* [out][in] */ WICRawCapabilitiesInfo *pInfo);
        
        HRESULT ( STDMETHODCALLTYPE *LoadParameterSet )( 
            __RPC__in IWICDevelopRaw * This,
            /* [in] */ WICRawParameterSet ParameterSet);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentParameterSet )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__deref_out_opt IPropertyBag2 **ppCurrentParameterSet);
        
        HRESULT ( STDMETHODCALLTYPE *SetExposureCompensation )( 
            __RPC__in IWICDevelopRaw * This,
            /* [in] */ double ev);
        
        HRESULT ( STDMETHODCALLTYPE *GetExposureCompensation )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__out double *pEV);
        
        HRESULT ( STDMETHODCALLTYPE *SetWhitePointRGB )( 
            __RPC__in IWICDevelopRaw * This,
            /* [in] */ UINT Red,
            /* [in] */ UINT Green,
            /* [in] */ UINT Blue);
        
        HRESULT ( STDMETHODCALLTYPE *GetWhitePointRGB )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__out UINT *pRed,
            /* [out] */ __RPC__out UINT *pGreen,
            /* [out] */ __RPC__out UINT *pBlue);
        
        HRESULT ( STDMETHODCALLTYPE *SetNamedWhitePoint )( 
            __RPC__in IWICDevelopRaw * This,
            /* [in] */ WICNamedWhitePoint WhitePoint);
        
        HRESULT ( STDMETHODCALLTYPE *GetNamedWhitePoint )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__out WICNamedWhitePoint *pWhitePoint);
        
        HRESULT ( STDMETHODCALLTYPE *SetWhitePointKelvin )( 
            __RPC__in IWICDevelopRaw * This,
            /* [in] */ UINT WhitePointKelvin);
        
        HRESULT ( STDMETHODCALLTYPE *GetWhitePointKelvin )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__out UINT *pWhitePointKelvin);
        
        HRESULT ( STDMETHODCALLTYPE *GetKelvinRangeInfo )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__out UINT *pMinKelvinTemp,
            /* [out] */ __RPC__out UINT *pMaxKelvinTemp,
            /* [out] */ __RPC__out UINT *pKelvinTempStepValue);
        
        HRESULT ( STDMETHODCALLTYPE *SetContrast )( 
            __RPC__in IWICDevelopRaw * This,
            /* [in] */ double Contrast);
        
        HRESULT ( STDMETHODCALLTYPE *GetContrast )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__out double *pContrast);
        
        HRESULT ( STDMETHODCALLTYPE *SetGamma )( 
            __RPC__in IWICDevelopRaw * This,
            /* [in] */ double Gamma);
        
        HRESULT ( STDMETHODCALLTYPE *GetGamma )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__out double *pGamma);
        
        HRESULT ( STDMETHODCALLTYPE *SetSharpness )( 
            __RPC__in IWICDevelopRaw * This,
            /* [in] */ double Sharpness);
        
        HRESULT ( STDMETHODCALLTYPE *GetSharpness )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__out double *pSharpness);
        
        HRESULT ( STDMETHODCALLTYPE *SetSaturation )( 
            __RPC__in IWICDevelopRaw * This,
            /* [in] */ double Saturation);
        
        HRESULT ( STDMETHODCALLTYPE *GetSaturation )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__out double *pSaturation);
        
        HRESULT ( STDMETHODCALLTYPE *SetTint )( 
            __RPC__in IWICDevelopRaw * This,
            /* [in] */ double Tint);
        
        HRESULT ( STDMETHODCALLTYPE *GetTint )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__out double *pTint);
        
        HRESULT ( STDMETHODCALLTYPE *SetNoiseReduction )( 
            __RPC__in IWICDevelopRaw * This,
            /* [in] */ double NoiseReduction);
        
        HRESULT ( STDMETHODCALLTYPE *GetNoiseReduction )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__out double *pNoiseReduction);
        
        HRESULT ( STDMETHODCALLTYPE *SetDestinationColorContext )( 
            __RPC__in IWICDevelopRaw * This,
            /* [unique][in] */ __RPC__in_opt IWICColorContext *pColorContext);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *SetToneCurve )( 
            IWICDevelopRaw * This,
            /* [in] */ UINT cbToneCurveSize,
            /* [annotation][in] */ 
            __in_bcount(cbToneCurveSize)  const WICRawToneCurve *pToneCurve);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetToneCurve )( 
            IWICDevelopRaw * This,
            /* [in] */ UINT cbToneCurveBufferSize,
            /* [annotation][unique][out] */ 
            __out_bcount_part_opt(cbToneCurveBufferSize, *pcbActualToneCurveBufferSize)  WICRawToneCurve *pToneCurve,
            /* [annotation][unique][out] */ 
            __inout_opt  UINT *pcbActualToneCurveBufferSize);
        
        HRESULT ( STDMETHODCALLTYPE *SetRotation )( 
            __RPC__in IWICDevelopRaw * This,
            /* [in] */ double Rotation);
        
        HRESULT ( STDMETHODCALLTYPE *GetRotation )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__out double *pRotation);
        
        HRESULT ( STDMETHODCALLTYPE *SetRenderMode )( 
            __RPC__in IWICDevelopRaw * This,
            /* [in] */ WICRawRenderMode RenderMode);
        
        HRESULT ( STDMETHODCALLTYPE *GetRenderMode )( 
            __RPC__in IWICDevelopRaw * This,
            /* [out] */ __RPC__out WICRawRenderMode *pRenderMode);
        
        HRESULT ( STDMETHODCALLTYPE *SetNotificationCallback )( 
            __RPC__in IWICDevelopRaw * This,
            /* [unique][in] */ __RPC__in_opt IWICDevelopRawNotificationCallback *pCallback);
        
        END_INTERFACE
    } IWICDevelopRawVtbl;

    interface IWICDevelopRaw
    {
        CONST_VTBL struct IWICDevelopRawVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWICDevelopRaw_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWICDevelopRaw_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWICDevelopRaw_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWICDevelopRaw_GetSize(This,puiWidth,puiHeight)	\
    ( (This)->lpVtbl -> GetSize(This,puiWidth,puiHeight) ) 

#define IWICDevelopRaw_GetPixelFormat(This,pPixelFormat)	\
    ( (This)->lpVtbl -> GetPixelFormat(This,pPixelFormat) ) 

#define IWICDevelopRaw_GetResolution(This,pDpiX,pDpiY)	\
    ( (This)->lpVtbl -> GetResolution(This,pDpiX,pDpiY) ) 

#define IWICDevelopRaw_CopyPalette(This,pIPalette)	\
    ( (This)->lpVtbl -> CopyPalette(This,pIPalette) ) 

#define IWICDevelopRaw_CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer)	\
    ( (This)->lpVtbl -> CopyPixels(This,prc,cbStride,cbBufferSize,pbBuffer) ) 


#define IWICDevelopRaw_GetMetadataQueryReader(This,ppIMetadataQueryReader)	\
    ( (This)->lpVtbl -> GetMetadataQueryReader(This,ppIMetadataQueryReader) ) 

#define IWICDevelopRaw_GetColorContexts(This,cCount,ppIColorContexts,pcActualCount)	\
    ( (This)->lpVtbl -> GetColorContexts(This,cCount,ppIColorContexts,pcActualCount) ) 

#define IWICDevelopRaw_GetThumbnail(This,ppIThumbnail)	\
    ( (This)->lpVtbl -> GetThumbnail(This,ppIThumbnail) ) 


#define IWICDevelopRaw_QueryRawCapabilitiesInfo(This,pInfo)	\
    ( (This)->lpVtbl -> QueryRawCapabilitiesInfo(This,pInfo) ) 

#define IWICDevelopRaw_LoadParameterSet(This,ParameterSet)	\
    ( (This)->lpVtbl -> LoadParameterSet(This,ParameterSet) ) 

#define IWICDevelopRaw_GetCurrentParameterSet(This,ppCurrentParameterSet)	\
    ( (This)->lpVtbl -> GetCurrentParameterSet(This,ppCurrentParameterSet) ) 

#define IWICDevelopRaw_SetExposureCompensation(This,ev)	\
    ( (This)->lpVtbl -> SetExposureCompensation(This,ev) ) 

#define IWICDevelopRaw_GetExposureCompensation(This,pEV)	\
    ( (This)->lpVtbl -> GetExposureCompensation(This,pEV) ) 

#define IWICDevelopRaw_SetWhitePointRGB(This,Red,Green,Blue)	\
    ( (This)->lpVtbl -> SetWhitePointRGB(This,Red,Green,Blue) ) 

#define IWICDevelopRaw_GetWhitePointRGB(This,pRed,pGreen,pBlue)	\
    ( (This)->lpVtbl -> GetWhitePointRGB(This,pRed,pGreen,pBlue) ) 

#define IWICDevelopRaw_SetNamedWhitePoint(This,WhitePoint)	\
    ( (This)->lpVtbl -> SetNamedWhitePoint(This,WhitePoint) ) 

#define IWICDevelopRaw_GetNamedWhitePoint(This,pWhitePoint)	\
    ( (This)->lpVtbl -> GetNamedWhitePoint(This,pWhitePoint) ) 

#define IWICDevelopRaw_SetWhitePointKelvin(This,WhitePointKelvin)	\
    ( (This)->lpVtbl -> SetWhitePointKelvin(This,WhitePointKelvin) ) 

#define IWICDevelopRaw_GetWhitePointKelvin(This,pWhitePointKelvin)	\
    ( (This)->lpVtbl -> GetWhitePointKelvin(This,pWhitePointKelvin) ) 

#define IWICDevelopRaw_GetKelvinRangeInfo(This,pMinKelvinTemp,pMaxKelvinTemp,pKelvinTempStepValue)	\
    ( (This)->lpVtbl -> GetKelvinRangeInfo(This,pMinKelvinTemp,pMaxKelvinTemp,pKelvinTempStepValue) ) 

#define IWICDevelopRaw_SetContrast(This,Contrast)	\
    ( (This)->lpVtbl -> SetContrast(This,Contrast) ) 

#define IWICDevelopRaw_GetContrast(This,pContrast)	\
    ( (This)->lpVtbl -> GetContrast(This,pContrast) ) 

#define IWICDevelopRaw_SetGamma(This,Gamma)	\
    ( (This)->lpVtbl -> SetGamma(This,Gamma) ) 

#define IWICDevelopRaw_GetGamma(This,pGamma)	\
    ( (This)->lpVtbl -> GetGamma(This,pGamma) ) 

#define IWICDevelopRaw_SetSharpness(This,Sharpness)	\
    ( (This)->lpVtbl -> SetSharpness(This,Sharpness) ) 

#define IWICDevelopRaw_GetSharpness(This,pSharpness)	\
    ( (This)->lpVtbl -> GetSharpness(This,pSharpness) ) 

#define IWICDevelopRaw_SetSaturation(This,Saturation)	\
    ( (This)->lpVtbl -> SetSaturation(This,Saturation) ) 

#define IWICDevelopRaw_GetSaturation(This,pSaturation)	\
    ( (This)->lpVtbl -> GetSaturation(This,pSaturation) ) 

#define IWICDevelopRaw_SetTint(This,Tint)	\
    ( (This)->lpVtbl -> SetTint(This,Tint) ) 

#define IWICDevelopRaw_GetTint(This,pTint)	\
    ( (This)->lpVtbl -> GetTint(This,pTint) ) 

#define IWICDevelopRaw_SetNoiseReduction(This,NoiseReduction)	\
    ( (This)->lpVtbl -> SetNoiseReduction(This,NoiseReduction) ) 

#define IWICDevelopRaw_GetNoiseReduction(This,pNoiseReduction)	\
    ( (This)->lpVtbl -> GetNoiseReduction(This,pNoiseReduction) ) 

#define IWICDevelopRaw_SetDestinationColorContext(This,pColorContext)	\
    ( (This)->lpVtbl -> SetDestinationColorContext(This,pColorContext) ) 

#define IWICDevelopRaw_SetToneCurve(This,cbToneCurveSize,pToneCurve)	\
    ( (This)->lpVtbl -> SetToneCurve(This,cbToneCurveSize,pToneCurve) ) 

#define IWICDevelopRaw_GetToneCurve(This,cbToneCurveBufferSize,pToneCurve,pcbActualToneCurveBufferSize)	\
    ( (This)->lpVtbl -> GetToneCurve(This,cbToneCurveBufferSize,pToneCurve,pcbActualToneCurveBufferSize) ) 

#define IWICDevelopRaw_SetRotation(This,Rotation)	\
    ( (This)->lpVtbl -> SetRotation(This,Rotation) ) 

#define IWICDevelopRaw_GetRotation(This,pRotation)	\
    ( (This)->lpVtbl -> GetRotation(This,pRotation) ) 

#define IWICDevelopRaw_SetRenderMode(This,RenderMode)	\
    ( (This)->lpVtbl -> SetRenderMode(This,RenderMode) ) 

#define IWICDevelopRaw_GetRenderMode(This,pRenderMode)	\
    ( (This)->lpVtbl -> GetRenderMode(This,pRenderMode) ) 

#define IWICDevelopRaw_SetNotificationCallback(This,pCallback)	\
    ( (This)->lpVtbl -> SetNotificationCallback(This,pCallback) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IWICDevelopRaw_Remote_QueryRawCapabilitiesInfo_Proxy( 
    __RPC__in IWICDevelopRaw * This,
    /* [out][in] */ __RPC__inout WICRawCapabilitiesInfo *pInfo);


void __RPC_STUB IWICDevelopRaw_Remote_QueryRawCapabilitiesInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IWICDevelopRaw_Remote_SetToneCurve_Proxy( 
    __RPC__in IWICDevelopRaw * This,
    /* [in] */ UINT cPoints,
    /* [size_is][in] */ __RPC__in_ecount_full(cPoints) const WICRawToneCurvePoint *aPoints);


void __RPC_STUB IWICDevelopRaw_Remote_SetToneCurve_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IWICDevelopRaw_Remote_GetToneCurve_Proxy( 
    __RPC__in IWICDevelopRaw * This,
    /* [out] */ __RPC__out UINT *pcPoints,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*pcPoints) WICRawToneCurvePoint **paPoints);


void __RPC_STUB IWICDevelopRaw_Remote_GetToneCurve_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWICDevelopRaw_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     __RPC__in unsigned long *, __RPC__in BSTR * ); 

unsigned long             __RPC_USER  HBITMAP_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in HBITMAP * ); 
unsigned char * __RPC_USER  HBITMAP_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in HBITMAP * ); 
unsigned char * __RPC_USER  HBITMAP_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out HBITMAP * ); 
void                      __RPC_USER  HBITMAP_UserFree(     __RPC__in unsigned long *, __RPC__in HBITMAP * ); 

unsigned long             __RPC_USER  HICON_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in HICON * ); 
unsigned char * __RPC_USER  HICON_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in HICON * ); 
unsigned char * __RPC_USER  HICON_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out HICON * ); 
void                      __RPC_USER  HICON_UserFree(     __RPC__in unsigned long *, __RPC__in HICON * ); 

unsigned long             __RPC_USER  HPALETTE_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in HPALETTE * ); 
unsigned char * __RPC_USER  HPALETTE_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in HPALETTE * ); 
unsigned char * __RPC_USER  HPALETTE_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out HPALETTE * ); 
void                      __RPC_USER  HPALETTE_UserFree(     __RPC__in unsigned long *, __RPC__in HPALETTE * ); 

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out LPSAFEARRAY * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree(     __RPC__in unsigned long *, __RPC__in LPSAFEARRAY * ); 

unsigned long             __RPC_USER  WICInProcPointer_UserSize(     __RPC__in unsigned long *, unsigned long            , __RPC__in WICInProcPointer * ); 
unsigned char * __RPC_USER  WICInProcPointer_UserMarshal(  __RPC__in unsigned long *, __RPC__inout_xcount(0) unsigned char *, __RPC__in WICInProcPointer * ); 
unsigned char * __RPC_USER  WICInProcPointer_UserUnmarshal(__RPC__in unsigned long *, __RPC__in_xcount(0) unsigned char *, __RPC__out WICInProcPointer * ); 
void                      __RPC_USER  WICInProcPointer_UserFree(     __RPC__in unsigned long *, __RPC__in WICInProcPointer * ); 

/* [local] */ HRESULT STDMETHODCALLTYPE IWICBitmapCodecProgressNotification_RegisterProgressNotification_Proxy( 
    IWICBitmapCodecProgressNotification * This,
    /* [annotation][unique][in] */ 
    __in_opt  PFNProgressNotification pfnProgressNotification,
    /* [annotation][unique][in] */ 
    __in_opt  LPVOID pvData,
    /* [in] */ DWORD dwProgressFlags);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IWICBitmapCodecProgressNotification_RegisterProgressNotification_Stub( 
    __RPC__in IWICBitmapCodecProgressNotification * This,
    /* [unique][in] */ __RPC__in_opt IWICProgressCallback *pICallback,
    /* [in] */ DWORD dwProgressFlags);

/* [local] */ HRESULT STDMETHODCALLTYPE IWICBitmapDecoderInfo_GetPatterns_Proxy( 
    IWICBitmapDecoderInfo * This,
    /* [in] */ UINT cbSizePatterns,
    /* [annotation][unique][size_is][out] */ 
    __out_bcount_part_opt(cbSizePatterns, *pcbPatternsActual)  WICBitmapPattern *pPatterns,
    /* [annotation][unique][out] */ 
    __inout_opt  UINT *pcPatterns,
    /* [annotation][out] */ 
    __inout_opt  UINT *pcbPatternsActual);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IWICBitmapDecoderInfo_GetPatterns_Stub( 
    __RPC__in IWICBitmapDecoderInfo * This,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*pcPatterns) WICBitmapPattern **ppPatterns,
    /* [out] */ __RPC__out UINT *pcPatterns);

/* [local] */ HRESULT STDMETHODCALLTYPE IWICDevelopRaw_QueryRawCapabilitiesInfo_Proxy( 
    IWICDevelopRaw * This,
    /* [out][in] */ WICRawCapabilitiesInfo *pInfo);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IWICDevelopRaw_QueryRawCapabilitiesInfo_Stub( 
    __RPC__in IWICDevelopRaw * This,
    /* [out][in] */ __RPC__inout WICRawCapabilitiesInfo *pInfo);

/* [local] */ HRESULT STDMETHODCALLTYPE IWICDevelopRaw_SetToneCurve_Proxy( 
    IWICDevelopRaw * This,
    /* [in] */ UINT cbToneCurveSize,
    /* [annotation][in] */ 
    __in_bcount(cbToneCurveSize)  const WICRawToneCurve *pToneCurve);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IWICDevelopRaw_SetToneCurve_Stub( 
    __RPC__in IWICDevelopRaw * This,
    /* [in] */ UINT cPoints,
    /* [size_is][in] */ __RPC__in_ecount_full(cPoints) const WICRawToneCurvePoint *aPoints);

/* [local] */ HRESULT STDMETHODCALLTYPE IWICDevelopRaw_GetToneCurve_Proxy( 
    IWICDevelopRaw * This,
    /* [in] */ UINT cbToneCurveBufferSize,
    /* [annotation][unique][out] */ 
    __out_bcount_part_opt(cbToneCurveBufferSize, *pcbActualToneCurveBufferSize)  WICRawToneCurve *pToneCurve,
    /* [annotation][unique][out] */ 
    __inout_opt  UINT *pcbActualToneCurveBufferSize);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IWICDevelopRaw_GetToneCurve_Stub( 
    __RPC__in IWICDevelopRaw * This,
    /* [out] */ __RPC__out UINT *pcPoints,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*pcPoints) WICRawToneCurvePoint **paPoints);



/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



