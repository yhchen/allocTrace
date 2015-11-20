#ifndef __ALLOC_TRACE_ALLOCTRACE_H__
#define __ALLOC_TRACE_ALLOCTRACE_H__

#include <tchar.h>

namespace alloctrace
{
	//内存记录类型
	enum AllocRecordType
	{
		atAlloc,
		atFree,
		atRealloc,
	};

	//内存申请记录
#pragma pack(push, 1)
	struct AllocRecord
	{
		void* ptr;//内存地址
		unsigned int size;//大小
		unsigned char type;//记录类型
		unsigned char numFrames;//调用栈数量
	};
#pragma pack(pop)


	//初始化内存分配追踪记录
	void InitializeAllocTracer();
	//销毁内存分配追踪记录
	void UninitializeAllocTracer();
	//设置内存分配追踪记录管道
	void SetAllocTraceOutputPipeName(const TCHAR* pipeName);

	//使用内置的内存分配追踪解析器解析内存追踪文件
	void SimpleDumpAllocTrace(const char* fileName);

}

#endif // __ALLOC_TRACE_ALLOCTRACE_H__