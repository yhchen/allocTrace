#include "Platform.h"
#include <string>
#include "NPComm.h"
#include "NamedPipe.h"

using namespace alloctrace::os::pipe;

#if defined(WIN32)

CNamedPipeServer::CNamedPipeServer():Inherited()
{
  m_dwInBufferSize	= NP_DEFAULT_INBUFSIZE;
  m_dwOutBufferSize	= NP_DEFAULT_OUTBUFSIZE;
  SetTimeOut( NP_DEFAULT_TIMEOUT );
}

DWORD CNamedPipeServer::GetInBufferSize()
{
	return m_dwInBufferSize;
}

VOID CNamedPipeServer::SetInBufferSize(CONST DWORD Value)
{
	CheckActiveProper();
	m_dwInBufferSize = Value;
}

DWORD CNamedPipeServer::GetOutBufferSize()
{
	return m_dwOutBufferSize;
}

VOID CNamedPipeServer::SetOutBufferSize(CONST DWORD Value)
{
	CheckActiveProper();
	m_dwOutBufferSize = Value;
}

VOID CNamedPipeServer::Open()
{
	if ( !m_boActive )
	{
		m_hPipe = CreateNamedPipe( m_sPipeName, PIPE_ACCESS_DUPLEX, PIPE_NOWAIT,
			PIPE_UNLIMITED_INSTANCES, m_dwOutBufferSize, m_dwInBufferSize, m_dwTimeOut, NULL );
		if ( m_hPipe == INVALID_HANDLE_VALUE )
		{
			PipeError( GetLastError() );
		}
		else 
		{
			m_boActive = TRUE;
			OnOpen();
		}
	}
}
#endif
