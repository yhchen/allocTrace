#ifndef __ALLOC_TRACE_WIN32ALLOC_RECORDER_H__
#define __ALLOC_TRACE_WIN32ALLOC_RECORDER_H__

#include "alloctrace.h"
#include "os/Thread.h"
#include "os/Lock.h"
#include "stream/BaseStream.h"
#include "os/pipe/NamedPipe.h"
#include "packet/HeapAllocDataPacket.hpp"

#pragma warning(disable:4996)

namespace alloctrace
{
	using namespace alloctrace::os;
	using namespace alloctrace::os::pipe;
	using namespace alloctrace::container;
	using namespace alloctrace::stream;
	using namespace alloctrace::packet;

#pragma pack(push, 1)
	struct AllocRecordEx : public AllocRecord
	{
		static const int MAX_STACK_SIZE = 80;
		size_t frames[MAX_STACK_SIZE];
	};
#pragma pack(pop)

	class AllocRecordList : public CBaseList<AllocRecordEx>
	{
	public:
		~AllocRecordList()
		{
			empty();
		}
		virtual void empty()
		{
			clear();
			m_tMaxCount = 0;
			if (m_pData)
			{
				HeapFree(GetProcessHeap(), 0, m_pData);
				m_pData = 0;
			}
		}
		virtual void reserve(INT_PTR count)
		{
			if (count > m_tCount && count != m_tMaxCount)
			{
				m_tMaxCount = count;
				if (m_pData)
					m_pData = (AllocRecordEx*)HeapReAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, m_pData, sizeof(AllocRecordEx) * count);
				else m_pData = (AllocRecordEx*)HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, sizeof(AllocRecordEx) * count);
			}
		}
	};

	class AllocRecordOutputPipe : public CBaseThread, public CNamedPipeClient
	{
	private:
		CCSLock m_PacketsLock;
		CHeapAllocDataPacket	m_Packets[2];
		CHeapAllocDataPacket* m_pAppendPacket;
		CHeapAllocDataPacket* m_pWritePacket;
		DWORD64 dwReconnectPipeTime;
	public:
		/*static void* operator new (size_t size)
		{
		return HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, size);
		}
		static void operator delete (void* ptr)
		{
		HeapFree(GetProcessHeap(), 0, ptr);
		}*/
		AllocRecordOutputPipe() :CBaseThread(TRUE)
		{
			m_pAppendPacket = &m_Packets[0];
			m_pWritePacket = &m_Packets[1];
			m_pAppendPacket->reserve(8 * 1024 * 1024);
			m_pWritePacket->reserve(8 * 1024 * 1024);
			dwReconnectPipeTime = 0;
			resume();
		}
		void writePipe(CHeapAllocDataPacket &packet)
		{
			if (m_boActive)
			{
				m_PacketsLock.Lock();
				m_pAppendPacket->writeBuf(packet.getMemoryPtr(), packet.getLength());
				m_PacketsLock.Unlock();
			}
		}
	protected:
		void OnRountine()
		{
			while (true)
			{
				if (connected())
				{
					sendBuffers();
					if (terminated() && m_pWritePacket->getAvaliableLength() + m_pAppendPacket->getLength() >= 0)
					{
						//等待管道数据在另一端被读取
						Sleep(1000);
						break;
					}
				}
				else
				{
					if (terminated())
						break;
				}
				Sleep(4);
			}
		}
	private:
		bool connected()
		{
			if (m_boActive)
				return true;
			if (!m_sPipeName[0])
				return false;
			DWORD64 dwCurrTick = ::GetTickCount64();
			if (dwCurrTick < dwReconnectPipeTime)
				return false;
			dwReconnectPipeTime = dwCurrTick + 3 * 1000;

			Open();
			if (m_boActive)
			{
				m_pWritePacket->setLength(0);
				m_pAppendPacket->setLength(0);
				DWORD dwPID = GetCurrentProcessId();
				WriteBuf(&dwPID, sizeof(dwPID));
				return true;
			}

			return false;
		}
		void sendBuffers()
		{
			size_t dwRemLen = m_pWritePacket->getAvaliableLength();
			if (dwRemLen <= 0)
			{
				m_pWritePacket->setLength(0);
				if (m_pAppendPacket->getLength() > 0)
				{
					m_PacketsLock.Lock();
					CHeapAllocDataPacket *temp = m_pWritePacket;
					m_pWritePacket = m_pAppendPacket;
					m_pAppendPacket = temp;
					m_PacketsLock.Unlock();
					m_pWritePacket->setPosition(0);
				}
			}

			dwRemLen = m_pWritePacket->getAvaliableLength();
			if (dwRemLen > 0)
			{
				m_pWritePacket->adjustOffset(WriteBuf(m_pWritePacket->getOffsetPtr(), (int)dwRemLen));
			}
		}
	};

	extern bool g_boDestroyFlag;

	class AllocRecorder : public CBaseThread
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
			g_boDestroyFlag = false;
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
			UninitializeAllocTracer();
			g_boDestroyFlag = true;
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

	extern AllocRecorder allocRecorder;
}

#endif __ALLOC_TRACE_WIN32ALLOC_RECORDER_H__
