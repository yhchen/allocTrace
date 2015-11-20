#ifndef __ALLOC_TRACE_BASESTREAM_H__
#define __ALLOC_TRACE_BASESTREAM_H__

#include "Type.h"

namespace alloctrace
{
	namespace stream
	{
		/*	基础抽象流
		流的基础类，提供流最基本的虚函数接口声明
		*/
		class CBaseStream
		{
		public:
			enum StreamSeekOrigin
			{
#if defined(WIN32)
				soBeginning = 0,	//从流的起点开始
				soCurrent = 1,		//从流的当前位置开始
				soEnd = 2,			//从流的末尾开始
#else
				soBeginning = SEEK_SET,	//从流的起点开始
				soCurrent = SEEK_CUR,		//从流的当前位置开始
				soEnd = SEEK_END,			//从流的末尾开始
#endif
			};
		private:
		public:
			CBaseStream(){};
			virtual ~CBaseStream(){};

			//获取流大小
			virtual size_t getSize();
			//设置流大小
			virtual bool setSize(size_t tSize){ return false; };
			//获取流中当前指针位置
			inline size_t getPosition(){ return seek(0, soCurrent); };
			//设置流当前指针位置
			size_t setPosition(const size_t tPosition);
			//调整流指针位置tOffset参数表示调整的长度，
			//Origin参数表示调整的启示位置，其值应当是StreamSeekOrigin枚举类型中的值
			virtual size_t seek(const size_t tOffset, const int Origin) = 0;
			//从流中读取数据lpBuffer为读取缓冲区，tSizeToRead为读取的字节数
			//如果读取失败，则函数返回-1，否则函数返回实际从流中读取的字节数
			virtual size_t read(LPVOID lpBuffer, const size_t tSizeToRead) = 0;
			//向流中写入数据lpBuffer为写入数据内容指针，tSizeToWrite为写入的字节数
			//如果写入失败，则函数返回-1，否则函数返回实际写入流中的字节数
			virtual size_t write(LPCVOID lpBuffer, const size_t tSizeToWrite) = 0;
			//从另一个流中拷贝数据lpStream为另一个流对象指针，tSizeToCopy表示拷贝的字节数
			//从lpStream中拷贝的数据的起始位置是lpStream的当前指针位置
			//若参数tSizeToCopy为0则表示拷贝lpStream整个流的数据
			virtual size_t copyFrom(CBaseStream& stream, size_t tSizeToCopy = 0);
		};
	}
}

#endif // __ALLOC_TRACE_BASESTREAM_H__
