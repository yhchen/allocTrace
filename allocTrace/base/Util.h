#ifndef __ALLOC_TRACE__UTIL_H__
#define __ALLOC_TRACE__UTIL_H__

#include <unordered_map>

using namespace std;

#ifndef __LINE_BREAK__
#   if defined(WIN32)
typedef void(__stdcall *WINDEBUGBREAK)();
extern WINDEBUGBREAK __DebugBreak;
#       define __LINE_BREAK__() __DebugBreak()
#   elif defined(__APPLE__) || defined (__MACH__)
#       include "Debugger_Apple.h"
#       define __LINE_BREAK__() DebugBreak()
#   else
#       define __LINE_BREAK__() assert(0)
#   endif
#endif
#include <imagehlp.h>

#ifndef __DEBUG_ASSERT__
#   ifdef _DEBUG
#       define __DEBUG_ASSERT__(expr) if (!(expr)) {__LINE_BREAK__();}
#   else
#       define __DEBUG_ASSERT__(expr) assert(expr)
#   endif
#endif

static int wExceptionDumpFlag = -1; //这个是生成dump的标记
inline void SetMiniDumpFlag(int nFlag)
{
	wExceptionDumpFlag = nFlag;
}

//定义异常转出文件名称
extern const TCHAR szExceptionDumpFile[];

LONG WINAPI DefaultUnHandleExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo);

LONG WINAPI NormalUnHandleExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo);

#endif // __ALLOC_TRACE__UTIL_H__