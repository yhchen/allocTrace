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
		AllocRecorder() :CBaseThread(TRUE)
		{
			m_pRecAppendList = &m_RecordList[0];
			m_pRecProcList = &m_RecordList[1];
			m_pRecAppendList->reserve(8192);
			m_pRecProcList->reserve(8192);
			m_pOutFile = NULL;
			m_pPipe = NULL;
			m_TempBuffer.reserve(10 * 1024 * 1024);

			char sFileName[1024] = { 0 };
			GetModuleFileNameA(NULL, sFileName, sizeof(sFileName) - 20);
			char* sExt = strrchr(sFileName, '.');
			if (sExt)
				strcpy(sExt, ".alloctrace\0");
			else strcat(sFileName, ".alloctrace");

			resume();
		}
		~AllocRecorder()
		{
			terminate();
			waitFor();
		}
		void setOutputFile(CBaseStream *file)
		{
			m_pOutFile = file;
		}
		void setOutputPipeName(const TCHAR* sPipeName)
		{
			if (sPipeName)
			{
				if (m_pPipe)
				{
					if (!strcmp((const char*)m_pPipe->PipeName, (const char*)sPipeName))
						return;
					m_pPipe->terminate();
					delete m_pPipe;
					m_pPipe = NULL;
				}

				m_pPipe = new AllocRecordOutputPipe();
				m_pPipe->TimeOut = NMPWAIT_NOWAIT;
				m_pPipe->PipeName = sPipeName;
			}
			else
			{
				if (m_pPipe)
				{
					m_pPipe->terminate();
					m_pPipe = NULL;
				}
			}
		}
		void record(AllocRecordType _type, void* _ptr, size_t _size)
		{
			AllocRecordEx rec;
			rec.type = _type;
			rec.ptr = _ptr;
			rec.size = (unsigned int)_size;
			if (_type == atAlloc)
				rec.numFrames = StackFrameList::getStackFrameToBuffer(rec.frames, AllocRecordEx::MAX_STACK_SIZE, 2);
			else rec.numFrames = 0;

			m_RecListLock.Lock();
			m_pRecAppendList->add(rec);
			m_RecListLock.Unlock();
		}
	protected:
		virtual void OnRountine()
		{
			while (true)
			{
				if (m_pRecAppendList->count() > 0)
				{
					m_RecListLock.Lock();
					m_pRecProcList->addList(*m_pRecAppendList);
					m_pRecAppendList->clear();
					m_RecListLock.Unlock();
				}
				const INT_PTR nCount = m_pRecProcList->count();
				if (nCount > 0)
				{
					if (m_pOutFile || m_pPipe)
					{
						const AllocRecordEx* recList = *m_pRecProcList;

						for (INT_PTR i = 0; i<nCount; ++i)
						{
							const AllocRecordEx* rec = &recList[i];
							m_TempBuffer.writeBuf(rec, sizeof(*rec) - sizeof(rec->frames));
							m_TempBuffer.writeBuf(rec->frames, sizeof(rec->frames[0]) * rec->numFrames);
							if ((i & 0x7F) == 0x7F)
							{
								if (m_TempBuffer.getAvaliableCapacity() < 1024 * 1024)
									m_TempBuffer.reserve(m_TempBuffer.getCapacity() * 2);
							}
						}

						if (m_pOutFile)
							m_pOutFile->write(m_TempBuffer.getMemoryPtr(), m_TempBuffer.getLength());
						if (m_pPipe)
							m_pPipe->writePipe(m_TempBuffer);

						m_TempBuffer.setLength(0);
					}
					m_pRecProcList->clear();
				}
				else
				{
					if (terminated())
						break;
				}
				Sleep(4);
			}

			if (m_pPipe)
			{
				m_pPipe->terminate();
				m_pPipe->waitFor();
				delete m_pPipe;
				m_pPipe = NULL;
			}
		}
	};
}

#endif // !__ALLOC_TRACE_WIN32ALLOC_RECORDER_H__
