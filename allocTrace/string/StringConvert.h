#ifndef __SG2D_STRING_CONVERT_H__
#define __SG2D_STRING_CONVERT_H__

namespace SG2D
{
	/**
	 * SG2D 不同编码格式之间字符串转换库
	 *
	 */
	UnicodeStringBase MultiByteString2UnicodeString(const MultiByteStringBase &mbs);
	void MultiByteString2UnicodeString(UnicodeStringBase& dest, const MultiByteStringBase &mbs);
	UnicodeStringBase UTF8String2UnicodeString(const UTF8StringBase &utf);
	void UTF8String2UnicodeString(UnicodeStringBase& dest, const UTF8StringBase &utf);
	UnicodeStringBase MultiByteString2UnicodeString(const char* cstr, size_t len);
	UnicodeStringBase UTF8String2UnicodeString(const char* cstr, size_t len);
	void MultiByteString2UnicodeString(UnicodeStringBase& dest, const char* cstr, size_t len);
	void MultiByteString2UnicodeString(UnicodeStringBase& dest, const char* cstr, size_t len, const char* encoding);
	void UTF8String2UnicodeString(UnicodeStringBase& dest, const char* cstr, size_t len);

	MultiByteStringBase UnicodeString2MultiByteString(const UnicodeStringBase& uns);
	void UnicodeString2MultiByteString(MultiByteStringBase& dest, const UnicodeStringBase& uns);
	void UnicodeString2MultiByteString(MultiByteStringBase& dest, const wchar_t* wstr, size_t len);
	MultiByteStringBase UTF8String2MultiByteString(const UTF8StringBase& utf);
	void UTF8String2MultiByteString(MultiByteStringBase& dest, const UTF8StringBase& utf);
	void UTF8String2MultiByteString(MultiByteStringBase& dest, const char* cstr, size_t len);
	void UTF8String2MultiByteString(MultiByteStringBase& dest, const UTF8StringBase& utf, const char*encoding);
	void UTF8String2MultiByteString(MultiByteStringBase& dest, const char* cstr, size_t len, const char*encoding);
	void MultiByteString2MultiByteString(MultiByteStringBase& dest, const MultiByteStringBase& mbs, 
		const char* destEncoding, const char* sourceEncoding);
	void MultiByteString2MultiByteString(MultiByteStringBase& dest, const char* cstr, size_t len, 
		const char* destEncoding, const char* sourceEncoding);

	UTF8StringBase UnicodeString2UTF8String(const UnicodeStringBase& uns);
	void UnicodeString2UTF8String(UTF8StringBase& dest, const UnicodeStringBase& uns);
	void UnicodeString2UTF8String(UTF8StringBase& dest, const wchar_t* wstr, size_t len);
	UTF8StringBase MultiByteString2UTF8String(const MultiByteStringBase& mbs);
	void MultiByteString2UTF8String(UTF8StringBase& dest, const MultiByteStringBase& mbs);
	void MultiByteString2UTF8String(UTF8StringBase& dest, const char* cstr, size_t len);
	void MultiByteString2UTF8String(UTF8StringBase& dest, const MultiByteStringBase& mbs, const char* encoding);
	void MultiByteString2UTF8String(UTF8StringBase& dest, const char* cstr, size_t len, const char* encoding);
}

#endif