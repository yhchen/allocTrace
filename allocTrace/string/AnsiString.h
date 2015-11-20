#ifndef __SG2D_ANSI_STRING_H__
#define __SG2D_ANSI_STRING_H__

namespace SG2D
{
	class AnsiString : public Array<char>
	{
	public:
		typedef Array<char> super;

	public:
		AnsiString():super()
		{

		}
		AnsiString(const AnsiString &another)
			:super(another)
		{

		}
		AnsiString(const char* str)
			:super(str, str ? strlen(str) : 0)
		{

		}
		AnsiString(const char* str, size_t len)
			:super(str, len)
		{

		}
		AnsiString(const size_t len):super(len)
		{

		}
		AnsiString(size_t max, const char* fmt, ...):super()
		{
			va_list args;
			va_start(args, fmt);
			if (max != 0)
				formatArgs(max, fmt, args);
			else formatArgs(fmt, args);
			va_end(args);
		}
		inline AnsiString& operator = (const char* str)
		{
			size_t len = str ? strlen(str) : 0;
			setLength(0);
			if (len > 0)
            {
                setLength(len);
				memcpy(m_ptr, str, len * sizeof(*m_ptr));
            }
			return *this;
		}
		inline AnsiString& operator = (const AnsiString& str)
		{
			super::operator = (str);
			return *this;
		}
		inline AnsiString& operator += (const char* str)
		{
			if (!str || !str[0])
				return *this;
			cat(str, strlen(str));
			return *this;
		}
		inline AnsiString& operator += (const AnsiString& str)
		{
			super::operator += (str);
			return *this;
		}
		AnsiString operator + (const char* str) const;
		inline AnsiString operator + (const AnsiString& str) const
		{
			AnsiString as = *this;
			as += str;
			return as;
		}
		inline bool operator == (const AnsiString& another) const
		{
			const char* aptr = another.ptr();
			if (m_ptr == aptr)
				return true;
			if (!m_ptr || !aptr)
				return false;
			if (length() != another.length())
				return false;
			return compare(another) == 0;
		}
		inline bool operator == (const char* str) const
		{
			if (m_ptr == str)
				return true;
			if (!m_ptr || !str)
				return false;
			return compare(str) == 0;
		}
		inline bool operator != (const AnsiString& another) const
		{
			return !(operator == (another));
		}
		inline bool operator != (const char* str) const
		{
			return !(operator == (str));
		}
		inline bool operator < (const AnsiString& another) const
		{
			return compare(another) < 0;
		}
		inline bool operator < (const char* another) const
		{
			return compare(another) < 0;
		}
		inline bool operator > (const AnsiString& another) const
		{
			return compare(another) > 0;
		}
		inline bool operator > (const char* another) const
		{
			return compare(another) > 0;
		}
		int compare(const AnsiString& another) const;
		inline int compare(const char* str) const
		{
			return strcmp(m_ptr, str);
		}
		AnsiString& format(const AnsiString fmt, ...);
		AnsiString& format(const char* fmt, ...);
		AnsiString& format(size_t max, const AnsiString fmt, ...);
		AnsiString& format(size_t max, const char* fmt, ...);
		AnsiString& formatArgs(const char *fmt, va_list args);
		AnsiString& formatArgs(size_t max, const char *fmt, va_list args);
		AnsiString& catWith(size_t max, const char *fmt, ...);
		AnsiString& catWithArgs(size_t max, const char *fmt, va_list args);
		inline static size_t formatStringArgs(char *buffer, size_t max, const char *fmt, va_list args);
		double toNumber() const;
		inline float toSingle() const
		{
			return (float)toNumber();
		}
		inline int toInt() const
		{
			return (int)toInt64();
		}
		long long toInt64() const;
	};
}

#endif