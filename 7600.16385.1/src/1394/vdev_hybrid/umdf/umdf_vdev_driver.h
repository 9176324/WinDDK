/*++

Copyright (C) Microsoft Corporation, All Rights Reserved

Module Name:

    Driver.h

Abstract:

    This module contains the type definitions for the UMDF Skeleton sample's
    driver callback class.

Environment:

    Windows User-Mode Driver Framework (WUDF)


--*/

#pragma once

//
// This class handles driver events for the skeleton sample.  In particular
// it supports the OnDeviceAdd event, which occurs when the driver is called
// to setup per-device handlers for a new device stack.
//

class CUmdfVDev : public CUnknown, public IDriverEntry
{
//
// Private data members.
//
private:

//
// Private methods.
//
private:

    //
    // Returns a refernced pointer to the IDriverEntry interface.
    //

    IDriverEntry *
    QueryIDriverEntry (VOID)
    {
        AddRef();
        return static_cast<IDriverEntry*>(this);
    }

    HRESULT
    Initialize (VOID);

//
// Public methods
//
public:

    //
    // The factory method used to create an instance of this driver.
    //
    
    static
    HRESULT
    CreateInstance (
        __out PCUmdfVDev *Driver);

//
// COM methods
//
public:

    //
    // IDriverEntry methods
    //

    virtual
    HRESULT
    STDMETHODCALLTYPE
    OnInitialize (
        __in IWDFDriver *FxWdfDriver)
    {
        UNREFERENCED_PARAMETER(FxWdfDriver);
        
        return S_OK;
    }

    virtual
    HRESULT
    STDMETHODCALLTYPE
    OnDeviceAdd (
        __in IWDFDriver *FxWdfDriver,
        __in IWDFDeviceInitialize *FxDeviceInit);

    virtual
    VOID
    STDMETHODCALLTYPE
    OnDeinitialize (
        __in IWDFDriver *FxWdfDriver)
    {
        UNREFERENCED_PARAMETER(FxWdfDriver);

        return;
    }

    //
    // IUnknown methods.
    //
    // We have to implement basic ones here that redirect to the 
    // base class becuase of the multiple inheritance.
    //

    virtual
    ULONG
    STDMETHODCALLTYPE
    AddRef (VOID)
    {
        return __super::AddRef();
    }
    
    __drv_arg(this, __drv_freesMem(object))
    virtual
    ULONG
    STDMETHODCALLTYPE
    Release (VOID)
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

