//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright  1997 - 2003  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:   Intrface.H
//    
//
//  PURPOSE:    Define COM interface for User Mode Printer Customization DLL.
//
//  PLATFORMS:
//
//    Windows XP, Windows Server 2003
//
//
//  History: 
//          06/26/03    xxx created.
//
//

#ifndef _INTERFACE_H
#define _INTERFACE_H

/////////////////////////////////////////////////////////
//      String Literals
/////////////////////////////////////////////////////////


CONST CHAR NAME_CommandCallback[]       = "CommandCallback";
CONST CHAR NAME_Compression[]               = "Compression";
CONST CHAR NAME_DownloadCharGlyph[]     = "DownloadCharGlyph";
CONST CHAR NAME_DownloadFontHeader[]        = "DownloadFontHeader";
CONST CHAR NAME_FilterGraphics[]            = "FilterGraphics";
CONST CHAR NAME_HalftonePattern[]           = "HalftonePattern";
CONST CHAR NAME_ImageProcessing[]           = "ImageProcessing";
CONST CHAR NAME_MemoryUsage[]           = "MemoryUsage";
CONST CHAR NAME_OutputCharStr[]         = "OutputCharStr";
CONST CHAR NAME_SendFontCmd[]           = "SendFontCmd";
CONST CHAR NAME_TextOutAsBitmap[]           = "TextOutAsBitmap";
CONST CHAR NAME_TTDownloadMethod[]      = "TTDownloadMethod";
CONST CHAR NAME_TTYGetInfo[]                = "TTYGetInfo";
CONST CHAR NAME_WritePrinter[]              = "WritePrinter";




////////////////////////////////////////////////////////////////////////////////
//
//  COemUni2
//
//  Interface for Unidrv OEM sample rendering module
//
class COemUni2 : public IPrintOemUni2
{
public:
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR* ppvObj);
    STDMETHOD_(ULONG,AddRef)  (THIS);
    STDMETHOD_(ULONG,Release) (THIS);

    //
    // Method for publishing Driver interface.
    //
    STDMETHOD(PublishDriverInterface)(THIS_ IUnknown    *pIUnknown);

    //
    // Method for getting the implemented methods.
    // Returns S_OK if the given method is implemneted.
    // Returns S_FALSE if the given method is notimplemneted.
    //

    STDMETHOD(GetImplementedMethod)(THIS_ PSTR  pMethodName);

    //
    // Method for OEM to specify DDIs that have been hooked
    //

    STDMETHOD(EnableDriver) (THIS_ DWORD            DriverVersion,
                                DWORD               cbSize,
                                PDRVENABLEDATA      pded);

    //
    // Method to notify OEM plugin that it is no longer required
    //

    STDMETHOD(DisableDriver) (THIS);

    //
    // Method for OEM plugin to contruct its own PDEV
    //

    STDMETHOD(EnablePDEV)(THIS_ PDEVOBJ     pdevobj,
                            PWSTR               pPrinterName,
                            ULONG               cPatterns,
                            HSURF               *phsurfPatterns,
                            ULONG               cjGdiInfo,
                            GDIINFO             *pGdiInfo,
                            ULONG               cjDevInfo,
                            DEVINFO             *pDevInfo,
                            DRVENABLEDATA       *pded,
                            OUT PDEVOEM     *pDevOem);

    //
    // Method for OEM to free any resource associated with its PDEV
    //

    STDMETHOD(DisablePDEV)(THIS_ PDEVOBJ    pdevobj);

    //
    // Method for OEM to transfer from old PDEV to new PDEV
    //

    STDMETHOD(ResetPDEV)(THIS_ PDEVOBJ      pdevobjOld,
                            PDEVOBJ             pdevobjNew);

    //
    // Get OEM dll related information
    //

    STDMETHOD(GetInfo)(THIS_ DWORD      dwMode,
                        PVOID               pBuffer,
                        DWORD               cbSize,
                        PDWORD              pcbNeeded);

    //
    // OEMDriverDMS - UNIDRV only, return E_NOTIMPL on Pscript
    //

    STDMETHOD(DriverDMS)(THIS_ PVOID        pDevObj,
                            PVOID           pBuffer,
                            DWORD           cbSize,
                            PDWORD          pcbNeeded);

    //
    // OEMDevMode
    //

    STDMETHOD(DevMode)(THIS_ DWORD      dwMode,
                        POEMDMPARAM     pOemDMParam);

    //
    // OEMCommandCallback - UNIDRV only, return E_NOTIMPL on Pscript
    //

    STDMETHOD(CommandCallback)(THIS_ PDEVOBJ        pdevobj,
                                    DWORD               dwCallbackID,
                                    DWORD               dwCount,
                                    PDWORD              pdwParams,
                                    OUT INT             *piResult);

    //
    // OEMImageProcessing - UNIDRV only, return E_NOTIMPL on Pscript
    //

    STDMETHOD(ImageProcessing)(THIS_ PDEVOBJ            pdevobj,
                                    PBYTE                   pSrcBitmap,
                                    PBITMAPINFOHEADER       pBitmapInfoHeader,
                                    PBYTE                   pColorTable,
                                    DWORD                   dwCallbackID,
                                    PIPPARAMS               pIPParams,
                                    OUT PBYTE               *ppbResult);

    //
    // OEMFilterGraphics - UNIDRV only, return E_NOTIMPL on Pscript
    //

    STDMETHOD(FilterGraphics)(THIS_ PDEVOBJ pdevobj,
                                PBYTE           pBuf,
                                DWORD           dwLen);
    //
    // OEMCompression - UNIDRV only, return E_NOTIMPL on Pscript
    //

    STDMETHOD(Compression)(THIS_ PDEVOBJ        pdevobj,
                                PBYTE               pInBuf,
                                PBYTE               pOutBuf,
                                DWORD               dwInLen,
                                DWORD               dwOutLen,
                                OUT INT             *piResult);

    //
    // OEMHalftone - UNIDRV only, return E_NOTIMPL on Pscript
    //

    STDMETHOD(HalftonePattern)(THIS_ PDEVOBJ        pdevobj,
                                PBYTE               pHTPattern,
                                DWORD               dwHTPatternX,
                                DWORD               dwHTPatternY,
                                DWORD               dwHTNumPatterns,
                                DWORD               dwCallbackID,
                                PBYTE               pResource,
                                DWORD               dwResourceSize);

    //
    // OEMMemoryUsage - UNIDRV only, return E_NOTIMPL on Pscript
    //

    STDMETHOD(MemoryUsage)(THIS_ PDEVOBJ        pdevobj,
                                POEMMEMORYUSAGE pMemoryUsage);

    //
    // OEMTTYGetInfo - UNIDRV only, return E_NOTIMPL on Pscript
    //

    STDMETHOD(TTYGetInfo)(THIS_ PDEVOBJ     pdevobj,
                            DWORD               dwInfoIndex,
                            PVOID               pOutputBuf,
                            DWORD               dwSize,
                            DWORD               *pcbcNeeded);

    //
    // OEMDownloadFontheader - UNIDRV only
    //

    STDMETHOD(DownloadFontHeader)(THIS_ PDEVOBJ pdevobj,
                                        PUNIFONTOBJ     pUFObj,
                                        OUT DWORD       *pdwResult);

    //
    // OEMDownloadCharGlyph - UNIDRV only
    //

    STDMETHOD(DownloadCharGlyph)(THIS_ PDEVOBJ  pdevobj,
                                    PUNIFONTOBJ         pUFObj,
                                    HGLYPH              hGlyph,
                                    PDWORD              pdwWidth,
                                    OUT DWORD           *pdwResult);

    //
    // OEMTTDownloadMethod - UNIDRV only
    //

    STDMETHOD(TTDownloadMethod)(THIS_ PDEVOBJ   pdevobj,
                                    PUNIFONTOBJ         pUFObj,
                                    OUT DWORD           *pdwResult);

    //
    // OEMOutputCharStr - UNIDRV only
    //

    STDMETHOD(OutputCharStr)(THIS_ PDEVOBJ      pdevobj,
                                PUNIFONTOBJ         pUFObj,
                                DWORD               dwType,
                                DWORD               dwCount,
                                PVOID               pGlyph);

    //
    // OEMSendFontCmd - UNIDRV only
    //


    STDMETHOD(SendFontCmd)(THIS_ PDEVOBJ        pdevobj,
                                PUNIFONTOBJ         pUFObj,
                                PFINVOCATION        pFInv);

    //
    // OEMTextOutAsBitmap - UNIDRV only
    //

    STDMETHOD(TextOutAsBitmap)(THIS_ SURFOBJ        *pso,
                                    STROBJ              *pstro,
                                    FONTOBJ             *pfo,
                                    CLIPOBJ             *pco,
                                    RECTL               *prclExtra,
                                    RECTL               *prclOpaque,
                                    BRUSHOBJ            *pboFore,
                                    BRUSHOBJ            *pboOpaque,
                                    POINTL              *pptlOrg,
                                    MIX                 mix);

    //
    // IPrintOemUni2 methods
    //

    //
    // Method for plugin to hook out spooler's WritePrinter API so it
    // can get access to output data Universal driver is generating
    //
    // At DrvEnablePDEV time, Universal driver will call this function with
    // pdevobj = NULL, pBuf = NULL, cbBuffer = 0 to detect if the plugin
    // implements this function. Plugin should return S_OK to indicate it is
    // implementing this function, or return E_NOTIMPL otherwise.
    //
    // In pcbWritten, plugins should return the number of bytes written to the
    // spooler's WritePrinter function. Zero doesn't carry a special meaning,
    // errors must be reported through the returned HRESULT.
    //

    STDMETHOD(WritePrinter)(THIS_ PDEVOBJ       pdevobj,
                            PVOID               pBuf,
                            DWORD               cbBuffer,
                            PDWORD              pcbWritten);


    COemUni2();
    ~COemUni2();

protected:
    LONG                m_cRef;
    IPrintOemDriverUni* m_pOEMHelp;
};





#endif





