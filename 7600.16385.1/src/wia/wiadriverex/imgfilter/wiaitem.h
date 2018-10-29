#pragma once

#define MIN_PROPID 2

class CWiaItem {
public:
    CWiaItem();
    ~CWiaItem();
    HRESULT SetIWiaItem(IWiaItem2 *pIWiaItem);
    void Release();

    HRESULT ReadRequiredPropertyLong(PROPID PropertyID, LONG *plPropertyValue);
    HRESULT ReadRequiredPropertyBSTR(PROPID PropertyID, BSTR *pbstrPropertyValue);
    HRESULT ReadRequiredPropertyGUID(PROPID PropertyID, GUID *pguidPropertyValue);

private:
    IWiaPropertyStorage *m_pIWiaPropStg;
protected:
};

