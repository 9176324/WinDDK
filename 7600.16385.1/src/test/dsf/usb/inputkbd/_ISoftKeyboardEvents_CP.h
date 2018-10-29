/*++ BUILD Version: 0000    

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    _ISoftKeyboardEvents_cp.h

Abstract:
    Implements the event source for the inteface ISoftKeyboardEvents
 
Revision History:

--*/
#pragma once

template <class T>
class CProxy_ISoftKeyboardEvents : public IConnectionPointImpl<T, &__uuidof( ISoftKeyboardEvents ), CComDynamicUnkArray>
{
    public:
        HRESULT _stdcall Fire_IndicatorOn(INDICATOR_KEY LedKey)
        {
            HRESULT hr = S_OK;
            T * pThis = static_cast<T *>(this);
            int cConnections = m_vec.GetSize();


            for (int iConnection = 0; iConnection < cConnections; iConnection++)
            {
                pThis->Lock();
                CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
                pThis->Unlock();

                IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

                if (pConnection)
                {
                    CComVariant varResult;
                    CComVariant varParam; ::VariantInit(&varParam);

                    varParam.vt = VT_I4;
                    varParam.lVal = LedKey;

                    DISPPARAMS params = {&varParam, NULL, 1, 0 };
                    hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
                }
            }
            return hr;

        }

        HRESULT _stdcall Fire_IndicatorOff(INDICATOR_KEY LedKey)
        {
            HRESULT hr = S_OK;
            T * pThis = static_cast<T *>(this);
            int cConnections = m_vec.GetSize();


            for (int iConnection = 0; iConnection < cConnections; iConnection++)
            {
                pThis->Lock();
                CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
                pThis->Unlock();

                IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

                if (pConnection)
                {
                    CComVariant varResult;
                    CComVariant varParam; ::VariantInit(&varParam);

                    varParam.vt = VT_I4;
                    varParam.lVal = LedKey;

                    DISPPARAMS params = {&varParam, NULL, 1, 0 };
                    hr = pConnection->Invoke(2, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
                }
            }
            return hr;
        }

        HRESULT _stdcall Fire_BufferOverflow(void)
        {
            HRESULT hr = S_OK;
            T * pThis = static_cast<T *>(this);
            int cConnections = m_vec.GetSize();


            for (int iConnection = 0; iConnection < cConnections; iConnection++)
            {
                pThis->Lock();
                CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
                pThis->Unlock();

                IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

                if (pConnection)
                {
                    CComVariant varResult;

                    DISPPARAMS params = { NULL, NULL, 0, 0 };
                    hr = pConnection->Invoke(3, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
                }
            }
            return hr;
        }
};

