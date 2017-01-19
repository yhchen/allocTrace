#include "alloctrace.h"
#include "Symbol.h"

namespace alloctrace
{
	/************************************************************************
	 * ���ڲ��ʽǶ��ÿ�
	 * 1.��������
	 * Common Properties -> References -> Add New Reference...
	 * 2.����force symbol
	 * Configuration Properties -> Linker -> Input -> Force Symbol References
	 * �������� : "__allocTrace;"
	 * alloctrace���������
	 ************************************************************************/

#if !defined(_LIB) && defined(_WIN32)
// Annoying stuff for windows -- makes sure clients can import these functions
#  define ALLOCTRACE_DLL_DECL  __declspec(dllimport)
#else
#  define ALLOCTRACE_DLL_DECL
#endif

	extern "C" ALLOCTRACE_DLL_DECL void _allocTrace();
	void _allocTrace() { }

	extern "C" ALLOCTRACE_DLL_DECL void __allocTrace();
	void __allocTrace() { }

	/************************************************************************
	 * allocTrace�Զ���ʼ������:
	 * ��ȫ��������������һ���������ñ�������main����ִ��ǰִ�й��캯����
	 * ��main������������������������Դ˷���ʵ��allocTrace���Զ���ʼ��
	 ************************************************************************/
#include "os/pipe/NPComm.h"
	class FirstInitor{
	public:
		FirstInitor(){
			InitializeAllocTracer(true, NP_DEFAULT_PIPENAME);
		}
		~FirstInitor(){
			// !!!ATTENTION:Ϊ����������ͷ��������Ƿ���й©������tracer!!!
			//SetAllocTraceOutputPipeName(NULL);
			//UninitializeAllocTracer();
		}
	} firstInitor;
}
