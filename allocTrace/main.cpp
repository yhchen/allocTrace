// sstServer.cpp : 定义控制台应用程序的入口点。
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
		static const char * szSolvMemLeak =  __FILE_LINE__"): 请解决内存泄露！\r\n";
		OutputDebugStringA(szSolvMemLeak);
		MessageBoxW(NULL, L"程序退出时检测到内存泄露，请检查并解决内存泄露问题！", L"提示", MB_ICONWARNING | MB_SYSTEMMODAL);
		if (IsDebuggerPresent())
			DebugBreak();
	} 
 	return 0;
}
