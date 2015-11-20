#pragma once

class MainWnd : public wylib::window::CClassedWnd
{
public:
	typedef CClassedWnd super;

private:
	HFONT				m_hFont;//����
	HWND				m_hProcessGroupBox;//�����б����
	HWND				m_hProcessListBox;//�����б��
	WNDPROC				m_pProcessGroupBoxWndProc;//�����б��ԭʼWndProc
	HWND				m_hMonitorGroupBox;//���������
	HWND				m_hMemoGroupBox;//�ı������
	HWND				m_hMemo;//�ı���
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
