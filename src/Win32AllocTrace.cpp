#include <crtdbg.h>
#include <malloc.h>
#include <errno.h>
#include "AppDebug.h"
#include "os/Lock.h"
#include "os/Thread.h"
#include "os/pipe/NamedPipe.h"
#include "stream/BaseStream.h"

#pragma warning(disable:4074)
#pragma warning(disable:4273)
#pragma init_seg(compiler)

#include "packet/HeapAllocDataPacket.hpp"
#include "Win32AllocRecorder.hpp"

namespace alloctrace
{
	extern "C" {
		extern int _newmode;    /* malloc new() handler mode */
#ifdef _DEBUG
		extern void * __cdecl _nh_malloc_dbg(
			_In_ size_t _Size,
			_In_ int _NhFlag,
			_In_ int _BlockType,
			_In_opt_z_ const char * _Filename,
			_In_ int _LineNumber
			);
		extern void * __cdecl _calloc_dbg(
			_In_ size_t _Count,
			_In_ size_t _Size,
			_In_ int _BlockType,
			_In_opt_z_ const char * _Filename,
			_In_ int _LineNumber
			);
		extern void * __cdecl _realloc_dbg(
			_Post_ptr_invalid_ void * _Memory,
			_In_ size_t _NewSize,
			_In_ int _BlockType,
			_In_opt_z_ const char * _Filename,
			_In_ int _LineNumber
			);
		extern void __cdecl _free_dbg(
			_Post_ptr_invalid_ void * _Memory,
			_In_ int _BlockType
			);


		void * __cdecl __at_malloc(
			size_t size
			)
		{
			void *res = _nh_malloc_dbg(size, _newmode, _NORMAL_BLOCK, NULL, 0);
			if (res) allocRecorder.record(atAlloc, res, size);
			return res;
		}

		void * __cdecl __at_calloc(
			size_t nNum,
			size_t nSize
			)
		{
			void *res = _calloc_dbg(nNum, nSize, _NORMAL_BLOCK, NULL, 0);
			if (res) allocRecorder.record(atAlloc, res, nSize);
			return res;
		}

		void __cdecl __at_free(
			void * pUserData
			)
		{
			if (pUserData)
			{
				_free_dbg(pUserData, _NORMAL_BLOCK);
				allocRecorder.record(atFree, pUserData, 0);
			}
		}

		void * __cdecl __at_realloc(
			void * pBlock,
			size_t newsize
			)
		{
			void *res = _realloc_dbg(pBlock, newsize, _NORMAL_BLOCK, NULL, 0);
			allocRecorder.record(atFree, pBlock, 0);
			if (res) allocRecorder.record(atAlloc, res, newsize);
			return res;
		}

#else

		extern HANDLE _crtheap;
		extern void * __cdecl _heap_alloc(size_t _Size);
		extern int __cdecl _callnewh(_In_ size_t _Size);
		extern int __cdecl _get_errno_from_oserr(unsigned long);
		extern void * __cdecl _calloc_impl (size_t num, size_t size, int * errno_tmp);

		void * __cdecl _malloc_no_trace (
			size_t size
			)
		{
			void *res = NULL;
			//  validate size
			if (size <= _HEAP_MAXREQ) {
				for (;;) {

					//  allocate memory block
					res = _heap_alloc(size);

					//  if successful allocation, return pointer to memory
					//  if new handling turned off altogether, return NULL

					if (res != NULL)
					{
						break;
					}
					if (_newmode == 0)
					{
						errno = ENOMEM;
						break;
					}

					//  call installed new handler
					if (!_callnewh(size))
						break;

					//  new handler was successful -- try to allocate again
				}
			} else {
				_callnewh(size);
				errno = ENOMEM;
				return NULL;
			}

			if (res == NULL)
				errno = ENOMEM;
			return res;
		}

		void * __cdecl __at_malloc (
			size_t size
			)
		{
			void *res = _malloc_no_trace(size);
			if (res)
				allocRecorder.record(atAlloc, res, size);
			return res;
		}

		void * __cdecl __at_calloc(
			size_t num,
			size_t size
			)
		{
			int errno_tmp = 0;
			void * pv = _calloc_impl(num, size, &errno_tmp);
			if ( pv == NULL && errno_tmp != 0 && _errno())
				errno = errno_tmp; // recall, #define errno *_errno()
			else allocRecorder.record(atAlloc, pv, size);
			return pv;
		}

		int __cdecl _free_no_trace(
			void * pBlock
			)
		{
			if (pBlock)
			{
				int retval = 0;
				retval = HeapFree(_crtheap, 0, pBlock);
				if (retval == 0)
				{
					errno = _get_errno_from_oserr(GetLastError());
					return 2;
				}
				return 0;
			}
			return 1;
		}

		void __cdecl __at_free(
			void * pBlock
			)
		{
			if (_free_no_trace(pBlock) == 0)
				allocRecorder.record(atFree, pBlock, 0);
		}

		void * __cdecl _realloc_no_trace(
			void * pBlock,
			size_t newsize
			)
		{
			void *res = NULL;
			void *      pvReturn;
			size_t      origSize = newsize;

			//  if ptr is NULL, call malloc
			if (pBlock == NULL)
				return(_malloc_no_trace(newsize));

			//  if ptr is nonNULL and size is zero, call free and return NULL
			if (newsize == 0)
			{
				_free_no_trace(pBlock);
				return(NULL);
			}


			for (;;) {

				pvReturn = NULL;
				if (newsize <= _HEAP_MAXREQ)
				{
					if (newsize == 0)
						newsize = 1;
					pvReturn = HeapReAlloc(_crtheap, 0, pBlock, newsize);
				}
				else
				{
					_callnewh(newsize);
					errno = ENOMEM;
					return NULL;
				}

				if ( pvReturn || _newmode == 0)
				{
					if (pvReturn)
					{
					}
					else
					{
						errno = _get_errno_from_oserr(GetLastError());
					}
					return pvReturn;
				}

				//  call installed new handler
				if (!_callnewh(newsize))
				{
					errno = _get_errno_from_oserr(GetLastError());
					return NULL;
				}

				//  new handler was successful -- try to allocate again
			}
			return res;
		}

		void * __cdecl _realloc_imp(
			void * pBlock,
			size_t newsize
			)
		{
			void *res = NULL;
			void *      pvReturn;
			size_t      origSize = newsize;

			//  if ptr is NULL, call malloc
			if (pBlock == NULL)
				return(__at_malloc(newsize));

			//  if ptr is nonNULL and size is zero, call free and return NULL
			if (newsize == 0)
			{
				__at_free(pBlock);
				return(NULL);
			}


			for (;;) {

				pvReturn = NULL;
				if (newsize <= _HEAP_MAXREQ)
				{
					if (newsize == 0)
						newsize = 1;
					pvReturn = HeapReAlloc(_crtheap, 0, pBlock, newsize);
				}
				else
				{
					_callnewh(newsize);
					errno = ENOMEM;
					return NULL;
				}

				if ( pvReturn || _newmode == 0)
				{
					if (pvReturn)
					{
					}
					else
					{
						errno = _get_errno_from_oserr(GetLastError());
					}
					return pvReturn;
				}

				//  call installed new handler
				if (!_callnewh(newsize))
				{
					errno = _get_errno_from_oserr(GetLastError());
					return NULL;
				}

				//  new handler was successful -- try to allocate again
			}
			allocRecorder.record(atFree, pBlock, 0);
			if (res) allocRecorder.record(atAlloc, res, newsize);
			return res;
		}

		void * __cdecl __at_realloc(
			void * pBlock,
			size_t newsize
			)
		{
			if (pBlock == NULL)
			{
				return __at_malloc(newsize);
			}
			if (newsize == 0)
			{
				__at_free(pBlock);
				return NULL;
			}
			void *res = _realloc_imp(pBlock, newsize);
			allocRecorder.record(atFree, pBlock, 0);
			if (res) allocRecorder.record(atAlloc, res, newsize);
			return res;
	}
#endif
};



	/*************************
	 * 跳转指令结构
	 *************************/
#pragma pack(push, 1)
	struct JMPInstruct32
	{
		unsigned char code;
		unsigned int offset;
	};

	struct JMPInstruct64
	{
		unsigned short code;
		unsigned int loAddrCST;
		unsigned __int64 offset;
	};

	union JMPInstruct
	{
		JMPInstruct32 c32;
		JMPInstruct64 c64;
	};
#pragma pack(pop)

	//计算跳转偏移地址
#define CalcJmpOffset32(s, d)	((SIZE_T)(d) - ((SIZE_T)(s) + 5))
#define CalcJmpOffset64(s, d)	((SIZE_T)(d))

	union JMPInstruct CodeSourceMalloc;
	union JMPInstruct CodeSourceCalloc;
	union JMPInstruct CodeSourceRealloc;
	union JMPInstruct CodeSourceFree;

	int PathJmpCode(JMPInstruct* code, JMPInstruct* newCode)
	{
		DWORD dwOldProtect;
		MEMORY_BASIC_INFORMATION mbi;

		if (VirtualQuery(code, &mbi, sizeof(mbi)) != sizeof(mbi))
			return GetLastError();

		//修改_output_l函数的内存保护模式，增加可读写保护
		if (!VirtualProtect(code, sizeof(*code), PAGE_EXECUTE_READWRITE, &dwOldProtect))
			return GetLastError();

		//改写跳转代码
		if (sizeof(void*) == 8)
			code->c64 = newCode->c64;
		else if (sizeof(void*) == 4)
			code->c32 = newCode->c32;

		//还原_output_l函数的内存保护模式
		if (!VirtualProtect(code, sizeof(*code), dwOldProtect, &dwOldProtect))
			return GetLastError();

		return 0;
	}

	int RedirectFunction(void* origin_fn, void* new_fn, JMPInstruct* save)
	{
		JMPInstruct* fn = (JMPInstruct*)origin_fn;
		*save = *fn;

		JMPInstruct pc;
		if (sizeof(void*) == 8)
		{
			//JMP [OFFSET]
			pc.c64.code = 0x25FF;
			pc.c64.loAddrCST = 0;//固定为0
			pc.c64.offset = CalcJmpOffset64(origin_fn, new_fn);
		}
		else if (sizeof(void*) == 4)
		{
			pc.c32.code = 0xE9;
			pc.c32.offset = (UINT)CalcJmpOffset32(origin_fn, new_fn);
		}


		return PathJmpCode(fn, &pc);
	}

	void InitializeAllocTracer()
	{
		RedirectFunction(&malloc, &__at_malloc, &CodeSourceMalloc);
		RedirectFunction(&calloc, &__at_calloc, &CodeSourceCalloc);
		RedirectFunction(&realloc, &__at_realloc, &CodeSourceRealloc);
		RedirectFunction(&free, &__at_free, &CodeSourceFree);
	}

	void UninitializeAllocTracer()
	{
		if (CodeSourceMalloc.c64.code)
		{
			PathJmpCode((JMPInstruct *)&malloc, &CodeSourceMalloc);
			CodeSourceMalloc.c64.code = 0;
		}
		if (CodeSourceCalloc.c64.code)
		{
			PathJmpCode((JMPInstruct *)&calloc, &CodeSourceCalloc);
			CodeSourceCalloc.c64.code = 0;
		}
		if (CodeSourceRealloc.c64.code)
		{
			PathJmpCode((JMPInstruct *)&realloc, &CodeSourceRealloc);
			CodeSourceRealloc.c64.code = 0;
		}
		if (CodeSourceFree.c64.code)
		{
			PathJmpCode((JMPInstruct *)&free, &CodeSourceFree);
			CodeSourceFree.c64.code = 0;
		}
	}

	AllocRecorder allocRecorder;
	static bool g_boDestroyFlag = true;

	void SetAllocTraceOutputPipeName(const TCHAR* pipeName)
	{
		allocRecorder.setOutputPipeName(pipeName);
	}
}


