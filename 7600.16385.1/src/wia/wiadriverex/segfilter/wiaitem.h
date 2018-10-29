#pragma once

#define MIN_PROPID 2

class CWiaItem {
public:
    CWiaItem();
    ~CWiaItem();
    HRESULT SetIWiaItem(__in IWiaItem2 *pIWiaItem);

    void Release();

    HRESULT ReadPropertyLong(PROPID PropertyID, __out LONG *plPropertyValue);
    HRESULT ReadPropertyGUID(PROPID PropertyID, __out GUID *pguidPropertyValue);
    HRESULT ReadPropertyBSTR(PROPID PropertyID, __out BSTR *pbstrPropertyValue);

    HRESULT WritePropertyLong(PROPID PropertyID, LONG lPropertyValue);
    HRESULT WritePropertyGUID(PROPID PropertyID, GUID guidPropertyValue);

private:
    IWiaPropertyStorage *m_pIWiaPropStg;
protected:
};

