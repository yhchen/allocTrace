#include "stdafx.h"
#ifdef _WIN32
#include <tchar.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include "stdafx.h"
#include <stdlib.h>
#if defined(WIN32)
#include "WinIconv.h"
#else
#include <iconv.h>
#define _UNSPPORT_CONV ((iconv_t*)-1)
#endif

namespace SG2D
{
	static iconv_t mbs2uni = _UNSPPORT_CONV;
	static iconv_t mbs2utf = _UNSPPORT_CONV;
	static iconv_t utf2mbs = _UNSPPORT_CONV;
	static iconv_t utf2uni = _UNSPPORT_CONV;
	static iconv_t uni2mbs = _UNSPPORT_CONV;
	static iconv_t uni2utf = _UNSPPORT_CONV;

	static const char *MBS_CODE = "";
	static const char *UTF8_CODE = "UTF-8";
// 	int __test(unsigned int namescount, const char * const * names, void* data)
// 	{
// 		for (int i=0; i<namescount; ++i)
// 		{
// 			OutputDebugString(names[i]);
// 			OutputDebugString("\n");
// 		}
// 		return 0;
// 	}
    
    static const char* _unicode_code_name_()
    {
        return sizeof(wchar_t) == 4 ? "UTF-32LE" : "UTF-16LE";
    }

	template<class SOURCE_RAWSTR, class DEST, int DESTPAND>
	static void StringConvert(const SOURCE_RAWSTR* src, size_t srcLen, DEST &destination, iconv_t icv)
	{
		if (srcLen > 0)
		{
			size_t destLen = (size_t)(srcLen * DESTPAND);
			destination.setLength(destLen);
			size_t srcbytes = srcLen * sizeof(*src);

			typename DEST::TYPE *dest = destination.own_ptr();
			size_t destbytesSize = destLen * sizeof(*dest);
			size_t destbytes = destbytesSize;

			size_t err;
#if defined(__APPLE__) || defined(__MACH__) || defined(__LINUX__)
			err = iconv(icv, (char**)&src, &srcbytes, (char**)&dest, &destbytes);
#else
			err = iconv(icv, (const char**)&src, &srcbytes, (char**)&dest, &destbytes);
#endif
			if (err >= 0 && destbytesSize >= destbytes)
				destination.setLength((destbytesSize - destbytes) / sizeof(*dest));
			else destination.setLength(0);
		}
		else destination.setLength(0);
	}

	template<class SOURCE, class DEST, int DESTPAND>
	static void StringConvert(const SOURCE& source, DEST &destination, iconv_t icv)
	{
		StringConvert<typename SOURCE::TYPE, DEST, DESTPAND>(source.ptr(), source.length(), destination, icv);
	}

	UnicodeStringBase MultiByteString2UnicodeString(const MultiByteStringBase &mbs)
	{
		UnicodeStringBase uni;
		MultiByteString2UnicodeString(uni, mbs);
		return uni;
	}

	void MultiByteString2UnicodeString(UnicodeStringBase& dest, const MultiByteStringBase &mbs)
	{
		if (mbs2uni == _UNSPPORT_CONV)
			mbs2uni = iconv_open(_unicode_code_name_(), MBS_CODE);
		if (mbs2uni != _UNSPPORT_CONV)
			StringConvert<MultiByteStringBase, UnicodeStringBase, 1>(mbs, dest, mbs2uni);
	}

	UnicodeStringBase UTF8String2UnicodeString(const UTF8StringBase &utf)
	{
		UnicodeStringBase uni;
		UTF8String2UnicodeString(uni, utf);
		return uni;
	}

	void UTF8String2UnicodeString(UnicodeStringBase& dest, const UTF8StringBase &utf)
	{
		if (utf2uni == _UNSPPORT_CONV)
			utf2uni = iconv_open(_unicode_code_name_(), UTF8_CODE);
		if (utf2uni != _UNSPPORT_CONV)
			StringConvert<UTF8StringBase, UnicodeStringBase, 1>(utf, dest, utf2uni);
	}

	UnicodeStringBase MultiByteString2UnicodeString(const char* cstr, size_t len)
	{
		UnicodeStringBase uns;
		MultiByteString2UnicodeString(uns, cstr, len);
		return uns;
	}

	UnicodeStringBase UTF8String2UnicodeString(const char* cstr, size_t len)
	{
		UnicodeStringBase uns;
		UTF8String2UnicodeString(uns, cstr, len);
		return uns;
	}

	void MultiByteString2UnicodeString(UnicodeStringBase& dest, const char* cstr, size_t len)
	{
		if (mbs2uni == _UNSPPORT_CONV)
			mbs2uni = iconv_open(_unicode_code_name_(), MBS_CODE);
		if (mbs2uni != _UNSPPORT_CONV)
			StringConvert<MultiByteString::TYPE, UnicodeStringBase, 1>(cstr, len, dest, mbs2uni);
	}

	void UTF8String2UnicodeString(UnicodeStringBase& dest, const char* cstr, size_t len)
	{
		if (utf2uni == _UNSPPORT_CONV)
			utf2uni = iconv_open(_unicode_code_name_(), UTF8_CODE);
		if (utf2uni != _UNSPPORT_CONV)
			StringConvert<UTF8StringBase::TYPE, UnicodeStringBase, 1>(cstr, len, dest, utf2uni);
	}

	void MultiByteString2UnicodeString(UnicodeStringBase& dest, const MultiByteStringBase& mbs, const char* encoding)
	{
		iconv_t convt = iconv_open(_unicode_code_name_(), encoding);
		if (convt != _UNSPPORT_CONV)
			StringConvert<MultiByteStringBase, UnicodeStringBase, 4>(mbs, dest, convt);
		iconv_close(convt);
	}

	void MultiByteString2UnicodeString(UnicodeStringBase& dest, const char* cstr, size_t len, const char* encoding)
	{
		iconv_t convt = iconv_open(_unicode_code_name_(), encoding);
		if (convt != _UNSPPORT_CONV)
			StringConvert<MultiByteString::TYPE, UnicodeStringBase, 1>(cstr, len, dest, convt);
		iconv_close(convt);
	}






	MultiByteStringBase UnicodeString2MultiByteString(const UnicodeStringBase& uns)
	{
		MultiByteStringBase mbs;
		UnicodeString2MultiByteString(mbs, uns);
		return mbs;
	}

	void UnicodeString2MultiByteString(MultiByteStringBase& dest, const UnicodeStringBase& uns)
	{
		UnicodeString2MultiByteString(dest, uns.ptr(), uns.length());
	}

	void UnicodeString2MultiByteString(MultiByteStringBase& dest, const wchar_t* wstr, size_t len)
	{
		if (uni2mbs == _UNSPPORT_CONV)
			uni2mbs = iconv_open(MBS_CODE, _unicode_code_name_());
		if (uni2mbs != _UNSPPORT_CONV)
			StringConvert<UnicodeStringBase::TYPE, MultiByteStringBase, 2>(wstr, len, dest, uni2mbs);
	}

	MultiByteStringBase UTF8String2MultiByteString(const UTF8StringBase& utf)
	{
		MultiByteStringBase mbs;
		UTF8String2MultiByteString(mbs, utf);
		return mbs;
	}

	void UTF8String2MultiByteString(MultiByteStringBase& dest, const UTF8StringBase& utf)
	{
		UTF8String2MultiByteString(dest, utf.ptr(), utf.length());
	}

	void UTF8String2MultiByteString(MultiByteStringBase& dest, const char* cstr, size_t len)
	{
		if (utf2mbs == _UNSPPORT_CONV)
			utf2mbs = iconv_open(MBS_CODE, UTF8_CODE);
		if (utf2mbs != _UNSPPORT_CONV)
			StringConvert<UTF8StringBase::TYPE, MultiByteStringBase, 1>(cstr, len, dest, utf2mbs);
	}
    
	void UTF8String2MultiByteString(MultiByteStringBase& dest, const UTF8StringBase& utf, const char* encoding)
	{
		UTF8String2MultiByteString(dest, utf.ptr(), utf.length(), encoding);
	}
    
	void UTF8String2MultiByteString(MultiByteStringBase& dest, const char* cstr, size_t len, const char* encoding)
	{
		iconv_t convt = iconv_open(encoding, UTF8_CODE);
		if (convt != _UNSPPORT_CONV)
			StringConvert<UTF8StringBase::TYPE, MultiByteStringBase, 1>(cstr, len, dest, convt);
		iconv_close(convt);
	}

	void MultiByteString2MultiByteString(MultiByteStringBase& dest, const MultiByteStringBase& mbs, 
		const char* destEncoding, const char* sourceEncoding)
	{
		MultiByteString2MultiByteString(dest, mbs.ptr(), mbs.length(), destEncoding, sourceEncoding);
	}

	void MultiByteString2MultiByteString(MultiByteStringBase& dest, const char* cstr, size_t len, 
		const char* destEncoding, const char* sourceEncoding)
	{
		iconv_t convt = iconv_open(destEncoding, sourceEncoding);
		if (convt != _UNSPPORT_CONV)
			StringConvert<MultiByteStringBase::TYPE, MultiByteStringBase, 4>(cstr, len, dest, convt);
		iconv_close(convt);
	}





	UTF8StringBase UnicodeString2UTF8String(const UnicodeStringBase& uns)
	{
		UTF8StringBase utf;
		UnicodeString2UTF8String(utf, uns);
		return utf;
	}

	void UnicodeString2UTF8String(UTF8StringBase& dest, const UnicodeStringBase& uns)
	{
		UnicodeString2UTF8String(dest, uns.ptr(), uns.length());
	}

	void UnicodeString2UTF8String(UTF8StringBase& dest, const wchar_t* wstr, size_t len)
	{
		if (uni2utf == _UNSPPORT_CONV)
			uni2utf = iconv_open(UTF8_CODE, _unicode_code_name_());
		if (uni2utf != _UNSPPORT_CONV)
			StringConvert<UnicodeStringBase::TYPE, UTF8StringBase, 3>(wstr, len, dest, uni2utf);
	}

	UTF8StringBase MultiByteString2UTF8String(const MultiByteStringBase& mbs)
	{
		UTF8StringBase utf;
		MultiByteString2UTF8String(utf, mbs);
		return utf;
	}

	void MultiByteString2UTF8String(UTF8StringBase& dest, const MultiByteStringBase& mbs)
	{
		MultiByteString2UTF8String(dest, mbs.ptr(), mbs.length());
	}

	void MultiByteString2UTF8String(UTF8StringBase& dest, const char* cstr, size_t len)
	{
		if (mbs2utf == _UNSPPORT_CONV)
			mbs2utf = iconv_open(UTF8_CODE, MBS_CODE);
		if (mbs2utf != _UNSPPORT_CONV)
			StringConvert<MultiByteStringBase::TYPE, UTF8StringBase, 2>(cstr, len, dest, mbs2utf);
	}

	void MultiByteString2UTF8String(UTF8StringBase& dest, const MultiByteStringBase& mbs, const char* encoding)
	{
        MultiByteString2UTF8String(dest, mbs.ptr(), mbs.length(), encoding);
	}
    
	void MultiByteString2UTF8String(UTF8StringBase& dest, const char* cstr, size_t len, const char* encoding)
    {
		iconv_t convt = iconv_open(UTF8_CODE, encoding);
		if (convt != _UNSPPORT_CONV)
			StringConvert<MultiByteStringBase::TYPE, UTF8StringBase, 4>(cstr, len, dest, convt);
		iconv_close(convt);
    }
}
