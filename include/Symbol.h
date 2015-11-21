#ifndef __ALLOC_TRACE_SYMBOL_H__
#define __ALLOC_TRACE_SYMBOL_H__

#include <windows.h>
#include "container/BaseList.h"

namespace alloctrace
{
	using namespace container;

#if defined(WIN32)

	//ͨ�����е�ַ��ȡ��������Դ�����ļ����Լ��к�
	class CodeAddressLineInfo
	{
	public:
		const char* fnName;//������
		const char* fileName;//�ļ���
		int lineNo;//�к�

	public:
		CodeAddressLineInfo();
		~CodeAddressLineInfo();
		bool getAddressInfo(const size_t address);
	};

	//ͨ�����е�ַ��ȡ����ջ֡�б�
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
