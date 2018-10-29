/*++

Copyright (c) 2005 Microsoft Corporation

All rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

File Name:

   rescache.h

Abstract:

   File resource cache defintion and resource writer interface defintion.
   Filters that need to add resources may be required to add the same resource
   to numerous pages in a job. To prevent the same resource being sent
   repeatedly, the file resource cache class implements an interface that
   filters can use to add resources without needing to keep track of whether
   they have been sent. For specific filter functionality, a class should be
   created that implements a resource writer interface. This should be able to
   write the resource to a stream. The filter can then add this class to the
   the resource cache and it will take care of writing the resource via the
   IResWriter::WriteData method if it has not already been written.

--*/

#pragma once

//
// The resource cache needs to map a unique name against the URI used
// and the part that was added. This allows us to retrieve the URI to
// write to the mark-up and also the part to set to the page/doc/docseq
// part.
//
typedef pair<CComBSTR, CComPtr<IPartBase> > URIPartPair;
typedef map<CComBSTR ,URIPartPair> ResCache;

class IResWriter
{
public:
    IResWriter(){}

    virtual ~IResWriter(){}

    virtual HRESULT
    WriteData(
        __in IPartBase*         pResource,
        __in IPrintWriteStream* pWriter
        ) = 0;

    virtual HRESULT
    GetKeyName(
        __deref_out BSTR* pbstrKeyName
        ) = 0;

    virtual HRESULT
    GetResURI(
        __deref_out BSTR* pbstrResURI
        ) = 0;

};

class CFileResourceCache
{
public:
    CFileResourceCache();

    virtual ~CFileResourceCache();

    HRESULT
    GetURI(
        __in_z      BSTR  bstrResName,
        __deref_out BSTR* pbstrURI
        );

    template <class _T>
    HRESULT
    WriteResource(
        __in IXpsDocumentConsumer*  pXpsConsumer,
        __in IFixedPage*            pFixedPage,
        __in IResWriter*            pResWriter
        );

protected:
    BOOL
    Cached(
        __in_z BSTR bstrResName
        );

private:
    ResCache m_resMap;
};

//
// Explicitly instantiate the IPartFont, IPartImage and IPartColorProfile WriteResource template functions
//
template
HRESULT
CFileResourceCache::WriteResource<IPartFont>(
        __in IXpsDocumentConsumer*  pXpsConsumer,
        __in IFixedPage*            pFixedPage,
        __in IResWriter*            pResWriter
        );

template
HRESULT
CFileResourceCache::WriteResource<IPartImage>(
        __in IXpsDocumentConsumer*  pXpsConsumer,
        __in IFixedPage*            pFixedPage,
        __in IResWriter*            pResWriter
        );

template
HRESULT
CFileResourceCache::WriteResource<IPartColorProfile>(
        __in IXpsDocumentConsumer*  pXpsConsumer,
        __in IFixedPage*            pFixedPage,
        __in IResWriter*            pResWriter
        );

template
HRESULT
CFileResourceCache::WriteResource<IPartResourceDictionary>(
        __in IXpsDocumentConsumer*  pXpsConsumer,
        __in IFixedPage*            pFixedPage,
        __in IResWriter*            pResWriter
        );


