#include "NamedPipe.h"
#include "NPComm.h"

using namespace alloctrace::os::pipe;

#ifdef WIN32

CNamedPipeClient::CNamedPipeClient():Inherited()
{
	SetTimeOut( NMPWAIT_WAIT_FOREVER );
}

VOID CNamedPipeClient::Open()
{
	INT nErr;

	while ( !m_boActive )
	{
		m_hPipe = CreateFile( m_sPipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0 );
		if ( m_hPipe != INVALID_HANDLE_VALUE )
		{
			m_boActive = TRUE;
			OnOpen();
			break;
		}

		nErr = GetLastError();
		if ( nErr != ERROR_PIPE_BUSY )
		{
			PipeError( nErr );
			break;
		}

		if ( !WaitNamedPipe( m_sPipeName, m_dwTimeOut ) )
		{
			PipeError( GetLastError() );
			break;
		}
	}
}
#endif