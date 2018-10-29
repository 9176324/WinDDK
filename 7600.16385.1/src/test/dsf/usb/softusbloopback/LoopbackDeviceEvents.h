/*++ BUILD Version 0000

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    LoopbackDeviceEvents.h

Abstract:
    Definition of the template class which implements the
    event interface ILoopbackDeviceEvents

--*/


template<class T>
class CProxy_ILoopbackDeviceEvents :
    public IConnectionPointImpl<T, &__uuidof(ILoopbackDeviceEvents)>
{
public:
    HRESULT _stdcall Fire_ContinueToPoll(VARIANT_BOOL *pfvarContinue)
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
                hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);

                //Set the return parameter
                *pfvarContinue  = varResult.boolVal;
            }
        }
        return hr;
    }

    HRESULT _stdcall Fire_ContinueEventProcessing(VARIANT_BOOL *pfvarContinue)
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
                hr = pConnection->Invoke(2, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);

                //Set the return parameter
                *pfvarContinue  = varResult.boolVal;
            }
        }
        return hr;
    }


};

