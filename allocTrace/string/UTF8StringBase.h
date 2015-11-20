#ifndef __SG2D_UTF8_STRING_BASE_H__
#define __SG2D_UTF8_STRING_BASE_H__

namespace SG2D
{
	class UTF8StringBase : public AnsiString
	{
	public:
		typedef AnsiString super;

	public:
		UTF8StringBase():super()
		{

		}
		UTF8StringBase(const UTF8StringBase &another)
			:super(another)
		{

		}
		UTF8StringBase(const AnsiString &another)
			:super(another)
		{

		}
		UTF8StringBase(const char* str)
			:super(str, str ? strlen(str) : 0)
		{

		}
		UTF8StringBase(const char* str, size_t len)
			:super(str, len)
		{

		}
		UTF8StringBase(const size_t len):super(len)
		{

		}
		UTF8StringBase(size_t max, const char* fmt, ...):super()
		{
			va_list args;
			va_start(args, fmt);
			if (max != 0)
				formatArgs(max, fmt, args);
			else formatArgs(fmt, args);
			va_end(args);
		}
		inline UTF8StringBase& operator = (const char* str)
		{
			super::operator = (str);
			return *this;
		}
		inline UTF8StringBase& operator = (const AnsiString& str)
		{
			super::operator = (str);
			return *this;
		}
		inline UTF8StringBase& operator = (const UTF8StringBase& str)
		{
			super::operator = (str);
			return *this;
		}
		inline UTF8StringBase& operator += (const char* str)
		{
			super::operator += (str);
			return *this;
		}
		inline UTF8StringBase& operator += (const AnsiString& str)
		{
			super::operator += (str);
			return *this;
		}
		inline UTF8StringBase operator + (const UTF8StringBase& str) const
		{
			UTF8StringBase utf = *this;
			utf += str;
			return utf;
		}
		inline bool operator == (const char* b) const
		{
			return super::operator == (b);
		}
		inline bool operator == (const AnsiString& b) const
		{
			return super::operator == (b);
		}
		inline bool operator == (const UTF8StringBase& b) const
		{
			return super::operator == (b);
		}
		inline bool operator != (const char* b) const
		{
			return !(operator == (b));
		}
		inline bool operator != (const AnsiString& b) const
		{
			return !(operator == (b));
		}
		inline bool operator != (const UTF8StringBase& b) const
		{
			return !(operator == (b));
		}
		inline UTF8StringBase operator + (const char* str)
		{
			UTF8StringBase result = *this;
			result += (str);
			return result;
		}
		UTF8StringBase& format(const UTF8StringBase fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(fmt.ptr(), args);
			va_end(args);
			return *this;
		}
		UTF8StringBase& format(const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(fmt, args);
			va_end(args);
			return *this;
		}
		UTF8StringBase& format(size_t max, const UTF8StringBase fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(max, fmt.ptr(), args);
			va_end(args);
			return *this;
		}
		UTF8StringBase& format(size_t max, const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(max, fmt, args);
			va_end(args);
			return *this;
		}
		UTF8StringBase& formatArgs(const char *fmt, va_list args)
		{
			super::formatArgs(fmt, args);
			return *this;
		}
		UTF8StringBase& formatArgs(size_t max, const char *fmt, va_list args)
		{
			super::formatArgs(max, fmt, args);
			return *this;
		}
		UTF8StringBase& catWith(size_t max, const char *fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			catWithArgs(max, fmt, args);
			va_end(args);
			return *this;
		}
		UTF8StringBase& catWithArgs(size_t max, const char *fmt, va_list args)
		{
			super::catWithArgs(max, fmt, args);
			return *this;
		}
	};
}

#endif