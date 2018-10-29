// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLFILE_H__
#define __ATLFILE_H__

#pragma once

#include <atlbase.h>


#pragma pack(push,_ATL_PACKING)
namespace ATL
{

class CAtlFile : public CHandle
{
public:
	CAtlFile() throw()
	{
	}
	CAtlFile( __in CAtlFile& file ) throw() :
		CHandle( file )  // Transfers ownership
	{
	}
	explicit CAtlFile( __in HANDLE hFile ) throw() :
		CHandle( hFile )  // Takes ownership
	{
	}

	HRESULT Create(
		__in LPCTSTR szFilename,
		__in DWORD dwDesiredAccess,
		__in DWORD dwShareMode,
		__in DWORD dwCreationDisposition,
		__in DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL,
		__in_opt LPSECURITY_ATTRIBUTES lpsa = NULL,
		__in_opt HANDLE hTemplateFile = NULL) throw()
	{
		ATLASSUME(m_h == NULL);

		HANDLE hFile = ::CreateFile(
			szFilename,
			dwDesiredAccess,
			dwShareMode,
			lpsa,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile);

		if (hFile == INVALID_HANDLE_VALUE)
			return AtlHresultFromLastError();

		Attach(hFile);
		return S_OK;
	}

	HRESULT Read(
		__out_bcount(nBufSize) LPVOID pBuffer,
		DWORD nBufSize) throw()
	{
		ATLASSUME(m_h != NULL);

		DWORD nBytesRead = 0;
		BOOL bSuccess = ::ReadFile(m_h, pBuffer, nBufSize, &nBytesRead, NULL);
		if (!bSuccess )
			return AtlHresultFromLastError();
		if (nBytesRead != nBufSize)
			return HRESULT_FROM_WIN32( ERROR_HANDLE_EOF );

		return S_OK;
	}

	HRESULT Read(
		__out_bcount_part(nBufSize, nBytesRead) LPVOID pBuffer,
		__in DWORD nBufSize,
		__out DWORD& nBytesRead) throw()
	{
		ATLASSUME(m_h != NULL);

		BOOL b = ::ReadFile(m_h, pBuffer, nBufSize, &nBytesRead, NULL);
		if (!b)
			return AtlHresultFromLastError();

		return S_OK;
	}

	// this function will usually return HRESULT_FROM_WIN32(ERROR_IO_PENDING)
	// indicating succesful queueing of the operation
	HRESULT Read(
		__out_bcount(nBufSize) LPVOID pBuffer,
		__in DWORD nBufSize,
		__in_opt LPOVERLAPPED pOverlapped) throw()
	{
		ATLASSUME(m_h != NULL);

		BOOL b = ::ReadFile(m_h, pBuffer, nBufSize, NULL, pOverlapped);
		if (!b)
			return AtlHresultFromLastError();

		return S_OK;
	}

	HRESULT Read(
		__in_bcount(nBufSize) LPVOID pBuffer,
		__in DWORD nBufSize,
		__in_opt LPOVERLAPPED pOverlapped,
		__in LPOVERLAPPED_COMPLETION_ROUTINE pfnCompletionRoutine) throw()
	{
		ATLASSUME(m_h != NULL);

		BOOL b = ::ReadFileEx(m_h, pBuffer, nBufSize, pOverlapped, pfnCompletionRoutine);
		if (!b)
			return AtlHresultFromLastError();

		return S_OK;
	}

	HRESULT Write(
		__in_bcount(nBufSize) LPCVOID pBuffer,
		__in DWORD nBufSize,
		__out_opt DWORD* pnBytesWritten = NULL) throw()
	{
		ATLASSUME(m_h != NULL);

		DWORD nBytesWritten;
		if (pnBytesWritten == NULL)
			pnBytesWritten = &nBytesWritten;
		BOOL b = ::WriteFile(m_h, pBuffer, nBufSize, pnBytesWritten, NULL);
		if (!b)
			return AtlHresultFromLastError();

		return S_OK;
	}

	// this function will usually return HRESULT_FROM_WIN32(ERROR_IO_PENDING)
	// indicating succesful queueing of the operation
	HRESULT Write(
		__in_bcount(nBufSize) LPCVOID pBuffer,
		__in DWORD nBufSize,
		__in_opt LPOVERLAPPED pOverlapped) throw()
	{
		ATLASSUME(m_h != NULL);

		BOOL b = ::WriteFile(m_h, pBuffer, nBufSize, NULL, pOverlapped);
		if (!b)
			return AtlHresultFromLastError();

		return S_OK;
	}

	HRESULT Write(
		__in_bcount(nBufSize) LPCVOID pBuffer,
		__in DWORD nBufSize,
		__in_opt LPOVERLAPPED pOverlapped,
		__in LPOVERLAPPED_COMPLETION_ROUTINE pfnCompletionRoutine) throw()
	{
		ATLASSUME(m_h != NULL);

		BOOL b = ::WriteFileEx(m_h, pBuffer, nBufSize, pOverlapped, pfnCompletionRoutine);
		if (!b)
			return AtlHresultFromLastError();

		return S_OK;
	}

	// this function returns HRESULT_FROM_WIN32(ERROR_IO_INCOMPLETE)
	// if bWait is false and the operation is still pending
	HRESULT GetOverlappedResult(
		__in LPOVERLAPPED pOverlapped,
		__out DWORD& dwBytesTransferred,
		__in BOOL bWait) throw()
	{
		BOOL b = ::GetOverlappedResult(m_h, pOverlapped, &dwBytesTransferred, bWait);
		if (!b)
			return AtlHresultFromLastError();

		return S_OK;
	}

	HRESULT Seek(__in LONGLONG nOffset, __in DWORD dwFrom = FILE_CURRENT) throw()
	{
		ATLASSUME(m_h != NULL);
		ATLASSERT(dwFrom == FILE_BEGIN || dwFrom == FILE_END || dwFrom == FILE_CURRENT);

		LARGE_INTEGER liOffset;
		liOffset.QuadPart = nOffset;
		DWORD nNewPos = ::SetFilePointer(m_h, liOffset.LowPart, &liOffset.HighPart, dwFrom);
		if (nNewPos == INVALID_SET_FILE_POINTER)
		{
			HRESULT hr;

			hr = AtlHresultFromLastError();
			if (FAILED(hr))
				return hr;
		}

		return S_OK;
	}

	HRESULT GetPosition(__out ULONGLONG& nPos) const throw()
	{
		ATLASSUME(m_h != NULL);

		LARGE_INTEGER liOffset;
		liOffset.QuadPart = 0;
		liOffset.LowPart = ::SetFilePointer(m_h, 0, &liOffset.HighPart, FILE_CURRENT);
		if (liOffset.LowPart == INVALID_SET_FILE_POINTER)
		{
			HRESULT hr;

			hr = AtlHresultFromLastError();
			if (FAILED(hr))
				return hr;
		}
		nPos = liOffset.QuadPart;

		return S_OK;
	}

	HRESULT Flush() throw()
	{
		ATLASSUME(m_h != NULL);

		if (!::FlushFileBuffers(m_h))
			return AtlHresultFromLastError();

		return S_OK;
	}

	HRESULT LockRange(__in ULONGLONG nPos, __in ULONGLONG nCount) throw()
	{
		ATLASSUME(m_h != NULL);

		LARGE_INTEGER liPos;
		liPos.QuadPart = nPos;

		LARGE_INTEGER liCount;
		liCount.QuadPart = nCount;

		if (!::LockFile(m_h, liPos.LowPart, liPos.HighPart, liCount.LowPart, liCount.HighPart))
			return AtlHresultFromLastError();

		return S_OK;
	}

	HRESULT UnlockRange(__in ULONGLONG nPos, __in ULONGLONG nCount) throw()
	{
		ATLASSUME(m_h != NULL);

		LARGE_INTEGER liPos;
		liPos.QuadPart = nPos;

		LARGE_INTEGER liCount;
		liCount.QuadPart = nCount;

		if (!::UnlockFile(m_h, liPos.LowPart, liPos.HighPart, liCount.LowPart, liCount.HighPart))
			return AtlHresultFromLastError();

		return S_OK;
	}

	HRESULT SetSize(__in ULONGLONG nNewLen) throw()
	{
		ATLASSUME(m_h != NULL);

		HRESULT hr = Seek(nNewLen, FILE_BEGIN);
		if (FAILED(hr))
			return hr;

		if (!::SetEndOfFile(m_h))
			return AtlHresultFromLastError();

		return S_OK;
	}

	HRESULT GetSize(__out ULONGLONG& nLen) const throw()
	{
		ATLASSUME(m_h != NULL);

		ULARGE_INTEGER liFileSize;
		liFileSize.LowPart = ::GetFileSize(m_h, &liFileSize.HighPart);
		if (liFileSize.LowPart == INVALID_FILE_SIZE)
		{
			HRESULT hr;

			hr = AtlHresultFromLastError();
			if (FAILED(hr))
				return hr;
		}

		nLen = liFileSize.QuadPart;

		return S_OK;
	}
};

// This class allows the creation of a temporary file that is written to.
// When the entire file has been successfully written it will be closed and given
// it's proper file name if required.
class CAtlTemporaryFile
{
public:
	CAtlTemporaryFile() throw()
	{
	}

	~CAtlTemporaryFile() throw()
	{
		// Ensure that the temporary file is closed and deleted,
		// if necessary.
		if (m_file.m_h != NULL)
		{
			Close();
		}
	}

	HRESULT Create(__in_opt LPCTSTR pszDir = NULL, __in DWORD dwDesiredAccess = GENERIC_WRITE) throw()
	{
		TCHAR szPath[_MAX_PATH]; 
		TCHAR tmpFileName[_MAX_PATH]; 

		ATLASSUME(m_file.m_h == NULL);

		if (pszDir == NULL)
		{
			DWORD dwRet = GetTempPath(_MAX_DIR, szPath);
			if (dwRet == 0)
			{
				// Couldn't find temporary path;
				return AtlHresultFromLastError();
			}
			else if (dwRet > _MAX_DIR)
			{
				return DISP_E_BUFFERTOOSMALL;
			}
		}
		else
		{
			if(Checked::tcsncpy_s(szPath, _countof(szPath), pszDir, _TRUNCATE)==STRUNCATE)
			{
				return DISP_E_BUFFERTOOSMALL;
			}
		}

		if (!GetTempFileName(szPath, _T("TFR"), 0, tmpFileName))
		{
			// Couldn't create temporary filename;
			return AtlHresultFromLastError();
		}
		tmpFileName[_countof(tmpFileName)-1]='\0';

		Checked::tcsncpy_s(m_szTempFileName, _countof(m_szTempFileName), tmpFileName, _TRUNCATE);
		SECURITY_ATTRIBUTES secatt;
		secatt.nLength = sizeof(secatt);
		secatt.lpSecurityDescriptor = NULL;
		secatt.bInheritHandle = TRUE;

		m_dwAccess = dwDesiredAccess;

		return m_file.Create(
			m_szTempFileName,
			m_dwAccess,
			0,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NOT_CONTENT_INDEXED | FILE_ATTRIBUTE_TEMPORARY,
			&secatt);
	}

	HRESULT Close(__in_opt LPCTSTR szNewName = NULL) throw()
	{
		ATLASSUME(m_file.m_h != NULL);

		// This routine is called when we are finished writing to the 
		// temporary file, so we now just want to close it and copy
		// it to the actual filename we want it to be called.

		// So let's close it first.
		m_file.Close();

		// no new name so delete it
		if (szNewName == NULL)
		{
			::DeleteFile(m_szTempFileName);
			return S_OK;
		}

		// delete any existing file and move our temp file into it's place
		if (!::DeleteFile(szNewName))
		{
			DWORD dwError = GetLastError();
			if (dwError != ERROR_FILE_NOT_FOUND)
				return AtlHresultFromWin32(dwError);
		}

		if (!::MoveFile(m_szTempFileName, szNewName))
			return AtlHresultFromLastError();

		return S_OK;
	}

	HRESULT HandsOff() throw()
	{
		m_file.Flush();
		m_file.Close();

		return S_OK;
	}

	HRESULT HandsOn() throw()
	{
		HRESULT hr = m_file.Create(
			m_szTempFileName,
			m_dwAccess,
			0,
			OPEN_EXISTING);
		if (FAILED(hr))
			return hr;

		return m_file.Seek(0, FILE_END);
	}

	HRESULT Read(
		__out_bcount(nBufSize) LPVOID pBuffer,
		__in DWORD nBufSize,
		__out DWORD& nBytesRead) throw()
	{
		return m_file.Read(pBuffer, nBufSize, nBytesRead);
	}

	HRESULT Write(
		__in_bcount(nBufSize) LPCVOID pBuffer,
		__in DWORD nBufSize,
		__out_opt DWORD* pnBytesWritten = NULL) throw()
	{
		return m_file.Write(pBuffer, nBufSize, pnBytesWritten);
	}

	HRESULT Seek(__in LONGLONG nOffset, __in DWORD dwFrom = FILE_CURRENT) throw()
	{
		return m_file.Seek(nOffset, dwFrom);
	}

	HRESULT GetPosition(__out ULONGLONG& nPos) const throw()
	{
		return m_file.GetPosition(nPos);
	}

	HRESULT Flush() throw()
	{
		return m_file.Flush();
	}

	HRESULT LockRange(__in ULONGLONG nPos, __in ULONGLONG nCount) throw()
	{
		return m_file.LockRange(nPos, nCount);
	}

	HRESULT UnlockRange(__in ULONGLONG nPos, __in ULONGLONG nCount) throw()
	{
		return m_file.UnlockRange(nPos, nCount);
	}

	HRESULT SetSize(__in ULONGLONG nNewLen) throw()
	{
		return m_file.SetSize(nNewLen);
	}

	HRESULT GetSize(__out ULONGLONG& nLen) const throw()
	{
		return m_file.GetSize(nLen);
	}

	operator HANDLE() throw()
	{
		return m_file;
	}

	LPCTSTR TempFileName() throw()
	{
		return m_szTempFileName;
	}

private:
	CAtlFile m_file;
	TCHAR m_szTempFileName[_MAX_FNAME+1];
	DWORD m_dwAccess;
};

class CAtlFileMappingBase
{
public:
	CAtlFileMappingBase() throw()
	{
		m_pData = NULL;
		m_hMapping = NULL;
	}

	~CAtlFileMappingBase() throw()
	{
		Unmap();
	}

	HRESULT MapFile(
		__in HANDLE hFile,
		__in SIZE_T nMappingSize = 0,
		__in ULONGLONG nOffset = 0,
		__in DWORD dwMappingProtection = PAGE_READONLY,
		__in DWORD dwViewDesiredAccess = FILE_MAP_READ) throw()
	{
		ATLASSUME(m_pData == NULL);
		ATLASSUME(m_hMapping == NULL);
		ATLASSERT(hFile != INVALID_HANDLE_VALUE && hFile != NULL);

		ULARGE_INTEGER liFileSize;
		liFileSize.LowPart = ::GetFileSize(hFile, &liFileSize.HighPart);
		if (liFileSize.QuadPart < nMappingSize)
			liFileSize.QuadPart = nMappingSize;

		m_hMapping = ::CreateFileMapping(hFile, NULL, dwMappingProtection, liFileSize.HighPart, liFileSize.LowPart, 0);
		if (m_hMapping == NULL)
			return AtlHresultFromLastError();

		if (nMappingSize == 0)
			m_nMappingSize = (SIZE_T) (liFileSize.QuadPart - nOffset);
		else
			m_nMappingSize = nMappingSize;

		m_dwViewDesiredAccess = dwViewDesiredAccess;
		m_nOffset.QuadPart = nOffset;

		m_pData = ::MapViewOfFileEx(m_hMapping, m_dwViewDesiredAccess, m_nOffset.HighPart, m_nOffset.LowPart, m_nMappingSize, NULL);
		if (m_pData == NULL)
		{
			HRESULT hr;

			hr = AtlHresultFromLastError();
			::CloseHandle(m_hMapping);
			m_hMapping = NULL;
			return hr;
		}

		return S_OK;
	}

	HRESULT MapSharedMem(
		__in SIZE_T nMappingSize,
		__in LPCTSTR szName,
		__out_opt BOOL* pbAlreadyExisted = NULL,
		__in_opt LPSECURITY_ATTRIBUTES lpsa = NULL,
		__in DWORD dwMappingProtection = PAGE_READWRITE,
		__in DWORD dwViewDesiredAccess = FILE_MAP_ALL_ACCESS) throw()
	{
		ATLASSUME(m_pData == NULL);
		ATLASSUME(m_hMapping == NULL);
		ATLASSERT(nMappingSize > 0);
		ATLASSERT(szName != NULL); // if you just want a regular chunk of memory, use a heap allocator

		m_nMappingSize = nMappingSize;

		ULARGE_INTEGER nSize;
		nSize.QuadPart = nMappingSize;
		m_hMapping = ::CreateFileMapping(INVALID_HANDLE_VALUE, lpsa, dwMappingProtection, nSize.HighPart, nSize.LowPart, szName);
		if (m_hMapping == NULL)
			return AtlHresultFromLastError();

		if (pbAlreadyExisted != NULL)
			*pbAlreadyExisted = (GetLastError() == ERROR_ALREADY_EXISTS);

		m_dwViewDesiredAccess = dwViewDesiredAccess;
		m_nOffset.QuadPart = 0;

		m_pData = ::MapViewOfFileEx(m_hMapping, m_dwViewDesiredAccess, m_nOffset.HighPart, m_nOffset.LowPart, m_nMappingSize, NULL);
		if (m_pData == NULL)
		{
			HRESULT hr;

			hr = AtlHresultFromLastError();
			::CloseHandle(m_hMapping);
			m_hMapping = NULL;
			return hr;
		}


		return S_OK;
	}

	HRESULT OpenMapping(
		__in LPCTSTR szName,
		__in SIZE_T nMappingSize,
		__in ULONGLONG nOffset = 0,
		__in DWORD dwViewDesiredAccess = FILE_MAP_ALL_ACCESS) throw()
	{
		ATLASSUME(m_pData == NULL);
		ATLASSUME(m_hMapping == NULL);
		ATLASSERT(szName != NULL); // if you just want a regular chunk of memory, use a heap allocator

		m_nMappingSize = nMappingSize;
		m_dwViewDesiredAccess = dwViewDesiredAccess;

		m_hMapping = ::OpenFileMapping(m_dwViewDesiredAccess, FALSE, szName);
		if (m_hMapping == NULL)
			return AtlHresultFromLastError();

		m_dwViewDesiredAccess = dwViewDesiredAccess;
		m_nOffset.QuadPart = nOffset;

		m_pData = ::MapViewOfFileEx(m_hMapping, m_dwViewDesiredAccess, m_nOffset.HighPart, m_nOffset.LowPart, m_nMappingSize, NULL);
		if (m_pData == NULL)
		{
			HRESULT hr;

			hr = AtlHresultFromLastError();
			::CloseHandle(m_hMapping);
			m_hMapping = NULL;
			return hr;
		}


		return S_OK;
	}

	HRESULT Unmap() throw()
	{
		HRESULT hr = S_OK;

		if (m_pData != NULL)
		{
			if (!::UnmapViewOfFile(m_pData))
				hr = AtlHresultFromLastError();
			m_pData = NULL;
		}
		if (m_hMapping != NULL)
		{
			if (!::CloseHandle(m_hMapping) && SUCCEEDED(hr))
				hr = AtlHresultFromLastError();
			m_hMapping = NULL;
		}
		return hr;
	}

	void* GetData() const throw()
	{
		return m_pData;
	}

	HANDLE GetHandle() const throw()
	{
		return m_hMapping;
	}

	SIZE_T GetMappingSize() throw()
	{
		return m_nMappingSize;
	}

	HRESULT CopyFrom(__in CAtlFileMappingBase& orig) throw()
	{
		if (this == &orig)
			return S_OK;
		ATLASSUME(m_pData == NULL);
		ATLASSUME(m_hMapping == NULL);
		ATLASSERT(orig.m_pData != NULL);
		ATLASSERT(orig.m_hMapping != NULL);

		m_dwViewDesiredAccess = orig.m_dwViewDesiredAccess;
		m_nOffset.QuadPart = orig.m_nOffset.QuadPart;
		m_nMappingSize = orig.m_nMappingSize;

		if (!::DuplicateHandle(GetCurrentProcess(), orig.m_hMapping, GetCurrentProcess(),
				&m_hMapping, NULL, TRUE, DUPLICATE_SAME_ACCESS))
			return AtlHresultFromLastError();

		m_pData = ::MapViewOfFileEx(m_hMapping, m_dwViewDesiredAccess, m_nOffset.HighPart, m_nOffset.LowPart, m_nMappingSize, NULL);
		if (m_pData == NULL)
		{
			HRESULT hr;

			hr = AtlHresultFromLastError();
			::CloseHandle(m_hMapping);
			m_hMapping = NULL;
			return hr;
		}

		return S_OK;
	}

	CAtlFileMappingBase(__in CAtlFileMappingBase& orig)
	{
		m_pData = NULL;
		m_hMapping = NULL;

		HRESULT hr = CopyFrom(orig);
		if (FAILED(hr))
			AtlThrow(hr);
	}

	CAtlFileMappingBase& operator=(__in CAtlFileMappingBase& orig)
	{
		HRESULT hr = CopyFrom(orig);
		if (FAILED(hr))
			AtlThrow(hr);

		return *this;
	}

private:
	void* m_pData;
	SIZE_T m_nMappingSize;
	HANDLE m_hMapping;
	ULARGE_INTEGER m_nOffset;
	DWORD m_dwViewDesiredAccess;
};

template <typename T = char>
class CAtlFileMapping : public CAtlFileMappingBase
{
public:
	operator T*() const throw()
	{
		return reinterpret_cast<T*>(GetData());
	}
};

}; //namespace ATL

#pragma pack(pop)
#endif //__ATLFILE_H__

