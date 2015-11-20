#ifndef __STRUCTURED_EXCEPTION_HANDLER_H__
#define __STRUCTURED_EXCEPTION_HANDLER_H__

#ifdef WIN32
int Win32RecordException(struct _EXCEPTION_POINTERS *exceptInfo);
#	define __BEGIN_SEH_CATCH__ __try{
#	define __END_SEH_CATCH__(...) }__except(Win32RecordException(GetExceptionInformation())){__VA_ARGS__;}
#else
#	define __BEGIN_SEH_CATCH__ {
#	define __END_SEH_CATCH__(...) }
#endif

#endif

void SetExceptionRecordFileName(const char* fileName);
