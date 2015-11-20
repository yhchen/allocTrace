#ifndef _CLASSED_WINDOW_PROC_H_
#define _CLASSED_WINDOW_PROC_H_

/***********************************************************************/
/*                                                                     */
/*                 将窗口回调函数调用到类的窗口处理函数中的窗口基础类           */
/*                 子类可以通过覆盖WndProc函数来实现在类中的消息处理           */
/*                          ！此类不是多线程安全的！                       */
/*                                                                     */
/*  ★实现原理简介：                                                      */
/*    Windows窗口消息处理不支持分页处理，也无法传递相关的穿透参数来实现对类指针的   */
/*  传递。在MFC中，通过将所有窗口的处理函数指向一个固定的函数，在创建类型的时候会将   */
/*  窗口的类型注册到一个全局的map中，并在消息处理的时候从map中查找窗口句柄对应的类   */
/*  实例并调用其消息处理函数。这样的做法在窗口组件较多且消息频繁的时候在通过map查找   */
/*  类实例的时候有较大的性能瓶颈，因为每处理一个消息都要从“窗口-实例”映射表中查找窗   */
/*  口对应的类实例。                                                       */
/*                                                                      */
/*    本分页消息处理模型参考自VCL的分页化消息处理机制，采用运行时动态打内存执行代码   */
/*  补丁的形式实现窗口消息的类化处理，没有任何的查询操作的额外开销，性能较高。        */
/*                                                                      */
/*  ★使用方式：                                                           */
/*    1) 继承此类，覆盖保护函数WndProc以便完成消息处理；                       */
/*    2) 在注册窗口类型的时候，将窗口处理函数设置为getClassedWndProc()函数的返   */
/*       回值；                                                          */
/*    3) 创建窗口实例的时候，调用MyCreateWindowEx不要调用全局的CreateWindow或
/*       CreateWindowEx函数;                                             */
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
			//获取本窗口类的窗口句柄
			inline HWND getHwnd(){ return m_hWnd; }
			//获取窗口类的WndProc函数，以便在RegistClass或SetWindowLong中设置支持类函数化的消息处理的窗口机制
			//在注册类型或改变窗口处理函数的时候，使用此函数的返回值作为窗口处理函数，才能正确的实现窗口消息的类化处理
			WNDPROC getClassedWndProc();

		protected:
			//窗口消息处理的需函数
			//子类必须覆盖此函数以实现对窗口消息的处理
			virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		protected:
			//通过内部实现CreateWindowEx函数以便完成对窗口消息类化处理所必要的操作
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
			/*      以下函数或成员为类私有数据，为实现窗口消息的类化处理的内部实现的函数或数据声明     */
			/*****************************************************************************/
			static CClassedWnd* CreationWnd;
			static LRESULT APIENTRY InitClassedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			static LRESULT APIENTRY StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, CClassedWnd *pWnd);

		protected:
			HWND		m_hWnd;				//窗口句柄
			WNDPROC		m_pClassWndProcInst;//窗口消息类化处理实例

		public:
			CClassedWnd(void);
			virtual ~CClassedWnd(void);
		};
	};
};
#endif
#endif

