#pragma once

class MainWnd : public wylib::window::CClassedWnd
{
public:
	typedef CClassedWnd super;

private:
	HFONT				m_hFont;//字体
	HWND				m_hProcessGroupBox;//进程列表外框
	HWND				m_hProcessListBox;//进程列表框
	WNDPROC				m_pProcessGroupBoxWndProc;//进程列表框原始WndProc
	HWND				m_hMonitorGroupBox;//监视器外框
	HWND				m_hMemoGroupBox;//文本框外框
	HWND				m_hMemo;//文本框
	CNamedPipeServer*	m_pServerPipe;
	SG2D::ObjectArray<AllocTraceMonitor> m_MonitorList;
	SG2D::SmartObject<AllocTraceMonitor> m_pActoveMonitor;
public:
	MainWnd();
	~MainWnd();

	void mainLoop();
	void showLog(const char* text, ...);

protected:
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	bool processMessages();
	void processPipeServer();

private:
	void initWindow();
	void initUIComponents();
	void adjustUILayout();
	void adjustActiveMonitorLayout();
	void setActiveMonitor(AllocTraceMonitor *monitor);
	void updateMonitorListState(AllocTraceMonitor *monitor);
};
