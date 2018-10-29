/**************************************************************************
*
*  Copyright (c) 2003  Microsoft Corporation
*
*  Title: wiadriver.h
*
*  Description: This contains the WIA driver class definition and needed
*               defines.
*
***************************************************************************/

#pragma once

#define MY_WIA_ERROR_HANDLING_PROP         WIA_PRIVATE_ITEMPROP
#define MY_WIA_ERROR_HANDLING_PROP_STR     L"My error handling property"

#define ERROR_HANDLING_NONE                0x00000000
#define ERROR_HANDLING_WARMING_UP          0x00000001
#define ERROR_HANDLING_COVER_OPEN          0x00000002
#define ERROR_HANDLING_PRIVATE_ERROR       0x00000004
#define ERROR_HANDLING_UNHANDLED_STATUS    0x00000008
#define ERROR_HANDLING_UNHANDLED_ERROR     0x00000010

//
// The only purpose of the MY_TEST_FILTER_PROP property is to illustrate
// the IWiaImageFilter::ApplyProperties method. It is never used by the 
// driver itself.
//
#define MY_TEST_FILTER_PROP                WIA_PRIVATE_ITEMPROP+1
#define MY_TEST_FILTER_PROP_STR            L"My test filter property"

#define REG_ENTRY_DEVICEDATA            TEXT("DeviceData")
#define REG_ENTRY_STORAGEPATH           TEXT("StoragePath")

#define WIA_DRIVER_ROOT_NAME            L"Root"    // THIS SHOULD NOT BE LOCALIZED
#define WIA_DRIVER_FLATBED_NAME         L"Flatbed" // THIS SHOULD NOT BE LOCALIZED
#define WIA_DRIVER_FEEDER_NAME          L"Feeder"  // THIS SHOULD NOT BE LOCALIZED
#define WIA_DRIVER_FILM_NAME            L"Film"    // THIS SHOULD NOT BE LOCALIZED
#define WIA_DRIVER_STORAGE_NAME         L"Storage" // THIS SHOULD NOT BE LOCALIZED

#define DEFAULT_LOCK_TIMEOUT            1000
#define DEFAULT_NUM_DRIVER_EVENTS       2
#define DEFAULT_NUM_DRIVER_COMMANDS     0
#define DEFAULT_NUM_DRIVER_FORMATS      2

typedef struct _WIA_DRIVER_ITEM_CONTEXT
{
    GUID    guidItemCategory;
    LONG    lNumItemsStored;
    BSTR    bstrStorageDataPath;
    ULONG   ulFeederTransferCount;
}WIA_DRIVER_ITEM_CONTEXT,*PWIA_DRIVER_ITEM_CONTEXT;

class INonDelegatingUnknown {
public:
    virtual STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,LPVOID *ppvObj) = 0;
    virtual STDMETHODIMP_(ULONG) NonDelegatingAddRef() = 0;
    virtual STDMETHODIMP_(ULONG) NonDelegatingRelease() = 0;
};

class CWIADriver : public INonDelegatingUnknown, // NonDelegatingUnknown
                   public IStiUSD,               // STI USD interface
                   public IWiaMiniDrv            // WIA Minidriver interface
{
public:

    ///////////////////////////////////////////////////////////////////////////
    // Construction/Destruction Section
    ///////////////////////////////////////////////////////////////////////////

    CWIADriver(__in_opt LPUNKNOWN punkOuter);
    ~CWIADriver();

private:

    ///////////////////////////////////////////////////////////////////////////
    // WIA driver internals
    ///////////////////////////////////////////////////////////////////////////

    LONG                    m_cRef;                     // Device object reference count.
    LPUNKNOWN               m_punkOuter;                // Pointer to outer unknown.
    IStiDevice             *m_pIStiDevice;              // STI device interface for locking
    IWiaDrvItem            *m_pIDrvItemRoot;            // WIA root item
    LONG                    m_lClientsConnected;        // number of applications connected
    CWIACapabilityManager   m_CapabilityManager;        // WIA driver capabilities
    WIA_FORMAT_INFO        *m_pFormats;                 // WIA format information
    ULONG                   m_ulNumFormats;             // number of data formats
    BSTR                    m_bstrDeviceID;             // WIA device ID;
    ULONG_PTR               m_ulImageLibraryToken;      // GDI plus token
    WiaDevice               m_WiaDevice;                // Simulated device object
    WCHAR                   m_wszStoragePath[MAX_PATH]; // WIA storage path
    BSTR                    m_bstrRootFullItemName;     // WIA root item (full item name)

public:

    ///////////////////////////////////////////////////////////////////////////
    // Standard COM Section
    ///////////////////////////////////////////////////////////////////////////

    STDMETHODIMP QueryInterface(__in REFIID riid, __out LPVOID * ppvObj);

    STDMETHODIMP_(ULONG) AddRef();

    STDMETHODIMP_(ULONG) Release();

    ///////////////////////////////////////////////////////////////////////////
    // IStiUSD Interface Section (for all WIA drivers)
    ///////////////////////////////////////////////////////////////////////////

    STDMETHOD(Initialize)(THIS_
                          __in  PSTIDEVICECONTROL pHelDcb,
                                DWORD             dwStiVersion,
                          __in  HKEY              hParametersKey);

    STDMETHOD(GetCapabilities)(THIS_ __out PSTI_USD_CAPS pDevCaps);

    STDMETHOD(GetStatus)(THIS_ __inout PSTI_DEVICE_STATUS pDevStatus);

    STDMETHOD(DeviceReset)(THIS);

    STDMETHOD(Diagnostic)(THIS_ __out LPDIAG pBuffer);

    STDMETHOD(Escape)(THIS_
                                                    STI_RAW_CONTROL_CODE EscapeFunction,
                      __in_bcount(cbInDataSize)     LPVOID               lpInData,
                                                    DWORD                cbInDataSize,
                      __out_bcount(dwOutDataSize)   LPVOID               pOutData,
                                                    DWORD                dwOutDataSize,
                      __out                         LPDWORD              pdwActualData);

    STDMETHOD(GetLastError)(THIS_ __out LPDWORD pdwLastDeviceError);

    STDMETHOD(LockDevice)();

    STDMETHOD(UnLockDevice)();

    STDMETHOD(RawReadData)(THIS_
                           __out_bcount(*lpdwNumberOfBytes)   LPVOID       lpBuffer,
                           __out                              LPDWORD      lpdwNumberOfBytes,
                           __out                              LPOVERLAPPED lpOverlapped);

    STDMETHOD(RawWriteData)(THIS_
                            __in_bcount(dwNumberOfBytes) LPVOID       lpBuffer,
                                                         DWORD        dwNumberOfBytes,
                            __out                        LPOVERLAPPED lpOverlapped);

    STDMETHOD(RawReadCommand)(THIS_
                              __out_bcount(*lpdwNumberOfBytes)    LPVOID       lpBuffer,
                              __out                               LPDWORD      lpdwNumberOfBytes,
                              __out                               LPOVERLAPPED lpOverlapped);

    STDMETHOD(RawWriteCommand)(THIS_
                               __in_bcount(dwNumberOfBytes)  LPVOID       lpBuffer,
                                                             DWORD        dwNumberOfBytes,
                               __out                         LPOVERLAPPED lpOverlapped);

    STDMETHOD(SetNotificationHandle)(THIS_ __in HANDLE hEvent);

    STDMETHOD(GetNotificationData)(THIS_ __in LPSTINOTIFY lpNotify);

    STDMETHOD(GetLastErrorInfo)(THIS_ STI_ERROR_INFO *pLastErrorInfo);

    /////////////////////////////////////////////////////////////////////////
    // IWiaMiniDrv Interface Section (for all WIA drivers)                 //
    /////////////////////////////////////////////////////////////////////////

    STDMETHOD(drvInitializeWia)(THIS_
                                __inout BYTE        *pWiasContext,
                                        LONG        lFlags,
                                __in    BSTR        bstrDeviceID,
                                __in    BSTR        bstrRootFullItemName,
                                __in    IUnknown    *pStiDevice,
                                __in    IUnknown    *pIUnknownOuter,
                                __out   IWiaDrvItem **ppIDrvItemRoot,
                                __out   IUnknown    **ppIUnknownInner,
                                __out   LONG        *plDevErrVal);

    STDMETHOD(drvAcquireItemData)(THIS_
                                  __in      BYTE                      *pWiasContext,
                                            LONG                      lFlags,
                                  __in      PMINIDRV_TRANSFER_CONTEXT pmdtc,
                                  __out     LONG                      *plDevErrVal);

    STDMETHOD(drvInitItemProperties)(THIS_
                                     __inout    BYTE *pWiasContext,
                                                LONG lFlags,
                                     __out      LONG *plDevErrVal);

    STDMETHOD(drvValidateItemProperties)(THIS_
                                         __inout    BYTE           *pWiasContext,
                                                    LONG           lFlags,
                                                    ULONG          nPropSpec,
                                         __in       const PROPSPEC *pPropSpec,
                                         __out      LONG           *plDevErrVal);

    STDMETHOD(drvWriteItemProperties)(THIS_
                                      __inout   BYTE                      *pWiasContext,
                                                LONG                      lFlags,
                                      __in      PMINIDRV_TRANSFER_CONTEXT pmdtc,
                                      __out     LONG                      *plDevErrVal);

    STDMETHOD(drvReadItemProperties)(THIS_
                                     __in       BYTE           *pWiasContext,
                                                LONG           lFlags,
                                                ULONG          nPropSpec,
                                     __in       const PROPSPEC *pPropSpec,
                                     __out      LONG           *plDevErrVal);

    STDMETHOD(drvLockWiaDevice)(THIS_
                                __in    BYTE *pWiasContext,
                                        LONG lFlags,
                                __out   LONG *plDevErrVal);

    STDMETHOD(drvUnLockWiaDevice)(THIS_
                                  __in      BYTE *pWiasContext,
                                            LONG lFlags,
                                  __out     LONG *plDevErrVal);

    STDMETHOD(drvAnalyzeItem)(THIS_
                              __in      BYTE *pWiasContext,
                                        LONG lFlags,
                              __out     LONG *plDevErrVal);

    STDMETHOD(drvGetDeviceErrorStr)(THIS_
                                              LONG     lFlags,
                                              LONG     lDevErrVal,
                                    __out     LPOLESTR *ppszDevErrStr,
                                    __out     LONG     *plDevErr);

    STDMETHOD(drvDeviceCommand)(THIS_
                                __inout     BYTE            *pWiasContext,
                                            LONG            lFlags,
                                __in        const GUID      *plCommand,
                                __out       IWiaDrvItem     **ppWiaDrvItem,
                                __out       LONG            *plDevErrVal);

    STDMETHOD(drvGetCapabilities)(THIS_
                                  __in      BYTE            *pWiasContext,
                                            LONG            ulFlags,
                                  __out     LONG            *pcelt,
                                  __out     WIA_DEV_CAP_DRV **ppCapabilities,
                                  __out     LONG            *plDevErrVal);

    STDMETHOD(drvDeleteItem)(THIS_
                             __inout    BYTE *pWiasContext,
                                        LONG lFlags,
                             __out      LONG *plDevErrVal);

    STDMETHOD(drvFreeDrvItemContext)(THIS_
                                     LONG lFlags,
                                     __in       BYTE *pSpecContext,
                                     __out      LONG *plDevErrVal);

    STDMETHOD(drvGetWiaFormatInfo)(THIS_
                                   __in     BYTE            *pWiasContext,
                                            LONG            lFlags,
                                   __out    LONG            *pcelt,
                                   __out    WIA_FORMAT_INFO **ppwfi,
                                   __out    LONG            *plDevErrVal);

    STDMETHOD(drvNotifyPnpEvent)(THIS_
                                 __in   const GUID *pEventGUID,
                                 __in   BSTR       bstrDeviceID,
                                        ULONG      ulReserved);

    STDMETHOD(drvUnInitializeWia)(THIS_ __inout BYTE *pWiasContext);

public:

    /////////////////////////////////////////////////////////////////////////
    // INonDelegating Interface Section (for all WIA drivers)              //
    /////////////////////////////////////////////////////////////////////////

    STDMETHODIMP NonDelegatingQueryInterface(REFIID  riid,LPVOID  *ppvObj);
    STDMETHODIMP_(ULONG) NonDelegatingAddRef();
    STDMETHODIMP_(ULONG) NonDelegatingRelease();

private:

    /////////////////////////////////////////////////////////////////////////
    // Minidriver private methods specific Section                         //
    /////////////////////////////////////////////////////////////////////////

    UINT GetBitmapResourceIDFromCategory(const GUID &guidItemCategory);

    HRESULT DownloadToStream(           LONG                           lFlags,
                             __in       BYTE                           *pWiasContext,
                             __in       PMINIDRV_TRANSFER_CONTEXT      pmdtc,
                                        const GUID                     &guidItemCategory,
                                        const GUID                     &guidFormatID,
                             __callback IWiaMiniDrvTransferCallback    *pTransferCallback,
                             __out      LONG                           *plDevErrVal);

    HRESULT DownloadRawHeader(__in      IStream                       *pDestination,
                              __inout   BYTE                          *pWiasContext,
                              __in      PMINIDRV_TRANSFER_CONTEXT     pmdtc);

    HRESULT UploadFromStream(           LONG                           lFlags,
                             __in       BYTE                           *pWiasContext,
                                        const GUID                     &guidItemCategory,
                             __callback IWiaMiniDrvTransferCallback    *pTransferCallback,
                             __out      LONG                           *plDevErrVal);

    HRESULT LegacyDownload(LONG                      lFlags,
                           BYTE                      *pWiasContext,
                           const GUID                &guidItemCategory,
                           PMINIDRV_TRANSFER_CONTEXT pmdtc,
                           LONG                      *plDevErrVal);

    HRESULT BuildDriverItemTree();

    HRESULT DestroyDriverItemTree();

    HRESULT DoSynchronizeCommand(__inout BYTE *pWiasContext);

};

