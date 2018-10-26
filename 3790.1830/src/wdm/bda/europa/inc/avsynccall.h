//////////////////////////////////////////////////////////////////////////////
//
//                     (C) Philips Semiconductors 2000
//  All rights are reserved. Reproduction in whole or in part is prohibited
//  without the written consent of the copyright owner.
//
//  Philips reserves the right to make changes without notice at any time.
//  Philips makes no warranty, expressed, implied or statutory, including but
//  not limited to any implied warranty of merchantability or fitness for any
//  particular purpose, or that the use will not infringe any third party
//  patent, copyright or trademark. Philips must not be liable for any loss
//  or damage arising from its use.
//
//////////////////////////////////////////////////////////////////////////////

#pragma once    // Specifies that the file, in which the pragma resides, will
                // be included (opened) only once by the compiler in a
                // build.

//////////////////////////////////////////////////////////////////////////////
//
// Description:
//
//  This class contains the implementation of the sync call. It is derived
//  from the virtual class COSDependSyncCall.
//
//////////////////////////////////////////////////////////////////////////////
class CAVSyncCall : public COSDependSyncCall
{
// Construction
public:
                CAVSyncCall(PKINTERRUPT pIntObject);

// Implementation
public:
    BOOL        SyncCall(VAMP_SYNC_FN pfn, PVOID pContext);
    void        InsertIRQObject(PKINTERRUPT pIntObject);
    void        RemoveIRQObject();

// Attributes
private:
    PKINTERRUPT m_pIntObject;
};
