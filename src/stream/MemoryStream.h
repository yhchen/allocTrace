#ifndef __ALLOC_TRACE_MEMORYSTREAM_H__
#define __ALLOC_TRACE_MEMORYSTREAM_H__

#include "Type.h"
#include "BaseStream.h"

namespace alloctrace
{
	namespace stream
	{
		/*	内存流
		使用内存作为数据存储区的流对象类
		*/
		class CMemoryStream
			: public CBaseStream
		{
		public:
			typedef CBaseStream Inherited;

		private:
			char	*m_pMemory;		//内存块指针
			char	*m_pPointer;	//流指针
			char	*m_pStreamEnd;	//文件尾指针
			char	*m_pMemoryEnd;	//内存块结束地址指针
		protected:
			//如果需要处理内存申请和释放相关事宜，则在继承类中覆盖此函数即可
			virtual char* Alloc(char* ptr, const size_t size);

		public:
			CMemoryStream();
			~CMemoryStream();

			bool setSize(size_t tSize);
			size_t seek(const size_t tOffset, const int Origin);
			size_t read(LPVOID lpBuffer, const size_t tSizeToRead);
			size_t write(LPCVOID lpBuffer, const size_t tSizeToWrite);

			//获取内存流的内存块指针
			inline LPVOID getMemory(){ return m_pMemory; }
			//从文件加流数据到内存
			size_t loadFromFile(LPCTSTR lpFileName);
			//将流数据保存到文件
			size_t saveToFile(LPCTSTR lpFileName);
		};
	}
}

#endif // __ALLOC_TRACE_MEMORYSTREAM_H__
