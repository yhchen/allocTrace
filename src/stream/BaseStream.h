#ifndef __ALLOC_TRACE_BASESTREAM_H__
#define __ALLOC_TRACE_BASESTREAM_H__

#include "Type.h"

namespace alloctrace
{
	namespace stream
	{
		/*	����������
		���Ļ����࣬�ṩ����������麯���ӿ�����
		*/
		class CBaseStream
		{
		public:
			enum StreamSeekOrigin
			{
#if defined(WIN32)
				soBeginning = 0,	//��������㿪ʼ
				soCurrent = 1,		//�����ĵ�ǰλ�ÿ�ʼ
				soEnd = 2,			//������ĩβ��ʼ
#else
				soBeginning = SEEK_SET,	//��������㿪ʼ
				soCurrent = SEEK_CUR,		//�����ĵ�ǰλ�ÿ�ʼ
				soEnd = SEEK_END,			//������ĩβ��ʼ
#endif
			};
		private:
		public:
			CBaseStream(){};
			virtual ~CBaseStream(){};

			//��ȡ����С
			virtual size_t getSize();
			//��������С
			virtual bool setSize(size_t tSize){ return false; };
			//��ȡ���е�ǰָ��λ��
			inline size_t getPosition(){ return seek(0, soCurrent); };
			//��������ǰָ��λ��
			size_t setPosition(const size_t tPosition);
			//������ָ��λ��tOffset������ʾ�����ĳ��ȣ�
			//Origin������ʾ��������ʾλ�ã���ֵӦ����StreamSeekOriginö�������е�ֵ
			virtual size_t seek(const size_t tOffset, const int Origin) = 0;
			//�����ж�ȡ����lpBufferΪ��ȡ��������tSizeToReadΪ��ȡ���ֽ���
			//�����ȡʧ�ܣ���������-1������������ʵ�ʴ����ж�ȡ���ֽ���
			virtual size_t read(LPVOID lpBuffer, const size_t tSizeToRead) = 0;
			//������д������lpBufferΪд����������ָ�룬tSizeToWriteΪд����ֽ���
			//���д��ʧ�ܣ���������-1������������ʵ��д�����е��ֽ���
			virtual size_t write(LPCVOID lpBuffer, const size_t tSizeToWrite) = 0;
			//����һ�����п�������lpStreamΪ��һ��������ָ�룬tSizeToCopy��ʾ�������ֽ���
			//��lpStream�п��������ݵ���ʼλ����lpStream�ĵ�ǰָ��λ��
			//������tSizeToCopyΪ0���ʾ����lpStream������������
			virtual size_t copyFrom(CBaseStream& stream, size_t tSizeToCopy = 0);
		};
	}
}

#endif // __ALLOC_TRACE_BASESTREAM_H__
