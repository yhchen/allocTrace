#ifndef __SG2D_UNICODE_STRING_H__
#define __SG2D_UNICODE_STRING_H__

namespace SG2D
{
	class UnicodeString : public UnicodeStringBase
	{
	public:
		typedef UnicodeStringBase super;

	public:
		UnicodeString():super()
		{

		}
		UnicodeString(const UnicodeString &another)
			:super(another)
		{

		}
		UnicodeString(const MultiByteStringBase &another)
			:super()
		{
			operator = (another);

		}
		UnicodeString(const UTF8StringBase &another)
			:super()
		{
			operator = (another);
		}
		UnicodeString(const wchar_t* str)
			:super(str, str ? wcslen(str) : 0)
		{

		}
		UnicodeString(const wchar_t* str, size_t len)
			:super(str, len)
		{

		}
		UnicodeString(const size_t len):super(len)
		{

		}
		UnicodeString(size_t max, const wchar_t* fmt, ...):super()
		{
			va_list args;
			va_start(args, fmt);
			if (max != 0)
				formatArgs(max, fmt, args);
			else formatArgs(fmt, args);
			va_end(args);
		}
		inline UnicodeString& operator = (const wchar_t* str)
		{
			super::operator = (str);
			return *this;
		}
		inline UnicodeString& operator = (const UnicodeStringBase str)
		{
			super::operator = (str);
			return *this;
		}
		inline UnicodeString& operator = (const UnicodeString str)
		{
			super::operator = (str);
			return *this;
		}
		inline UnicodeString& operator = (const UTF8StringBase& str)
		{
			UTF8String2UnicodeString(*this, str);
			return *this;
		}
		inline UnicodeString& operator = (const MultiByteStringBase& str)
		{
			MultiByteString2UnicodeString(*this, str);
			return *this;
		}
		inline UnicodeString& operator += (const wchar_t* str)
		{
			super::operator += (str);
			return *this;
		}
		inline UnicodeString& operator += (const UnicodeStringBase& str)
		{
			super::operator += (str);
			return *this;
		}
		inline UnicodeString& operator += (const UnicodeString& str)
		{
			super::operator += (str);
			return *this;
		}
		inline UnicodeString& operator += (const UTF8StringBase& str)
		{
			UnicodeString mbs = str;
			super::operator += (mbs);
			return *this;
		}
		inline UnicodeString& operator += (const MultiByteStringBase& str)
		{
			UnicodeString mbs = str;
			super::operator += (mbs);
			return *this;
		}
		inline UnicodeString operator + (const wchar_t* str) const
		{
			UnicodeString result = *this;
			result += (str);
			return result;
		}
		inline UnicodeString operator + (const UnicodeStringBase& str) const
		{
			UnicodeString result = *this;
			result += (str);
			return result;
		}
		inline UnicodeString operator + (const UnicodeString& str) const
		{
			UnicodeString result = *this;
			result += (str);
			return result;
		}
		inline UnicodeString operator + (const UTF8StringBase& str) const
		{
			UnicodeString result = *this;
			result += (str);
			return result;
		}
		inline UnicodeString operator + (const MultiByteStringBase& str) const
		{
			UnicodeString result = *this;
			result += (str);
			return result;
		}
		inline bool operator == (const UnicodeStringBase& str) const
		{
			return super::operator==(str);
		}
		inline bool operator == (const UnicodeString& str) const
		{
			return super::operator==(str);
		}
		inline bool operator == (const UTF8StringBase& str) const
		{
			UnicodeString utf = str;
			return super::operator==(utf);
		}
		inline bool operator == (const MultiByteStringBase& str) const
		{
			UnicodeString utf = str;
			return super::operator==(utf);
		}
		inline bool operator != (const UnicodeStringBase& str) const
		{
			return !(operator == (str));
		}
		inline bool operator != (const UnicodeString& str) const
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
		inline bool operator < (const UnicodeStringBase& another) const
		{
			return compare(another) < 0;
		}
		inline bool operator < (const MultiByteStringBase& another) const
		{
			return compare(MultiByteString2UnicodeString(another)) < 0;
		}
		inline bool operator < (const UTF8StringBase& another) const
		{
			return compare(UTF8String2UnicodeString(another)) < 0;
		}
		inline bool operator > (const UnicodeStringBase& another) const
		{
			return compare(another) > 0;
		}
		inline bool operator > (const MultiByteStringBase& another) const
		{
			return compare(MultiByteString2UnicodeString(another)) > 0;
		}
		inline bool operator > (const UTF8StringBase& another) const
		{
			return compare(UTF8String2UnicodeString(another)) > 0;
		}
		UnicodeString& format(const UnicodeStringBase& fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(fmt.ptr(), args);
			va_end(args);
			return *this;
		}
		UnicodeString& format(const UnicodeString& fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(fmt.ptr(), args);
			va_end(args);
			return *this;
		}
		UnicodeString& format(const wchar_t* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(fmt, args);
			va_end(args);
			return *this;
		}
		UnicodeString& format(size_t max, const UnicodeStringBase& fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(max, fmt.ptr(), args);
			va_end(args);
			return *this;
		}
		UnicodeString& format(size_t max, const wchar_t* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(max, fmt, args);
			va_end(args);
			return *this;
		}
		UnicodeString& formatArgs(const wchar_t *fmt, va_list args)
		{
			super::formatArgs(fmt, args);
			return *this;
		}
		UnicodeString& formatArgs(size_t max, const wchar_t *fmt, va_list args)
		{
			super::formatArgs(max, fmt, args);
			return *this;
		}
		UnicodeString& catWith(size_t max, const wchar_t *fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			catWithArgs(max, fmt, args);
			va_end(args);
			return *this;
		}
		UnicodeString& catWithArgs(size_t max, const wchar_t *fmt, va_list args)
		{
			super::catWithArgs(max, fmt, args);
			return *this;
		}
	};
}

#endif
