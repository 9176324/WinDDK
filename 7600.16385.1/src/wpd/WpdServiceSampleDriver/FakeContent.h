#pragma once

class FakeContent
{
public:
    FakeContent() :
        CanDelete(false), 
        RequiredScope(FULL_DEVICE_ACCESS),
        MarkedForDeletion(false)
    {
        Format = WPD_OBJECT_FORMAT_UNSPECIFIED;
        ContentType = WPD_CONTENT_TYPE_UNSPECIFIED;
    }

    FakeContent(const FakeContent& src) :
        CanDelete(false)
    {
        *this = src;
    }

    virtual ~FakeContent()
    {
        for(size_t index = 0; index < m_Children.GetCount(); index++)
        {
            if (m_Children[index])
            {
                delete(m_Children[index]);
                m_Children[index] = NULL;
            }
        }
        m_Children.RemoveAll();
    }

    virtual FakeContent& operator= (const FakeContent& src)
    {
        ObjectID                    = src.ObjectID;
        PersistentUniqueID          = src.PersistentUniqueID;
        ParentID                    = src.ParentID;
        Name                        = src.Name;
        ContentType                 = src.ContentType;
        Format                      = src.Format;
        CanDelete                   = src.CanDelete;
        RequiredScope               = src.RequiredScope;
        ParentPersistentUniqueID    = src.ParentPersistentUniqueID;
        ContainerFunctionalObjectID = src.ContainerFunctionalObjectID;

        return *this;
    }

    virtual HRESULT InitializeContent(
        __inout DWORD *pdwLastObjectID);
        
    virtual HRESULT InitializeEnumerationContext(
                ACCESS_SCOPE                Scope,
        __out   WpdObjectEnumeratorContext* pEnumeratorContext);

    virtual HRESULT GetSupportedProperties(
        __out   IPortableDeviceKeyCollection *pKeys);

    virtual HRESULT GetPropertyAttributes(
                REFPROPERTYKEY         Key,
        __out   IPortableDeviceValues* pAttributes);

    virtual HRESULT GetValue(
                REFPROPERTYKEY         Key, 
        __out   IPortableDeviceValues* pStore);
        
    virtual HRESULT WriteValue(
                REFPROPERTYKEY         Key, 
                const PROPVARIANT&     Value);

    virtual HRESULT CreatePropertiesOnlyObject(
        __in    IPortableDeviceValues* pObjectProperties, 
        __out   DWORD*                 pdwLastObjectID,
        __out   FakeContent**          ppNewObject);

    virtual HRESULT GetSupportedResources(
        __out   IPortableDeviceKeyCollection* pResources);

    virtual HRESULT GetResourceAttributes(
                REFPROPERTYKEY         Resource,
        __out   IPortableDeviceValues* pAttributes);
    
    virtual HRESULT OpenResource(
                REFPROPERTYKEY            Resource,
                const DWORD               dwMode,
        __out   WpdObjectResourceContext* pResourceContext); 

    virtual HRESULT ReadResourceData(
        __in                            WpdObjectResourceContext*   pResourceContext,
        __out_bcount(dwNumBytesToRead)  BYTE*                       pBuffer,
                                        const DWORD                 dwNumBytesToRead,
        __out                           DWORD*                      pdwNumBytesRead);

    virtual HRESULT WriteValues(
        __in    IPortableDeviceValues* pValues,
        __out   IPortableDeviceValues* pResults,
        __out   bool*                  pbObjectChanged); 

public:
    bool CanAccess(
                ACCESS_SCOPE Scope);

    HRESULT GetAllValues(
        __out   IPortableDeviceValues* pStore);

    bool FindNext(
                            ACCESS_SCOPE  Scope,
                            const DWORD   dwIndex,
        __deref_out_opt     FakeContent** ppChild);

    HRESULT GetContent(
                ACCESS_SCOPE   Scope,
        __in    LPCWSTR        wszObjectID,
        __out   FakeContent**  ppContent);

    HRESULT GetObjectIDsByFormat(
                ACCESS_SCOPE                          Scope,
                REFGUID                               Format,
                const DWORD                           dwDepth,
        __out   IPortableDevicePropVariantCollection* pObjectIDs);

    HRESULT GetObjectIDByPersistentID(
                ACCESS_SCOPE                          Scope,
        __in    LPCWSTR                               wszPersistentID,
        __out   IPortableDevicePropVariantCollection* pObjectIDs);

    HRESULT MarkForDelete(
                const DWORD dwOptions);

    HRESULT RemoveObjectsMarkedForDeletion(
                ACCESS_SCOPE Scope);

public:
    CAtlStringW     ObjectID;
    CAtlStringW     PersistentUniqueID;
    CAtlStringW     ParentID;
    CAtlStringW     Name;
    CAtlStringW     ParentPersistentUniqueID;
    CAtlStringW     ContainerFunctionalObjectID;
    GUID            ContentType;
    GUID            Format;
    bool            CanDelete;
    bool            MarkedForDeletion;
    
    // A bitmask of all the required scopes in order to access this object
    ACCESS_SCOPE    RequiredScope;

    CAtlArray<FakeContent*> m_Children;
};

