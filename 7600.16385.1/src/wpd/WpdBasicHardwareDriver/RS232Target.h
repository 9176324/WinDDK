#pragma once

class RS232Target :
    public IRequestCallbackRequestCompletion
{
public:    
    RS232Target();

    ~RS232Target();    

    HRESULT Create(WpdBaseDriver* pBaseDriver,
                   IWDFDevice*    pDevice, 
                   HANDLE         hRS232Port);

    void    Delete();

    HRESULT Start();    

    HRESULT Stop();

    BOOL    IsReady();

    WDF_IO_TARGET_STATE GetState();

    HRESULT SendReadRequest();

    HRESULT SendWriteRequest(BYTE*   pBuffer,
                             size_t  cbBufferSize);

public: // IUnknown
    ULONG __stdcall AddRef();
    ULONG __stdcall Release();
    HRESULT __stdcall QueryInterface(REFIID riid, void** ppv);

    //
    // IRequestCallbackRequestCompletion
    //
    STDMETHOD_ (void, OnCompletion)(IWDFIoRequest*                 pWdfRequest,
                                    IWDFIoTarget*                  pIoTarget,
                                    IWDFRequestCompletionParams*   pParams,
                                    PVOID                          pContext);

private:
    ULONG                          m_cRef;

    WpdBaseDriver*                 m_pBaseDriver;

    // Ensure ample buffer size
    BYTE                           m_pReadBuffer[MAX_AMOUNT_TO_READ*2];
 
    CComPtr<IWDFDevice>            m_pWDFDevice;
    CComPtr<IWDFIoTarget>          m_pFileTarget;
};
