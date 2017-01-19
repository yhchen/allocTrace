#include "stdafx.h"

extern const char* extractFileName(const char* fullName);

BOOL DoubleBufferedPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hWnd, &ps);
	if(hDC)
	{
		RECT rect;
		GetClientRect(hWnd, &rect);

		HDC hDCMem = CreateCompatibleDC(NULL);
		HBITMAP hBitMapMem = CreateCompatibleBitmap(hDC, rect.right - rect.left, rect.bottom - rect.top);
		HBITMAP hOldBitMap = (HBITMAP)SelectObject(hDCMem, hBitMapMem);

		FillRect(hDCMem, &rect, (HBRUSH)(COLOR_BTNFACE + 1));

		DefWindowProc(hWnd, WM_PAINT, (WPARAM)hDCMem, PRF_CLIENT | PRF_NONCLIENT);

		BitBlt(hDC, 0, 0, rect.right, rect.bottom, hDCMem, 0, 0, SRCCOPY);
		SelectObject(hDCMem, hOldBitMap);

		DeleteObject(hBitMapMem);
		DeleteDC(hDCMem);
		EndPaint(hWnd, &ps);
		return TRUE;
	}
	return FALSE;
}

MainWnd::MainWnd():super()
{
	m_hFont = NULL;
	m_hProcessGroupBox = NULL;
	m_hProcessListBox = NULL;
	m_hMonitorGroupBox = NULL;
	m_hMemoGroupBox = NULL;
	m_hMemo = NULL;
	m_pServerPipe = NULL;
	initWindow();
}

MainWnd::~MainWnd()
{
	if (m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}

void MainWnd::mainLoop()
{
	while (true)
	{
		if (!processMessages())
			break;
		processPipeServer();
		Sleep(1);
	}
	if (m_pServerPipe)
	{
		m_pServerPipe->Active = FALSE;
		delete m_pServerPipe;
		m_pServerPipe = NULL;
	}
}

void MainWnd::showLog(const char* text, ...)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	String sText(0, "[%d-%d-%d %d:%d:%d]", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	va_list args;
	va_start(args, text);
	sText.catWithArgs(0, text, args);
	va_end(args);

	sText += "\r\n";
	SendMessageA(m_hMemo, EM_SETSEL, -1, 0);
	SendMessageA(m_hMemo, EM_REPLACESEL, TRUE, (LPARAM)sText.ptr());
}

LRESULT MainWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (hWnd == m_hWnd)
	{
		switch (uMsg)
		{
		case WM_CREATE:
			{
				initUIComponents();
				ShowWindow(hWnd, SW_SHOW);
			}
			break;
		case WM_CLOSE:
			if (::MessageBoxA(hWnd, "确定退出吗？", "提示", MB_ICONQUESTION | MB_YESNO) == IDYES)
			{
				PostMessage(0, WM_QUIT, 0, 0);
			}
			return 0;
		case WM_SIZE:
			{
				adjustUILayout();
			}
			break;
		case WM_PAINT:
			{
				if (DoubleBufferedPaint(hWnd))
					return 1;
			}
			break;
		case MONITOR_DISCONNECTED:
			updateMonitorListState((AllocTraceMonitor*)wParam);
			return 0;
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	else if (hWnd == m_hProcessGroupBox)
	{
		switch(uMsg)
		{
		case WM_COMMAND:
			{
				if (lParam == (LPARAM)m_hProcessListBox)
				{
					switch (HIWORD(wParam))
					{
					case LBN_SELCHANGE:
						{
							int itemIndex = (int)SendMessageA(m_hProcessListBox, LB_GETCURSEL, 0, 0);
							if (itemIndex > -1)
							{
								AllocTraceMonitor *monitor = m_MonitorList[itemIndex];
								setActiveMonitor(monitor);
							}
						}
						return 0;
					}
				}
			}
			break;
		}
		return m_pProcessGroupBoxWndProc(hWnd, uMsg, wParam, lParam);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool MainWnd::processMessages()
{
	MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT)
			return false;
	}
	return true;
}

#include "os/pipe/NPComm.h"

void MainWnd::processPipeServer()
{
	static const TCHAR* sPipeName = NP_DEFAULT_PIPENAME;

	if (m_pServerPipe == NULL)
	{
		m_pServerPipe = new CNamedPipeServer();
		m_pServerPipe->SetInBufferSize(10 * 1024 * 1024);
		m_pServerPipe->SetOutBufferSize(10 * 1024 * 1024);
		m_pServerPipe->SetPipeName(sPipeName);
	}
	if (!m_pServerPipe->Active)
	{
		m_pServerPipe->Active = TRUE;
		if (m_pServerPipe->Active)
		{
			showLog("管道'%s'已打开，等待客户端连接...", UTF8String(String(sPipeName)).ptr());
		}
	}
	if (m_pServerPipe->GetAvaliableReadSize() >= sizeof(DWORD))
	{
		DWORD dwProcId = 0;
		m_pServerPipe->ReadBuf(&dwProcId, sizeof(dwProcId));
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcId);
		if (hProcess)
		{
			char sModName[1024];
			DWORD dwNameLen = sizeof(sModName) - 1;
			if (QueryFullProcessImageNameA(hProcess, 0, sModName, &dwNameLen))
			{
				const char* sNamePtr = extractFileName(sModName);

				AllocTraceMonitor *monitor = new AllocTraceMonitor(hProcess, dwProcId, sModName, m_pServerPipe, m_hWnd);
				m_MonitorList.add(monitor);
				monitor->release();

				while (!monitor->getHwnd())
				{
					Sleep(1);
				}

				int pos = (int)SendMessageA(m_hProcessListBox, LB_ADDSTRING, 0, (LPARAM)sNamePtr);
				//SendMessageA(m_hProcessListBox, LB_SETITEMDATA, pos, (LPARAM)monitor); 

				if (pos == 0)
					setActiveMonitor(monitor);

				m_pServerPipe = NULL;
				showLog("进程%d已连接，开始接收内存监视数据", dwProcId);
			}
			else 
			{
				m_pServerPipe->Close();
				showLog("无法查询进程%d模块名，连接被关闭", dwProcId);
			}
		}
		else 
		{
			m_pServerPipe->Close();
			showLog("无法打开进程%d，连接被关闭", dwProcId);
		}
	}
}

void MainWnd::initWindow()
{
	const char* szWndClassName = "ALLOCTRACESERVERWND";
	static ATOM WndClassAtom = 0;
	if (WndClassAtom == 0)
	{
		WNDCLASSEXA wcex;

		wcex.cbSize = sizeof(wcex);

		wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wcex.lpfnWndProc	= getClassedWndProc();
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= GetModuleHandle(NULL);
		wcex.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= szWndClassName;
		wcex.hIconSm		= NULL;

		WndClassAtom = ::RegisterClassExA(&wcex);
	}
	MyCreateWindowEx(0, szWndClassName, "AllocTrace 1.0.2 Beta", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, GetModuleHandle(NULL), NULL);
}

void MainWnd::initUIComponents()
{
	HWND hParent = getHwnd();
	HINSTANCE hInstance = GetModuleHandle(NULL);

	//初始化字体
	m_hFont = CreateFontA(FONTSIZE,0,0,0,FW_NORMAL,
		FALSE,FALSE,FALSE,DEFAULT_CHARSET,
		OUT_RASTER_PRECIS,CLIP_DEFAULT_PRECIS,
		VARIABLE_PITCH|PROOF_QUALITY,FF_DONTCARE,FONTNAME);
	SendMessageA(hParent, WM_SETFONT, (WPARAM)m_hFont, TRUE);

	//初始化进程列表外框
	m_hProcessGroupBox = CreateWindowA("BUTTON", "已连接进程", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		0, 0, 100, 100, hParent, NULL, hInstance, 0);
	SendMessageA(m_hProcessGroupBox, WM_SETFONT, (WPARAM)m_hFont, TRUE);
	m_pProcessGroupBoxWndProc = (WNDPROC)SetWindowLongPtr(m_hProcessGroupBox, GWLP_WNDPROC, (ULONG_PTR)m_pClassWndProcInst);

	//初始化进程列表框
	m_hProcessListBox = CreateWindowA("LISTBOX", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_BORDER | LBS_NOTIFY,
		0, 0, 100, 100, m_hProcessGroupBox, NULL, hInstance, 0);
	SendMessageA(m_hProcessListBox, WM_SETFONT, (WPARAM)m_hFont, TRUE);

	//初始化监视器外框
	m_hMonitorGroupBox = CreateWindowA("BUTTON", "监视窗口", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		0, 0, 100, 100, hParent, NULL, hInstance, 0);
	SendMessageA(m_hMonitorGroupBox, WM_SETFONT, (WPARAM)m_hFont, TRUE);

	//初始化文本框外框
	m_hMemoGroupBox = CreateWindowA("BUTTON", "日志窗口", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		0, 0, 100, 100, hParent, NULL, hInstance, 0);
	SendMessageA(m_hMemoGroupBox, WM_SETFONT, (WPARAM)m_hFont, TRUE);

	//初始化文本框
	m_hMemo = CreateWindowA("Edit", NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_READONLY, 
		0, 0, 100, 100, 
		m_hMemoGroupBox, NULL, hInstance, 0);
	SendMessageA(m_hMemo, WM_SETFONT, (WPARAM)m_hFont, TRUE);

	//调整UI布局
	adjustUILayout();
}

void MainWnd::adjustUILayout()
{
	RECT rWnd;
	GetClientRect(getHwnd(), &rWnd);
	const int wndWidth = rWnd.right - rWnd.left;
	const int wndHeight = rWnd.bottom - rWnd.top;
	const int MemoGroupHeight = 200;
	const int ProcessGroupWidth = 200;

	
	MoveWindow(m_hProcessGroupBox, 0, 0, ProcessGroupWidth, wndHeight - MemoGroupHeight, TRUE);
	MoveWindow(m_hProcessListBox, 4, 16, ProcessGroupWidth - 8, wndHeight - MemoGroupHeight - 20, TRUE);

	MoveWindow(m_hMonitorGroupBox, ProcessGroupWidth, 0, wndWidth - ProcessGroupWidth, wndHeight - MemoGroupHeight, TRUE);

	MoveWindow(m_hMemoGroupBox, 0, wndHeight - MemoGroupHeight, wndWidth, MemoGroupHeight, TRUE);
	MoveWindow(m_hMemo, 4, 16, wndWidth - 8, MemoGroupHeight - 20, TRUE);

	adjustActiveMonitorLayout();
}

void MainWnd::adjustActiveMonitorLayout()
{
	if (m_pActoveMonitor)
	{
		RECT r;
		GetClientRect(m_hMonitorGroupBox, &r);
		MoveWindow(m_pActoveMonitor->getHwnd(), 4, 16, r.right - r.left - 8, r.bottom - r.top - 20, TRUE);
	}
}

void MainWnd::setActiveMonitor(AllocTraceMonitor *monitor)
{
	if (m_pActoveMonitor != monitor)
	{
		if (m_pActoveMonitor)
		{
			m_pActoveMonitor->closeWnd();
		}
		m_pActoveMonitor = monitor;
		if (m_pActoveMonitor)
		{
			SetParent(m_pActoveMonitor->getHwnd(), m_hMonitorGroupBox);
			SetWindowLong(m_pActoveMonitor->getHwnd(), GWL_STYLE, WS_CHILDWINDOW);
			adjustActiveMonitorLayout();
			m_pActoveMonitor->showWnd();
		}
	}
}

void MainWnd::updateMonitorListState(AllocTraceMonitor *monitor)
{
	const int nIdx = m_MonitorList.indexOf(monitor);
	if (nIdx > -1)
	{
		SendMessageA(m_hProcessListBox, LB_DELETESTRING, nIdx, 0);
		String sText = extractFileName(monitor->moduleName().ptr());
		sText += "[已断开]";
		SendMessageA(m_hProcessListBox, LB_INSERTSTRING, nIdx, (LPARAM)sText.ptr());
	}
}
