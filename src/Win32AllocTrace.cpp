#include <crtdbg.h>
#include <malloc.h>
#include <errno.h>
#include "Symbol.h"
#include "os/Lock.h"
#include "os/Thread.h"
#include "os/pipe/NamedPipe.h"
#include "stream/BaseStream.h"

#pragma warning(disable:4074)
#pragma warning(disable:4273)
#pragma init_seg(compiler)

#include "packet/HeapAllocDataPacket.hpp"
#include "recoder/Win32AllocRecorder.h"
#include "container/singleton.h"
#include "os/pipe/NPComm.h"

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
			void *res = HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, size);
// 			void *res = _nh_malloc_dbg(size, _newmode, _NORMAL_BLOCK, NULL, 0);
			if (res) AllocRecorder::instance()->record(atAlloc, res, size);
			return res;
		}

		void * __cdecl __at_calloc(
			size_t nNum,
			size_t nSize
			)
		{
			void *res = HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, nNum * nSize);
// 			void *res = _calloc_dbg(nNum, nSize, _NORMAL_BLOCK, NULL, 0);
			if (res) AllocRecorder::instance()->record(atAlloc, res, nSize);
			return res;
		}

		void __cdecl __at_free(
			void * pUserData
			)
		{
			if (pUserData)
			{
				HeapFree(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, pUserData);
// 				_free_dbg(pUserData, _NORMAL_BLOCK);
				AllocRecorder::instance()->record(atFree, pUserData, 0);
			}
		}

		void * __cdecl __at_realloc(
			void * pBlock,
			size_t newsize
			)
		{
			void *res = HeapReAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, pBlock, newsize);
// 			void *res = _realloc_dbg(pBlock, newsize, _NORMAL_BLOCK, NULL, 0);
			AllocRecorder::instance()->record(atFree, pBlock, 0);
			if (res) AllocRecorder::instance()->record(atAlloc, res, newsize);
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
				AllocRecorder::instance()->record(atAlloc, res, size);
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
			else AllocRecorder::instance()->record(atAlloc, pv, size);
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
				AllocRecorder::instance()->record(atFree, pBlock, 0);
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
			AllocRecorder::instance()->record(atFree, pBlock, 0);
			if (res) AllocRecorder::instance()->record(atAlloc, res, newsize);
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
			AllocRecorder::instance()->record(atFree, pBlock, 0);
			if (res) AllocRecorder::instance()->record(atAlloc, res, newsize);
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
		void CalcJmpOffset(void* origin_fn, void* new_fn)
		{	//计算跳转偏移地址
			code = 0xE9;
			offset = (UINT)((SIZE_T)(new_fn)-((SIZE_T)(origin_fn)+5));
		}
	};

	struct JMPInstruct64
	{
		unsigned short code;
		unsigned int loAddrCST;
		unsigned __int64 offset;
		void CalcJmpOffset(void* origin_fn, void* new_fn)
		{	//计算跳转偏移地址
			code = 0x25FF;
			loAddrCST = 0;//固定为0
			offset = (SIZE_T)new_fn;
		}
	};

#if defined(x64)
	typedef JMPInstruct64 JMPInstruct;
#else
	typedef JMPInstruct32 JMPInstruct;
#endif

#pragma pack(pop)

	JMPInstruct CodeSourceMalloc = { 0 };
	JMPInstruct CodeSourceCalloc = { 0 };
	JMPInstruct CodeSourceRealloc = { 0 };
	JMPInstruct CodeSourceFree = { 0 };

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
		*code = *newCode;

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
		//JMP [OFFSET]
		pc.CalcJmpOffset(origin_fn, new_fn);
		return PathJmpCode(fn, &pc);
	}

	void InitializeAllocTracer(bool boRedirectFunc, const TCHAR* szPipeName)
	{
		if (!szPipeName)
			szPipeName = NP_DEFAULT_PIPENAME;
		if (AllocRecorder::instance() == NULL)
			AllocRecorder::initialize();
		if (boRedirectFunc)
		{
			if (!CodeSourceMalloc.code)
				RedirectFunction(&malloc, &__at_malloc, &CodeSourceMalloc);
			if (!CodeSourceCalloc.code)
				RedirectFunction(&calloc, &__at_calloc, &CodeSourceCalloc);
			if (!CodeSourceRealloc.code)
				RedirectFunction(&realloc, &__at_realloc, &CodeSourceRealloc);
			if (!CodeSourceFree.code)
				RedirectFunction(&free, &__at_free, &CodeSourceFree);
		}
		SetAllocTraceOutputPipeName(szPipeName);
	}

	void UninitializeAllocTracer()
	{
		if (CodeSourceMalloc.code)
		{
			PathJmpCode((JMPInstruct *)&malloc, &CodeSourceMalloc);
			CodeSourceMalloc.code = 0;
		}
		if (CodeSourceCalloc.code)
		{
			PathJmpCode((JMPInstruct *)&calloc, &CodeSourceCalloc);
			CodeSourceCalloc.code = 0;
		}
		if (CodeSourceRealloc.code)
		{
			PathJmpCode((JMPInstruct *)&realloc, &CodeSourceRealloc);
			CodeSourceRealloc.code = 0;
		}
		if (CodeSourceFree.code)
		{
			PathJmpCode((JMPInstruct *)&free, &CodeSourceFree);
			CodeSourceFree.code = 0;
		}
		AllocRecorder::uninitialize();
	}

	void SetAllocTraceOutputPipeName(const TCHAR* pipeName)
	{
		AllocRecorder::instance()->setOutputPipeName(pipeName);
	}

	void CustomAllocRecorder(AllocRecordType _type, const void* _buffer, size_t _size)
	{
		AllocRecorder::instance()->record(_type, _buffer, _size);
	}
}


