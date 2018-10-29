/*++

Copyright (C) Microsoft Corporation, All Rights Reserved

Module Name:

    Device.h

Abstract:

    This module contains the type definitions for the UMDF Skeleton sample
    driver's device callback class.

Environment:

    Windows User-Mode Driver Framework (WUDF)

--*/

#pragma once

//
// Class for the iotrace driver.
//

class ATL_NO_VTABLE CMyDevice : 
    public CComObjectRootEx<CComMultiThreadModel>,
    public IFileCallbackCleanup,
    public IFileCallbackClose,
    public IObjectCleanup
{
public:

DECLARE_NOT_AGGREGATABLE(CMyDevice)

BEGIN_COM_MAP(CMyDevice)
    COM_INTERFACE_ENTRY(IFileCallbackCleanup)
    COM_INTERFACE_ENTRY(IFileCallbackClose)
    COM_INTERFACE_ENTRY(IObjectCleanup)
END_COM_MAP()

public:
    
    //IFileCallbackCleanup
    STDMETHOD_(void,OnCleanupFile)(__in IWDFFile* pWdfFileObject);
    //IFileCallbackClose
    STDMETHOD_(void,OnCloseFile)(__in IWDFFile* pWdfFileObject);
    //IObjectCleanup
    STDMETHOD_(void,OnCleanup)(__in IWDFObject* pWdfObject);

public:

    STDMETHOD(Initialize)(__in IWDFDriver* pWdfDriver, __in IWDFDeviceInitialize* pWdfDeviceInit);
    
    HRESULT
    Configure(
        );

    IWDFDevice *
    GetFxDevice(
        )
    {
        return m_FxDevice;
    }

private:
    CComPtr<IWDFDevice> m_FxDevice;
    HRESULT  ReadAndAssignPropertyStoreValue();
    HRESULT GetAnsiValFromPropVariant(__in PROPVARIANT val, __inout LPSTR *PropertyValueA);
    
};

