#ifndef __ALLOC_TRACE_NAMEPIPE_H__
#define __ALLOC_TRACE_NAMEPIPE_H__

/******************************************************************
 *
 *	$ 命名管道库 $
 *  
 *	- 主要功能 - 
 *
 *	建立和连接命名管道并读写数据。数据的读取使用异步的方式，写入
 *	则采用阻塞的方式。命名管道服务器类是CNamedPipeObject,客户端类
 *	是CNamedPipeClient。
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

			/*	命名管道基础类

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
				/*	读取数据，返回实际读取的字节数。注意：实际读取的字节数可能小于请求读取的字节数！ */
				INT		ReadBuf(LPVOID lpBuffer, CONST INT BufferSize);
				/*	写入数据，返回实际写入的字节数。注意：实际写入的字节数可能小于请求写入的字节数，此时需要多次循环写入！ */
				INT		WriteBuf(LPVOID lpBuffer, CONST INT BufferSize);
				/*	获取管道中可读取的数据大小	*/
				INT		GetAvaliableReadSize();

			public:
				/*	public propertys	*/
				__declspec(property(get = GetPipeName, put = SetPipeName))				LPCTSTR PipeName;
				__declspec(property(get = GetHandle))									HANDLE	Handle;
				__declspec(property(get = GetActive, put = SetActive))					BOOL	Active;
				__declspec(property(get = GetTimeOut, put = SetTimeOut))				DWORD	TimeOut;
				__declspec(property(get = GetAvaliableReadSize))						INT		AvaliableSize;

			};


			/*	名名管道服务器类

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

			/*	名名管道客户端类

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

