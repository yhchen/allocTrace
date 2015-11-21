#include "stdafx.h"

using namespace alloctrace;
using namespace alloctrace::container;
using namespace alloctrace::os;
using namespace alloctrace::stream;
using namespace SG2D;

#pragma warning(disable:4996)

#include "AllocTraceMonitorFile.hpp"

extern BOOL DoubleBufferedPaint(HWND hWnd);

static char* size2Str(char* buffer, size_t size)
{
	static const double KB = 1024;
	static const double MB = KB * 1024;
	static const double GB = MB * 1024;

	if (size >= GB)
		sprintf(buffer, "%.1lfG", size / GB);
	else if (size >= MB)
		sprintf(buffer, "%.1lfM", size / MB);
	else if (size >= KB)
		sprintf(buffer, "%.1lfK", size / KB);
	else sprintf(buffer, "%lluB", (unsigned long long)size);
	return buffer;
}

const char* extractFileName(const char* fullName)
{
	const char* sModName = strrchr(fullName, '\\');
	if (sModName)
		sModName++;
	else sModName = fullName;
	return sModName;
}

#define WM_UPDATE_STATE_INFO	(WM_USER + 91001)
#define WM_ADD_NEW_CALLNODE		(WM_USER + 91002)

//------------------------------------------------------------------------//
//
//							AllocTraceMonitor
//
//------------------------------------------------------------------------//
AllocTraceMonitor::AllocTraceMonitor(HANDLE hProcess, DWORD dwPID, const char* sModName, CNamedPipeServer *pPipe, HWND hNotifyWnd):super()
{
	m_hMainWorkThread = NULL;
	m_hPipeWorkThread = NULL;
	m_boStoped = FALSE;
	m_hNotifyWnd = hNotifyWnd;
	m_hFont = NULL;
	m_hInfoMemo = NULL;
	//m_hTreeView = NULL;
	m_hTreeList = NULL;
	m_hProcess = hProcess;
	m_dwPID = dwPID;
	m_sModuleName = sModName;
	m_dwRefStatInfoTick = 0;
	m_pPipe = pPipe;

	m_pFile = new AllocTraceMonitorFile();
	m_pRootNode = m_NodeAllocator.allocObject();
	m_pRootNode->init(extractFileName(sModName));

	m_boSymbolLoaded = SymInitialize(hProcess, NULL, TRUE);
	m_boTreeDataDirty = FALSE;

	initWindow();

	m_hMainWorkThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&AllocTraceMonitor::MainThreadProc, this, 0, NULL);
	m_hPipeWorkThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&AllocTraceMonitor::PipeThreadProc, this, 0, NULL);
}

AllocTraceMonitor::~AllocTraceMonitor()
{
	if (m_hMainWorkThread || m_hPipeWorkThread)
	{
		m_boStoped = TRUE;
		if (m_hPipeWorkThread)
		{
			WaitForSingleObject(m_hPipeWorkThread, INFINITE);
			CloseHandle(m_hPipeWorkThread);
			m_hPipeWorkThread = NULL;
		}
		if (m_hMainWorkThread)
		{
			WaitForSingleObject(m_hMainWorkThread, INFINITE);
			CloseHandle(m_hMainWorkThread);
			m_hMainWorkThread = NULL;
		}
	}
	if (m_hProcess)
	{
		SymCleanup(m_hProcess);
		CloseHandle(m_hProcess);
		m_hProcess = NULL;
		m_boSymbolLoaded = FALSE;
	}
	if (m_pPipe)
	{
		delete m_pPipe;
		m_pPipe = NULL;
	}
	m_pRootNode->uninit();
	m_NodeAllocator.freeAll();
	if (m_hTreeList)
	{
		TreeListDestroy(m_hTreeList);
		m_hTreeList = NULL;
	}
	if (m_pFile)
	{
		delete m_pFile;
	}
	if (m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}

void AllocTraceMonitor::showWnd()
{
	ShowWindow(getHwnd(), SW_SHOW);
}

void AllocTraceMonitor::closeWnd()
{
	ShowWindow(getHwnd(), SW_HIDE);
}

LRESULT AllocTraceMonitor::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CREATE:
		if (!m_hInfoMemo)
		{
			HINSTANCE hInstance = GetModuleHandle(NULL);
			//初始化字体
			m_hFont = CreateFontA(FONTSIZE,0,0,0,FW_NORMAL,
				FALSE,FALSE,FALSE,DEFAULT_CHARSET,
				OUT_RASTER_PRECIS,CLIP_DEFAULT_PRECIS,
				VARIABLE_PITCH|PROOF_QUALITY,FF_DONTCARE,FONTNAME);
			SendMessageA(hWnd, WM_SETFONT, (WPARAM)m_hFont, TRUE);

			m_hInfoMemo = CreateWindowA("Edit", NULL,
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_MULTILINE | ES_WANTRETURN | ES_READONLY, 
				0, 0, 100, 100, hWnd, NULL, hInstance, 0);
			SendMessageA(m_hInfoMemo, WM_SETFONT, (WPARAM)m_hFont, TRUE);

			//m_hTreeView = CreateWindowA("SysTreeView32", "AllocTree", 
			//	WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS, 
			//	0, 0, 100, 300, hWnd, NULL, hInstance, 0);
			//SendMessageA(m_hTreeView, WM_SETFONT, (WPARAM)m_hFont, TRUE);

			RECT r;
			GetClientRect(hWnd, &r);
			r.top += 22;
			m_hTreeList = TreeListCreate(hInstance, hWnd, &r, TREELIST_ANCHOR_RIGHT|TREELIST_ANCHOR_BOTTOM, NULL);
			TreeListAddColumn(m_hTreeList, _TEXT("函数"), 500);
			TreeListAddColumn(m_hTreeList, _TEXT("活跃内存"), 100);
			TreeListAddColumn(m_hTreeList, _TEXT("活跃块"), 100);
			TreeListAddColumn(m_hTreeList, _TEXT("历史内存"), 100);
			TreeListAddColumn(m_hTreeList, _TEXT("历史块"), TREELIST_LAST_COLUMN);

			adjustUILayout();
			createTreeNode(m_pRootNode);
		}
		break;
	case WM_SIZE:
		adjustUILayout();
		break;
	case WM_CLOSE:
		if (m_hProcess)
		{
			MessageBoxA(hWnd, "目标进程退出前不可关闭监视窗口", "提示", MB_ICONWARNING);
			return 0;
		}
		break;
	case WM_PAINT:
		{
			if (DoubleBufferedPaint(hWnd))
				return 1;
		}
		break;
// 	case WM_NOTIFY:
// 		{
// 			LPNMTVDISPINFO lpDispInfo =(LPNMTVDISPINFO)lParam;
// 			if ((lpDispInfo->hdr.hwndFrom == m_hTreeView) && (LOWORD(lpDispInfo->hdr.code) == 0xFE6D))
// 			{
// 				if (lpDispInfo->item.mask & TVIF_TEXT)
// 				{
// 					AllocCallNode *node = (AllocCallNode*)lpDispInfo->item.lParam;
// 					if (node)
// 					{
// 						formatTreeNodeText(node, lpDispInfo->item.pszText, lpDispInfo->item.cchTextMax);
// 						return 1;
// 					}
// 				}
// 			}
// 		}
		break;
	case WM_UPDATE_STATE_INFO:
		{
			char sInfoText[512];
			char sRemSize[32];
			sprintf(sInfoText, "[%s:%d]%s 剩余数据：%s", extractFileName(m_sModuleName.ptr()), 
				m_dwPID, m_hProcess ? "" : "已断开", size2Str(sRemSize, m_pFile->remainSize()));
			SetWindowTextA(m_hInfoMemo, sInfoText);

			if (m_boTreeDataDirty)
			{
				m_boTreeDataDirty = FALSE;
				//InvalidateRect(m_hTreeView, NULL, TRUE);
				//InvalidateRect(m_hWnd, NULL, TRUE);
				InvalidateRect(TreeListGetTreeHwnd(m_hTreeList), NULL, TRUE);
			}
		}
		return 0;
	case WM_ADD_NEW_CALLNODE:
		{
			createTreeNode((AllocCallNode*)wParam);
		}
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void AllocTraceMonitor::threadMain()
{
	while (!m_boStoped)
	{
		processTraceData();
		Sleep(2);
	}
}

void AllocTraceMonitor::readPipeThreadMain()
{
	while (!m_boStoped && m_pPipe->Active)
	{
		m_pFile->readFromPipe(m_pPipe);
		Sleep(4);
	}
}

void AllocTraceMonitor::initWindow()
{
	const char* szWndClassName = "SST-ALLOC-TRACE-MINITOR";
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
		wcex.hIcon			= NULL;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= szWndClassName;
		wcex.hIconSm		= NULL;

		WndClassAtom = ::RegisterClassExA(&wcex);
	}
	HWND hWnd = MyCreateWindowEx(0, szWndClassName, "AllocMonitorWindow", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, GetModuleHandle(NULL), NULL);
}

void AllocTraceMonitor::adjustUILayout()
{
	if (m_hInfoMemo)
	{
		RECT r;
		GetClientRect(getHwnd(), &r);
		const int InfoMemoHeight = 22;

		MoveWindow(m_hInfoMemo, 0, 0, r.right - r.left, InfoMemoHeight, TRUE);
		//MoveWindow(m_hTreeView, 0, InfoMemoHeight, r.right - r.left, r.bottom - r.top - InfoMemoHeight, TRUE);
	}
}

void AllocTraceMonitor::processMessages()
{
	MSG msg;
	HWND hWnd = getHwnd();
	while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void AllocTraceMonitor::processTraceData()
{
	AllocRecord rec;
	size_t frames[256];
	DWORD64 dwStart = ::GetTickCount64();
	while (true)
	{
		if (!m_pFile->readRecord(rec, frames))
			break;
		switch(rec.type)
		{
		case atAlloc:
			recordAlloc(rec, frames);
			break;
		case atFree:
			recordFree(rec);
			break;
		}
		if (::GetTickCount64() - dwStart >= 500)
			break;
	}

	DWORD64 dwCurrTick = ::GetTickCount64();
	if (dwCurrTick >= m_dwRefStatInfoTick)
	{
		m_dwRefStatInfoTick = dwCurrTick + 1000;
		PostMessage(m_hWnd, WM_UPDATE_STATE_INFO, 0, 0);

		DWORD dwExitCode;
		if (m_pFile->remainSize() <= 0 && GetExitCodeProcess(m_hProcess, &dwExitCode) && dwExitCode != STILL_ACTIVE)
		{
			SymCleanup(m_hProcess);
			CloseHandle(m_hProcess);
			m_hProcess = NULL;
			m_boSymbolLoaded = FALSE;
			PostMessage(m_hNotifyWnd, MONITOR_DISCONNECTED, (WPARAM)this, 0);
		}
	}
}

void AllocTraceMonitor::recordAlloc(const AllocRecord &rec, size_t *frames)
{
	AllocCallNode *parentNode = m_pRootNode;
	String fnName;

	parentNode->totalBytes += rec.size;
	parentNode->aliveBytes += rec.size;
	parentNode->totalBlocks++;
	parentNode->aliveBlocks++;

	for (int i=rec.numFrames-1; i>-1; --i)
	{
		getSymbolName(frames[i], fnName);

		NameMap::iterator it = m_NameMap.find(fnName);
		if (it != m_NameMap.end())
			fnName = it->first;
		else m_NameMap.insert(pair<String, int>(fnName, 0));

		AllocCallNode* node = parentNode->getChild(fnName);
		if (!node)
		{
			node = m_NodeAllocator.allocObject();
			node->init(fnName);
			node->sibling = parentNode->children;
			node->parent = parentNode;
			parentNode->children = node;

			//添加TreeView节点
			//createTreeNode(node, NULL, (HTREEITEM)parentNode->hTreeNode);
			PostMessage(m_hWnd, WM_ADD_NEW_CALLNODE, (WPARAM)node, 0);
		}
		node->totalBytes += rec.size;
		node->aliveBytes += rec.size;
		node->totalBlocks++;
		node->aliveBlocks++;

		parentNode = node;
	}

	AllocBlockRecord block;
	block.blockSize = rec.size;
	block.node = parentNode;
	m_BlockMap.insert(std::pair<void*, AllocBlockRecord>(rec.ptr, block));
	m_boTreeDataDirty = TRUE;
}

void AllocTraceMonitor::recordFree(const AllocRecord &rec)
{
	BlockMap::iterator it = m_BlockMap.find(rec.ptr);
	if (it != m_BlockMap.end())
	{
		AllocBlockRecord &block = it->second;
		AllocCallNode* node = block.node;
		while (node)
		{
			node->aliveBytes -= block.blockSize;
			node->aliveBlocks--;
			node = node->parent;
		}
		m_BlockMap.erase(it);
			m_boTreeDataDirty = TRUE;
	}
}

VOID _stdcall GetTreeListColumnTextCallBack(const TREELIST_HANDLE, int ColumnIndex, void *pExternalPtr, char *lpText, int cchTextMax)
{
	AllocCallNode *node = (AllocCallNode*)pExternalPtr;
	switch(ColumnIndex)
	{
	case 0:
		strncpy(lpText, node->fnName.ptr(), cchTextMax - 1);
		lpText[cchTextMax - 1] = 0;
		break;
	case 1:
		size2Str(lpText, node->aliveBytes);
		break;
	case 2:
		ultoa((unsigned long)node->aliveBlocks, lpText, 10);
		break;
	case 3:
		size2Str(lpText, node->totalBytes);
		break;
	case 4:
		ultoa((unsigned long)node->totalBlocks, lpText, 10);
		break;
	default:
		lpText[0] = 0;
		break;
	}
}

void AllocTraceMonitor::createTreeNode(AllocCallNode* node)
{
// 	TVINSERTSTRUCT tvInst;
// 	tvInst.item.mask = TVIF_TEXT | TVIF_PARAM;
// 	tvInst.hInsertAfter = TVI_LAST;
// 	tvInst.hParent = (HTREEITEM)hParentNode;
// 	tvInst.item.pszText = LPSTR_TEXTCALLBACK;//text;
// 	tvInst.item.lParam = (LPARAM)node;
// 	node->hTreeNode = (HTREEITEM)SendMessageA(m_hTreeView, TVM_INSERTITEM, 0, (LPARAM)&tvInst);
	TreeListNodeData cols[5], *pCol;
	memset(cols, 0, sizeof(cols));
	pCol = cols;

	pCol->TextCB = &GetTreeListColumnTextCallBack;
	pCol->pExternalPtr = node;
	pCol++;

	pCol->TextCB = &GetTreeListColumnTextCallBack;
	pCol->pExternalPtr = node;
	pCol++;

	pCol->TextCB = &GetTreeListColumnTextCallBack;
	pCol->pExternalPtr = node;
	pCol++;

	pCol->TextCB = &GetTreeListColumnTextCallBack;
	pCol->pExternalPtr = node;
	pCol++;

	pCol->TextCB = &GetTreeListColumnTextCallBack;
	pCol->pExternalPtr = node;
	pCol++;

	node->hTreeNode = TreeListAddNode(m_hTreeList, (node->parent) ? node->parent->hTreeNode : NULL, cols, 4);
}

void AllocTraceMonitor::formatTreeNodeText(AllocCallNode* node, char* buffer, int cchMax)
{
	char sNameBuf[256];
	char sAliveBytes[32], sTotalBytes[32];

	const int MaxNameLen = 180;
	const char* sNamePtr = node->fnName.ptr();
	if (node->fnName.length() >= MaxNameLen)
	{
		strncpy(sNameBuf, sNamePtr, MaxNameLen);
		sNameBuf[MaxNameLen] = '.';
		sNameBuf[MaxNameLen + 1] = '.';
		sNameBuf[MaxNameLen + 2] = '.';
		sNameBuf[MaxNameLen + 3] = 0;
		sNamePtr = sNameBuf;
	}
	
	buffer[sprintf_s(buffer, cchMax - 1, "%s AliveBytes:%s AliveBlocks:%llu TotalBytes:%s TotalBlocks:%llu",
		sNamePtr, size2Str(sAliveBytes, node->aliveBytes), node->aliveBlocks, 
		size2Str(sTotalBytes, node->totalBytes), node->totalBlocks	)] = 0;
}

void AllocTraceMonitor::getSymbolName(size_t address, SG2D::String& name)
{
	SymbolMap::iterator it = m_SymbolMap.find(address);
	if (it != m_SymbolMap.end())
	{
		name = it->second;
	}
	else
	{
		PSYMBOL_INFO pSymbol;
		char symbBuf[sizeof(*pSymbol) + 1024];
		pSymbol = (PSYMBOL_INFO)symbBuf;
		pSymbol->SizeOfStruct = sizeof(*pSymbol);
		pSymbol->MaxNameLen = sizeof(symbBuf) - sizeof(*pSymbol);

		if (SymFromAddr(m_hProcess, address, NULL, pSymbol))
			name = pSymbol->Name;
		else name.format("%llx", address);
		m_SymbolMap.insert(pair<size_t, SG2D::String>(address, name));
	}
}

void AllocTraceMonitor::MainThreadProc(AllocTraceMonitor* thiz)
{
	CoInitialize(NULL);
	thiz->threadMain();
	CoUninitialize();
}

void AllocTraceMonitor::PipeThreadProc(AllocTraceMonitor* thiz)
{
	thiz->readPipeThreadMain();
}
