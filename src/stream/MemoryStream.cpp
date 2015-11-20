#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#if defined(WIN32)
#include <tchar.h>
#include <windows.h>
#endif
#include "_ast.h"
#include "MemoryStream.h"
#include "FileStream.h"

using namespace alloctrace::stream;

CMemoryStream::CMemoryStream()
{
	m_pMemory = m_pPointer = m_pMemoryEnd = m_pStreamEnd = NULL;
}

CMemoryStream::~CMemoryStream()
{
	SafeFree(m_pMemory);
}

char* CMemoryStream::Alloc(char* ptr, const size_t size)
{
	if (size == 0 && ptr)
	{
		free(ptr);
		return NULL;
	}
	return (char*)realloc(ptr, (size_t)size);
}

bool CMemoryStream::setSize(size_t tSize)
{
	size_t nOldPos = m_pPointer - m_pMemory;

	m_pMemory = Alloc(m_pMemory, tSize + sizeof(INT_PTR));
	m_pStreamEnd = m_pMemoryEnd = m_pMemory + tSize;
	*(PINT_PTR)m_pStreamEnd = 0;//在流末尾写入字符0，以便将流内容当做字符串使用
	m_pPointer = m_pMemory + nOldPos;
	if (m_pPointer > m_pStreamEnd)
		m_pPointer = m_pStreamEnd;

	return true;
}

size_t CMemoryStream::seek(const size_t tOffset, const int Origin)
{
	if (Origin == soBeginning)
		m_pPointer = m_pMemory + tOffset;
	else if (Origin == soCurrent)
		m_pPointer += tOffset;
	else if (Origin == soEnd)
		m_pPointer = m_pStreamEnd + tOffset;
	return m_pPointer - m_pMemory;
}

size_t CMemoryStream::read(LPVOID lpBuffer, const size_t tSizeToRead)
{
	size_t nRemain = m_pStreamEnd - m_pPointer;
	size_t tReadBytes = tSizeToRead;

	if (nRemain <= 0 || !tReadBytes)
		return 0;

	if (tReadBytes > nRemain)
		tReadBytes = nRemain;

	memcpy(lpBuffer, m_pPointer, (size_t)tReadBytes);
	m_pPointer += tReadBytes;
	return tReadBytes;
}

size_t CMemoryStream::write(LPCVOID lpBuffer, const size_t tSizeToWrite)
{
	char *pStreamEnd = m_pPointer + tSizeToWrite;

	if (pStreamEnd > m_pMemoryEnd)
	{
		setSize(pStreamEnd - m_pMemory);
	}

	memcpy(m_pPointer, lpBuffer, (size_t)tSizeToWrite);
	m_pPointer += tSizeToWrite;
	return tSizeToWrite;
}

size_t CMemoryStream::loadFromFile(LPCTSTR lpFileName)
{
	size_t nFileSize;
	CFileStream stm(lpFileName, CFileStream::faRead | CFileStream::faShareRead);

	nFileSize = stm.getSize();
	if (nFileSize > 0)
	{
		setSize(nFileSize);
		stm.read(m_pMemory, nFileSize);
	}
	return nFileSize;
}

size_t CMemoryStream::saveToFile(LPCTSTR lpFileName)
{
	CFileStream stm(lpFileName, CFileStream::faCreate);

	return stm.write(m_pMemory, getSize());
}
