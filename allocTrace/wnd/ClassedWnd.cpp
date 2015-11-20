#include "stdafx.h"
#include "ClassedWnd.h"
#include <Windows.h>
#include <TCHAR.h>
/************************************************************************/
/*
/*  消息处理函数与类实例挂钩的原理简析：                                
/*    1) CClassedWnd类在构造函数中会调用AllocClassedWndProcInstance函数，函数内
/*       初始化分页消息处理内存代码补丁块；
/*    2) 注册类型的时候，将窗口处理函数指定为getClassedWndProc()函数的返回值，是
/*       指向CClassedWnd::InitClassedWndProc静态函数的一个函数指针；
/*    3) 在CClassedWnd::InitClassedWndProc函数内部，做的仅仅是将窗口处理函数再
/*       次改变窗口实例的消息处理函数的内存补丁地址，自此完成将窗口处理函数与类实例的挂钩；
/*              
/*  将消息处理调用到类实例的内存补丁简析：            
/*    1) 有用的结构有：ClassedWndProcInstance和CWInstanceBlock，两个结构中有成员
/*       说明；
/*    2) 内存补丁的代码调用过程为：
/*       从pWnd->m_pClassWndProcInst开始执行：
/*         CALL NEAR PTR Offset - 执行代码内存补丁函数(CWInstanceBlock.Code)
/*         POP EBX              - 将pWnd->m_pClassWndProcInst->ClassedProc的地址弹出到EBX中（该地址是上一个CALL的返回地址，正好指向pWnd->m_pClassWndProcInst->ClassedProc的地址）
/*         JMP ClassedWndProc   - 跳转到ClassedWndProc函数
/*       在ClassedWndProc函数中,[EBX]即为pWnd->m_pClassWndProcInst->ClassedProc.lpFunction（CClassedWnd::StaticWndProc）,
/*       [EBX+4]为pWnd->m_pClassWndProcInst->ClassedProc.lpInstance（窗口实例），通过这2个数据即可调用CClassedWnd::StaticWndProc，
/*       CClassedWnd::StaticWndProc函数内则完成对pWnd->WndProc虚函数的调用，从此完成消息处理函数！
/*              
/************************************************************************/

using namespace wylib::window;

/*
 * 定义是否将调用CClassedWnd::StaticWndProc的函数代码完全混合到ClassedWndProcInstance中的代码中
 * 如果取消此宏的定义，则会使用完整的函数调用的形式通过ClassedWndProc汇编函数完成对CClassedWnd::StaticWndProc的调用，
 * 否则将ClassedWndProc汇编函数的代码拷贝到ClassedWndProcInstance中从而免去函数调用。
 * 如果是出于学习和了解这套类化窗口消息函数处理机制的目的，取消这个宏将使得调试过程中更容易理解这套机制。
 * 正式使用的时候使用混合代码的方式提高效率
 */
#define MERGE_CLASSED_WND_PROC_CODE


#ifndef MERGE_CLASSED_WND_PROC_CODE
extern "C" LRESULT APIENTRY ClassedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

#pragma pack(push, 1)
typedef struct ClassedWndProcInstance
{
	BYTE	Code;		//用于实现代码：E8 CALL NEAR PTR Offset
	INT		Offset;		//用于实现代码：补丁内存块跳转函数地址
	union
	{
		//初始化的时候指向下一个可用的ClassedWndProcInstance
		struct ClassedWndProcInstance* Next;
		struct
		{
			//窗口类消息处理函数地址，
			//函数形式必须是：LRESULT APIENTRY (*StaticClassedWndProc)(CClassedWnd *, HWND, UINT, WPARAM, LPARAM)
			LPVOID lpFunction;
			//窗口对象实例，是一个CClassWnd类的指针
			LPVOID lpInstance;	
#ifndef MERGE_CLASSED_WND_PROC_CODE
			//ClassedWndProc的地址，每个实例存放一份是为了实现x86与x64汇编调用ClassedWndProc函数的一致性
			LPVOID lpClassedWndProc;  将调用CClassedWnd::StaticWndProc的代码内联后不再需要此成员
#endif
		}ClassedProc;
	};
}CLASSED_WNDPROC_INSTANCE, *PCLASSED_WNDPROC_INSTANCE;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct CWInstanceBlock
{
	//定义一个分页窗口处理执行代码补丁块的内存大小
	static const INT_PTR PageSize = 4096;
	//此结构基于4096的内存对齐规则，允许的CLASSED_WNDPROC_INSTANCE结构的最大数量
	static const INT_PTR InstanceCount = (PageSize - 40) / sizeof(ClassedWndProcInstance);	

	struct CWInstanceBlock*	Next;	//指向下一个可用的分页窗口处理执行代码补丁块
#ifndef MERGE_CLASSED_WND_PROC_CODE
	BYTE Code[8];					//用于实现代码：POP EBX MOV EAX, [EBX+C] JMP EAX
#else
	BYTE Code[32];					//用于实现调用CClassedWnd::StaticWndProc的混合代码
#endif
	//用于在一个内存块中实现多个分页窗口处理实例
	CLASSED_WNDPROC_INSTANCE Instances[PageSize];
}CWINSTANCEBLOCK, *PCWINSTANCEBLOCK;
#pragma pack(pop)

static PCWINSTANCEBLOCK				InstBlockList = NULL;
static PCLASSED_WNDPROC_INSTANCE	InstFreeList = NULL;

#define CalcJmpOffset(Src, Dest)	((SIZE_T)(Dest) - ((SIZE_T)(Src) + 5))

static const BYTE CallStaticWndProcCode_x64[32] = 
{		
	0x5B,					/*	POP  RBX	*/
#ifndef MERGE_CLASSED_WND_PROC_CODE
	0x48, 0x8B, 0x43, 0x0C, /*  mov  rax,qword ptr [rbx+0Ch]   ; ClassedWndProc */
	0xFF, 0xE0,             /*  JMP  RAX  */
#else
	0x48, 0x8B, 0x43, 0x08,	/*  MOV  RAX, QWORD PTR [RBX + 8H] ; WndObject instance */
	0x50,					/*  PUSH RAX                       ; arg5 */
	0x48, 0x83, 0xEC, 0x20,	/*  SUB  RSP, 20H                  ; arg1 ~ arg4 */
	0xFF, 0x13,				/*  CALL QWORD PTR [RBX]           ; CClassedWnd::StaticWndProc */
	0x48, 0x83, 0xC4, 0x28,	/*  ADD  RSP, 28H                  ; 5 args */
	0xC3,					/*  RET  */
#endif
};
static const BYTE CallStaticWndProcCode_x86[32] = 
{		
	0x5B,					/*	POP  EBX	*/
#ifndef MERGE_CLASSED_WND_PROC_CODE
	0x8B, 0x43, 0x8,		/*	MOV  EAX,[EBX+8]	*/
	0xFF, 0xE0				/*	JMP  EAX	*/
	0xCC,					/*  INT  3  */
#else
	0x8B, 0x43, 0x04,		/*  MOV  EAX, DWORD PTR [EBX+4]    ; WndObject instance */
	0x50,					/*  PUSH EAX                       ; arg5 */
	0x8B, 0x44, 0x24, 0x14,	/*  MOV  EAX, DWORD PTR [ESP+14H] */
	0x50,					/*  PUSH EAX                       ; arg4 lParam */
	0x8B, 0x44, 0x24, 0x14,	/*  MOV  EAX, DWORD PTR [ESP+14H] */
	0x50,					/*  PUSH EAX                       ; arg3 wParam */
	0x8B, 0x44, 0x24, 0x14,	/*  MOV  EAX, DWORD PTR [ESP+14H] */
	0x50,					/*	PUSH EAX                       ; arg2 uMsg */
	0x8B, 0x44, 0x24, 0x14,	/*  MOV  EAX, DWORD PTR [ESP+14H] */
	0x50,					/*  PUSH EAX                       ; arg1 hWnd */
	0xFF, 0x13,				/*  CALL DWORD PTR [EBX]           ; CClassedWnd::StaticWndProc */
	0xC2, 0x10, 0x00,		/*  RET  10H */
#endif	
};

//将自动选择64位/32位的混合代码
static const BYTE *pCallStaticWndProcCodePtr = (sizeof(INT_PTR) == 8) ? CallStaticWndProcCode_x64 : CallStaticWndProcCode_x86;

WNDPROC AllocClassedWndProcInstance(LPVOID lpInstance, LPVOID lpStaticProc)
{
	PCWINSTANCEBLOCK Block;
	PCLASSED_WNDPROC_INSTANCE Instance, InstanceEnd;

	if ( !InstFreeList )
	{
		Block = (PCWINSTANCEBLOCK)VirtualAlloc( NULL, CWInstanceBlock::PageSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE );
		Block->Next = InstBlockList;
		memcpy(Block->Code, pCallStaticWndProcCodePtr, sizeof(Block->Code));
		Instance = Block->Instances;

		InstanceEnd = Instance + CWInstanceBlock::InstanceCount;
		do
		{
			Instance->Code = 0xE8;	/*	CALL NEAR PTR Offset */
			Instance->Offset = (INT)CalcJmpOffset(Instance, &Block->Code);
			Instance->Next = InstFreeList;
			InstFreeList = Instance;
			Instance++;
		}
		while (Instance < InstanceEnd);
		InstBlockList = Block;
	}
	Instance = InstFreeList;
	InstFreeList = Instance->Next;
	Instance->ClassedProc.lpFunction = lpStaticProc;
	Instance->ClassedProc.lpInstance = lpInstance;
#ifndef MERGE_CLASSED_WND_PROC_CODE
	Instance->ClassedProc.lpClassedWndProc = ClassedWndProc;
#endif
	return (WNDPROC)Instance;
}

VOID FreeClassedWndProcInstance(LPVOID lpInstance)
{
	if ( lpInstance )
	{
		PCLASSED_WNDPROC_INSTANCE lpClassedWndProcInst = (PCLASSED_WNDPROC_INSTANCE)lpInstance;
		lpClassedWndProcInst->Next = InstFreeList;
		InstFreeList = lpClassedWndProcInst;
	}
}

CClassedWnd* CClassedWnd::CreationWnd = NULL;

CClassedWnd::CClassedWnd(void)
{
	m_hWnd = NULL;
	m_pClassWndProcInst = AllocClassedWndProcInstance(this, StaticWndProc);
}

CClassedWnd::~CClassedWnd(void)
{
	FreeClassedWndProcInstance(m_pClassWndProcInst);
	m_pClassWndProcInst = NULL;
}

LRESULT APIENTRY CClassedWnd::InitClassedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CClassedWnd::CreationWnd->m_hWnd = hWnd;
	//优先将窗口处理函数设置为窗口类实例的消息处理函数指针
	SetWindowLongPtr(hWnd, GWLP_WNDPROC, (ULONG_PTR)CClassedWnd::CreationWnd->m_pClassWndProcInst);
	if ( (GetWindowLongPtr(hWnd, GWL_STYLE) & WS_CHILD) && !GetWindowLongPtr(hWnd, GWL_ID) )
	{
		SetWindowLongPtr(hWnd, GWL_ID, (LONG_PTR)hWnd);
	}
	//以上是使用此函数的关键，本来是可以将上面的代码省去而直接对MakeObjectInstance调用将窗口处理函数设置为
	//ClassedWndProc的，但由于有必要为正在创建的窗口类实例保存句柄并检测和设置窗口句柄的GWL_ID，因此这个函数
	//就有了存在的必要。

	//本函数内不得处理任何消息，必须将消息交给窗口类实例的消息处理函数处理
	LRESULT lr = CClassedWnd::CreationWnd->m_pClassWndProcInst(hWnd, uMsg, wParam, lParam);
	CClassedWnd::CreationWnd = NULL;
	return lr;
}

HWND CClassedWnd::MyCreateWindowEx(DWORD dwExStyle, const char* lpClassName, const char* lpWindowName, DWORD dwStyle, int x, int y,
	int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam )
{
	CreationWnd = this;
	return ::CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

LRESULT APIENTRY CClassedWnd::StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, CClassedWnd *pWnd)
{
	return pWnd->WndProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CClassedWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

WNDPROC CClassedWnd::getClassedWndProc()
{
	return InitClassedWndProc;
}
