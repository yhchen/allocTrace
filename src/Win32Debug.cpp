#include <windows.h>
#include "stream/BaseStream.h"
#include <DbgHelp.h>
#include "AppDebug.h"

#pragma warning(disable:4996)

namespace alloctrace
{

	static bool InitializeDebugLib()
	{
		static bool __first = true;
		static bool __initOK = false;
		if (__first)
		{
			__first = false;
			BOOL symbInited = SymInitialize(GetCurrentProcess(), NULL, TRUE);
			if (symbInited)
				__initOK = true;
		}
		return __initOK;
	}

	//------------------------ CodeAddressLineInfo -------------------------
	CodeAddressLineInfo::CodeAddressLineInfo()
	{
		fnName = NULL;
		fileName = NULL;
		lineNo = 0;
	}

	CodeAddressLineInfo::~CodeAddressLineInfo()
	{
		if (fileName)
		{
			free((void*)fileName);
			fileName = NULL;
		}
		if (fnName)
		{
			free((void*)fnName);
			fnName = NULL;
		}
	}

	bool CodeAddressLineInfo::getAddressInfo(const size_t address)
	{
		if (fileName)
		{
			free((void*)fileName);
			fileName = NULL;
		}
		if (fnName)
		{
			free((void*)fnName);
			fnName = NULL;
		}
		InitializeDebugLib();

		HANDLE hProcess = GetCurrentProcess();
		DWORD dwLineDisplacement = 0;
		IMAGEHLP_LINE ln;
		ln.SizeOfStruct = sizeof(ln);
		if (!SymGetLineFromAddr(hProcess, address, &dwLineDisplacement, &ln))
			return false;
		fileName = strdup(ln.FileName);
		lineNo = ln.LineNumber;

		PSYMBOL_INFO pSymbol;
		char symbBuf[sizeof(*pSymbol) + 1024];
		pSymbol = (PSYMBOL_INFO)symbBuf;
		pSymbol->SizeOfStruct = sizeof(*pSymbol);
		pSymbol->MaxNameLen = sizeof(symbBuf) - sizeof(*pSymbol);

		if (!SymFromAddr(hProcess, address, NULL, pSymbol))
			return false;
		fnName = strdup(pSymbol->Name);

		return true;
	}


	//------------------------ StackFrameList -------------------------
	StackFrameList::StackFrameList()
	{

	}

	StackFrameList::~StackFrameList()
	{

	}

	void StackFrameList::getStackFrame(CONTEXT* context)
	{
		frameAddressList.trunc(0);
		frameAddressList.reserve(32);
		CONTEXT ctx;
		DWORD MachineType = 0;
		STACKFRAME frame = { 0 };
		HANDLE hThread = GetCurrentThread();
		if (!context)
			RtlCaptureContext(&ctx);
		else ctx = *context;
#ifdef _M_IX86
		MachineType = IMAGE_FILE_MACHINE_I386;
		frame.AddrPC.Offset = ctx.Eip;
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrFrame.Offset = ctx.Ebp;
		frame.AddrFrame.Mode = AddrModeFlat;
		frame.AddrStack.Offset = ctx.Esp;
		frame.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
		MachineType = IMAGE_FILE_MACHINE_AMD64;
		frame.AddrPC.Offset = ctx.Rip;
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrFrame.Offset = ctx.Rsp;
		frame.AddrFrame.Mode = AddrModeFlat;
		frame.AddrStack.Offset = ctx.Rsp;
		frame.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
		MachineType = IMAGE_FILE_MACHINE_IA64;
		frame.AddrPC.Offset = ctx.StIIP;
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrFrame.Offset = ctx.IntSp;
		frame.AddrFrame.Mode = AddrModeFlat;
		frame.AddrBStore.Offset = ctx.RsBSP;
		frame.AddrBStore.Mode = AddrModeFlat;
		frame.AddrStack.Offset = ctx.IntSp;
		frame.AddrStack.Mode = AddrModeFlat;
#else
#error "Unsupported platform"
#endif
		while (StackWalk(MachineType, GetCurrentProcess(), hThread, &frame, &ctx, NULL, NULL, NULL, NULL))
		{
			if (frame.AddrPC.Offset == 0)
				break;
			frameAddressList.add(frame.AddrPC.Offset);
		}
	}

	int StackFrameList::getStackFrameToBuffer(size_t *frameBuffer, int maxFrameCount, int skipFrames /* = 0 */, CONTEXT* context /* = NULL */)
	{
		return RtlCaptureStackBackTrace(skipFrames, maxFrameCount, (PVOID*)frameBuffer, NULL);
	}

}
