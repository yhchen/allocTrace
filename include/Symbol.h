#ifndef __ALLOC_TRACE_SYMBOL_H__
#define __ALLOC_TRACE_SYMBOL_H__

#include <windows.h>
#include "container/BaseList.h"

namespace alloctrace
{
	using namespace container;

#if defined(WIN32)

	//通过运行地址获取函数名、源代码文件名以及行号
	class CodeAddressLineInfo
	{
	public:
		const char* fnName;//函数名
		const char* fileName;//文件名
		int lineNo;//行号

	public:
		CodeAddressLineInfo();
		~CodeAddressLineInfo();
		bool getAddressInfo(const size_t address);
	};

	//通过运行地址获取调用栈帧列表
	class StackFrameList
	{
	public:
		CBaseList<size_t> frameAddressList;
	public:
		StackFrameList();
		~StackFrameList();
		void getStackFrame(CONTEXT* context = NULL);
		static int getStackFrameToBuffer(size_t *frameBuffer, int maxFrameCount, int skipFrames = 0, CONTEXT* context = NULL);
	};
#endif
}


#endif // __ALLOC_TRACE_SYMBOL_H__
