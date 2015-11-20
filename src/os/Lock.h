#ifndef __ALLOC_TRACE_LOCK_H__
#define	__ALLOC_TRACE_LOCK_H__

/******************************************************************
 *
 *	����������
 *
 *  ��Ҫ����  *
 *		�����ڲ��Ļ�������Windows���ṩ�ٽ�����(CCSLock)��ԭ����(
 *		CAtomLock)���ֻ���ʵ�ַ�ʽ�������ṩһ�����ں����ڲ���ȫ
 *		ʹ�õ�CSafeLock�ࡣ
 *
 *****************************************************************/
#include "Platform.h"

namespace alloctrace
{
	namespace os
	{
		/*	��������
		������
		*/
		class CBaseLock
		{
		public:
			virtual void Lock() = 0;
			virtual BOOL TryLock() = 0;
			virtual void Unlock() = 0;
		};

		class CCSLock
			: public CBaseLock
		{
		public:
			typedef CBaseLock Inherited;
		private:
			CRITICAL_SECTION m_CriticalSection;
		public:
			CCSLock();
			virtual ~CCSLock();
			void Lock();
			BOOL TryLock();
			void Unlock();
		};

		class CAtomLock
			: public CBaseLock
		{
		public:
			typedef CBaseLock Inherited;
			enum AtomLockState
			{
				Unlocked = 0,
				Locked = 1,
			};
		private:
			LONG volatile m_uLockFlag;
			DWORD volatile m_dwLockThread;
			DWORD volatile m_dwLockCount;
		public:
			CAtomLock();
			virtual ~CAtomLock();
			void Lock();
			BOOL TryLock();
			void Unlock();
			virtual void OnLockWait();
		};

		class CSafeLock
		{
		private:
			CBaseLock *m_pLock;
		public:
			inline CSafeLock(CBaseLock *pLock)
			{
				m_pLock = pLock;
				pLock->Lock();
			}
			inline ~CSafeLock()
			{
				m_pLock->Unlock();
			}
		};
	}
}

#endif // __ALLOC_TRACE_LOCK_H__

