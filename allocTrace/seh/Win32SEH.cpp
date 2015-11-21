#include "stdafx.h"
#include <stdio.h>
#include "stream/BaseStream.h"
#include "Symbol.h"
#include "SEH.h"

class ExceptionFileRecorder
{
private:
	HANDLE m_hFile;
public:
	ExceptionFileRecorder()
	{
		m_hFile = INVALID_HANDLE_VALUE;
	}
	~ExceptionFileRecorder()
	{
		close();
	}
	void close()
	{
		if (m_hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
	}
	void open(const char* fileName)
	{
		HANDLE hFile = CreateFileA(fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			//检测文件大小，如果过大，则清空重新
			const int MAX_SIZE = 102400;
			int nFileSize = (int)GetFileSize(hFile, NULL);
			if (nFileSize >= MAX_SIZE)
			{
				CloseHandle(hFile);
				DeleteFileA(fileName);
				hFile = CreateFileA(fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
			}
		}

		if (hFile != INVALID_HANDLE_VALUE)
		{
			LARGE_INTEGER li, nNewPointer;
			li.QuadPart = 0;
			SetFilePointerEx(hFile, li, &nNewPointer, FILE_END);

			SYSTEMTIME st;
			GetLocalTime(&st);
			char sDelimiter[1024];
			int nLen = sprintf(sDelimiter, "---------------------------- %d-%d-%d %d:%d:%d ----------------------------\r\n",
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

			DWORD dwBytesWriten = 0;
			WriteFile(hFile, sDelimiter, nLen, &dwBytesWriten, NULL);

			close();

			m_hFile = hFile;
		}
	}
	void write(const void* buf, size_t len)
	{
		if (m_hFile == INVALID_HANDLE_VALUE)
			open(".\\Exception.txt");
		if (m_hFile != INVALID_HANDLE_VALUE)
		{
			LARGE_INTEGER li, nNewPointer;
			li.QuadPart = 0;
			SetFilePointerEx(m_hFile, li, &nNewPointer, FILE_END);
			DWORD dwBytesWriten = 0;
			WriteFile(m_hFile, buf, (DWORD)len, &dwBytesWriten, NULL);
		}
	}
};

ExceptionFileRecorder recorder;
BOOL alright = TRUE;

void SetExceptionRecordFileName(const char* fileName)
{
	recorder.open(fileName);
}

int Win32RecordException(struct _EXCEPTION_POINTERS *exceptInfo)
{
	const char* sCodeName = "NA";
	char sExtInfo[1024];

	sExtInfo[0] = 0;

	switch(exceptInfo->ExceptionRecord->ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION        : 
		{
			sCodeName = "ACCESS_VIOLATION";
			if(exceptInfo->ExceptionRecord->ExceptionInformation[0] == 0)
				sprintf(sExtInfo, "ACCESS_VIOLATION reading location 0x%llx", exceptInfo->ExceptionRecord->ExceptionInformation[1]);
			else if(exceptInfo->ExceptionRecord->ExceptionInformation[0] == 1)
				sprintf(sExtInfo, "ACCESS_VIOLATION writing location 0x%llx", exceptInfo->ExceptionRecord->ExceptionInformation[1]);
			else if(exceptInfo->ExceptionRecord->ExceptionInformation[0] == 8)
				sprintf(sExtInfo, "ACCESS_VIOLATION DEP violation 0x%llx", exceptInfo->ExceptionRecord->ExceptionInformation[1]);
			else sprintf(sExtInfo, "ACCESS_VIOLATION I0:%llx I1:%llx", exceptInfo->ExceptionRecord->ExceptionInformation[0], 
				exceptInfo->ExceptionRecord->ExceptionInformation[1]);
		}
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT   : sCodeName = "DATATYPE_MISALIGNMENT"; break;
	case EXCEPTION_BREAKPOINT              : sCodeName = "BREAKPOINT"; break;
	case EXCEPTION_SINGLE_STEP             : sCodeName = "SINGLE_STEP"; break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED   : sCodeName = "ARRAY_BOUNDS_EXCEEDED"; break;
	case EXCEPTION_FLT_DENORMAL_OPERAND    : sCodeName = "FLT_DENORMAL_OPERAND"; break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO      : sCodeName = "FLT_DIVIDE_BY_ZERO"; break;
	case EXCEPTION_FLT_INEXACT_RESULT      : sCodeName = "FLT_INEXACT_RESULT"; break;
	case EXCEPTION_FLT_INVALID_OPERATION   : sCodeName = "FLT_INVALID_OPERATION"; break;
	case EXCEPTION_FLT_OVERFLOW            : sCodeName = "FLT_OVERFLOW"; break;
	case EXCEPTION_FLT_STACK_CHECK         : sCodeName = "FLT_STACK_CHECK"; break;
	case EXCEPTION_FLT_UNDERFLOW           : sCodeName = "FLT_UNDERFLOW"; break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO      : sCodeName = "INT_DIVIDE_BY_ZERO"; break;
	case EXCEPTION_INT_OVERFLOW            : sCodeName = "INT_OVERFLOW"; break;
	case EXCEPTION_PRIV_INSTRUCTION        : sCodeName = "PRIV_INSTRUCTION"; break;
	case EXCEPTION_IN_PAGE_ERROR           : sCodeName = "IN_PAGE_ERROR"; break;
	case EXCEPTION_ILLEGAL_INSTRUCTION     : sCodeName = "ILLEGAL_INSTRUCTION"; break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: sCodeName = "NONCONTINUABLE_EXCEPTION"; break;
	case EXCEPTION_STACK_OVERFLOW          : sCodeName = "STACK_OVERFLOW"; break;
	case EXCEPTION_INVALID_DISPOSITION     : sCodeName = "INVALID_DISPOSITION"; break;
	case EXCEPTION_GUARD_PAGE              : sCodeName = "GUARD_PAGE"; break;
	case EXCEPTION_INVALID_HANDLE          : sCodeName = "INVALID_HANDLE"; break;
	}

	//输出错误信息以及错误地址
	CodeAddressLineInfo cln;
	cln.getAddressInfo((size_t)exceptInfo->ExceptionRecord->ExceptionAddress);

	char sBuf[8192];
	char *sBufPtr = sBuf;

	sBufPtr += sprintf(sBufPtr, "%8X: %s at address %llX(%s) %s -- %s(%d)\r\n", 
		exceptInfo->ExceptionRecord->ExceptionCode, sCodeName,
		(size_t)exceptInfo->ExceptionRecord->ExceptionAddress, cln.fnName, sExtInfo, 
		cln.fileName, cln.lineNo);

	//输出调用堆栈
	StackFrameList stacker;
	stacker.getStackFrame(exceptInfo->ContextRecord);
	const INT_PTR nCount = stacker.frameAddressList.count();
	if (nCount > 0)
	{
		const char sListStackTrace[] = "\tStack Trace:\r\n";
		memcpy(sBufPtr, sListStackTrace, sizeof(sListStackTrace) - 1);
		sBufPtr += sizeof(sListStackTrace) - 1;

		const size_t* addressList = stacker.frameAddressList;
		for (INT_PTR i=0; i<nCount;++i)
		{
			if (cln.getAddressInfo(addressList[i]))
				sBufPtr += sprintf(sBufPtr, "\t\t+%llX(%s) -- %s(%d)\r\n", addressList[i], cln.fnName, cln.fileName, cln.lineNo);
			else sBufPtr += sprintf(sBufPtr, "\t\t+%llX\r\n", addressList[i]);
		}
	}
	else
	{
		const char sNoStackTrace[] = "\tNo Stack Trace\r\n\r\n";
		memcpy(sBufPtr, sNoStackTrace, sizeof(sNoStackTrace) - 1);
		sBufPtr += sizeof(sNoStackTrace) - 1;
	}

	sBufPtr[0] = 0;
	recorder.write(sBuf, sBufPtr - sBuf);

	if (alright)
	{
		NormalUnHandleExceptionFilter(exceptInfo);
	}
	alright = FALSE;

	return !IsDebuggerPresent();
}
