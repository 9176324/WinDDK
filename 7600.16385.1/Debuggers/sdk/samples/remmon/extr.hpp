//----------------------------------------------------------------------------
//
// Simple remote dbgeng extension host.
//
// Copyright (C) Microsoft Corporation, 2006.
//
//----------------------------------------------------------------------------

#ifndef __EXTR_HPP__
#define __EXTR_HPP__

class Extension
{
public:
    Extension(void);
    ~Extension(void);
    
    HRESULT Load(__in PCSTR File);
    void Unload(void);

    void NotifyAccessible(void);
    
    HRESULT Call(__in PDEBUG_CLIENT Client,
                 __in PCSTR Command,
                 __in PCSTR Args,
                 __out HRESULT* CommandStatus);
    
protected:
    HINSTANCE m_Dll;
    ULONG m_Version;
    ULONG m_InitFlags;
    PDEBUG_EXTENSION_INITIALIZE m_Init;
    PDEBUG_EXTENSION_UNINITIALIZE m_Uninit;
    PDEBUG_EXTENSION_NOTIFY m_Notify;
};

#endif // #ifndef __EXTR_HPP__
