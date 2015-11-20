#include <stdlib.h>
#include <assert.h>
#if defined(WIN32)
#include <windows.h>
#endif
#include "BaseStream.h"

using namespace alloctrace::stream;

size_t CBaseStream::getSize()
{
	size_t nPos = seek(0, soCurrent);
	size_t nSize = seek(0, soEnd);
	seek( nPos, soBeginning );
	return nSize;
}

size_t CBaseStream::setPosition(const size_t tPosition)
{
	return seek( tPosition, soBeginning );
}

size_t CBaseStream::copyFrom(CBaseStream& stream, size_t tSizeToCopy)
{
	static const size_t OnceReadBytes = 8192;
	size_t nSizeCopyed, nSizeToRead, nSizeReaded, nOldPosition;

	//保存原始流当前的指针位置
	nOldPosition = stream.seek( 0, soCurrent );

	if ( tSizeToCopy == 0 )
	{
		stream.seek( 0, soBeginning );
		tSizeToCopy = stream.getSize();
	}

	if ( tSizeToCopy <= 0 )
		return tSizeToCopy;

	nSizeCopyed = 0;
	void *pBuffer = malloc( OnceReadBytes );

	while ( nSizeCopyed < tSizeToCopy )
	{
		nSizeToRead = tSizeToCopy - nSizeCopyed;
		if ( nSizeToRead > OnceReadBytes )
			nSizeToRead = OnceReadBytes;
		nSizeReaded = stream.read( pBuffer, (DWORD)nSizeToRead );
		if ( nSizeReaded <= 0 )
			break;
		write( pBuffer, (DWORD)nSizeReaded );
		nSizeCopyed += nSizeReaded;
	}

	free( pBuffer );

	//恢复原始流的指针位置
	stream.seek( nOldPosition, soBeginning );
	return nSizeCopyed;
}
