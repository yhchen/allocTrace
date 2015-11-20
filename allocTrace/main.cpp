// sstServer.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include <imagehlp.h>
#pragma warning(disable:4996)

const TCHAR szExceptionDumpFile[] = _T("./allocTrace.dmp");

int MemoryLeakReportRoutine(int blockType, char *message, int *p);


int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd)
{
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
	_CrtSetReportHook(&MemoryLeakReportRoutine);
#endif
	SetUnhandledExceptionFilter( DefaultUnHandleExceptionFilter );

	CoInitialize(NULL);

	MainWnd *mainWnd = new MainWnd();
	mainWnd->mainLoop();
	delete mainWnd;

	CoUninitialize();
	return 0;
}

static int LeakReportCount = 0;
int MemoryLeakReportRoutine(int blockType, char *message, int *p)
{
	LeakReportCount++;
	if (LeakReportCount > 9 && strcmp(message, "Object dump complete.\n") == 0)
	{
		static const char * szSolvMemLeak =  __FILE_LINE__"): �����ڴ�й¶��\r\n";
		OutputDebugStringA(szSolvMemLeak);
		MessageBoxW(NULL, L"�����˳�ʱ��⵽�ڴ�й¶�����鲢����ڴ�й¶���⣡", L"��ʾ", MB_ICONWARNING | MB_SYSTEMMODAL);
		if (IsDebuggerPresent())
			DebugBreak();
	} 
 	return 0;
}
