#ifndef __SG2D_MULTI_BYTE_STRING_BASE_H__
#define __SG2D_MULTI_BYTE_STRING_BASE_H__

namespace SG2D
{
	class MultiByteStringBase : public AnsiString
	{
	public:
		typedef AnsiString super;

	public:
		MultiByteStringBase():super()
		{

		}
		MultiByteStringBase(const MultiByteStringBase &another)
			:super(another)
		{

		}
		MultiByteStringBase(const AnsiString &another)
			:super(another)
		{

		}
		MultiByteStringBase(const char* str)
			:super(str, str ? strlen(str) : 0)
		{

		}
		MultiByteStringBase(const char* str, size_t len)
			:super(str, len)
		{

		}
		MultiByteStringBase(const size_t len):super(len)
		{

		}
		MultiByteStringBase(size_t max, const char* fmt, ...):super()
		{
			va_list args;
			va_start(args, fmt);
			if (max != 0)
				formatArgs(max, fmt, args);
			else formatArgs(fmt, args);
			va_end(args);
		}
		inline MultiByteStringBase& operator = (const char* str)
		{
			super::operator = (str);
			return *this;
		}
		inline MultiByteStringBase& operator = (const AnsiString& str)
		{
			super::operator = (str);
			return *this;
		}
		inline MultiByteStringBase& operator = (const MultiByteStringBase& str)
		{
			super::operator = (str);
			return *this;
		}
		inline MultiByteStringBase& operator += (const char* str)
		{
			super::operator += (str);
			return *this;
		}
		inline MultiByteStringBase& operator += (const AnsiString& str)
		{
			super::operator += (str);
			return *this;
		}
		inline MultiByteStringBase& operator += (const MultiByteStringBase& str)
		{
			super::operator += (str);
			return *this;
		}
		inline MultiByteStringBase operator + (const char* str) const
		{
			MultiByteStringBase result = *this;
			result += (str);
			return result;
		}
		inline MultiByteStringBase operator + (const AnsiString& str) const
		{
			MultiByteStringBase mbs = *this;
			mbs += str;
			return mbs;
		}
		inline MultiByteStringBase operator + (const MultiByteStringBase& str) const
		{
			MultiByteStringBase mbs = *this;
			mbs += str;
			return mbs;
		}
		inline bool operator == (const char* b) const
		{
			return super::operator == (b);
		}
		inline bool operator == (const AnsiString& b) const
		{
			return super::operator == (b);
		}
		inline bool operator == (const MultiByteStringBase& b) const
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
		inline bool operator != (const MultiByteStringBase& b) const
		{
			return !(operator == (b));
		}
		MultiByteStringBase& format(const MultiByteStringBase fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(fmt.ptr(), args);
			va_end(args);
			return *this;
		}
		MultiByteStringBase& format(const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(fmt, args);
			va_end(args);
			return *this;
		}
		MultiByteStringBase& format(size_t max, const MultiByteStringBase fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(max, fmt.ptr(), args);
			va_end(args);
			return *this;
		}
		MultiByteStringBase& format(size_t max, const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			formatArgs(max, fmt, args);
			va_end(args);
			return *this;
		}
		MultiByteStringBase& formatArgs(const char *fmt, va_list args)
		{
			super::formatArgs(fmt, args);
			return *this;
		}
		MultiByteStringBase& formatArgs(size_t max, const char *fmt, va_list args)
		{
			super::formatArgs(max, fmt, args);
			return *this;
		}
		MultiByteStringBase& catWith(size_t max, const char *fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			catWithArgs(max, fmt, args);
			va_end(args);
			return *this;
		}
		MultiByteStringBase& catWithArgs(size_t max, const char *fmt, va_list args)
		{
			super::catWithArgs(max, fmt, args);
			return *this;
		}
	};
}

#endif