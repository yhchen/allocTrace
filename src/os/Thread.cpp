#include <assert.h>
#include "Thread.h"

#ifdef WIN32

namespace alloctrace
{
	namespace os
	{
		CBaseThread::CBaseThread()
		{
			m_hThread = NULL;
			m_dwThreadId = 0;
			m_boTerminated = false;
			m_boFreeOnTerminate = false;
			CreateThreadHandle(0);
		}

		CBaseThread::CBaseThread(bool boCreateSuspended)
		{
			m_hThread = NULL;
			m_dwThreadId = 0;
			m_boTerminated = false;
			m_boFreeOnTerminate = false;
			CreateThreadHandle(boCreateSuspended ? CREATE_SUSPENDED : 0);
		}

		CBaseThread::~CBaseThread()
		{
			terminate();
			waitFor();
			if (m_hThread)
			{
				CloseHandle(m_hThread);
				m_hThread = NULL;
			}
		}

		HANDLE CBaseThread::CreateThreadHandle(const DWORD dwFlags)
		{
			m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadRoutine, this, dwFlags, &m_dwThreadId);
			return m_hThread;
		}

		void CALLBACK CBaseThread::ThreadRoutine(CBaseThread *lpThread)
		{
			lpThread->OnRountine();
			lpThread->OnTerminated();
			if (lpThread->m_boFreeOnTerminate)
				delete lpThread;
			ExitThread(0);
		}
		void CBaseThread::OnRountine()
		{
		}

		void CBaseThread::OnSuspend(int nSuspendCount)
		{
		}

		void CBaseThread::OnResume(int nSuspendCount)
		{
		}

		void CBaseThread::OnTerminated()
		{
		}

		int CBaseThread::suspend()
		{
			DWORD dwSuspended;
			dwSuspended = SuspendThread(m_hThread);
			if (dwSuspended != -1)
			{
				OnSuspend(dwSuspended + 1);
				return dwSuspended + 1;
			}
			return dwSuspended;
		}

		int CBaseThread::resume()
		{
			DWORD dwSuspended;
			dwSuspended = ResumeThread(m_hThread);
			if (dwSuspended != -1)
			{
				OnResume(dwSuspended - 1);
				return dwSuspended - 1;
			}
			return dwSuspended;
		}

		int CBaseThread::getPriority()
		{
			return GetThreadPriority(m_hThread);
		}

		bool CBaseThread::setPriority(int nPriority)
		{
			return SetThreadPriority(m_hThread, nPriority) != 0;
		}

		int CBaseThread::waitFor(DWORD dwWaitLong, bool boWaitAlertAble)
		{
			return WaitForSingleObjectEx(m_hThread, dwWaitLong, boWaitAlertAble);
		}

		void CBaseThread::terminate()
		{
			m_boTerminated = true;
		}
	}
}

#endif

#ifdef __GNUC__
namespace alloctrace
{
	namespace os
	{

		CBaseThread::CBaseThread()
		{
			m_pid = 0;
			m_boTerminated = false;
			m_delayStart = false;
			m_boFreeOnTerminate = false;
			createThread();
		}
		CBaseThread::CBaseThread(bool boCreateSuspended)
		{
			m_pid = 0;
			m_boTerminated = false;
			m_delayStart = true;
			m_boFreeOnTerminate = false;
		}
		CBaseThread::~CBaseThread()
		{

		}

		int CBaseThread::suspend()
		{

			assert(false);
			return 0;
		}

		int CBaseThread::resume()
		{
			if(m_delayStart)
			{
				createThread();
			}
			return 0;
		}

		int CBaseThread::getPriority()
		{
			int policy;
			sched_param sp;
			if(0 != pthread_getschedparam(m_pid, &policy, &sp))
			{
				return 0;
			}
			return sp.sched_priority;
		}

		bool CBaseThread::setPriority(int nPriority)
		{
			sched_param sp = {nPriority};
			if(0 == pthread_setschedparam(m_pid, SCHED_RR, &sp))
			{
				return true;
			}
			return false;
		}

		void CBaseThread::createThread()
		{
			pthread_create(&m_pid, 0, (LPTHREAD_START_ROUTINE)(&CBaseThread::threadRoutine), this);
		}

		void *CBaseThread::threadRoutine(CBaseThread *lpThread)
		{
			lpThread->OnRountine();
			lpThread->OnTerminated();
			if (lpThread->m_boFreeOnTerminate)
				delete lpThread;
			pthread_exit(0);
			return 0;
		}

		int CBaseThread::waitFor()
		{
			pthread_join(m_pid, 0);
			return 0;
		}

		void CBaseThread::terminate()
		{
			m_boTerminated = true;;
		}
	}
}
#endif
