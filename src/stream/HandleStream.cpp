#if defined(WIN32)
#include <windows.h>
#elif defined(__GUNC__)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#include "HandleStream.h"
#include <stdlib.h>
#include <assert.h>

using namespace alloctrace::stream;

#if defined(WIN32)
CHandleStream::CHandleStream(HANDLE StreamHandle)
	:Inherited()
{
	m_hHandle = StreamHandle;
}

inline HANDLE CHandleStream::getHandle()
{
	return m_hHandle;
}

inline bool CHandleStream::handleValid()
{
	return m_hHandle != INVALID_HANDLE_VALUE;
}

void CHandleStream::setHandle(HANDLE Handle)
{
	m_hHandle = Handle;
}

bool CHandleStream::setSize(size_t tSize)
{
	seek(tSize, soBeginning);
	return SetEndOfFile(m_hHandle) != FALSE;
}

size_t CHandleStream::seek(const size_t tOffset, const int Origin)
{
	LARGE_INTEGER li, nNewPointer;
	li.QuadPart = tOffset;

	if (SetFilePointerEx(m_hHandle, li, &nNewPointer, Origin))
		return (size_t)nNewPointer.QuadPart;
	else return -1;
}

size_t CHandleStream::read(LPVOID lpBuffer, const size_t tSizeToRead)
{
	DWORD dwBytesReaded;
	if (ReadFile(m_hHandle, lpBuffer, (DWORD)tSizeToRead, &dwBytesReaded, NULL))
		return dwBytesReaded;
	else return -1;
}

size_t CHandleStream::write(LPCVOID lpBuffer, const size_t tSizeToWrite)
{
	DWORD dwBytesWriten;
	if (WriteFile(m_hHandle, lpBuffer, (DWORD)tSizeToWrite, &dwBytesWriten, NULL))
		return dwBytesWriten;
	else return -1;
}

#elif defined(__GNUC__)

CHandleStream::CHandleStream(FILE* StreamHandle)
	:Inherited()
{
	m_hHandle = StreamHandle;
}

void CHandleStream::setHandle(FILE* Handle)
{
	m_hHandle = Handle;
}

bool CHandleStream::setSize(size_t tSize)
{
	if (!m_hHandle)
	{
		return false;
	}
	if (0 == ftruncate(fileno(m_hHandle), tSize))
	{
		return true;
	}
	return false;
}

size_t CHandleStream::seek(const size_t tOffset, const int Origin)
{
	if (!m_hHandle)
	{
		return 0;
	}
	if (fseek(m_hHandle, tOffset, Origin))
	{
		return 0;
	}
	return ftell(m_hHandle);
}

size_t CHandleStream::read(LPVOID lpBuffer, const size_t tSizeToRead)
{
	if (!m_hHandle)
	{
		return 0;
	}
	return fread(lpBuffer, 1, (size_t)tSizeToRead, m_hHandle);
}

size_t CHandleStream::write(LPCVOID lpBuffer, const size_t tSizeToWrite)
{
	if (!m_hHandle)
	{
		return 0;
	}
	return  fwrite(lpBuffer, 1, (size_t)tSizeToWrite, m_hHandle);
}

#endif // __GNUC__
