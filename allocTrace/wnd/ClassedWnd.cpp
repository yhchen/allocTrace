#include "stdafx.h"
#include "ClassedWnd.h"
#include <Windows.h>
#include <TCHAR.h>
/************************************************************************/
/*
/*  ��Ϣ����������ʵ���ҹ���ԭ�������                                
/*    1) CClassedWnd���ڹ��캯���л����AllocClassedWndProcInstance������������
/*       ��ʼ����ҳ��Ϣ�����ڴ���벹���飻
/*    2) ע�����͵�ʱ�򣬽����ڴ�����ָ��ΪgetClassedWndProc()�����ķ���ֵ����
/*       ָ��CClassedWnd::InitClassedWndProc��̬������һ������ָ�룻
/*    3) ��CClassedWnd::InitClassedWndProc�����ڲ������Ľ����ǽ����ڴ�������
/*       �θı䴰��ʵ������Ϣ���������ڴ油����ַ���Դ���ɽ����ڴ���������ʵ���Ĺҹ���
/*              
/*  ����Ϣ������õ���ʵ�����ڴ油��������            
/*    1) ���õĽṹ�У�ClassedWndProcInstance��CWInstanceBlock�������ṹ���г�Ա
/*       ˵����
/*    2) �ڴ油���Ĵ�����ù���Ϊ��
/*       ��pWnd->m_pClassWndProcInst��ʼִ�У�
/*         CALL NEAR PTR Offset - ִ�д����ڴ油������(CWInstanceBlock.Code)
/*         POP EBX              - ��pWnd->m_pClassWndProcInst->ClassedProc�ĵ�ַ������EBX�У��õ�ַ����һ��CALL�ķ��ص�ַ������ָ��pWnd->m_pClassWndProcInst->ClassedProc�ĵ�ַ��
/*         JMP ClassedWndProc   - ��ת��ClassedWndProc����
/*       ��ClassedWndProc������,[EBX]��ΪpWnd->m_pClassWndProcInst->ClassedProc.lpFunction��CClassedWnd::StaticWndProc��,
/*       [EBX+4]ΪpWnd->m_pClassWndProcInst->ClassedProc.lpInstance������ʵ������ͨ����2�����ݼ��ɵ���CClassedWnd::StaticWndProc��
/*       CClassedWnd::StaticWndProc����������ɶ�pWnd->WndProc�麯���ĵ��ã��Ӵ������Ϣ��������
/*              
/************************************************************************/

using namespace wylib::window;

/*
 * �����Ƿ񽫵���CClassedWnd::StaticWndProc�ĺ���������ȫ��ϵ�ClassedWndProcInstance�еĴ�����
 * ���ȡ���˺�Ķ��壬���ʹ�������ĺ������õ���ʽͨ��ClassedWndProc��ຯ����ɶ�CClassedWnd::StaticWndProc�ĵ��ã�
 * ����ClassedWndProc��ຯ���Ĵ��뿽����ClassedWndProcInstance�дӶ���ȥ�������á�
 * ����ǳ���ѧϰ���˽������໯������Ϣ����������Ƶ�Ŀ�ģ�ȡ������꽫ʹ�õ��Թ����и�����������׻��ơ�
 * ��ʽʹ�õ�ʱ��ʹ�û�ϴ���ķ�ʽ���Ч��
 */
#define MERGE_CLASSED_WND_PROC_CODE


#ifndef MERGE_CLASSED_WND_PROC_CODE
extern "C" LRESULT APIENTRY ClassedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

#pragma pack(push, 1)
typedef struct ClassedWndProcInstance
{
	BYTE	Code;		//����ʵ�ִ��룺E8 CALL NEAR PTR Offset
	INT		Offset;		//����ʵ�ִ��룺�����ڴ����ת������ַ
	union
	{
		//��ʼ����ʱ��ָ����һ�����õ�ClassedWndProcInstance
		struct ClassedWndProcInstance* Next;
		struct
		{
			//��������Ϣ��������ַ��
			//������ʽ�����ǣ�LRESULT APIENTRY (*StaticClassedWndProc)(CClassedWnd *, HWND, UINT, WPARAM, LPARAM)
			LPVOID lpFunction;
			//���ڶ���ʵ������һ��CClassWnd���ָ��
			LPVOID lpInstance;	
#ifndef MERGE_CLASSED_WND_PROC_CODE
			//ClassedWndProc�ĵ�ַ��ÿ��ʵ�����һ����Ϊ��ʵ��x86��x64������ClassedWndProc������һ����
			LPVOID lpClassedWndProc;  ������CClassedWnd::StaticWndProc�Ĵ�������������Ҫ�˳�Ա
#endif
		}ClassedProc;
	};
}CLASSED_WNDPROC_INSTANCE, *PCLASSED_WNDPROC_INSTANCE;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct CWInstanceBlock
{
	//����һ����ҳ���ڴ���ִ�д��벹������ڴ��С
	static const INT_PTR PageSize = 4096;
	//�˽ṹ����4096���ڴ������������CLASSED_WNDPROC_INSTANCE�ṹ���������
	static const INT_PTR InstanceCount = (PageSize - 40) / sizeof(ClassedWndProcInstance);	

	struct CWInstanceBlock*	Next;	//ָ����һ�����õķ�ҳ���ڴ���ִ�д��벹����
#ifndef MERGE_CLASSED_WND_PROC_CODE
	BYTE Code[8];					//����ʵ�ִ��룺POP EBX MOV EAX, [EBX+C] JMP EAX
#else
	BYTE Code[32];					//����ʵ�ֵ���CClassedWnd::StaticWndProc�Ļ�ϴ���
#endif
	//������һ���ڴ����ʵ�ֶ����ҳ���ڴ���ʵ��
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

//���Զ�ѡ��64λ/32λ�Ļ�ϴ���
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
	//���Ƚ����ڴ���������Ϊ������ʵ������Ϣ������ָ��
	SetWindowLongPtr(hWnd, GWLP_WNDPROC, (ULONG_PTR)CClassedWnd::CreationWnd->m_pClassWndProcInst);
	if ( (GetWindowLongPtr(hWnd, GWL_STYLE) & WS_CHILD) && !GetWindowLongPtr(hWnd, GWL_ID) )
	{
		SetWindowLongPtr(hWnd, GWL_ID, (LONG_PTR)hWnd);
	}
	//������ʹ�ô˺����Ĺؼ��������ǿ��Խ�����Ĵ���ʡȥ��ֱ�Ӷ�MakeObjectInstance���ý����ڴ���������Ϊ
	//ClassedWndProc�ģ��������б�ҪΪ���ڴ����Ĵ�����ʵ�����������������ô��ھ����GWL_ID������������
	//�����˴��ڵı�Ҫ��

	//�������ڲ��ô����κ���Ϣ�����뽫��Ϣ����������ʵ������Ϣ����������
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
