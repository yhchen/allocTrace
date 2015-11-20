#ifndef __ALLOC_TRACE_THREAD_H__
#define __ALLOC_TRACE_THREAD_H__

/******************************************************************
 *
 *	$ 线程基础类 $
 *  
 *  - 主要功能 - 
 *
 *	实现线程的基本封装操作。支持暂停、恢复线程，等待线程以及设
 *	置线程优先级等基本操作。
 *
 *****************************************************************/

#include "Platform.h"

namespace alloctrace
{
	namespace os
	{

#ifdef WIN32
		/*	线程基础类

		*/
		class CBaseThread
		{
		private:
			HANDLE	m_hThread;			//线程句柄
			DWORD	m_dwThreadId;		//线程ID
			bool	m_boTerminated;		//线程终止标记，子类在线程例程循环中应当通过调用terminated函数判断线程是否被设置了终止标记
			bool	m_boFreeOnTerminate;//终止后销毁
		private:
			HANDLE CreateThreadHandle(const DWORD dwFlags);
			static void CALLBACK ThreadRoutine(CBaseThread *lpThread);
		protected:
			//terminated函数用于判断现成是否被设置了终止标记
			inline bool terminated(){ return m_boTerminated; }
			//线程事物处理例程函数，子类应当继承此函数
			virtual void OnRountine();
			//线程被终止后的通知函数，nSuspendCount参数表示线程被终止的次数
			virtual void OnSuspend(int nSuspendCount);
			//线程被恢复后的通知函数，nSuspendCount参数表示线程还需恢复多少次后才能恢复执行,为0表示线程已恢复执行
			virtual void OnResume(int nSuspendCount);
			//线程例程终止后的通知函数
			virtual void OnTerminated();
		public:
			CBaseThread();
			CBaseThread(bool boCreateSuspended);
			~CBaseThread();

			//获取线程句柄
			inline HANDLE getHandle(){ return m_hThread; }
			//获取线程ID
			inline DWORD getThreadId(){ return m_dwThreadId; }
			//设置线程退出后自动销毁标记
			inline void setFreeOnTerminate(bool value){ m_boFreeOnTerminate = value; }
			//暂停线程的执行，返回值表示线程到本次暂停操作后为止总计被暂停的次数。若返回-1则表示暂停线程失败。
			int suspend();
			//恢复线程的执行，返回值表示线程到本次恢复操作后，线程仍需恢复多少次才能恢复执行。
			//若返回-1则表示恢复线程失败，返回0表示线程已经完全恢复执行。
			int resume();
			//获取线程优先级,失败会返回THREAD_PRIORITY_ERROR_RETURN，否则返回值表示线程优先级
			int getPriority();
			//设置线程优先级，成功则返回true
			bool setPriority(int nPriority);
			//等待线程执行完毕，dwWaitLong参数表示等待的最大毫秒数，INFINITE表示无限等待。
			//注意，调用此函数的线程在此线程执行完毕后会一直处于阻塞状态
			//参数boWaitAlertAble表示调用线程在阻塞期间是否允许进入警告状态（仅对于windows有效)
			int waitFor(DWORD dwWaitLong = INFINITE, bool boWaitAlertAble = true);
			//标记线程的终止标记
			void terminate();
		};
#endif

#ifdef __GNUC__
		class CBaseThread
		{
		public:
			friend void *threadRoutine(CBaseThread *lpThread);
			CBaseThread();
			CBaseThread(bool boCreateSuspended);
			virtual ~CBaseThread();
			int suspend();
			int resume();
			int getPriority();
			bool setPriority(int nPriority);
			///暂时无限等待
			int waitFor();
			void terminate();
			inline void setFreeOnTerminate(bool value){ m_boFreeOnTerminate = value; }
		protected:
			inline bool terminated(){ return m_boTerminated; }
			virtual void OnRountine(){};
			virtual void OnSuspend(int nSuspendCount){};
			virtual void OnResume(int nSuspendCount){};
			virtual void OnTerminated(){};
		private:
			void createThread();
			static void *threadRoutine(CBaseThread *lpThread);
		private:
			pthread_t m_pid;
			bool	m_boTerminated;
			bool	m_delayStart;
			bool	m_boFreeOnTerminate;
		};
#endif

	}
}

#endif // __ALLOC_TRACE_THREAD_H__

