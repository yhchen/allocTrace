#ifndef __SG2D_STRING_DEF_H__
#define __SG2D_STRING_DEF_H__

#include "base/Array.h"
#include "string/AnsiString.h"
#include "string/MultiByteStringBase.h"
#include "string/UTF8StringBase.h"
#include "string/UnicodeStringBase.h"
#include "string/StringConvert.h"
#include "string/MultiByteString.h"
#include "string/UTF8String.h"
#include "string/UnicodeString.h"
#include "string/String2String.hpp"

namespace SG2D
{
	//编译环境使用的字符串类型
	enum UsingStringType
	{
		StringOfUTF8,
		StringOfMultiByte,
		StringOfUnicode,
	};
    
	//字符串类型选择
#if defined(_MBSTR)
	typedef SG2D::MultiByteString String;
	static const UsingStringType usingStringType = StringOfMultiByte;
#elif defined(_UNICODESTR)
	typedef SG2D::UnicodeString String;
	static const UsingStringType usingStringType = StringOfUnicode;
#else
	typedef SG2D::UTF8String String;
	static const UsingStringType usingStringType = StringOfUTF8;
#endif

	extern const AnsiString NullAnsiStr;
	extern const UTF8String NullUTF8Str;
	extern const UnicodeString NullUnicodeStr;
	extern const String NullStr;
}

#endif
