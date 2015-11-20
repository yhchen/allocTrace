#ifndef __SG2D_STRING_TO_STRING_H__
#define __SG2D_STRING_TO_STRING_H__

namespace SG2D
{
	template<class SOURCE>
	inline UnicodeString String2UnicodeString(const SOURCE &str)
	{
		UnicodeString ret = str;
		return ret;
	}

	template<class SOURCE>
	inline UnicodeString& String2UnicodeString(UnicodeString &dest, const SOURCE &str)
	{
		dest = str;
		return dest;
	}

	template<class SOURCE>
	inline MultiByteString String2MultiByteString(const SOURCE &str)
	{
		MultiByteString ret = str;
		return ret;
	}

	template<class SOURCE>
	inline MultiByteString& String2MultiByteString(MultiByteString &dest, const SOURCE &str)
	{
		dest = str;
		return dest;
	}

	template<class SOURCE>
	inline UTF8String String2UTF8String(const SOURCE &str)
	{
		UTF8String ret = str;
		return ret;
	}

	template<class SOURCE>
	inline UTF8String& String2UTF8String(UTF8String &dest, const SOURCE &str)
	{
		dest = str;
		return dest;
	}

	template<class DEST, class SOURCE>
	inline DEST String2String(const SOURCE &str)
	{
		DEST dest = str;
		return dest;
	}

	template<class DEST, class SOURCE>
	inline DEST& String2String(DEST &dest, const SOURCE &str)
	{
		dest = str;
		return dest;
	}
}

#endif