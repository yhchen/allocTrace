#ifndef __ALLOC_TRACE_FILESTREAM_H__
#define __ALLOC_TRACE_FILESTREAM_H__

#include "HandleStream.h"

namespace alloctrace
{
	namespace stream
	{
		/*	�ļ�����
		���ļ�ϵͳ�е��ļ���д��������
		*/
		class CFileStream
			: public CHandleStream
		{
		public:
			typedef CHandleStream Inherited;
			//�ļ��Ĵ򿪷�ʽ
			enum FileAccessType
			{
				faRead = 0x0001,		//��ȡ
				faWrite = 0x0002,		//д��
				faCreate = 0x1000,		//����

				faShareRead = 0x0100,		//������
				faShareWrite = 0x0200,		//д����
				faShareDelete = 0x0400,		//ɾ������
			};
			//win32�ļ�������ģʽ
			enum OpenDisposition
			{
				CreateIfNotExists = 1,		//�ļ��������򴴽�
				AlwaysCreate,				//���Ǵ����ļ�������ļ��������ļ��ض�Ϊ0�ֽ�
				OpenExistsOnly,				//���򿪴��ڵ��ļ�
				AlwaysOpen,					//���Ǵ��ļ������ļ�������ֱ�Ӵ򿪣������Դ����ļ�����
				TruncExistsOnly,			//����ļ���������ļ����ض�Ϊ0�ֽ�
			};
		private:
			LPTSTR	m_sFileName;
		protected:
			void setFileName(LPCTSTR lpFileName);
			void construct(LPCTSTR lpFileName, DWORD dwAccessType, DWORD dwWin32CreationDisposition);
		public:
			CFileStream(LPCTSTR lpFileName, DWORD dwAccessType);
			CFileStream(LPCTSTR lpFileName, DWORD dwAccessType, OpenDisposition eWin32CreateionDisposition);
			~CFileStream();

			inline LPCTSTR getFileName(){ return m_sFileName; }
		};
	}
}

#endif // __ALLOC_TRACE_FILESTREAM_H__
