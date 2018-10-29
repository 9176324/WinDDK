/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   colchan.h

Abstract:

   Color channel class definition. The color channel class is responsible for maintaining
   simple single color multiple channel data intialised from color references in the XPS markup.
   It provides methods for intialization, access and conversion of the data.

--*/

#pragma once

enum EColorDataType
{
    sRGB = 0,
    scRGB,
    nChannel
};

class CColorChannelData
{
public:
    CColorChannelData();

    ~CColorChannelData();

    template <class _T>
    HRESULT
    AddChannelData(
        __in CONST _T& channelValue
        );

    HRESULT
    GetChannelCount(
        __out DWORD* pcChannels
        );

    HRESULT
    GetChannelCountNoAlpha(
        __out DWORD* pcChannels
        );

    HRESULT
    GetChannelType(
        __out COLORDATATYPE* pChannelType
        );

    HRESULT
    ResetChannelType(
        __in CONST COLORDATATYPE& channelType
        );

    //
    // See colchan.cpp for a note about the annotation on ppData
    //
    HRESULT
    GetChannelData(
        __out           DWORD* pcbDataSize,
        __ecount(1) __post __byte_readableTo(1)
        __post __drv_when(*pcbDataSize >  0, __post __deref __byte_writableTo(*pcbDataSize) __post __deref __valid __refparam)
        __post __drv_when(*pcbDataSize == 0, __post __deref __maybenull)
                        PVOID* ppData
        );

    //
    // See colchan.cpp for a note about the annotation on ppData
    //
    HRESULT
    GetChannelDataNoAlpha(
        __out           DWORD* pcbDataSize,
        __ecount(1) __post __byte_readableTo(1)
        __post __drv_when(*pcbDataSize >  0, __post __deref __byte_writableTo(*pcbDataSize) __post __deref __valid __refparam)
        __post __drv_when(*pcbDataSize == 0, __post __deref __maybenull)
                        PVOID* ppData
        );

    template <class _T>
    HRESULT
    ClampChannelValues(
        __in CONST _T& min,
        __in CONST _T& max
        );

    template <class _T>
    HRESULT
    InitializeChannelData(
        __in CONST COLORDATATYPE& channelType,
        __in CONST EColorDataType& dataType,
        __in CONST DWORD&         cChannels,
        __in CONST _T&            channelValue
        );

    HRESULT
    SetColorDataType(
        __in CONST EColorDataType& dataType
        );

    HRESULT
    GetColorDataType(
        __out EColorDataType* pDataType
        );

    BOOL
    HasAlpha(
        VOID
        );

    HRESULT
    InitializeAlphaChannel(
        __in CColorChannelData* pSrcChannelData
        );

    HRESULT
    GetColor(
        __out PCOLOR     pColor,
        __out COLORTYPE* pType
        );

    HRESULT
    SetColor(
        __in  COLOR*           pColor,
        __in  CONST COLORTYPE& type
        );

private:
    HRESULT
    GetChannelSizeFromType(
        __in  CONST COLORDATATYPE& channelType,
        __out DWORD*               pcbChannelSize
        );

    template <class _T>
    HRESULT
    ValidateDataSize(
        VOID
        );

    HRESULT
    AllocateChannelBuffers(
        __out                          UINT*  pcbBuffer,
        __deref_out_bcount(*pcbBuffer) PBYTE* ppBuffer
        );

    VOID
    FreeChannelBuffers(
        VOID
        );

    HRESULT
    GetAlphaAsFloat(
        __out PFLOAT pAlpha
        );

    HRESULT
    ColorToByte(
        __out_bcount(cbDstData) PBYTE      pDstData,
        __in                    CONST UINT cbDstData
        );

    HRESULT
    ColorToWord(
        __out_bcount(cbDstData) PWORD      pDstData,
        __in                    CONST UINT cbDstData
        );

    HRESULT
    ColorFromByte(
        __in_bcount(cbSrcData) PBYTE       pSrcData,
        __in                   CONST UINT& cbSrcData
        );

    HRESULT
    ColorFromWord(
        __in_bcount(cbSrcData) PWORD       pSrcData,
        __in                   CONST UINT& cbSrcData
        );

    HRESULT
    GetAlphaChannelSize(
        __out DWORD* pcbAlphaChan
        );

private:
    DWORD          m_cChannels;

    COLORDATATYPE  m_channelType;

    PBYTE          m_pChannelData;

    UINT           m_cbChannelData;

    EColorDataType m_dataType;
};

//
// Explicitly instantiate the template functions for BYTE, WORD and FLOAT
//
template
HRESULT
CColorChannelData::AddChannelData<BYTE>(
    __in CONST BYTE& channelValue
    );

template
HRESULT
CColorChannelData::ClampChannelValues<BYTE>(
    __in CONST BYTE& min,
    __in CONST BYTE& max
    );

template
HRESULT
CColorChannelData::InitializeChannelData<BYTE>(
    __in CONST COLORDATATYPE& channelType,
    __in CONST EColorDataType& dataType,
    __in CONST DWORD&         cChannels,
    __in CONST BYTE&          channelValue
    );

template
HRESULT
CColorChannelData::ValidateDataSize<BYTE>(
    VOID
    );

template
HRESULT
CColorChannelData::AddChannelData<WORD>(
    __in CONST WORD& channelValue
    );

template
HRESULT
CColorChannelData::ClampChannelValues<WORD>(
    __in CONST WORD& min,
    __in CONST WORD& max
    );

template
HRESULT
CColorChannelData::InitializeChannelData<WORD>(
    __in CONST COLORDATATYPE& channelType,
    __in CONST EColorDataType& dataType,
    __in CONST DWORD&         cChannels,
    __in CONST WORD&          channelValue
    );

template
HRESULT
CColorChannelData::ValidateDataSize<WORD>(
    VOID
    );

template
HRESULT
CColorChannelData::AddChannelData<FLOAT>(
    __in CONST FLOAT& channelValue
    );

template
HRESULT
CColorChannelData::ClampChannelValues<FLOAT>(
    __in CONST FLOAT& min,
    __in CONST FLOAT& max
    );

template
HRESULT
CColorChannelData::InitializeChannelData<FLOAT>(
    __in CONST COLORDATATYPE& channelType,
    __in CONST EColorDataType& dataType,
    __in CONST DWORD&         cChannels,
    __in CONST FLOAT&         channelValue
    );

template
HRESULT
CColorChannelData::ValidateDataSize<FLOAT>(
    VOID
    );


