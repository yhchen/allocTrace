#ifndef __SG2D_MULTIBYTE_STRING_H__
#define __SG2D_MULTIBYTE_STRING_H__

namespace SG2D
{
	class MultiByteString : public MultiByteStringBase
	{
	public:
		typedef MultiByteStringBase super;

	public:
		MultiByteString():super()
		{

		}
		MultiByteString(const MultiByteString &another)
			:super(another)
		{

		}
		MultiByteString(const AnsiString &another)
			:super(another)
		{

		}
		MultiByteString(const UTF8StringBase &another)
			:super()
		{
			operator = (another);
		}
		MultiByteString(const UnicodeStringBase &another)
			:super()
		{
			operator = (another);
		}
		MultiByteString(const char* str)
			:super(str, str ? strlen(str) : 0)
		{

		}
		MultiByteString(const char* str, size_t len)
			:super(str, len)
		{

		}
		MultiByteString(const size_t len):super(len)
		{

		}
		MultiByteString(size_t max, const char* fmt, ...):super()
		{
			va_list args;
			va_start(args, fmt);
			if (max != 0)
				formatArgs(max, fmt, args);
			else formatArgs(fmt, args);
			va_end(args);
		}
		inline MultiByteString& operator = (const char* str)
		{
			super::operator = (str);
			return *this;
		}
		inline MultiByteString& operator = (const AnsiString& str)
		{
			super::operator = (str);
			return *this;
		}
		inline MultiByteString& operator = (const MultiByteStringBase& str)
		{
			super::operator=(str);
			return *this;
		}
		inline MultiByteString& operator = (const MultiByteString& str)
		{
			super::operator=(str);
			return *this;
		}
		inline MultiByteString& operator = (const UnicodeStringBase& str)
		{
			UnicodeString2MultiByteString(*this, str);
			return *this;
		}
		inline MultiByteString& operator = (const UTF8StringBase& str)
		{
			UTF8String2MultiByteString(*this, str);
			return *this;
		}
		inline MultiByteString& operator += (const char* str)
		{
			super::operator += (str);
			return *this;
		}
		inline MultiByteString& operator += (const AnsiString& str)
		{
			super::operator += (str);
			return *this;
		}
		inline MultiByteString& operator += (const MultiByteStringBase& str)
		{
			super::operator += (str);
			return *this;
		}
		inline MultiByteString& operator += (const MultiByteString& str)
		{
			super::operator += (str);
			return *this;
		}
		inline MultiByteString& operator += (const UnicodeStringBase& str)
		{
			MultiByteString mbs = str;
			operator += (mbs);
			return *this;
		}
		inline MultiByteString& operator += (const UTF8StringBase& str)
		{
			MultiByteString mbs = str;
			operator += (mbs);
			return *this;
		}
		inline MultiByteString operator + (const char* str) const
		{
			MultiByteString result = *this;
			result += (str);
			return result;
		}
		inline MultiByteString operator + (const AnsiString& str) const
		{
			MultiByteString result = *this;
			result += (str);
			return result;
		}
		inline MultiByteString operator + (const MultiByteStringBase& str) const
		{
			MultiByteString result = *this;
			result += (str);
			return result;
		}
		inline MultiByteString operator + (const MultiByteString& str) const
		{
			MultiByteString result = *this;
			result += (str);
			return result;
		}
		inline MultiByteString operator + (const UnicodeStringBase& str) const
		{
			MultiByteString result = *this;
			result += (str);
			return result;
		}
		inline MultiByteString operator + (const UTF8StringBase& str) const
		{
			MultiByteString result = *this;
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
		inline bool operator == (const MultiByteStringBase& str) const
		{
			return super::operator==(str);
		}
		inline bool operator == (const MultiByteString& str) const
		{
			return super::operator==(str);
		}
		inline bool operator == (const UTF8StringBase& str) const
		{
			MultiByteString mbs = str;
			return super::operator==(mbs);
		}
		inline bool operator == (const UnicodeStringBase& str) const
		{
			MultiByteString mbs = str;
			return super::operator==(mbs);
		}
		inline bool operator != (const char* str) const
		{
			return !(operator == (str));
		}
		inline bool operator != (const AnsiString& str) const
		{
			return !(operator == (str));
		}
		inline bool operator != (const MultiByteStringBase& str) const
		{
			return !(operator == (str));
		}
		inline bool operator != (const MultiByteString& str) const
		{
			return !(operator == (str));
		}
		inline bool operator != (const UTF8StringBase& str) const
		{
			return !(operator == (str));
		}
		inline bool operator != (const UnicodeStringBase& str) const
		{
			return !(operator == (str));
		}
		inline bool operator < (const MultiByteString& another) const
		{
			return compare(another) < 0;
		}
		inline bool operator < (const UTF8StringBase& another) const
		{
			return compare(UTF8String2MultiByteString(another)) < 0;
		}
		inline bool operator < (const UnicodeStringBase& another) const
		{
			return compare(UnicodeString2MultiByteString(another)) < 0;
		}
		inline bool operator > (const MultiByteString& another) const
		{
			return compare(another) > 0;
		}
		inline bool operator > (const UTF8StringBase& another) const
		{
			return compare(UTF8String2MultiByteString(another)) > 0;
		}
		inline bool operator > (const UnicodeStringBase& another) const
		{
			return compare(UnicodeString2MultiByteString(another)) > 0;
		}
		MultiByteString& format(const AnsiString fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(fmt.ptr(), args);
			va_end(args);
			return *this;
		}
		MultiByteString& format(const MultiByteStringBase fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(fmt.ptr(), args);
			va_end(args);
			return *this;
		}
		MultiByteString& format(const MultiByteString fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(fmt.ptr(), args);
			va_end(args);
			return *this;
		}
		MultiByteString& format(const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(fmt, args);
			va_end(args);
			return *this;
		}
		MultiByteString& format(size_t max, const MultiByteStringBase fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(max, fmt.ptr(), args);
			va_end(args);
			return *this;
		}
		MultiByteString& format(size_t max, const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(max, fmt, args);
			va_end(args);
			return *this;
		}
		MultiByteString& formatArgs(const char *fmt, va_list args)
		{
			super::formatArgs(fmt, args);
			return *this;
		}
		MultiByteString& formatArgs(size_t max, const char *fmt, va_list args)
		{
			super::formatArgs(max, fmt, args);
			return *this;
		}
		MultiByteString& catWith(size_t max, const char *fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			catWithArgs(max, fmt, args);
			va_end(args);
			return *this;
		}
		MultiByteString& catWithArgs(size_t max, const char *fmt, va_list args)
		{
			super::catWithArgs(max, fmt, args);
			return *this;
		}
	};
}

#endif