#include "Lock.h"

using namespace alloctrace::os;

CCSLock::CCSLock()
{
	InitializeCriticalSection(&m_CriticalSection);
}

CCSLock::~CCSLock()
{
	DeleteCriticalSection(&m_CriticalSection);
}

void CCSLock::Lock()
{
	EnterCriticalSection(&m_CriticalSection);
}

BOOL CCSLock::TryLock()
{
	return TryEnterCriticalSection(&m_CriticalSection);
}

void CCSLock::Unlock()
{
	LeaveCriticalSection(&m_CriticalSection);
}



CAtomLock::CAtomLock()
{
	m_uLockFlag = Unlocked;
	m_dwLockThread = 0;
	m_dwLockCount = 0;
}

CAtomLock::~CAtomLock()
{
}

void CAtomLock::Lock()
{
	DWORD dwThreadId = GetCurrentThreadId();
	if (dwThreadId == m_dwLockThread)
	{
		m_dwLockCount++;
	}
	else
	{
		while (Unlocked != InterlockedCompareExchange(&m_uLockFlag, Locked, Unlocked))
		{
			OnLockWait();
		}
		m_dwLockThread = dwThreadId;
		m_dwLockCount++;
	}
}

BOOL CAtomLock::TryLock()
{
	DWORD dwThreadId = GetCurrentThreadId();
	if (dwThreadId == m_dwLockThread)
	{
		m_dwLockCount++;
	}
	else
	{
		if (Unlocked != InterlockedCompareExchange(&m_uLockFlag, Unlocked, Unlocked))
			return FALSE;
		m_dwLockThread = dwThreadId;
		m_dwLockCount++;
	}
	return TRUE;
}

void CAtomLock::Unlock()
{
	if (GetCurrentThreadId() == m_dwLockThread)
	{
		m_dwLockCount--;
		if (!m_dwLockCount)
		{
			m_uLockFlag = Unlocked;
			m_dwLockThread = 0;
		}
	}
}

void CAtomLock::OnLockWait()
{
}


