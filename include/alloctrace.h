#ifndef __ALLOC_TRACE_ALLOCTRACE_H__
#define __ALLOC_TRACE_ALLOCTRACE_H__

#include <tchar.h>

namespace alloctrace
{
	//�ڴ��¼����
	enum AllocRecordType
	{
		atAlloc,
		atFree,
		atRealloc,
	};

	//�ڴ������¼
#pragma pack(push, 1)
	struct AllocRecord
	{
		const void* ptr;//�ڴ��ַ
		unsigned int size;//��С
		unsigned char type;//��¼����
		unsigned char numFrames;//����ջ����
	};
#pragma pack(pop)


	//��ʼ���ڴ����׷�ټ�¼
	//���szPipeNameΪNULL��ʹ��Ĭ�Ϲܵ���
	void InitializeAllocTracer(bool boRedirectFunc, const TCHAR* szPipeName = NULL);
	//�����ڴ����׷�ټ�¼
	void UninitializeAllocTracer();
	//�����ڴ����׷�ټ�¼�ܵ�
	void SetAllocTraceOutputPipeName(const TCHAR* pipeName);

	//ʹ�����õ��ڴ����׷�ٽ����������ڴ�׷���ļ�
	void SimpleDumpAllocTrace(const char* fileName);
	//�û��Զ���ӿ����ڼ�¼�ڴ���Ϣ�������ڴ��ػ�ȫ���ڴ�ؼ���ã�
	void CustomAllocRecorder(AllocRecordType _type, const void* _buffer, size_t _size);
}

#endif // __ALLOC_TRACE_ALLOCTRACE_H__