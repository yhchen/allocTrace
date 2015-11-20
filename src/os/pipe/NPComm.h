#ifndef	__ALLOC_TRACE_NPCOMM_H__
#define __ALLOC_TRACE_NPCOMM_H__

#include <tchar.h>

/*	Ĭ�������ܵ�����		*/
#if defined(x64)
#define	NP_DEFAULT_PIPENAME		_TEXT("\\\\.\\pipe\\HSAllocTraceServer_x64")
#else
#define	NP_DEFAULT_PIPENAME		_TEXT("\\\\.\\pipe\\HSAllocTraceServer_x86")
#endif

/*	�ܵ�Ĭ�����뻺���С	*/
#define	NP_DEFAULT_INBUFSIZE	8192

/*	�ܵ�Ĭ����������С	*/
#define	NP_DEFAULT_OUTBUFSIZE	8192

/*	�ܵ�Ĭ�ϳ�ʱʱ��		*/
#define	NP_DEFAULT_TIMEOUT		NMPWAIT_USE_DEFAULT_WAIT

#endif //__ALLOC_TRACE_NPCOMM_H__
