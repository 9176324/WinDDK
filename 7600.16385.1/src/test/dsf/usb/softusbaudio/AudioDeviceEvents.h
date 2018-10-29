/*++ BUILD Version 0000

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    AudioDeviceEvents.h

Abstract:
    Definition of the template class which implements the
    event interface IAudioDeviceEvents

--*/


template<class T>
class CProxy_IAudioDeviceEvents :
    public IConnectionPointImpl<T, &__uuidof(IAudioDeviceEvents)>
{
public:
    HRESULT _stdcall Fire_ContinueSimulating(VARIANT_BOOL *pfvarContinue)
    {
        HRESULT hr = S_OK;
        T * pThis = static_cast<T *>(this);
        int cConnections = m_vec.GetSize();

        *pfvarContinue = VARIANT_TRUE;
        
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
                if (VT_BOOL == varResult.vt)
                {
                    *pfvarContinue  = varResult.boolVal;
                }
            }
        }
        return hr;
    }
};

