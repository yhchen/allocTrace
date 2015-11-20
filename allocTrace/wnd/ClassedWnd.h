#ifndef _CLASSED_WINDOW_PROC_H_
#define _CLASSED_WINDOW_PROC_H_

/***********************************************************************/
/*                                                                     */
/*                 �����ڻص��������õ���Ĵ��ڴ������еĴ��ڻ�����           */
/*                 �������ͨ������WndProc������ʵ�������е���Ϣ����           */
/*                          �����಻�Ƕ��̰߳�ȫ�ģ�                       */
/*                                                                     */
/*  ��ʵ��ԭ���飺                                                      */
/*    Windows������Ϣ����֧�ַ�ҳ����Ҳ�޷�������صĴ�͸������ʵ�ֶ���ָ���   */
/*  ���ݡ���MFC�У�ͨ�������д��ڵĴ�����ָ��һ���̶��ĺ������ڴ������͵�ʱ��Ὣ   */
/*  ���ڵ�����ע�ᵽһ��ȫ�ֵ�map�У�������Ϣ�����ʱ���map�в��Ҵ��ھ����Ӧ����   */
/*  ʵ������������Ϣ�������������������ڴ�������϶�����ϢƵ����ʱ����ͨ��map����   */
/*  ��ʵ����ʱ���нϴ������ƿ������Ϊÿ����һ����Ϣ��Ҫ�ӡ�����-ʵ����ӳ����в��Ҵ�   */
/*  �ڶ�Ӧ����ʵ����                                                       */
/*                                                                      */
/*    ����ҳ��Ϣ����ģ�Ͳο���VCL�ķ�ҳ����Ϣ������ƣ���������ʱ��̬���ڴ�ִ�д���   */
/*  ��������ʽʵ�ִ�����Ϣ���໯����û���κεĲ�ѯ�����Ķ��⿪�������ܽϸߡ�        */
/*                                                                      */
/*  ��ʹ�÷�ʽ��                                                           */
/*    1) �̳д��࣬���Ǳ�������WndProc�Ա������Ϣ����                       */
/*    2) ��ע�ᴰ�����͵�ʱ�򣬽����ڴ���������ΪgetClassedWndProc()�����ķ�   */
/*       ��ֵ��                                                          */
/*    3) ��������ʵ����ʱ�򣬵���MyCreateWindowEx��Ҫ����ȫ�ֵ�CreateWindow��
/*       CreateWindowEx����;                                             */
/*                                                                      */
/************************************************************************/
#include "Platform.h"
#if PLATFORM == WINDOWS32 || PLATFORM == WINDOWS64
namespace wylib
{
	namespace window
	{
		class CClassedWnd
		{
		public:
			//��ȡ��������Ĵ��ھ��
			inline HWND getHwnd(){ return m_hWnd; }
			//��ȡ�������WndProc�������Ա���RegistClass��SetWindowLong������֧���ຯ��������Ϣ����Ĵ��ڻ���
			//��ע�����ͻ�ı䴰�ڴ�������ʱ��ʹ�ô˺����ķ���ֵ��Ϊ���ڴ�������������ȷ��ʵ�ִ�����Ϣ���໯����
			WNDPROC getClassedWndProc();

		protected:
			//������Ϣ������躯��
			//������븲�Ǵ˺�����ʵ�ֶԴ�����Ϣ�Ĵ���
			virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		protected:
			//ͨ���ڲ�ʵ��CreateWindowEx�����Ա���ɶԴ�����Ϣ�໯��������Ҫ�Ĳ���
			HWND MyCreateWindowEx(DWORD dwExStyle,
				const char* lpClassName,
				const char* lpWindowName,
				DWORD dwStyle,
				int x,
				int y,
				int nWidth,
				int nHeight,
				HWND hWndParent,
				HMENU hMenu,
				HINSTANCE hInstance,
				LPVOID lpParam
				);

		private:
			/*****************************************************************************/
			/*      ���º������ԱΪ��˽�����ݣ�Ϊʵ�ִ�����Ϣ���໯������ڲ�ʵ�ֵĺ�������������     */
			/*****************************************************************************/
			static CClassedWnd* CreationWnd;
			static LRESULT APIENTRY InitClassedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			static LRESULT APIENTRY StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, CClassedWnd *pWnd);

		protected:
			HWND		m_hWnd;				//���ھ��
			WNDPROC		m_pClassWndProcInst;//������Ϣ�໯����ʵ��

		public:
			CClassedWnd(void);
			virtual ~CClassedWnd(void);
		};
	};
};
#endif
#endif

