#include "stdafx.h"

#ifdef WIN32
#include <Windows.h>
WINDEBUGBREAK __DebugBreak = &DebugBreak;
#endif


LONG WINAPI DefaultUnHandleExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
#ifdef WIN32
	TCHAR sDumpFileName[256];
	_tcscpy(sDumpFileName, szExceptionDumpFile);
	INT_PTR nDotPos = _tcslen(sDumpFileName);
	for (; nDotPos >= 0; nDotPos--)
	{
		if (sDumpFileName[nDotPos] == '.')
		{
			break;;
		}
	}
	if (nDotPos < 0)
	{
		nDotPos = 0;
	}

	SYSTEMTIME Systime;
	GetLocalTime(&Systime);

	_stprintf(&sDumpFileName[nDotPos], _T("-%d-%d-%d-%d-%d-%d"), Systime.wYear - 2000, Systime.wMonth, Systime.wDay, Systime.wHour, Systime.wMinute, Systime.wSecond);
	_tcscat(sDumpFileName, _T(".dmp"));

	HANDLE	hFile = CreateFile(sDumpFileName, GENERIC_READ | GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	MINIDUMP_TYPE wDumpFlag = MiniDumpWithFullMemory;
	if (wExceptionDumpFlag >= 0)
	{
		wDumpFlag = (MINIDUMP_TYPE)wExceptionDumpFlag;
	}
	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId = GetCurrentThreadId();
		ExInfo.ExceptionPointers = ExceptionInfo;
		ExInfo.ClientPointers = NULL;

		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
			hFile, wDumpFlag, &ExInfo, NULL, NULL);
		CloseHandle(hFile);
	}


	ExitProcess(-1);
#else
	throw;
#endif
	return 0;
}

LONG WINAPI NormalUnHandleExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
#ifdef WIN32
	TCHAR sDumpFileName[256];
	_tcscpy(sDumpFileName, szExceptionDumpFile);
	INT_PTR nDotPos = _tcslen(sDumpFileName);
	for (; nDotPos >= 0; nDotPos--)
	{
		if (sDumpFileName[nDotPos] == '.')
		{
			break;;
		}
	}
	if (nDotPos < 0)
	{
		nDotPos = 0;
	}

	SYSTEMTIME Systime;
	GetLocalTime(&Systime);

	_stprintf(&sDumpFileName[nDotPos], _T("-%d-%d-%d-%d-%d-%d"), Systime.wYear - 2000, Systime.wMonth, Systime.wDay, Systime.wHour, Systime.wMinute, Systime.wSecond);
	_tcscat(sDumpFileName, _T(".dmp"));

	HANDLE	hFile = CreateFile(sDumpFileName, GENERIC_READ | GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	MINIDUMP_TYPE wDumpFlag = MiniDumpWithFullMemory;
	if (wExceptionDumpFlag >= 0)
	{
		wDumpFlag = (MINIDUMP_TYPE)wExceptionDumpFlag;
	}
	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId = GetCurrentThreadId();
		ExInfo.ExceptionPointers = ExceptionInfo;
		ExInfo.ClientPointers = NULL;

		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
			hFile, wDumpFlag, &ExInfo, NULL, NULL);
		CloseHandle(hFile);
	}

#else
	throw;
#endif
	return 0;
}