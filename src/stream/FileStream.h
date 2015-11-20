#ifndef __ALLOC_TRACE_FILESTREAM_H__
#define __ALLOC_TRACE_FILESTREAM_H__

#include "HandleStream.h"

namespace alloctrace
{
	namespace stream
	{
		/*	文件流，
		对文件系统中的文件读写操作的类
		*/
		class CFileStream
			: public CHandleStream
		{
		public:
			typedef CHandleStream Inherited;
			//文件的打开方式
			enum FileAccessType
			{
				faRead = 0x0001,		//读取
				faWrite = 0x0002,		//写入
				faCreate = 0x1000,		//创建

				faShareRead = 0x0100,		//读共享
				faShareWrite = 0x0200,		//写共享
				faShareDelete = 0x0400,		//删除共享
			};
			//win32文件创建的模式
			enum OpenDisposition
			{
				CreateIfNotExists = 1,		//文件不存在则创建
				AlwaysCreate,				//总是创建文件，如果文件存在则将文件截断为0字节
				OpenExistsOnly,				//仅打开存在的文件
				AlwaysOpen,					//总是打开文件，若文件存在则直接打开，否则尝试创建文件并打开
				TruncExistsOnly,			//如果文件存在则打开文件并截断为0字节
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
