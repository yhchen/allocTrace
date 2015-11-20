#ifndef __SG2D_UNICODE_STRING_BASE_H__
#define __SG2D_UNICODE_STRING_BASE_H__

namespace SG2D
{
	class UnicodeStringBase : public Array<wchar_t>
	{
	public:
		typedef Array<wchar_t> super;

	public:
		UnicodeStringBase():super()
		{

		}
		UnicodeStringBase(const UnicodeStringBase &another)
			:super(another)
		{

		}
		UnicodeStringBase(const wchar_t* str)
			:super(str, str ? wcslen(str) : 0)
		{

		}
		UnicodeStringBase(const wchar_t* str, size_t len)
			:super(str, len)
		{

		}
		UnicodeStringBase(const size_t len):super(len)
		{

		}
		UnicodeStringBase(size_t max, const wchar_t* fmt, ...):super()
		{
			va_list args;
			va_start(args, fmt);
			if (max != 0)
				formatArgs(max, fmt, args);
			else formatArgs(fmt, args);
			va_end(args);
		}
		inline UnicodeStringBase& operator = (const wchar_t* str)
		{
			size_t len = str ? wcslen(str) : 0;
			setLength(0);
			if (len > 0)
            {
                setLength(len);
				memcpy(m_ptr, str, len * sizeof(*m_ptr));
            }
			return *this;
		}
		inline UnicodeStringBase& operator = (const UnicodeStringBase& str)
		{
			super::operator = (str);
			return *this;
		}
		inline UnicodeStringBase& operator += (const wchar_t* str)
		{
			if (!str || !str[0])
				return *this;
			cat(str, wcslen(str));
			return *this;
		}
		inline UnicodeStringBase& operator += (const UnicodeStringBase& str)
		{
			super::operator += (str);
			return *this;
		}
		UnicodeStringBase operator + (const wchar_t* str) const;
		inline UnicodeStringBase operator + (const UnicodeStringBase& str) const
		{
			UnicodeStringBase uns = *this;
			uns += str;
			return uns;
		}
		bool operator == (const UnicodeStringBase& another) const;
		inline bool operator == (const wchar_t* str) const
		{
			if (m_ptr == str)
				return true;
			if (!m_ptr && !str[0])
				return true;
			if (!m_ptr[0] && !str)
				return true;
			return compare(str) == 0;
		}
		inline bool operator != (const UnicodeStringBase& another) const
		{
			return !(operator == (another));
		}
		inline bool operator != (const wchar_t* str) const
		{
			return !(operator == (str));
		}
		inline int compare(const UnicodeStringBase& another) const
		{
			if (another.ptr() == m_ptr)
				return 0;
			int cmp = (int)(length() - another.length());
			if (cmp != 0)
				return cmp;
			return wcscmp(m_ptr, another.m_ptr);
		}
		inline int compare(const wchar_t* str) const
		{
			return wcscmp(m_ptr, str);
		}
		UnicodeStringBase& format(const UnicodeStringBase fmt, ...);
		UnicodeStringBase& format(const wchar_t* fmt, ...);
		UnicodeStringBase& format(size_t max, const UnicodeStringBase fmt, ...);
		UnicodeStringBase& format(size_t max, const wchar_t* fmt, ...);
		UnicodeStringBase& formatArgs(const wchar_t *fmt, va_list args);
		UnicodeStringBase& formatArgs(size_t max, const wchar_t *fmt, va_list args);
		UnicodeStringBase& catWith(size_t max, const wchar_t *fmt, ...);
		UnicodeStringBase& catWithArgs(size_t max, const wchar_t *fmt, va_list args);
		inline static size_t formatStringArgs(wchar_t *buffer, size_t max, const wchar_t *fmt, va_list args);
		double toNumber() const;
		inline float toSingle() const
		{
			return (float)toNumber();
		}
		inline int toInt() const
		{
			return (int)toNumber();
		}
		long long toInt64() const;
	};
}

#endif