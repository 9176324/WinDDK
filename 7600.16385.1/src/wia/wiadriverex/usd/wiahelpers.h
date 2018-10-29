/**************************************************************************
*
*  Copyright (c) 2003  Microsoft Corporation
*
*  Title: wiahelpers.h
*
*  Description: This contains the WIA driver class helper functions.
*
***************************************************************************/
#pragma once

#define NO_FIXED_FRAME 0xFFFFFFFF

typedef struct _FILM_FRAME {
    LONG    XPOS;
    LONG    YPOS;
    LONG    XEXTENT;
    LONG    YEXTENT;
} FILM_FRAME;

HRESULT MakeFullItemName(
    __in    IWiaDrvItem *pParent,
    __in    BSTR        bstrItemName,
    __out   BSTR        *pbstrFullItemName);

HRESULT CreateWIAChildItem(
    __in            LPOLESTR    wszItemName,
    __in            IWiaMiniDrv *pIWiaMiniDrv,
    __in            IWiaDrvItem *pParent,
                    LONG        lItemFlags,
                    GUID        guidItemCategory,
   __out_opt        IWiaDrvItem **ppChild       = NULL,
   __in_opt         const WCHAR *wszStoragePath = NULL);

HRESULT CreateWIAFlatbedItem(
    __in    LPOLESTR    wszItemName,
    __in    IWiaMiniDrv *pIWiaMiniDrv,
    __in    IWiaDrvItem *pParent);

HRESULT CreateWIAFeederItem(
    __in    LPOLESTR    wszItemName,
    __in    IWiaMiniDrv *pIWiaMiniDrv,
    __in    IWiaDrvItem *pParent);

HRESULT CreateWIAFilmItem(
    __in    LPOLESTR    wszItemName,
    __in    IWiaMiniDrv *pIWiaMiniDrv,
    __in    IWiaDrvItem *pParent);

HRESULT CreateWIAStorageItem(
    __in    LPOLESTR    wszItemName,
    __in    IWiaMiniDrv *pIWiaMiniDrv,
    __in    IWiaDrvItem *pParent,
    __in    const WCHAR *wszStoragePath);

HRESULT InitializeRootItemProperties(
    __in    BYTE        *pWiasContext);

HRESULT InitializeWIAItemProperties(
    __in    BYTE        *pWiasContext,
    __in    HINSTANCE   hInstance,
            UINT        uiResourceID);

HRESULT InitializeWIAStorageItemProperties(
    __in    BYTE        *pWiasContext,
            BOOL        bRootItem,
            BOOL        bFolderItem);

HRESULT wiasGetDriverItemPrivateContext(
    __in    BYTE *pWiasContext,
    __out   BYTE **ppWiaDriverItemContext);

HRESULT wiasGetAppItemParent(
    __in    BYTE *pWiasContext,
    __out   BYTE **ppWiasContext);

LONG ConvertTo1000thsOfAnInch(
    LONG lPixelSize,
    LONG lResolution);

HRESULT GetBitmapHeaderFromBitmapData(
    __in    Gdiplus::BitmapData *pGDIPlusBitmapData,
    __out BITMAPINFOHEADER    *pBitmapInfoHeader);

HRESULT GetSelectionAreaRect(
    __in    BYTE            *pWiasContext,
    __out   Gdiplus::Rect   *pRect);

HRESULT LockSelectionAreaOnBitmap(
    __in    BYTE                    *pWiasContext,
    __in    Gdiplus::Bitmap         *pBitmap,
    __in    Gdiplus::BitmapData     *pBitmapData,
    __in    BITMAPINFOHEADER        *pbmih,
    __out   BYTE                    **ppBitmapBits);

void UnlockSelectionAreaOnBitmap(
    __in    Gdiplus::Bitmap         *pBitmap,
    __in    Gdiplus::BitmapData     *pBitmapData);

HRESULT GetTransferCallback(
    __in        PMINIDRV_TRANSFER_CONTEXT       pmdtc,
    __callback  IWiaMiniDrvTransferCallback     **ppIWiaMiniDrvTransferCallback);

HRESULT AllocateTransferBuffer(
    __in    BYTE                    *pWiasContext,
    __out   BYTE                    **ppBuffer,
    __in    ULONG                    *pulBufferSize);

void FreeTransferBuffer(
    __in    BYTE                    *pBuffer);

HRESULT GetFileExtensionFromPath(
    __in    BSTR    bstrFullPath,
    __out   BSTR    *pbstrExtension);

bool IsProgrammableItem(
    __in    BYTE    *pWiasContext);

void QueueWIAEvent(
    __in    BYTE        *pWiasContext,
            const GUID  &guidWIAEvent);

