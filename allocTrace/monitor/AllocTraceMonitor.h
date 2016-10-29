#ifndef __WIN32_ALLOC_TRACE_SERVER_H__
#define __WIN32_ALLOC_TRACE_SERVER_H__

#include "alloctrace.h"
#include "AllocTraceMonitorNode.hpp"

#define MONITOR_DISCONNECTED	 (WM_USER + 8001)

using namespace alloctrace::os::pipe;
using namespace alloctrace::allocator;
using namespace alloctrace;

class AllocTraceMonitorFile;

class AllocTraceMonitor : public Object, public wylib::window::CClassedWnd
{
public:
	typedef CClassedWnd super;
	typedef SG2D::StringHashMap<int> NameMap;
	//typedef unordered_map<SG2D::String, int> NameMap;
	typedef unordered_map<const void*, AllocBlockRecord> BlockMap;
	typedef unordered_map<size_t, SG2D::String> SymbolMap;

protected:
	HANDLE m_hMainWorkThread;
	HANDLE m_hPipeWorkThread;
	volatile BOOL m_boStoped;
	HWND m_hNotifyWnd;
	DWORD m_dwPID;
	HANDLE m_hProcess;
	String m_sModuleName;
	DWORD64 m_dwRefStatInfoTick;
	CNamedPipeServer *m_pPipe;
	AllocTraceMonitorFile *m_pFile;
	HFONT m_hFont;
	HWND m_hInfoMemo;
	//HWND m_hTreeView;
	TREELIST_HANDLE m_hTreeList;
	CSingleObjectAllocator<AllocCallNode> m_NodeAllocator;
	NameMap m_NameMap;
	BlockMap m_BlockMap;
	SymbolMap m_SymbolMap;
	AllocCallNode *m_pRootNode;
	BOOL m_boSymbolLoaded;
	BOOL m_boTreeDataDirty;

public:
	AllocTraceMonitor(HANDLE hProcess, DWORD dwPID, const char* sModeName, CNamedPipeServer *pPipe, HWND hNotifyWnd);
	~AllocTraceMonitor();

	inline alloctrace::stream::CBaseStream* file(){ return (alloctrace::stream::CBaseStream*)m_pFile; }
	inline HANDLE processHandle() const { return m_hProcess; }
	inline const String& moduleName() const { return m_sModuleName; }

	void showWnd();
	void closeWnd();

protected:
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void threadMain();
	void readPipeThreadMain();
	void initWindow();
	void adjustUILayout();
	void processMessages();
	void processTraceData();
	void recordAlloc(const AllocRecord &rec, size_t *frames);
	void recordFree(const AllocRecord &rec);
	void createTreeNode(AllocCallNode* node);
	void formatTreeNodeText(AllocCallNode* node, char* buffer, int cchMax);
	void getSymbolName(size_t address, String& name);
private:
	static void CALLBACK MainThreadProc(AllocTraceMonitor* thiz);
	static void CALLBACK PipeThreadProc(AllocTraceMonitor* thiz);
};

#endif
