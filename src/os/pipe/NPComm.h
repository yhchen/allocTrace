#ifndef	__ALLOC_TRACE_NPCOMM_H__
#define __ALLOC_TRACE_NPCOMM_H__

#include <tchar.h>

/*	默认命名管道名称		*/
#if defined(x64)
#	define	NP_DEFAULT_PIPENAME		_TEXT("\\\\.\\pipe\\HSAllocTraceServer_x64")
#elif defined(x86)
#	define	NP_DEFAULT_PIPENAME		_TEXT("\\\\.\\pipe\\HSAllocTraceServer_x86")
#else
#	error "x64 or x86 not defined!"
#endif

/*	管道默认输入缓冲大小	*/
#define	NP_DEFAULT_INBUFSIZE	8192

/*	管道默认输出缓冲大小	*/
#define	NP_DEFAULT_OUTBUFSIZE	8192

/*	管道默认超时时间		*/
#define	NP_DEFAULT_TIMEOUT		NMPWAIT_USE_DEFAULT_WAIT

#endif //__ALLOC_TRACE_NPCOMM_H__
