#include "alloctrace.h"
#include "Symbol.h"

namespace alloctrace
{
	/************************************************************************
	 * 用于插件式嵌入该库
	 * 1.新增引用
	 * Common Properties -> References -> Add New Reference...
	 * 2.设置force symbol
	 * Configuration Properties -> Linker -> Input -> Force Symbol References
	 * 键入内容 : "__allocTrace;"
	 * alloctrace库引入完成
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
	 * allocTrace自动初始化机制:
	 * 在全局作用域中申明一个变量，该变量会在main函数执行前执行构造函数，
	 * 在main函数结束后调用析构函数。以此方法实现allocTrace的自动初始化
	 ************************************************************************/
#include "os/pipe/NPComm.h"
	class FirstInitor{
	public:
		FirstInitor(){
			InitializeAllocTracer(true, NP_DEFAULT_PIPENAME);
		}
		~FirstInitor(){
			// !!!ATTENTION:为了完整监控释放流程中是否有泄漏不析构tracer!!!
			//SetAllocTraceOutputPipeName(NULL);
			//UninitializeAllocTracer();
		}
	} firstInitor;
}
