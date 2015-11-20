#ifndef __ALLOC_TRACE_MEMORYSTREAM_H__
#define __ALLOC_TRACE_MEMORYSTREAM_H__

#include "Type.h"
#include "BaseStream.h"

namespace alloctrace
{
	namespace stream
	{
		/*	�ڴ���
		ʹ���ڴ���Ϊ���ݴ洢������������
		*/
		class CMemoryStream
			: public CBaseStream
		{
		public:
			typedef CBaseStream Inherited;

		private:
			char	*m_pMemory;		//�ڴ��ָ��
			char	*m_pPointer;	//��ָ��
			char	*m_pStreamEnd;	//�ļ�βָ��
			char	*m_pMemoryEnd;	//�ڴ�������ַָ��
		protected:
			//�����Ҫ�����ڴ�������ͷ�������ˣ����ڼ̳����и��Ǵ˺�������
			virtual char* Alloc(char* ptr, const size_t size);

		public:
			CMemoryStream();
			~CMemoryStream();

			bool setSize(size_t tSize);
			size_t seek(const size_t tOffset, const int Origin);
			size_t read(LPVOID lpBuffer, const size_t tSizeToRead);
			size_t write(LPCVOID lpBuffer, const size_t tSizeToWrite);

			//��ȡ�ڴ������ڴ��ָ��
			inline LPVOID getMemory(){ return m_pMemory; }
			//���ļ��������ݵ��ڴ�
			size_t loadFromFile(LPCTSTR lpFileName);
			//�������ݱ��浽�ļ�
			size_t saveToFile(LPCTSTR lpFileName);
		};
	}
}

#endif // __ALLOC_TRACE_MEMORYSTREAM_H__
