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
			//��д
			mode = "wb+";
		}
		else
		{
			//ֻ��
			mode = "rb";
		}

	}
	else if ((dwAccessType & faWrite) || (dwAccessType & faShareWrite))
	{
		//ֻд
		mode = "wb";
	}

	/* Ϊ��Ӧʵ��
	enum OpenDisposition
	{
	CreateIfNotExists = 1,		//�ļ��������򴴽�
	AlwaysCreate,				//���Ǵ����ļ�������ļ��������ļ��ض�Ϊ0�ֽ�
	OpenExistsOnly,				//���򿪴��ڵ��ļ�
	AlwaysOpen,					//���Ǵ��ļ������ļ�������ֱ�Ӵ򿪣������Դ����ļ�����
	TruncExistsOnly,			//����ļ���������ļ����ض�Ϊ0�ֽ�
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
