#ifndef __WIN32_ALLOC_TRACE_FILE_H__
#define __WIN32_ALLOC_TRACE_FILE_H__

#include "alloctrace.h"

using namespace alloctrace;
using namespace alloctrace::stream;
using namespace alloctrace::packet;

class AllocTraceMonitorFile : public CBaseStream
{
private:
	CDataPacket m_Packets[2];
	CDataPacket *m_pWritePack;
	CDataPacket *m_pReadPack;
	CDataPacket m_ReadRecRemPack;
	CCSLock m_Lock;
public:
	AllocTraceMonitorFile()
	{
		m_pWritePack = &m_Packets[0];
		m_pReadPack = &m_Packets[1];

		m_pWritePack->reserve(200 * 1024 * 1024);
		m_pReadPack->reserve(200 * 1024 * 1024);
		m_ReadRecRemPack.reserve(8192);
	}
	void readFromPipe(alloctrace::os::pipe::CNamedPipeObject *pipe)
	{
		int size = pipe->AvaliableSize;
		if (size > 0)
		{
			m_Lock.Lock();
			if ((int)m_pWritePack->getAvaliableCapacity() < size)
				m_pWritePack->reserve(m_pWritePack->getPosition() + size);
			m_pWritePack->adjustOffset(pipe->ReadBuf(m_pWritePack->getOffsetPtr(), size));
			m_Lock.Unlock();
		}
	}
	size_t remainSize()
	{
		return m_pWritePack->getLength() + m_pReadPack->getAvaliableLength();
	}
public:
	//������ָ��λ��tOffset������ʾ�����ĳ��ȣ�
	//Origin������ʾ��������ʾλ�ã���ֵӦ����StreamSeekOriginö�������е�ֵ
	virtual size_t seek(const size_t tOffset, const int Origin)
	{
		return 0;
	}
	bool readRecord(AllocRecord &rec, size_t *frames)
	{
		size_t remSize = m_ReadRecRemPack.getAvaliableLength();
		if (remSize < 1024)
		{
			if (remSize > 0)
			{
				if (m_ReadRecRemPack.getPosition() > 0)
				{
					memcpy(m_ReadRecRemPack.getMemoryPtr(), m_ReadRecRemPack.getOffsetPtr(), remSize);
					m_ReadRecRemPack.setLength(remSize);
					m_ReadRecRemPack.setPosition(0);
				}
			}
			else
			{
				m_ReadRecRemPack.setLength(0);
			}
			int len = (int)read(m_ReadRecRemPack.getOffsetPtr() + remSize, 4096);
			if (len > 0)
				m_ReadRecRemPack.setLength(remSize + len);
		}

		remSize = m_ReadRecRemPack.getAvaliableLength();
		if (remSize >= sizeof(rec))
		{
			AllocRecord *pRec = (AllocRecord*)m_ReadRecRemPack.getOffsetPtr();
			assert(pRec->type <= atRealloc);
			if (pRec->numFrames > 0)
			{
				if (remSize >= sizeof(rec) + sizeof(frames[0]) * pRec->numFrames)
				{
					rec = *pRec;
					m_ReadRecRemPack.adjustOffset(sizeof(rec));
					m_ReadRecRemPack.readBuf(frames, sizeof(frames[0]) * pRec->numFrames);
					return true;
				}
			}
			else
			{
				rec = *pRec;
				m_ReadRecRemPack.adjustOffset(sizeof(rec));
				return true;
			}
		}
		return false;
	}
	//�����ж�ȡ����lpBufferΪ��ȡ��������tSizeToReadΪ��ȡ���ֽ���
	//�����ȡʧ�ܣ���������-1������������ʵ�ʴ����ж�ȡ���ֽ���
	virtual size_t read(LPVOID lpBuffer, const size_t tSizeToRead)
	{
		size_t sizeReaded = m_pReadPack->readBuf(lpBuffer, tSizeToRead);
		
		if (sizeReaded < tSizeToRead)
		{
			const size_t writen = m_pWritePack->getLength();
			if (writen > 0)
			{
				m_pReadPack->setLength(0);

				m_Lock.Lock();
				CDataPacket *tmp = m_pReadPack;
				m_pReadPack = m_pWritePack;
				m_pWritePack = tmp;
				m_Lock.Unlock();

				m_pReadPack->setPosition(0);
				sizeReaded += m_pReadPack->readBuf((char*)lpBuffer + sizeReaded, tSizeToRead - sizeReaded);
			}
		}
		return sizeReaded;
	}
	//������д������lpBufferΪд����������ָ�룬tSizeToWriteΪд����ֽ���
	//���д��ʧ�ܣ���������-1������������ʵ��д�����е��ֽ���
	virtual size_t write(LPCVOID lpBuffer, const size_t tSizeToWrite)
	{
		m_Lock.Lock();
		m_pWritePack->writeBuf(lpBuffer, (size_t)tSizeToWrite);
		m_Lock.Unlock();
		return tSizeToWrite;
	}
};

#endif
