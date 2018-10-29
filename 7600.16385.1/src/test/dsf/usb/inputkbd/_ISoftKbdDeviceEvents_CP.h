/*++ BUILD Version: 0000    

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    _ISoftKdbDeviceEvents_cp.h

Abstract:
    Implements the event source for the inteface ISoftInputKbdDeviceEvents
 
Revision History:

--*/
#pragma once

template <class T>
class CProxy_ISoftInputKbdDeviceEvents : public IConnectionPointImpl<T, &__uuidof( ISoftInputKbdDeviceEvents ), CComDynamicUnkArray>
{

public:
    HRESULT _stdcall Fire_KeyTouched(void)
    {
        HRESULT hr = S_OK;
        T * pThis = static_cast<T *>(this);
        int cConnections = m_vec.GetSize();
        ISoftInputKbdDeviceEvents  *pConnection = NULL;


        for (int iConnection = 0; iConnection < cConnections; iConnection++)
        {
            pThis->Lock();
            CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
            pThis->Unlock();

            //Check that this connection point is valid
            if (punkConnection)
            {
                hr = punkConnection.QueryInterface(&pConnection);
            
                IfFailHrGo(hr);

                //Call the function which handles the event
                hr = pConnection->KeyTouched();
            }

            //Release the interface
            RELEASE(pConnection);
        }
Exit:
        RELEASE(pConnection);
        return hr;
    }

};

