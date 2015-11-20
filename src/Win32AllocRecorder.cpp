#include <crtdbg.h>
#include <malloc.h>
#include <errno.h>
#include "AppDebug.h"

#pragma warning(disable:4074)
#pragma init_seg(compiler)

#include "alloctrace.h"

namespace alloctrace
{
	class AllocRecorder;
	extern AllocRecorder allocRecorder;
	extern bool g_boDestroyFlag;

	enum EAllocRecordType
	{
		eAlloc,
		eFree,
		eRealloc,
	};

	void AllocTraceRecord(EAllocRecordType _type, void* _ptr, size_t _size)
	{
		// FIXME : add code here
		//if (!g_boDestroyFlag)
		//	allocRecorder.record((AllocRecordType)_type, _ptr, _size);
	}
}

