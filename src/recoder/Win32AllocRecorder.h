#ifndef __ALLOC_TRACE_WIN32ALLOC_RECORDER_H__
#define __ALLOC_TRACE_WIN32ALLOC_RECORDER_H__

#include "alloctrace.h"
#include "os/Thread.h"
#include "os/Lock.h"
#include "stream/BaseStream.h"
#include "packet/HeapAllocDataPacket.hpp"
#include "container/HeapAllocList.h"
#include "container/singleton.h"
#include "recoder/AllocRecordOutputPipe.h"

#pragma warning(disable:4996)

namespace alloctrace
{
	using namespace alloctrace::os;
	using namespace alloctrace::os::pipe;
	using namespace alloctrace::container;
	using namespace alloctrace::stream;
	using namespace alloctrace::packet;
	using namespace alloctrace::recoder;

#pragma pack(push, 1)
	struct AllocRecordEx : public AllocRecord
	{
		static const int MAX_STACK_SIZE = 128;
		size_t frames[MAX_STACK_SIZE];
	};
#pragma pack(pop)
	typedef HeapAllocList<AllocRecordEx> AllocRecordList;

	class AllocRecorder : public singleton<AllocRecorder>, public CBaseThread
	{
	public:
		AllocRecordList m_RecordList[2];
		AllocRecordList *m_pRecAppendList;
		AllocRecordList *m_pRecProcList;
		CCSLock m_RecListLock;
		CBaseStream *m_pOutFile;
		CHeapAllocDataPacket m_TempBuffer;
		CHeapAllocDataPacket m_PipeWaitBuffer;
		AllocRecordOutputPipe* m_pPipe;
	public:
		AllocRecorder();
		~AllocRecorder();
		void setOutputFile(CBaseStream *file);
		void setOutputPipeName(const TCHAR* sPipeName);
		void record(AllocRecordType _type, const void* _ptr, size_t _size);
	protected:
		virtual void OnRountine();
	};
}

#endif // !__ALLOC_TRACE_WIN32ALLOC_RECORDER_H__
