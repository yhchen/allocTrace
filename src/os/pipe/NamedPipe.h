#ifndef __ALLOC_TRACE_NAMEPIPE_H__
#define __ALLOC_TRACE_NAMEPIPE_H__

/******************************************************************
 *
 *	$ �����ܵ��� $
 *  
 *	- ��Ҫ���� - 
 *
 *	���������������ܵ�����д���ݡ����ݵĶ�ȡʹ���첽�ķ�ʽ��д��
 *	����������ķ�ʽ�������ܵ�����������CNamedPipeObject,�ͻ�����
 *	��CNamedPipeClient��
 *
 *****************************************************************/
#ifdef WIN32

#include "Platform.h"

namespace alloctrace
{
	namespace os
	{
		namespace pipe
		{

			/*	�����ܵ�������

			*/
			class	CNamedPipeObject
			{
			protected:
				/*	implamentation	data-member	declarations	*/
				HANDLE		m_hPipe;
				TCHAR		m_sPipeName[MAX_PATH];
				BOOL		m_boActive;
				DWORD		m_dwTimeOut;
				/*	protected implamentation functions	*/
				VOID	PipeError(INT ErrorCode);
				VOID	CheckActiveProper();
			protected:
				/*	notification call back functions declera for any child class to override. */
				virtual VOID OnOpen();
				virtual VOID OnClose();
				virtual VOID OnError(INT &ErrorCode);
			public:
				/*	object constructor and destructor functions	*/
				CNamedPipeObject();
				virtual	~CNamedPipeObject();

			public:
				/*	property read and adjust functions	*/
				HANDLE	GetHandle();
				LPCTSTR	GetPipeName();
				VOID	SetPipeName(LPCTSTR Value);
				BOOL	GetActive();
				VOID	SetActive(CONST BOOL Value);
				DWORD	GetTimeOut();
				VOID	SetTimeOut(CONST DWORD Value);

			public:
				/*	instance public control functions	*/
				virtual	VOID	Open() = NULL;
				virtual	VOID	Close();
				/*	��ȡ���ݣ�����ʵ�ʶ�ȡ���ֽ�����ע�⣺ʵ�ʶ�ȡ���ֽ�������С�������ȡ���ֽ����� */
				INT		ReadBuf(LPVOID lpBuffer, CONST INT BufferSize);
				/*	д�����ݣ�����ʵ��д����ֽ�����ע�⣺ʵ��д����ֽ�������С������д����ֽ�������ʱ��Ҫ���ѭ��д�룡 */
				INT		WriteBuf(LPVOID lpBuffer, CONST INT BufferSize);
				/*	��ȡ�ܵ��пɶ�ȡ�����ݴ�С	*/
				INT		GetAvaliableReadSize();

			public:
				/*	public propertys	*/
				__declspec(property(get = GetPipeName, put = SetPipeName))				LPCTSTR PipeName;
				__declspec(property(get = GetHandle))									HANDLE	Handle;
				__declspec(property(get = GetActive, put = SetActive))					BOOL	Active;
				__declspec(property(get = GetTimeOut, put = SetTimeOut))				DWORD	TimeOut;
				__declspec(property(get = GetAvaliableReadSize))						INT		AvaliableSize;

			};


			/*	�����ܵ���������

			*/
			class	CNamedPipeServer : public CNamedPipeObject
			{
				/*	type definition of parent class	*/
				typedef	CNamedPipeObject Inherited;
			private:
				/*	implamentation	data-member	declarations	*/
				DWORD		m_dwOutBufferSize;
				DWORD		m_dwInBufferSize;
			public:
				/*	object constructor and destructor functions	*/
				CNamedPipeServer();

			public:
				/*	property read and adjust functions	*/
				DWORD	GetInBufferSize();
				VOID	SetInBufferSize(CONST DWORD Value);
				DWORD	GetOutBufferSize();
				VOID	SetOutBufferSize(CONST DWORD Value);
			public:
				/*	instance public control functions	*/
				VOID	Open();
			public:
				/*	public propertys	*/
				__declspec(property(get = GetInBufferSize, put = SetInBufferSize))		DWORD	InBufferSize;
				__declspec(property(get = GetOutBufferSize, put = SetOutBufferSize))	DWORD	OutBufferSize;

			};

			/*	�����ܵ��ͻ�����

			*/
			class	CNamedPipeClient : public CNamedPipeObject
			{
				/*	type definition of parent class	*/
				typedef	CNamedPipeObject Inherited;
			public:
				/*	object constructor and destructor functions	*/
				CNamedPipeClient();

			public:
				/*	instance public control functions	*/
				VOID	Open();
			};

		}
	}
}

#endif

#endif // __ALLOC_TRACE_NAMEPIPE_H__

