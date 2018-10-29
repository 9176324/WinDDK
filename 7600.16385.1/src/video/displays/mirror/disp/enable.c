/******************************Module*Header*******************************\
*
*                           *******************
*                           * GDI SAMPLE CODE *
*                           *******************
*
* Module Name: enable.c
*
* This module contains the functions that enable and disable the
* driver, the pdev, and the surface.
*
* Copyright (c) 1992-1998 Microsoft Corporation
\**************************************************************************/
#define DBG 1

#include "driver.h"

// The driver function table with all function index/address pairs

static DRVFN gadrvfn[] =
{
    {   INDEX_DrvEnablePDEV,            (PFN) DrvEnablePDEV         },
    {   INDEX_DrvCompletePDEV,          (PFN) DrvCompletePDEV       },
    {   INDEX_DrvDisablePDEV,           (PFN) DrvDisablePDEV        },
    {   INDEX_DrvEnableSurface,         (PFN) DrvEnableSurface      },
    {   INDEX_DrvDisableSurface,        (PFN) DrvDisableSurface     },
    {   INDEX_DrvAssertMode,            (PFN) DrvAssertMode         },
    {   INDEX_DrvNotify,                (PFN) DrvNotify             },
    {   INDEX_DrvCreateDeviceBitmap,    (PFN) DrvCreateDeviceBitmap },
    {   INDEX_DrvDeleteDeviceBitmap,    (PFN) DrvDeleteDeviceBitmap },
    {   INDEX_DrvTextOut,               (PFN) DrvTextOut            },
    {   INDEX_DrvBitBlt,                (PFN) DrvBitBlt             },
    {   INDEX_DrvCopyBits,              (PFN) DrvCopyBits           },
    {   INDEX_DrvStrokePath,            (PFN) DrvStrokePath         },
    {   INDEX_DrvLineTo,                (PFN) DrvLineTo             },
    {   INDEX_DrvFillPath,              (PFN) DrvFillPath           },
    {   INDEX_DrvStrokeAndFillPath,     (PFN) DrvStrokeAndFillPath  },
    {   INDEX_DrvStretchBlt,            (PFN) DrvStretchBlt         },
    {   INDEX_DrvAlphaBlend,            (PFN) DrvAlphaBlend         },
    {   INDEX_DrvTransparentBlt,        (PFN) DrvTransparentBlt     },
    {   INDEX_DrvGradientFill,          (PFN) DrvGradientFill       },
    {   INDEX_DrvPlgBlt,                (PFN) DrvPlgBlt             },
    {   INDEX_DrvStretchBltROP,         (PFN) DrvStretchBltROP      },
#if (NTDDI_VERSION >= NTDDI_VISTA)
    {   INDEX_DrvRenderHint,            (PFN) DrvRenderHint         },
#endif
    {   INDEX_DrvEscape,                (PFN) DrvEscape             }
};

//
// always hook these routines to ensure the mirrored driver
// is called for our surfaces
//

#define flGlobalHooks   HOOK_BITBLT|HOOK_TEXTOUT|HOOK_COPYBITS|HOOK_STROKEPATH|HOOK_LINETO|HOOK_FILLPATH|HOOK_STROKEANDFILLPATH|HOOK_STRETCHBLT|HOOK_ALPHABLEND|HOOK_TRANSPARENTBLT|HOOK_GRADIENTFILL|HOOK_PLGBLT|HOOK_STRETCHBLTROP

// Define the functions you want to hook for 8/16/24/32 pel formats

#define HOOKS_BMF8BPP 0

#define HOOKS_BMF16BPP 0

#define HOOKS_BMF24BPP 0

#define HOOKS_BMF32BPP 0

/******************************Public*Routine******************************\
* DrvEnableDriver
*
* Enables the driver by retrieving the drivers function table and version.
*
\**************************************************************************/

BOOL DrvEnableDriver(
ULONG iEngineVersion,
ULONG cj,
PDRVENABLEDATA pded)
{
// Engine Version is passed down so future drivers can support previous
// engine versions.  A next generation driver can support both the old
// and new engine conventions if told what version of engine it is
// working with.  For the first version the driver does nothing with it.

    iEngineVersion;

    DISPDBG((0,"DrvEnableDriver:\n"));

// Fill in as much as we can.

    if (cj >= sizeof(DRVENABLEDATA))
        pded->pdrvfn = gadrvfn;

    if (cj >= (sizeof(ULONG) * 2))
        pded->c = sizeof(gadrvfn) / sizeof(DRVFN);

// DDI version this driver was targeted for is passed back to engine.
// Future graphic's engine may break calls down to old driver format.

    if (cj >= sizeof(ULONG))
	// DDI_DRIVER_VERSION is now out-dated. See winddi.h
	// DDI_DRIVER_VERSION_NT4 is equivalent to the old DDI_DRIVER_VERSION
        pded->iDriverVersion = DDI_DRIVER_VERSION_NT4;

    return(TRUE);
}

/******************************Public*Routine******************************\
* DrvEnablePDEV
*
* DDI function, Enables the Physical Device.
*
* Return Value: device handle to pdev.
*
\**************************************************************************/

DHPDEV
DrvEnablePDEV(
    __in     DEVMODEW   *pDevmode,                // Pointer to DEVMODE
    __in_opt PWSTR       pwszLogAddress,          // Logical address
    __in     ULONG       cPatterns,               // number of patterns
    __in_opt HSURF      *ahsurfPatterns,          // return standard patterns
    __in     ULONG       cjGdiInfo,               // Length of memory pointed to by pGdiInfo
    __out_bcount(cjGdiInfo)  ULONG    *pGdiInfo,  // Pointer to GdiInfo structure
    __in     ULONG       cjDevInfo,               // Length of following PDEVINFO structure
    __out_bcount(cjDevInfo)  DEVINFO  *pDevInfo,  // physical device information structure
    __in_opt HDEV        hdev,                    // HDEV, used for callbacks
    __in_opt PWSTR       pwszDeviceName,          // DeviceName - not used
    __in     HANDLE      hDriver                  // Handle to base driver
    )
{
    GDIINFO GdiInfo;
    DEVINFO DevInfo;
    PPDEV   ppdev = (PPDEV) NULL;

    DISPDBG((0,"DrvEnablePDEV:\n"));

    UNREFERENCED_PARAMETER(pwszLogAddress);
    UNREFERENCED_PARAMETER(cPatterns);
    UNREFERENCED_PARAMETER(ahsurfPatterns);
    UNREFERENCED_PARAMETER(hdev);
    UNREFERENCED_PARAMETER(pwszDeviceName);

    // Allocate a physical device structure.

    ppdev = (PPDEV) EngAllocMem(FL_ZERO_MEMORY, sizeof(PDEV), ALLOC_TAG);

    if (ppdev == (PPDEV) NULL)
    {
        RIP("DISP DrvEnablePDEV failed EngAllocMem\n");
        return((DHPDEV) 0);
    }

    // Save the screen handle in the PDEV.

    ppdev->hDriver = hDriver;

    // Get the current screen mode information.  Set up device caps and devinfo.

    if (!bInitPDEV(ppdev, pDevmode, &GdiInfo, &DevInfo))
    {
        DISPDBG((0,"DISP DrvEnablePDEV failed\n"));
        goto error_free;
    }
    
    // Copy the devinfo into the engine buffer.

    // memcpy(pDevInfo, &DevInfo, min(sizeof(DEVINFO), cjDevInfo));
    if (sizeof(DEVINFO) > cjDevInfo)
    {
        DISPDBG((0,"DISP DrvEnablePDEV failed: insufficient pDevInfo memory\n"));
        goto error_free;
    }
	RtlCopyMemory(pDevInfo, &DevInfo, sizeof(DEVINFO));

    // Set the pdevCaps with GdiInfo we have prepared to the list of caps for this
    // pdev.

    //memcpy(pGdiInfo, &GdiInfo, min(cjGdiInfo, sizeof(GDIINFO)));
    if (sizeof(GDIINFO) > cjGdiInfo)
    {
        DISPDBG((0,"DISP DrvEnablePDEV failed: insufficient pDevInfo memory\n"));
        goto error_free;
    }
	RtlCopyMemory(pGdiInfo, &GdiInfo, sizeof(GDIINFO));

    return((DHPDEV) ppdev);

    // Error case for failure.
error_free:
    EngFreeMem(ppdev);
    return((DHPDEV) 0);
}

/******************************Public*Routine******************************\
* DrvCompletePDEV
*
* Store the HPDEV, the engines handle for this PDEV, in the DHPDEV.
*
\**************************************************************************/

VOID DrvCompletePDEV(
DHPDEV dhpdev,
HDEV  hdev)
{
    DISPDBG((1,"DrvCompletePDEV:\n"));
    ((PPDEV) dhpdev)->hdevEng = hdev;
}

/******************************Public*Routine******************************\
* DrvDisablePDEV
*
* Release the resources allocated in DrvEnablePDEV.  If a surface has been
* enabled DrvDisableSurface will have already been called.
*
\**************************************************************************/

VOID DrvDisablePDEV(
DHPDEV dhpdev)
{
   PPDEV ppdev = (PPDEV) dhpdev;

   DISPDBG((1,"DrvDisablePDEV:\n"));
   
   EngDeletePalette(ppdev->hpalDefault);

   EngFreeMem(dhpdev);
}

/******************************Public*Routine******************************\
* DrvEnableSurface
*
* Enable the surface for the device.  Hook the calls this driver supports.
*
* Return: Handle to the surface if successful, 0 for failure.
*
\**************************************************************************/

HSURF DrvEnableSurface(
DHPDEV dhpdev)
{
    PPDEV ppdev;
    HSURF hsurf;
    SIZEL sizl;
    ULONG ulBitmapType;
    FLONG flHooks;
    ULONG mirrorsize;
    MIRRSURF *mirrsurf;
    DHSURF dhsurf;

    // Create engine bitmap around frame buffer.

    DISPDBG((0,"DrvEnableSurface:\n"));

    ppdev = (PPDEV) dhpdev;

    ppdev->ptlOrg.x = 0;
    ppdev->ptlOrg.y = 0;

    sizl.cx = ppdev->cxScreen;
    sizl.cy = ppdev->cyScreen;

    if (ppdev->ulBitCount == 16)
    {
        ulBitmapType = BMF_16BPP;
        flHooks = HOOKS_BMF16BPP;
    }
    else if (ppdev->ulBitCount == 24)
    {
        ulBitmapType = BMF_24BPP;
        flHooks = HOOKS_BMF24BPP;
    }
    else
    {
        ulBitmapType = BMF_32BPP;
        flHooks = HOOKS_BMF32BPP;
    }
    
    flHooks |= flGlobalHooks;

    mirrorsize = (ULONG)(sizeof(MIRRSURF) + 
                         ppdev->lDeltaScreen * sizl.cy);
    
    mirrsurf = (MIRRSURF *) EngAllocMem(FL_ZERO_MEMORY,
                                        mirrorsize,
                                        0x4D495252);
    if (!mirrsurf) {
        RIP("DISP DrvEnableSurface failed EngAllocMem\n");
        return(FALSE);
    }
    
    
    dhsurf = (DHSURF) mirrsurf;

    hsurf = EngCreateDeviceSurface(dhsurf,
                                   sizl,
                                   ulBitmapType);

    if (hsurf == (HSURF) 0)
    {
        RIP("DISP DrvEnableSurface failed EngCreateBitmap\n");
        return(FALSE);
    }

    if (!EngAssociateSurface(hsurf, ppdev->hdevEng, flHooks))
    {
        RIP("DISP DrvEnableSurface failed EngAssociateSurface\n");
        EngDeleteSurface(hsurf);
        return(FALSE);
    }

    ppdev->hsurfEng = (HSURF) hsurf;
    ppdev->pvTmpBuffer = (PVOID) dhsurf;

    mirrsurf->cx = ppdev->cxScreen;
    mirrsurf->cy = ppdev->cyScreen;
    mirrsurf->lDelta = ppdev->lDeltaScreen;
    mirrsurf->ulBitCount = ppdev->ulBitCount;
    mirrsurf->bIsScreen = TRUE;

    return(hsurf);
}

/******************************Public*Routine******************************\
* DrvNotify
*
* Receives notification on where the mirror driver is positioned.
* Also gets notified before drawing happens 
*
\**************************************************************************/

VOID DrvNotify(
SURFOBJ *pso,
ULONG iType,
PVOID pvData)
{
    UNREFERENCED_PARAMETER(pso);
    UNREFERENCED_PARAMETER(pvData);

    switch(iType)
    {
        case DN_DEVICE_ORIGIN:
            DISPDBG((0,"DrvNotify: DN_DEVICE_ORIGIN (%d,%d)\n", ((POINTL*)pvData)->x, ((POINTL*)pvData)->y));
            break;
        case DN_DRAWING_BEGIN:
            DISPDBG((0,"DrvNotify: DN_DRAWING_BEGIN\n"));
            break;
    }
}

/******************************Public*Routine******************************\
* DrvDisableSurface
*
* Free resources allocated by DrvEnableSurface.  Release the surface.
*
\**************************************************************************/

VOID DrvDisableSurface(
DHPDEV dhpdev)
{
    PPDEV ppdev = (PPDEV) dhpdev;

    DISPDBG((0,"DrvDisableSurface:\n"));

    EngDeleteSurface( ppdev->hsurfEng );
    
    // deallocate MIRRSURF structure.

    EngFreeMem( ppdev->pvTmpBuffer );
}

/******************************Public*Routine******************************\
* DrvCopyBits
*
\**************************************************************************/

BOOL DrvCopyBits(
   OUT SURFOBJ *psoDst,
   IN SURFOBJ *psoSrc,
   IN CLIPOBJ *pco,
   IN XLATEOBJ *pxlo,
   IN RECTL *prclDst,
   IN POINTL *pptlSrc
   )
{
   INT cnt1 = 0, cnt2 = 0;

   UNREFERENCED_PARAMETER(pco);
   UNREFERENCED_PARAMETER(pxlo);
   UNREFERENCED_PARAMETER(prclDst);
   UNREFERENCED_PARAMETER(pptlSrc);

   DISPDBG((1,"Mirror Driver DrvCopyBits: \n"));

   if (psoSrc)
   {
       if (psoSrc->dhsurf)
       {
          MIRRSURF *mirrsurf = (MIRRSURF *)psoSrc->dhsurf;

          if (mirrsurf->bIsScreen) 
          {
             DISPDBG((1, "From Mirror Screen "));
          }
          else
          {
             DISPDBG((1, "From Mirror DFB "));
          }
          cnt1 ++;
       }
       else
       {
          DISPDBG((1, "From DIB "));
       }
   }

   if (psoDst)
   {
       if (psoDst->dhsurf)
       {
          MIRRSURF *mirrsurf = (MIRRSURF *)psoDst->dhsurf;

          if (mirrsurf->bIsScreen) 
          {
             DISPDBG((1, "to MirrorScreen "));
          }
          else
          {
             DISPDBG((1, "to Mirror DFB "));
          }
          cnt2 ++;
       }
       else
       {
          DISPDBG((1, "to DIB "));
       }
   }

   if (cnt1 && cnt2)
   {
      DISPDBG((1, " [Send Request Over Wire]\n"));
   }
   else if (cnt1)
   {
      DISPDBG((1, " [Read Cached Bits, Or Pull Bits]\n"));
   }
   else if (cnt2) 
   {
      DISPDBG((1, " [Push Bits/Compress]\n"));
   }
   else
   {
      DISPDBG((1, " [What Are We Doing Here?]\n"));
   }

   return TRUE;
}

/******************************Public*Routine******************************\
* DrvBitBlt
*
\**************************************************************************/

BOOL DrvBitBlt(
   IN SURFOBJ *psoDst,
   IN SURFOBJ *psoSrc,
   IN SURFOBJ *psoMask,
   IN CLIPOBJ *pco,
   IN XLATEOBJ *pxlo,
   IN RECTL *prclDst,
   IN POINTL *pptlSrc,
   IN POINTL *pptlMask,
   IN BRUSHOBJ *pbo,
   IN POINTL *pptlBrush,
   IN ROP4 rop4
   )
{
   INT cnt1 = 0, cnt2 = 0;

   UNREFERENCED_PARAMETER(psoMask);
   UNREFERENCED_PARAMETER(pco);
   UNREFERENCED_PARAMETER(pxlo);
   UNREFERENCED_PARAMETER(prclDst);
   UNREFERENCED_PARAMETER(pptlSrc);
   UNREFERENCED_PARAMETER(pptlMask);
   UNREFERENCED_PARAMETER(pbo);
   UNREFERENCED_PARAMETER(pptlBrush);
   UNREFERENCED_PARAMETER(rop4);

   DISPDBG((1,
            "Mirror Driver DrvBitBlt (Mask=%08x, rop=%08x:\n",
            psoMask, 
            rop4));

   if (psoSrc)
   {
       if (psoSrc->dhsurf)
       {
          MIRRSURF *mirrsurf = (MIRRSURF *)psoSrc->dhsurf;

          if (mirrsurf->bIsScreen) 
          {
             DISPDBG((1, "From Mirror Screen "));
          }
          else
          {
             DISPDBG((1, "From Mirror DFB "));
          }
          cnt1 ++;
       }
       else
       {
          DISPDBG((1, "From DIB "));
       }
   }

   if (psoDst)
   {
       if (psoDst->dhsurf)
       {
          MIRRSURF *mirrsurf = (MIRRSURF *)psoDst->dhsurf;

          if (mirrsurf->bIsScreen) 
          {
             DISPDBG((1, "to MirrorScreen "));
          }
          else
          {
             DISPDBG((1, "to Mirror DFB "));
          }
          cnt2 ++;
       }
       else
       {
          DISPDBG((1, "to DIB "));
       }
   }

   if (cnt1 && cnt2)
   {
      DISPDBG((1, " [Send Request Over Wire]\n"));
   }
   else if (cnt1)
   {
      DISPDBG((1, " [Read Cached Bits, Or Pull Bits]\n"));
   }
   else if (cnt2) 
   {
      DISPDBG((1, " [Push Bits/Compress]\n"));
   }
   else
   {
      DISPDBG((1, " [What Are We Doing Here?]\n"));
   }

   return TRUE;
}

BOOL DrvTextOut(
   IN SURFOBJ *psoDst,
   IN STROBJ *pstro,
   IN FONTOBJ *pfo,
   IN CLIPOBJ *pco,
   IN RECTL *prclExtra,
   IN RECTL *prclOpaque,
   IN BRUSHOBJ *pboFore,
   IN BRUSHOBJ *pboOpaque,
   IN POINTL *pptlOrg,
   IN MIX mix
   )
{
   UNREFERENCED_PARAMETER(psoDst);
   UNREFERENCED_PARAMETER(pstro);
   UNREFERENCED_PARAMETER(pfo);
   UNREFERENCED_PARAMETER(pco);
   UNREFERENCED_PARAMETER(prclExtra);
   UNREFERENCED_PARAMETER(prclOpaque);
   UNREFERENCED_PARAMETER(pboFore);
   UNREFERENCED_PARAMETER(pboOpaque);
   UNREFERENCED_PARAMETER(pptlOrg);
   UNREFERENCED_PARAMETER(mix);

   DISPDBG((1,
            "Mirror Driver DrvTextOut: pwstr=%08x\n",
            pstro ? pstro->pwszOrg : (WCHAR*)-1));

   return TRUE;
}

BOOL
DrvStrokePath(SURFOBJ*   pso,
              PATHOBJ*   ppo,
              CLIPOBJ*   pco,
              XFORMOBJ*  pxo,
              BRUSHOBJ*  pbo,
              POINTL*    pptlBrush,
              LINEATTRS* pLineAttrs,
              MIX        mix)
{
    UNREFERENCED_PARAMETER(pso);
    UNREFERENCED_PARAMETER(ppo);
    UNREFERENCED_PARAMETER(pco);
    UNREFERENCED_PARAMETER(pxo);
    UNREFERENCED_PARAMETER(pbo);
    UNREFERENCED_PARAMETER(pptlBrush);
    UNREFERENCED_PARAMETER(pLineAttrs);
    UNREFERENCED_PARAMETER(mix);

   DISPDBG((1,"Mirror Driver DrvStrokePath:\n"));

   return TRUE;
}

BOOL DrvLineTo(
SURFOBJ   *pso,
CLIPOBJ   *pco,
BRUSHOBJ  *pbo,
LONG       x1,
LONG       y1,
LONG       x2,
LONG       y2,
RECTL     *prclBounds,
MIX        mix)
{
    UNREFERENCED_PARAMETER(pso);
    UNREFERENCED_PARAMETER(pco);
    UNREFERENCED_PARAMETER(pbo);
    UNREFERENCED_PARAMETER(x1);
    UNREFERENCED_PARAMETER(y1);
    UNREFERENCED_PARAMETER(x2);
    UNREFERENCED_PARAMETER(y2);
    UNREFERENCED_PARAMETER(prclBounds);
    UNREFERENCED_PARAMETER(mix);

    DISPDBG((1,"Mirror Driver DrvLineTo: \n"));
    return TRUE;
}



BOOL DrvFillPath(
 SURFOBJ  *pso,
 PATHOBJ  *ppo,
 CLIPOBJ  *pco,
 BRUSHOBJ *pbo,
 PPOINTL   pptlBrushOrg,
 MIX       mix,
 FLONG     flOptions)
{
    UNREFERENCED_PARAMETER(pso);
    UNREFERENCED_PARAMETER(ppo);
    UNREFERENCED_PARAMETER(pco);
    UNREFERENCED_PARAMETER(pbo);
    UNREFERENCED_PARAMETER(pptlBrushOrg);
    UNREFERENCED_PARAMETER(mix);
    UNREFERENCED_PARAMETER(flOptions);

    DISPDBG((1,"Mirror Driver DrvFillPath: \n"));
    return TRUE;   
}

BOOL DrvStrokeAndFillPath(
SURFOBJ*   pso,
PATHOBJ*   ppo,
CLIPOBJ*   pco,
XFORMOBJ*  pxo,
BRUSHOBJ*  pboStroke,
LINEATTRS* plineattrs,
BRUSHOBJ*  pboFill,
POINTL*    pptlBrushOrg,
MIX        mixFill,
FLONG      flOptions)
{
    UNREFERENCED_PARAMETER(pso);
    UNREFERENCED_PARAMETER(ppo);
    UNREFERENCED_PARAMETER(pco);
    UNREFERENCED_PARAMETER(pxo);
    UNREFERENCED_PARAMETER(pboStroke);
    UNREFERENCED_PARAMETER(plineattrs);
    UNREFERENCED_PARAMETER(pboFill);
    UNREFERENCED_PARAMETER(pptlBrushOrg);
    UNREFERENCED_PARAMETER(mixFill);
    UNREFERENCED_PARAMETER(flOptions);

    DISPDBG((1,"Mirror Driver DrvStrokeAndFillPath: \n"));
    return TRUE;
}

BOOL DrvTransparentBlt(
SURFOBJ*    psoDst,
SURFOBJ*    psoSrc,
CLIPOBJ*    pco,
XLATEOBJ*   pxlo,
RECTL*      prclDst,
RECTL*      prclSrc,
ULONG       iTransColor,
ULONG       ulReserved)
{
    UNREFERENCED_PARAMETER(psoDst);
    UNREFERENCED_PARAMETER(psoSrc);
    UNREFERENCED_PARAMETER(pco);
    UNREFERENCED_PARAMETER(pxlo);
    UNREFERENCED_PARAMETER(prclDst);
    UNREFERENCED_PARAMETER(prclSrc);
    UNREFERENCED_PARAMETER(iTransColor);
    UNREFERENCED_PARAMETER(ulReserved);

    DISPDBG((1,"Mirror Driver DrvTransparentBlt: \n"));
    return TRUE;
}


BOOL DrvAlphaBlend(
SURFOBJ*            psoDst,
SURFOBJ*            psoSrc,
CLIPOBJ*            pco,
XLATEOBJ*           pxlo,
RECTL*              prclDst,
RECTL*              prclSrc,
BLENDOBJ*           pBlendObj)
{
    UNREFERENCED_PARAMETER(psoDst);
    UNREFERENCED_PARAMETER(psoSrc);
    UNREFERENCED_PARAMETER(pco);
    UNREFERENCED_PARAMETER(pxlo);
    UNREFERENCED_PARAMETER(prclDst);
    UNREFERENCED_PARAMETER(prclSrc);
    UNREFERENCED_PARAMETER(pBlendObj);

    DISPDBG((1,"Mirror Driver DrvAlphaBlend: \n"));
    return TRUE;
}

BOOL DrvGradientFill(
SURFOBJ*            pso,
CLIPOBJ*            pco,
XLATEOBJ*           pxlo,
TRIVERTEX*          pVertex,
ULONG               nVertex,
PVOID               pMesh,
ULONG               nMesh,
RECTL*              prclExtents,
POINTL*             pptlDitherOrg,
ULONG               ulMode)
{
    UNREFERENCED_PARAMETER(pso);
    UNREFERENCED_PARAMETER(pco);
    UNREFERENCED_PARAMETER(pxlo);
    UNREFERENCED_PARAMETER(pVertex);
    UNREFERENCED_PARAMETER(nVertex);
    UNREFERENCED_PARAMETER(pMesh);
    UNREFERENCED_PARAMETER(nMesh);
    UNREFERENCED_PARAMETER(prclExtents);
    UNREFERENCED_PARAMETER(pptlDitherOrg);
    UNREFERENCED_PARAMETER(ulMode);

    DISPDBG((1,"Mirror Driver DrvGradientFill: \n"));
    return TRUE;
}

BOOL DrvStretchBlt(
SURFOBJ*            psoDst,
SURFOBJ*            psoSrc,
SURFOBJ*            psoMsk,
CLIPOBJ*            pco,
XLATEOBJ*           pxlo,
COLORADJUSTMENT*    pca,
POINTL*             pptlHTOrg,
RECTL*              prclDst,
RECTL*              prclSrc,
POINTL*             pptlMsk,
ULONG               iMode)
{
    UNREFERENCED_PARAMETER(psoDst);
    UNREFERENCED_PARAMETER(psoSrc);
    UNREFERENCED_PARAMETER(psoMsk);
    UNREFERENCED_PARAMETER(pco);
    UNREFERENCED_PARAMETER(pxlo);
    UNREFERENCED_PARAMETER(pca);
    UNREFERENCED_PARAMETER(pptlHTOrg);
    UNREFERENCED_PARAMETER(prclDst);
    UNREFERENCED_PARAMETER(prclSrc);
    UNREFERENCED_PARAMETER(pptlMsk);
    UNREFERENCED_PARAMETER(iMode);

    DISPDBG((1,"Mirror Driver DrvStretchBlt: \n"));
    return TRUE;
}

BOOL DrvStretchBltROP(
SURFOBJ         *psoTrg,
SURFOBJ         *psoSrc,
SURFOBJ         *psoMask,
CLIPOBJ         *pco,
XLATEOBJ        *pxlo,
COLORADJUSTMENT *pca,
POINTL          *pptlBrushOrg,
RECTL           *prclTrg,
RECTL           *prclSrc,
POINTL          *pptlMask,
ULONG            iMode,
BRUSHOBJ        *pbo,
ROP4            rop4)
{
    UNREFERENCED_PARAMETER(psoTrg);
    UNREFERENCED_PARAMETER(psoSrc);
    UNREFERENCED_PARAMETER(psoMask);
    UNREFERENCED_PARAMETER(pco);
    UNREFERENCED_PARAMETER(pxlo);
    UNREFERENCED_PARAMETER(pca);
    UNREFERENCED_PARAMETER(pptlBrushOrg);
    UNREFERENCED_PARAMETER(prclTrg);
    UNREFERENCED_PARAMETER(prclSrc);
    UNREFERENCED_PARAMETER(pptlMask);
    UNREFERENCED_PARAMETER(iMode);
    UNREFERENCED_PARAMETER(pbo);
    UNREFERENCED_PARAMETER(rop4);

    DISPDBG((1,"Mirror Driver DrvStretchBltROP: \n"));
    return TRUE; 
}

BOOL DrvPlgBlt(
SURFOBJ         *psoTrg,
SURFOBJ         *psoSrc,
SURFOBJ         *psoMsk,
CLIPOBJ         *pco,
XLATEOBJ        *pxlo,
COLORADJUSTMENT *pca,
POINTL          *pptlBrushOrg,
POINTFIX        *pptfx,
RECTL           *prcl,
POINTL          *pptl,
ULONG            iMode)
{
    UNREFERENCED_PARAMETER(psoTrg);
    UNREFERENCED_PARAMETER(psoSrc);
    UNREFERENCED_PARAMETER(psoMsk);
    UNREFERENCED_PARAMETER(pco);
    UNREFERENCED_PARAMETER(pxlo);
    UNREFERENCED_PARAMETER(pca);
    UNREFERENCED_PARAMETER(pptlBrushOrg);
    UNREFERENCED_PARAMETER(pptfx);
    UNREFERENCED_PARAMETER(prcl);
    UNREFERENCED_PARAMETER(pptl);
    UNREFERENCED_PARAMETER(iMode);

    DISPDBG((1,"Mirror Driver DrvPlgBlt: \n"));
    return TRUE;
}

HBITMAP DrvCreateDeviceBitmap(
   IN DHPDEV dhpdev,
   IN SIZEL sizl,
   IN ULONG iFormat
   )
{
   MIRRSURF *mirrsurf;
   ULONG mirrorsize;
   DHSURF dhsurf;
   ULONG stride;
   HSURF hsurf;

   PPDEV ppdev = (PPDEV) dhpdev;
   
   DISPDBG((1,"CreateDeviceBitmap:\n"));
   
   if (iFormat == BMF_1BPP || iFormat == BMF_4BPP)
   {
      return NULL;
   }

   // DWORD align each stride
   stride = (sizl.cx*(iFormat/8)+3);
   stride -= stride % 4;
   
   mirrorsize = (int)(sizeof(MIRRSURF) + stride * sizl.cy);

   mirrsurf = (MIRRSURF *) EngAllocMem(FL_ZERO_MEMORY,
                                       mirrorsize,
                                       0x4D495252);
   if (!mirrsurf) {
        RIP("DISP DrvCreateDeviceBitmap failed EngAllocMem\n");
        return(FALSE);
   }
                                       
   dhsurf = (DHSURF) mirrsurf;

   hsurf = (HSURF) EngCreateDeviceBitmap(dhsurf,
                                 sizl,
                                 iFormat);

   if (hsurf == (HSURF) 0)
   {
       RIP("DISP DrvCreateDeviceBitmap failed EngCreateBitmap\n");
       return(FALSE);
   }

   if (!EngAssociateSurface(hsurf, 
                            ppdev->hdevEng, 
                            flGlobalHooks))
   {
       RIP("DISP DrvCreateDeviceBitmap failed EngAssociateSurface\n");
       EngDeleteSurface(hsurf);
       return(FALSE);
   }
  
   mirrsurf->cx = sizl.cx;
   mirrsurf->cy = sizl.cy;
   mirrsurf->lDelta = stride;
   mirrsurf->ulBitCount = iFormat;
   mirrsurf->bIsScreen = FALSE;
  
   return((HBITMAP)hsurf);
}

VOID DrvDeleteDeviceBitmap(
   IN DHSURF dhsurf
   )
{
   MIRRSURF *mirrsurf;
   
   DISPDBG((1, "DeleteDeviceBitmap:\n"));

   mirrsurf = (MIRRSURF *) dhsurf;

   EngFreeMem((PVOID) mirrsurf);
}

#if (NTDDI_VERSION >= NTDDI_VISTA)
LONG
DrvRenderHint(DHPDEV dhpdev,
              ULONG  NotifyCode,
              SIZE_T Length,
              PVOID  Data)
{
    PPDEV ppdev = (PPDEV) dhpdev;
    PDRH_APIBITMAPDATA pData = (PDRH_APIBITMAPDATA)Data;

    UNREFERENCED_PARAMETER(ppdev);

    if (NotifyCode == DRH_APIBITMAP && Length && Data)
    {
        DISPDBG((1, "DrvRenderHint(API Render: %08x, %lx)\n", pData->pso, pData->b));
    }

    return TRUE;
}
#endif

/******************************Public*Routine******************************\
* DrvAssertMode
*
* Enable/Disable the given device.
*
\**************************************************************************/

DrvAssertMode(DHPDEV  dhpdev,
              BOOL    bEnable)
{
    PPDEV ppdev = (PPDEV) dhpdev;

    UNREFERENCED_PARAMETER(bEnable);
    UNREFERENCED_PARAMETER(ppdev);

    DISPDBG((0, "DrvAssertMode(%lx, %lx)\n", dhpdev, bEnable));

    return TRUE;

}// DrvAssertMode()

/******************************Public*Routine******************************\
* DrvEscape
*
* We only handle WNDOBJ_SETUP escape. 
*
\**************************************************************************/

typedef struct _WndObjENUMRECTS
{
  ULONG c;
  RECTL arcl[100];
} WNDOBJENUMRECTS;

VOID
vDumpWndObjRgn(WNDOBJ *pwo)
{
    ULONG ulRet;

    ulRet = WNDOBJ_cEnumStart(pwo, CT_RECTANGLES, CD_RIGHTDOWN, 100);

    if (ulRet != 0xFFFFFFFF)
    {
        BOOL bMore;
        ULONG i;
        WNDOBJENUMRECTS enumRects;

        do
        {
          bMore = WNDOBJ_bEnum(pwo, sizeof(enumRects), &enumRects.c);

          for (i = 0; i < enumRects.c; i++)
          {
              DISPDBG((0,"\nWNDOBJ_rect:[%d,%d][%d,%d]",
                          enumRects.arcl[i].left,
                          enumRects.arcl[i].top,
                          enumRects.arcl[i].right,
                          enumRects.arcl[i].bottom));

          }
        } while (bMore);
    }
}

VOID
WndObjCallback(WNDOBJ *pwo,
               FLONG fl)
{
#if (NTDDI_VERSION < NTDDI_VISTA)
    UNREFERENCED_PARAMETER(pwo);
#endif

    if (fl & (WOC_RGN_CLIENT_DELTA |
              WOC_RGN_CLIENT |
              WOC_RGN_SURFACE_DELTA |
              WOC_RGN_SURFACE |
              WOC_CHANGED |
              WOC_DELETE |
              WOC_DRAWN |
              WOC_SPRITE_OVERLAP |
              WOC_SPRITE_NO_OVERLAP
#if (NTDDI_VERSION >= NTDDI_VISTA)
              | WOC_RGN_SPRITE
#endif 
              ))
    {
        DISPDBG((0,"WndObjCallback: "));

        if (fl & WOC_RGN_CLIENT_DELTA) 
            DISPDBG((0,"WOC_RGN_CLIENT_DELTA "));
        if (fl & WOC_RGN_CLIENT) 
            DISPDBG((0,"WOC_RGN_CLIENT "));
        if (fl & WOC_RGN_SURFACE_DELTA) 
            DISPDBG((1,"WOC_RGN_SURFACE_DELTA "));
        if (fl & WOC_RGN_SURFACE) 
            DISPDBG((1,"WOC_RGN_SURFACE "));
        if (fl & WOC_CHANGED) 
            DISPDBG((1,"WOC_CHANGED "));
        if (fl & WOC_DELETE) 
            DISPDBG((0,"WOC_DELETE "));
        if (fl & WOC_DRAWN) 
            DISPDBG((1,"WOC_DRAWN "));
        if (fl & WOC_SPRITE_OVERLAP) 
            DISPDBG((0,"WOC_SPRITE_OVERLAP "));
        if (fl & WOC_SPRITE_NO_OVERLAP)
            DISPDBG((0,"WOC_SPRITE_NO_OVERLAP "));
#if (NTDDI_VERSION >= NTDDI_VISTA)
        if (fl & WOC_RGN_SPRITE)
        {
            DISPDBG((0,"WOC_RGN_SPRITE "));
            vDumpWndObjRgn(pwo);
        }
#endif
        DISPDBG((0,"\n"));
    }
}

ULONG
DrvEscape(SURFOBJ *pso,
          ULONG iEsc,
          ULONG cjIn,
          PVOID pvIn,
          ULONG cjOut,
          PVOID pvOut)
{
    ULONG ulRet = 0;

    UNREFERENCED_PARAMETER(cjIn);
    UNREFERENCED_PARAMETER(pvIn);
    UNREFERENCED_PARAMETER(cjOut);
    UNREFERENCED_PARAMETER(pvOut);

    if (pso->dhsurf)
    {

        if (iEsc == WNDOBJ_SETUP)
        {
            WNDOBJ *pwo = NULL;

            DISPDBG((0,"Attempt to create WndObj\n"));

            pwo = EngCreateWnd(pso,
                               *(HWND*)pvIn,
                               WndObjCallback,
                               WO_DRAW_NOTIFY |
                               WO_RGN_CLIENT |
                               WO_RGN_CLIENT_DELTA |
                               WO_RGN_WINDOW |
                               WO_SPRITE_NOTIFY
#if (NTDDI_VERSION >= NTDDI_VISTA)
                               | WO_RGN_SPRITE
#endif
                               ,
                               0);
            if (pwo != NULL)
            {
                DISPDBG((0,"WndObj creat success\n"));
                ulRet = 1;
            }
        }
    }

    return ulRet;
}

