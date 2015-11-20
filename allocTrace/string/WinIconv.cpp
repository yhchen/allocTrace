#include "stdafx.h"
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "WinIconv.h"

#pragma warning(disable:4996)

namespace SG2D
{
	union iconv_t_data
	{
		unsigned int iconver;
		struct 
		{
			unsigned short dest;
			unsigned short src;
		};
	};

	enum CodeNameType
	{
		cnUnknown = 0,
		cnUCS = 1,
		cnUTF8 = 2,
		cnMBS = 3,
	};

	const char UCSCodeName[] = "UTF-16LE";
	const char UTF8CodeName[] = "UTF-8\0\0\0\0";
	const char MBSCodeName[] = "\0\0\0\0\0\0\0\0";
	const char GBKCodeName[] = "GB18030\0\0";
	
	static CodeNameType getCodeNameType(const char *codeName)
	{
		long long c1 = 0;
		strncpy((char*)&c1, codeName, sizeof(c1));
		if (c1 == *((long long*)UCSCodeName))
			return cnUCS;
		if (c1 == *((long long*)UTF8CodeName))
			return cnUTF8;
		if (c1 == *((long long*)MBSCodeName))
			return cnMBS;
		if (c1 == *((long long*)GBKCodeName))
			return cnMBS;
		return cnUnknown;
	}

	iconv_t iconv_open(const char* targetEncoding, const char* sourceEncoding)
	{
		iconv_t_data id;
		id.dest = getCodeNameType(targetEncoding);
		id.src = getCodeNameType(sourceEncoding);
		if (id.src == 0 || id.dest == 0)
			return -1;
		return id.iconver;
	}

	void iconv_close(iconv_t iconver)
	{

	}

	int iconv(iconv_t iconver, const char** source, size_t *bytesSource, char** dest, size_t *bytesDest)
	{
		iconv_t_data id;
		id.iconver = iconver;
		if (id.src == 0 || id.dest == 0)
			return -1;
		int result = -1;
		switch(id.dest)
		{
		case cnUCS:
			{
				result = MultiByteToWideChar(id.src == cnUTF8 ? CP_UTF8 : CP_ACP, 0, *source, (int)*bytesSource, (LPWSTR)*dest, (int)(*bytesDest/sizeof(wchar_t)));
				if (result > 0)
				{
					*source += *bytesSource;
					*bytesSource = 0;
					result *= sizeof(wchar_t);
					*dest += result;
					*bytesDest -= result;
				}
			}
			break;
		case cnUTF8:
		case cnMBS:
			{
				wchar_t *tempBuffer = NULL, *ucsBuffer = NULL;
				size_t ucsLength = 0;
				if (id.src == cnUCS)
				{
					ucsBuffer = (wchar_t *)*source;
					ucsLength = *bytesSource / sizeof(wchar_t);
				}
				else
				{
					tempBuffer = (wchar_t*)malloc(*bytesSource * sizeof(wchar_t));
					ucsLength = MultiByteToWideChar(id.src == cnUTF8 ? CP_UTF8 : CP_ACP, 0, *source, (int)*bytesSource, tempBuffer, (int)*bytesSource);
					if (ucsLength <= 0)
					{
						free(tempBuffer);
						return -1;
					}
					ucsBuffer = tempBuffer;
				}
				result = WideCharToMultiByte(id.dest == cnUTF8 ? CP_UTF8 : CP_ACP, 0, ucsBuffer, (int)ucsLength, *dest, (int)*bytesDest, NULL, FALSE);
				if (result > 0)
				{
					*source += *bytesSource;
					*bytesSource = 0;
					*dest += result;
					*bytesDest -= result;
				}
				if (tempBuffer) free(tempBuffer);
			}
			break;
		}
		return result;
	}
}
