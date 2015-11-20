#ifndef __SG2D_WIN_ICONV_H__
#define __SG2D_WIN_ICONV_H__

namespace SG2D
{
	typedef unsigned int iconv_t;
#define _UNSPPORT_CONV -1

	iconv_t iconv_open(const char* targetEncoding, const char* sourceEncoding);
	void iconv_close(iconv_t iconver);
	int iconv(iconv_t iconver, const char** source, size_t *bytesSource, char** dest, size_t *bytesDest);
}

#endif
