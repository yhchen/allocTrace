#include "AllocRecordOutputPipe.h"

#include "os/pipe/NamedPipe.h"
#include "os/Thread.h"
#include "os/Lock.h"
#include "packet/HeapAllocDataPacket.hpp"

namespace alloctrace
{
	namespace recoder
	{

		using namespace alloctrace::os;
		using namespace alloctrace::os::pipe;
		using namespace alloctrace::packet;

		AllocRecordOutputPipe::AllocRecordOutputPipe() :CBaseThread(TRUE)
		{
			m_pAppendPacket = &m_Packets[0];
			m_pWritePacket = &m_Packets[1];
			m_pAppendPacket->reserve(8 * 1024 * 1024);
			m_pWritePacket->reserve(8 * 1024 * 1024);
			dwReconnectPipeTime = 0;
			resume();
		}

		void AllocRecordOutputPipe::writePipe(CHeapAllocDataPacket &packet)
		{
			if (m_boActive)
			{
				m_PacketsLock.Lock();
				m_pAppendPacket->writeBuf(packet.getMemoryPtr(), packet.getLength());
				m_PacketsLock.Unlock();
			}
		}

		void AllocRecordOutputPipe::OnRountine()
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

		bool AllocRecordOutputPipe::connected()
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

		void AllocRecordOutputPipe::sendBuffers()
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

	}
}

