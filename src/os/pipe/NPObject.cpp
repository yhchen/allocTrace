#include <string>
#include "alloctrace.h"
#include "NamedPipe.h"
#include "NPComm.h"
#if defined(WIN32)
#include <tchar.h>
#pragma warning(disable:4996)
#endif

using namespace alloctrace::os::pipe;


#if defined(WIN32)

#define	ArrayCount(a)		(sizeof(a)/sizeof((a)[0]))

CNamedPipeObject::CNamedPipeObject()
{
  m_hPipe			= INVALID_HANDLE_VALUE;
  m_boActive		= FALSE;
  m_dwTimeOut		= NP_DEFAULT_TIMEOUT;
  SetPipeName(NP_DEFAULT_PIPENAME);
}

CNamedPipeObject::~CNamedPipeObject()
{
	Close();
}

VOID CNamedPipeObject::PipeError(INT ErrorCode)
{
	TCHAR str[MAX_PATH];

	OnError( ErrorCode );

	if ( ErrorCode != ERROR_SUCCESS )
	{
		_sntprintf( str, ArrayCount(str), _T("管道错误:%d"), ErrorCode );
		//OutputDebugString(str);
		Close();
	}
}

VOID CNamedPipeObject::CheckActiveProper()
{
	if ( m_boActive )
		OutputDebugString(_T("不能在管道建立后更改属性！"));
}

HANDLE CNamedPipeObject::GetHandle()
{
	return m_hPipe;
}

LPCTSTR CNamedPipeObject::GetPipeName()
{
	return m_sPipeName;
}

VOID CNamedPipeObject::SetPipeName(LPCTSTR Value)
{
	CheckActiveProper();
	size_t len = _tcslen( Value );
	if ( len >= sizeof(m_sPipeName) )
	{
		_tcsncpy( m_sPipeName, Value, sizeof(m_sPipeName) - 1 );
		m_sPipeName[len] = 0;
	}
	else _tcscpy( m_sPipeName, Value );
}

BOOL CNamedPipeObject::GetActive()
{
	return m_boActive;
}

VOID CNamedPipeObject::SetActive(CONST BOOL Value)
{
	if ( m_boActive != Value )
	{
		if ( Value )
			Open();
		else Close();
	}
}

DWORD CNamedPipeObject::GetTimeOut()
{
	return m_dwTimeOut;
}

VOID CNamedPipeObject::SetTimeOut(CONST DWORD Value)
{
	CheckActiveProper();
	m_dwTimeOut = Value;
}

VOID CNamedPipeObject::OnOpen()
{
}

VOID CNamedPipeObject::OnClose()
{
}

VOID CNamedPipeObject::OnError(INT &ErrorCode)
{
}

VOID CNamedPipeObject::Close()
{
  if ( m_boActive )
  {
	  m_boActive  = FALSE;
	  OnClose();
	  CloseHandle( m_hPipe );
	  m_hPipe = INVALID_HANDLE_VALUE;
  }
}

INT CNamedPipeObject::ReadBuf(LPVOID lpBuffer, CONST INT BufferSize)
{
	DWORD	BytesReaded;
	INT		nErr;
	
	if ( !ReadFile( m_hPipe, lpBuffer, BufferSize, &BytesReaded, NULL ) )
	{
		nErr = GetLastError();
		switch ( nErr )
		{
		case ERROR_NO_DATA:
		case ERROR_PIPE_LISTENING:
			return 0;
		default:
			PipeError( nErr );
			return 0;
		}
	}
	else return BytesReaded;
}

INT CNamedPipeObject::WriteBuf(LPVOID lpBuffer, CONST INT BufferSize)
{
	DWORD BytesWriten;

	if ( !WriteFile( m_hPipe, lpBuffer, BufferSize, &BytesWriten, NULL ) )
	{
		PipeError( GetLastError() );
		return 0;
	}
	else return BytesWriten;
}

INT CNamedPipeObject::GetAvaliableReadSize()
{
	return m_boActive ? GetFileSize( m_hPipe, NULL ) : 0 ;
}
#endif
