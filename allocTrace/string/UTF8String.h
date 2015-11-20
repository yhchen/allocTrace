#ifndef __SG2D_UTF8_STRING_H__
#define __SG2D_UTF8_STRING_H__

namespace SG2D
{
	class UTF8String : public UTF8StringBase
	{
	public:
		typedef UTF8StringBase super;

	public:
		UTF8String():super()
		{

		}
		UTF8String(const UTF8String &another)
			:super(another)
		{

		}
		UTF8String(const AnsiString &another)
			:super(another)
		{

		}
		UTF8String(const UTF8StringBase &another)
			:super(another)
		{

		}
		UTF8String(const MultiByteStringBase &another)
			:super()
		{
			operator = (another);

		}
		UTF8String(const UnicodeStringBase &another)
			:super()
		{
			operator = (another);
		}
		UTF8String(const char* str)
			:super(str, str ? strlen(str) : 0)
		{

		}
		UTF8String(const char* str, size_t len)
			:super(str, len)
		{

		}
		UTF8String(const size_t len):super(len)
		{

		}
		UTF8String(size_t max, const char* fmt, ...):super()
		{
			va_list args;
			va_start(args, fmt);
			if (max != 0)
				formatArgs(max, fmt, args);
			else formatArgs(fmt, args);
			va_end(args);
		}
		inline UTF8String& operator = (const char* str)
		{
			super::operator = (str);
			return *this;
		}
		inline UTF8String& operator = (const AnsiString& str)
		{
			super::operator=(str);
			return *this;
		}
		inline UTF8String& operator = (const UTF8StringBase& str)
		{
			super::operator=(str);
			return *this;
		}
		inline UTF8String& operator = (const UTF8String& str)
		{
			super::operator=(str);
			return *this;
		}
		inline UTF8String& operator = (const UnicodeStringBase& str)
		{
			UnicodeString2UTF8String(*this, str);
			return *this;
		}
		inline UTF8String& operator = (const MultiByteStringBase& str)
		{
			MultiByteString2UTF8String(*this, str);
			return *this;
		}
		inline UTF8String& operator += (const char* str)
		{
			super::operator += (str);
			return *this;
		}
		inline UTF8String& operator += (const AnsiString& str)
		{
			super::operator += (str);
			return *this;
		}
		inline UTF8String& operator += (const UTF8StringBase& str)
		{
			super::operator += (str);
			return *this;
		}
		inline UTF8String& operator += (const UTF8String& str)
		{
			super::operator += (str);
			return *this;
		}
		inline UTF8String& operator += (const UnicodeStringBase& str)
		{
			UTF8String mbs = str;
			operator += (mbs);
			return *this;
		}
		inline UTF8String& operator += (const MultiByteStringBase& str)
		{
			UTF8String mbs = str;
			operator += (mbs);
			return *this;
		}
		inline UTF8String operator + (const char* str) const
		{
			UTF8String result = *this;
			result += (str);
			return result;
		}
		inline UTF8String operator + (const AnsiString& str) const
		{
			UTF8String result = *this;
			result += (str);
			return result;
		}
		inline UTF8String operator + (const UTF8String& str) const
		{
			UTF8String result = *this;
			result += (str);
			return result;
		}
		inline UTF8String operator + (const UTF8StringBase& str) const
		{
			UTF8String result = *this;
			result += (str);
			return result;
		}
		inline UTF8String operator + (const UnicodeStringBase& str) const
		{
			UTF8String result = *this;
			result += (str);
			return result;
		}
		inline UTF8String operator + (const MultiByteStringBase& str) const
		{
			UTF8String result = *this;
			result += (str);
			return result;
		}
		inline bool operator == (const char* str) const
		{
			return super::operator==(str);
		}
		inline bool operator == (const AnsiString& str) const
		{
			return super::operator==(str);
		}
		inline bool operator == (const UTF8StringBase& str) const
		{
			return super::operator==(str);
		}
		inline bool operator == (const UTF8String& str) const
		{
			return super::operator==(str);
		}
		inline bool operator == (const MultiByteStringBase& str) const
		{
			UTF8String utf = str;
			return super::operator==(utf);
		}
		inline bool operator == (const UnicodeStringBase& str) const
		{
			UTF8String utf = str;
			return super::operator==(utf);
		}
		inline bool operator != (const char* str) const
		{
			return !(operator == (str));
		}
		inline bool operator != (const AnsiString& str) const
		{
			return !(operator == (str));
		}
		inline bool operator != (const UTF8String& str) const
		{
			return !(operator == (str));
		}
		inline bool operator != (const UTF8StringBase& str) const
		{
			return !(operator == (str));
		}
		inline bool operator != (const MultiByteStringBase& str) const
		{
			return !(operator == (str));
		}
		inline bool operator != (const UnicodeStringBase& str) const
		{
			return !(operator == (str));
		}
		inline bool operator < (const UTF8StringBase& another) const
		{
			return compare(another) < 0;
		}
		inline bool operator < (const MultiByteStringBase& another) const
		{
			return compare(MultiByteString2UTF8String(another)) < 0;
		}
		inline bool operator < (const UnicodeStringBase& another) const
		{
			return compare(UnicodeString2MultiByteString(another)) < 0;
		}
		inline bool operator > (const UTF8StringBase& another) const
		{
			return compare(another) > 0;
		}
		inline bool operator > (const MultiByteStringBase& another) const
		{
			return compare(MultiByteString2UTF8String(another)) > 0;
		}
		inline bool operator > (const UnicodeStringBase& another) const
		{
			return compare(UnicodeString2MultiByteString(another)) > 0;
		}
		UTF8String& format(const AnsiString fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(fmt.ptr(), args);
			va_end(args);
			return *this;
		}
		UTF8String& format(const UTF8StringBase fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(fmt.ptr(), args);
			va_end(args);
			return *this;
		}
		UTF8String& format(const UTF8String fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(fmt.ptr(), args);
			va_end(args);
			return *this;
		}
		UTF8String& format(const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(fmt, args);
			va_end(args);
			return *this;
		}
		UTF8String& format(size_t max, const UTF8StringBase fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(max, fmt.ptr(), args);
			va_end(args);
			return *this;
		}
		UTF8String& format(size_t max, const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(max, fmt, args);
			va_end(args);
			return *this;
		}
		UTF8String& formatArgs(const char *fmt, va_list args)
		{
			super::formatArgs(fmt, args);
			return *this;
		}
		UTF8String& formatArgs(size_t max, const char *fmt, va_list args)
		{
			super::formatArgs(max, fmt, args);
			return *this;
		}
		UTF8String& catWith(size_t max, const char *fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			catWithArgs(max, fmt, args);
			va_end(args);
			return *this;
		}
		UTF8String& catWithArgs(size_t max, const char *fmt, va_list args)
		{
			super::catWithArgs(max, fmt, args);
			return *this;
		}
	};
}

#endif
