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
#include "internal.h"

//
// Class for the iotrace driver.
//

class CMyDevice : 
    public CUnknown
{

//
// Private data members.
//
private:
    //
    // Weak reference to framework device
    //

    IWDFDevice *m_FxDevice;

//
// Private methods.
//

private:

    CMyDevice(
        VOID
        ) :
        m_FxDevice(NULL)
    {
    }

    HRESULT
    Initialize(
        __in IWDFDriver *FxDriver,
        __in IWDFDeviceInitialize *FxDeviceInit
        );


//
// Public methods
//
public:

    //
    // The factory method used to create an instance of this driver.
    //
    
    static
    HRESULT
    CreateInstance(
        __in IWDFDriver *FxDriver,
        __in IWDFDeviceInitialize *FxDeviceInit,
        __out PCMyDevice *Device
        );

    IWDFDevice *
    GetFxDevice(
        VOID
        )
    {
        return m_FxDevice;
    }

    HRESULT
    Configure(
        VOID
        );

//
// COM methods
//
public:

    //
    // IUnknown methods.
    //

    virtual
    ULONG
    STDMETHODCALLTYPE
    AddRef(
        VOID
        )
    {
        return __super::AddRef();
    }

    __drv_arg(this, __drv_freesMem(object))
    virtual
    ULONG
    STDMETHODCALLTYPE
    Release(
        VOID
       )
    {
        return __super::Release();
    }

    virtual
    HRESULT
    STDMETHODCALLTYPE
    QueryInterface(
        __in REFIID InterfaceId,
        __deref_out PVOID *Object
        );
};

