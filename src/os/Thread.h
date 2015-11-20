#ifndef __ALLOC_TRACE_THREAD_H__
#define __ALLOC_TRACE_THREAD_H__

/******************************************************************
 *
 *	$ �̻߳����� $
 *  
 *  - ��Ҫ���� - 
 *
 *	ʵ���̵߳Ļ�����װ������֧����ͣ���ָ��̣߳��ȴ��߳��Լ���
 *	���߳����ȼ��Ȼ���������
 *
 *****************************************************************/

#include "Platform.h"

namespace alloctrace
{
	namespace os
	{

#ifdef WIN32
		/*	�̻߳�����

		*/
		class CBaseThread
		{
		private:
			HANDLE	m_hThread;			//�߳̾��
			DWORD	m_dwThreadId;		//�߳�ID
			bool	m_boTerminated;		//�߳���ֹ��ǣ��������߳�����ѭ����Ӧ��ͨ������terminated�����ж��߳��Ƿ���������ֹ���
			bool	m_boFreeOnTerminate;//��ֹ������
		private:
			HANDLE CreateThreadHandle(const DWORD dwFlags);
			static void CALLBACK ThreadRoutine(CBaseThread *lpThread);
		protected:
			//terminated���������ж��ֳ��Ƿ���������ֹ���
			inline bool terminated(){ return m_boTerminated; }
			//�߳����ﴦ�����̺���������Ӧ���̳д˺���
			virtual void OnRountine();
			//�̱߳���ֹ���֪ͨ������nSuspendCount������ʾ�̱߳���ֹ�Ĵ���
			virtual void OnSuspend(int nSuspendCount);
			//�̱߳��ָ����֪ͨ������nSuspendCount������ʾ�̻߳���ָ����ٴκ���ָܻ�ִ��,Ϊ0��ʾ�߳��ѻָ�ִ��
			virtual void OnResume(int nSuspendCount);
			//�߳�������ֹ���֪ͨ����
			virtual void OnTerminated();
		public:
			CBaseThread();
			CBaseThread(bool boCreateSuspended);
			~CBaseThread();

			//��ȡ�߳̾��
			inline HANDLE getHandle(){ return m_hThread; }
			//��ȡ�߳�ID
			inline DWORD getThreadId(){ return m_dwThreadId; }
			//�����߳��˳����Զ����ٱ��
			inline void setFreeOnTerminate(bool value){ m_boFreeOnTerminate = value; }
			//��ͣ�̵߳�ִ�У�����ֵ��ʾ�̵߳�������ͣ������Ϊֹ�ܼƱ���ͣ�Ĵ�����������-1���ʾ��ͣ�߳�ʧ�ܡ�
			int suspend();
			//�ָ��̵߳�ִ�У�����ֵ��ʾ�̵߳����λָ��������߳�����ָ����ٴβ��ָܻ�ִ�С�
			//������-1���ʾ�ָ��߳�ʧ�ܣ�����0��ʾ�߳��Ѿ���ȫ�ָ�ִ�С�
			int resume();
			//��ȡ�߳����ȼ�,ʧ�ܻ᷵��THREAD_PRIORITY_ERROR_RETURN�����򷵻�ֵ��ʾ�߳����ȼ�
			int getPriority();
			//�����߳����ȼ����ɹ��򷵻�true
			bool setPriority(int nPriority);
			//�ȴ��߳�ִ����ϣ�dwWaitLong������ʾ�ȴ�������������INFINITE��ʾ���޵ȴ���
			//ע�⣬���ô˺������߳��ڴ��߳�ִ����Ϻ��һֱ��������״̬
			//����boWaitAlertAble��ʾ�����߳��������ڼ��Ƿ�������뾯��״̬��������windows��Ч)
			int waitFor(DWORD dwWaitLong = INFINITE, bool boWaitAlertAble = true);
			//����̵߳���ֹ���
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
			///��ʱ���޵ȴ�
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

