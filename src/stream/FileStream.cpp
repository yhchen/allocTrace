#include <stdio.h>
#include <stdlib.h>
#if defined(WIN32)
#include <windows.h>
#include <tchar.h>
#pragma warning(disable:4996)
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#include "_ast.h"
#include "FileStream.h"

using namespace alloctrace::stream;

#if defined(WIN32)

CFileStream::CFileStream(LPCTSTR lpFileName, DWORD dwAccessType)
	:Inherited(INVALID_HANDLE_VALUE)
{
	DWORD dwWin32CreationDisposition;

	if (dwAccessType & faCreate)
		dwWin32CreationDisposition = CREATE_ALWAYS;
	else dwWin32CreationDisposition = OPEN_EXISTING;

	construct(lpFileName, dwAccessType, dwWin32CreationDisposition);
}

CFileStream::CFileStream(LPCTSTR lpFileName, DWORD dwAccessType, OpenDisposition eWin32CreateionDisposition)
	:Inherited(INVALID_HANDLE_VALUE)
{
	construct(lpFileName, dwAccessType, eWin32CreateionDisposition);
}

CFileStream::~CFileStream()
{
	if (m_hHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hHandle);
	}
	SafeFree(m_sFileName);
}

void CFileStream::setFileName(LPCTSTR lpFileName)
{
	m_sFileName = (LPTSTR)realloc(m_sFileName, (_tcslen(lpFileName) + 1) * sizeof(lpFileName[0]));
	_tcscpy(m_sFileName, lpFileName);
}

void CFileStream::construct(LPCTSTR lpFileName, DWORD dwAccessType, DWORD dwWin32CreationDisposition)
{
	DWORD dwDesiredAccess, dwShareMode;

	m_sFileName = NULL;
	setFileName(lpFileName);

	dwDesiredAccess = dwShareMode = 0;

	if (dwAccessType & faCreate)
	{
		dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
	}
	else
	{
		if (dwAccessType & faRead) dwDesiredAccess |= GENERIC_READ;
		if (dwAccessType & faWrite) dwDesiredAccess |= GENERIC_WRITE;
		if (dwAccessType & faShareRead)
		{
			dwShareMode |= FILE_SHARE_READ;
			dwDesiredAccess |= GENERIC_READ;
		}
		if (dwAccessType & faShareWrite)
		{
			dwShareMode |= FILE_SHARE_WRITE;
			dwDesiredAccess |= GENERIC_WRITE;
		}
		if (dwAccessType & faShareDelete) dwShareMode |= FILE_SHARE_DELETE;
	}

	setHandle(CreateFile(m_sFileName, dwDesiredAccess, dwShareMode, NULL, dwWin32CreationDisposition, 0, NULL));
}

#else

CFileStream::CFileStream(LPCTSTR lpFileName, DWORD dwAccessType)
	:Inherited(0)
{
	DWORD dwWin32CreationDisposition;

	if (dwAccessType & faCreate)
		dwWin32CreationDisposition = O_CREAT;
	//else dwWin32CreationDisposition = OPEN_EXISTING;

	construct(lpFileName, dwAccessType, dwWin32CreationDisposition);
}

CFileStream::CFileStream(LPCTSTR lpFileName, DWORD dwAccessType, OpenDisposition eWin32CreateionDisposition)
	:Inherited(0)
{
	construct(lpFileName, dwAccessType, eWin32CreateionDisposition);
}

CFileStream::~CFileStream()
{
	if (m_hHandle)
	{
		fclose(m_hHandle);
	}
	SafeFree(m_sFileName);
}

void CFileStream::setFileName(LPCTSTR lpFileName)
{
	m_sFileName = (LPTSTR)realloc(m_sFileName, (_tcslen(lpFileName) + 1) * sizeof(lpFileName[0]));
	strcpy(m_sFileName, lpFileName);
}

void CFileStream::construct(LPCTSTR lpFileName, DWORD dwAccessType, DWORD dwWin32CreationDisposition)
{
	m_sFileName = NULL;
	setFileName(lpFileName);

	const char* mode = "wb+";
	if ((dwAccessType & faRead) || (dwAccessType & faShareRead))
	{
		if ((dwAccessType & faWrite) || (dwAccessType & faShareWrite))
		{
			//读写
			mode = "wb+";
		}
		else
		{
			//只读
			mode = "rb";
		}

	}
	else if ((dwAccessType & faWrite) || (dwAccessType & faShareWrite))
	{
		//只写
		mode = "wb";
	}

	/* 为对应实现
	enum OpenDisposition
	{
	CreateIfNotExists = 1,		//文件不存在则创建
	AlwaysCreate,				//总是创建文件，如果文件存在则将文件截断为0字节
	OpenExistsOnly,				//仅打开存在的文件
	AlwaysOpen,					//总是打开文件，若文件存在则直接打开，否则尝试创建文件并打开
	TruncExistsOnly,			//如果文件存在则打开文件并截断为0字节
	};
	*/

	FILE *pFile = fopen(lpFileName, mode);
	if (pFile)
	{
		setHandle(pFile);
		fseek((FILE*)getHandle(), 0, SEEK_SET);
	}
}

#endif
